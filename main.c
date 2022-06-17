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
        int movie_id;
        int year;
        List* genres;
        int userScore;
        int runtime;       
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

void login (userType* user, char* username)
{
        char path[100];
        system("cls");
        logo();

        gotoxy(25, 5);
        printf("Enter your username: ");
        scanf("%100[^\n]s", username);

        //snprintf(path, sizeof(path), "users/%s.csv", username);
        //FILE* f = fopen(path, "rt");
        /*if (f == NULL)
        {
                printf("This user doesn't exist!\nCreating it now...\n");
                getch();
                f = fopen(path, "wt");
        }*/
}

int main()
{
    int option;
    char* loggedUserName = (char*) malloc (100*sizeof(char));
    userType* loggedUser = (userType*) malloc (sizeof(loggedUser));
    login(loggedUser, loggedUserName);
    getchar();
    //MENÚ PROVISORIO
    while (option != 0)
    {
        system("cls");
        gotoxy(30, 1);
        printf("WELCOME, %s\n", loggedUser);
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
                        break;
        }
    }
    return 0;
}