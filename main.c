#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

#define TAMANHO_TABELA_HASH 999999

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

void listarPalavras(int p_indice, char *p_chave);

void indexarPalavras(int p_lenth, int p_tamanhoPrefixo, char p_palavra[]);

void limparArrayBusca(char *p_array, int p_tamanho);

bool isChavesIguais(char *p_chave1, char *p_chave2);

void ordernar(Palavra p_palavra);

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
    limparArrayBusca(v_busca, v_tamanhoPrefixo);
    //v_busca[v_tamanhoPrefixo] = '\0'; // Removendo caracter que identificar uma string ('@')
    while(v_usuarioNaoApertouEnter)
    {
        v_char = getch();
        if(usuarioApertouEnter(v_char))
        {
            v_usuarioNaoApertouEnter = false;
            printf("\n\nPrograma Finalizado!!\n");
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
                    char v_aux[strlen(v_busca)];
                    v_aux[strlen(v_busca)] = '\0';
                    memcpy(v_aux, v_busca, strlen(v_busca));
                    printf("\nBusca: %s\n", v_busca);
                    int v_indice = funcaoHash(v_aux, strlen(v_aux));
                    listarPalavras(v_indice, v_aux);
                }
                else
                {
                    v_busca[v_numeroLetras] = v_char;
                    char v_aux[strlen(v_busca)];
                    v_aux[strlen(v_busca)] = '\0';
                    memcpy(v_aux, v_busca, strlen(v_busca));
                    printf("\nBusca: %s\n", v_busca);
                    int v_indice = funcaoHash(v_aux, strlen(v_aux));
                    listarPalavras(v_indice, v_aux);
                    v_numeroLetras++;

                }
            }
        }   
    }
    
    return 0;
}

void limparArrayBusca(char *p_array, int p_tamanho)
{
    int v_indice;
    for(v_indice = 0; v_indice <= p_tamanho; v_indice++)
    {
        p_array[v_indice] = '\0';
    }
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

    //printf("\n\nPalavras: \n");
    //printf("========");
    int v_indiceTabelaHash;
    while(fscanf(v_dicionario, "%s", v_palavra) != EOF)
    {
        if(p_tamanhoPrefixo >= strlen(v_palavra))
        {
            int v_lenth = strlen(v_palavra);
            indexarPalavras(v_lenth, p_tamanhoPrefixo, v_palavra);
        }
        else 
        {
            indexarPalavras(p_tamanhoPrefixo, p_tamanhoPrefixo, v_palavra);
        }

    }

    printf("\n\n");
    fclose(v_dicionario);
    printf("\n[Fim] Carregando dicionário...\n");
}

void indexarPalavras(int p_lenth, int p_tamanhoPrefixo, char p_palavra[])
{
    int v_indiceTabelaHash;
    int v_lenthLimit = 1;
    while (v_lenthLimit <= p_lenth)
    {
        Palavra v_novaPalavra;
        v_novaPalavra.chave = (char *)malloc(p_tamanhoPrefixo);
        v_novaPalavra.lista = malloc(100 * sizeof(char*));
        char v_aux[v_lenthLimit];
        v_aux[v_lenthLimit] = '\0';
        memcpy(v_aux, p_palavra, v_lenthLimit);
        v_indiceTabelaHash = funcaoHash(v_aux, v_lenthLimit);
        strcpy(v_novaPalavra.chave,v_aux);
        v_lenthLimit++;
        
        if(v_tabelaHash[v_indiceTabelaHash].chave != NULL)
        {
            if(isChavesIguais(v_tabelaHash[v_indiceTabelaHash].chave, v_aux))
            {
                int v_tamanhoLista = v_tabelaHash[v_indiceTabelaHash].tamanhoLista;
                v_tabelaHash[v_indiceTabelaHash].lista[v_tamanhoLista] = malloc((100) * sizeof(char));
                strcpy(v_tabelaHash[v_indiceTabelaHash].lista[v_tamanhoLista], p_palavra);
                v_tabelaHash[v_indiceTabelaHash].tamanhoLista++;
            }
            else
            {
                int v_proximoIndiceNull = v_indiceTabelaHash;
                bool v_continua = true;
                while(v_continua)
                {
                    if(v_tabelaHash[v_proximoIndiceNull].chave == NULL)
                    {
                        //printf("\nComflito corrigido!!!\n");
                        v_novaPalavra.lista[0] = malloc((100) * sizeof(char));
                        strcpy(v_novaPalavra.lista[0], p_palavra);
                        v_novaPalavra.tamanhoLista = 1;
                        v_tabelaHash[v_proximoIndiceNull] = v_novaPalavra;
                        v_continua = false;
                    }
                    else if(isChavesIguais(v_tabelaHash[v_proximoIndiceNull].chave, v_aux))
                    {
                        int v_tamanhoLista = v_tabelaHash[v_proximoIndiceNull].tamanhoLista;
                        v_tabelaHash[v_proximoIndiceNull].lista[v_tamanhoLista] = malloc((100) * sizeof(char));
                        strcpy(v_tabelaHash[v_proximoIndiceNull].lista[v_tamanhoLista], p_palavra);
                        v_tabelaHash[v_proximoIndiceNull].tamanhoLista++;
                        v_continua = false;
                    }
                    v_proximoIndiceNull++;
                }
            }
        }
        else
        {
            v_novaPalavra.lista[0] = malloc((100) * sizeof(char));
            strcpy(v_novaPalavra.lista[0], p_palavra);
            v_novaPalavra.tamanhoLista = 1;
            v_tabelaHash[v_indiceTabelaHash] = v_novaPalavra;
        }
    }

}

int funcaoHash(char *p_prefixo, int p_fator)
{
    int v_hash = 0;
    int v_tamanho = strlen(p_prefixo);
    int v_indice;
    int v_decresce = p_fator;
    for(v_indice = 0; v_indice < v_tamanho; v_indice++)
    {
        v_hash += (v_decresce*v_decresce)*( ((int)p_prefixo[v_indice]) / (p_fator) );
        v_decresce--;
    }
    printf(" Funcao Hash calculada: %d\n", v_hash);
    return v_hash;
}

void listarPalavras(int p_indice, char *p_chave)
{
    //printf("\nOriginal: %s, Busca: %s\n", v_tabelaHash[p_indice].chave, p_chave);
    if(v_tabelaHash[p_indice].chave != NULL)
    {
        if(!isChavesIguais(v_tabelaHash[p_indice].chave, p_chave))
        {
                int v_proximoIndiceNull = p_indice;
                bool v_continua = true;
                while(v_continua)
                {
                    if(v_tabelaHash[v_proximoIndiceNull].chave == NULL)
                    {
                        v_continua = false;
                    }
                    else if(isChavesIguais(v_tabelaHash[v_proximoIndiceNull].chave, p_chave))
                    {
                        //printf("\nAchou o perdido!!!\n");
                        int i;
                        ordernar(v_tabelaHash[v_proximoIndiceNull]);
                        printf("Sugestões: ");
                        for(i = 0; i < v_tabelaHash[v_proximoIndiceNull].tamanhoLista; i++)
                        {
                            printf("%s, ", v_tabelaHash[v_proximoIndiceNull].lista[i]);
                        }
                        printf("\n\n\n");
                        v_continua = false;
                    }
                    v_proximoIndiceNull++;
                }
        }
        else
        {
            int i;
            ordernar(v_tabelaHash[p_indice]);
            printf("Sugestões: ");
            for(i = 0; i < v_tabelaHash[p_indice].tamanhoLista; i++)
            {
                printf("%s, ", v_tabelaHash[p_indice].lista[i]);
            }
            printf("\n\n\n");
        }
                
    }
}

bool isChavesIguais(char *p_chave1, char *p_chave2)
{
    int v_indice;
    if(strlen(p_chave1) == strlen(p_chave2))
    {
        for(v_indice = 0; v_indice < strlen(p_chave1); v_indice++)
        {
            if(p_chave1[v_indice] != p_chave2[v_indice])
            {
                return false;
            }
        }
        return true;
    }
    return false;
}

void ordernar(Palavra p_palavra)
{
    int i, j;
    char temp[100];
    for( i =0; i < p_palavra.tamanhoLista; i++)
    {
        for(j = i + 1; j< p_palavra.tamanhoLista; j++)
        {
            if(strcmp(p_palavra.lista[i],p_palavra.lista[j])>0)
            {
                strcpy(temp,p_palavra.lista[i]);
                strcpy(p_palavra.lista[i],p_palavra.lista[j]);
                strcpy(p_palavra.lista[j],temp);
            }
        }
    }
}