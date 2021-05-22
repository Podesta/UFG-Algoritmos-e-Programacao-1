#include "lclient.h"

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


int menuMain(FILE *dbClient, FILE *dbAccount, FILE *dbTransaction)
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
                menuConta(dbClient, dbAccount, dbTransaction);
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

void addClient(FILE *dbClient, long position)
{
    int id;
    char nome[30];
    char cpf[15];
    char phone[15];
    char addr[90];

    printf("Informe o código do cliente: ");
    // Read scanf until it matches one int. Otherwise flush buffer and loop
    while (scanf("%d", &id), id <= 0) {
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
