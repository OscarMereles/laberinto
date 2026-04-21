#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#ifdef _WIN32
#include <windows.h>
#endif

#define MAX 25
#define QUEUE_MAX 625

typedef struct {
    char celdas[MAX][MAX];
    int filas;
    int columnas;
} Laberinto;

typedef struct {
    int x, y;
} Posicion;

typedef struct {
    Posicion items[QUEUE_MAX];
    int frente, atras;
} Cola;

void inicializarLaberinto(Laberinto *l);
void configurarEntradaSalida(Laberinto *l);
void generarLaberinto(Laberinto *l);
void backtrackGenerar(Laberinto *l, int x, int y);
void mezclarDirecciones(int dirs[]);
void imprimirLaberinto(const Laberinto *l, bool mostrarSolucion);
bool resolverLaberinto(Laberinto *l);
bool esValida(const Laberinto *l, int x, int y);
void garantizarSalidaConectada(Laberinto *l);
Cola* crearCola();
void encolar(Cola *q, int x, int y);
bool desencolar(Cola *q, int *x, int *y);
bool colaVacia(Cola *q);
int reconstruirCamino(Laberinto *l, Posicion padre[MAX][MAX]);

int main() {
    Laberinto lab = {0};
    clock_t inicio, fin;
    double tiempo_gen = 0.0;
    char opcion;
    bool usarDefault = true;

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    srand(time(NULL));

    printf("=== LABERINTO MAESTRO ===\n\n");

    do {
        if (usarDefault) {
            printf("\n--- Laberinto por defecto (10x10) ---\n");
            lab.filas = 10;
            lab.columnas = 10;
            usarDefault = false;
        } else {
            printf("\n--- Nuevo laberinto personalizado ---\n");
            printf("Ingrese el ancho del laberinto (minimo 5, maximo %d): ", MAX);
            scanf("%d", &lab.columnas);
            printf("Ingrese el alto del laberinto (minimo 5, maximo %d): ", MAX);
            scanf("%d", &lab.filas);

            if (lab.filas < 5 || lab.filas > MAX || lab.columnas < 5 || lab.columnas > MAX) {
                printf("Error: Las dimensiones deben estar entre 5 y %d\n", MAX);
                printf("Usando valores por defecto (10x10)...\n");
                lab.filas = 10;
                lab.columnas = 10;
            }
        }

        inicio = clock();
        inicializarLaberinto(&lab);
        configurarEntradaSalida(&lab);
        generarLaberinto(&lab);
        garantizarSalidaConectada(&lab);
        fin = clock();
        tiempo_gen = (double)(fin - inicio) / CLOCKS_PER_SEC;

        printf("\nLaberinto generado (%dx%d) en %.6f segundos\n",
               lab.filas, lab.columnas, tiempo_gen);
        imprimirLaberinto(&lab, false);

        printf("\nResolviendo laberinto... (BFS)\n");
        inicio = clock();
        bool resuelto = resolverLaberinto(&lab);
        fin = clock();

        if (resuelto) {
            printf("Laberinto resuelto en %.6f segundos!\n", (double)(fin-inicio)/CLOCKS_PER_SEC);
            imprimirLaberinto(&lab, true);
        }

        printf("\n=== Estadisticas ===\n");
        printf("Tiempo total : %.6f s\n", tiempo_gen);

        printf("\n¿Desea generar otro laberinto? (s/n): ");
        scanf(" %c", &opcion);

        if (opcion == 's' || opcion == 'S') {
            usarDefault = false;
        } else {
            break;
        }
    } while (1);

    printf("\n¡Eso es todo amigos 🐰🥕!\n");
    return 0;
}

void inicializarLaberinto(Laberinto *l) { // Rellena la matriz con paredes
    for (int i = 0; i < l->filas; i++)
        for (int j = 0; j < l->columnas; j++)
            l->celdas[i][j] = '#';
}

void configurarEntradaSalida(Laberinto *l) { // Marca entrada (S) y salida (E)
    l->celdas[1][1] = 'S';
    l->celdas[l->filas-2][l->columnas-2] = 'E';
}

void mezclarDirecciones(int dirs[]) { // Baraja aleatoriamente las 4 direcciones (Fisher-Yates)
    for (int i = 0; i < 4; i++) dirs[i] = i;
    for (int i = 3; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = dirs[i]; dirs[i] = dirs[j]; dirs[j] = temp;
    }
}

bool esValida(const Laberinto *l, int x, int y) { // Verifica que (x,y) esté dentro del laberinto
    return x > 0 && x < l->filas-1 && y > 0 && y < l->columnas-1;
}

void garantizarSalidaConectada(Laberinto *l) { // Asegura que la salida esté conectada
    int ex = l->filas - 2;
    int ey = l->columnas - 2;
    int dx[] = {-1, 1, 0, 0};
    int dy[] = {0, 0, -1, 1};
    bool conectado = false;

    for (int i = 0; i < 4; i++) {
        int nx = ex + dx[i];
        int ny = ey + dy[i];
        if (esValida(l, nx, ny) && l->celdas[nx][ny] == ' ') {
            conectado = true;
            break;
        }
    }

    if (conectado) return;

    printf("Salida bloqueada. Conectando...\n");
    for (int i = 0; i < 4; i++) {
        int nx = ex + dx[i];
        int ny = ey + dy[i];
        if (esValida(l, nx, ny) && l->celdas[nx][ny] == '#') {
            l->celdas[nx][ny] = ' ';
            break;
        }
    }
}

void backtrackGenerar(Laberinto *l, int x, int y) { // Genera el laberinto recursivamente (DFS)
    l->celdas[x][y] = ' ';
    int dirs[4], dx[] = {-2,0,2,0}, dy[] = {0,2,0,-2};
    mezclarDirecciones(dirs);

    for (int i = 0; i < 4; i++) {
        int nx = x + dx[dirs[i]];
        int ny = y + dy[dirs[i]];
        if (esValida(l, nx, ny) && l->celdas[nx][ny] == '#') {
            l->celdas[x + dx[dirs[i]]/2][y + dy[dirs[i]]/2] = ' ';
            backtrackGenerar(l, nx, ny);
        }
    }
}

void generarLaberinto(Laberinto *l) { // Inicia la generación del laberinto
    backtrackGenerar(l, 1, 1);
}

void imprimirLaberinto(const Laberinto *l, bool mostrarSolucion) { // Imprime el laberinto con emojis
    for (int i = 0; i < l->filas; i++) {
        for (int j = 0; j < l->columnas; j++) {
            if (i == 1 && j == 1)
                printf("🕳   ");
            else if (i == l->filas-2 && j == l->columnas-2)
                printf("🚪  ");
            else if (mostrarSolucion && l->celdas[i][j] == 'O')
                printf("🟨  ");
            else if (l->celdas[i][j] == ' ')
                printf("🟩  ");
            else
                printf("🧱  ");
        }
        printf("\n");
    }
}

Cola* crearCola() { // Crea una cola vacía con memoria dinámica
    Cola *q = (Cola*)malloc(sizeof(Cola));
    q->frente = 0;
    q->atras = -1;
    return q;
}

void encolar(Cola *q, int x, int y) { // Agrega elemento (x,y) al final de la cola
    if (q->atras < QUEUE_MAX - 1) {
        q->atras++;
        q->items[q->atras].x = x;
        q->items[q->atras].y = y;
    }
}

bool desencolar(Cola *q, int *x, int *y) { // Extrae el primer elemento de la cola
    if (q->frente <= q->atras) {
        *x = q->items[q->frente].x;
        *y = q->items[q->frente].y;
        q->frente++;
        return true;
    }
    return false;
}

bool colaVacia(Cola *q) { // Verifica si la cola está vacía
    return q->frente > q->atras;
}

int reconstruirCamino(Laberinto *l, Posicion padre[MAX][MAX]) { // Traza el camino desde salida a entrada
    int x = l->filas - 2, y = l->columnas - 2;
    int pasos = 0;

    while (!(x == 1 && y == 1)) {
        l->celdas[x][y] = 'O';
        int px = padre[x][y].x;
        int py = padre[x][y].y;
        x = px;
        y = py;
        pasos++;
    }
    return pasos;
}

bool resolverLaberinto(Laberinto *l) { // Resuelve el laberinto usando BFS (cola FIFO)
    Cola *q = crearCola();
    bool visitado[MAX][MAX] = {false};
    Posicion padre[MAX][MAX];

    int dx[] = {-1, 1, 0, 0};
    int dy[] = {0, 0, -1, 1};

    encolar(q, 1, 1);
    visitado[1][1] = true;
    padre[1][1].x = -1;
    padre[1][1].y = -1;

    while (!colaVacia(q)) {
        int x, y;
        desencolar(q, &x, &y);

        if (x == l->filas - 2 && y == l->columnas - 2) {
            reconstruirCamino(l, padre);
            free(q);
            return true;
        }

        for (int i = 0; i < 4; i++) {
            int nx = x + dx[i];
            int ny = y + dy[i];

            if (esValida(l, nx, ny) && !visitado[nx][ny] &&
                (l->celdas[nx][ny] == ' ' || l->celdas[nx][ny] == 'E')) {
                visitado[nx][ny] = true;
                padre[nx][ny].x = x;
                padre[nx][ny].y = y;
                encolar(q, nx, ny);
            }
        }
    }

    free(q);
    return false;
}
