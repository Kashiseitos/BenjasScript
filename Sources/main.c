#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Headers/BenjasSript.h"

int intSymbolCount = 0;
t_token *ptrTokenList;
t_symbol symbolArray[MAX_SYMBOLS];
FILE *OutputFile = NULL, *OutputFileText;
int boolParserError = FALSE;




        //Cuantos   //Que argumentos?
int main(int argc, char *argv[])
{
    //No hay argumentos
    if (argc < 2) 
    {
        printf("Error: Falta la interfaz de red.\n");
        printf("Uso correcto: %s <interfaz_de_red>\n", argv[0]);
        printf("Ejemplo: %s eth0\n", argv[0]);
        
        // Salimos del programa con un código de error para que no intente ejecutar la MV
        return 1; 
    }

    scanner("SourceCode.txt");
    PrintTokenList();

    printf("\n");
    parser_RDCP();
    //parser_LL();

    FreeTokenList();
    PrintTokenList();

    printf("\n=================================================\n");
    printf("\nNo errors, press ENTER to proceed...\n");
    printf("\n=================================================\n");
    
    getchar();

    ejecutar_VM(argv[1]);
    
    return 0;
}