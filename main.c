/*
COMENTARIOS:
- Promediar cada película según un usuario la vaya agregando
*/

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <math.h>
#include <windows.h>

#include "include/list.h"
#include "include/hashmap.h"
#include "include/treemap.h"
#include "include/menu.h"

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

/*
  función para comparar claves de tipo string
  retorna 1 si son iguales
*/
int is_equal_string(void * key1, void * key2) {
    if(strcmp((char*)key1, (char*)key2)==0) return 1;
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
  retorna 1 si son iguales
*/
int is_equal_int(void * key1, void * key2) {
    if(*(int*)key1 == *(int*)key2) return 1;
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

//Función que recibe campos de un archivo CSV separado por comas.
char *get_csv_field (char * tmp, int k) {
    int open_mark = 0;
    char* ret=(char*) malloc (100*sizeof(char));
    int ini_i=0, i=0;
    int j=0;
    while(tmp[i+1]!='\0'){

        if(tmp[i]== '\"'){
            open_mark = 1-open_mark;
            if(open_mark) ini_i = i+1;
            i++;
            continue;
        }

        if(open_mark || tmp[i]!= ','){
            if(k==j) ret[i-ini_i] = tmp[i];
            i++;
            continue;
        }

        if(tmp[i]== ','){
            if(k==j) {
               ret[i-ini_i] = 0;
               return ret;
            }
            j++; ini_i = i+1;
        }

        i++;
    }

    if(k==j) {
       ret[i-ini_i] = 0;
       return ret;
    }

    return NULL;
}

void login (char* username, userType* loggedUser, HashMap* globalMovieMap)
{
        char path[100];
        system("cls");
        logo();

        gotoxy(25, 5);
        printf("Enter your username: ");
        scanf("%[^\n]s", username);
        getchar();

        snprintf(path, sizeof(path), "users/%s.csv", username);
        FILE* f = fopen(path, "rt");
        if (f == NULL)
        {
                gotoxy(25, 7);
                printf("This user doesn't exist!\n");
                gotoxy(25,8);
                printf("Creating it now...\n");
                getch();
                f = fopen(path, "wt");
                return;
        }

        strcpy(loggedUser->user_id, username);
        printf("se copia nombre de usuario\n");
        loggedUser->movieNumber = 0;
        loggedUser->movieMap = createMap(20);
        loggedUser->abcOrder = createTreeMap(lower_than_string);
        loggedUser->moviesByGenre = createMap(20);
        loggedUser->ratingOrder = createTreeMap(lower_than_int);
        loggedUser->runtimeOrder = createTreeMap(lower_than_int);
        loggedUser->yearOrder = createTreeMap(lower_than_int);

        char linea[1024];
        int i;
        int k = 0;
        Pair* searchData;

        while (fgets(linea, 1023, f) != NULL)
        {
                for (i = 0; i < 1; i++)
                {
                        movieType* newMovie = (movieType*) malloc (sizeof(movieType));
                        newMovie->movie_id = (char*) malloc (100*sizeof(char));
                        newMovie->movieName = (char*) malloc (200*sizeof(char));
                        newMovie->year = (int*) malloc (sizeof(int));
                        newMovie->runtime = (int*) malloc (sizeof(int));
                        newMovie->userScore = (int*) malloc (sizeof(int));

                        char* movie_id = get_csv_field(linea, i);
                        strcpy(newMovie->movie_id, movie_id);

                        char* movieName = get_csv_field(linea, i+1);
                        strcpy(newMovie->movieName, movieName);
                        
                        char* movieYear = get_csv_field(linea, i+2);
                        int yearToNumber = atoi(movieYear);
                        *newMovie->year = yearToNumber;

                        //aqui van a ir los generos, pero tengo que hacer la funcion de separar todavía :p

                        char* runtime = get_csv_field(linea, i+4);
                        int runtimeToNumber = atoi(runtime);
                        *newMovie->runtime = runtimeToNumber;
                        
                        char* userScore = get_csv_field(linea, i+5);
                        int scoreToNumber = atoi(userScore);
                        *newMovie->userScore = scoreToNumber;

                        insertMap(loggedUser->movieMap, movie_id, newMovie);
                        
                        insertMap(globalMovieMap, movie_id, newMovie);
                        
                        insertTreeMap(loggedUser->abcOrder, newMovie->movieName, newMovie);

                        if (searchTreeMap(loggedUser->yearOrder, newMovie->year) == NULL)
                        {
                                movieCategory* newCategory = (movieCategory*) malloc (sizeof(movieCategory));
                                newCategory->name = (char*) malloc (100*sizeof(char));
                                strcpy(newCategory->name, movieYear);
                                newCategory->movie_list = createList();
                                pushBack(newCategory->movie_list, newMovie);
                                insertTreeMap(loggedUser->yearOrder, newMovie->year, newCategory);
                        }
                        else
                        {
                                searchData = searchTreeMap(loggedUser->yearOrder, newMovie->year);
                                movieCategory* yearSearched = searchData->value;
                                pushBack(yearSearched->movie_list, newMovie);
                        }

                        if (searchTreeMap(loggedUser->runtimeOrder, newMovie->runtime) == NULL)
                        {
                                movieCategory* newCategory = (movieCategory*) malloc (sizeof(movieCategory));
                                newCategory->name = (char*) malloc (100*sizeof(char));
                                strcpy(newCategory->name, runtime);
                                newCategory->movie_list = createList();
                                pushBack(newCategory->movie_list, newMovie);
                                insertTreeMap(loggedUser->runtimeOrder, newMovie->runtime, newCategory);
                        }
                        else
                        {
                                searchData = searchTreeMap(loggedUser->runtimeOrder, newMovie->runtime);
                                movieCategory* runtimeSearched = searchData->value;
                                pushBack(runtimeSearched->movie_list, newMovie);
                        }
                        

                        if (searchTreeMap(loggedUser->ratingOrder, newMovie->userScore) == NULL)
                        {
                                movieCategory* newCategory = (movieCategory*) malloc (sizeof(movieCategory));
                                newCategory->name = (char*) malloc (100*sizeof(char));
                                strcpy(newCategory->name, userScore);
                                newCategory->movie_list = createList();
                                pushBack(newCategory->movie_list, newMovie);
                                insertTreeMap(loggedUser->ratingOrder, newMovie->userScore, newCategory);
                        }
                        else
                        {
                                searchData = searchTreeMap(loggedUser->ratingOrder, newMovie->userScore);
                                movieCategory* ratingSearched = searchData->value;
                                pushBack(ratingSearched->movie_list, newMovie);
                        }
                        loggedUser->movieNumber++;
                }
                k++;
        }
        
        fclose(f);
}

int main()
{
    HashMap* globalMovieMap = createMap(30);
    char* loggedUserName = (char*) malloc (100*sizeof(char));
    userType* loggedUser = (userType*) malloc (sizeof(userType));
    login(loggedUserName, loggedUser, globalMovieMap);
    //estaba pensando que, en vez de abrir un usuario cuando se necesite, tras el login del usuario, abrir los demás archivos con otra función
    //https://www.decodeschool.com/C-Programming/File-Operations/C-Program-to-read-all-the-files-located-in-the-specific-directory
    int option;
    //MENÚ PROVISORIO
    while (1)
    {
        system("cls");
        gotoxy(30, 1);
        printf("WELCOME, %s\n", loggedUserName);
        gotoxy(30, 2);
        printf("1. Add movies\n");
        gotoxy(30, 3);
        printf("2. Search for a movie\n"); //así que, en vez de aquí buscar csv por csv, solo bastaría con abrir el mapa global
        gotoxy(30, 4);
        printf("3. Show user's movies\n");
        gotoxy(30, 5);
        printf("4. Movie Discovery\n");
        gotoxy(30, 6);
        printf("5. Users Discovery\n");
        gotoxy(30, 7);
        printf("6. Profiles\n");
        gotoxy(30, 8);
        printf("0. Exit\n");
        printf("\n");
        gotoxy(30, 9);
        printf("Select an option: ");
        scanf("%d", &option);    
        switch (option)
        {
                case 1: system("cls");
                        gotoxy(30,4);
                        printf("NOT IMPLEMENTED YET\n");
                        getch();
                        break;
                case 2: system("cls");
                        gotoxy(30,4);
                        printf("NOT IMPLEMENTED YET\n");
                        getch();
                        break;
                case 3: system("cls");
                        gotoxy(30,4);
                        printf("NOT IMPLEMENTED YET\n");
                        getch();
                        break;
                case 4: system("cls");
                        gotoxy(30,4);
                        printf("NOT IMPLEMENTED YET\n");
                        getch();
                        break;
                case 5: system("cls");
                        gotoxy(30,4);
                        printf("NOT IMPLEMENTED YET\n");
                        getch();
                        break;
                case 6: system("cls");
                        gotoxy(30,4);
                        printf("NOT IMPLEMENTED YET\n");
                        getch();
                        break;
                case 0: system("cls");
                        centerText("See you later :)", 5);
                        getch();
                        system("cls");
                        return 0;
        }
    }
    return 0;
}