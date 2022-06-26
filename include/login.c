#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#include "list.h"
#include "hashmap.h"
#include "treemap.h"
#include "menu.h"
#include "csv.h"
#include "initialize.h"
#include "datatypes.h"

void login (char* username, userType* loggedUser, HashMap* globalMovieMap, HashMap* usersMap)
{
        char path[100];
        system("cls");
        logo();

        //aquí tengo que arreglar un problema donde se acepta la tecla enter como username, y crea un csv con nombre basura
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

        //loggedUser = createUser(username);

        char linea[1024];
        int i;
        Pair* searchTreeMapData;

        while (fgets(linea, 1023, f) != NULL)
        {
                for (i = 0; i < 1; i++)
                {
                        movieType* newMovie = createMovie();

                        char* movie_id = get_csv_field(linea, i);
                        strcpy(newMovie->movie_id, movie_id);

                        char* movieName = get_csv_field(linea, i+1);
                        strcpy(newMovie->movieName, movieName);
                        
                        char* movieYear = get_csv_field(linea, i+2);
                        int yearToNumber = atoi(movieYear);
                        *newMovie->year = yearToNumber;

                        char* movie_genres = get_csv_field(linea, i+3);
                        split_and_AddGenres(movie_genres, newMovie);

                        char* runtime = get_csv_field(linea, i+4);
                        int runtimeToNumber = atoi(runtime);
                        *newMovie->runtime = runtimeToNumber;
                        
                        char* userScore = get_csv_field(linea, i+5);
                        int scoreToNumber = atoi(userScore);
                        *newMovie->userScore = scoreToNumber;

                        insertMap(globalMovieMap, movie_id, newMovie);

                        if (searchMap(loggedUser->movieMap, movie_id) != NULL)
                                continue;

                        insertMap(loggedUser->movieMap, movie_id, newMovie);
                        
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
                                searchTreeMapData = searchTreeMap(loggedUser->yearOrder, newMovie->year);
                                movieCategory* yearSearched = searchTreeMapData->value;
                                pushBack(yearSearched->movie_list, newMovie);
                        }

                        char* searched_genre = firstList(newMovie->genres);
                        while (searched_genre != NULL)
                        {
                                if (searchMap(loggedUser->moviesByGenre, searched_genre) == NULL)
                                {
                                        movieCategory* newCategory = (movieCategory*) malloc (sizeof(movieCategory));
                                        newCategory->name = (char*) malloc (100*sizeof(char));
                                        strcpy(newCategory->name, searched_genre);
                                        newCategory->movie_list = createList();
                                        pushBack(newCategory->movie_list, newMovie);
                                        insertMap(loggedUser->moviesByGenre, searched_genre, newCategory);
                                }
                                else
                                {
                                        Par* searchData = searchMap(loggedUser->moviesByGenre, searched_genre);
                                        movieCategory* genreInMap = searchData->value;
                                        pushBack(genreInMap->movie_list, newMovie);
                                }
                                searched_genre = nextList(newMovie->genres);
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
                                searchTreeMapData = searchTreeMap(loggedUser->runtimeOrder, newMovie->runtime);
                                movieCategory* runtimeSearched = searchTreeMapData->value;
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
                                searchTreeMapData = searchTreeMap(loggedUser->ratingOrder, newMovie->userScore);
                                movieCategory* ratingSearched = searchTreeMapData->value;
                                pushBack(ratingSearched->movie_list, newMovie);
                        }
                        loggedUser->movieNumber++;
                }
        }

        insertMap(usersMap, username, loggedUser);
        
        fclose(f);
}

void addOtherUsers(char* ignored_user, HashMap* usersMap, HashMap* globalMovieMap)
{
        DIR *d;
        struct dirent *dir;
        FILE *f;

        char ignored_file[100];
        strcpy(ignored_file, ignored_user);
        strcat(ignored_file, ".csv");

        d = opendir("./users");

        if (d)
        {
                while((dir = readdir(d)) != NULL)
                {
                        if (strcmp(dir->d_name, ignored_file) == 0)
                                continue;

                        f = fopen(dir->d_name, "rt");
                        char newuser_username[100];
                        if (strstr(dir->d_name, ".csv") != NULL)
                        {
                                for (int i = 0; i < strlen(dir->d_name)-4; i++)
                                        newuser_username[i] = dir->d_name[i];
                        } else continue;

                        userType* newUser = createUser(newuser_username);
                        
                        char linea[1024];
                        int k;
                        Pair* searchData;

                        while(fgets(linea, 1023, f) != NULL)
                        {
                                for (k = 0; k < 1; k++)
                                {
                                        movieType* newMovie = createMovie();

                                        char* movie_id = get_csv_field(linea, k);
                                        strcpy(newMovie->movie_id, movie_id);

                                        char* movieName = get_csv_field(linea, k+1);
                                        strcpy(newMovie->movieName, movieName);
                        
                                        char* movieYear = get_csv_field(linea, k+2);
                                        int yearToNumber = atoi(movieYear);
                                        *newMovie->year = yearToNumber;

                                        char* movie_genres = get_csv_field(linea, k+3);
                                        split_and_AddGenres(movie_genres, newMovie);
                                        if (firstList(newMovie->genres) != NULL) printf("se insertan generos\n");getch(); //testing

                                        char* runtime = get_csv_field(linea, k+4);
                                        int runtimeToNumber = atoi(runtime);
                                        *newMovie->runtime = runtimeToNumber;
                                        
                                        char* userScore = get_csv_field(linea, k+5);
                                        int scoreToNumber = atoi(userScore);
                                        *newMovie->userScore = scoreToNumber;

                                        insertMap(globalMovieMap, movie_id, newMovie);

                                        if (searchMap(newUser->movieMap, movie_id) != NULL)
                                                continue;
                                        
                                        //aquí tendría que modularizar más esta parte => osea hacer una función que haga todo lo de abajo
                                        insertMap(newUser->movieMap, movie_id, newMovie);
                                        insertTreeMap(newUser->abcOrder, newMovie->movieName, newMovie);

                                        if (searchTreeMap(newUser->yearOrder, newMovie->year) == NULL)
                                        {
                                                movieCategory* newCategory = (movieCategory*) malloc (sizeof(movieCategory));
                                                newCategory->name = (char*) malloc (100*sizeof(char));
                                                strcpy(newCategory->name, movieYear);
                                                newCategory->movie_list = createList();
                                                pushBack(newCategory->movie_list, newMovie);
                                                insertTreeMap(newUser->yearOrder, newMovie->year, newCategory);
                                        }
                                        else
                                        {
                                                searchData = searchTreeMap(newUser->yearOrder, newMovie->year);
                                                movieCategory* yearSearched = searchData->value;
                                                pushBack(yearSearched->movie_list, newMovie);
                                        }
                                        char* searched_genre = firstList(newMovie->genres);
                                        while (searched_genre != NULL)
                                        {
                                                if (searchMap(newUser->moviesByGenre, searched_genre) == NULL)
                                                {
                                                        movieCategory* newCategory = (movieCategory*) malloc (sizeof(movieCategory));
                                                        newCategory->name = (char*) malloc (100*sizeof(char));
                                                        strcpy(newCategory->name, searched_genre);
                                                        newCategory->movie_list = createList();
                                                        pushBack(newCategory->movie_list, newMovie);
                                                        insertMap(newUser->moviesByGenre, searched_genre, newCategory);
                                                }
                                                else
                                                {
                                                        Par* searchData = searchMap(newUser->moviesByGenre, searched_genre);
                                                        movieCategory* genreInMap = searchData->value;
                                                        pushBack(genreInMap->movie_list, newMovie);
                                                }
                                                searched_genre = nextList(newMovie->genres);
                                        }

                                        if (searchTreeMap(newUser->runtimeOrder, newMovie->runtime) == NULL)
                                        {
                                                movieCategory* newCategory = (movieCategory*) malloc (sizeof(movieCategory));
                                                newCategory->name = (char*) malloc (100*sizeof(char));
                                                strcpy(newCategory->name, runtime);
                                                newCategory->movie_list = createList();
                                                pushBack(newCategory->movie_list, newMovie);
                                                insertTreeMap(newUser->runtimeOrder, newMovie->runtime, newCategory);
                                        }
                                        else
                                        {
                                                searchData = searchTreeMap(newUser->runtimeOrder, newMovie->runtime);
                                                movieCategory* runtimeSearched = searchData->value;
                                                pushBack(runtimeSearched->movie_list, newMovie);
                                        }
                                        

                                        if (searchTreeMap(newUser->ratingOrder, newMovie->userScore) == NULL)
                                        {
                                                movieCategory* newCategory = (movieCategory*) malloc (sizeof(movieCategory));
                                                newCategory->name = (char*) malloc (100*sizeof(char));
                                                strcpy(newCategory->name, userScore);
                                                newCategory->movie_list = createList();
                                                pushBack(newCategory->movie_list, newMovie);
                                                insertTreeMap(newUser->ratingOrder, newMovie->userScore, newCategory);
                                        }
                                        else
                                        {
                                                searchData = searchTreeMap(newUser->ratingOrder, newMovie->userScore);
                                                movieCategory* ratingSearched = searchData->value;
                                                pushBack(ratingSearched->movie_list, newMovie);
                                        }
                                        newUser->movieNumber++;
                                }
                        }

                        insertMap(usersMap, newuser_username, newUser);
                        
                        memset(newuser_username,0,sizeof(newuser_username));
                        fclose(f);
                        
                }
        }

        closedir(d);
}