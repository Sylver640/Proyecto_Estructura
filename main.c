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
#include <dirent.h>

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

userType* createUser(char* username)
{
        userType* newUser = (userType*) malloc (sizeof(userType));
        newUser->user_id = (char*) malloc (100*sizeof(char));
        strcpy(newUser->user_id, username);
        newUser->movieNumber = 0;
        newUser->movieMap = createMap(20);
        newUser->abcOrder = createTreeMap(lower_than_string);
        newUser->moviesByGenre = createMap(20);
        newUser->ratingOrder = createTreeMap(lower_than_int);
        newUser->runtimeOrder = createTreeMap(lower_than_int);
        newUser->yearOrder = createTreeMap(lower_than_int);
        return newUser;
}

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

void split_and_AddGenres(char* genres, movieType* movie)
{
        char limit[3] = ", ";
        char* token = strtok(genres, limit);
        if (token != NULL)
        {
                while (token != NULL)
                {
                        pushBack(movie->genres, token);
                        token = strtok(NULL, limit);
                }
        }
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

//al terminar cada función de añadir, recordar actualizar el csv del usuario

void manualAdd(HashMap* globalMap, userType* user)
{
        system("cls");
        
        movieType* newMovie = createMovie();
        char* movie_id = (char*) malloc (100*sizeof(char));
        gotoxy(30, 2);
        printf("We need some information first...\n");

        gotoxy(29, 3);
        printf("Movie ID: ");
        scanf("%[^\n]s", movie_id);
        getchar();
        movieType* a = (movieType*) searchMap(user->movieMap, movie_id);
        if (a != NULL)
        {
                system("cls");
                gotoxy(30, 4);
                printf("You already added this movie!\n");
                return;
        }
        gotoxy(29, 4);
        printf("Name: ");
        scanf("%[^\n]s", newMovie->movieName);
        getchar();


        getch();
        return;
}

void import_csv(HashMap* globalMap, userType* user)
{
        system("cls");

        getch();
        return;
}

void addMovies(HashMap* globalMap, userType* user)
{
        int option;
        system("cls");
        gotoxy(30, 2);
        printf("(1) Add a movie manually\n");
        gotoxy(30,3);
        printf("(2) Import your ratings from IMDb\n");
        gotoxy(30,4);
        printf("Select an option: ");
        scanf("%i", &option);
        getchar();

        switch(option)
        {
                case 1: manualAdd(globalMap, user);
                        break;
                case 2: import_csv(globalMap, user);
                        break;
        }

}

void movieDiscovery (HashMap* usersMap, userType* loggedUser) //tendría que poner movie list para la búsqueda por género?
{
        //recomendar películas según coincidencia de películas favoritas con algún usuario (definir número), y recomendar películas que el usuario 
        //no haya visto pero si el usuario que encontramos con afinidad
        //si eso no funciona, hacer el mismo estilo de búsqueda pero con géneros
        char username[100];
        printf("Enter other username for comparison: ");
        scanf("%[^\n]s", username);
        getchar();

        Par* searched_result = searchMap(usersMap, username);
        if ( searched_result == NULL)
        {
                printf("The user you are searching for does not exist.");
                return;
        }
        userType* otherUser = searched_result->value;
        
        Pair* firstRating = firstTreeMap(otherUser->ratingOrder);
        movieCategory* firstMovieRating = firstRating->value;
        //pushBack(firstMovieRating->movie_list, newMovie);
        movieType* currentMovie = firstList(firstMovieRating->movie_list);
        while (currentMovie != NULL)
        {
                printf("movie name: %s\n", currentMovie->movieName);
                printf("movie ID: %s\n", currentMovie->movie_id);
                printf("year of release: %d\n", currentMovie->year);
                printf("movie genres: ");
                char* currentGenre = firstList(currentMovie->genres);
                while(currentGenre != NULL)
                {
                        printf("%s, ", currentGenre);
                        currentGenre = nextList(currentMovie->genres);
                }
                printf("\n");
                printf("user score: %d\n", currentMovie->userScore);
                printf("runtime: %d\n", currentMovie->runtime);
                currentMovie = nextList(firstMovieRating->movie_list);
        }
}

void searchByID(HashMap* allMovies, char* ID){
        system("cls");
        gotoxy(30,4);
        printf("function successfully initialized!\n");
        Par* foundMovie = searchMap(allMovies, ID);
        gotoxy(30,4);
        printf("Searching for movie...\n");
        if(foundMovie != NULL){
            printf("Your movie has been found!\n\n");    
            movieType* movieData = foundMovie->value;

            printf("Title: %s\nID: %s\nYear: %d\n", movieData->movieName, movieData->movie_id, movieData->year);

            char* genre = firstList(movieData->genres);
            printf("Genres: ");
            if(genre != NULL){
                while(genre != NULL){
                  printf("%s, ", genre);
                  genre = nextList(movieData->genres);
                }

            }
            printf("\n");

            printf("User Score: %d\nRuntime: %d minutes", movieData->userScore, movieData->runtime);

        }else printf("We are sorry, your movie could not be found.");
}

int main()
{
    HashMap* usersMap = createMap(30); //<-- así como veo, también veo más factible crear un mapa con los usuarios
    HashMap* globalMovieMap = createMap(30);
    char* loggedUserName = (char*) malloc (100*sizeof(char));
    userType* loggedUser = createUser("a");
    char* movieID = (char*) malloc(100*sizeof(char)); //<-- Para recibir el ID de la pelicula a buscar en la función 2.
    
    login(loggedUserName, loggedUser, globalMovieMap, usersMap);
    addOtherUsers(loggedUserName, usersMap, globalMovieMap);
    
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
                case 1: addMovies(globalMovieMap, loggedUser);
                        break;
                case 2: system("cls");
                        gotoxy(30,4);
                        printf("Please enter the ID of the movie you are looking for: ");
                        scanf("%s", movieID);
                        getchar();
                        searchByID(globalMovieMap, movieID);
                        gotoxy(30,4);
                        printf("WORK IN PROGRESS!\n");
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
}