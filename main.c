#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

#define TAMANHO_TABELA_HASH 10124

struct palavra
{
    char *chave;
};

typedef struct palavra Palavra;

/**
 * FUNCOES
 */
int getch(void);

int getche(void);

bool carregarDicionario(char *p_nomeArquivo, Palavra *p_tabelaHash, int p_tamanhoPrefixo);

bool usuarioApertouEnter(char p_caracter);

bool usuarioApertouBackspace(char p_caracter);

bool numeroLetrasIncorreto(int p_numeroLetras, int p_numeroPermitido);

int funcaoHash(char *p_prefixo, int p_fator);

int main(int p_argc,char **p_argv)
{
    if(p_argc < 3)
    {
        printf("O comando não executado corretamente. \nFavor informar o nome do arquivo de Dicionário e número de letras para indexação");
        return EXIT_FAILURE;
    }
    printf("==============================\n");
    printf("\tAutocomplete\n");
    printf("==============================\n");

    int v_tamanhoPrefixo = atoi(p_argv[2]);
    printf("Tamanho prefixo para indexar: %d\n", v_tamanhoPrefixo);

    //Crinado tabela Hash
    Palavra *tabelaHash = malloc(sizeof(Palavra) * TAMANHO_TABELA_HASH);
    Palavra v_ola;
    v_ola.chave = "Ola";
    tabelaHash[0] = v_ola;
    carregarDicionario(p_argv[1], tabelaHash, v_tamanhoPrefixo);


    int v_numeroMaximoIndexacao = TAMANHO_TABELA_HASH - 1;
    int v_numeroLetras = 0;
    bool v_usuarioNaoApertouEnter = true;
    char v_char;
    char v_busca[v_numeroMaximoIndexacao];
    v_busca[v_numeroMaximoIndexacao] = '\0'; // Removendo caracter que identificar uma string ('@')

    while(v_usuarioNaoApertouEnter)
    {
        v_char = getch();
        if(usuarioApertouEnter(v_char))
        {
            v_usuarioNaoApertouEnter = false;
            printf("\n\nFim\n");
        }
        else
        {
            if(numeroLetrasIncorreto(v_numeroLetras, v_numeroMaximoIndexacao) && !usuarioApertouBackspace(v_char))
            {
                printf("\n\nSem sugestoes...");
            }
            else
            {
                if(usuarioApertouBackspace(v_char))
                {
                    if(v_numeroLetras > 0)
                    {
                        v_busca[v_numeroLetras - 1] = '\0';
                        v_numeroLetras--;
                    }
                }
                else
                {
                    v_busca[v_numeroLetras] = v_char;
                    v_numeroLetras++;
                }
            }
            printf("%s\n\n", v_busca);
        }
    }
    
    return 0;
}

/**
 * Função implementada para conseguir pegar o char sem precisar apertar a tecla Enter
/* reads from keypress, doesn't echo */
int getch(void)
{
    struct termios oldattr, newattr;
    int ch;
    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    return ch;
}

/* reads from keypress, echoes */
int getche(void)
{
    struct termios oldattr, newattr;
    int ch;
    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON );
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    return ch;
}

/**
* Função que identifica quando o usuário apertou a tecla enter
*/
bool usuarioApertouEnter(char p_caracter)
{
    if(p_caracter == '\n')
    {
        return true;
    }
    return false;
}

/**
* Função que identifica quando o usuário apertou a tecla Backspace
*/
bool usuarioApertouBackspace(char p_caracter)
{
    if((int)p_caracter == 127)
    {
        return true;
    }
    return false;
}

/**
 * Função que identifica caso o usuário digitou mais letras do que o permitido 
 */
bool numeroLetrasIncorreto(int p_numeroLetras, int p_numeroPermitido)
{
    if(p_numeroLetras > p_numeroPermitido)
    {
        return true;
    }
    return false;
}


/**
 * Função que carrega o dicionário de palávras 
 */
bool carregarDicionario(char *p_nomeArquivo, Palavra *p_tabelaHash, int p_tamanhoPrefixo)
{
    printf("%s", p_tabelaHash[0].chave);
    printf("\n[Início] Carregando dicionário...\n");
    
    FILE *v_dicionario = fopen(p_nomeArquivo, "r");
    char v_palavra [100];
    if(v_dicionario == NULL)
    {
        printf("Erro ao encontrar dicionário de palavras.");
        return EXIT_FAILURE;
    }

//subbuff[4] = '\0';
    printf("\n\nPalavras: \n");
    printf("========");
    
    while(fscanf(v_dicionario, "%s", v_palavra) != EOF)
    {
        //printf("\n%s", v_palavra);
        if(p_tamanhoPrefixo >= strlen(v_palavra))
        {
            char v_aux[strlen(v_palavra)];
            memcpy(v_aux, v_palavra, strlen(v_palavra));
            printf("\nPrefixo encontrado: %s ", v_aux);
            funcaoHash(v_aux, strlen(v_palavra));
        }
        else 
        {
            char v_prefixo[p_tamanhoPrefixo];
            memcpy( v_prefixo, v_palavra, p_tamanhoPrefixo );
            printf("\nIgual Prefixo encontrado: %s ", v_prefixo);
            funcaoHash(v_prefixo,p_tamanhoPrefixo);
        }
        //strncpy(v_prefixo,v_palavra , 4);
        //strcpy(v_prefixo, v_palavra);
        
        
    }
    printf("\n\n");
    fclose(v_dicionario);

    printf("\n[Fim] Carregando dicionário...\n");
}

int funcaoHash(char *p_prefixo, int p_fator)
{
    int v_hash = 0;
    //printf("\nCalculando Hash");
    int v_tamanho = strlen(p_prefixo);
    int v_indice;
    int v_decresce = p_fator;
    for(v_indice = 0; v_indice < v_tamanho; v_indice++)
    {
        //printf("\nChar :  %c\n", p_prefixo[v_indice]);
        v_hash += (v_decresce*v_decresce*v_decresce)*( ((int)p_prefixo[v_indice]) / (p_fator) );
        v_decresce--;
    }
    printf(" Funcao Hash calculada: %d\n", v_hash);
    return v_hash;
}