#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

#include "QueryProtocol.h"
#include "MovieSet.h"
#include "MovieIndex.h"
#include "DocIdMap.h"
#include "Hashtable.h"
#include "QueryProcessor.h"
#include "FileParser.h"
#include "DirectoryParser.h"
#include "FileCrawler.h"
#include "Util.h"

#define BUFFER_SIZE 1000

int Cleanup();

DocIdMap docs;
MovieTitleIndex docIndex;
const char* server = "[Server]";
char *ip = "127.0.0.1";

#define SEARCH_RESULT_LENGTH 1500
#define BACKLOG 20  // The number of pending connections queue will hold.

char movieSearchResult[SEARCH_RESULT_LENGTH];

void sigchld_handler(int s) {
  write(0, "Handling zombies...\n", 20);
  // waitpid() might overwrite errno, so we save and restore it:
  int saved_errno = errno;

  while (waitpid(-1, NULL, WNOHANG) > 0);

  errno = saved_errno;
}


void sigint_handler(int sig) {
  write(0, "Ahhh! SIGINT!\n", 14);
  Cleanup();
  exit(0);
}

int send_msg(int sockfd, void* buffer, size_t len) {
  if (send(sockfd, buffer, len, 0) == -1) {
    perror("Send");
    return -1;
  }

  return 0;
}

int recv_msg(int sockfd, char* buffer, size_t len) {
  int numbytes = 0;
  if ((numbytes = recv(sockfd, buffer, len, 0)) == -1) {
    perror("Recv");
    return -1;
  }

  buffer[numbytes] = '\0';
  return numbytes;
}

struct addrinfo* FindAddress(char* ip, char* port) {
  int status;
  struct addrinfo hints;
  struct addrinfo *result;  // Save address result.

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;  // Use 'TCP' stream sockets.

  if ((status = getaddrinfo(ip, port, &hints, &result)) != 0) {
    fprintf(stderr, "%s getaddrinfo error: %s\n", server, gai_strerror(status));
    return NULL;
  }

  return result;
}

/**
 * Return 0 for successful connection;
 * Return -1 for some error.
 */
int HandleClient(int client_fd, char* query) {
  // Run query and get responses.
  SearchResultIter results = FindMovies(docIndex, query);

  int count = 0;
  if (results != NULL) {
    count = NumResultsInIter(results);
  }

  // Send number of responses.
  send_msg(client_fd, &count, sizeof(count));
  printf("%s Sent 'Results [%d] Found' To Client.\n", server, count);

  // Wait for ACK.
  char ack_buffer[BUFFER_SIZE];
  recv_msg(client_fd, ack_buffer, BUFFER_SIZE - 1);
  printf("%s Received '%s' From Client.\n", server, ack_buffer);

  if (count > 0) {
    // For each response.
    SearchResult sr = (SearchResult)malloc(sizeof(*sr));
    if (sr == NULL) {
      printf("%s Couldn't malloc SearchResult.\n", server);
      return -1;
    }

    // Get the first result.
    int seq = 1;
    SearchResultGet(results, sr);

    // Get search result.
    char buffer[BUFFER_SIZE];
    CopyRowFromFile(sr, docs, buffer);

    // Send response.
    send_msg(client_fd, buffer, strlen(buffer));
    printf("%s Sent {Response [%d]} '%s' To Client.\n", server, seq, buffer);

    // Wait for ACK.
    recv_msg(client_fd, ack_buffer, BUFFER_SIZE - 1);
    printf("%s Received '%s' From Client For {Response [%d]}.\n",
                                                  server, ack_buffer, seq++);

    int result;
    while (SearchResultIterHasMore(results) != 0) {
      result =  SearchResultNext(results);
      if (result < 0) {
        printf("%s Error Retrieving Result\n", server);
        break;
      }

      SearchResultGet(results, sr);

      // Get search result.
      CopyRowFromFile(sr, docs, buffer);

      // Send response.
      send_msg(client_fd, buffer, strlen(buffer));
      printf("%s Sent {Response [%d]} '%s' To Client.\n",
                                                  server, seq, buffer);

      // Wait ACK.
      recv_msg(client_fd, ack_buffer, BUFFER_SIZE - 1);
      printf("%s Received '%s' From Client For {Response [%d]}.\n",
                                                  server, ack_buffer, seq++);
    }

    free(sr);
  }

  // Cleanup.
  if (results != NULL) {
    DestroySearchResultIter(results);
  }

  // Send GOODBYE.
  if (SendGoodbye(client_fd) != 0) {
    perror("SendGoodbye");
    return 0;
  }

  printf("%s Sent 'GOODBYE' To Client.\n", server);

  // Close connection.
  close(client_fd);

  return 0;
}

/**
 * Handle Connection.
 * Return 0 if successful; -1 failed.
 */
int HandleConnections(int sock_fd, int debug) {
  // Step 5: Accept connection.
  // Fork on every connection.
  struct sockaddr_storage their_addr;
  socklen_t addr_size;
  addr_size = sizeof their_addr;
  int new_fd;

  // Main accept loop.
  while (1) {
    // Step 5: Accept connection.
    new_fd = accept(sock_fd, (struct sockaddr *)&their_addr, &addr_size);
    if (new_fd == -1) {
      perror("Accept");
      continue;
    }

    printf("%s Got Connection From Client.\n", server);

    // Process connection in child process.
    if (!fork()) {
      // Sleep for 10 seconds in debug mode.
      if (debug == 1) {
        printf("%s Sleeping for 10 seconds before handling the query.\n",
                                                                    server);
        sleep(10);
      }

      // Step 6: Read, then write if you want.
      // Send ACK to client.
      if (SendAck(new_fd) != 0) {
        perror("SendAck");
        continue;
      }

      printf("%s Sent 'ACK' To Client.\n", server);

      // Listen for query.
      char query_buffer[BUFFER_SIZE];
      recv_msg(new_fd, query_buffer, BUFFER_SIZE - 1);
      printf("%s Received Query '%s' From Client.\n", server, query_buffer);

      // If query is GOODBYE close connection.
      if (CheckGoodbye(query_buffer) == 0) {
        close(new_fd);
        continue;
      }

      // Handle client.
      HandleClient(new_fd, query_buffer);
    }
  }

  return 0;
}

int Setup(char *dir) {
  struct sigaction sa;

  sa.sa_handler = sigchld_handler;  // reap all dead processes
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
    perror("sigaction");
    exit(1);
  }

  struct sigaction kill;

  kill.sa_handler = sigint_handler;
  kill.sa_flags = 0;  // or SA_RESTART
  sigemptyset(&kill.sa_mask);

  if (sigaction(SIGINT, &kill, NULL) == -1) {
    perror("sigaction");
    exit(1);
  }

  printf("Crawling directory tree starting at: %s\n", dir);
  // Create a DocIdMap
  docs = CreateDocIdMap();
  CrawlFilesToMap(dir, docs);
  printf("Crawled %d files.\n", NumElemsInHashtable(docs));

  // Create the index
  docIndex = CreateMovieTitleIndex();

  if (NumDocsInMap(docs) < 1) {
    printf("No documents found.\n");
    return 0;
  }

  // Index the files
  printf("Parsing and indexing files...\n");
  ParseTheFiles(docs, docIndex);
  printf("%d entries in the index.\n", NumElemsInHashtable(docIndex->ht));
  return NumElemsInHashtable(docIndex->ht);
}

int Cleanup() {
  DestroyMovieTitleIndex(docIndex);
  DestroyDocIdMap(docs);
  return 0;
}

int main(int argc, char **argv) {
  char *port = NULL;
  char *dir_to_crawl = NULL;

  int debug_flag = 0;
  int c;

  printf("\n");
  printf("==============================================================\n");
  printf("                      Get Port & Dir To Crawl                 \n");
  printf("==============================================================\n\n");

  while ((c = getopt(argc, argv, "dp:f:")) != -1) {
    switch (c) {
      case 'd':
        debug_flag = 1;
        break;
      case 'p':
        port = optarg;
        break;
      case 'f':
        dir_to_crawl = optarg;
        break;
      }
  }

  if (port == NULL) {
    printf("%s No port provided; please include with a -p flag.\n", server);
    exit(0);
  }

  if (dir_to_crawl == NULL) {
    printf("%s No directory provided; please include with a -f flag.\n",
                                                                    server);
    exit(0);
  }

  printf("%s Get Port: [%s], Dir: [%s]\n", server, port, dir_to_crawl);

  printf("\n");
  printf("==============================================================\n");
  printf("                Get Port & Dir To Crawl Completed             \n");
  printf("==============================================================\n\n");
  printf("\n");

  printf("\n");
  printf("==============================================================\n");
  printf("                            Setup Index                       \n");
  printf("==============================================================\n\n");

  int num_entries = Setup(dir_to_crawl);
  if (num_entries == 0) {
    printf("%s No entries in index. Quitting. \n", server);
    exit(0);
  }

  printf("\n");
  printf("==============================================================\n");
  printf("                     Setup Index Completed                    \n");
  printf("==============================================================\n\n");
  printf("\n");

  printf("\n");
  printf("==============================================================\n");
  printf("                           Listen To Port                     \n");
  printf("==============================================================\n\n");

  // Step 1: Get address stuff.
  struct addrinfo *res = FindAddress(ip, port);

  // Step 2: Open socket.
  int sockfd;
  if ((sockfd = socket(res->ai_family, res->ai_socktype,
                                       res->ai_protocol)) == -1) {
    perror("Socket");
    exit(1);
  }

  // Step 3: Bind socket.
  int yes = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
    perror("Setsockopt");
    exit(1);
  }

  if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
    perror("Bind");
    exit(1);
  }

  printf("%s Bind To Port [%s] Successfully.\n", server, port);

  // Free address info.
  freeaddrinfo(res);

  // Step 4: Listen on the socket.
  if (listen(sockfd, BACKLOG) == -1) {
    perror("Listen");
    exit(1);
  }

  printf("%s Listen To Client Connection.\n", server);

  printf("\n");
  printf("==============================================================\n");
  printf("                     Listen To Port Completed                 \n");
  printf("==============================================================\n\n");
  printf("\n");

  printf("\n");
  printf("==============================================================\n");
  printf("                           Handle Client                      \n");
  printf("==============================================================\n\n");

  // Handle Client.
  if (HandleConnections(sockfd, debug_flag) == -1) {
    perror("Handle Client");
    exit(1);
  }

  // Got Kill signal.
  close(sockfd);
  Cleanup();

  printf("\n");
  printf("==============================================================\n");
  printf("                    Handle Client Completed                   \n");
  printf("==============================================================\n\n");
  printf("\n");

  return 0;
}
