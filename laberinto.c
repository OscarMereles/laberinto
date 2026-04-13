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
    SetConsoleOutputCP(65001);
    SetConsoleCP(CP_UTF8);
    #endif
    const char *emojis[]={"🧱","🟩","🕳️ "," 🚪"};
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
    int entrada[2]={0,0},salida[2]={(alto-1),(ancho-1)};
    const char *laberinto[alto][ancho];
    for(int fila = 0;fila<alto;fila++){
        for (int colu = 0; colu < ancho; colu++){
            int aleatorio=rand()%5;
                if (aleatorio==0 || aleatorio== 1 || aleatorio==2 || aleatorio==3){
                    laberinto[fila][colu]=emojis[0];
                }
                else{
                    laberinto[fila][colu]=emojis[1];
                }
                laberinto[entrada[0]][entrada[1]]=emojis[2];
                laberinto[salida[0]][salida[1]]=emojis[3];
        }
    }
    for (int fil = 0; fil < alto; fil++)
    {
        for (int col = 0; col < ancho; col++)
        {
            printf("%s",laberinto[fil][col]);
            //printf("%d %d",fil, col);
        }
        printf("\n");
    }
    return 0;
}
