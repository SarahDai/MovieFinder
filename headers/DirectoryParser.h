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

#ifndef DIRPARSER_H
#define DIRPARSER_H

#include "MovieTitleIndex.h"
#include "DocIdMap.h"
#include "LinkedList.h"

/**
 * Given a map of all the files that we want to index
 * and search, open each file and index the contents to index
 *
 * \param docs the DocIdMap that contains all the files we want to parse.
 * \param the index to hold all the indexed docs.
 *
 * \return The number of records parsed
 */
int ParseTheFiles(DocIdMap docs, MovieTitleIndex index);


/**
 * Helper function to index a single file. 
 *
 * \return the number of records (lines) indexed from the file
 */
int IndexTheFile(char *file, uint64_t docId, MovieTitleIndex index);

#endif
