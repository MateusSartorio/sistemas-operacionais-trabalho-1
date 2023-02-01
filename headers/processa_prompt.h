#ifndef PROCESSA_PROMPT
#define PROCESSA_PROMPT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <regex.h>

#include "constantes.h"

// Processa a string prompt, e preenche o vetor_comandos com cada um dos comandos individuais
// O vetor de comandos eh terminado pela string literal "NULL"
// Retorna -1 caso algo de errado, e 0 caso contrario
int processa_prompt(char* prompt, char vetor_comandos[][TAMANHO_MAXIMO_COMANDO]);

#endif
