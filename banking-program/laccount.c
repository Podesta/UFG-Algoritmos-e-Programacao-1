#include "laccount.h"
#include "lclient.h"

void menuConta(FILE *dbCli, FILE *dbAcc, FILE *dbTra)
{
    int input;
    int id;
    int idAcc;
    int idRec;  // If a transfer, the receiving account
    int idCli;
    long long value;

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
            sortClient(dbCli);
            sortAccount(dbAcc);
            listAccount(dbAcc, dbCli, true, -1);
            break;
        case 'c':
            id = searchClient(dbCli);
            if (id != 0) {
                printf("\n");
                addAccount(dbAcc, id);
            }
            break;
        case 'l':
            sortClient(dbCli);
            sortAccount(dbAcc);
            id = searchClient(dbCli);
            if (id != 0)
                listAccount(dbAcc, dbCli, false, id);
            break;
        case 'w':
            if (searchAccount(dbAcc, &idAcc, &idCli)) {
                printClient(dbCli, idCli);
                printAccount(dbAcc, idAcc);
                idRec = 0; // TODO:REMOVE
                value = withdraw(dbAcc, dbTra, idAcc, idRec, false);
                if (value > 0) {
                    printNotes(value);
                }
            } else {
                printf("\nConta não encontrada.\n");
            }
            break;
        case 's':
            cleanExit(dbCli, dbAcc, 0);
            break;
        default:
            break;
    }
}

void addAccount(FILE *dbAcc, int idClient)
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

    if (checkAccount(bankNum, accountNum, dbAcc)) {

        accountLi.id = id;
        accountLi.idClient = idClient;
        strcpy(accountLi.bankNum, bankNum);
        strcpy(accountLi.accountNum, accountNum);
        accountLi.balance = rand() % 1000000;

        printf("\nConta adicionada com sucesso!\n"
                "Cliente: %d\n"
                "Agência: %s\n"
                "Conta:   %s\n"
                "Saldo:   %.2lf\n", accountLi.idClient, accountLi.bankNum,
                accountLi.accountNum, (double)accountLi.balance / 100);

        ++id;
        fseek(idAcc, 0, SEEK_SET);
        fwrite(&id, sizeof(int), 1, idAcc);
        fclose(idAcc);

        fseek(dbAcc, 0, SEEK_END);
        fwrite(&accountLi, sizeof(struct Account), 1, dbAcc);
    } else {
        printf("\nConta já existente.\nA combinação de agência com conta "
                                                        "deve ser única.\n");
    }
}

// Make sure there is no repeated Bank number with Account number.
// Returns true if not repeated and false otherwise.
bool checkAccount(char *bankNum, char *accountNum, FILE *dbAcc)
{
    struct Account accountLi;
    rewind(dbAcc);
    while (fread(&accountLi, sizeof(struct Account), 1, dbAcc)) {
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
        return 1;
    else if (v1 == v2)
        return 0;
    else
        return -1;
}

void sortAccount(FILE *dbAcc)
{
    struct Account accountLi;
    rewind (dbAcc);

    // Get how many elements in the file
    size_t qtyAccount = 0;
    while (fread(&accountLi, sizeof(struct Account), 1, dbAcc) == 1)
        ++qtyAccount;

    // Store everything in an array
    rewind(dbAcc);
    struct Account sortAcc[qtyAccount];
    for (size_t i = 0; i < qtyAccount; ++i)
        fread(&sortAcc[i], sizeof(struct Account), 1, dbAcc);

    // Sort
    qsort(sortAcc, qtyAccount, sizeof(struct Account), longComp);
    for (size_t i = 0; i < qtyAccount; ++i)

    // Overwrite file with sorted accounts
    fseek(dbAcc, 0, SEEK_SET);
    for (size_t i = 0; i < qtyAccount; ++i)
        fwrite(&sortAcc[i], sizeof(struct Account), 1, dbAcc);
}

// If printAll is true, print all accounts, and ignore id
// If printAll is false, print only when it matches id
void listAccount(FILE *dbAcc, FILE *dbCli, bool printAll, int id)
{
    struct Cliente clienteLi;
    struct Account accountLi;
    int noAccount = 0;

    rewind(dbCli);
    while (fread(&clienteLi, sizeof(struct Cliente), 1, dbCli)) {
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

            rewind(dbAcc);
            while (fread(&accountLi, sizeof(struct Account), 1, dbAcc)) {
                if (clienteLi.id == accountLi.idClient) {
                    printf("\n"
                            "    Agência: %s\n"
                            "    Conta:   %s\n"
                            "    Saldo:   %.2lf\n", accountLi.bankNum,
                        accountLi.accountNum, (double)accountLi.balance / 100);
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
bool searchAccount(FILE *dbAcc, int *idAcc, int *idCli)
{
    struct Account accountLi;

    char combinedNum[16];
    char bankNum[5];
    char accountNum[11];
    char *tmpCheck;     // Make sure I don't get a segmentation fault
    size_t length;
    bool digitOrHy;

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
                digitOrHy = false;
                break;
            } else {
                digitOrHy = true;
            }
        }
        if (!digitOrHy) {
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

    rewind(dbAcc);
    while (fread(&accountLi, sizeof(struct Account), 1, dbAcc) == 1) {
        if ((strcmp(accountLi.bankNum, bankNum) == 0) &&
            (strcmp(accountLi.accountNum, accountNum) == 0)) {
            *idAcc = accountLi.id;
            *idCli = accountLi.idClient;
            return true;
        }
    }

    return false;
}

void printClient(FILE *dbCli, int idCli)
{
    struct Cliente clienteLi;

    rewind(dbCli);
    while (fread(&clienteLi, sizeof(struct Cliente), 1, dbCli)) {
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

void printAccount(FILE *dbAcc, int idAcc)
{
    struct Account accountLi;

    rewind(dbAcc);
    while (fread(&accountLi, sizeof(struct Account), 1, dbAcc)) {
        if (accountLi.id == idAcc) {
            printf("\n"
                   "    Agência: %s\n"
                   "    Conta:   %s\n"
                   "    Saldo:   %.2lf\n", accountLi.bankNum,
                   accountLi.accountNum, (double)accountLi.balance / 100);
            return;
        }
    }
    printf("\n\nERRO AO PROCURAR CONTA!\n\n");
}

long long withdraw(FILE *dbAcc, FILE *dbTra, int idAcc, int idRec, bool trsfr)
{
    struct Account accountLi;
    struct Account rcvAcc;  // Receiving account, if trasnfer.
    struct Transaction traLi;
    long position;
    double tmp;
    long long amount;
    char description[150];
    int idTra;  // Transaction id. Auto generated.
    time_t date;

    rewind(dbAcc);
    while (fread(&accountLi, sizeof(struct Account), 1, dbAcc)) {
        if (accountLi.id == idAcc) {
            position = (ftell(dbAcc) - (long int)sizeof(struct Account));
            break;
        }
    }

    while (true) {
        trsfr ?
            printf("Valor a ser transferido: R$ ") :
            printf("Valor a ser sacado: R$ ");

        scanf("%lf", &tmp);

        if (tmp <= 0) {
            printf("Etrada inválida. Digite um valor positivo em reais.\n");
            continue;
        }

        // Store in cents, and don't deal with float values;
        amount = (long long)(tmp * 100);

        // If withdrawing, do not accept coins. Only accept multiples of 2 or 5
        if ((!trsfr) && ((amount % 100 != 0) ||
                    (amount % 200 != 0 && amount % 500 != 0))) {
            printf("Caixa contém somente notas de 2, 5, 10, 20, 50, 100 e 200 "
                    "reais. Entre um valor apropriado.\n");
            continue;
        }
        break;
    }
    while (getchar() != '\n');

    if (amount > accountLi.balance) {
        printf("Saldo insuficiente.\n");
        return 0;
    }

    // Get Transaction id.
    FILE *indexTra;
    indexTra = fopen("indexTra.db", "r+b");
    if (indexTra == NULL) {
        indexTra = fopen("indexTra.db", "w+b");
        idTra = 0;
        fwrite(&idTra, sizeof(int), 1, indexTra);
    }
    rewind(indexTra);
    fread(&idTra, sizeof(int), 1, indexTra);

    // If transfer is true, get account info of the other account and set
    // description as per specification.
    if (trsfr) {
        rewind(dbAcc);
        while (fread(&rcvAcc, sizeof(struct Account), 1, dbAcc)) {
            if (rcvAcc.id == idRec)
                break;
        }

        snprintf(description, sizeof(description),
                 "Transferência de R$%.2lf para conta: %s-%s.",
                 ((double)amount / 100), rcvAcc.bankNum, rcvAcc.accountNum);
        time(&date);
        ++idTra;

        traLi.id = idTra;
        traLi.idAccount = idAcc;
        traLi.credit = false;
        traLi.debit = true;
        traLi.amount = amount;
        traLi.date = date;
        strcpy(traLi.description, description);

        accountLi.balance -= amount;

        fseek(dbTra, 0, SEEK_END);
        fwrite(&traLi, sizeof(struct Transaction), 1, dbTra);

        fseek(dbAcc, position, SEEK_SET);
        fwrite(&accountLi, sizeof(struct Account), 1, dbAcc);

        fseek(indexTra, 0, SEEK_SET);
        fwrite(&idTra, sizeof(int), 1, indexTra);
        fclose(indexTra);

        printf("\nTransferência realizada com sucesso!\n"
               "Novo saldo: R$ %.2lf.\n", (double)accountLi.balance / 100);

        return amount;
    }

    // Redundant. Here to make it clear that it is for basic withdraw.
    if (!trsfr) {
        printf("Informe a descrição para o saque: ");
        fgets(description, 150, stdin);
        size_t length = strlen(description);
        if (description[length - 1] == '\n')
            description[length - 1] = 0;
        else
            while (getchar() != '\n');

        time(&date);
        ++idTra;

        traLi.id = idTra;
        traLi.idAccount = idAcc;
        traLi.credit = false;
        traLi.debit = true;
        traLi.amount = amount;
        traLi.date = date;
        strcpy(traLi.description, description);

        accountLi.balance -= amount;

        fseek(dbTra, 0, SEEK_END);
        fwrite(&traLi, sizeof(struct Transaction), 1, dbTra);

        fseek(dbAcc, position, SEEK_SET);
        fwrite(&accountLi, sizeof(struct Account), 1, dbAcc);

        fseek(indexTra, 0, SEEK_SET);
        fwrite(&idTra, sizeof(int), 1, indexTra);
        fclose(indexTra);

        printf("\nSaque realizado com sucesso!\n"
               "Novo saldo: R$ %.2lf.\n", (double)accountLi.balance / 100);

        return amount;
    }
    printf("\n\nERRO AO SACAR DINHEIRO!\n\n");
    return 0;
}

void printNotes(long long value)
{
    long long n200;
    long long n100;
    long long n50;
    long long n20;
    long long n10;
    long long n5;
    long long n2;

    value = value / 100;

    n200 = value / 200;
    value = value % 200;

    n100 = value / 100;
    value = value % 100;

    n50 = value / 50;
    value = value % 50;

    n20 = value / 20;
    value = value % 20;

    n10 = value / 10;
    value = value % 10;

    n5 = value / 5;
    value = value % 5;

    n2 = value / 2;
    value = value % 2;

    if (value != 0)
        printf("\n\nERRO CONTANDO AS NOTAS!\n\n");

    printf("Notas emitidas:\n");

    if (n200 !=0)
        printf("%lld notas de 200 reais.\n", n200);
    if (n100 !=0)
        printf("%lld notas de 100 reais.\n", n100);
    if (n50 != 0)
        printf("%lld notas de 50 reais.\n", n50);
    if (n20 != 0)
        printf("%lld notas de 20 reais.\n", n20);
    if (n10 != 0)
        printf("%lld notas de 10 reais.\n", n10);
    if (n5 !=0)
        printf("%lld notas de 5 reais.\n", n5);
    if (n2 != 0)
        printf("%lld notas de 2 reais.\n", n2);
}





