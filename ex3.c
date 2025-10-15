#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

sem_t podeConsumir, vazio, lock;
int* buffer;
int N_CONSUMIDORAS = 10;
int M, N;

int ehPrimo(long long int n) {
  int i;
  if (n <= 1) return 0;
  if (n == 2) return 1;
  if (n % 2 == 0) return 0;
  for (i = 3; i < sqrt(n) + 1; i += 2) 
    if (n % i == 0) return 0;
  return 1;
}

void* produtora(void* arg) {
  long long int aInserir = 0;
  while (1) {
    if (aInserir >= N) break;
    sem_wait(&vazio);
    sem_wait(&lock);
    for (int i = 0; i < M; i++) {
      buffer[i] = aInserir;
      aInserir++;
    }
    sem_post(&lock);
    for (int i = 0; i < M; i++) {
      sem_post(&podeConsumir);
    }
  } 
  pthread_exit(NULL);
}

int fim = 0;
void* consumidora(void* arg) {
  static int out = 0;
  int* primos = malloc(sizeof(int));
  *primos = 0;
  while (1) {
    sem_wait(&podeConsumir);
    if (fim) break;
    long long int lido;
    sem_wait(&lock);
    lido = buffer[out];
    if (lido == N - 1) {
      fim++; 
      for (int i = 0; i < N_CONSUMIDORAS; i++){
        sem_post(&podeConsumir);
      }
    }
    out++;
    if (out == M) {
      out = 0;
      sem_post(&vazio);
    }
    sem_post(&lock);
    int primalidade = ehPrimo(lido);
    sem_wait(&lock);
    *primos += primalidade;
    sem_post(&lock);
  }
  pthread_exit((void*) primos);
}

int main(int argc, char** argv) {
  sem_init(&podeConsumir, 0, 0);
  sem_init(&vazio, 0, 1);
  sem_init(&lock, 0, 1);

  if (argc != 3) {
    printf("Número incorreto de argumentos.\nUso: %s <tamanho da sequencia> <tamanho do canal>\n", argv[0]);
    exit(1);
  }
  N = atoi(argv[1]);
  M = atoi(argv[2]);
  buffer = malloc(sizeof(int) * M);

  pthread_t tids[N_CONSUMIDORAS + 1];
  pthread_create(&tids[0], NULL, produtora, NULL);
  for (int i = 0; i < N_CONSUMIDORAS; i++) {
    pthread_create(&tids[i+1], NULL, consumidora, NULL);
  }
  pthread_join(tids[0], NULL);

  int primosTotais = 0;
  int vencedora = 0;
  int maxPrimos = -1;
  for (int i = 0; i < N_CONSUMIDORAS; i++) {
    int* primos = malloc(sizeof(int));
    pthread_join(tids[i+1], (void**) &primos);
    primosTotais += *primos;
    if (*primos > maxPrimos) {
      maxPrimos = *primos;
      vencedora = i;
    }
    printf("Thread %d contou %d primos\n", i, *primos);
  }
  printf("Ao todo, %d primos foram encontrados\n", primosTotais);
  printf("A thread %d venceu com uma contagem de %d primos\n", vencedora, maxPrimos);
}
