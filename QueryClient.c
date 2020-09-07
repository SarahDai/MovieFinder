#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "QueryProtocol.h"

char *port_string = "1500";
char *ip = "127.0.0.1";
const char* client = "[Client]";

#define BUFFER_SIZE 1000

struct addrinfo* FindAddress() {
  int status;
  struct addrinfo hints;
  struct addrinfo *result;  // Save address result.

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;  // Use 'TCP' stream sockets.

  if ((status = getaddrinfo(ip, port_string, &hints, &result)) != 0) {
    fprintf(stderr, "%s getaddrinfo error: %s\n", client, gai_strerror(status));
    return NULL;
  }

  return result;
}

void RunQuery(char *query) {
  // Find the address.
  struct addrinfo *res = FindAddress();

  // Create the socket.
  int sockfd;
  if ((sockfd = socket(res->ai_family, res->ai_socktype,
                                       res->ai_protocol)) == -1) {
    perror("Socket");
    return;
  }

  // Connect to the server.
  if (connect(sockfd, res->ai_addr, res->ai_addrlen) != 0) {
    perror("Connect");
    return;
  }

  printf("%s Connected To Server Successfully.\n", client);

  // Free address info.
  freeaddrinfo(res);

  // Recevie ACK from server.
  int numbytes;
  char ack_buffer[BUFFER_SIZE];
  if ((numbytes = recv(sockfd, ack_buffer, BUFFER_SIZE - 1, 0)) == -1) {
    perror("Recv");
    return;
  }

  ack_buffer[numbytes] = '\0';
  printf("%s Received '%s' From Server.\n", client, ack_buffer);

  // Do the query-protocol
  if (send(sockfd, query, strlen(query), 0) == -1) {
    perror("Send");
    return;
  }

  printf("%s Sent Query '%s' To Server.\n", client, query);

  // Recevie response count from server.
  int count = 0;
  int* response_count = (int*)malloc(sizeof(count));
  if ((numbytes = recv(sockfd, response_count, sizeof(count), 0)) == -1) {
    perror("Recv");
    return;
  }

  count = *response_count;
  printf("%s Received [%d] Responses From Server.\n", client, count);
  free(response_count);

  // Sent 'ACK' to server.
  if (SendAck(sockfd) != 0) {
    perror("SendAck");
    return;
  }

  printf("%s Sent 'ACK' To Server.\n", client);

  // Read response from server.
  for (int i = 1; i <= count; i++) {
    char response[BUFFER_SIZE];
    if ((numbytes = recv(sockfd, response, BUFFER_SIZE - 1, 0)) == -1) {
      perror("Recv");
      return;
    }

    response[numbytes] = '\0';
    printf("%s {Response [%d]}: %s\n", client, i, response);

    if (SendAck(sockfd) != 0) {
      perror("SendAck");
      return;
    }

    printf("%s {Response [%d]}: Sent 'ACK' To Server.\n", client, i);
  }

  char goodbye_buffer[BUFFER_SIZE];
  if ((numbytes = recv(sockfd, goodbye_buffer, BUFFER_SIZE - 1, 0)) == -1) {
    perror("Recv");
    return;
  }

  goodbye_buffer[numbytes] = '\0';
  printf("%s Received '%s' From Server.\n", client, goodbye_buffer);

  // Close the connection
  close(sockfd);
  printf("%s Closed Client Connection.\n", client);
}

void RunPrompt() {
  char input[BUFFER_SIZE];

  while (1) {
    printf("%s Enter A Term To Search For, Or 'q' To Quit: ", client);
    scanf("%s", input);

    printf("%s Input: '%s'\n", client, input);

    if (strlen(input) == 1) {
      if (input[0] == 'q') {
        printf("%s Thanks for playing! \n", client);
        return;
      }
    }
    printf("\n\n");
    RunQuery(input);
  }
}

// This function connects to the given IP/port to ensure
// that it is up and running, before accepting queries from users.
// Returns 0 if can't connect; 1 if can.
int CheckIpAddress(char *ip, char *port) {
  // Find the address.
  struct addrinfo *res = FindAddress();

  // Creating socket file descriptor.
  int sockfd;
  if ((sockfd = socket(res->ai_family, res->ai_socktype,
                                       res->ai_protocol)) == -1) {
    perror("Socket");
    return 0;
  }

  // Connect to the server
  if (connect(sockfd, res->ai_addr, res->ai_addrlen) != 0) {
    perror("Connect");
    return 0;
  }

  printf("%s Connected To Server Successfully.\n", client);

  // Free address info.
  freeaddrinfo(res);

  // Listen for an ACK
  int numbytes;
  char buf[BUFFER_SIZE];
  if ((numbytes = recv(sockfd, buf, BUFFER_SIZE - 1, 0)) == -1) {
    perror("Recv");
    return 0;
  }

  buf[numbytes] = '\0';
  printf("%s Received '%s' From Server.\n", client, buf);

  // Send a goodbye
  if (SendGoodbye(sockfd) != 0) {
    perror("SendGoodbye");
    return 0;
  }

  printf("%s Sent 'GOODBYE' To Server.\n", client);

  // Close the connection
  close(sockfd);

  printf("%s Closed Client Connection.\n", client);

  return 1;
}

int main(int argc, char **argv) {
  if (argc != 3) {
    printf("%s Incorrect number of arguments. \n", client);
    printf("%s Correct usage: ./queryclient [IP] [port]\n", client);
  } else {
    ip = argv[1];
    port_string = argv[2];
  }

  printf("\n");
  printf("==============================================================\n");
  printf("                        Check Ip Address                      \n");
  printf("==============================================================\n\n");
  int connected = CheckIpAddress(ip, port_string);
  if (connected > 0) {
    printf("%s Check Ip Address Successfully!\n", client);
  } else {
    printf("%s Check Ip Address Failed!\n", client);
  }

  printf("\n");
  printf("==============================================================\n");
  printf("                       Check Ip Completed                     \n");
  printf("==============================================================\n\n");
  printf("\n");

  printf("\n");
  printf("==============================================================\n");
  printf("                            Run Query                         \n");
  printf("==============================================================\n\n");

  // Run query if client connected successfully.
  if (connected > 0) {
    RunPrompt();
  }

  printf("\n");
  printf("==============================================================\n");
  printf("                      Run Query Completed                     \n");
  printf("==============================================================\n\n");
  printf("\n");

  return 0;
}
