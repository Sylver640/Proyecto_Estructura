#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#include "list.h"
#include "datatypes.h"

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

int fileExists(char* filename)
{
    FILE* f;
    if (f = fopen(filename, "r"))
    {
        fclose(f);
        return 1;
    }
    return 0;
}

/*int isBlank(const char* buffer)
{
    char character;

    do
    {
        character = *(buffer)++;

        if (character != ' ' && character != '\t' && character != '\n' && character != '\r' && character != '\0')
            return 0;

    } while (character != '\0');

    return 1;
    
}

void removeBlankLines(FILE* original, char* path)
{
    printf("entra\n");
    getch();
    original = fopen(path, "rt");
    if (original != NULL) printf("se abre archivo\n");
    getch();
    FILE* temp = fopen("temp.csv", "wt");
    if (temp != NULL) printf("se abre temporal\n");
    getch();

    char buffer[1024];
    while ((fgets(buffer, 1024, original)) != NULL)
    {
        if (!isBlank(buffer));
            fputs(buffer, temp);
    }

    fclose(original);
    fclose(temp);
    
    remove(path);
    rename("temp.csv", path);
}*/