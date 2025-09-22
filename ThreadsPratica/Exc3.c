#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>


// Lê o conteúdo do arquivo filename e retorna um vetor E o tamanho dele
// Se filename for da forma "gen:%d", gera um vetor aleatório com %d elementos
//
// +-------> retorno da função, ponteiro para vetor malloc()ado e preenchido
// | 
// |         tamanho do vetor (usado <-----+
// |         como 2o retorno)              |
// v                                       v
double* load_vector(const char* filename, int* out_size);


// Avalia se o prod_escalar é o produto escalar dos vetores a e b. Assume-se
// que ambos a e b sejam vetores de tamanho size.
void avaliar(double* a, double* b, int size, double prod_escalar);

typedef struct {
    int inicio;
    int fim;
    double *vetor_a;
    double *vetor_b;
} thread_args;


void* operacaoProdutoEscalar (void* arg){

    double resultadoConta = 0;

    //convertendo o argumento recebido pela função (que é um void*) para um ponteiro de thread_args
    thread_args *args = (thread_args *) arg;
    
    //criando um ponteiro e alocando um espaço da memoria para ele (tem que fazer isso sempre)
    double* resultadoFuncao = malloc(sizeof(double));

    //vai repetir a soma de a com b do lugar de inicio até o lugar fim do vetor (a area que essa thread é responsavel)
    //e somar isso a variavel local "resultadoConta"
    for (int i = args->inicio; i < args->fim; i++ ){
    
        //fazendo a conta   
        resultadoConta += args->vetor_a[i] * args->vetor_b[i];
    }

    //fazendo o ponteiro apontar para o resultado
    *resultadoFuncao = resultadoConta;

    //retornando um ponteiro void
    return (void*) resultadoFuncao;
}


int main(int argc, char* argv[]) {
    srand(time(NULL));

    //Temos argumentos suficientes?
    if(argc < 4) {
        printf("Uso: %s n_threads a_file b_file\n"
               "    n_threads    número de threads a serem usadas na computação\n"
               "    *_file       caminho de arquivo ou uma expressão com a forma gen:N,\n"
               "                 representando um vetor aleatório de tamanho N\n", 
               argv[0]);
        return 1;
    }
  
    //Quantas threads?
    int n_threads = atoi(argv[1]);
    if (!n_threads) {
        printf("Número de threads deve ser > 0\n");
        return 1;
    }

    //Lê números de arquivos para vetores alocados com malloc
    int a_size = 0, b_size = 0;
    double* a = load_vector(argv[2], &a_size);
    if (!a) {
        //load_vector não conseguiu abrir o arquivo
        printf("Erro ao ler arquivo %s\n", argv[2]);
        return 1;
    }

    double* b = load_vector(argv[3], &b_size);
    if (!b) {
        printf("Erro ao ler arquivo %s\n", argv[3]);
        return 1;
    }
    
    //Garante que entradas são compatíveis
    if (a_size != b_size) {
        printf("Vetores a e b tem tamanhos diferentes! (%d != %d)\n", a_size, b_size);
        return 1;
    }
    
    pthread_t thread[n_threads];
    thread_args argumentos[n_threads];

    //garante que no max serão criadas uma thread por [i]
    if (n_threads > a_size){
        n_threads = a_size;
    }



    //Parte que eu fiz: paralelização do produto escalar
    double result = 0;
    double* retornoDaThread;
    
    

    for (int i = 0; i < n_threads; ++i){
    
        //cada thread recebe o lugar que vai começar e terminar
        //isso é calculado divindo o tamanho do vetor pela qtd de threads
        argumentos[i].inicio = i *((int)a_size/n_threads); 
        argumentos[i].fim = (i + 1)*((int)a_size/n_threads);

        //aqui faz uma checagem se o número de itens do vetor é divisível pelo número de threads e se é a última iteração do for
        //caso o número seja ímpar, a lógica anterior não funcionaria, daí a gente usa esse if pra somar ao "fim" a qtd de casas que falta
        //exemplo: 11 itens no vetor, 2 threads. a lógica acima vai separar de 0-5 p/ primeira thread e de 5 até 10 para a segunda. 
        //com o if, multiplicamos o numero de threads pela parte decimal da divisão a_size/n_threads, ou seja, 2 * 0,5 e somamos isso ao fim
            if ((double)a_size/n_threads != (int)a_size/n_threads && i == n_threads -1){
                argumentos[i].fim += n_threads*((double)a_size/n_threads - (int)a_size/n_threads);
            }        

        //todas as threads vão receber o vetor inteiro
        argumentos[i].vetor_a = a;
        argumentos[i].vetor_b = b;

    pthread_create(&thread[i], NULL, operacaoProdutoEscalar, (void *) &argumentos[i]); //converter inteiro para void * pq o tipo de ponteiro q ele quer  
    }

    //Aqui o programa espera todas essas threads acabarem para poder finalizar
    for (int i = 0; i < n_threads; ++i){
        
        //o valor que a thread retorna está sendo alocado a variavel retornoDaThread
        pthread_join(thread[i], (void *)&retornoDaThread); 
        result += *retornoDaThread;
        free(retornoDaThread);
    }

    //quando você aloca um espaço na memória tem que liberar ele depois
    //muito importante fazer isso TODA vez que voê usa => free() deveestar dentro do for
   
   
 
    //    +---------------------------------+
    // ** | IMPORTANTE: avalia o resultado! | **
    //    +---------------------------------+
    avaliar(a, b, a_size, result);

    //Libera memória
    free(a);
    free(b);

    return 0;
}
