#ifndef DATATYPES_H
#define DATATYPES_H
#include <stdio.h>
#include <stdlib.h>

#include "treemap.h"
#include "list.h"
#include "hashmap.h"

typedef struct movieType //Representa a cada película
{
        char* movieName;
        char* movie_id;
        int* year;
        List* genres; 
        int* userScore;
        int* runtime;       
} movieType;

typedef struct userType //Guarda todo lo necesario para el usuario
{
        char* user_id;
        long movieNumber;
        HashMap* movieMap; //mapa general
        TreeMap* abcOrder; //orden alfabético
        TreeMap* yearOrder; //orden de lanzamiento
        TreeMap* ratingOrder; //orden de puntaje
        TreeMap* runtimeOrder; //orden de duración
        HashMap* moviesByGenre; //películas ordenadas por género
} userType;

typedef struct movieCategory //Corresponde a cada categoría que se puede encasillar cada película. Útil para mapas ordenados
{
        char* name;
        List* movie_list;
        double numberOfMovies;
} movieCategory;

#endif