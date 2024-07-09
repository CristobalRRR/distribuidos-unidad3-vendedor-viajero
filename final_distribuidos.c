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
#include <time.h>
#include <math.h>
#include <string.h>


int largo_cromosoma;
int tamaño_poblacion; // Numero de cromosomas
int numero_iteraciones; //Numero de iteraciones o generaciones
float **matriz_distancia;  //Matriz de distancias
char nombre_archivo[255]; //Nombre del archivo TSP

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
    float fitness;
} Cromosoma;
Cromosoma * poblacion;

//Muestra la aptitud y la ruta del cromosoma
void mostrar_cromosoma(Cromosoma *ptr_cromosoma){
    float fitness_a_invertir = ptr_cromosoma->fitness;
    float distancia = pow(10, 10 / fitness_a_invertir);
    printf("Costo de del viaje: %f\n", distancia);
    printf("Fitness = %f, Genes= ", ptr_cromosoma->fitness);
    for (int i = 0; i < largo_cromosoma; i++)
        printf("%d_", ptr_cromosoma->genes[i]);
    printf("\n");
}
//Muestra los valores de los cromosomas para todos los miembros de la poblacion
void mostrar_poblacion(Cromosoma *poblacion){
    //for(int i = 0; i < tamaño_poblacion; i++){
        mostrar_cromosoma(&poblacion[tamaño_poblacion-1]);
   // }
}

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
    
    //Busca la palabra DIMENSION para obtener el valor y asignarlo a numero de ciudades
    while (fgets(buffer, sizeof(buffer), fp)) {
        if (sscanf(buffer, "DIMENSION : %d", &num_ciudades) == 1 || sscanf(buffer, "DIMENSION: %d", &num_ciudades) == 1) {
            break;
        }
    }
    //Verifica si encontro DIMENSION
    if (num_ciudades <= 0) {
        printf("Error, DIMENSION no encontrada.\n");
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
float euclidiana(Ciudad ciudad1, Ciudad ciudad2){
    return sqrt(pow(ciudad1.x - ciudad2.x,2) + pow(ciudad1.y - ciudad2.y,2));
}

void mostrar_matriz_distancia(){
    printf("\n\t");
    for (int i = 0; i < largo_cromosoma; ++i){
        printf("\n|%d|\t", i);
        for (int j = 0; j < largo_cromosoma; j++)
            printf("(%d,%d) = %.4f\t",i,j, matriz_distancia[i][j]);
    }
}
    
void iniciar_matriz_distancia(){
    leer_desde_archivo();
    matriz_distancia = malloc(sizeof (float *)* largo_cromosoma);
    for (int i = 0; i < largo_cromosoma; i++)
        matriz_distancia[i] = calloc(largo_cromosoma, sizeof (float));
    
    for (int i = 0; i < largo_cromosoma - 1; i++){
        for (int j = i + 1; j < largo_cromosoma; j++){
            float distancia_ruta = euclidiana(ciudades[i], ciudades[j]);
            matriz_distancia[i][j] = matriz_distancia[j][i] = distancia_ruta;
        }
    }
    free(ciudades);
}

//En esta funcion se calcula el nivel de aptitud para el cromosoma
void calcular_fitness(Cromosoma *ptr_cromosoma){
//las variables fitness e i se inician en 0 para calcular la aptitud y que empiezen
//en 0 para cada llamado a la funcion
    float fitness = 0;
    int i = 0;
//El ciclo for recorre los genes, que son las ciudades, que conforman al cromosoma, buscando
//desde la matriz de distancia el costo del recorrido entre esas 2 ciudades y lo acumula en fitness
//EL rango y posicion de la matriz llevan "-1" porque cuando se guardan los datos los indices comienzan
//desde 1 en vez de 0
    for (i = 0; i < largo_cromosoma-1; i++)
        fitness += matriz_distancia[ptr_cromosoma->genes[i] -1][ptr_cromosoma->genes[i+1] -1];
//Cuando llega al final del recorrido rescata el costo del viaje de la ultima ciudad hasta la primera
//para sumarlo a la aptitud para cumplir con el "Visita cada ciudad 1 vez y al finalizar regresa a la ciudad de origen"
    fitness += matriz_distancia[ptr_cromosoma->genes[i]-1 ][0];
//Finalmente para determinar la aptitud obtiene el inverso de fitness, fitness^-1, y le aplica un logaritmo base 10
//de forma de que al obtener una distancia mas pequeña, la aptitud sera mas alta y el logaritmo crea un valor mas entendible
    fitness = 10/log10(fitness);
    ptr_cromosoma-> fitness= fitness;
}

//Send each chromosome to the above function
void calcular_fitness_poblacion(Cromosoma *poblacion){
    for(int i = 0; i < tamaño_poblacion; i++)
        calcular_fitness(&poblacion[i]);
}

//Generar numero random con semillas dependiendo del reloj
int obtenerNumAleatorio(){
	int seed = (unsigned)(time(NULL)+rand());
	srand(seed);
	return rand()%largo_cromosoma;
}


//funcion que llena los cromosomas y genera las las posibles rutas aleatorias
void llenar_cromosoma_aleatoriamente(Cromosoma *cromo){
    int array[largo_cromosoma];
    cromo->genes = malloc(largo_cromosoma * sizeof (int));
    
    for(int i = 0; i < largo_cromosoma; i++)
        array[i] = i+1;

    for(int i = 0; i < largo_cromosoma; i++){
        int nbRand = obtenerNumAleatorio()%(largo_cromosoma -i);
        int tmp = array[nbRand];
        array[nbRand] = array[largo_cromosoma - i - 1];
        array[largo_cromosoma - i - 1] = tmp;
        cromo->genes[i] = tmp;
    }
    calcular_fitness(cromo); //Encontrar el valor de aptitud o fitness
}

//El intercambio de cromosomas servira para ordenarlos por su aptitud
void intercambiar_cromosomas(Cromosoma *pobla, int src, int dest){
	Cromosoma cromo  = pobla[src];
	pobla[src] = pobla[dest];
	pobla[dest] = cromo;
}

//Ordena la poblacion segun el valor de aptitud de cada cromosoma
void ordena_poblacion(Cromosoma *poblacion){
    for(int i = 0; i < tamaño_poblacion; i++)
        for(int j = i+1; j < tamaño_poblacion; j++)
            if(poblacion[i].fitness <poblacion[j].fitness)
                intercambiar_cromosomas(poblacion, i, j);
}

//Esta funciona genera un indice aleatorio de un cromosoma para ser usado
//en el crossover
int indice_aleatorio_cromosoma(){
    int seed = (unsigned)(time(NULL)+rand());
    srand(seed);
    return rand()%tamaño_poblacion;
}

//Aqui se hace la seleccion de los cromosomas a ser usados para el crossover
void seleccionar(Cromosoma *pobla){
    int n = (40*tamaño_poblacion)/100;
    //Swapping 10 chromosomes from the last 50% to the middle
    for(int i = 0; i < (10*tamaño_poblacion)/100; i++){ 
        int randNb =(tamaño_poblacion/2) + indice_aleatorio_cromosoma()%(tamaño_poblacion/2);
        intercambiar_cromosomas(poblacion, n+i, randNb);
    }
}

//Muestra el valor de fitness para cada cromosoma
void mostrar_fitness(){
    printf("\n------------------------------------------------------Fitness-------------------------------------------\n");
    for(int i = 0; i < tamaño_poblacion; i++)
        printf("%.3f - ",poblacion[i].fitness);
    printf("\n------------------------------------------------------Fitness-------------------------------------------------\n");
}


//ESta funcion revisa la diferencia entre 2 cromosomas
float porcentaje_de_diferencia(Cromosoma cro1, Cromosoma cro2){
	float sum = 0;
	for(int i = 0; i < largo_cromosoma; i++)
		if(cro1.genes[i] != cro2.genes[i])
			sum++;
	return (sum*100)/largo_cromosoma;
}

//Revisa si existe un vertice en el cromosoma
int existe_vertice(Cromosoma *crom, int x){
	for(int i=0; i<largo_cromosoma; i++)
		if(x == crom->genes[i])
			return 1;
	return 0;
}

//Genera al hijo obtenido del crossover
void crear_hijoV2(Cromosoma p, Cromosoma m, Cromosoma *Crom){
	int z=1;
	int n = obtenerNumAleatorio()%(largo_cromosoma);

	for(int i=0; i < largo_cromosoma;i++)
		Crom->genes[i] =0;
        
	for(int i = n; i < n+((largo_cromosoma*30)/100);i++){
		z=i%largo_cromosoma;
		Crom->genes[z]=p.genes[z];
	}
	int c=0;
	int i=(z+1)%largo_cromosoma;
	while(i!=z){
		c = c%largo_cromosoma;
		if(existe_vertice(Crom, m.genes[c]) != 1)
			Crom->genes[i] = m.genes[c];

		else{
			if(Crom->genes[i] == 0){
				while(existe_vertice(Crom, m.genes[c]) == 1){
					c++;
				}
				Crom->genes[i] = m.genes[c];
			}
		}
		c++;
		i++;
		i=i%largo_cromosoma;
	}
}

//Esta funcion realiza el crossover entre 2 cromosomas
void crossoverV2(Cromosoma *pobla){
	int nb=0;
	for(int i = 0; i < (tamaño_poblacion/2) ; i++){
		do{
			nb= obtenerNumAleatorio() % (tamaño_poblacion/2);
		}while(nb == i && porcentaje_de_diferencia(pobla[i], pobla[nb]) < 70);
		crear_hijoV2(pobla[i], pobla[nb], &pobla[(tamaño_poblacion/2) +i]);
	}
}

//Esta funcion realiza las mutaciones del cromosoma reordenando
//aleatoriamente las rutas en base a un numero aleatorio generado por una semilla
void mutacion(Cromosoma *pobla){
//En este ciclo for se define un z<5 que significa que hara 5 mutaciones al cromosoma
	for(int z=0; z<5; z++){
		int i = obtenerNumAleatorio()%(largo_cromosoma);
		int j = obtenerNumAleatorio()%(largo_cromosoma);
		int k = obtenerNumAleatorio()%(tamaño_poblacion -(20*tamaño_poblacion/100));
		int temp = pobla[(20*tamaño_poblacion/100)+k].genes[j];
		pobla[(20*tamaño_poblacion/100)+k].genes[j] = pobla[(20*tamaño_poblacion/100)+k].genes[i];
		pobla[(20*tamaño_poblacion/100)+k].genes[i] = temp;
	}
}

int main(int argc, char **argv){
    tamaño_poblacion = 50; // Numero de cromosomas
    numero_iteraciones = 1000; //Numero de iteraciones o generaciones
    
    //Estas lineas piden ingresar el nombre del archivo para obtener las coordenadas
    printf("Ingrese el nombre completo del archivo TSP\n");
    scanf("%s",nombre_archivo); 
    recibir_nombre(nombre_archivo);
    
    //Se inicia el calculo de la distancia euclidiana entre todas las ciudades
    //para que sea mas facil calcular los costos más tarde
    iniciar_matriz_distancia(); 
    //Se asigna memoria dinamica a la poblacion de rutas (cromosomas) con un largo igual a la dimension(numero de ciudades)
    poblacion = (Cromosoma *)malloc(tamaño_poblacion*sizeof(Cromosoma));
    

    //Iniciamos el reloj para medir cuanto se demora
    clock_t tiempo;
    tiempo = clock();
    //Se inicia la creacion de cromosomas
    for(int i = 0; i < tamaño_poblacion; i++){
        llenar_cromosoma_aleatoriamente(&poblacion[i]);
    }
    //Se llama a la funcion que ordena la poblacion de cromosomas en base al nivel de fitness(aptitud)
    ordena_poblacion(poblacion);
    int  i = 0;
    while(i < numero_iteraciones){
        seleccionar(poblacion); //Select 50% of poblacion
        crossoverV2(poblacion); //Perform the crossover
        mutacion(poblacion); //Perform mutation
        calcular_fitness_poblacion(poblacion); // Calculate new fitness value
        ordena_poblacion(poblacion); //Sort acc to fitness value
        i++;
    }

    //Tomamos tiempo actual del reloj y lo restamos al inicial para obtener el tiempo
    tiempo = clock() - tiempo;
    double tiempo_de_ejecucion = ((double)tiempo)/CLOCKS_PER_SEC;
    printf("Tiempo: \t %f \n", tiempo_de_ejecucion);
    mostrar_poblacion(poblacion);
    return 0;
}