/*
Autores:
- Alfredo Becerra
- Cristobal Ramirez

Asignatura: Sistemas Distribuidos

Compilado y ejecutado en Linux Debian 12 Bookworm
*/

//#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//Para lograr un codigo mas ordenado vamos a hacer que la distancia euclidiana sea una funcion
/*
double euclideanDistance(double v1[], double v2[], int dim){
    double sum = 0.0;
    for (int i = 0; i < dim; i++){
        double diff = v1[i] - v2[i];
        sum += diff * diff;
    }
    return sqrt(sum);
}
*/

int main(int argc, char **argv){
    //int rank, size;
    FILE *archivo;
    char nombre_archivo[15]; //Los archivos a recibir son lin318.tsp y pcb442.tsp
    printf("Ingrese nombre completo del archivo (Ejemplo: lin318.tsp; pcb442.tsp): ");
    scanf("%s", nombre_archivo);

    //Comenzamos la lectura
    archivo = fopen(nombre_archivo, "r");
    if(archivo == NULL){
        perror("Error al abrir el archivo");
        return 1;
    }
    printf("Si se pudú\n");
    fclose(archivo);

    /*MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Finalize();*/

    
    return 0;
}