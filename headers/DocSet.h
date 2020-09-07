/*
 *  Created by Adrienne Slaughter
 *  CS 5007 Summer 2019
 *  Northeastern University, Seattle
 *
 *  This is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  It is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  See <http://www.gnu.org/licenses/>.
 */

#ifndef DOCSET_H
#define DOCSET_H

#include "Hashtable.h"
#include "Movie.h"

/**
 * A MovieSet is a set of movies.
 *
 * doc_index is a hashtable where the key is a doc_id,
 * and the value is a linked list. The payloads in the linked
 * list is a row_id that indicates which row in the specified file
 * has the info about the movie that belongs in this set.
 */
typedef struct docSet {
  char *desc; /*!< A string describing the movie set. */
  /*!< A hashtable that holds the info about which doc each movie is in*/
  Hashtable doc_index;
  int num_movies;
} *DocumentSet;

/**
 * Adds a reference to a doc/row to the set.
 *
 * \param set The DocumentSet to add the movie to
 * \param doc_id Which document/file the movie is stored in
 * \param row_id Which row in the file the movie can be found.
 *
 * \return 0 if successful.
 */
int AddDocInfoToSet(DocumentSet set, uint64_t doc_id, int row_id);

/**
 * Destroys a document set, freeing everything necessary.
 *
 * /param set the DocumentSet to be destroyed.
 *
 */
void DestroyDocumentSet(DocumentSet set);


/**
 * Prints the "offsetList" for a given set of documents.
 * The offset list is the row IDs for each document (movie) in the set.
 * Helpful for debugging.
 *
 * \param list A linkedlist of row Ids (the value of the doc_index)
 */
void PrintOffsetList(LinkedList list);

/**
 * Determines if a DocumentSet contains movies from a specifid
 * document or file.
 *
 * \param set The DocumentSet to query
 * \param doc_id Which doc to look for.
 *
 * \return 0 if the doc_id is found, -1 otherwise.
 */
int DocumentSetContainsDoc(DocumentSet set, uint64_t doc_id);

/**
 * Creates a new, empty DocumentSet given the description.
 *
 * \param desc the description of what relates the docs that will be in this DocumentSet
 *
 * \return A pointer to the new DocumentSet that has been allocated.
 */
DocumentSet CreateDocumentSet(char *desc);

/**
 * Destroys the offset lists that are the values
 * of the hashtable.
 *
 * \param val the Offset list to be destroyed.
 */
void DestroyOffsetList(void *val);

/**
 * Destroys the DocumentSet.
 *
 * \param val the movieSet to be destroyed.
 */
void DestroyDocumentSet(DocumentSet val);

int NumMoviesInSet(DocumentSet);

#endif
