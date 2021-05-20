#ifndef L_ACCOUNT_H_
#define L_ACCOUNT_H_

#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>

void addAccount(FILE *dbAccount, int idClient);
bool checkAccount(char *bankNum, char *accountNum, FILE *dbAccount);
int longComp(const void *p1, const void *p2);
void sortAccount(FILE *dbAccount);
void listAllAcc(FILE *dbAccount, FILE *dbClient);

struct Account {
    int id;
    int idClient;
    char bankNum[5];
    char accountNum[11];
    long long balance;
};

#endif
