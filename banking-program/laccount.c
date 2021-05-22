#include "laccount.h"
#include "lclient.h"

void menuConta(FILE *dbClient, FILE *dbAccount, FILE *dbTransaction)
{
    int input;
    int id;
    int idAcc;
    int idCli;

    printf("\n============= Gerenciar Contas =============\n"
           "Digite um comando para prosseguir:\n"
           "R – Listagem de todas as contas cadastradas.\n"
           "C – Cadastrar uma conta para um cliente.\n"
           "L – Listar todas as contas de um cliente.\n"
           "W – Realizar um saque em uma conta.\n"
           "D – Realizar um depósito em uma conta.\n"
           "T – Realizar transferência entre contas.\n"
           "E – Exibir extrato de uma conta.\n"
           "V - Voltar\n"
           "S – Sair\n");

    do {
        // Get first char and discards everything else, including newline
        input = getchar();
        while (getchar() != '\n');

        // Accept both uppercase and lowercase. Transform to lowercase
        if (input >= 'A' && input <= 'Z')
            input += 'a' - 'A';

    } while ((input != 'r') && (input != 'c') && (input != 'l') && 
             (input != 'w') && (input != 'd') && (input != 't') &&
             (input != 'e') && (input != 'v') && (input != 's'));

    switch (input) {
        case 'r':
            sortClient(dbClient);
            sortAccount(dbAccount);
            listAccount(dbAccount, dbClient, true, -1);
            break;
        case 'c':
            id = searchClient(dbClient);
            if (id != 0) {
                printf("\n");
                addAccount(dbAccount, id);
            }
            break;
        case 'l':
            sortClient(dbClient);
            sortAccount(dbAccount);
            id = searchClient(dbClient);
            if (id != 0)
                listAccount(dbAccount, dbClient, false, id);
            break;
        case 'w':
            if (searchAccount(dbAccount, &idAcc, &idCli)) {
                printClient(dbClient, idCli);
                printAccount(dbAccount, idAcc);

            } else {
                printf("\nConta não encontrada.\n");
            }
            break;
        case 's':
            cleanExit(dbClient, dbAccount, 0);
            break;
        default:
            break;
    }
}

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

// If printAll is true, print all accounts, and ignore id
// If printAll is false, print only when it matches id
void listAccount(FILE *dbAccount, FILE *dbClient, bool printAll, int id)
{
    struct Cliente clienteLi;
    struct Account accountLi;
    int noAccount = 0;

    rewind(dbClient);
    while (fread(&clienteLi, sizeof(struct Cliente), 1, dbClient)) {
        if (printAll ? true : clienteLi.id == id) {
            noAccount = 0;

            // When not printing all, the search function already returns the
            // client information, so no need to repeat it, only add the line
            // Contas. If printing all, do add the account information.
            printAll ? printf("\n\n"
                              "Código:   %d\n"
                              "Nome:     %s\n"
                              "CPF/CNPJ: %s\n"
                              "Telefone: %s\n"
                              "Endereço: %s\n"
                              "Contas:\n",
                              clienteLi.id, clienteLi.nome, clienteLi.cpf,
                              clienteLi.phone, clienteLi.addr)

                     : printf("Contas:\n");

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
}

// Pointers because I need to return two values, both the Client id and the
// Account id itself.
bool searchAccount(FILE *dbAccount, int *idAcc, int *idCli)
{
    struct Account accountLi;

    char combinedNum[16];
    char bankNum[5];
    char accountNum[11];
    char *tmpCheck;     // Make sure I don't get a segmentation fault
    size_t length;
    bool digitOrHi;

    printf("Informe o número da agência e conta separados por um hífen: ");

    // Validate input. When input is invalid, continue to next iteration.
    // When input is valid, break out of loop.
    while (true) {
        fgets(combinedNum, 16, stdin);

        length = strlen(combinedNum);
        if (length < 4) {
            printf("Entrada inválida. Entre os dados da maneira correta.\n");
            continue;
        }
        if (combinedNum[length - 1] == '\n')
            combinedNum[length - 1] = 0;
        else
            while (getchar() != '\n');

        // Only accepts digits or hyphens
        length = strlen(combinedNum);
        for (size_t i = 0; i < length; ++i) {
            if (!(isdigit(combinedNum[i]) || combinedNum[i] == '-')) {
                digitOrHi = false;
                break;
            } else {
                digitOrHi = true;
            }
        }
        if (!digitOrHi) {
            printf("Entrada inválida. Entre os dados da maneira correta.\n");
            continue;
        }

        // Make sure both strings are within individual limits;
        tmpCheck = strtok(combinedNum, "-");
        if ((tmpCheck == NULL || strlen(tmpCheck) > 4)) {
            printf("Entrada inválida. Entre os dados da maneira correta.\n");
            continue;
        }
        strcpy(bankNum, tmpCheck);

        tmpCheck = strtok(NULL, "-");
        if ((tmpCheck == NULL || strlen(tmpCheck) > 10)) {
            printf("Entrada inválida. Entre os dados da maneira correta.\n");
            continue;
        }
        strcpy(accountNum, tmpCheck);

        // There should be only one delimiter
        tmpCheck = strtok(NULL, "-");
        if (tmpCheck != NULL) {
            printf("Entrada inválida. Entre os dados da maneira correta.\n");
            continue;
        }
        break;
    }

    rewind(dbAccount);
    while (fread(&accountLi, sizeof(struct Account), 1, dbAccount) == 1) {
        if ((strcmp(accountLi.bankNum, bankNum) == 0) &&
            (strcmp(accountLi.accountNum, accountNum) == 0)) {
            *idAcc = accountLi.id;
            *idCli = accountLi.idClient;
            return true;
        }
    }

    return false;
}

void printClient(FILE *dbClient, int idCli)
{
    struct Cliente clienteLi;

    rewind(dbClient);
    while (fread(&clienteLi, sizeof(struct Cliente), 1, dbClient)) {
        if (clienteLi.id == idCli) {
            printf("\n"
                   "Código:   %d\n"
                   "Nome:     %s\n"
                   "CPF/CNPJ: %s\n"
                   "Telefone: %s\n"
                   "Endereço: %s\n",
                   clienteLi.id, clienteLi.nome, clienteLi.cpf,
                   clienteLi.phone, clienteLi.addr);
            return;
        }
    }
    printf("\n\nERRO AO PROCURAR CLIENTE!\n\n");
}

void printAccount(FILE *dbAccount, int idAcc)
{
    struct Account accountLi;

    rewind(dbAccount);
    while (fread(&accountLi, sizeof(struct Account), 1, dbAccount)) {
        if (accountLi.id == idAcc) {
            printf("\n"
                   "    Agência: %s\n"
                   "    Conta:   %s\n"
                   "    Saldo:   %lld\n", accountLi.bankNum,
                   accountLi.accountNum, accountLi.balance);
            return;
        }
    }
    printf("\n\nERRO AO PROCURAR CONTA!\n\n");
}
