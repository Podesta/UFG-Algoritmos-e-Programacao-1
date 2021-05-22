#ifndef L_ACCOUNT_H_
#define L_ACCOUNT_H_

#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

void menuConta(FILE *dbClient, FILE *dbAccount, FILE *dbTransaction);
void addAccount(FILE *dbAccount, int idClient);
bool checkAccount(char *bankNum, char *accountNum, FILE *dbAccount);
int longComp(const void *p1, const void *p2);
void sortAccount(FILE *dbAccount);
void listAccount(FILE *dbAccount, FILE *dbClient, bool printAll, int id);
bool searchAccount(FILE *dbAccount, int *idAcc, int *idCli);
void printClient(FILE *dbClient, int idCli);
void printAccount(FILE *dbAccount, int idAcc);
//long long withdraw(FILE *db

struct Account {
    int id;
    int idClient;
    char bankNum[5];
    char accountNum[11];
    long long balance;
};

struct Transaction {
    int id;
    int idAccount;
    bool credit;
    bool debit;
    long long amount;
    time_t date;
    char description[150];
};

#endif
