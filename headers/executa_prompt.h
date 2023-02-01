#ifndef EXECUTA_PROMPT_H
#define EXECUTA_PROMPT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <regex.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>

#include "constantes.h"

// Executa o prompt recebido pelo asch
void executa_prompt(char vetor_comandos[][TAMANHO_MAXIMO_COMANDO], pid_t* buffer_secoes, int* posicao_buffer_secoes);

#endif
