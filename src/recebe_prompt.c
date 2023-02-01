#include "../headers/recebe_prompt.h"

// Recebe uma linha da entrada padrao e a copia para prompt
// Retorna -1 caso a linha seja vazia
int recebe_prompt(char* prompt) {
    printf("acsh> ");
    
    fgets(prompt, TAMANHO_PROMPT, stdin);
    if(strcmp(prompt, "\n") == 0)
        return -1;

    return 0;
}