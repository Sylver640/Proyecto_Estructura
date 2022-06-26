#ifndef INITIALIZE_H
#define INITIALIZE_H

typedef struct movieType movieType;
typedef struct userType userType;

int is_equal_int(void * key1, void * key2);
int is_equal_string(void * key1, void * key2);
int lower_than_string(void * key1, void * key2);
int lower_than_int(void * key1, void * key2);
userType* createUser(char* username);
movieType* createMovie();

#endif