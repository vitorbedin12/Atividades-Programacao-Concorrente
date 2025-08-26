#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <math.h>
#include "timer.h"

// Estrutura de dados dos argumentos
typedef struct {
  float* vetor1;
  float* vetor2;
  long int inicio;
  long int fim;
} Args_t;

// Função executada pelas threads
void* produtoInternoParcial(void* arg) {
  Args_t* args = (Args_t*) arg;
  
  float* resultado = malloc(sizeof(float));
  if (resultado == NULL) {
    printf("! Erro - malloc() resutlado\n");
    exit(-1);
  }
  *resultado = 0;

  for (long int i = args->inicio; i < args->fim; i++) {
      *resultado += (args->vetor1)[i] * (args->vetor2)[i];
  }

  pthread_exit((void*) resultado);
}

// Função principal
int main(int argc, char** argv) {
  
  // Tamanho dos vetores
  long int tamanhoVetores;

  // Vetores a serem operados
  float* vetor1;
  float* vetor2;
  
  // Produto interno sequencial (no arquivo)
  float produtoInternoSeq;

  // Produto interno concorrente
  float produtoInternoConc;

  // Descritor do arquivo de leitura
  FILE* descritorArquivo;

  // Número de threads (lido da entrada)
  int numThreads;

  // Vetor de IDs das threads
  pthread_t* tids;

  // Medidas de tempo;
  double start, end, delta;
  
  // Testa número de argumentos
  if (argc < 3 || argc > 4) {
    printf("! Erro - quantidade de argumentos de linha de comando incorreta\n");
    printf("Uso: %s <arquivo de leitura> <número de threads> <flag 'l' (opcional)>\n", argv[0]);
    exit(1);
  }

  // Abre arquivo de leitura
  descritorArquivo = fopen(argv[1], "rb");
  if (!descritorArquivo) {
    printf("! Erro na abertura do arquivo\n");
    exit(2);
  }

  // Lê tamanho dos vetores
  long int teste = fread(&tamanhoVetores, sizeof(long int), 1, descritorArquivo);
  if (!teste) {
    printf("! Erro na leitura do arquivo (tamanho)\n");
    exit(3);
  }

  // Aloca espaço para os vetores
  vetor1 = (float*) malloc(sizeof(float) * tamanhoVetores);
  vetor2 = (float*) malloc(sizeof(float) * tamanhoVetores);
  if (vetor1 == NULL || vetor2 == NULL) {
    printf("! Erro - malloc() vetores\n");
    exit(-1);
  }

  // Lê vetor 1
  teste = fread(vetor1, sizeof(float), tamanhoVetores, descritorArquivo);
  if (teste < tamanhoVetores) {
    printf("! Erro na leitura do arquivo (vetor 1): li apenas %ld\n", teste);
    exit(4);
  }

  // Lê vetor 2
  teste = fread(vetor2, sizeof(float), tamanhoVetores, descritorArquivo);
  if (teste < tamanhoVetores) {
    printf("! Erro na leitura do arquivo (vetor 2): li apenas %ld\n", teste);
    exit(5);
  }
  
  // Lê produto interno sequencial
  teste = fread(&produtoInternoSeq, sizeof(float), 1, descritorArquivo);
  if (!teste) {
    printf("! Erro na leitura do arquivo (produto interno sequencial)\n");
    exit(6);
  }

  // Imprime vetores se flag de log estiver ativa
  if (argc == 4) {
    if (argv[3][0] == 'l' && argv[3][1] == '\0') {

      printf("Tamanho dos vetores: %ld\n", tamanhoVetores);

      printf("Vetor 1: [");
      for (long int i = 0; i < tamanhoVetores-1; i++) {
        printf("%f, ", vetor1[i]);
      }
      printf("%f]\n", vetor1[tamanhoVetores-1]);

      printf("Vetor 2: [");
      for (long int i = 0; i < tamanhoVetores-1; i++) {
        printf("%f, ", vetor2[i]);
      }
      printf("%f]\n", vetor2[tamanhoVetores-1]);
    }
    else {
      printf("! Erro - uso incorreto dos argumentos de linha de comando\n");
      printf("Uso: %s <arquivo de leitura> <número de threads> <flag 'l' (opcional)>\n", argv[0]);
      exit(7);
    }
  }
  
  // Lê número de threads e cria vetor de tids
  numThreads = atoi(argv[2]);
  tids = malloc(sizeof(pthread_t) * numThreads);
  if (tids == NULL) {
    printf("! Erro - malloc() tids\n");
    exit(-1);
  }

  // Cria threads
  GET_TIME(start)
  for (int i = 0; i < numThreads; i++) {

    Args_t* arg = (Args_t*) malloc(sizeof(Args_t));
    if (arg == NULL) {
      printf("! Erro - malloc() Args_t\n");
      exit(-1);
    }
    arg->vetor1 = vetor1;
    arg->vetor2 = vetor2;
    arg->inicio = i * tamanhoVetores / numThreads;
    arg->fim = i == numThreads-1 ? tamanhoVetores : (i + 1) * tamanhoVetores / numThreads;

    if (pthread_create(&tids[i], NULL, produtoInternoParcial, (void*) arg)) {
      printf("! Erro - pthread_create() da thread %d\n", i);
      exit(8);
    }
  }

  // Obtem resultado das threads
  produtoInternoConc = 0;
  float* res = malloc(sizeof(float));
  if (res == NULL) {
    printf("! Erro - malloc() res\n");
    exit(-1);
  }

  for (int i = 0; i < numThreads; i++) {
      if (pthread_join(tids[i], (void*) &res)) {
      printf("! Erro - pthread_join() da thread %d\n", i);
        exit(9);
    }
    produtoInternoConc += *res;
  }
  GET_TIME(end)
  delta = end - start;

  // Mostra produtos internos sequencial e concorrente
  printf("Produto interno sequencial: %f\n", produtoInternoSeq);
  printf("Produto interno concorrente: %f\n", produtoInternoConc);

  // Mostra erro relativo
  double erroRelativo = fabs((produtoInternoSeq - produtoInternoConc)/produtoInternoSeq);
  printf("Erro relativo: %lf\n", erroRelativo);
  printf("Tempo: %lf\n", delta);
}

