#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

int menuMain();
void menuCliente(FILE *db);
void menuConta(void);
void addCliente(FILE *db);
void listCliente(FILE *db);
bool checkCliente(int id, char *cpf, FILE *db);

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
    char addr[70];
    struct Conta contaCl;
};

int main(int argc, char *argv[])
{

    struct Cliente clienteLi[10];
    memset(clienteLi, 0, sizeof clienteLi);

    // User must provide one and only one database at runtime
    if (argc < 2 || argc > 2) {
        printf("Por favor, informe um, e somente um, banco de dados a "
               "ser utilizado ao iniciar o programa.\n");
        return 1;
    }
    
    FILE *database;
    database = fopen(argv[1], "a+b");

    int test = menuMain(database);
    printf("%c\n", test);

    fclose(database);


    return 0;
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
                menuCliente(db);
                break;
            case 't':
                menuConta();
                break;
            case 's':
                exit(0);
                break;
        }

    }
}

void menuCliente(FILE *db)
{
    int input;

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
            addCliente(db);
            break;
        case 'l':
            listCliente(db);
            break;
        case 's':
            exit(0);
            break;
        default :
            break;
    }
}

void menuConta(void)
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
            exit(0);
            break;
        default:
            break;
    }
}

void addCliente(FILE *db)
{
    int id;
    char nome[30];
    char cpf[15];
    char phone[15];
    char addr[70];

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
    fgets(addr, 15, stdin);
    length = strlen(addr) - 1;
    if (addr[length] == '\n')
        addr[length] = 0;
    else
        while (getchar() != '\n');

    if (checkCliente(id, cpf, db)) {
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

            fwrite(&clienteLi, sizeof(struct Cliente), 1, db);
    } else {
        printf("\nCliente já cadastrado no banco de dados.\n"
               "Cliente deve ter Código e CPF/CNPJ únicos.\n");
    }
}

// Make sure there is no repeated ID or CPF/CNPJ
// Returns true if not repeated and false otherwise.
bool checkCliente(int id, char *cpf, FILE *db)
{
    struct Cliente clienteLi;
    rewind(db);
    while(fread(&clienteLi, sizeof (struct Cliente), 1, db)) {
        if (clienteLi.id == id)
            return false;
        else if (strcmp(clienteLi.cpf, cpf) == 0)
            return false;
    }

    return true;
}


void listCliente(FILE *db)
{
    struct Cliente clienteLi;
    rewind(db);

    printf("\n============= Lista de Clientes ============\n");
    if (fread(&clienteLi, sizeof (struct Cliente), 1, db) != 1) {
        printf("Nenhum cliente cadastrado.\n");
    } else {
        rewind(db);
        while(fread(&clienteLi, sizeof (struct Cliente), 1, db))
            printf("Código:   %d\n"
                    "Nome:     %s\n"
                    "CPF/CNPJ: %s\n"
                    "Telefone: %s\n"
                    "Endereço: %s\n\n",
                    clienteLi.id, clienteLi.nome, clienteLi.cpf,
                    clienteLi.phone, clienteLi.addr);
    }
}
