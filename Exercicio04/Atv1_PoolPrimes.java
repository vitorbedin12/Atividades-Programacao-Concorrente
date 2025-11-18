/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Laboratório: 11 */
/* Codigo: Exemplo de uso de futures */
/* -------------------------------------------------------------------*/

import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

import java.util.ArrayList;
import java.util.List;


//classe runnable
class MyCallable implements Callable<Long> {
  //construtor
  MyCallable() {}
 
  //método para execução
  public Long call() throws Exception {
    long s = 0;
    for (long i=1; i<=100; i++) {
      s++;
    }
    return s;
  }
}

class ehPrimo implements Callable<Integer> {
   int num;
   ehPrimo (int num) {
      this.num = num;
   }

   public Integer call() throws Exception {
      if (num <= 1) return 0;
      if (num == 2) return 1;
      if (num % 2 == 0) return 0;
      for (int i = 3; i < Math.sqrt(num) + 1; i += 2) {
         if (num % i == 0) return 0;
      } 
      return 1;
   }
}

//classe do método main
public class FutureHello  {
  private static final int N = 100;
  private static final int NTHREADS = 10;

  public static void main(String[] args) {
    //cria um pool de threads (NTHREADS)
    ExecutorService executor = Executors.newFixedThreadPool(NTHREADS);
    //cria uma lista para armazenar referencias de chamadas assincronas
    List<Future<Long>> list = new ArrayList<Future<Long>>();

    //lista de futuros para checagem de primos
    List<Future<Integer>> primesList = new ArrayList<Future<Integer>>();

    for (int i = 0; i < N; i++) {
      Callable<Long> worker = new MyCallable();
      Callable<Integer> workerPrime = new ehPrimo(i+1);
      /*submit() permite enviar tarefas Callable ou Runnable e obter um objeto Future para acompanhar o progresso e recuperar o resultado da tarefa
       */
      Future<Long> submit = executor.submit(worker);
      list.add(submit);
      Future<Integer> submitPrime = executor.submit(workerPrime);
      primesList.add(submitPrime);
    }

    System.out.println(list.size());
    //pode fazer outras tarefas...

    //recupera os resultados e faz o somatório final
    long sum = 0;
    for (Future<Long> future : list) {
      try {
        sum += future.get(); //bloqueia se a computação nao tiver terminado
      } catch (InterruptedException e) {
        e.printStackTrace();
      } catch (ExecutionException e) {
        e.printStackTrace();
      }
    }
    System.out.println(sum);

    long primos = 0;
    for (Future<Integer> future : primesList) {
      try {
        primos += future.get();
      } catch (InterruptedException e) {
        e.printStackTrace();
      } catch (ExecutionException e) {
        e.printStackTrace();
      }
    }
    System.out.println("De 1 a " + N + " existem " + primos + " primos.");

    executor.shutdown();
  }
}
