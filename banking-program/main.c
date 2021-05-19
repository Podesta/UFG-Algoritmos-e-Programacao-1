#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <ctype.h>

int menuMain(FILE *dbClient, FILE *dbAccount);
void menuClient(FILE *dbClient, FILE *dbAccount);
void menuConta(FILE *dbClient, FILE *dbAccount);
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

struct Cliente {
    int id;
    char nome[30];
    char cpf[15];
    char phone[15];
    char addr[90];
};

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


int main(int argc, char *argv[])
{

    struct Cliente clienteLi[10];
    memset(clienteLi, 0, sizeof(clienteLi));

    // User must provide one and only one database at runtime
    if (argc < 3 || argc > 3) {
        printf("Por favor, informe os bancos de dados ao iniciar o programa na "
               "seguinte ordem:\n 1 - Banco de dados do cliente\n 2 - Banco "
               "de dados das Contas.\n 3 - Banco de dados das Transações.\n");
        return 1;
    }
    
    FILE *dbClient;
    dbClient = fopen(argv[1], "r+b");
    if (dbClient == NULL)
        dbClient = fopen(argv[1], "w+b");

    FILE *dbAccount;
    dbAccount = fopen(argv[2], "r+b");
    if (dbAccount == NULL) {
        dbAccount = fopen(argv[2], "w+b");
    }


    int test = menuMain(dbClient, dbAccount);
    printf("%c\n", test);

    fclose(dbClient);
    fclose(dbAccount);

    return 0;
}


// Function to compare strings. To with qsort for alphabetical sorting.
// TODO: Fully understand how the compare function works. Dereference value.
int stringComp(const void *s1, const void *s2)
{
    return strcmp(((struct Cliente *)s1) -> nome,
                  ((struct Cliente *)s2) -> nome);
}


// Close database before exit. Exit already performs close,
// but step is required by instructors.
int cleanExit(FILE *dbClient, FILE *dbAccount, int a)
{
    fclose(dbClient);
    fclose(dbAccount);
    exit(a);
}


void sortClient(FILE *dbClient)
{
    struct Cliente clienteLi;
    rewind(dbClient);

    // Get how many elemets are in the struct
    size_t qtyClient = 0;
    while (fread(&clienteLi, sizeof(struct Cliente), 1, dbClient) == 1)
        ++qtyClient;

    // Store all of the structs into an array for use with qsort
    rewind(dbClient);
    struct Cliente sortCli[qtyClient];
    for (size_t i = 0; i < qtyClient; ++i)
        fread(&sortCli[i], sizeof(struct Cliente), 1, dbClient);

    qsort(sortCli, qtyClient, sizeof(struct Cliente), stringComp);

    // Overwrite new sorted database
    fseek(dbClient, 0, SEEK_SET);
    for (size_t i = 0; i < qtyClient; ++i)
        fwrite(&sortCli[i], sizeof(struct Cliente), 1, dbClient);
}


int menuMain(FILE *dbClient, FILE *dbAccount)
{
    int input;

    while (true) {
        printf("\n=============== Bem vindo! =================\n"
                "Digite um comando para prosseguir:\n"
                "C – Gerenciar Clientes\n"
                "T – Gerenciar Contas\n"
                "S – Sair\n");

        do {
            // Get first char and discards everything else, including newline
            input = getchar();
            while (getchar() != '\n');

            // Accept both uppercase and lowercase. Transform to lowercase
            if (input >= 'A' && input <= 'Z')
                input += 'a' - 'A';

        } while (input != 'c' && input != 't' && input != 's');

        switch (input) {
            case 'c':
                menuClient(dbClient, dbAccount);
                break;
            case 't':
                menuConta(dbClient, dbAccount);
                break;
            case 's':
                cleanExit(dbClient, dbAccount, 0);
                break;
        }

    }
}


void menuClient(FILE *dbClient, FILE *dbAccount)
{
    int input;
    int id = 0;
    long position = 0;

    printf("\n============ Gerenciar Clientes ============\n"
           "Digite um comando para prosseguir:\n"
           "C – Cadastrar um cliente\n"
           "L – Listar todos os clientes cadastrados\n"
           "B – Buscar cliente já cadastrado\n"
           "A – Atualizar um cliente cadastrado\n"
           "E – Excluir um cliente cadastrado\n"
           "V – Voltar\n"
           "S - Sair\n");

    do {
        // Get first char and discards everything else, including newline
        input = getchar();
        while (getchar() != '\n');

        // Accept both uppercase and lowercase. Transform to lowercase
        if (input >= 'A' && input <= 'Z')
            input += 'a' - 'A';

    } while ((input != 'c') && (input != 'l') && (input != 'b') && 
             (input != 'a') && (input != 'e') && (input != 'v') &&
             (input != 's'));

    switch (input) {
        case 'c':
            addClient(dbClient, position);
            break;
        case 'l':
            sortClient(dbClient);
            listClient(dbClient);
            break;
        case 'b':
            searchClient(dbClient);
            break;
        case 'a':
            id = searchClient(dbClient);
            position = positionClient(dbClient, id);
            if (id != 0) {
                printf("\n");
                zeroClient(dbClient, position);
                addClient(dbClient, position);
            }
            break;
        case 'e':
            id = searchClient(dbClient);
            position = positionClient(dbClient, id);
            if (id != 0)
                removeClient(dbClient, position);
            break;
        case 'v':
            break;
        case 's':
            cleanExit(dbClient, dbAccount, 0);
            break;
        default:
            break;
    }
}


void menuConta(FILE *dbClient, FILE *dbAccount)
{
    int input;
    int id;

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
            listAllAcc(dbAccount, dbClient);
            break;
        case 'c':
            id = searchClient(dbClient);
            if (id != 0) {
                printf("\n");
                addAccount(dbAccount, id);
            }
            break;
        case 's':
            cleanExit(dbClient, dbAccount, 0);
            break;
        default:
            break;
    }
}


void addClient(FILE *dbClient, long position)
{
    int id;
    char nome[30];
    char cpf[15];
    char phone[15];
    char addr[90];

    printf("Informe o código do cliente: ");
    // Read scanf until it matches one int. Otherwise flush buffer and loop
    while ((scanf("%d", &id) != 1) && (id <= 0)) {
        printf("Entrada inválida. Digite um número positivo para o ID do "
                                                                "cliente.\n");
        while (getchar() != '\n');
    }
    while (getchar() != '\n');

    // Check for newline in buffer. If it is there, remove it.
    // Otherwise, flush the buffer to remove it and anything else from there.
    printf("Informe o nome do cliente. Max 29 caracteres: ");
    fgets(nome, 30, stdin);
    size_t length = strlen(nome) - 1;
    if (nome[length] == '\n')
        nome[length] = 0;
    else
        while (getchar() != '\n');

    // TODO: Add check to validate input as number
    printf("Informe o CPF ou CNPJ do cliente: ");
    fgets(cpf, 15, stdin);
    length = strlen(cpf) - 1;
    if (cpf[length] == '\n')
        cpf[length] = 0;
    else
        while (getchar() != '\n');
    
    // TODO: Add check to validate input as number
    printf("Informe o telefone do cliente: ");
    fgets(phone, 15, stdin);
    length = strlen(phone) - 1;
    if (phone[length] == '\n')
        phone[length] = 0;
    else
        while (getchar() != '\n');

    printf("Informe o endereço do cliente. Max 70 caracteres: ");
    fgets(addr, 90, stdin);
    length = strlen(addr) - 1;
    if (addr[length] == '\n')
        addr[length] = 0;
    else
        while (getchar() != '\n');

    if (checkClient(id, cpf, dbClient)) {
            struct Cliente clienteLi;
            clienteLi.id = id;
            strcpy(clienteLi.nome, nome);
            strcpy(clienteLi.cpf, cpf);
            strcpy(clienteLi.phone, phone);
            strcpy(clienteLi.addr, addr);

            printf("\nCliente cadastrado com sucesso.\n"
                    "Código:   %d\n"
                    "Nome:     %s\n"
                    "CPF/CNPJ: %s\n"
                    "Telefone: %s\n"
                    "Endereço: %s\n",
                    clienteLi.id, clienteLi.nome, clienteLi.cpf,
                    clienteLi.phone, clienteLi.addr);

            // Used when updating
            if (position == 0)
                fseek(dbClient, 0, SEEK_END);
            else
                fseek(dbClient, ((long)sizeof(struct Cliente) * (position - 1)),
                                                                    SEEK_SET);

            fwrite(&clienteLi, sizeof(struct Cliente), 1, dbClient);
    } else {
        printf("\nCliente já cadastrado no banco de dados.\n"
               "Cliente deve ter Código e CPF/CNPJ únicos.\n");
    }
}

// Make sure there is no repeated ID or CPF/CNPJ
// Returns true if not repeated and false otherwise.
bool checkClient(int id, char *cpf, FILE *dbClient)
{
    struct Cliente clienteLi;
    rewind(dbClient);
    while(fread(&clienteLi, sizeof(struct Cliente), 1, dbClient)) {
        if (clienteLi.id == id)
            return false;
        else if (strcmp(clienteLi.cpf, cpf) == 0)
            return false;
    }

    return true;
}


void listClient(FILE *dbClient)
{
    struct Cliente clienteLi;

    rewind(dbClient);
    while (fread(&clienteLi, sizeof(struct Cliente), 1, dbClient)) {
        printf("Código:   %d\n"
                "Nome:     %s\n"
                "CPF/CNPJ: %s\n"
                "Telefone: %s\n"
                "Endereço: %s\n\n",
                clienteLi.id, clienteLi.nome, clienteLi.cpf,
                clienteLi.phone, clienteLi.addr);
    }

    //TODO: DELETE ONCE VALIDATED
    /** New sorting function **
    struct Cliente clienteLi;
    rewind(dbClient);

    // Get how many elemets are in the struct
    size_t qtyClient = 0;
    while (fread(&clienteLi, sizeof(struct Cliente), 1, dbClient) == 1)
        ++qtyClient;

    printf("\n============= Lista de Clientes ============\n");
    if (qtyClient == 0) {
        printf("Nenhum cliente cadastrado.\n");
        return;
    }

    // Store all of the structs into an array for use with qsort
    rewind(dbClient);
    struct Cliente sortCli[qtyClient];
    for (size_t i = 0; i < qtyClient; ++i)
        fread(&sortCli[i], sizeof(struct Cliente), 1, dbClient);

    qsort(sortCli, qtyClient, sizeof(struct Cliente), stringComp);

    // Overwrite new sorted database, and also print it
    fseek(dbClient, 0, SEEK_SET);
    for (size_t i = 0; i < qtyClient; ++i) {
        fwrite(&sortCli[i], sizeof(struct Cliente), 1, dbClient);
        printf("Código:   %d\n"
               "Nome:     %s\n"
               "CPF/CNPJ: %s\n"
               "Telefone: %s\n"
               "Endereço: %s\n\n",
                sortCli[i].id, sortCli[i].nome, sortCli[i].cpf,
                sortCli[i].phone, sortCli[i].addr);
    }
***************************/
}




// Returns id, so the functions can be reused when sarching accounts.
// The previous position return has been made it's own function.
int searchClient(FILE *dbClient)
{
    struct Cliente clienteLi;
    char type[5];
    char cpfComp[5] = {"cpf:"};
    char codComp[5] = {"cod:"};
    char cpf[15];
    size_t length;
    int id;

    printf("\nBuscar Cliente. A busca pode ser feita por CPF/CNPJ ou código,\n"
           "seguindo o exemplo abaixo:\n"
           "cpf:<NUMERO DO CPF>\ncod:<NUMERO DO CODIGO>\n");
    fgets(type, 5, stdin);
    rewind(dbClient);

    // If fgets got the new line, get rid of it, but no need to clear the buffer
    // Otherwise, clear the buffer with getchar
    if (!strcmp(type, cpfComp)) {
        fgets(cpf, 15, stdin);
        length = strlen(cpf) - 1;
        if (cpf[length] == '\n')  
            cpf[length] = 0;
        else
            while (getchar() != '\n');

        while (fread(&clienteLi, sizeof(struct Cliente), 1, dbClient)) {
            if (strcmp(cpf, clienteLi.cpf) == 0) {
                printf("\nCliente com CPF/CNPJ encontrado:\n"
                        "Código:   %d\n"
                        "Nome:     %s\n"
                        "CPF/CNPJ: %s\n"
                        "Telefone: %s\n"
                        "Endereço: %s\n",
                        clienteLi.id, clienteLi.nome, clienteLi.cpf,
                        clienteLi.phone, clienteLi.addr);
                return clienteLi.id;
            }
        }
        printf("\nCPF/CNPJ não encontrado.\n");

    } else if (!strcmp(type, codComp)) {
        if (scanf("%d", &id) != 1) {
            printf("\nBusca inválida. O ID do cliente é um número.\n");
            while (getchar() != '\n');
            return 0;
        }
        while (getchar() != '\n');

        while (fread(&clienteLi, sizeof(struct Cliente), 1, dbClient)) {
            if (id == clienteLi.id) {
                printf("\nCliente com código encontrado:\n"
                        "Código:   %d\n"
                        "Nome:     %s\n"
                        "CPF/CNPJ: %s\n"
                        "Telefone: %s\n"
                        "Endereço: %s\n",
                        clienteLi.id, clienteLi.nome, clienteLi.cpf,
                        clienteLi.phone, clienteLi.addr);
                return clienteLi.id;
            }
        }
        printf("\nCódigo não encontrado.\n");
    } else {
        printf("\nBusca Inválida! Leia as instruções de busca novamente.\n");
    }
    return 0;
}

// Returns location so function can be reused when updating or removing Client.
// ftell divided by sizeof. Do not subtract now to prevent function from
// returning 0. Subtract by sizeof, or 1, if already divided, before use. This 
// way pointer can go to the beginning of the position that needs to be changed
long positionClient(FILE *dbClient, int id)
{
    struct Cliente clienteLi;
    long position = 0;

    rewind(dbClient);
    while (fread(&clienteLi, sizeof(struct Cliente), 1, dbClient)) {
        if (id == clienteLi.id) {
            position = (ftell(dbClient) / (long int)sizeof(struct Cliente));
            return position;
        }
    }
    printf("ERRO NO BANCO DE DADOS. CLIENTE COM ID INEXISTENTE.\n");
    return 1;
}



// Zero position on the file, so that when updating it doen't get matched
// as a repeated cod or cpf/cnpj by checkClient();
void zeroClient(FILE *dbClient, long position)
{
    struct Cliente clienteLi;
    memset(&clienteLi, 0, sizeof(struct Cliente));
    fseek(dbClient, ((long)sizeof(struct Cliente) * (position - 1)), SEEK_SET);
    fwrite(&clienteLi, sizeof(struct Cliente), 1, dbClient);
}


// Load everything into ram, remove the target client, and write again to file
// and truncate. Another option would be creating a new file, and renaming both
// but I need to pass argv[] all the way to this function. And third option,
// currently implemented, is simply reopening the file with the w+b option, and
// that will truncate the file, and then write, while keeping the FILE stream.
void removeClient(FILE *dbClient, long position)
{
    struct Cliente clienteLi;
    int qtyClient = 0;
    --position;

    rewind(dbClient);
    while (fread(&clienteLi, sizeof(struct Cliente), 1, dbClient) == 1)
        ++qtyClient;

    rewind(dbClient);
    struct Cliente removeCli[qtyClient];
    for (long i = 0; i < qtyClient; ++i) {
        if (i == position) {
            fseek(dbClient, sizeof(struct Cliente), SEEK_CUR);
            continue;
        }
        fread(&removeCli[i], sizeof(struct Cliente), 1, dbClient);
    }

    freopen(NULL, "w+b", dbClient);
    rewind(dbClient);
    for (long i = 0; i < qtyClient; ++i) {
        if (i == position)
            continue;
        fwrite(&removeCli[i], sizeof(struct Cliente), 1, dbClient);
    }
    printf("Cliente removido com sucesso!\n");
}
