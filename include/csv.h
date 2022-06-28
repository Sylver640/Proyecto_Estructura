#ifndef CSV_H
#define CSV_H

typedef struct movieType movieType;

char *get_csv_field (char * tmp, int k);
void split_and_AddGenres(char* genres, movieType* movie);
int fileExists(char* filename);
void removeBlankLines(FILE* original, char* path);
int isBlank(char* str);

#endif