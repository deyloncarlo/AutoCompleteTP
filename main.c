#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <termios.h>
#include <unistd.h>


#define TAMANHO_TABELA_HASH 127

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

bool carregarDicionario(char *p_nomeArquivo);

bool usuarioApertouEnter(char p_caracter);

bool usuarioApertouBackspace(char p_caracter);

bool numeroLetrasIncorreto(int p_numeroLetras, int p_numeroPermitido);

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

    carregarDicionario(p_argv[1]);

    int v_numeroMaximoIndexacao = TAMANHO_TABELA_HASH - 1;
    int v_numeroLetras = 0;
    bool v_usuarioNaoApertouEnter = true;
    char v_char;
    char v_busca[v_numeroMaximoIndexacao];
    v_busca[v_numeroMaximoIndexacao] = '\0'; // Removendo caracter que identificar uma string ('@')

    //Crinado tabela Hash
    Palavra *tabelaHash = malloc(sizeof(Palavra) * TAMANHO_TABELA_HASH);
    Palavra v_ola;
    v_ola.chave = "Ola";
    tabelaHash[0] = v_ola;
    printf("%s", tabelaHash[0].chave);

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

bool carregarDicionario(char *p_nomeArquivo)
{
    printf("\n[Início] Carregando dicionário...\n");
    
    FILE *v_dicionario = fopen(p_nomeArquivo, "r");
    char v_palavra [100];
    if(v_dicionario == NULL)
    {
        printf("Erro ao encontrar dicionário de palavras.");
        return EXIT_FAILURE;
    }

    printf("\n\nPalavras: \n");
    printf("========");
    while(fscanf(v_dicionario, "%s", v_palavra) != EOF)
    {
        printf("\n%s", v_palavra);
    }
    printf("\n\n");
    fclose(v_dicionario);

    printf("\n[Fim] Carregando dicionário...\n");
}