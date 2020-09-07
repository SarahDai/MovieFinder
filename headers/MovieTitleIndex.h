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

#ifndef MOVIETITLEINDEX_H
#define MOVIETITLEINDEX_H

#include "Hashtable.h"
#include "LinkedList.h"
#include "Movie.h"
#include "DocSet.h"


/**
 * An index is a hashtable where they key is a term, and the value is
 * either a MovieSet or SetOfMovies.
 *
 * The Index is designed to allow indexing by one of multiple fields.
 */
typedef struct title_index {
  /**
   * The hashtable that takes care of the indexing of a given movie.
   */
  Hashtable ht;
} *MovieTitleIndex;

/**
 * If this Index is indexing by movie title rather than
 * one of the fields covered in AddMovieToIndex, use this
 * function to iterate through all words in the title and
 * add them to the index.
 *
 * INPUT:
 *  index: the index to add the movie to.
 *  movie: a Movie to be added to the index.
 *
 *  \return 0 if successful.
 */
int AddMovieTitleToIndex(MovieTitleIndex index,
                         Movie *movie,
                         uint64_t docId,
                         int row);



/**
 * Gets a MovieSet for a given word from the supplied index.
 *
 * INPUT:
 *  index: the index to search
 *  term: the term to search for in the index.
 *
 *  \return A MovieSet. Returns NULL if the term is not in the index.
 *    (that is, there are no movies with the given word in the title)
 *
 */
DocumentSet GetDocumentSet(MovieTitleIndex index,
                           const char *term);

/**
 *
 * (value is a MovieSet)
 */
int DestroyMovieTitleIndex(MovieTitleIndex index);

/**
 * Creates a new Index. Allocates all the memory
 *  necessary for this index.
 *
 */
MovieTitleIndex CreateMovieTitleIndex();

#endif
