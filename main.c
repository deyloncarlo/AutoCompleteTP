#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

#define TAMANHO_TABELA_HASH 3000

struct palavra
{
    char *chave;
    char **lista;
    int tamanhoLista;

};

typedef struct palavra Palavra;

/**
 * FUNCOES
 */
int getch(void);

int getche(void);

bool carregarDicionario(char *p_nomeArquivo, int p_tamanhoPrefixo);

bool usuarioApertouEnter(char p_caracter);

bool usuarioApertouBackspace(char p_caracter);

bool numeroLetrasIncorreto(int p_numeroLetras, int p_numeroPermitido);

int funcaoHash(char *p_prefixo, int p_fator);

void listarPalavras(int p_indice);

static Palavra *v_tabelaHash;

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
    v_tabelaHash = malloc(sizeof(Palavra) * TAMANHO_TABELA_HASH);
    carregarDicionario(p_argv[1], v_tamanhoPrefixo);


    int v_numeroMaximoIndexacao = v_tamanhoPrefixo - 1;
    int v_numeroLetras = 0;
    bool v_usuarioNaoApertouEnter = true;
    char v_char;
    char v_busca[v_numeroMaximoIndexacao];
    v_busca[v_tamanhoPrefixo] = '\0'; // Removendo caracter que identificar uma string ('@')
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
                    //printf("\nChar digitado: %c; Char na posicao: %c\n", v_char, v_busca[v_numeroLetras]);
                    v_busca[v_numeroLetras] = v_char;
                    char v_aux[strlen(v_busca)];
                    v_aux[strlen(v_busca)] = '\0';
                    memcpy(v_aux, v_busca, strlen(v_busca));
                    int v_indice = funcaoHash(v_aux, strlen(v_aux));
                    //Palavra v_palavra = v_tabelaHash[v_indice];
                    listarPalavras(v_indice);
                    //printf("\nPalavra encontrada: %s\n", v_palavra.chave);
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
bool carregarDicionario(char *p_nomeArquivo, int p_tamanhoPrefixo)
{
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
    int v_indiceTabelaHash;
    while(fscanf(v_dicionario, "%s", v_palavra) != EOF)
    {
        Palavra v_novaPalavra;
        v_novaPalavra.chave = (char *)malloc(p_tamanhoPrefixo);
        v_novaPalavra.lista = malloc(100 * sizeof(char*));
        if(p_tamanhoPrefixo >= strlen(v_palavra))
        {
            char v_aux[strlen(v_palavra)];
            v_aux[strlen(v_palavra)] = '\0';
            memcpy(v_aux, v_palavra, strlen(v_palavra));
            v_indiceTabelaHash = funcaoHash(v_aux, strlen(v_palavra));
            strcpy(v_novaPalavra.chave,v_aux);
        }
        else 
        {
            char v_prefixo[p_tamanhoPrefixo];
            v_prefixo[p_tamanhoPrefixo] = '\0';
            memcpy( v_prefixo, v_palavra, p_tamanhoPrefixo );
            v_indiceTabelaHash = funcaoHash(v_prefixo,p_tamanhoPrefixo);
            strcpy(v_novaPalavra.chave,v_prefixo);
        }
        if(v_tabelaHash[v_indiceTabelaHash].chave != NULL)
        {
            int v_tamanhoLista = v_tabelaHash[v_indiceTabelaHash].tamanhoLista;
            v_tabelaHash[v_indiceTabelaHash].lista[v_tamanhoLista] = malloc((100) * sizeof(char));
            strcpy(v_tabelaHash[v_indiceTabelaHash].lista[v_tamanhoLista], v_palavra);
            v_tabelaHash[v_indiceTabelaHash].tamanhoLista++;
        }
        else
        {
            v_novaPalavra.lista[0] = malloc((100) * sizeof(char));
            strcpy(v_novaPalavra.lista[0], v_palavra);
            v_novaPalavra.tamanhoLista = 1;
            v_tabelaHash[v_indiceTabelaHash] = v_novaPalavra;
        }

    }

    printf("\n\n");
    fclose(v_dicionario);
    printf("\n[Fim] Carregando dicionário...\n");
}

int funcaoHash(char *p_prefixo, int p_fator)
{
    int v_hash = 0;
    int v_tamanho = strlen(p_prefixo);
    int v_indice;
    int v_decresce = p_fator;
    for(v_indice = 0; v_indice < v_tamanho; v_indice++)
    {
        v_hash += (v_decresce*v_decresce*v_decresce)*( ((int)p_prefixo[v_indice]) / (p_fator) );
        v_decresce--;
    }
    printf(" Funcao Hash calculada: %d\n", v_hash);
    return v_hash;
}

void listarPalavras(int p_indice)
{
    if(v_tabelaHash[p_indice].chave != NULL)
    {
        int i;
        for(i = 0; i < v_tabelaHash[p_indice].tamanhoLista; i++)
        {
            printf(" - %s \n", v_tabelaHash[p_indice].lista[i]);
        }
    }
}