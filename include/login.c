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

void insertMovie(HashMap* globalMap, movieType* movie, userType* user, char* year, char* score, char* runtime)
{
        insertMap(globalMap, movie->movie_id, movie);

        if (searchMap(user->movieMap, movie->movie_id) != NULL)
                return;

        insertMap(user->movieMap, movie->movie_id, movie);
                        
        insertTreeMap(user->abcOrder, movie->movieName, movie);

        if (searchTreeMap(user->yearOrder, movie->year) == NULL)
        {
                movieCategory* newCategory = (movieCategory*) malloc (sizeof(movieCategory));
                newCategory->name = (char*) malloc (100*sizeof(char));
                strcpy(newCategory->name, year);
                newCategory->movie_list = createList();
                pushBack(newCategory->movie_list, movie);
                insertTreeMap(user->yearOrder, movie->year, newCategory);
        }
        else
        {
                Pair* searchTreeMapData = searchTreeMap(user->yearOrder, movie->year);
                movieCategory* yearSearched = searchTreeMapData->value;
                pushBack(yearSearched->movie_list, movie);
        }

        char* searched_genre = firstList(movie->genres);
        while (searched_genre != NULL)
        {
                if (searchMap(user->moviesByGenre, searched_genre) == NULL)
                {
                        movieCategory* newCategory = (movieCategory*) malloc (sizeof(movieCategory));
                        newCategory->name = (char*) malloc (100*sizeof(char));
                        strcpy(newCategory->name, searched_genre);
                        newCategory->movie_list = createList();
                        pushBack(newCategory->movie_list, movie);
                        insertMap(user->moviesByGenre, searched_genre, newCategory);
                }
                else
                {
                        Par* searchData = searchMap(user->moviesByGenre, searched_genre);
                        movieCategory* genreInMap = searchData->value;
                        pushBack(genreInMap->movie_list, movie);
                }
                searched_genre = nextList(movie->genres);
        }

        if (searchTreeMap(user->runtimeOrder, movie->runtime) == NULL)
        {
                movieCategory* newCategory = (movieCategory*) malloc (sizeof(movieCategory));
                newCategory->name = (char*) malloc (100*sizeof(char));
                strcpy(newCategory->name, runtime);
                newCategory->movie_list = createList();
                pushBack(newCategory->movie_list, movie);
                insertTreeMap(user->runtimeOrder, movie->runtime, newCategory);
        }
        else
        {
                Pair* searchTreeMapData = searchTreeMap(user->runtimeOrder, movie->runtime);
                movieCategory* runtimeSearched = searchTreeMapData->value;
                pushBack(runtimeSearched->movie_list, movie);
        }
                        
        if (searchTreeMap(user->ratingOrder, movie->userScore) == NULL)
        {
                movieCategory* newCategory = (movieCategory*) malloc (sizeof(movieCategory));
                newCategory->name = (char*) malloc (100*sizeof(char));
                strcpy(newCategory->name, score);
                newCategory->movie_list = createList();
                pushBack(newCategory->movie_list, movie);
                insertTreeMap(user->ratingOrder, movie->userScore, newCategory);
        }
        else
        {
                Pair* searchTreeMapData = searchTreeMap(user->ratingOrder, movie->userScore);
                movieCategory* ratingSearched = searchTreeMapData->value;
                pushBack(ratingSearched->movie_list, movie);
        }
        user->movieNumber++;

        //exportMovie();
        
}


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

        assignUserName(loggedUser, username);

        char linea[1024];
        int i;

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

                        insertMovie(globalMovieMap, newMovie, loggedUser, movieYear, userScore, runtime);
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

                        userType* newUser = createUser();
                        assignUserName(newUser, newuser_username);
                        
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

                                        insertMovie(globalMovieMap, newMovie, newUser, movieYear, userScore, runtime);
                                }
                        }

                        insertMap(usersMap, newuser_username, newUser);
                        
                        memset(newuser_username,0,sizeof(newuser_username));
                        fclose(f);
                }
        }

        closedir(d);
}