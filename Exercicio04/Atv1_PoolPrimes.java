/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Laboratório: 11 */
/* Codigo: Criando um pool de threads em Java */

import java.util.LinkedList;
import java.util.Scanner;

//-------------------------------------------------------------------------------
//!!! Documentar essa classe !!!
class FilaTarefas {
    private final int nThreads;
    private final MyPoolThreads[] threads;
    private final LinkedList<Runnable> queue;
    private boolean shutdown;

    public FilaTarefas(int nThreads) {
        this.shutdown = false;
        this.nThreads = nThreads;
        queue = new LinkedList<Runnable>();
        threads = new MyPoolThreads[nThreads];
        for (int i=0; i<nThreads; i++) {
            threads[i] = new MyPoolThreads();
            threads[i].start();
        } 
    }

    public void execute(Runnable r) {
        synchronized(queue) {
            if (this.shutdown) return;
            queue.addLast(r);
            queue.notify();
        }
    }
    
    public void shutdown() {
        synchronized(queue) {
            this.shutdown=true;
            queue.notifyAll();
        }
        for (int i=0; i<nThreads; i++) {
          try { threads[i].join(); } catch (InterruptedException e) { return; }
        }
    }

    private class MyPoolThreads extends Thread {
       public void run() {
         Runnable r;
         while (true) {
           synchronized(queue) {
             while (queue.isEmpty() && (!shutdown)) {
               try { queue.wait(); }
               catch (InterruptedException ignored){}
             }
             if (queue.isEmpty()) return;   
             r = (Runnable) queue.removeFirst();
           }
           try { r.run(); }
           catch (RuntimeException e) {}
         } 
       } 
    } 
}
//-------------------------------------------------------------------------------

//--PASSO 1: cria uma classe que implementa a interface Runnable 
class Hello implements Runnable {
   String msg;
   public Hello(String m) { msg = m; }

   //--metodo executado pela thread
   public void run() {
      System.out.println(msg);
   }
}

class Primo implements Runnable {
   int num; 
   public Primo(int num) { this.num = num; }

   public void run() {
      if (num <= 1) {
         System.out.println(num + " não é primo");
         return;
      }

      if (num == 2) {
         System.out.println(num + " é primo");
         return;
      }

      if (num % 2 == 0) {
         System.out.println(num + " não é primo");
         return;
      }

      for (int i = 3; i < Math.sqrt(num) + 1; i += 2) {
         if (num % i == 0) {
            System.out.println(num + " não é primo");
            return;
         }
      }
      System.out.println(num + " é primo");
      return;
   }
}

//Classe da aplicação (método main)
class Atv1_PoolPrimes{
    private static int NTHREADS;
    private static int N;

    public static void main (String[] args) {
      // Pega NTHREADS e N
      Scanner scan = new Scanner(System.in);
      System.out.println("NTHREADS:");
      NTHREADS = scan.nextInt();
      System.out.println("N:");
      N = scan.nextInt();

      //--PASSO 2: cria o pool de threads
      FilaTarefas pool = new FilaTarefas(NTHREADS); 
      
      //--PASSO 3: dispara a execução dos objetos runnable usando o pool de threads
      for (int i = 0; i < N; i++) {
        final String m = "Hello da tarefa " + i;
        Runnable hello = new Hello(m);
        pool.execute(hello);
        Runnable primo = new Primo(i);
        pool.execute(primo);
      }

      //--PASSO 4: esperar pelo termino das threads
      pool.shutdown();
      System.out.println("Terminou");
   }
}
