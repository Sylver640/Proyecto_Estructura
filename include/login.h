#ifndef LOGIN_H
#define LOGIN_H

typedef struct movieType movieType;
typedef struct userType userType;
typedef struct HashMap HashMap;

void login (char* username, userType* loggedUser, HashMap* globalMovieMap, HashMap* usersMap);

void addOtherUsers(char* ignored_user, HashMap* usersMap, HashMap* globalMovieMap);

#endif