/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Codigo: Comunicação entre threads usando variável compartilhada e exclusao mutua com bloqueio */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

long int soma = 0; //variavel compartilhada entre as threads
pthread_mutex_t mutex; //variavel de lock para exclusao mutua
pthread_cond_t cond; // ALTERAÇÃO: variável de condicao
int somando = 1; // ALTERAÇÃO: variável que indica se as somas ainda estão sendo executadas 
int finalizadas = 0; // ALTERAÇÃO: variável que indica quantas threads de soma foram finalizadas

//ALTERAÇÃO: estrutura de dados usada pela funcao que as threads executam
typedef struct {
  long int id;
  int nthreads;
} arg_t;

//funcao executada pelas threads
void *ExecutaTarefa (void *arg) {
  arg_t* args = (arg_t*) arg;
  long int id = args->id;
  printf("Thread : %ld esta executando...\n", id);

  for (int i=0; i<100000; i++) {
     //--entrada na SC
     pthread_mutex_lock(&mutex);
     //--SC (seção critica)
     if (!(soma % 1000))
       pthread_cond_wait(&cond, &mutex);
     soma++; //incrementa a variavel compartilhada 
     //--saida da SC
     pthread_mutex_unlock(&mutex);
  }
  // ALTERAÇÃO: cada vez que uma thread termina sua execução, ela altera a variável finalizadas
  // e se for a última, seta a variável somando para 0 
  pthread_mutex_lock(&mutex);
  finalizadas++;
  if (finalizadas == args->nthreads) {
    somando = 0;
  }
  pthread_mutex_unlock(&mutex);
  printf("Thread : %ld terminou!\n", id);
  pthread_exit(NULL);
}

//funcao executada pela thread de log
void *extra (void *args) {
  printf("Extra : esta executando...\n");
  while (somando) { // ALTERAÇÃO: a thread de log deve continuar executando até que somas já não estejam mais sendo feitas
    pthread_mutex_lock(&mutex);
     if (!(soma%1000)) //imprime se 'soma' for multiplo de 10
        printf("soma = %ld \n", soma);
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);
  }
  pthread_cond_broadcast(&cond);

  printf("Extra : terminou!\n");
  pthread_exit(NULL);
}

//fluxo principal
int main(int argc, char *argv[]) {
   pthread_t *tid; //identificadores das threads no sistema
   int nthreads; //qtde de threads (passada linha de comando)

   //--le e avalia os parametros de entrada
   if(argc<2) {
      printf("Digite: %s <numero de threads>\n", argv[0]);
      return 1;
   }
   nthreads = atoi(argv[1]);

   //--aloca as estruturas
   tid = (pthread_t*) malloc(sizeof(pthread_t)*(nthreads+1));
   if(tid==NULL) {puts("ERRO--malloc"); return 2;}

   //--inicilaiza o mutex (lock de exclusao mutua)
   pthread_mutex_init(&mutex, NULL);
   pthread_cond_init(&cond, NULL);

   //--cria as threads
   for(long int t=0; t<nthreads; t++) {
     arg_t* arg = malloc(sizeof(arg_t));
     arg->id = t;
     arg->nthreads = nthreads;
     if (pthread_create(&tid[t], NULL, ExecutaTarefa, (void *)arg)) {
       printf("--ERRO: pthread_create()\n"); exit(-1);
     }
   }

   //--cria thread de log
   if (pthread_create(&tid[nthreads], NULL, extra, NULL)) {
      printf("--ERRO: pthread_create()\n"); exit(-1);
   }

   //--espera todas as threads terminarem
   for (int t=0; t<nthreads+1; t++) {
     if (pthread_join(tid[t], NULL)) {
         printf("--ERRO: pthread_join() \n"); exit(-1); 
     } 
   } 

   //--finaliza o mutex
   pthread_mutex_destroy(&mutex);
   pthread_cond_destroy(&cond);
   
   printf("Valor de 'soma' = %ld\n", soma);

   return 0;
}
