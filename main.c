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
        Sleep(2000); //se espera dos segundos para que la pantalla actualice con la información correspondiente

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

//funcion que recorre todo el arbol hasta que encuentra el ultimo par
Pair* lastTreeMap (TreeMap * tree)
{
        Pair* current = firstTreeMap(tree);
        if (current == NULL)
        {
                printf("there's no first element in the tree.\n");
                return NULL; 
        }
        Pair* next = nextTreeMap(tree);
        while (next != NULL)
        {
                current = next;
                next = nextTreeMap(tree);
        }
        return current;
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
        Pair* bestRating = lastTreeMap(otherUser->ratingOrder);
        if (bestRating == NULL)
        {
                //este usuario no ha calificado ninguna pelicula
                printf("The user you are searching for has not rated any movie.\n");
                getchar();
                return;
        }
        movieCategory* bestMovieRating = bestRating->value;
        movieType* currentMovie = firstList(bestMovieRating->movie_list);
        while (currentMovie != NULL)
        {
                //saltar peliculas ya vistas por el usuario
                if (searchMovieUserList(loggedUser, currentMovie->movie_id) == NULL)
                {
                        //película no ha sido vista, se recomienda a logged user
                        hasRecommended = true;
                        printf("movie name: %s\n", currentMovie->movieName);
                        printf("year of release: %d\n", *(currentMovie->year));
                        printf("movie genres: ");
                        char* currentGenre = firstList(currentMovie->genres);
                        while(currentGenre != NULL)
                        {
                                printf("%s, ", currentGenre);
                                currentGenre = nextList(currentMovie->genres);
                        }
                        printf("\n");
                        printf("user score: %d\n", *(currentMovie->userScore));
                        printf("runtime: %d\n", *(currentMovie->runtime));
                }
                currentMovie = nextList(bestMovieRating->movie_list);
        }
        printf("Searching finished.\n");
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

        Pair* currentRating = firstTreeMap(user1->ratingOrder);
        while(currentRating != NULL)
        {
                movieCategory* currentMovieRating = currentRating->value;
                movieType* currentMovie = firstList(currentMovieRating->movie_list);

                while (currentMovie != NULL)
                {
                        movieType* foundMovie = searchMovieUserList(user2, currentMovie->movie_id);
                        if (foundMovie != NULL)
                        {       
                                pushBack(matchingMovies, foundMovie->movie_id);
                        }
                        currentMovie = nextList(currentMovieRating->movie_list);
                }
                currentRating = nextTreeMap(user1->ratingOrder);
        }
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

void userDiscovery (HashMap* usersMap, userType* loggedUser)
{
        printf("Searching for compatible users...\n");
        bool hasFoundUser = false;
        Par* firstUserPair = firstMap(usersMap);
        Par* currentUserPair = firstMap(usersMap);
        while (currentUserPair != NULL)
        {
                userType* currentUser = currentUserPair->value;
                if (strcmp(currentUser->user_id, loggedUser->user_id) != 0)
                {
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
                } 
                currentUserPair = nextMap(usersMap);
                if (currentUserPair->key == firstUserPair->key)
                {
                        break;
                }
        }
        if (hasFoundUser == false)
        {
                printf("No users have been found. \n");
        }
        printf("Search finished.\n");
        printf("Press enter to go back to menu.\n");
        getchar();
        getchar();
}

void searchByID(HashMap* allMovies, char* ID){ //Función que ayuda a buscar una película por su ID.
        system("cls"); 
        Par* foundMovie = searchMap(allMovies, ID); //Variable que recibirá el pair de la película si se halla.
        gotoxy(30,4);
        printf("Searching for the movie...");
        Sleep(3000); 
        if(foundMovie != NULL){ //Si se encuentra la película, se procede a mostrar sus datos.
            system("cls");
            gotoxy(30,1);    
            printf("Your movie has been found!\n\n");  
            movieType* movieData = foundMovie->value; //Variable para acceder a los datos de la película.
            gotoxy(30,3);
            printf("Title: %s\n", movieData->movieName); //Nombre. 
            gotoxy(30,4);
            printf("ID: %s\n", movieData->movie_id); //ID.
            gotoxy(30,5);
            printf("Year: %d\n", *movieData->year); //Año.
            gotoxy(30,6);
            char* genre = firstList(movieData->genres); //Variable para acceder a la lista de generos de la película.
            printf("Genres: ");
            if(genre != NULL){
                printf("%s", genre); //Se muestra el primer genero, los demas irán acompañados de una coma.
                genre = nextList(movieData->genres);
                while(genre != NULL){
                  printf(", %s", genre);
                  genre = nextList(movieData->genres);
                }
                printf("."); //Al llegar al último genero, se imprime un punto.
            }
            printf("\n");
            gotoxy(30,7);
            printf("User Score: %d\n", *movieData->userScore); //Puntuación del usuario (1 al 10).
            gotoxy(30,8);
            printf("Runtime: %d minutes\n", *movieData->runtime); //Duración de película en minutos.

            gotoxy(30,10);
            printf("Press any button to return to the main menu."); 

        }else{ //Si no se halla la película, se avisa al usuario al respecto.
          system("cls");      
          gotoxy(30,4);
          printf("We are sorry, your movie could not be found.\n");
          gotoxy(30,6);
          printf("Press any button to return to the main menu.");
        }
        getch(); //Terminado el proceso, se espera a que el usuario oprima cualquier botón para volver al menú principal.
}

void showMoviesTitle(TreeMap* TitleMap, char* user_Name){ //Muestras las películas ordenadas por alfabeto.
        gotoxy(30,4);
        printf("Preparing to show the movies..."); 
        Sleep(2000);
        Pair* position = firstTreeMap(TitleMap); //Se ingresa a la primera posicion del mapa de películas.
        if(position == NULL){ //Si no existe, entonces no hay películas aún.
                system("cls");
                gotoxy(30,4);
                printf("There are no movies to show!");
                gotoxy(30,6);
                printf("Press any button to return to the main menu.");
                getch();
                return;
        }

        printf("\n");

        while(position != NULL){ //Muestra películas de la A a la Z.
                movieType* dataInPos = position->value; //Acceder a datos de una posicion.
                printf("%s, %d\n", dataInPos->movieName, *dataInPos->year); 
                position = nextTreeMap(TitleMap); 
        }

        printf("\n");
        printf("All movies have been successfully analysed, press any button to return to the menu");
        getch(); //Terminado el proceso, se regresa al menú principal al presionar cualquier botón.

}

void showMoviesThreeFiveSix(TreeMap* criteriaMap, char* user_Name){ //Muestra películas ordenadas por Rating, duración o fecha de lanzamiento.
        gotoxy(30,4);
        printf("Preparing to show the movies...");
        Sleep(2000);
        Pair* position = firstTreeMap(criteriaMap); //Recibe la primera posicion del mapa categorizado.
        if(position == NULL){ //Si no existen peliculas, se avisa al usuario y se retorna al menú principal.
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
                movieCategory* dataInPos = position->value; //Acceso a los datos de dicha posicion.
                movieType* movieData = firstList(dataInPos->movie_list); //Acceso a datos de una pelicula.


                while(movieData != NULL){ //Se muestran las películas respetando la condición.
                        printf("%s, %d\n", movieData->movieName, *movieData->year);
                        movieData = nextList(dataInPos->movie_list);
                }

                position = nextTreeMap(criteriaMap); //Siguiente posición.
        }

        printf("\n");
        printf("All movies have been successfully analysed, press any button to return to the menu");
        getch(); //Terminado el proceso, se regresa al menú principal al presionar cualquier botón.

}

void showMoviesGenre(HashMap* genreMap, char* user_Name){ //Muestra las películas que contienen un cierto genero.
        char* pickedGenre = (char*)malloc(100*sizeof(char)); //Recibe un genero insertado.
        gotoxy(30,4);
        printf("Please enter a movie genre: "); //Se pide ingresar un genero de películas.
        scanf("%s", pickedGenre);
        getchar();

        system("cls");
        gotoxy(30,4);
        printf("Preparing to show the movies...");
        Sleep(2000);
        Par* foundGenre = searchMap(genreMap, pickedGenre); //Recibe par del genero hallado.
        if(foundGenre != NULL){ //Si hay películas con dicho genero, se muestran por pantalla.
            system("cls");
            gotoxy(30,1);    
            printf("The movies that contain this genre are:\n\n");    
            movieCategory* category = foundGenre->value; //Ingreso a los datos de la categoría
            movieType* movieData = firstList(category->movie_list); //Ingreso a las películas de la categoría.

            while(movieData != NULL){ //Se muestran las películas que contienen al genero.
                printf("%s\n", movieData->movieName);
                movieData = nextList(category->movie_list);
            }

            printf("\n\nPress any button to return to the main menu.");
        }else{ //Si no hay peliculas con ese genero, se avisa al usuario por pantalla.
          system("cls");      
          gotoxy(30,4);
          printf("We are sorry, there are no movies containing this genre\n");
          gotoxy(30,6);
          printf("Press any button to return to the main menu.");
        }
        getch(); //Terminado el proceso, se regresa al menú principal al presionar cualquier botón.

}

void showMoviesInYear(TreeMap* yearMap, char* user_Name){ //Muestra las películas lanzadas dentro de un año específico.
        int* pickedYear = (int*)malloc(sizeof(int)); //Recibe el año insertado.
        gotoxy(30,4);
        printf("Please enter a year of movies: "); //Se pide al usuario un año de lanzamientos.
        scanf("%i", pickedYear);
        getchar();

        system("cls");
        gotoxy(30,4);
        printf("Preparing to show the movies...");
        Sleep(2000);
        Pair* foundYear = searchTreeMap(yearMap, pickedYear); //Variable que recibe el par de un año.

        if(foundYear != NULL){ //Si existen películas para dicho año, se muestran una tras una.
            system("cls");
            gotoxy(30,1);    
            printf("The movies released within this year are:\n\n");
            movieCategory* dataInYear = foundYear->value; //Para acceder a los datos de la categoría.
            movieType* movieData = firstList(dataInYear->movie_list); //Para acceder a la lista de películas de dicha categoría.

            while(movieData != NULL){ //Se imprimen las películas lanzadas en el año insertado.
                printf("%s\n", movieData->movieName);
                movieData = nextList(dataInYear->movie_list); 
            }

            printf("\nPress any button to return to the main menu.");
        }else{ //Si no existen películas para ese año, se avisa por pantalla.
          system("cls");      
          gotoxy(30,4);
          printf("We are sorry, there are no movies released within this year\n");
          gotoxy(30,6);
          printf("Press any button to return to the main menu.");
        }
        getch(); //Terminado el proceso, se regresa al menú principal al presionar cualquier botón.
}

void movieCriteria(userType* user_Info, char* user){ //Función que ayuda seleccionar criterio de películas.
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
                case 1: system("cls"); //Por genero.
                        showMoviesGenre(user_Info->moviesByGenre, user);
                        break;

                case 2: system("cls"); //Por año.
                        showMoviesInYear(user_Info->yearOrder, user);
                        break;

                case 3: system("cls"); //Por Rating. (Menor a mayor)
                        showMoviesThreeFiveSix(user_Info->ratingOrder, user);
                        break;

                case 4: system("cls"); //Por Titulo. (A-Z)
                        showMoviesTitle(user_Info->abcOrder, user);
                        break;

                case 5: system("cls"); //Por duración. (Menor a mayor)
                        showMoviesThreeFiveSix(user_Info->runtimeOrder, user);
                        break;

                case 6: system("cls"); //Por fecha de lanzamiento. (Orden cronológico)
                        showMoviesThreeFiveSix(user_Info->yearOrder, user);
                        break;
        }
}

void userChoice(HashMap* allUsers, char* userName, userType* loggedUserInfo){ //Función encargada de enviar un usuario al menú de categorías.
        int option; //Variable para elección de opción.
        char* user = (char*) malloc(100*sizeof(char)); //Recibe un usuario insertado.
        Par* foundUser; //Variable que recibirá el par de un usuario hallado en opción 2.
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
                case 1: system("cls"); //Se envían los datos del usuario que inició sesión al menú de categorías.
                        movieCriteria(loggedUserInfo, userName);
                        break;

                case 2: system("cls");
                        gotoxy(30,4);
                        printf("Please enter the user you're looking for: "); //Se pide el nombre de un usuario.
                        scanf("%s", user);
                        system("cls");
                        gotoxy(30,4);
                        printf("Searching for your user...");
                        Sleep(3000);
                        foundUser = searchMap(allUsers, user); 
                        if(foundUser != NULL){ //Si el usuario es hallado, se avisa por pantalla y sus datos son enviados al menú de categorías.
                             gotoxy(30,5);
                             printf("The user has been found!");
                             Sleep(2000);   
                             userType* userInfo = foundUser->value; //Variable para acceder a los mapas de películas del usuario hallado.
                             movieCriteria(userInfo, user);

                        }else{ //En caso contrario, se avisa por pantalla y se retorna al menú principal.
                           gotoxy(30,5);
                           printf("The user has not been found.");
                           gotoxy(30,7);
                           printf("Press any button to return to the main menu.");
                           getch();
                        }
                        break;
        }
}

//Función para mostrar el perfil de un usuario
void showProfile(userType* user, Mheap* genres, Mheap* favorites)
{
        Par* test = firstMap(user->movieMap);
        if (test == NULL)
        {
                gotoxy(30,4);
                printf("%s haven't any movies yet!\n");
                gotoxy(30,5);
                printf("Press any key to return");
                getch();
        }
        gotoxy(35, 1);
        printf("User: %s\n", user->user_id);
        gotoxy(29, 2);
        printf("Movies Watched: %ld\n", user->movieNumber);
        gotoxy(28, 3);
        printf("Favorite Movies: ");
        int k = 4; //contador de pixeles verticales para imprimir de mejor manera cada valor con salto de línea
        for (int i = 0; i < 5; i++)
        {
                //Aquí se muestran las 5 películas con mejor rating
                movieType* favorite = heap_top(favorites); //Se saca la primera película del montículo
                heap_pop(favorites); //Se elimina la película
                printf ("%s (%i)\n", favorite->movieName, *favorite->year); //Se imprime en pantalla su nombre y año
                gotoxy(45, k);
                k++;
        }
        gotoxy(22, 8);
        printf("Least Favorite Movies: ");
        int i = 0;
        k = 9;
        Pair* data = firstTreeMap(user->ratingOrder); //dato auxiliar para recorrer el mapa
        while (data != NULL)
        {
                movieCategory* worse = data->value;
                movieType* movie = firstList(worse->movie_list);
                while (movie != NULL)
                {
                        //Aquí se muestran las 5 películas con peor rating
                        if (i >= 5) break;       
                        printf("%s (%i)\n", movie->movieName, *movie->year);
                        gotoxy(45, k);
                        i++;
                        k++;
                        movie = nextList(worse->movie_list);
                }
                data = nextTreeMap(user->ratingOrder);
        }
        gotoxy(18, 13);
        printf("%s's most viewed genres: ", user->user_id);
        k = 14;
        for (int i = 0; i < 5; i++)
        {
                //Mientras que aquí se muestran los 5 géneros más vistos por el usuario
                movieCategory* genre = heap_top(genres);
                heap_pop(genres);
                printf("%s (%.0lf movies)\n", genre->name, genre->numberOfMovies);
                gotoxy(45, k);
                k++;
        }
        gotoxy(30, 19);
        printf("Press any key to return\n");
        getch();
}

/* La razón para el uso de montículo en las siguientes dos funciones es para tener un nuevo orden para cada mapa, sin recurrir a
métodos más lentos como un arreglo ordenado o lista ordenada  */

//Función que muestra el perfil del usuario que inició sesión
void loggedProfile(userType* user)
{
        system("cls");

        //Se ordenan los generos de más a menos vistos

        Mheap* genreOrder = createMheap(); 
        Par* genreData = firstMap(user->moviesByGenre);
        while (genreData != NULL)
        {
                movieCategory* genre = genreData->value;
                heap_push(genreOrder, genre, genre->numberOfMovies);
                genreData = nextMap(user->moviesByGenre);
        }

        //Se ordenan las películas de mejor a peor

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

        showProfile(user, genreOrder, ratingOrder);
}

//Función que muestra el perfil de un usuario que se busca en el mapa
void userProfile(HashMap* usersMap)
{
        char* username = (char*) malloc (100*sizeof(char));

        system("cls");
        gotoxy(30, 4);
        printf("Enter the name of another user: ");
        scanf("%s", username);
        getchar();

        Par* data = searchMap(usersMap, username); //si el usuario no se encuentra, se retorna al menú principal
        if (data == NULL)
        {
                gotoxy(30, 6);
                printf("There seems to be no user with this name...");
                gotoxy(30,7);
                printf("Press any key to return to the main menu");
                getch();
                return;
        }

        system("cls");

        //Se ordenan los géneros

        userType* user = data->value;
        Mheap* genreOrder = createMheap();
        Par* genreData = firstMap(user->moviesByGenre);
        while (genreData != NULL)
        {
                movieCategory* genre = genreData->value;
                heap_push(genreOrder, genre, genre->numberOfMovies);
                genreData = nextMap(user->moviesByGenre);
        }

        //Se ordenan las películas de mejores a peores

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

        showProfile(user, genreOrder, ratingOrder);
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
                        printf("Please enter the ID of the movie you are looking for: "); //Se pide al usuario que inserte el ID de una película.
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
                        userDiscovery(usersMap, loggedUser);
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