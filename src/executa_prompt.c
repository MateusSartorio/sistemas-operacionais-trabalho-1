#include "../headers/executa_prompt.h"

// Handler para o sinal SIGUSR1, que mata todos os sinais do mesmo grupo do processo que recebeu o sinal
static void mata_todos_processos_do_grupo(int signo) {
    killpg(0, SIGKILL);
}

// Fecha todos os processos em background e terminal o acsh
static void fecha_tudo_e_sai(char* comando, pid_t* buffer_secoes, int* posicao_buffer_secoes) {
    char* token = strtok(comando, " ");
    token = strtok(NULL, " ");

    if(token) {
        printf("O comando exit nao pode receber argumentos.\n");
        return;
    }

    for(int i = 0; i < *posicao_buffer_secoes; i++)
        killpg(buffer_secoes[i], SIGKILL);

    exit(0);
}

// Troca o diretorio do acsh
static void troca_diretorio(char* caminho) {
    char* token = strtok(caminho, " ");
    token = strtok(NULL, " ");
    
    if(strtok(NULL, "") != NULL)
        printf("Por favor passe apenas um argumento para cd.\n");
    else if(token == NULL)
        printf("Por favor passe pelo menos um argumento para cd.\n");
    else {    
        int chdir_return_value = chdir(token);

        if(chdir_return_value == -1)
            perror("Erro ao trocar de diretorio: ");
    }
}

// Cria um novo processo e o executa em foreground
// A mascara de sinais do novo processo criado eh mudadad para o tratamento padrao
static void processo_em_foreground(char* comando) {
    pid_t pid = fork();
    
    if(pid == 0) {
        struct sigaction new_sa;

        new_sa.sa_handler = SIG_DFL;
        new_sa.sa_flags = SA_RESTART;
        sigfillset(&new_sa.sa_mask);
        sigaction(SIGINT, &new_sa, NULL);
        sigaction(SIGQUIT, &new_sa, NULL);
        sigaction(SIGTSTP, &new_sa, NULL);

        // Quantidade maxima de argumentos + o proprio nome do programa + NULL para sinalizar fim do vetor
        char* argv[QTD_MAXIMA_ARGUMENTOS + 2];
        
        int i = 0;
        char* token = strtok(comando, " ");
        while(token) {
            if(strcmp(token, "%") == 0)
                break;

            argv[i] = token;
            i++;

            if(i > 4)
                exit(1);

            token = strtok(NULL, " ");
        }
        argv[i] = NULL;

        execvp(argv[0], argv);

        exit(2);
    }
    else {
        int status = 0;
        while(wait(&status) != -1) {
            if(WIFEXITED(status)) {
                if(WEXITSTATUS(status) == 1)
                    printf("Muitos argumentos passados para programa, o maximo eh 3.\n");
                else if(WEXITSTATUS(status) == 2)
                    printf("Falha ao executar o comando: <%s>.\n", comando);
            }
        }
    }
}

// Executa um comando em background
static void executa_comando(char* comando) {
    // Quantidade maxima de argumentos + o proprio nome do programa + NULL para sinalizar fim do vetor
    char* argv[QTD_MAXIMA_ARGUMENTOS + 2];
    
    int i = 0;
    char* token = strtok(comando, " ");
    while(token) {
        argv[i] = token;
        i++;

        if(i > 4)
            exit(1);

        token = strtok(NULL, " ");
    }
    argv[i] = NULL;
    
    int dev_null = open("/dev/null", O_CREAT | O_WRONLY);
    if(dup2(dev_null, STDOUT_FILENO) == -1) perror("");
    // dup2(dev_null, STDERR_FILENO);
    execvp(argv[0], argv);
    
    exit(2);
}

// Cria uma nova secao, e cria novos processos para todos os programas que executarao nesse secao
// Estes programas rodarao em background e nao terao acesso ao terminal
static void cria_nova_secao(char vetor_comandos[][TAMANHO_MAXIMO_COMANDO], pid_t* buffer_secoes, int* posicao_buffer_secoes) {
    pid_t pid = fork();
    
    if(pid == -1) {
        // Caso ocorra erro no fork
        printf("Erro ao realizar o fork - cria_nova_secao().\n");
        exit(1);
    }
    else if(pid == 0) {
        // Filho executa        
        int setsid_return = setsid();
        if(setsid_return == -1) {
            printf("Erro ao criar nova secao - cria_nova_secao().\n");
            exit(1);
        }
        int qtd_processos = 0;
        while(strcmp(vetor_comandos[qtd_processos], "NULL") != 0)
            qtd_processos++;
        
        if(qtd_processos == 1) {
            struct sigaction sa;
            sa.sa_handler = SIG_IGN;
            sa.sa_flags = SA_RESTART;
            sigfillset(&sa.sa_mask);
            sigaction(SIGUSR1, &sa, NULL);
        } 
        else if(qtd_processos > 1) {
            struct sigaction sa;
            sa.sa_handler = mata_todos_processos_do_grupo;
            sa.sa_flags = SA_RESTART;
            sigfillset(&sa.sa_mask);
            sigaction(SIGUSR1, &sa, NULL);
        }
        else
            exit(0);

        int i = 0;
        pid_t pid_comando_individual = 0;
        while(strcmp(vetor_comandos[i], "NULL") != 0) {
            pid_comando_individual = fork();
            
            if(pid_comando_individual == 0)
                executa_comando(vetor_comandos[i]);
            
            i++;
        }
        
        int status = 0;
        while(wait(&status) != -1)
            if(WIFSIGNALED(status))
                if(WTERMSIG(status) == SIGUSR1)
                    raise(SIGUSR1);

        exit(0);
    }
    else {
        // Pai executa
        buffer_secoes[*posicao_buffer_secoes] = pid;
        *posicao_buffer_secoes += 1;
    }
}

// Executa o prompt recebido pelo asch
void executa_prompt(char vetor_comandos[][TAMANHO_MAXIMO_COMANDO], pid_t* buffer_secoes, int* posicao_buffer_secoes) {
    // Expressao regular responsavel por identificar se um processo deve ser executado em background
    regex_t foreground_regex;
    int foreground_regex_value = regcomp(&foreground_regex, " \%$", 0);
    if(foreground_regex_value) {
        printf("Problema ao compilar foreground_regex.\n");
        exit(foreground_regex_value);
    }

    // Expressao regular responsavel por identificar se o comando de exit foi passado
    regex_t exit_regex_1;
    int exit_regex_value_1 = regcomp(&exit_regex_1, "^exit$", 0);
    if(exit_regex_value_1) {
        printf("Problema ao compilar exit_regex_1.\n");
        exit(exit_regex_value_1);
    }

    // Expressao regular responsavel por identificar se o comando de exit foi passado
    regex_t exit_regex_2;
    int exit_regex_value_2 = regcomp(&exit_regex_2, "^exit ", 0);
    if(exit_regex_value_2) {
        printf("Problema ao compilar exit_regex_2.\n");
        exit(exit_regex_value_2);
    }

    // Expressao regular responsavel por identificar se o comando cd foi passado
    regex_t cd_regex_1;
    int cd_regex_value_1 = regcomp(&cd_regex_1, "^cd$", 0);
    if(cd_regex_value_1) {
        printf("Problema ao compilar cd_regex_1.\n");
        exit(cd_regex_value_1);
    }

    // Expressao regular responsavel por identificar se o comando cd foi passado
    regex_t cd_regex_2;
    int cd_regex_value_2 = regcomp(&cd_regex_2, "^cd ", 0);
    if(cd_regex_value_2) {
        printf("Problema ao compilar cd_regex_2.\n");
        exit(cd_regex_value_2);
    }
    
    // Verifica se existem comandos de exit, cd ou foreground
    bool tem_exit = false, tem_foreground = false, tem_cd = false;
    int i = 0;
    while(strcmp(vetor_comandos[i], "NULL") != 0) {
        if(!regexec(&foreground_regex, vetor_comandos[i], 0, NULL, 0))
            tem_foreground = true;
        if( (regexec(&exit_regex_1, vetor_comandos[i], 0, NULL, 0) == 0) || (regexec(&exit_regex_2, vetor_comandos[i], 0, NULL, 0) == 0) )
            tem_exit = true;
        if( (regexec(&cd_regex_1, vetor_comandos[i], 0, NULL, 0) == 0) || (regexec(&cd_regex_2, vetor_comandos[i], 0, NULL, 0) == 0) )
            tem_cd = true;

        i++;
    }
    
    // Libera a memoria na heap alocada para as expressoes regulares
    regfree(&foreground_regex);
    regfree(&exit_regex_1);
    regfree(&exit_regex_2);
    regfree(&cd_regex_1);
    regfree(&cd_regex_2);

    // Verifica qual tipo de comando deve ser executado, e no caso de processo em foregound altera o tratamento de sinais do processo principal
    if(i > 1 && (tem_exit || tem_foreground || tem_cd))
        printf("A operacao de exit, cd ou comandos com o operador de foreground (%%) devem ser comandos unicos.\n");
    else if(tem_exit && tem_foreground)
        printf("A operacao de exit nao pode ser usada junto com o operador de foreground (%%).\n");
    else if(tem_cd && tem_foreground)
        printf("A operacao de cd nao pode ser usada junto com o operador de foreground (%%).\n");
    else if(tem_exit)
        fecha_tudo_e_sai(vetor_comandos[0], buffer_secoes, posicao_buffer_secoes);
    else if(tem_foreground) {
        struct sigaction new_sa, old_sa;

        new_sa.sa_handler = SIG_IGN;
        new_sa.sa_flags = SA_RESTART;
        sigfillset(&new_sa.sa_mask);
        sigaction(SIGINT, &new_sa, &old_sa);
        sigaction(SIGQUIT, &new_sa, &old_sa);
        sigaction(SIGTSTP, &new_sa, &old_sa);
        
        processo_em_foreground(vetor_comandos[0]);
        
        sigaction(SIGINT, &old_sa, NULL);
        sigaction(SIGQUIT, &old_sa, NULL);
        sigaction(SIGTSTP, &old_sa, NULL);
    }
    else if(tem_cd)
        troca_diretorio(vetor_comandos[0]);
    else
        cria_nova_secao(vetor_comandos, buffer_secoes, posicao_buffer_secoes); 
}