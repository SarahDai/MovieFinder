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
#ifndef QUERYPROCESSOR_H
#define QUERYPROCESSOR_H

#include "MovieTitleIndex.h"
#include "DocSet.h"

/**
 *  A SearchResult is a doc_idd and row_id,
 * representing a particular row in a particular file.
 *
 */
typedef struct searchResult {
  uint64_t doc_id;
  int row_id;
} *SearchResult;

/**
 * A SearchResultIter goes through every element in the hashtable,
 * which are all lists of document locations.
 *
 */
typedef struct searchResultIter {
  int cur_doc_id;
  HTIter doc_iter;
  LLIter offset_iter;
  int num_results;
} *SearchResultIter;

SearchResultIter CreateSearchResultIter(DocumentSet set);

void DestroySearchResultIter(SearchResultIter iter);

int SearchResultGet(SearchResultIter iter, SearchResult output);

int SearchResultNext(SearchResultIter iter);

int SearchResultIterHasMore(SearchResultIter iter);

SearchResultIter FindMovies(MovieTitleIndex index, char *term);

/**
 * Returns the number of elements in the provided SearchResultIter.
 * This is the number of search results.
 *
 */
int NumResultsInIter(SearchResultIter iter);

#endif
