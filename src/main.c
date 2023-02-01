#include "../headers/recebe_prompt.h"
#include "../headers/executa_prompt.h"
#include "../headers/processa_prompt.h"

char prompt[TAMANHO_PROMPT];
char vetor_comandos[QTD_COMANDOS_MAXIMA+1][TAMANHO_MAXIMO_COMANDO];
pid_t buffer_secoes[TAMANHO_BUFFER_SECOES];
int posicao_buffer_secoes = 0;

// Handler para os sinais SIGINT, SIGQUIT e SIGTSTP do processo principal
void handler(int signo) {
    switch (signo) {
        case SIGINT:
            printf("\nNao adianta me enviar o sinal por Ctrl-C. Estou vacinado!!\n");
            break;
        case SIGQUIT:
            printf("\nNao adianta me enviar o sinal por Ctrl-\\. Estou vacinado!!\n");
            break;
        case SIGTSTP:
            printf("\nNao adianta me enviar o sinal por Ctrl-Z. Estou vacinado!!\n");
            break;
        default:
            printf("\nNao deveria ter recebido este sinal aqui (%d) - handler() [main.c]\n", signo);
            exit(1);
    }
}

int main(int argc, char** argv, char** envp) { 
    struct sigaction sa;
    sa.sa_handler = handler;
    sa.sa_flags = SA_RESTART;
    sigfillset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
    sigaction(SIGTSTP, &sa, NULL);
    
    while(1) {
        int prompt_return_value = recebe_prompt(prompt);
        
        if(prompt_return_value == 0) {
            int processa_prompt_return_value = processa_prompt(prompt, vetor_comandos);
             
            if(processa_prompt_return_value == 0)
                executa_prompt(vetor_comandos, buffer_secoes, &posicao_buffer_secoes);
        }
    }
    
    return 0;
}