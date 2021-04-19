#include <stdio.h>

int main(void)
{
    int n = 0;
    int atual = 0;
    int maior = 0;
    int menor = 0;
    int soma  = 0;
    scanf("%d", &n);

    int i = 0;
    while (i < n)
    {
        scanf("%d", &atual);
        if (atual > maior)
            maior = atual;
        if (atual < menor)
            menor = atual;

        soma += atual;
        ++i;
    }
    printf("Maior: %d\nMenor: %d\nMedia: %d\n", maior, menor, soma / n);
    return 0;
}
