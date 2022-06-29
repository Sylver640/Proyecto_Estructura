#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "hashmap.h"
#include "treemap.h"
#include "datatypes.h"

/*
  función para comparar claves de tipo string
  retorna 1 si son iguales
*/
int is_equal_string(void * key1, void * key2) {
    if(strcmp((char*)key1, (char*)key2)==0) return 1;
    return 0;
}

/*
  función para comparar claves de tipo int
  retorna 1 si son iguales
*/
int is_equal_int(void * key1, void * key2) {
    if(*(int*)key1 == *(int*)key2) return 1;
    return 0;
}

/*
  función para comparar claves de tipo string
  retorna 1 si son key1<key2
*/
int lower_than_string(void * key1, void * key2) {
    if(strcmp((char*)key1, (char*)key2) < 0) return 1;
    return 0;
}

/*
  función para comparar claves de tipo int
  retorna 1 si son key1<key2
*/
int lower_than_int(void * key1, void * key2) {
    if(*(int*)key1 < *(int*)key2) return 1;
    return 0;
}

//Función para inicializar a un usuario
userType* createUser()
{
        userType* newUser = (userType*) malloc (sizeof(userType));
        newUser->user_id = (char*) malloc (100*sizeof(char));
        newUser->movieNumber = 0;
        newUser->movieMap = createMap(20);
        newUser->abcOrder = createTreeMap(lower_than_string);
        newUser->moviesByGenre = createMap(20);
        newUser->ratingOrder = createTreeMap(lower_than_int);
        newUser->runtimeOrder = createTreeMap(lower_than_int);
        newUser->yearOrder = createTreeMap(lower_than_int);
        return newUser;
}

//Función para inicializar una película
movieType* createMovie()
{
        movieType* new = (movieType*) malloc (sizeof(movieType));
        new->movie_id = (char*) malloc (100*sizeof(char));
        new->movieName = (char*) malloc (200*sizeof(char));
        new->year = (int*) malloc (sizeof(int));
        new->genres = createList();
        new->runtime = (int*) malloc (sizeof(int));
        new->userScore = (int*) malloc (sizeof(int));
}

//Asignación de nombre para cada usuario
void assignUserName(userType* user, char* username)
{
  //se asegura que el usuario ya va a estar creado
  if (user == NULL) return;
  strcpy(user->user_id, username);
}