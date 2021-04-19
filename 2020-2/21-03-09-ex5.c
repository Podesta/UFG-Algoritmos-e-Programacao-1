#include <stdio.h>

#define PI 3.14159265358979323846

int main(void)
{
    double voltas, raio, circumf, distance;

    printf("Forneca o raio, em cm, e a quantidade de "
            "voltas que sua roda deu\n");
    scanf("%lf %lf", &voltas, &raio);
    circumf = 2 * raio * PI;
    distance = voltas * circumf / 100;
    
    printf("Ele percorreu %.2lf metros.\n", distance);
    return 0;
}
