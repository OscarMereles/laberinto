#include <stdio.h>    // Librería para entrada/salida (printf, scanf)
#include <stdlib.h>   // Librería para funciones estándar (malloc, srand, etc.)
#include <time.h>     // Librería para manejo de tiempo (clock, time, etc.)
#include <stdbool.h>  // Librería para tipo booleano (true, false)

#ifdef _WIN32
#include <windows.h>  // Librería de Windows para manejar consola
#endif

#define MAX 25  // Define el tamaño máximo del laberinto (25x25)

typedef struct {
    char celdas[MAX][MAX];  // Matriz 2D que almacena el laberinto (celdas con '#' o ' ')
    int filas;              // Número de filas del laberinto
    int columnas;           // Número de columnas del laberinto
} Laberinto;  // Define una estructura que contiene un laberinto

//PROTOTIPOS 
void inicializarLaberinto(Laberinto *l);
void configurarEntradaSalida(Laberinto *l);
void generarLaberinto(Laberinto *l);
void backtrackGenerar(Laberinto *l, int x, int y);
void mezclarDirecciones(int dirs[]);

void imprimirLaberinto(const Laberinto *l, bool mostrarSolucion);

bool resolverLaberinto(Laberinto *l);
bool backtrackResolver(Laberinto *l, int x, int y);

bool esValida(const Laberinto *l, int x, int y);
bool verificarCaminoExiste(Laberinto *l);
bool backtrackVerificar(Laberinto *l, bool visitado[MAX][MAX], int x, int y);

void garantizarSalidaConectada(Laberinto *l);   // ← NUEVA FUNCIÓN



int main() {
    Laberinto lab = {0};                    // Crea estructura del laberinto, inicializada en 0
    clock_t inicio, fin;                    // Variables para medir tiempo
    double tiempo_gen = 0.0;                // Variable para almacenar tiempo de generación

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);            // Configura consola de Windows para UTF-8 (emojis)
    SetConsoleCP(CP_UTF8);                  // Configura entrada de consola para UTF-8
#endif

    srand(time(NULL));                      // Inicializa generador de números aleatorios con tiempo actual

    printf("=== LABERINTO MAESTRO ===\n\n");                        // Imprime encabezado del programa
    printf("Ingrese el tamaño del laberinto (impar, ejemplo: 11, 15, 21): ");  // Solicita tamaño al usuario
    scanf("%d", &lab.filas);                    // Lee el tamaño desde teclado
    lab.columnas = lab.filas;               // Hace el laberinto cuadrado (filas = columnas)

    if (lab.filas < 5 || lab.filas > MAX || lab.filas % 2 == 0) {  // Valida que sea impar y esté en rango [5, MAX]
        printf("Error: El tamaño debe ser impar y entre 5 y %d\n", MAX);  // Mensaje de error
        return 1;                           // Termina el programa con error
    }

    inicio = clock();                       // Registra tiempo de inicio
    inicializarLaberinto(&lab);             // Llena todo de paredes ('#')
    configurarEntradaSalida(&lab);          // Marca entrada 'S' y salida 'E'
    generarLaberinto(&lab);                 // Genera el laberinto con algoritmo backtracking
    garantizarSalidaConectada(&lab);        // Asegura que la salida tenga conexión
    fin = clock();                          // Registra tiempo de fin
    tiempo_gen = (double)(fin - inicio) / CLOCKS_PER_SEC;  // Calcula tiempo transcurrido en segundos

    printf("\nLaberinto generado (%dx%d) en %.4f segundos\n",  // Imprime info de generación
           lab.filas, lab.columnas, tiempo_gen);
    imprimirLaberinto(&lab, false);         // Muestra el laberinto sin solución

    printf("\nResolviendo laberinto...\n");  // Mensaje de inicio de resolución
    inicio = clock();                       // Registra tiempo de inicio de resolución
    bool resuelto = resolverLaberinto(&lab);  // Intenta resolver el laberinto
    fin = clock();                          // Registra tiempo de fin de resolución

    if (resuelto) {                         // Si se encontró solución
        printf("¡Laberinto resuelto en %.4f segundos!\n", (double)(fin-inicio)/CLOCKS_PER_SEC);  // Muestra tiempo
        imprimirLaberinto(&lab, true);      // Imprime laberinto con la solución marcada
    }

    printf("\n=== Estadísticas ===\n");     // Encabezado de estadísticas
    printf("Tiempo total : %.4f s\n", tiempo_gen);  // Imprime tiempo de generación

    return 0;  // Termina el programa correctamente
}// Funcion PRINCIPAL: Controla todo el flujo del programa (generación y resolución de laberintos)

// ====================== IMPLEMENTACIÓN ======================

void inicializarLaberinto(Laberinto *l) {
    for (int i = 0; i < l->filas; i++)        // Itera sobre todas las filas
        for (int j = 0; j < l->columnas; j++) // Itera sobre todas las columnas
            l->celdas[i][j] = '#';            // Rellena cada celda con '#' (pared)
}
// Funcion: Rellena toda la matriz con paredes '#' para crear la estructura base

void configurarEntradaSalida(Laberinto *l) {
    l->celdas[1][1] = 'S';                              // Coloca la ENTRADA en la posición (1,1)
    l->celdas[l->filas-2][l->columnas-2] = 'E';        // Coloca la SALIDA (E=Exit) en la esquina opuesta
}
// Funcion: Marca el punto de inicio (S) y final (E) del laberinto

void mezclarDirecciones(int dirs[]) {
    for (int i = 0; i < 4; i++) dirs[i] = i;           // Inicializa array con [0,1,2,3] (4 direcciones)
    for (int i = 3; i > 0; i--) {                       // Itera desde 3 hasta 1 (algoritmo Fisher-Yates)
        int j = rand() % (i + 1);                       // Elige un índice aleatorio entre 0 e i
        int temp = dirs[i]; dirs[i] = dirs[j]; dirs[j] = temp;  // Intercambia dos elementos
    }
}
// Funcion: Mezcla aleatoriamente las 4 direcciones (arriba, abajo, izquierda, derecha)

bool esValida(const Laberinto *l, int x, int y) {
    return x > 0 && x < l->filas-1 && y > 0 && y < l->columnas-1;  // Verifica que (x,y) esté dentro del rango válido
}
// Funcion: Valida que una posición sea dentro de los límites del laberinto

void garantizarSalidaConectada(Laberinto *l) {
    int ex = l->filas - 2;                  // Obtiene la coordenada x de la salida
    int ey = l->columnas - 2;               // Obtiene la coordenada y de la salida

    int dx[] = {-1, 1, 0, 0};               // Desplazamientos en x para 4 direcciones (arriba, abajo, quieto, quieto)
    int dy[] = {0, 0, -1, 1};               // Desplazamientos en y para 4 direcciones (quieto, quieto, izq, der)

    bool conectado = false;                 // Bandera para verificar si la salida está conectada

    for (int i = 0; i < 4; i++) {           // Itera sobre las 4 direcciones adyacentes a la salida
        int nx = ex + dx[i];                // Calcula coordenada x adyacente
        int ny = ey + dy[i];                // Calcula coordenada y adyacente
        if (esValida(l, nx, ny) && l->celdas[nx][ny] == ' ') {  // Si es válida y es pasillo
            conectado = true;               // Marca que está conectada
            break;                          // Sale del bucle
        }
    }

    if (conectado) return;                  // Si ya está conectada, no hace nada y retorna

    printf("Salida bloqueada. Conectando...\n");  // Mensaje si la salida está bloqueada
    for (int i = 0; i < 4; i++) {           // Itera nuevamente sobre las 4 direcciones
        int nx = ex + dx[i];                // Calcula coordenada x adyacente
        int ny = ey + dy[i];                // Calcula coordenada y adyacente
        if (esValida(l, nx, ny) && l->celdas[nx][ny] == '#') {  // Si es pared
            l->celdas[nx][ny] = ' ';        // Convierte la pared en pasillo (abre camino)
            break;                          // Sale del bucle (solo necesita abrir un pasillo)
        }
    }
}
// Funcion: Garantiza que la salida esté conectada con al menos un pasillo adyacente

void backtrackGenerar(Laberinto *l, int x, int y) {
    l->celdas[x][y] = ' ';                                 // Marca la celda actual como pasillo (' ')

    int dirs[4], dx[] = {-2,0,2,0}, dy[] = {0,2,0,-2};    // dirs[] para 4 direcciones, dx/dy movimientos de 2 celdas
    mezclarDirecciones(dirs);                             // Baraja aleatoriamente las 4 direcciones

    for (int i = 0; i < 4; i++) {                         // Prueba las 4 direcciones en orden aleatorio
        int nx = x + dx[dirs[i]];                         // Calcula nueva posición x
        int ny = y + dy[dirs[i]];                         // Calcula nueva posición y
        if (esValida(l, nx, ny) && l->celdas[nx][ny] == '#') {  // Si la nueva posición es válida y es pared
            l->celdas[x + dx[dirs[i]]/2][y + dy[dirs[i]]/2] = ' ';  // Abre la pared del medio (conecta)
            backtrackGenerar(l, nx, ny);                            // Llamada recursiva para explorar desde (nx,ny)
        }
    }
}
// Funcion: Genera el laberinto recursivamente usando algoritmo de profundidad primero (backtracking)

void generarLaberinto(Laberinto *l) {
    backtrackGenerar(l, 1, 1);  // Inicia la generación del laberinto desde la posición (1,1)
}
// Funcion: Envoltura que inicia el algoritmo de generación del laberinto

bool verificarCaminoExiste(Laberinto *l) {
    bool visitado[MAX][MAX] = {false};      // Crea matriz de visitados inicializada en false
    return backtrackVerificar(l, visitado, 1, 1);  // Inicia la verificación desde la entrada (1,1)
}
// Funcion: Verifica si existe un camino válido desde entrada a salida

bool backtrackVerificar(Laberinto *l, bool visitado[MAX][MAX], int x, int y) {
    if (x == l->filas-2 && y == l->columnas-2) return true;  // Si llegamos a la salida, retorna true
    if (!esValida(l, x, y) || visitado[x][y] || l->celdas[x][y] == '#') return false;  // Si no es válido/visitado/pared, retorna false

    visitado[x][y] = true;                  // Marca la celda actual como visitada

    return backtrackVerificar(l, visitado, x-1, y) ||  // Prueba ir ARRIBA
           backtrackVerificar(l, visitado, x+1, y) ||  // Prueba ir ABAJO
           backtrackVerificar(l, visitado, x, y-1) ||  // Prueba ir IZQUIERDA
           backtrackVerificar(l, visitado, x, y+1);    // Prueba ir DERECHA
}
// Funcion: Verifica recursivamente si existe un camino desde (x,y) hasta la salida

void imprimirLaberinto(const Laberinto *l, bool mostrarSolucion) {
    for (int i = 0; i < l->filas; i++) {                    // Itera sobre todas las filas
        for (int j = 0; j < l->columnas; j++) {            // Itera sobre todas las columnas
            if (i == 1 && j == 1)                          // Si es la posición de entrada
                printf("🕳️ ");                             // Dibuja entrada con emoji
            else if (i == l->filas-2 && j == l->columnas-2)  // Si es la posición de salida
                printf("🚪");                              // Dibuja salida con emoji
            else if (mostrarSolucion && l->celdas[i][j] == 'O')  // Si debe mostrar solución y es parte del camino
                printf("🟨");                              // Dibuja camino con emoji amarillo
            else if (l->celdas[i][j] == ' ')               // Si es un pasillo vacío
                printf("🟩");                              // Dibuja con emoji verde
            else                                             // Si es una pared
                printf("🧱");                              // Dibuja con emoji de pared
        }
        printf("\n");                                        // Nueva línea después de cada fila
    }
}
// Funcion: Imprime el laberinto usando emojis (entrada, salida, pasillos, paredes y solución)

bool backtrackResolver(Laberinto *l, int x, int y) {
    if (x == l->filas-2 && y == l->columnas-2) {           // Si llegamos a la salida
        l->celdas[x][y] = 'O';                              // Marca la salida en la solución
        return true;                                        // Retorna true (solución encontrada)
    }
    if (!esValida(l, x, y) || (l->celdas[x][y] != ' ' && l->celdas[x][y] != 'E' && l->celdas[x][y] != 'S'))  // Si no es válido o es pared
        return false;                                       // Retorna false

    l->celdas[x][y] = 'O';                                  // Marca la celda actual con 'O' (parte del camino)

    if (backtrackResolver(l, x-1, y)) return true;          // Intenta ir ARRIBA
    if (backtrackResolver(l, x+1, y)) return true;          // Intenta ir ABAJO
    if (backtrackResolver(l, x, y-1)) return true;          // Intenta ir IZQUIERDA
    if (backtrackResolver(l, x, y+1)) return true;          // Intenta ir DERECHA

    l->celdas[x][y] = ' ';                                  // Si ninguna dirección funciona, deshace el marcado (backtrack)
    return false;                                           // Retorna false (sin solución en esta rama)
}
// Funcion: Resuelve el laberinto marcando el camino con 'O' usando backtracking

bool resolverLaberinto(Laberinto *l) {
    return backtrackResolver(l, 1, 1);      // Inicia la resolución desde la entrada (1,1)
}