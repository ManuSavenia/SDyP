#include<stdio.h>
#include<stdlib.h>
#include <sys/time.h>
#define ORDENXFILAS 0
#define ORDENXCOLUMNAS 1

//Dimension por defecto de las matrices
int N=100;

//Retorna el valor de la matriz en la posicion fila y columna segun el orden que este ordenada
double getValor(double *matriz,int fila,int columna,int orden){
 if(orden==ORDENXFILAS){
  return(matriz[fila*N+columna]);
 }else{
  return(matriz[fila+columna*N]);
 }
}

//Establece el valor de la matriz en la posicion fila y columna segun el orden que este ordenada
void setValor(double *matriz,int fila,int columna,int orden,double valor){
 if(orden==ORDENXFILAS){
  matriz[fila*N+columna]=valor;
 }else{
  matriz[fila+columna*N]=valor;
 }
}

//Para calcular tiempo
double dwalltime(){
        double sec;
        struct timeval tv;

        gettimeofday(&tv,NULL);
        sec = tv.tv_sec + tv.tv_usec/1000000.0;
        return sec;
}

int main(int argc,char*argv[]){
 double *A,*B,*C;
 int i,j,k;
 int check=1;
 double timetick;

 //Controla los argumentos al programa
 if ((argc != 2) || ((N = atoi(argv[1])) <= 0) )
  {
    printf("\nUsar: %s n\n  n: Dimension de la matriz (nxn X nxn)\n", argv[0]);
    exit(1);
  }

 //Aloca memoria para las matrices
  A=(double*)malloc(sizeof(double)*N*N);
  B=(double*)malloc(sizeof(double)*N*N);
  C=(double*)malloc(sizeof(double)*N*N);

//Inicializa las matrices A
  for(i=0;i<N;i++){
   for(j=0;j<N;j++){
	A[i*N+j]=j;
   }
  }

timetick = dwalltime(); 

// Inicializa B
  for(i=0;i<N;i++){
   for(j=0;j<N;j++){
	B[i+j*N]=A[i*N+j];
   }
  } 

//Multiplicación de matrices sin funciones auxiliares


for(i=0;i<N;i++){
    for(j=0;j<N;j++){
        C[i*N + j] = 0;
        for(k=0;k<N;k++){
            C[i*N + j] += A[i*N + k] * B[k + j*N];
        }
    }
}
  

 printf("Tiempo en segundos dando vuelta %f\n", dwalltime() - timetick);

//Multiplicación de matrices sin funciones auxiliares

timetick = dwalltime();

for(i=0;i<N;i++){
    for(j=0;j<N;j++){
        C[i*N + j] = 0;
        for(k=0;k<N;k++){
            C[i*N + j] += A[i*N + k] * A[i*N + k];
        }
    }
}
  

 printf("Tiempo en segundos sin dar vuelta %f\n", dwalltime() - timetick);


 free(A);
 free(B);
 free(C);
 return(0);
}
