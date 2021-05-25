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

int menuMain(FILE *dbCli, FILE *dbAcc, FILE *dbTra);
void menuClient(FILE *dbCli, FILE *dbAcc);
void addClient(FILE *dbCli, long position);
void listClient(FILE *dbCli);
bool checkClient(int id, char *cpf, FILE *dbCli);
int cleanExit(FILE *dbCli, FILE *dbAcc, int a);
int stringComp(const void *s1, const void *s2);
void sortClient(FILE *dbCli);
int searchClient(FILE *dbCli);
long positionClient(FILE *dbCli, int id);
void zeroClient(FILE *dbCli, long position);
void removeClient(FILE *dbCli, FILE *dbAcc, long position, int id);

#endif
