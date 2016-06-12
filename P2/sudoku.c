/*********************************************************************
 * 
 * Programa en C con OpenMP con 4 procesos, para obetener toas las
 * las soluciones de un sudoku de NxN con bolsa de tareas.
 *
 * Autor: Francisco Gago Villanueva
 * Fichero: sudoku.c
 * Título: Soduko en OpenMP
 * Complilado: gcc -fopenmp sudoku.c -o sudoku
 * Compillado en Mac(2016 El Capitan): gcc-5 -fopenmp sudoku.c -o sudoku
 * 
 *********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>

/*********************************************************************
 *
 * Es necesario incluir estas dos líneas si se hace uso del
 * compilador que viene en Mac OS 10.8.2:
 *
 * gcc version 4.2.1 (Based on Apple Inc. build 5658)
 * (LLVM build 2336.11.00)
 *
 *********************************************************************/
#include <pthread.h>
pthread_attr_t gomp_thread_attr;

#define N 9 //9x9
#define THREADS 4 //Procesos que vamos a lanzar

struct tarea{
    
    int s[N][N];
    int fila;
    int col;
};

struct nodo{
    
    struct tarea t;
    struct nodo *sig;
};

/*********************************************************************
 *
 * int buscaCeldaVacia(int s[N][N], int *x,int *y)
 *
 * Devuelve 1 si encuentra celda vacía; devuelve 0 en caso contrario
 *
 *********************************************************************/
int buscaCeldaVacia(int s[N][N], int *x,int *y)
{
	for(*x;(*x)<N;(*x)++)
	{
        
		for(*y;(*y)<N;(*y)++)
		{
			if(s[*x][*y] == 0)
            {
                return 1;
            }
		}
		*y = 0;
	}
	return 0;
}

/*********************************************************************
 *
 * iniciar(struct nodo **d)
 *
 * Método que inicia un nodo
 *
 *********************************************************************/
void iniciar(struct nodo **d){
    
    int i,j;
    *d = (struct nodo *) malloc(sizeof(struct nodo));
    
    //Tres soluciones
    int sudoku[9][9] =
    {
        {6,0,1 , 0,0,4 , 0,8,0},
        {0,4,9 , 7,3,0 , 0,0,0},
        {8,2,3 , 0,0,0 , 0,4,0},
        
        {0,0,4 , 0,0,0 , 0,0,6},
        {0,0,7 , 0,0,0 , 2,0,0},
        {9,8,0 , 0,0,0 , 3,0,0},
        
        {4,1,0 , 0,0,0 , 0,6,5},
        {0,0,0 , 0,4,1 , 8,3,0},
        {0,5,0 , 2,0,0 , 4,0,7}
    };

    //Una única solución
/*    int sudoku[9][9] =
    {
        {0,6,3 , 0,8,0 , 4,0,0},
        {0,9,0 , 0,0,4 , 0,0,7},
        {4,0,0 , 0,2,0 , 0,0,0},
        
        {0,0,7 , 2,0,3 , 0,5,8},
        {0,5,0 , 0,4,0 , 0,0,0},
        {0,0,8 , 0,0,0 , 2,0,4},
        
        {6,0,0 , 0,0,0 , 7,0,0},
        {0,0,5 , 0,3,6 , 0,0,0},
        {0,3,0 , 8,7,0 , 0,2,6}
    };*/
    
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            (*d)->t.s[i][j] = sudoku[i][j];
        
    (*d)->t.fila = 0;
    (*d)->t.col = 0;
    (*d)->sig = NULL;
}

/*********************************************************************
 *
 * void escribirtarea (struct tarea tareaact)
 *
 * Método que muestra por pantalla el sudoku
 *
 *********************************************************************/
void escribirtarea (struct tarea tareaact) {
    
    printf("\n");
    
    int i,j=0;
    for (i = 0; i < N; i++){
        for (j = 0; j < N; j++){
                printf("%d ", tareaact.s[i][j]);
            
        }
        printf("\n");
    }
    
    printf("\n*****************\n");
}

/*********************************************************************
 *
 * void escribir(struct nodo *d)
 *
 * Método que muestra la tarea de un nodo
 *
 *********************************************************************/
void escribir(struct nodo *d){
    
    int i;
    while (d != NULL) {
        escribirtarea(d->t);
        d = d->sig;
    }
}

/*********************************************************************
 *
 * int vacia(struct nodo *d)
 *
 * Devuelve 0 si un nodo está vacío y 1 en caso contrario
 *
 *********************************************************************/
int vacia(struct nodo *d){
    
    return d == NULL;
}

/*********************************************************************
 *
 * struct tarea extraer(struct nodo **d)
 *
 * Dado un nodo devuelve la tarea que contiene
 *
 *********************************************************************/
struct tarea extraer(struct nodo **d){
    
    struct tarea tareaact;
    struct nodo *p;
    int i,j;
    
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            tareaact.s[i][j] = (*d)->t.s[i][j];
    tareaact.fila = (*d)->t.fila;
    tareaact.col = (*d)->t.col;
    p = (*d);
    (*d) = (*d)->sig;
    free(p);
    return tareaact;
    
}

/*********************************************************************
 *
 * int valido(struct tarea tareaact, int nro)
 *
 * Dado una tarea y un número devuelve si el número es correcto
 *
 *********************************************************************/
int valido(struct tarea tareaact, int nro){
    
    int i = 0, j = 0;
    int fini = 0, cini = 0;
    
    //Bucle se encarga de recorrer la fila y la columna
    for(i = 0;i < N;i++){
        //fila
        if(tareaact.s[i][tareaact.col] == nro){
            return 0;
        }
        //columna
        if(tareaact.s[tareaact.fila][i] == nro){
            return 0;
        }
    }
    //Bucle se encarga de recorrerr el cuadrante
    fini = tareaact.fila - (tareaact.fila % 3);
    cini = tareaact.col - (tareaact.col % 3);
    
    for(i = fini;i < (fini + 3);i++){
        for(j = cini;j < (cini + 3);j++){
            if(tareaact.s[i][j] == nro){
                return 0;
            }
        }
    }
    return 1;
}

/*********************************************************************
 *
 * struct tarea ponerNumero(struct tarea tareaact, int nro)
 *
 * Devuelve la tarea tras colocarle el número
 *
 *********************************************************************/
struct tarea ponerNumero(struct tarea tareaact, int nro){
    
    int i,j;
    struct tarea nuevatarea;
    
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            nuevatarea.s[i][j] = tareaact.s[i][j];
    nuevatarea.s[tareaact.fila][tareaact.col] = nro;
    if (tareaact.col<8)
    {
        nuevatarea.fila=tareaact.fila;
        nuevatarea.col=tareaact.col+1;
    }
    else if (tareaact.fila<8)
    {
        nuevatarea.fila=tareaact.fila+1;
        nuevatarea.col=0;
    }
    else
    {
        nuevatarea.fila=8;
        nuevatarea.col=8;
    }
    
    return nuevatarea;
}

/*********************************************************************
 *
 * insertar(struct nodo **d, struct tarea nuevatarea)
 *
 * Método que inserta en un nodo la tarea correspondiente
 *
 *********************************************************************/
void insertar(struct nodo **d, struct tarea nuevatarea){
    
    struct nodo *temp;
    int i,j;
    
    temp = (struct nodo *) malloc(sizeof(struct nodo));
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            temp->t.s[i][j] = nuevatarea.s[i][j];
    temp->t.fila = nuevatarea.fila;
    temp->t.col = nuevatarea.col;
    temp->sig = (*d);
    (*d) = temp;
}

/*********************************************************************
 *
 * resolverSudoku()
 *
 * Función que realiza las llamadas necesarias para resolver el
 * sudoku indicado en la matriz que se encuentra en el método iniciar
 *
 *********************************************************************/
void resolverSudoku(){
    
    struct nodo *bolsa;
    struct tarea tareaact;
    int i,acabados,fin,tareas,tomada;
	int fila,col;
	
    iniciar(&bolsa);
    tareas=1;
    omp_set_num_threads (THREADS);
    acabados=THREADS;
    
    #pragma omp parallel private(fin, tareaact, i, tomada)
    {
        fin = 0;
        
        while (!fin){
            
            // acceso en exclusión mutua a la bolsa de tareas
            // y a las variables de control
            #pragma omp critical
            {
                fin = 0;
                tomada = 0;
                
                // tomada indica si se ha tomado una tarea de la bolsa
                if (tareas == 0 && acabados == THREADS){
                    fin=1;
                    
                }
                
                else if(tareas != 0){
                    
                    acabados--;
                    tareas--;
                    tomada = 1;
                    tareaact = extraer(&bolsa);
                }
            }
            
            if (tomada){
                
                //Inicializamos fila y columna a 0 en el momento
                //que tomada es igual a 1.
                fila=0;
                col=0;
                
                
                /***************************************************
                 *
                 * El método consiste en busca una celda
                 * recorrer todos los números (1-9) y comprobar
                 * si es válido para estar en la casilla
                 * de ser así insertamos en la bolsa de tarea
                 * con el número encontrado.
                 *
                 * En caso contrario estarán todas las casillas
                 * y por tanto mostramos por pantalla la solución.
                 *
                 ***************************************************/
                
                if (buscaCeldaVacia(tareaact.s,&tareaact.fila,&tareaact.col)){
                    
                    //Bucle que va desde 1 hasta 9
                    //es decir los números que pueden
                    //estar en una casilla
                    for (i = 1; i <= N; i++){
                        
                        if (valido(tareaact, i)){
                            
                            //En este caso se accede en sección crítica
                            //para proteger el incremento de tareas.
                            #pragma omp critical
                            {
                                
                                tareas++;
                                insertar(&bolsa, ponerNumero(tareaact, i));
                                
                            }
                            
                        }
                        
                    }//for
                    
                    
                }else{
                    
                    //En caso de no poner esta sección crítica
                    //no muestra de forma adecuada los resultados
                    #pragma omp critical
                    {
                        escribirtarea(tareaact);
                    }
                
                }//if buscaCeldaVacia
                
                #pragma omp critical
                {
                    acabados++;
                }
                
            }//if (tomada)
            
        }//while
        
    }//#pragma omp parallel private(fin, tareaact, i, tomada)
    
}//resolverSudoku()

/*********************************************************************
 *
 * Función principal: Imprime por pantalla y llama a resolverSudoku();
 *
 *********************************************************************/
int main(int argc, char *argv[]){
    
    printf("\nSolucion(es): \n");
    resolverSudoku();
    
}
