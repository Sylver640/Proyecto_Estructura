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
#include "include/heap.h"
#include "include/menu.h"
#include "include/login.h"
#include "include/csv.h"
#include "include/datatypes.h"
#include "include/initialize.h"

//Función para añadir manualmente
void manualAdd(HashMap* globalMap, userType* user)
{
        system("cls"); //Se limpia la pantalla
        
        //Inicialización de variables
        movieType* newMovie = createMovie();
        char* movie_id = (char*) malloc (100*sizeof(char));
        char* genres = (char*) malloc (100*sizeof(char));
        
        gotoxy(30, 2);
        printf("We need some information first...\n");

        gotoxy(29, 3);
        //Recepción de datos
        printf("ID: ");
        scanf("%[^\n]s", newMovie->movie_id);
        getchar();
        Par* search = searchMap(user->movieMap, newMovie->movie_id);//Si la película se encuentra en el perfil del usuario, se muestran
                                                                    //sus datos y se retorna al menú anterior.
        if (search != NULL)
        {
                //Impresión de datos
                movieType* movieData = search->value;
                system("cls");
                gotoxy(30, 4);
                printf("You already added this movie!\n");
                gotoxy(30,5);
                printf("ID: %s\n", movieData->movie_id);
                gotoxy(30,6);
                printf("Name: %s\n", movieData->movieName);
                gotoxy(30,7);
                printf("Year: %d\n", *movieData->year);
                gotoxy(30,8);
                char* genre = firstList(movieData->genres);
                printf("Genres: %s", genre);
                while (genre != NULL)
                {
                        printf(", %s", genre);
                        genre = nextList(movieData->genres);
                }
                gotoxy(30,9);
                printf("Runtime (in minutes): %d\n", *movieData->runtime);
                gotoxy(30,10);
                printf("Your Rating: %d\n", *movieData->userScore);
                getch();
                return;
        }
        
        //Más lectura de datos
        gotoxy(29, 4);
        printf("Name: ");
        scanf("%[^\n]s", newMovie->movieName);
        getchar();
        
        gotoxy(29, 5);
        printf("Year: ");
        scanf("%d", newMovie->year);
        getchar();
        //Copia de año a char para su uso en la función de insertMovie
        char* string_year = (char*) malloc (20*sizeof(char));
        sprintf(string_year, "%d", *newMovie->year);
        gotoxy(29,6);
        
        printf("Genres (separated by commas): ");
        scanf("%[^\n]s", genres);
        getchar();
        split_and_AddGenres(genres, newMovie);
        gotoxy(29, 7);
        
        printf("Runtime (in minutes): ");
        scanf("%d", newMovie->runtime);
        getchar();
        //Copia de duración a char para su uso en la función de insertMovie
        char* string_runtime = (char*) malloc (20*sizeof(char));
        sprintf(string_runtime, "%d", *newMovie->runtime);
        
        gotoxy(29, 8);
        printf("Your Rating (from 1 to 10): ");
        scanf("%d", newMovie->userScore);
        getchar();
        //Copia de score a char para su uso en la función de insertMovie
        char* string_rating = (char*) malloc (20*sizeof(char));
        sprintf(string_rating, "%d", *newMovie->userScore);
        
        insertMovie(globalMap, newMovie, user, string_year, string_rating, string_runtime); //Se inserta la película
        exportMovie(newMovie, user->user_id, string_year, string_runtime, string_rating); //Se agrega al archivo del usuario

        gotoxy(30, 10);
        printf("Your movie was added!\n");
        
        getch();
}

//Función para importar películas desde archivo externo de IMDb
void import_csv(HashMap* globalMap, userType* user)
{
        system("cls");
        gotoxy(30,4);
        printf("Checking for \"ratings.csv\" file...\n");
        getch();

        if (fileExists("ratings.csv") == 0) //Se busca si el archivo está presente en la raíz del programa. Es decir, donde se ubica el ejecutable
        {
                gotoxy(30, 5);
                printf("File not found!\n");
                gotoxy(30, 6);
                printf("Make sure the file is in the same place as the executable.\n");
                getch();
                return; //Como no se encontró, se retorna al menú principal
        }

        FILE* f = fopen("ratings.csv", "rt"); //se abre el archivo ratings en modo lectura
        gotoxy(30,5);
        printf("File opened!\n");
        gotoxy(30,6);
        printf("Press any key to process your movies.\n");
        getch();

        system("cls");

        char path[100];
        snprintf(path, sizeof(path), "users/%s.csv", user->user_id); //se copia el nombre del usuario a un string que dirige a la ubicación de su archivo
        FILE* userfile = fopen(path, "at"); //se abre el archivo en modo adición

        char linea[1024];
        int i;
        long processedMovies = 0; //contador para luego mostrar en pantalla cuántas películas se agregaron

        fgets(linea, 1023, f);
        while (fgets(linea, 1023, f) != NULL)
        {
                //Se copian los datos de cada película con la ayuda de get_csv_field
                for (i = 0; i < 1; i++)
                {
                        char* check = get_csv_field(linea, i+5);
                        if (strcmp(check, "movie") != 0) //si se detecta que en la línea presente no se trabaja con una película, se continua a la siguiente
                                continue; 
                        movieType* newMovie = createMovie();
                        
                        char* csv_id = get_csv_field(linea, i);
                        char* movie_id = (char*) malloc (100*sizeof(char));
                        strncpy(movie_id, csv_id+2, strlen(csv_id)); //como el id en ratings.csv tiene dos 't' al principio, se copia lo que retorna get_csv_field
                                                                     //luego de dos caracteres
                        strcpy(newMovie->movie_id, movie_id);

                        if (searchMap(user->movieMap, movie_id) != NULL) //si se detecta que la película ya la tiene el usuario en su perfil, se continua con la siguiente línea
                                continue;

                        //Copia de datos
                        //Con atoi se convierte a entero para agregar el dato a la película (movieType)
                        
                        char* movieName = get_csv_field(linea, i+3);
                        strcpy(newMovie->movieName, movieName);

                        char* movieYear = get_csv_field(linea, i+8);
                        int yearToNumber = atoi(movieYear);
                        *newMovie->year = yearToNumber;

                        char* genres = get_csv_field(linea, i+9);
                        split_and_AddGenres(genres, newMovie);

                        char* runtime = get_csv_field(linea, i+7);
                        int runtimeToNumber = atoi(runtime);
                        *newMovie->runtime = runtimeToNumber;

                        char* userScore = get_csv_field(linea, i+1);
                        int scoreToNumber = atoi (userScore);
                        *newMovie->userScore = scoreToNumber;

                        if (searchMap(user->movieMap, movie_id) == NULL)
                                processedMovies++; //si la película no se encontró en el mapa del usuario, el contador de agregadas aumenta en uno

                        insertMovie(globalMap, newMovie, user, movieYear, userScore, runtime);
                        exportMovie(newMovie, user->user_id, movieYear, runtime, userScore);
                }
        }

        gotoxy(30, 3);
        printf("%ld movies were added to your profile.\n", processedMovies);
        gotoxy(30, 5);
        printf("Press any key to return to the main menu.");
        
        fclose(f);
        getch();
}

//Función que entrega opciones para añadir películas
void addMovies(HashMap* globalMap, userType* user)
{
        //Inicialización de variables

        int option;
        char choice[2];
        //Creación de submenú
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
                        while (choice[0] != 'n') //bucle para confirmar si el usuario desea agregar otra película
                        {
                                system("cls");
                                gotoxy(28, 3);
                                printf("Do you wish to add another movie? (y/n) : ");
                                scanf("%[^\n]s", choice);
                                getchar();
                                if (choice[0] == 'y')
                                        manualAdd(globalMap, user);
                        }
                        break;
                case 2: import_csv(globalMap, user);
                        break;
        }

}

//función para ver si una película ha sido vista por logged user
movieType* searchMovieUserList(userType* user, char* movie_id) 
{
        Par* searchResult = searchMap(user->movieMap, movie_id);
        if (searchResult != NULL)
        {
                movieType* foundMovie = searchResult->value;
                return foundMovie;
        } 
        return NULL;
}

void movieDiscovery (HashMap* usersMap, userType* loggedUser)
{
        getchar();
        char username[100];
        printf("Enter other username for comparison: ");
        scanf("%[^\n]s", username);
        getchar();

        Par* searched_result = searchMap(usersMap, username);
        if ( searched_result == NULL)
        {
                //no se ha encontrado el usuario buscado
                printf("The user you are searching for does not exist.");
                getchar();
                return;
        }
        userType* otherUser = searched_result->value;
        
        bool hasRecommended = false;
        Pair* firstRating = firstTreeMap(otherUser->ratingOrder);
        if (firstRating == NULL)
        {
                //este usuario no ha calificado ninguna pelicula
                printf("The user you are searching for has not rated any movie.\n");
                getchar();
                return;
        }
        movieCategory* firstMovieRating = firstRating->value;
        movieType* currentMovie = firstList(firstMovieRating->movie_list);
        while (currentMovie != NULL)
        {
                //saltar peliculas ya vistas por el usuario
                if (searchMovieUserList(loggedUser, currentMovie->movie_id) == NULL)
                {
                        //película no ha sido vista, se recomienda a logged user
                        hasRecommended = true;
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
                }
                currentMovie = nextList(firstMovieRating->movie_list);
        }
        if (hasRecommended == false)
        {
                //no se han encontrado peliculas por recomendar
                printf("Sorry, we couldn't find a movie to recommend you :(\n");
        }
        getchar();
}

//función para encontrar todas las películas en comun que han visto usuario1 y usuario2
List* findAllMatchingMovies (userType* user1, userType* user2)
{
        List* matchingMovies = createList();
        if (matchingMovies == NULL)
        {
                printf("error creating list of matching movies\n");
                getchar();
                return NULL;
        }
        Par* currentMoviePair = firstMap(user1->movieMap);
        while (currentMoviePair != NULL)
        {
                movieType* foundMovie = searchMovieUserList(user2, currentMoviePair->key);
                if (foundMovie != NULL)
                {       
                        pushBack(matchingMovies, foundMovie->movie_id);
                }
                currentMoviePair = nextMap(user1->movieMap);
        }
        getchar();
        return matchingMovies;
}

int calculateAffinity (userType* user1, userType* user2)
{
        int sumAffinity = 0; //suma de afinidades, dividida al final por la cantidad de peliculas
        int numMovies = 0; //cantidad de peliculas
        List* matchingMovies = findAllMatchingMovies(user1, user2);
        char* currentMovieID = firstList(matchingMovies);
        while (currentMovieID != NULL)
        {
                numMovies++;
                movieType* movie1 = searchMovieUserList(user1, currentMovieID);
                movieType* movie2 = searchMovieUserList(user2, currentMovieID);
                int score1 = *(movie1->userScore);
                int score2 = *(movie2->userScore);

                //calculo afinidad, afinidad es un porcentaje que va de 0 a 100
                //100 siendo la misma nota, y 1 cuando las notas son los extremos opuestos
                int affinity = 100 - (abs(score1-score2))*11; //se necesita el valor absoluto del calculo
                sumAffinity += affinity; //se suma la afinidad a la cantidad total de afinidad
                currentMovieID = nextList(matchingMovies);
        }
        if (numMovies == 0) return 0;
        //afinidad promedio se calcula con la suma de las afinidades dividido por el numero de peliculas
        int avgAffinity = sumAffinity / numMovies; 
        return avgAffinity;
}

void userDiscovery (userType* loggedUser, HashMap* usersMap)
{
        printf("Searching for compatible users...\n");
        getchar();
        bool hasFoundUser = false;
        Par* currentUserPair = firstMap(usersMap);
        while (currentUserPair != NULL)
        {
                userType* currentUser = currentUserPair->value;
                int affinity = calculateAffinity(loggedUser, currentUser);
                //se consideran usuarios afines a aquellos que tengan más o igual a 75% de afinidad
                if (affinity >= 75)
                {
                        hasFoundUser = true;
                        printf("user name: %s\n", currentUser->user_id);
                        printf("number of movies: %d\n", currentUser->movieNumber);
                        printf("affinity percentage: %d\n", affinity);
                        printf("\n");
                }
                currentUserPair = nextMap(usersMap);
        }
        if (hasFoundUser == false)
        {
                printf("No users have been found. \n");
        }
        printf("Search finished.\n");
        getchar();
}

void searchByID(HashMap* allMovies, char* ID){
        system("cls");
        Par* foundMovie = searchMap(allMovies, ID);
        gotoxy(30,4);
        printf("Searching for the movie...");
        Sleep(3000);
        if(foundMovie != NULL){
            system("cls");
            gotoxy(30,1);    
            printf("Your movie has been found!\n\n");    
            movieType* movieData = foundMovie->value;
            gotoxy(30,3);
            printf("Title: %s\n", movieData->movieName);
            gotoxy(30,4);
            printf("ID: %s\n", movieData->movie_id);
            gotoxy(30,5);
            printf("Year: %d\n", *movieData->year);
            gotoxy(30,6);
            char* genre = firstList(movieData->genres);
            printf("Genres: ");
            if(genre != NULL){
                printf("%s", genre);
                genre = nextList(movieData->genres);
                while(genre != NULL){
                  printf(", %s", genre);
                  genre = nextList(movieData->genres);
                }
                printf(".");
            }
            printf("\n");
            gotoxy(30,7);
            printf("User Score: %d\n", *movieData->userScore);
            gotoxy(30,8);
            printf("Runtime: %d minutes\n", *movieData->runtime);

            gotoxy(30,10);
            printf("Press any button to return to the main menu.");

        }else{
          system("cls");      
          gotoxy(30,4);
          printf("We are sorry, your movie could not be found.\n");
          gotoxy(30,6);
          printf("Press any button to return to the main menu.");
        }
        getch();
}

void showMoviesTitle(TreeMap* TitleMap, char* user_Name){
        gotoxy(30,4);
        printf("Preparing to show the movies...");
        Sleep(2000);
        Pair* position = firstTreeMap(TitleMap);
        if(position == NULL){
                system("cls");
                gotoxy(30,4);
                printf("There are no movies to show!");
                gotoxy(30,6);
                printf("Press any button to return to the main menu.");
                getch();
                return;
        }

        printf("\n");

        while(position != NULL){
                movieType* dataInPos = position->value;
                printf("%s, %d\n", dataInPos->movieName, *dataInPos->year);
                position = nextTreeMap(TitleMap);
        }

        printf("\n");
        printf("All movies have been successfully analysed, press any button to return to the menu");
        getch();

}

void showMoviesThreeFiveSix(TreeMap* criteriaMap, char* user_Name){
        gotoxy(30,4);
        printf("Preparing to show the movies...");
        Sleep(2000);
        Pair* position = firstTreeMap(criteriaMap);
        if(position == NULL){
                system("cls");
                gotoxy(30,4);
                printf("There are no movies to show!");
                gotoxy(30,6);
                printf("Press any button to return to the main menu.");
                getch();
                return;
        }

        printf("\n");

        while(position != NULL){
                movieCategory* dataInPos = position->value;
                movieType* movieData = firstList(dataInPos->movie_list);


                while(movieData != NULL){
                        printf("%s, %d\n", movieData->movieName, *movieData->year);
                        movieData = nextList(dataInPos->movie_list);
                }

                position = nextTreeMap(criteriaMap);
        }

        printf("\n");
        printf("All movies have been successfully analysed, press any button to return to the menu");
        getch();

}

void showMoviesGenre(HashMap* genreMap, char* user_Name){
        //Pedir genero y hacer searchMap().
        char* pickedGenre = (char*)malloc(100*sizeof(char));
        gotoxy(30,4);
        printf("Please enter a movie genre: ");
        scanf("%s", pickedGenre);
        getchar();

        system("cls");
        gotoxy(30,4);
        printf("Preparing to show the movies...");
        Sleep(2000);
        Par* foundGenre = searchMap(genreMap, pickedGenre);
        if(foundGenre != NULL){
            system("cls");
            gotoxy(30,1);    
            printf("The movies that contain this genre are:\n\n");    
            movieCategory* category = foundGenre->value;
            movieType* movieData = firstList(category->movie_list);

            while(movieData != NULL){
                printf("%s\n", movieData->movieName);
                movieData = nextList(category->movie_list);
            }

            printf("\n\nPress any button to return to the main menu.");
        }else{
          system("cls");      
          gotoxy(30,4);
          printf("We are sorry, there are no movies containing this genre\n");
          gotoxy(30,6);
          printf("Press any button to return to the main menu.");
        }
        getch();

}

void showMoviesInYear(TreeMap* yearMap, char* user_Name){
        int* pickedYear = (int*)malloc(sizeof(int));
        gotoxy(30,4);
        printf("Please enter a year of movies: ");
        scanf("%i", pickedYear);
        getchar();

        system("cls");
        gotoxy(30,4);
        printf("Preparing to show the movies...");
        Sleep(2000);
        Pair* foundYear = searchTreeMap(yearMap, pickedYear);

        if(foundYear != NULL){
            system("cls");
            gotoxy(30,1);    
            printf("The movies released within this year are:\n\n");
            movieCategory* dataInYear = foundYear->value;    
            movieType* movieData = firstList(dataInYear->movie_list);

            while(movieData != NULL){
                printf("%s\n", movieData->movieName);
                movieData = nextList(dataInYear->movie_list);
            }

            printf("\nPress any button to return to the main menu.");
        }else{
          system("cls");      
          gotoxy(30,4);
          printf("We are sorry, there are no movies released within this year\n");
          gotoxy(30,6);
          printf("Press any button to return to the main menu.");
        }
        getch();
        //Pedir año como rango.
        //Buscar la posicion con key dicho año.
        //Mostrar todas las peliculas de la lista de ese genero y su info.


}

void movieCriteria(userType* user_Info, char* user){
        int option;
        system("cls");
        gotoxy(30, 2);
        printf("(1) By Genre\n");
        gotoxy(30,3);
        printf("(2) By Year\n");
        gotoxy(30,4);
        printf("(3) By Rating\n");
        gotoxy(30,5);
        printf("(4) By Title\n");
        gotoxy(30,6);
        printf("(5) By Runtime\n");
        gotoxy(30,7);
        printf("(6) By Release Date\n");
        gotoxy(30,8);
        printf("Select a criteria: ");
        scanf("%i", &option);
        getchar();

        switch(option)
        {
                case 1: system("cls");
                        showMoviesGenre(user_Info->moviesByGenre, user);
                        break;

                case 2: system("cls");
                        showMoviesInYear(user_Info->yearOrder, user);
                        break;

                case 3: system("cls");
                        showMoviesThreeFiveSix(user_Info->ratingOrder, user);
                        break;

                case 4: system("cls");
                        showMoviesTitle(user_Info->abcOrder, user);
                        break;

                case 5: system("cls");
                        showMoviesThreeFiveSix(user_Info->runtimeOrder, user);
                        break;

                case 6: system("cls");
                        showMoviesThreeFiveSix(user_Info->yearOrder, user);
                        break;
        }
}

void userChoice(HashMap* allUsers, char* userName, userType* loggedUserInfo){
        int option;
        char* user = (char*) malloc(100*sizeof(char));
        Par* foundUser; 
        system("cls");
        gotoxy(30, 2);
        printf("(1) Show your own movies\n");
        gotoxy(30,3);
        printf("(2) Show movies from another user\n");
        gotoxy(30,4);
        printf("Select an option: ");
        scanf("%i", &option);
        getchar();

        switch(option)
        {
                case 1: system("cls");
                        movieCriteria(loggedUserInfo, userName);
                        break;

                case 2: system("cls");
                        gotoxy(30,4);
                        printf("Please enter the user you're looking for: ");
                        scanf("%s", user);
                        system("cls");
                        gotoxy(30,4);
                        printf("Searching for your user...");
                        Sleep(3000);
                        foundUser = searchMap(allUsers, user);
                        if(foundUser != NULL){
                             gotoxy(30,5);
                             printf("The user has been found!");
                             Sleep(2000);   
                             userType* userInfo = foundUser->value;
                             movieCriteria(userInfo, user);

                        }else{
                           gotoxy(30,5);
                           printf("The user has not been found.");
                           gotoxy(30,7);
                           printf("Press any button to return to the main menu.");
                           getch();
                        }
                        break;
        }
}

void showProfile(userType* user, Mheap* genres, Mheap* favorites)
{
        gotoxy(35, 1);
        printf("User: %s\n", user->user_id);
        gotoxy(29, 2);
        printf("Movies Watched: %ld\n", user->movieNumber);
        gotoxy(28, 3);
        printf("Favorite Movies: ");
        int k = 4;
        for (int i = 0; i < 5; i++)
        {
                movieType* favorite = heap_top(favorites);
                heap_pop(favorites);
                printf ("%s (%i)\n", favorite->movieName, *favorite->year);
                gotoxy(45, k);
                k++;
        }
        gotoxy(22, 8);
        printf("Least Favorite Movies: ");
        int i = 0;
        k = 9;
        Pair* data = firstTreeMap(user->ratingOrder);
        while (data != NULL)
        {
                movieCategory* worse = data->value;
                movieType* movie = firstList(worse->movie_list);
                while (movie != NULL)
                {
                        if (i >= 5) break;       
                        printf("%s (%i)\n", movie->movieName, *movie->year);
                        gotoxy(45, k);
                        i++;
                        k++;
                        movie = nextList(worse->movie_list);
                }
                data = nextTreeMap(user->ratingOrder);
        }
        gotoxy(22, 13);
        printf("%s most viewed genres: ", user->user_id);

        //gotoxy(22, 14);
        //printf("Press any key to return");
        getch();
}

//Función que muestra el perfil del usuario que inició sesión
void loggedProfile(userType* user)
{
        system("cls");

        Mheap* genreOrder = createMheap();
        Par* genreData = firstMap(user->moviesByGenre);
        while (genreData != NULL)
        {
                movieCategory* genre = genreData->value;
                heap_push(genreOrder, genre, genre->numberOfMovies);
                genreData = nextMap(user->moviesByGenre);
        }

        Mheap* ratingOrder = createMheap();
        Pair* ratingData = firstTreeMap(user->ratingOrder);
        while (ratingData != NULL)
        {
                movieCategory* rating = ratingData->value;
                movieType* movie = firstList(rating->movie_list);
                while (movie != NULL)
                {
                        double score = *movie->userScore;
                        heap_push(ratingOrder, movie, score);
                        movie = nextList(rating->movie_list);
                }
                ratingData = nextTreeMap(user->ratingOrder);
        }

        /*for (int i = 0; i < 3; i++)
        {
                movieCategory* genre = heap_top(genreOrder);
                heap_pop(genreOrder);
                printf("genero: %s // cantidad: %.0lf\n", genre->name, genre->numberOfMovies);
        }*/

        showProfile(user, genreOrder, ratingOrder);
}

//Función que muestra el perfil de un usuario que se busca en el mapa
void userProfile(HashMap* usersMap)
{

        getch();
}

//Función que muestra las opciones para ver los perfiles de algún usuario
void showProfiles(HashMap* usersMap, userType* loggedUser)
{
        //Creación de submenú

        int option;
        system("cls");
        gotoxy(30, 2);
        printf("(1) Your Profile\n");
        gotoxy(30,3);
        printf("(2) Search for another user's profile\n");
        gotoxy(30,4);
        printf("Select an option: ");
        scanf("%i", &option);
        getchar();
        
        switch(option)
        {
                case 1: loggedProfile(loggedUser);
                        break;
                case 2: userProfile(usersMap);
                        break;
        }
}

int main()
{
    //Inicialización de variables
            
    HashMap* usersMap = createMap(30); //Mapa de usuarios
    HashMap* globalMovieMap = createMap(30); //Mapa global que contiene todas las películas
    char* loggedUserName = (char*) malloc (100*sizeof(char));
    userType* loggedUser = createUser();
    char* movieID = (char*) malloc(100*sizeof(char)); //<-- Para recibir el ID de la pelicula a buscar en la función 2.
    
    login(loggedUserName, loggedUser, globalMovieMap, usersMap);
    addOtherUsers(loggedUserName, usersMap, globalMovieMap);
    
    //Creación de menú

    int option;
    while (1)
    {
        system("cls");
        gotoxy(30, 1);
        printf("WELCOME, %s\n", loggedUserName); //se imprime el nombre del usuario
        gotoxy(30, 2);
        printf("1. Add movies\n");
        gotoxy(30, 3);
        printf("2. Search for a movie\n");
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
                        break;
                case 3: system("cls");
                        gotoxy(30,4);
                        userChoice(usersMap, loggedUserName, loggedUser);
                        break;
                case 4: system("cls");
                        gotoxy(30,4);
                        movieDiscovery(usersMap, loggedUser);
                        break;
                case 5: system("cls");
                        gotoxy(30,4);
                        userDiscovery(loggedUser, usersMap);
                        break;
                case 6: showProfiles(usersMap, loggedUser);
                        break;
                case 0: system("cls");
                        centerText("See you later :)", 5);
                        Sleep(2000);
                        system("cls");
                        return 0;
        }
    }
}