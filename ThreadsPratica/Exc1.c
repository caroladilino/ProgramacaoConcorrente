#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>


int contador_global = 0;

void* funct(void* arg) {
    //convertendo o argumento recebido pela função (que é um void*) para um ponteiro de int
    int *n = (int *) arg; 
    
    //Realizando a quantidade necessária de iterações
    for (int i = 0; i < *n; i++){
        contador_global++;
    }      
    return NULL;
}

int main(int argc, char* argv[]) {
    
    //conferindo que foram inseridos a quantidade suficiente de argumentos
    if (argc < 3) {
        printf("n_threads é obrigatório!\n");
        printf("Uso: %s n_threads n_loops\n", argv[0]);
        return 1;
    }

    //inicializando as variáveis que vamos usar
    int n_threads = atoi(argv[1]);
    int n_loops = atoi(argv[2]);

    //criando um vetor de pthread_t com n_threads espaços para as quantidades de threads que vamos criar
    pthread_t thread[n_threads]; 

    //criando n_threads threads
    for (int i = 0; i < n_threads; ++i){
        pthread_create(&thread[i], NULL, funct, (void *) &n_loops); //converter inteiro para void 8 pq o tipo de ponteiro q ele quer  
    }

    //Aqui o programa espera todas essas threads acabarem para poder finalizar
    for (int i = 0; i < n_threads; ++i){
       pthread_join(thread[i], NULL); 
    }
}    
