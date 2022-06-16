#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <math.h>

#include "include/list.h"
#include "include/hashmap.h"
#include "include/treemap.h"

int main()
{
    //MENÚ PROVISORIO
    int option;

    while (option != 0)
    {
        printf("************Welcome************\n");
        printf("1. Add movies\n");
        printf("2. Search for a movie\n");
        printf("3. Show user's movies\n");
        printf("4. Movie Discovery\n");
        printf("5. Users Discovery\n");
        printf("6. Profiles\n");
        printf("0. Exit\n");
        printf("\n");
        printf("Press any option to continue...\n");
        scanf("%d", &option);    
        switch (option)
        {
                case 1: printf("NOT IMPLEMENTED YET\n");
                        getch();
                        break;
                case 2: printf("NOT IMPLEMENTED YET\n");
                        getch();
                        break;
                case 3: printf("NOT IMPLEMENTED YET\n");
                        getch();
                        break;
                case 4: printf("NOT IMPLEMENTED YET\n");
                        getch();
                        break;
                case 5: printf("NOT IMPLEMENTED YET\n");
                        getch();
                        break;
                case 6: printf("NOT IMPLEMENTED YET\n");
                        getch();
                        break;
                case 0: printf("See you later :)\n");
                        getch();
                        break;
        }
    }
    return 0;
}