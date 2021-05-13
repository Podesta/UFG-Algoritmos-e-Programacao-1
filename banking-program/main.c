#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

int menuMain();
void menuClient(FILE *db);
void menuConta(FILE *db);
void addClient(FILE *db, long position);
void listClient(FILE *db);
bool checkClient(int id, char *cpf, FILE *db);
int cleanExit(FILE *db, int a);
int stringComp(const void *s1, const void *s2);
long searchClient(FILE *db);
void zeroClient(FILE *db, long position);
void removeClient(FILE *db, long position);

struct Conta {
    char agencia[5];
    char conta[11];
    int saldo;
};

struct Cliente {
    int id;
    char nome[30];
    char cpf[15];   // CNPJ pode ter 14 digitos
    char phone[15];
    char addr[90];
    struct Conta contaCl;
};


int main(int argc, char *argv[])
{

    struct Cliente clienteLi[10];
    memset(clienteLi, 0, sizeof(clienteLi));

    // User must provide one and only one database at runtime
    if (argc < 2 || argc > 2) {
        printf("Por favor, informe um, e somente um, banco de dados a "
               "ser utilizado ao iniciar o programa.\n");
        return 1;
    }
    
    FILE *database;
    database = fopen(argv[1], "r+b");

    if (database == NULL) {
        printf("Banco de dados inexistente!.\nPor favor, crie o banco de dados "
               "antes de iniciar o programa.\n");
        return 1;
    }

    int test = menuMain(database);
    printf("%c\n", test);

    fclose(database);

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
int cleanExit(FILE *db, int a)
{
    fclose(db);
    exit(a);
}



int menuMain(FILE *db)
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
                menuClient(db);
                break;
            case 't':
                menuConta(db);
                break;
            case 's':
                cleanExit(db, 0);
                break;
        }

    }
}


void menuClient(FILE *db)
{
    int input;
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
            addClient(db, position);
            break;
        case 'l':
            listClient(db);
            break;
        case 'b':
            searchClient(db);
            break;
        case 'a':
            position = searchClient(db);
            if (position != 0) {
                zeroClient(db, position);
                addClient(db, position);
            }
            break;
        case 'e':
            position = searchClient(db);
            if (position != 0)
                removeClient(db, position);
            break;
        case 's':
            cleanExit(db, 0);
            break;
        default:
            break;
    }
}


void menuConta(FILE *db)
{
    int input;

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
        case 's':
            cleanExit(db, 0);
            break;
        default:
            break;
    }
}


void addClient(FILE *db, long position)
{
    int id;
    char nome[30];
    char cpf[15];
    char phone[15];
    char addr[90];

    printf("Informe o código do cliente: ");
    // Read scanf until it matches one int. Otherwise flush buffer and loop
    while (scanf("%d", &id) != 1) {
        printf("Entrada inválida. Digite um número para o ID do cliente.\n");
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

    if (checkClient(id, cpf, db)) {
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
                fseek(db, 0, SEEK_END);
            else
                fseek(db, ((long)sizeof(struct Cliente) * (position - 1)),
                                                                    SEEK_SET);

            fwrite(&clienteLi, sizeof(struct Cliente), 1, db);
    } else {
        printf("\nCliente já cadastrado no banco de dados.\n"
               "Cliente deve ter Código e CPF/CNPJ únicos.\n");
    }
}

// Make sure there is no repeated ID or CPF/CNPJ
// Returns true if not repeated and false otherwise.
bool checkClient(int id, char *cpf, FILE *db)
{
    struct Cliente clienteLi;
    rewind(db);
    while(fread(&clienteLi, sizeof(struct Cliente), 1, db)) {
        if (clienteLi.id == id)
            return false;
        else if (strcmp(clienteLi.cpf, cpf) == 0)
            return false;
    }

    return true;
}


void listClient(FILE *db)
{
    struct Cliente clienteLi;
    rewind(db);

    // Get how many elemets are in the struct
    size_t qtyClient = 0;
    while (fread(&clienteLi, sizeof(struct Cliente), 1, db) == 1)
        ++qtyClient;

    printf("\n============= Lista de Clientes ============\n");
    if (qtyClient == 0) {
        printf("Nenhum cliente cadastrado.\n");
        return;
    }

    // Store all of the structs into an array for use with qsort
    rewind(db);
    struct Cliente sortCli[qtyClient];
    for (size_t i = 0; i < qtyClient; ++i)
        fread(&sortCli[i], sizeof(struct Cliente), 1, db);

    qsort(sortCli, qtyClient, sizeof(struct Cliente), stringComp);

    // Overwrite new sorted database, and also print it
    fseek(db, 0, SEEK_SET);
    for (size_t i = 0; i < qtyClient; ++i) {
        fwrite(&sortCli[i], sizeof(struct Cliente), 1, db);
        printf("Código:   %d\n"
               "Nome:     %s\n"
               "CPF/CNPJ: %s\n"
               "Telefone: %s\n"
               "Endereço: %s\n\n",
                sortCli[i].id, sortCli[i].nome, sortCli[i].cpf,
                sortCli[i].phone, sortCli[i].addr);
    }
}


// Returns location so function can be reused when updating or removing Client.
// ftell divided by sizeof. Do not subtract now to prevent function from
// returning 0. Subtract by sizeof, or 1, if already divided, before use. This 
// way pointer can go to the beginning of the position that needs to be changed
long searchClient(FILE *db)
{
    struct Cliente clienteLi;
    char type[5];
    char cpfComp[5] = {"cpf:"};
    char codComp[5] = {"cod:"};
    char cpf[15];
    size_t length;
    int id;
    long position;

    printf("/nBuscar Cliente. A busca pode ser feita por CPF/CNPJ ou código,\n"
           "seguindo o exemplo abaixo:\n"
           "cpf:<NUMERO DO CPF>\ncod:<NUMERO DO CODIGO>\n");
    fgets(type, 5, stdin);
    rewind(db);

    // If fgets got the new line, get rid of it, but no need to clear the buffer
    // Otherwise, clear the buffer with getchar
    if (!strcmp(type, cpfComp)) {
        fgets(cpf, 15, stdin);
        length = strlen(cpf) - 1;
        if (cpf[length] == '\n')  
            cpf[length] = 0;
        else
            while (getchar() != '\n');

        while (fread(&clienteLi, sizeof(struct Cliente), 1, db)) {
            if (strcmp(cpf, clienteLi.cpf) == 0) {
                printf("\nCliente com CPF/CNPJ encontrado:\n"
                        "Código:   %d\n"
                        "Nome:     %s\n"
                        "CPF/CNPJ: %s\n"
                        "Telefone: %s\n"
                        "Endereço: %s\n",
                        clienteLi.id, clienteLi.nome, clienteLi.cpf,
                        clienteLi.phone, clienteLi.addr);
                position = (ftell(db) / (long int)sizeof(struct Cliente));
                return position;
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

        while (fread(&clienteLi, sizeof(struct Cliente), 1, db)) {
            if (id == clienteLi.id) {
                printf("\nCliente com código encontrado:\n"
                        "Código:   %d\n"
                        "Nome:     %s\n"
                        "CPF/CNPJ: %s\n"
                        "Telefone: %s\n"
                        "Endereço: %s\n",
                        clienteLi.id, clienteLi.nome, clienteLi.cpf,
                        clienteLi.phone, clienteLi.addr);
                position = (ftell(db) / (long int)sizeof(struct Cliente));
                return position;
            }
        }
        printf("\nCódigo não encontrado.\n");
    } else {
        printf("\nBusca Inválida! Leia as instruções de busca novamente.\n");
    }
    return 0;
}


// Zero position on the file, so that when updating it doen't get matched
// as a repeated cod or cpf/cnpj by checkClient();
void zeroClient(FILE *db, long position)
{
    struct Cliente clienteLi;
    memset(&clienteLi, 0, sizeof(struct Cliente));
    fseek(db, ((long)sizeof(struct Cliente) * (position - 1)), SEEK_SET);
    fwrite(&clienteLi, sizeof(struct Cliente), 1, db);
}


// Load everything into ram, remove the target client, and write again to file
// and truncate. Another option would be creating a new file, and renaming both
// but I need to pass argv[] all the way to this function. And third option,
// currently implemented, is simply reopening the file with the w+b option, and
// that will truncate the file, and then write, while keeping the FILE stream.
void removeClient(FILE *db, long position)
{
    struct Cliente clienteLi;
    int qtyClient = 0;
    --position;

    rewind(db);
    while (fread(&clienteLi, sizeof(struct Cliente), 1, db) == 1)
        ++qtyClient;

    rewind(db);
    struct Cliente removeCli[qtyClient];
    for (long i = 0; i < qtyClient; ++i) {
        if (i == position) {
            fseek(db, sizeof(struct Cliente), SEEK_CUR);
            continue;
        }
        fread(&removeCli[i], sizeof(struct Cliente), 1, db);
    }

    freopen(NULL, "w+b", db);
    rewind(db);
    for (long i = 0; i < qtyClient; ++i) {
        if (i == position)
            continue;
        fwrite(&removeCli[i], sizeof(struct Cliente), 1, db);
    }
    printf("Cliente removido com sucesso!\n");
}
