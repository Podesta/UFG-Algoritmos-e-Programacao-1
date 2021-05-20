#include "lclient.h"
#include "laccount.h"

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

