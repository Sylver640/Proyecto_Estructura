#ifndef DATATYPES_H
#define DATATYPES_H
#include <stdio.h>
#include <stdlib.h>

#include "treemap.h"
#include "list.h"
#include "hashmap.h"

typedef struct movieType
{
        char* movieName;
        char* movie_id;
        int* year;
        List* genres; 
        int* userScore;
        int* runtime;       
} movieType;

typedef struct userType
{
        char* user_id;
        int movieNumber;
        HashMap* movieMap;
        TreeMap* abcOrder;
        TreeMap* yearOrder;
        TreeMap* ratingOrder;
        TreeMap* runtimeOrder;
        HashMap* moviesByGenre;
} userType;

typedef struct movieCategory
{
        char* name;
        List* movie_list;
} movieCategory;

#endif