#ifndef L_CLIENT_H_     // Guard against being added multiple times
#define L_CLIENT_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "laccount.h"

struct Cliente {
    int id;
    char nome[30];
    char cpf[15];
    char phone[15];
    char addr[90];
};

int menuMain(FILE *dbClient, FILE *dbAccount, FILE *dbTransaction);
void menuClient(FILE *dbClient, FILE *dbAccount);
void addClient(FILE *dbClient, long position);
void listClient(FILE *dbClient);
bool checkClient(int id, char *cpf, FILE *dbClient);
int cleanExit(FILE *dbClient, FILE *dbAccount, int a);
int stringComp(const void *s1, const void *s2);
void sortClient(FILE *dbClient);
int searchClient(FILE *dbClient);
long positionClient(FILE *dbClient, int id);
void zeroClient(FILE *dbClient, long position);
void removeClient(FILE *dbClient, long position);

#endif
