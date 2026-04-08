#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#ifdef _WIN32
#include <windows.h>
#endif

int main(){
    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    #endif
    const char *emojis[]={"🧱","🟩"};
    int alto=0,ancho=0,validacion_alto=0,validacion_ancho=0;
    srand(time(NULL));
    do{
        do{
            printf("Ingrese ancho del laberinto ");
            validacion_ancho=scanf("%d",&ancho);
            printf("Ingrese alto del laberinto ");
            validacion_alto=scanf("%d",&alto);

        } while (validacion_alto!=1 || validacion_ancho!=1);
    } while (ancho<1 && alto<1);
    const char *laberinto[ancho][alto];
    for(int fila = 0;fila<alto;fila++){
        for (int colu = 0; colu < ancho; colu++){
            int aleatorio=rand()%4;
            if (aleatorio==0){
                laberinto[fila][colu]=emojis[0];
            }
            else{
                laberinto[fila][colu]=emojis[1];
            }
        }
    }
    for (int fila = 0; fila < alto; fila++)
    {
        for (int colu = 0; colu < ancho; colu++)
        {
            printf("%c",laberinto[fila][colu]);
        }
        printf("\n");
    }
    
    return 0;
}
