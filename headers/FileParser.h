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

#ifndef FILEPARSER_H
#define FILEPARSER_H

#include "MovieIndex.h"
#include "LinkedList.h"

// Reads in the specified file with a given structure and reads in each row,
// creating a Movie from each row.
//
// Expected sample row:
// id       |type |Title1   |Title2   |IsAdult|Year|?|?|Genres
// tt0003609|movie|Alexandra|Alexandra|0      |1915|-|-|-
//
// Fields are separated by a pipe (|), and a dash (-) specifies an empty value.
//
// 
//
// INPUT: filename: the data file with movies
// Returns a LinkedList of Movie structs from the specified file
LinkedList ReadFile(const char* filename);

// A FileParser is Responsible for reading in a file,
//  checking for errors, and parse out movies. 

// Given a list of movies and a field to index on, builds an index
// of the movies.
//
// If a movie to be indexed has no value for that field, create a MovieSet
// that represents "NONE" for that field, and holds all docs with no value. 
//
// INPUT: movies: A LinkedList of Movies
// INPUT: field_to_index: Which field to use as an index
//
// RETURN: An Index (Hashtable) where the keys are values of the indexed fields,
//          and the values are MovieSets. 
Index BuildMovieIndex(LinkedList movies, enum IndexField field_to_index);


#endif
