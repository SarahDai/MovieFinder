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
#ifndef UTIL_H
#define UTIL_H

#include "DocIdMap.h"
#include "QueryProcessor.h"

void NullFree(void *freeme);

void SimpleFree(void *payload);

void toLower(char *str, int len);

/**
 * Given a string, trims spaces and punctuation from the
 * beginning and end; leaves the interior alone. 
 *
 */
void Trim(char* str_to_trim);

/**
 * Removes all whitespace and punctuation from a string. 
 *
 */
void CleanString(char* str_to_trim);

int CopyRowFromFile(SearchResult result,
                   DocIdMap docs,
                    char *movieSearchResult);

#endif
