#include "laccount.h"
#include "lclient.h"

void addAccount(FILE *dbAccount, int idClient)
{
    struct Account accountLi;
    int id;
    char bankNum[5];
    char accountNum[11];
    size_t length = 0;
    bool isNumber = false;

    // Get index from database
    FILE *idAcc;
    idAcc = fopen("indexAcc.db", "r+b");
    if (idAcc == NULL) {
        id = 0;
        idAcc = fopen("indexAcc.db", "w+b");
        fwrite(&id, sizeof(int), 1, idAcc);
    }
    rewind(idAcc);
    fread(&id, sizeof(int), 1, idAcc);


    while (!isNumber) {
        printf("Informe a agência: ");
        fgets(bankNum, 5, stdin);
        length = strlen(bankNum);
        if (bankNum[length-1] == '\n')
            bankNum[length-1] = 0;
        else
            while (getchar() != '\n');

        for (size_t i = 0; i < length - 1; ++i) {
            if (!isdigit(bankNum[i])) {
                printf("Agência deve conter apenas números!\n");
                isNumber = false;
                break;
            }
            isNumber = true;
        }
    }

    isNumber = false;
    while (!isNumber) {
        printf("Informe o número da conta: ");
        fgets(accountNum, 11, stdin);
        length = strlen(accountNum);
        if (accountNum[length-1] == '\n')
            accountNum[length-1] = 0;
        else
            while (getchar() != '\n');

        for (size_t i = 0; i < length - 1; ++i) {
            if (!isdigit(accountNum[i])) {
                printf("Conta deve conter apenas números!\n");
                isNumber = false;
                break;
            }
            isNumber = true;
        }
    }

    if (checkAccount(bankNum, accountNum, dbAccount)) {

        accountLi.id = id;
        accountLi.idClient = idClient;
        strcpy(accountLi.bankNum, bankNum);
        strcpy(accountLi.accountNum, accountNum);
        accountLi.balance = rand() % 1000;

        printf("\nConta adicionada com sucesso!\n"
                "Cliente: %d\n"
                "Agência: %s\n"
                "Conta:   %s\n"
                "Saldo:   %lld\n", accountLi.idClient, accountLi.bankNum,
                accountLi.accountNum, accountLi.balance);

        ++id;
        fseek(idAcc, 0, SEEK_SET);
        fwrite(&id, sizeof(int), 1, idAcc);
        fclose(idAcc);

        fseek(dbAccount, 0, SEEK_END);
        fwrite(&accountLi, sizeof(struct Account), 1, dbAccount);
    } else {
        printf("\nConta já existente.\nA combinação de agência com conta "
                                                        "deve ser única.\n");
    }
}


// Make sure there is no repeated Bank number with Account number.
// Returns true if not repeated and false otherwise.
bool checkAccount(char *bankNum, char *accountNum, FILE *dbAccount)
{
    struct Account accountLi;
    rewind(dbAccount);
    while (fread(&accountLi, sizeof(struct Account), 1, dbAccount)) {
        if (strcmp(accountLi.bankNum, bankNum) == 0)
            return false;
        else if (strcmp(accountLi.accountNum, accountNum) == 0)
            return false;
    }
    return true;
}

/*
int longComp(const void *p1, const void *p2)
{
    struct Account *v1 = (struct Account *)p1;
    struct Account *v2 = (struct Account *)p2;
    return (int)(v1->balance - v2->balance);
}*/
int longComp(const void *p1, const void *p2)
{
    long long v1 = ((struct Account *)p1) -> balance;
    long long v2 = ((struct Account *)p2) -> balance;

    if (v1 < v2)
        return -1;
    else if (v1 == v2)
        return 0;
    else
        return 1;
}


void sortAccount(FILE *dbAccount)
{
    struct Account accountLi;
    rewind (dbAccount);

    // Get how many elements in the file
    size_t qtyAccount = 0;
    while (fread(&accountLi, sizeof(struct Account), 1, dbAccount) == 1)
        ++qtyAccount;

    // Store everything in an array
    rewind(dbAccount);
    struct Account sortAcc[qtyAccount];
    for (size_t i = 0; i < qtyAccount; ++i)
        fread(&sortAcc[i], sizeof(struct Account), 1, dbAccount);

    // Sort
    qsort(sortAcc, qtyAccount, sizeof(struct Account), longComp);
    for (size_t i = 0; i < qtyAccount; ++i)

    // Overwrite file with sorted accounts
    fseek(dbAccount, 0, SEEK_SET);
    for (size_t i = 0; i < qtyAccount; ++i)
        fwrite(&sortAcc[i], sizeof(struct Account), 1, dbAccount);
}


void listAllAcc(FILE *dbAccount, FILE *dbClient)
{
    struct Cliente clienteLi;
    struct Account accountLi;
    int noAccount = 0;

    rewind(dbClient);
    while (fread(&clienteLi, sizeof(struct Cliente), 1, dbClient)) {
        noAccount = 0;
        printf("\n\n"
               "Código:   %d\n"
               "Nome:     %s\n"
               "CPF/CNPJ: %s\n"
               "Telefone: %s\n"
               "Endereço: %s\n"
               "Contas:\n",
               clienteLi.id, clienteLi.nome, clienteLi.cpf,
               clienteLi.phone, clienteLi.addr);

        rewind(dbAccount);
        while (fread(&accountLi, sizeof(struct Account), 1, dbAccount)) {
            if (clienteLi.id == accountLi.idClient) {
                printf("\n"
                       "    Agência: %s\n"
                       "    Conta:   %s\n"
                       "    Saldo:   %lld\n", accountLi.bankNum,
                        accountLi.accountNum, accountLi.balance);
                ++noAccount;
            }
        }
        if (noAccount == 0)
            printf("\n    Nenhuma conta cadastrada.\n");
    }
}
