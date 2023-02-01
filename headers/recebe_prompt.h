#ifndef RECEBE_PROMPT_H
#define RECEBE_PROMPT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constantes.h"

// Recebe uma linha da entrada padrao e a copia para prompt
// Retorna -1 caso a linha seja vazia
int recebe_prompt(char* prompt);

#endif
