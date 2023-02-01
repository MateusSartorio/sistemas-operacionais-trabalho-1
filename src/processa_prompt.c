#include "../headers/processa_prompt.h"

// Mesmo funcionamento da funcao strtok() da biblioteca padrao
// No entanto, strtok2() so quebra a string str em tokens quando encontra todo o delimitador, ao inves que qualquer um de seus caracteres
static char* strtok2(char* str, char* delimitador) {
    static bool chegou_no_final = false;
    static char* str_estatica = NULL;
    static char* p = NULL;
    char* p2 = NULL;


    if(delimitador == NULL || (strcmp(delimitador, "") == 0) ) {
        printf("Delimitador nao pode ser nulo ou uma string vazia - strtok2().\n");
        return NULL;
    }

    if(str != NULL) {
        str_estatica = str;
        p = str;
        chegou_no_final = false;
    }

    if( (str == NULL) && (str_estatica == NULL) ) {
        printf("str nao pode ser nula na primeira invocacao de strtok2().\n");
        return NULL;
    }

    if(chegou_no_final)
        return NULL;
        
    p2 = p;

    while(*p2 != '\0') {
        if(*p2 == delimitador[0]) {
            int i;
            for(i = 1; i < strlen(delimitador); i++)
                if(*(p2 + i) != delimitador[i])
                    break;

            if(i == strlen(delimitador)) {
                for(int j = 0; j < strlen(delimitador); j++)
                    *(p2 + j) = '\0';

                char* p_temp = p;
                p = p2 + strlen(delimitador);

                if(*p == '\0')
                    chegou_no_final = true;

                return p_temp;                
            }
        }
        p2++;
    }

    chegou_no_final = true;
    return p;
}

// Remove espaços em branco no comeco da string e espacoes em branco ou quebra de linha do final
// Copia a string sem espacoes em destino
// Não altera a string original
static void trim(char* origem, char* destino) {
    if(!origem || strlen(origem) == 0) {
        perror("String de origem nula ou vazia recebida - trim().\n");
        exit(1);
    }

    unsigned i = 0;
    while(origem[i] == ' ' || origem[i] == '\n')
        i++;

    unsigned j = strlen(origem) - 1;
    while(origem[j] == ' ' || origem[j] == '\n')
        j--;
    
    int k = 0;
    for(int l = i; l <= j; l++)
        destino[k++] = origem[l];
    destino[k] = '\0';
}

// Processa a string prompt, e preenche o vetor_comandos com cada um dos comandos individuais
// O vetor de comandos eh terminado pela string literal "NULL"
// Retorna -1 caso algo de errado, e 0 caso contrario
int processa_prompt(char* prompt, char vetor_comandos[][TAMANHO_MAXIMO_COMANDO]) {
    char* token = strtok2(prompt, "<3");
    int i = 0;
    while(token) {
        trim(token, vetor_comandos[i]);
        i++;
        
        // Verifica se mais de 5 comandos foram passados
        if(i > 5) {
            printf("Por favor, digite no maximo 5 comandos por vez.\n");
            return -1;
        }

        token = strtok2(NULL, "<3");
    }
    
    strcpy(vetor_comandos[i], "NULL");

    return 0;
}