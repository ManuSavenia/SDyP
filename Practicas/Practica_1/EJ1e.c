#include<stdio.h>
#include<stdlib.h>
#include <sys/time.h>
#define ORDENXFILAS 0
#define ORDENXCOLUMNAS 1

//Dimension por defecto de las matrices
int N=100;

//Para calcular tiempo
double dwalltime(){
        double sec;
        struct timeval tv;

        gettimeofday(&tv,NULL);
        sec = tv.tv_sec + tv.tv_usec/1000000.0;
        return sec;
}

int main(int argc,char*argv[]){
 double *M,*Lf,*Uf,*Lc,*Uc,*MU,*ML,*UM,*LM;
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
  M=(double*)malloc(sizeof(double)*N*N);
  Lf=(double*)malloc(sizeof(double)*N*N);
  Uf=(double*)malloc(sizeof(double)*N*N);
  Lc=(double*)malloc(sizeof(double)*N*N);
  Uc=(double*)malloc(sizeof(double)*N*N);
  MU=(double*)malloc(sizeof(double)*N*N);
  ML=(double*)malloc(sizeof(double)*N*N);
  UM=(double*)malloc(sizeof(double)*N*N);
  LM=(double*)malloc(sizeof(double)*N*N);

//Inicializacion resultados y U,L

for(i=0;i<N;i++){
    for(j=0;j<N;j++){
        MU[i*N + j] = 0;
        ML[i*N + j] = 0;
        UM[i*N + j] = 0;
        LM[i*N + j] = 0;
        Uf[i*N + j] = 0;
        Uc[i + j*N] = 0;
        Lf[i*N + j] = 0;
        Lc[i + j*N] = 0;
        }
    }
  
//Inicializacion de M,Uf,Uc,Lf,Lc en 1
  for(i=0;i<N;i++){
   for(j=0;j<N;j++){
	M[i*N+j]=1;
    Uf[i*N + j-(i*(i+1)/2)] = 1;
    Uc[i+(j*(j+1)/2)] = 1;
    Lf[j+(i*(i+1)/2)] = 1;
    Lc[i+ N*j-(j*(j+1)/2)] = 1;
   }
  }

timetick = dwalltime(); 

//Multiplicacion MU sin guardar 0s

for(i=0;i<N;i++){
    for(j=0;j<N;j++){
        for(k=0;k<=j;k++){  
            MU[i*N + j] += M[i*N + k] * Uc[k + j*N];
        }
    }
}

//Multiplicacion ML sin guardar 0s

for(i=0;i<N;i++){
    for(j=0;j<N;j++){
        for(k=j;k<N;k++){  
            ML[i*N + j] += M[i*N + k] * Lc[k + j*N];
        }
    }
}

//Multiplicacion UM sin guardar 0s

for(i=0;i<N;i++){
    for(j=0;j<N;j++){
        for(k=i;k<N;k++){  
            UM[i*N + j] += Uf[i*N + k] * M[k + j*N];
        }
    }
}

//Multiplicacion LM sin guardar 0s

for(i=0;i<N;i++){
    for(j=0;j<N;j++){
        for(k=0;k<=i;k++){  
            UM[i*N + j] += Lf[i*N + k] * M[k + j*N];
        }
    }
}

printf("Tiempo en segundos sin guardar 0s %f\n", dwalltime() - timetick);

timetick = dwalltime();

//Multiplicacion MU 

for(i=0;i<N;i++){
    for(j=0;j<N;j++){
        for(k=0;k<N;k++){  
            MU[i*N + j] += M[i*N + k] * Uc[k + j*N];
        }
    }
}

//Multiplicacion ML 

for(i=0;i<N;i++){
    for(j=0;j<N;j++){
        for(k=0;k<N;k++){  
            ML[i*N + j] += M[i*N + k] * Lc[k + j*N];
        }
    }
}

//Multiplicacion UM 

for(i=0;i<N;i++){
    for(j=0;j<N;j++){
        for(k=0;k<N;k++){  
            UM[i*N + j] += Uf[i*N + k] * M[k + j*N];
        }
    }
}

//Multiplicacion LM 

for(i=0;i<N;i++){
    for(j=0;j<N;j++){
        for(k=0;k<N;k++){  
            UM[i*N + j] += Lf[i*N + k] * M[k + j*N];
        }
    }
}

printf("Tiempo en segundos guardando 0s %f\n", dwalltime() - timetick);

 free(M);
 free(Uf);
 free(Lf);
 free(Uc);
 free(Lc);
 free(MU);
 free(ML);
 free(UM);
 free(LM);
 
 return(0);
}
