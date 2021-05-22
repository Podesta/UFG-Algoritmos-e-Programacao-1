#ifndef L_ACCOUNT_H_
#define L_ACCOUNT_H_

#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

void menuConta(FILE *dbCli, FILE *dbAcc, FILE *dbTra);
void addAccount(FILE *dbAcc, int idClient);
bool checkAccount(char *bankNum, char *accountNum, FILE *dbAcc);
int longComp(const void *p1, const void *p2);
void sortAccount(FILE *dbAcc);
void listAccount(FILE *dbAcc, FILE *dbCli, bool printAll, int id);
bool searchAccount(FILE *dbAcc, int *idAcc, int *idCli);
void printClient(FILE *dbCli, int idCli);
void printAccount(FILE *dbAcc, int idAcc);
long long withdraw(FILE *dbAcc, FILE *dbTra, int idAcc, bool transfer);

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
