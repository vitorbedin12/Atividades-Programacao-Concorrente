#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char** argv) {

  // Tamanho dos vetores
  long int tamanhoVetores; 

  // Vetores a serem operados 
  float* vetor1;
  float* vetor2;

  // Produto interno dos vetores
  float produtoInterno = 0.0;

  // Descritor do arquivo  
  FILE* descritorArquivo;

  // Testa quantidade de argumentos
  if (argc < 3 || argc > 4) {
    printf("! Erro - número de argumentos de linha incorreto.\n");
    printf("Uso: %s <tamanho dos vetores> <nome do arquivo de saída> <flag 'l' (opcional)>\n", argv[0]);
    exit(1);
  }

  // Pega tamanho dos vetores da entrada
  tamanhoVetores = atoi(argv[1]);

  // Aloca espaço para vetores
  vetor1 = (float*) malloc(sizeof(float) * tamanhoVetores);
  vetor2 = (float*) malloc(sizeof(float) * tamanhoVetores);

  // Preenche os vetores com números aleatórios
  srand(time(NULL));
  float elem;
  for(long int i = 0; i < tamanhoVetores; i++) {

    elem = (rand() * 100.0 / RAND_MAX);
    vetor1[i] = elem;

    elem = (rand() * 100.0 / RAND_MAX);
    vetor2[i] = elem;

    // Calcula o produto interno simultaneamente
    produtoInterno += vetor1[i] * vetor2[i];
  }
  
  // Imprime na saída principal o número de vetores se a opção de log foi ativada
  if (argc > 3) {
    if (argv[3][0] == 'l' && argv[3][1] == '\0') {
      printf("Tamanho dos vetores: %ld\n", tamanhoVetores);
      printf("Vetor 1:\n[");
      for (long int i = 0; i < tamanhoVetores-1; i++){
        printf("%f, ", vetor1[i]);
      }
      printf("%f]\n", vetor1[tamanhoVetores-1]);
      printf("Vetor 2:\n[");
      for (long int i = 0; i < tamanhoVetores-1; i++){
        printf("%f, ", vetor2[i]);
      }
      printf("%f]\n", vetor2[tamanhoVetores-1]);
      printf("Produto interno: %f\n", produtoInterno);
    }
    else {
      printf("! Erro - uso de argumentos de linha incorreto.\n");
      printf("Uso: %s <tamanho dos vetores> <nome do arquivo de saída> <flag 'l' (opcional)>\n", argv[0]);
      exit(2);
    }
  } 

  // Abre arquivo de escrita
  descritorArquivo = fopen(argv[2], "wb");
  if (!descritorArquivo) {
    printf("! Erro na abertura do arquivo\n");
    exit(3);
  }

  // Escreve tamanho dos vetores no arquivo
  int teste = fwrite(&tamanhoVetores, sizeof(long int), 1, descritorArquivo);
  if (!teste) {
    printf("! Erro na escrita do arquivo\n");
    exit(4);
  }
  
  // Escreve vetor 1 no arquivo
  teste = fwrite(vetor1, sizeof(float), tamanhoVetores, descritorArquivo);
  if (teste != tamanhoVetores) {
    printf("! Erro na escrita do arquivo\n");
    exit(5);
  }

  // Escreve vetor 2 no arquivo
  teste = fwrite(vetor2, sizeof(float), tamanhoVetores, descritorArquivo);
  if (teste != tamanhoVetores) {
    printf("! Erro na escrita do arquivo\n");
    exit(6);
  }

  // Escreve produto interno no arquivo
  teste = fwrite(&produtoInterno, sizeof(float), 1, descritorArquivo);
  if (!teste) {
    printf("! Erro na escrita do arquivo\n");
    exit(7);
  }

  // Libera memória alocada e finaliza o programa
  free(vetor1);
  free(vetor2);
  fclose(descritorArquivo);
  return 0;
}
