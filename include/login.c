#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>

#include "list.h"
#include "hashmap.h"
#include "treemap.h"
#include "menu.h"
#include "csv.h"
#include "initialize.h"
#include "datatypes.h"

//Función para exportar una película al archivo del usuario
void exportMovie(movieType* movie, char* username, char* year, char* runtime, char* score)
{
        char path[100];
        snprintf(path, sizeof(path), "users/%s.csv", username);
        FILE* f = fopen(path, "at"); //se abre el archivo en modo adición
        fputs(movie->movie_id, f);
        fputc(',', f);
        fputc('"', f);
        fputs(movie->movieName, f);
        fputc('"', f);
        fputc(',', f);
        fputs(year, f);
        fputc(',', f);
        fputc('"', f);
        char* genre = firstList(movie->genres);
        fputs(genre, f);
        genre = nextList(movie->genres);
        if (genre != NULL)
                fputc(',', f);
        while (genre != NULL)
        {
                fputs(genre, f);
                genre = nextList(movie->genres);
                if (genre == NULL) break;
                fputc(',', f);
        }
        fputc('"', f); 
        fputc(',', f);
        fputs(runtime, f);
        fputc(',', f);
        fputs(score, f);
        if (feof(f) == 0) //se imprime un salto de línea si no es el final del archivo   
                putc('\n', f);
        fclose(f);

}

//Función para insertar una película
void insertMovie(HashMap* globalMap, movieType* movie, userType* user, char* year, char* score, char* runtime)
{
        insertMap(globalMap, movie->movie_id, movie); //se inserta en el mapa global

        if (searchMap(user->movieMap, movie->movie_id) != NULL) //si la película ya existe en el perfil del usuario, se retorna
                return;

        insertMap(user->movieMap, movie->movie_id, movie); //se inserta en el mapa por id
                        
        insertTreeMap(user->abcOrder, movie->movieName, movie); //se inserta en el mapa por orden alfabético

        //A continuación, como se trabaja con mapas ordenados y las claves se podían repetir, primero se busca en el mapa la categoría.
        //Si esta no existe, se crea una nueva variable movieCategory, su nombre es aquel de la categoría a agregar, y se agrega
        //la película en su lista correspondiente, para luego agregar esta nueva categoría al mapa. En cambio, si ya existe solo
        //se busca en el mapa y se agrega el filme a su lista.

        if (searchTreeMap(user->yearOrder, movie->year) == NULL)
        {
                movieCategory* newCategory = (movieCategory*) malloc (sizeof(movieCategory));
                newCategory->name = (char*) malloc (100*sizeof(char));
                strcpy(newCategory->name, year);
                newCategory->movie_list = createList();
                newCategory->numberOfMovies = 0;
                pushBack(newCategory->movie_list, movie);
                newCategory->numberOfMovies++;
                insertTreeMap(user->yearOrder, movie->year, newCategory);
        }
        else
        {
                Pair* searchTreeMapData = searchTreeMap(user->yearOrder, movie->year);
                movieCategory* yearSearched = searchTreeMapData->value;
                pushBack(yearSearched->movie_list, movie);
                yearSearched->numberOfMovies++;
        }

        char* searched_genre = firstList(movie->genres);
        Par* searchGenre;
        while (searched_genre != NULL)
        {
                searchGenre = searchMap(user->moviesByGenre, searched_genre);
                if (searchGenre == NULL)
                {
                        movieCategory* newCategory = (movieCategory*) malloc (sizeof(movieCategory));
                        newCategory->name = (char*) malloc (100*sizeof(char));
                        searched_genre[0] = toupper(searched_genre[0]);
                        strcpy(newCategory->name, searched_genre);
                        newCategory->movie_list = createList();
                        newCategory->numberOfMovies = 0;
                        pushBack(newCategory->movie_list, movie);
                        newCategory->numberOfMovies++;
                        insertMap(user->moviesByGenre, searched_genre, newCategory);
                }
                else
                {
                        movieCategory* genreInMap = searchGenre->value;
                        pushBack(genreInMap->movie_list, movie);
                        genreInMap->numberOfMovies++;
                }
                searched_genre = nextList(movie->genres);
        }

        if (searchTreeMap(user->runtimeOrder, movie->runtime) == NULL)
        {
                movieCategory* newCategory = (movieCategory*) malloc (sizeof(movieCategory));
                newCategory->name = (char*) malloc (100*sizeof(char));
                strcpy(newCategory->name, runtime);
                newCategory->movie_list = createList();
                newCategory->numberOfMovies = 0;
                pushBack(newCategory->movie_list, movie);
                newCategory->numberOfMovies++;
                insertTreeMap(user->runtimeOrder, movie->runtime, newCategory);
        }
        else
        {
                Pair* searchTreeMapData = searchTreeMap(user->runtimeOrder, movie->runtime);
                movieCategory* runtimeSearched = searchTreeMapData->value;
                pushBack(runtimeSearched->movie_list, movie);
                runtimeSearched->numberOfMovies++;
        }
                        
        if (searchTreeMap(user->ratingOrder, movie->userScore) == NULL)
        {
                movieCategory* newCategory = (movieCategory*) malloc (sizeof(movieCategory));
                newCategory->name = (char*) malloc (100*sizeof(char));
                strcpy(newCategory->name, score);
                newCategory->movie_list = createList();
                newCategory->numberOfMovies = 0;
                pushBack(newCategory->movie_list, movie);
                newCategory->numberOfMovies++;
                insertTreeMap(user->ratingOrder, movie->userScore, newCategory);
        }
        else
        {
                Pair* searchTreeMapData = searchTreeMap(user->ratingOrder, movie->userScore);
                movieCategory* ratingSearched = searchTreeMapData->value;
                pushBack(ratingSearched->movie_list, movie);
                ratingSearched->numberOfMovies++;
        }
        user->movieNumber++; //se aumenta en una unidad el número de películas del usuario en uno
        
}

//Inicio de sesión de un usuario en específico
void login (char* username, userType* loggedUser, HashMap* globalMovieMap, HashMap* usersMap)
{
        char path[100];
        system("cls");
        logo();

        //aquí tengo que arreglar un problema donde se acepta la tecla enter como username, y crea un csv con nombre basura
        //Se lee el nombre del usuario
        gotoxy(25, 5);
        printf("Enter your username: ");
        scanf("%s", username);
        getchar();

        snprintf(path, sizeof(path), "users/%s.csv", username);
        FILE* f = fopen(path, "rt"); //se abre el archivo del usuario en modo lectura
        if (f == NULL)
        {
                //Si el usuario no se encuentra en la carpeta, se crea su archivo csv y se retorna al menú principal
                assignUserName(loggedUser, username);
                gotoxy(25, 7);
                printf("This user doesn't exist!\n");
                gotoxy(25,8);
                printf("Creating it now...\n");
                getch();
                f = fopen(path, "wt");
                fclose(f);
                return;
        }

        assignUserName(loggedUser, username);

        char linea[1024];
        int i;

        //Se lee el archivo csv, insertando cada película
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

        insertMap(usersMap, username, loggedUser); //se inserta al usuario en el mapa de éstos
        
        fclose(f); //se cierra el archivo
}

//Función para añadir a todos los demás usuarios
void addOtherUsers(char* ignored_user, HashMap* usersMap, HashMap* globalMovieMap)
{
        //Con ayuda de estas variables, será posible recorrer cada usuario dentro del directorio users

        DIR *d;
        struct dirent *dir;
        FILE *f;

        //String para evitar abrir de nuevo el archivo del usuario logueado
        char ignored_file[100];
        strcpy(ignored_file, ignored_user);
        strcat(ignored_file, ".csv");

        d = opendir("./users"); //se abre el directorio

        //Se recorre todo el directorio
        if (d)
        {
                while((dir = readdir(d)) != NULL)
                {
                        if (strcmp(dir->d_name, ignored_file) == 0) //si se encuentra el nombre del usuario logueado, se continua al siguiente archivo
                                continue;

                        f = fopen(dir->d_name, "rt"); //se abre el archivo en modo lectura
                        char newuser_username[100];
                        if (strstr(dir->d_name, ".csv") != NULL) //se verifica que el archivo sea .csv, para evitar errores
                        {
                                for (int i = 0; i < strlen(dir->d_name)-4; i++)
                                        newuser_username[i] = dir->d_name[i]; //se elimina el csv del nombre para copiarlo en los datos del usuario
                        } else continue;

                        userType* newUser = createUser();
                        assignUserName(newUser, newuser_username);
                        
                        char linea[1024];
                        int k;

                        //Se recorre el archivo
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
                        
                        memset(newuser_username,0,sizeof(newuser_username)); //para evitar errores, se limpia la variable correspondiente al nombre del usuario
                        fclose(f); //se cierra el archivo
                }
        }

        closedir(d); //se cierra el directorio
}