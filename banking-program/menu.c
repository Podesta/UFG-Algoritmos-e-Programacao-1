#include "lclient.h"
#include "laccount.h"

int main(int argc, char *argv[])
{

    struct Cliente clienteLi[10];
    memset(clienteLi, 0, sizeof(clienteLi));

    // User must provide one and only one database at runtime
    if (argc < 4 || argc > 4) {
        printf("Por favor, informe os bancos de dados ao iniciar o programa na "
               "seguinte ordem:\n 1 - Banco de dados do cliente\n 2 - Banco "
               "de dados das Contas.\n 3 - Banco de dados das Transações.\n");
        return 1;
    }
    
    FILE *dbCli;
    dbCli = fopen(argv[1], "r+b");
    if (dbCli == NULL)
        dbCli = fopen(argv[1], "w+b");

    FILE *dbAcc;
    dbAcc = fopen(argv[2], "r+b");
    if (dbAcc == NULL) {
        dbAcc = fopen(argv[2], "w+b");
    }

    FILE *dbTra;
    dbTra = fopen(argv[3], "r+b");
    if (dbTra == NULL) {
        dbTra = fopen (argv[3], "w+b");
    }

    int test = menuMain(dbCli, dbAcc, dbTra);
    printf("%c\n", test);

    fclose(dbCli);
    fclose(dbAcc);

    return 0;
}

