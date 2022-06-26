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
#include "include/login.h"
#include "include/csv.h"
#include "include/datatypes.h"
#include "include/initialize.h"

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

//función para ver si una película ha sido vista por logged user
movieType* searchMovieUserList(userType* loggedUser, char* movie_id) //TO DO
{
        Par* searchResult = searchMap(loggedUser->movieMap, movie_id);
        if (searchResult != NULL)
        {
                movieType* foundMovie = searchResult->value;
                return foundMovie;
        } 
        return NULL;
}

void movieDiscovery (HashMap* usersMap, userType* loggedUser)
{
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
        movieType* currentMovie = firstList(firstMovieRating->movie_list);
        while (currentMovie != NULL)
        {
                //saltar peliculas ya vistas por el usuario
                if (searchMovieUserList(loggedUser, currentMovie->movie_id) == NULL)
                {
                        //película no ha sido vista, se recomienda a logged user
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
}

void searchByID(HashMap* allMovies, char* ID){
        system("cls");
        Par* foundMovie = searchMap(allMovies, ID);
        gotoxy(30,4);
        printf("Searching for the movie...");
        Sleep(3000);
        if(foundMovie != NULL){
            gotoxy(30,4);    
            printf("Your movie has been found!\n\n");    
            movieType* movieData = foundMovie->value;
            gotoxy(30,5);
            printf("Title: %s\n", movieData->movieName);
            gotoxy(30,6);
            printf("ID: %s\n", movieData->movie_id);
            gotoxy(30,7);
            printf("Year: %d\n", movieData->year);
            gotoxy(30,8);
            char* genre = firstList(movieData->genres);
            printf("Genres: ");
            if(genre != NULL){
                while(genre != NULL){
                  printf("%s, ", genre);
                  genre = nextList(movieData->genres);
                }
                printf(".");
            }
            printf("\n");
            gotoxy(30,9);
            printf("User Score: %d\n", movieData->userScore);
            gotoxy(30,10);
            printf("Runtime: %d minutes\n", movieData->runtime);

            gotoxy(30,12);
            printf("Press any button to return to the main menu.");

        }else{
          gotoxy(30,4);
          printf("We are sorry, your movie could not be found.\n");
          gotoxy(30,6);
          printf("Press any button to return to the main menu.");
        }
        getch();
}

void showMovies(HashMap* allUsers, char* userName, userType* loggedUserInfo){
        int option;
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
                        gotoxy(30,4);
                        printf("WORK IN PROGRESS!");
                        getch();
                        //showOwnMovies(loggedUserInfo, userName);
                        break;

                case 2: system("cls");
                        gotoxy(30,4);
                        printf("WORK IN PROGRESS!");
                        getch();
                        //showUserMovies(allUsers);
                        break;
        }


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
                        break;
                case 3: system("cls");
                        gotoxy(30,4);
                        showMovies(usersMap, loggedUserName, loggedUser);
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