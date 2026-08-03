#ifndef BenjasScript_H
#define BenjasScript_H

#include <stdio.h>
#include <stdlib.h>

#define FALSE (0)
#define TRUE  (-1)

void InsertInTokenList(int, char *, int, int);
void PrintTokenList();
void FreeTokenList();
int GetInputSymbolCode(char);
char *GetText_TerminalSymbol(int);
int IsSeparatorToken(int);
int scanner(char *);
void parser_RDCP();

struct t_token
{
    int intTokenCode;
    char *strTokenSourceCodeText;
    int intRow;
    int intColumn;
    struct t_token *ptrNext;
};
typedef struct t_token t_token;
extern t_token *ptrTokenList;//NULL;


//Scanner===========================================
#define T_main 1
#define T_while 2
#define T_Stand 3
#define T_Sit 4
#define T_Walk 5
#define T_Climb 6
#define T_Rotate 7
#define T_GetDist 8
#define T_GetBattery 9
#define T_LogOpAnd 10
#define T_LogOpOr 11
#define T_Assignment 12
#define T_Equals 13
#define T_Less 14
#define T_LessOrEqual 15
#define T_Greater 16
#define T_GreaterOrEqual 17
#define T_Not 18
#define T_NotEquals 19
#define T_Lparentheses 20
#define T_Rparentheses 21
#define T_Lbraces 22
#define T_Rbraces 23
#define T_Lbrackets 24
#define T_Rbrackets 25
#define T_Addition 26
#define T_Substraction 27
#define T_Multiplication 28
#define T_Division 29
#define T_Residue 30
#define T_SemiColon 31
#define T_EOL 32
#define T_CR 33
#define T_Tab 34
#define T_DoubleQuotes 35
#define T_Quote 36
#define T_Space 37
#define T_EOF 38
#define T_ID 39
#define T_integer 40
#define T_decimal 41
#define T_if 42
#define T_else 43

//===========================================

//Parser=====================================
#define NT_Program 1
#define NT_Element 2
#define NT_Loop 3
#define NT_WhileLoop 4
#define NT_WhileBlock 5
#define NT_FullCondition 6
#define NT_Condition 7
#define NT_Instruction 8
#define NT_MovementInstr 9
#define NT_AssignInstr 10
#define NT_Expression 11
#define NT_Term 12
#define NT_Factor 13
#define NT_Value 14
#define NT_SeneseFunc 15
#define NT_RelationalOperator 16
#define NT_Logicaloperator 17
#define NT_AdditiveOp 18
#define NT_MultiplicativeOp 19
#define NT_Num 20
#define NT_IfInstr 21

//============================================

//YA es para la maquina virtual

//Creamos el archivo
extern FILE *OutputFile;
extern FILE *OutputFileText;

enum InstruccionesCM
{
    OP_HALT = 0,    // HALT: Termina el programa
    OP_PUSH = 1,  // PUSH [valor]: Mete un número constante a la pila
    OP_LOAD = 2,  // LOAD [index]: Lee la variable en 'index' y la mete a la pila
    OP_STORE = 3,  // STORE [index]: Saca el tope de la pila y lo guarda en 'index'
    OP_ADD = 4,  // Saca dos valores, los suma, mete el resultado
    OP_SUB = 5,  // Saca dos valores, los resta, mete el resultado
    OP_MUL = 6,  // Saca dos valores, los multiplica, mete el resultado
    OP_DIV = 7,   // Saca dos valores, los divide, mete el resultado
    OP_MOD = 8,  // Saca dos valores, hace residuo, mete el resultado

    OP_GT = 9,   //Greater
    OP_GE = 10, //Greater or Equal
    OP_LT = 11,  //Lower
    OP_LE = 12, //Lower or Equal
    OP_EQ = 13, //Equal
    OP_NEQ = 14,    //Not Equal
    
    OP_GETDIST = 15,
    OP_GETBATTERY = 16,
    
    OP_WALK = 17,
    OP_ROTATE = 18,
    OP_CLIMB = 19,
    OP_SIT = 20,
    OP_STAND = 21,

    OP_NOT_JUMP = 22,
    OP_JUMP = 23,

    OP_AND = 24,
    OP_OR = 25
}; typedef enum InstruccionesCM OpCode;


//Para guardar los id
int get_symbol_index(char *strIdName);
#define MAX_SYMBOLS 100

// Definimos qué guarda cada variable
struct s_symbol {
    char strName[32];      // Nombre de la variable (ej: "hola")
    double doubleValue;    // Valor real que tiene (para la Máquina Virtual)
    int intIsInitialized;  // Para saber si ya se le asignó algo
}; typedef struct s_symbol t_symbol;

// Tu Tabla de Símbolos es un arreglo de estas estructuras
extern t_symbol symbolArray[MAX_SYMBOLS];
extern int intSymbolCount; // Contador de cuántas variables llevamos registradas

// PUENTE C / C++ ============================================
// Esto debe ir adentro del guardabarros para que proteja la función en todo el proyecto
#ifdef __cplusplus
extern "C" {
#endif

// Ahora tu MV puede recibir dinámicamente la ruta de tu Object.bin
void ejecutar_VM(const char* networkInt);

#ifdef __cplusplus
}
#endif

#endif

