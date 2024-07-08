/*
Autores:
- Alfredo Becerra
- Cristobal Ramirez

Asignatura: Sistemas Distribuidos

Compilado y ejecutado en Linux Debian 12 Bookworm

Repositorios utilizados para desarrollar el trabajo:

-TSP hecho con algoritmos geneticos utilizando OpenMP
    Autor: ygutgutia
    Enlace: https://github.com/ygutgutia/TSP-Genetic-Algorithm-OpenMP/tree/main
*/


//#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

int largo_cromosoma;
int tamaño_poblacion; // Numero de cromosomas
int numero_iteraciones; //Numero de iteraciones o generaciones
double **matriz_distancia;  //Matriz de distancias
char nombre_archivo[15]; //Nombre del archivo TSP

//Para hacer el codigo mas modular, vamos a separar la ejecucion en varias funciones
//Definir una estructura para manejar las ciudades como estructuras con variables para numero, coordenada x y coordenada y
typedef struct{ //Structure used to store a city id and coordinate
    int id;
    float x;
    float y;
} Ciudad;
Ciudad *ciudades;

//Definimos el cromosoma, que sera la combinacion de ciudades para formar una ruta
typedef struct{
    int * genes;
    double fitness;
} Cromosoma;
Cromosoma * poblacion;


//Adaptacion del metodo para recibir el nombre del archivo por teclado
//para ser usado en la lectura del archivo
void recibir_nombre(const char *archivo){
    strcpy(nombre_archivo, archivo);
}

//Lee el archivo y asigna memoria dinamicamente al guardar las ciudades
void leer_desde_archivo(){
    int num_ciudades, linea;
    char buffer[1024];
    float x, y;
    FILE *fp;
    fp = fopen(nombre_archivo, "r");
    
    //Busca la palabra DIMENSION para obtener el numero de ciudades
    while (fgets(buffer, sizeof(buffer), fp)) {
        if (sscanf(buffer, "DIMENSION : %d", &num_ciudades) == 1 || sscanf(buffer, "DIMENSION: %d", &num_ciudades) == 1) {
            break;
        }
    }
    //Verifica si encontro DIMENSION
    if (num_ciudades <= 0) {
        printf("Erros, DIMENSION no encontrada.\n");
        exit(0);
    }
    largo_cromosoma = num_ciudades;
        
    //Luego de saber la dimension, escanea las coordenadas
    ciudades = (Ciudad *) malloc(sizeof (Ciudad) * num_ciudades);
    rewind(fp);
    for (int i = 0; i < 7; i++)
        fgets(buffer, 1024, fp);
	
    while (fscanf(fp, "%d %f %f", &linea, &x, &y) > 0 && linea <= num_ciudades){
        ciudades[linea - 1].id = linea;
        ciudades[linea - 1].x = x;
        ciudades[linea - 1].y = y;
    }
    fclose(fp);
    }

//Calcula la distancia euclidiana de las 2 ciudades usando los miembros o atributos
//de la estructura ciudades que definimos al principio
double euclidiana(Ciudad ciudad1, Ciudad ciudad2){
    double resultado = sqrt(pow(ciudad1.x - ciudad2.x,2) + pow(ciudad1.y - ciudad2.y,2));
    return resultado;
}

void mostrar_matriz_distancia(){
    printf("\n\t");
    for (int i = 0; i < largo_cromosoma; ++i){
        printf("\n|%d|\t", i);
        for (int j = 0; j < largo_cromosoma; j++)
            printf("(%d,%d) = %.4f\t\n",i,j, matriz_distancia[i][j]);
    }
}
    
void iniciar_matriz_distancia(){
    leer_desde_archivo();
    matriz_distancia = malloc(sizeof (double *)* largo_cromosoma);
    for (int i = 0; i < largo_cromosoma; i++)
        matriz_distancia[i] = calloc(largo_cromosoma, sizeof (double));
    
    for (int i = 0; i < largo_cromosoma - 1; i++){
        for (int j = i + 1; j < largo_cromosoma; j++){
            double distancia_ruta = euclidiana(ciudades[i], ciudades[j]);
            matriz_distancia[i][j] = matriz_distancia[j][i] = distancia_ruta;
        }
    }
    free(ciudades);
}

void calcular_fitness(Cromosoma *ptr_chromosome){
    float fitness = 0;
    int i = 0;
    for (i = 0; i < largo_cromosoma-1; i++)
        fitness += matriz_distancia[ptr_chromosome->genes[i] -1][ptr_chromosome->genes[i+1] -1];
        
    fitness += matriz_distancia[ptr_chromosome->genes[i]-1 ][0];
    fitness = 10/log10(fitness);
    ptr_chromosome-> fitness= fitness;
}

//Send each chromosome to the above function
void calculate_population_fitness(Cromosoma *poblacion){
    for(int i = 0; i < tamaño_poblacion; i++)
        calcular_fitness(&poblacion[i]);
}

//Generar numero random con semillas dependiendo del reloj
int getRandomNumber(){
	int seed = (unsigned)(time(NULL)+rand());
	srand(seed);
	return rand()%largo_cromosoma;
}

void llenar_cromosoma_aleatoriamente(Cromosoma *cromo){
    int array[largo_cromosoma];
    cromo->genes = malloc(largo_cromosoma * sizeof (int));
    //calcular_fitness(cromo); //Encontrar el valor de aptitud o fitness
}

int main(int argc, char **argv){
    tamaño_poblacion = 100; // Numero de cromosomas
    numero_iteraciones = 1000; //Numero de iteraciones o generaciones
    printf("Ingrese el nombre completo del archivo TSP\n");
    scanf("%s",nombre_archivo);
    recibir_nombre(nombre_archivo);
    iniciar_matriz_distancia();
    //mostrar_matriz_distancia();

    for(int i = 0; i < largo_cromosoma ; i++){
        llenar_cromosoma_aleatoriamente(&poblacion[i]);
    }
        
    /*MPI_Init(&argc, &argv);
    /*MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Finalize();*/

    return 0;
}
