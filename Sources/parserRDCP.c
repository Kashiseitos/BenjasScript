#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Headers/BenjasSript.h"

t_token *ptrCurrentToken;

extern int boolParserError;

void Error(char *msg);
void Expect(int);
int CurrentToken(int intTSymbol);
void Expect(int intTSymbol);
int CurrentTokenInFirst(int intNTSymbol);
void Program();
void Element();
void Loop();
void WhileLoop();
void WhileBlock();
void FullCondition();
void Condition();
void IfInstr();
void Instruction();
void MovementInstr();
void AssignInstr();
void Expression();
void Term();
void Factor();
void Value();
void SenseFunc();
void RelationalOperator(); 
void LogicalOperator();
void AdditiveOp();
void MultiplicativeOp();
void Num();
void parser_RDCP();



void Error(char *msg)
{
    boolParserError = TRUE;
    printf("\nSyntactic Error r[%d]c[%d] - %s - Buffer:[%s]\n",
           ptrCurrentToken->intRow,
           ptrCurrentToken->intColumn,
           msg,
           ptrCurrentToken->strTokenSourceCodeText);
}

int CurrentToken(int intTSymbol)
{
    int boolres = FALSE;

    if(!boolParserError)
    {
        if(ptrCurrentToken->intTokenCode == intTSymbol)
        {
            boolres=TRUE;
        }
        else
        {
            boolres=FALSE;
        }
    }
    return boolres;
}

void Expect(int intTSymbol)
{
    if(!boolParserError)
    {
        if(CurrentToken(intTSymbol))
        {
            printf("Token syntactically OK. r[%d]c[%d] - Buffer:[%s]\n",
                   ptrCurrentToken->intRow,
                   ptrCurrentToken->intColumn,
                   ptrCurrentToken->strTokenSourceCodeText);
            ptrCurrentToken=ptrCurrentToken->ptrNext;
        }
        else
        {
            boolParserError = TRUE;
            printf("\nSyntactic Error r[%d]c[%d] - Buffer:[%s]\n",
                   ptrCurrentToken->intRow,
                   ptrCurrentToken->intColumn,
                   ptrCurrentToken->strTokenSourceCodeText);
        }
    }
}

int CurrentTokenInFirst(int intNTSymbol)
{
    int intRes = FALSE;

    if(!boolParserError)
    {
        switch(intNTSymbol)
        {
            //Program         = "main" "{" { Element } "}" 
            case NT_Program:
                if(CurrentToken(T_main))
                {
                    intRes = TRUE;
                }
                break;
            //Element         = Loop | Instruction ";" 
            case NT_Element:
                if(CurrentTokenInFirst(NT_Loop))
                {
                    intRes = TRUE;
                }
                else if(CurrentTokenInFirst(NT_Instruction))
                {
                    intRes = TRUE;
                }
                else if(CurrentTokenInFirst(NT_IfInstr))
                {
                    intRes = TRUE;
                }
                break;

            //Loop            = WhileLoop 
            case NT_Loop:
                if(CurrentTokenInFirst(NT_WhileLoop))
                {
                    intRes = TRUE;
                }
                break;

            //WhileLoop       = WhileBlock "{" { Element } "}" 
            case NT_WhileLoop:
                if(CurrentTokenInFirst(NT_WhileBlock))
                {
                    intRes = TRUE;
                }
                break;

            //WhileBlock      = "while" "(" FullCondition ")" 
            case NT_WhileBlock:
                if(CurrentToken(T_while))
                {
                    intRes = TRUE;
                }
                break;


            // FullCondition = Condition { LogicalOperator Condition } 
            case NT_FullCondition:
                if(CurrentTokenInFirst(NT_Condition))
                {
                    intRes = TRUE;
                }
                break;

            // Condition = [ "(" ] Value RelationalOperator Value [ ")" ]
            case NT_Condition:
                if(CurrentToken(T_Lparentheses) || CurrentTokenInFirst(NT_Value))
                {
                    intRes = TRUE;
                }
                break;

            // Instruction = MovementInstr | AssignInstr | Expression 
            case NT_Instruction:
                if(CurrentTokenInFirst(NT_MovementInstr) || 
                   CurrentTokenInFirst(NT_AssignInstr)   || 
                   CurrentTokenInFirst(NT_Expression))
                {
                    intRes = TRUE;
                }
                break;

            // MovementInstr = "Stand" | "Sit" | "Walk" Num | "Climb" Num | "Rotate" Num
            case NT_MovementInstr:
                if(CurrentToken(T_Stand) || CurrentToken(T_Sit)   || 
                   CurrentToken(T_Walk)  || CurrentToken(T_Climb) || 
                   CurrentToken(T_Rotate))
                {
                    intRes = TRUE;
                }
                break;
            
            // AssignInstr = ID "=" Expression 
            case NT_AssignInstr:
                if(CurrentToken(T_ID))
                {
                    intRes = TRUE;
                }
                break;

            // Expression = Term { AdditiveOp Term } 
            case NT_Expression:
                if(CurrentTokenInFirst(NT_Term))
                {
                    intRes = TRUE;
                }
                break;

            // Term = Factor { MultiplicativeOp Factor } 
            case NT_Term:
                if(CurrentTokenInFirst(NT_Factor))
                {
                    intRes = TRUE;
                }
                break;

            // Factor = Value | "(" Expression ")" 
            case NT_Factor:
                if(CurrentTokenInFirst(NT_Value) || CurrentToken(T_Lparentheses))
                {
                    intRes = TRUE;
                }
                break;

            // Value = Num | ID | SenseFunc 
            case NT_Value:
                if(CurrentTokenInFirst(NT_Num) || CurrentToken(T_ID) || CurrentTokenInFirst(NT_SeneseFunc))
                {
                    intRes = TRUE;
                }
                break;

            // SenseFunc = "GetDist()" | "GetBattery()" 
            case NT_SeneseFunc:
                if(CurrentToken(T_GetDist) || CurrentToken(T_GetBattery))
                {
                    intRes = TRUE;
                }
                break;

            // RelationalOperator = ">" | ">=" | "!=" | "==" | "<" | "<=" 
            case NT_RelationalOperator:
                if(CurrentToken(T_Greater) || CurrentToken(T_GreaterOrEqual) || 
                   CurrentToken(T_NotEquals) || CurrentToken(T_Equals)         || 
                   CurrentToken(T_Less)    || CurrentToken(T_LessOrEqual))
                {
                    intRes = TRUE;
                }
                break;

            // LogicalOperator = "&&" | "||"
            case NT_Logicaloperator:
                if(CurrentToken(T_LogOpAnd) || CurrentToken(T_LogOpOr))
                {
                    intRes = TRUE;
                }
                break;

            // AdditiveOp = "+" | "-" 
            case NT_AdditiveOp:
                if(CurrentToken(T_Addition) || CurrentToken(T_Substraction))
                {
                    intRes = TRUE;
                }
                break;

            // MultiplicativeOp = "*" | "/" | "%" 
            case NT_MultiplicativeOp:
                if(CurrentToken(T_Multiplication) || CurrentToken(T_Division) || CurrentToken(T_Residue))
                {
                    intRes = TRUE;
                }
                break;

            // Num = integer | decimal 
            case NT_Num:
                if(CurrentToken(T_integer) || CurrentToken(T_decimal))
                {
                    intRes = TRUE;
                }
                break;
            
            case NT_IfInstr:
                if(CurrentToken(T_if))
                {
                    intRes = TRUE;
                }
                break;
        }
    }

    return intRes;
}
// S ::= "program" CompoundInstruction
void Program()
{
    if(!boolParserError)
    {
        Expect(T_main);
        Expect(T_Lbraces);
        while(CurrentTokenInFirst(NT_Element))
        {
            Element();
        }
        Expect(T_Rbraces);
    }
}

//Element         = Loop | Instruction ";" 
void Element()
{
    if(!boolParserError)
    {
        if(CurrentTokenInFirst(NT_Loop))
        {
            Loop();
        }
        else if(CurrentTokenInFirst(NT_Instruction))
        {
            Instruction();
            Expect(T_SemiColon);
        }
        else if(CurrentTokenInFirst(NT_IfInstr))
        {   
            IfInstr();
        }
    }
}

//Loop            = WhileLoop 
void Loop()
{
    if(!boolParserError)
    {
        WhileLoop();
    }
}

//WhileLoop       = WhileBlock "{" { Element } "}" 
void WhileLoop()
{
    if(!boolParserError)
    {
        //Guardamos el byte en donde empieza el while
        int startAddress = (int)ftell(OutputFile);


        WhileBlock(); //Imprime la condicion en el archivo

        //Se genera el salto condicional
        int opCode = OP_NOT_JUMP;
        fwrite(&opCode, sizeof(int), 1, OutputFile);
        //Posicion donde va a estar el parche
        long patchPosition = ftell(OutputFile);
        //Escribimos un 0 temporal
        int tempDest = 0;
        fwrite(&tempDest, sizeof(int), 1, OutputFile);
        //Lo equivalente en txt
        fprintf(OutputFileText, "%d [PENDIENTE]\n", opCode);

        Expect(T_Lbraces);
        while(CurrentTokenInFirst(NT_Element))
        {
            Element();
        }
        Expect(T_Rbraces);

        //Generamos el salto de regreso
        opCode = OP_JUMP;
        fwrite(&opCode, sizeof(int), 1, OutputFile);
        fwrite(&startAddress, sizeof(int), 1, OutputFile);
        fprintf(OutputFileText, "%d %d\n", opCode, startAddress);

        //BACKPATCHING (Reellenar el hueco)
        int endAddress = (int)ftell(OutputFile);

        fseek(OutputFile, patchPosition, SEEK_SET); //vamos al hueco
        fwrite(&endAddress, sizeof(int),1, OutputFile); //Parcheamos con la direccion
        fseek(OutputFile, 0, SEEK_END); //Volvemos al final
    }
}

//WhileBlock      = "while" "(" FullCondition ")" 
void WhileBlock()
{
    if(!boolParserError)
    {
        Expect(T_while);
        Expect(T_Lparentheses);
        FullCondition();
        Expect(T_Rparentheses);
    }
}

//FullCondition   = Condition { LogicalOperator Condition } 
void FullCondition()
{
    if(!boolParserError)
    {
        Condition();
        while (CurrentTokenInFirst(NT_Logicaloperator))
        {

            int logOpType = ptrCurrentToken->intTokenCode;

            LogicalOperator();
            Condition();

            int opCode;
            if(logOpType == T_LogOpAnd) opCode = OP_AND;
            else opCode = OP_OR;
            
            fwrite(&opCode, sizeof(int), 1, OutputFile);
            fprintf(OutputFileText, "%d\n", opCode);
        }
        
    }
}

//Condition       = "("  Value RelationalOperator Value “)” 
void Condition()
{
    if(!boolParserError)
    {
        Expect(T_Lparentheses);
        Value();

        //Obtenemos la operacion antes de que la quite
        int opType = ptrCurrentToken->intTokenCode;

        RelationalOperator();
        Value();
        Expect(T_Rparentheses);

        int opCode;
        switch(opType)
        {
            case T_Greater: opCode = OP_GT; break;
            case T_GreaterOrEqual: opCode = OP_GE; break;
            case T_Less: opCode = OP_LT; break;
            case T_LessOrEqual: opCode = OP_LE; break;
            case T_Equals: opCode = OP_EQ; break;
            case T_NotEquals: opCode = OP_NEQ; break;
        }

        fwrite(&opCode, sizeof(int), 1, OutputFile);
        fprintf(OutputFileText, "%d\n", opCode);
    }
}


//"if" "(" FullCondition ")" "{" { Element } "}" [ "else" "{" { Element } "}" ]
void IfInstr()
{
    if(!boolParserError)
    {
        Expect(T_if);
        Expect(T_Lparentheses);
        FullCondition();
        Expect(T_Rparentheses);

        //Salto por si la condicion es falsa
        int opCode = OP_NOT_JUMP;
        fwrite(&opCode, sizeof(int), 1, OutputFile);
        long patchIfFalse = ftell(OutputFile);
        //Como el while, guardamos un 0 temporal
        int tempDest = 0;
        fwrite(&tempDest, sizeof(int), 1, OutputFile);
        fprintf(OutputFileText, "%d [Saltar_else_fin]\n", opCode);

        //Bloque del if
        Expect(T_Lbraces);

        while (CurrentTokenInFirst(NT_Element))
        {
            Element();
        }

        Expect(T_Rbraces);

        if(CurrentToken(T_else))
        {
            //Como existe un else, hay que enviar el salto para aca
            int jumpCode = OP_JUMP;

            Expect(T_else);

            fwrite(&jumpCode, sizeof(int), 1, OutputFile);
            long patchToFin = ftell(OutputFile); //Guardamos el segundo parche
            fwrite(&tempDest, sizeof(int), 1, OutputFile);
            fprintf(OutputFileText, "%d [Saltar_fin]\n", jumpCode);

            //Ya sabemos donde empieza el else ----------------
            int elseAddress = (int) ftell(OutputFile);
            fseek(OutputFile, patchIfFalse, SEEK_SET);
            fwrite(&elseAddress, sizeof(int), 1, OutputFile);
            fseek(OutputFile, 0, SEEK_END);
            //-------------------------------------------------

            Expect(T_Lbraces);

            while (CurrentTokenInFirst(NT_Element))
            {
                Element();
            }

            Expect(T_Rbraces);

            //Ya sabemos donde termina el else ----------------
            int finAddress = (int)ftell(OutputFile);
            fseek(OutputFile, patchToFin, SEEK_SET);
            fwrite(&finAddress, sizeof(int), 1, OutputFile);
            fseek(OutputFile, 0, SEEK_END);
            //-------------------------------------------------
        }
        //Si no habia else
        else
        {
            //Ya sabemos donde termina el else ----------------
            int finAddress = (int)ftell(OutputFile);
            fseek(OutputFile, patchIfFalse, SEEK_SET);
            fwrite(&finAddress, sizeof(int), 1, OutputFile);
            fseek(OutputFile, 0, SEEK_END);
            //-------------------------------------------------
        }
        
    }
}

//Instruction     = MovementInstr | AssignInstr | Expression 
void Instruction()
{
    if(!boolParserError)
    {
        if(CurrentTokenInFirst(NT_MovementInstr))
        {
            MovementInstr();
        }
        else if(CurrentTokenInFirst(NT_AssignInstr))
        {
            AssignInstr();
        }
        else if(CurrentTokenInFirst(NT_Expression))
        {
            Expression();
        }
    }
}

//MovementInstr   = "Stand" | "Sit" | "Walk" Num | "Climb" Num | "Rotate" Num
void MovementInstr()
{
    if(!boolParserError)
    {
        int opCode;

        if(CurrentToken(T_Stand))
        {
            opCode = OP_STAND;
            Expect(T_Stand);
            fwrite(&opCode, sizeof(int), 1, OutputFile);
            fprintf(OutputFileText, "%d\n", opCode);
        }
        else if(CurrentToken(T_Sit))
        {   
            opCode = OP_SIT;
            Expect(T_Sit);
            fwrite(&opCode, sizeof(int), 1, OutputFile);
            fprintf(OutputFileText, "%d\n", opCode);
        }
        else if(CurrentToken(T_Walk))
        {
            opCode = OP_WALK;
            Expect(T_Walk);
            Value();

            fwrite(&opCode, sizeof(int), 1, OutputFile);
            fprintf(OutputFileText, "%d\n", opCode);
        }
        else if(CurrentToken(T_Climb))
        {
            opCode = OP_CLIMB;
            Expect(T_Climb);
            Value();

            fwrite(&opCode, sizeof(int), 1, OutputFile);
            fprintf(OutputFileText, "%d\n", opCode);
        }
        else if(CurrentToken(T_Rotate))
        {
            opCode = OP_ROTATE;
            Expect(T_Rotate);
            Value();

            fwrite(&opCode, sizeof(int), 1, OutputFile);
            fprintf(OutputFileText, "%d\n", opCode);
        }
    }
}
 
//AssignInstr     = ID "=" Expression 
void AssignInstr()
{
    if(!boolParserError)
    {
        //Antes de descartar el ID, registramos el nombre
        char idName[32];
        strcpy(idName, ptrCurrentToken->strTokenSourceCodeText);

        Expect(T_ID);
        Expect(T_Assignment);

        Expression();

        //Vamos a guardar el index del valor (Su posicion en la memoria)
        int varIndex = get_symbol_index(idName);
        int opCode = OP_STORE;
        fwrite(&opCode, sizeof(int), 1, OutputFile);
        fwrite(&varIndex, sizeof(int), 1, OutputFile);

        fprintf(OutputFileText, "%d %d\n", OP_STORE, varIndex);
    }
}

//Expression      = Term { AdditiveOp Term } 
void Expression()
{
    if(!boolParserError)
    {
        Term();
        while(CurrentTokenInFirst(NT_AdditiveOp))
        {
            //Guardamos el tipo (+ -)
            int opType = ptrCurrentToken->intTokenCode;

            AdditiveOp();
            Term();
            
            int opCode;
            if(opType == T_Addition) opCode = OP_ADD;
            else opCode = OP_SUB;
            fwrite(&opCode, sizeof(int), 1, OutputFile);

            fprintf(OutputFileText, "%d\n", opCode);
        }
    }
} 

//Term            = Factor { MultiplicativeOp Factor } 
void Term()
{
    if(!boolParserError)
    {
        Factor();
        while(CurrentTokenInFirst(NT_MultiplicativeOp))
        {
            int opType = ptrCurrentToken->intTokenCode;

            MultiplicativeOp();
            Factor();

            int opCode;
            if(opType == T_Multiplication) opCode = OP_MUL;
            else if(opType == T_Division) opCode = OP_DIV;
            else opCode = OP_MOD;

            fwrite(&opCode, sizeof(int), 1, OutputFile);

            fprintf(OutputFileText, "%d\n", opCode);
        }
    }
}

//Factor          = Value | "(" Expression ")" 
void Factor()
{
    if(!boolParserError)
    {
        Value();
        if(CurrentToken(T_Lparentheses))
        {
            Expect(T_Lparentheses);
            Expression();
            Expect(T_Rparentheses);
        }
    }
}

//Value           = Num | ID | SenseFunc 
void Value()
{
    if(!boolParserError)
    {
        if(CurrentTokenInFirst(NT_Num))
        {
            char valText[32]; //float/int
            strcpy(valText, ptrCurrentToken->strTokenSourceCodeText);

            Num();

            int opCode = OP_PUSH;
            double valDo = atof(valText);

            fwrite(&opCode, sizeof(int), 1, OutputFile);
            fwrite(&valDo, sizeof(double), 1, OutputFile);

            fprintf(OutputFileText, "%d %lf\n", opCode, valDo);
        }
        else if(CurrentToken(T_ID))
        {
            //Antes de descartar el ID, registramos el nombre
            char idName[32];
            strcpy(idName, ptrCurrentToken->strTokenSourceCodeText);
            
            Expect(T_ID);

            int  varIndex = get_symbol_index(idName);

            int opCode = OP_LOAD;
            fwrite(&opCode, sizeof(int), 1, OutputFile);
            fwrite(&varIndex, sizeof(int), 1, OutputFile);

            fprintf(OutputFileText, "%d %d\n", OP_LOAD, varIndex);
        }
        else if(CurrentTokenInFirst(NT_SeneseFunc))
        {
            SenseFunc();
        }
    }
}

//SenseFunc       = "GetDist()" | "GetBattery()" 
void SenseFunc()
{
    if(!boolParserError)
    {
        int opCode;
        if(CurrentToken(T_GetDist))
        {
            Expect(T_GetDist);

            opCode = OP_GETDIST; 
        }
        else if(CurrentToken(T_GetBattery))
        {
            Expect(T_GetBattery);

            opCode = OP_GETBATTERY; 
        }
        fwrite(&opCode, sizeof(int), 1, OutputFile);
        fprintf(OutputFileText, "%d\n", opCode);
    }
}

//RelationalOperator = ">" | ">=" | "!=" | "==" | "<" | "<=" 
void RelationalOperator()
{
    if(!boolParserError)
    {
        if(CurrentToken(T_Greater))
        {
            Expect(T_Greater);
        }
        else if(CurrentToken(T_GreaterOrEqual))
        {
            Expect(T_GreaterOrEqual);
        }
        else if(CurrentToken(T_NotEquals))
        {
            Expect(T_NotEquals);
        }
        else if(CurrentToken(T_Equals))
        {
            Expect(T_Equals);
        }
        else if(CurrentToken(T_Less))
        {
            Expect(T_Less);
        }
        else if(CurrentToken(T_LessOrEqual))
        {
            Expect(T_LessOrEqual);
        }
    }
}

//LogicalOperator    = "&&" | "||" 
void LogicalOperator()
{
    if(!boolParserError)
    {
        int opCode;

        if(CurrentToken(T_LogOpAnd))
        {
            Expect(T_LogOpAnd);
            opCode = OP_AND;
        }
        else if(CurrentToken(T_LogOpOr))
        {
            Expect(T_LogOpOr);
            opCode = OP_OR;
        }
    }
}

//AdditiveOp         = "+" | "-" 
void AdditiveOp()
{
    if(!boolParserError)
    {
        if(CurrentToken(T_Addition))
        {
            Expect(T_Addition);
        }
        else if(CurrentToken(T_Substraction))
        {
            Expect(T_Substraction);
        }
    }
}

//MultiplicativeOp   = "*" | "/" | "%" 
void MultiplicativeOp()
{
    if(!boolParserError)
    {
        if(CurrentToken(T_Multiplication))
        {
            Expect(T_Multiplication);
        }
        else if(CurrentToken(T_Division))
        {
            Expect(T_Division);
        }
        else if(CurrentToken(T_Residue))
        {
            Expect(T_Residue);
        }
    }
}

//Num                = integer | decimal 
void Num()
{
    if(!boolParserError)
    {
        if(CurrentToken(T_integer))
        {
            Expect(T_integer);
        }
        else if(CurrentToken(T_decimal))
        {
            Expect(T_decimal);
        }
    }
}

//==================
//Para guardar los IDs
int get_symbol_index(char *strIdName)
{
    // PASO 1: Buscar si la variable ya existe
    for (int i = 0; i < intSymbolCount; i++)
    {
        // Si encontramos el nombre en la tabla...
        if (strcmp(symbolArray[i].strName, strIdName) == 0)
        {
            return i; // Regresa su índice actual (0, 1, 2...)
        }
    }

    // PASO 2: Si el ciclo terminó y no la encontró, significa que es NUEVA.
    // Validamos que no se llene la tabla
    if (intSymbolCount >= MAX_SYMBOLS)
    {
        Error("Symbol Table Overflow - Demasiadas variables declaradas");
        return -1;
    }

    // Registramos la nueva variable en la posición libre actual
    strcpy(symbolArray[intSymbolCount].strName, strIdName);
    symbolArray[intSymbolCount].doubleValue = 0.0;     // Valor por defecto
    symbolArray[intSymbolCount].intIsInitialized = FALSE;

    // Guardamos el índice actual para regresarlo antes de incrementar el contador
    int intNewIndex = intSymbolCount;
    intSymbolCount++; // Incrementamos para la siguiente variable nueva

    return intNewIndex; // Regresa el índice donde se guardó
}

void parser_RDCP()
{
    ptrCurrentToken = ptrTokenList;
    boolParserError = FALSE;

    //OutputFile es global, esta en BenjasScript.h
    OutputFile = fopen("../../Object.bin", "wb");
    OutputFileText = fopen("../../Object.txt", "w");
    //Como es wb, exista o no existe, lo crea

    Program();

    int opCode = OP_HALT;
    fwrite(&opCode, sizeof(int), 1, OutputFile);

    fprintf(OutputFileText, "%d\n", OP_HALT);

    fclose(OutputFile);
    fclose(OutputFileText);
}
