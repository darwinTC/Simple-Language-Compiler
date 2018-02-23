/*The implementation of this code was based on the material of the course*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "codegen.h"
#include "slc.h"

SymbEntryPtr symbolTable = NULL, currentFunction = NULL;
int currentLabel = 0, currentLevel = -1, currentDisplacement = -1,spaces=6,buffer=64;
TypeDescrPtr predefinedInteger = NULL, predefinedBoolean = NULL;
int stackHeight(){ return top; }

//--- Print code generated
void printArgument(char* arg1, char* arg2, char* arg3)
{
    if (arg1 != NULL) printf("   %s", arg1);
    if (arg2 != NULL) printf(",%s", arg2);
    if (arg3 != NULL) printf(",%s", arg3);
    printf("\n");
    fflush(stdout);
}
void printSpaces(int space, char* label){
    if (space > 0) printf("%s%-*s", label, space, ":");
    else printf("%s:", label);
}
void genCode(char* label, char* instruction, char* arg1, char* arg2, char* arg3)
{
    int labelLenght = 0;
    int space = 0;
    if (label != NULL){
        labelLenght = strlen(label);
        space = spaces - labelLenght;
        printSpaces(spaces,label);
    }
    else printf("%*s", spaces, "");
    printf("%s", instruction);
    
    printArgument(arg1,arg2,arg3);
}
//--- convert int to array of chars
char* IntToChar(int number, int isLabel){
    char* str = malloc(buffer);
    if(isLabel) snprintf(str, buffer, "L%d", number);
    else snprintf(str, buffer, "%d", number);
    //printf("%s\n",str);
    return str;
}
//--- Generate Instruction for Label
void genCodeLabel1(int label, char *statement, int level)
{
    genCode(IntToChar(label,1), statement, IntToChar(level,0), NULL, NULL);
}
//--- Generate only instruction
void genCode0(char* statement)
{
    genCode(NULL, statement, NULL, NULL, NULL);
}
//--- Generate instruction for function with 1 parameter
void genCode1(char *statement, int arg){
    genCode(NULL, statement, IntToChar(arg,0), NULL, NULL);
}
//--- Generate instruction for function with 2 parameters
void genCode2(char* statement, int arg1, int arg2)
{
    genCode(NULL, statement, IntToChar(arg1,0), IntToChar(arg2,0), NULL);
}
//--- Generate instruction for function with 3 parameters
void genCode3(char* statement, int arg1, int arg2, int arg3)
{
    genCode(NULL, statement, IntToChar(arg1,0), IntToChar(arg2,0), IntToChar(arg3,0));
}
//--- Generate instruction for Jump
void genCodeJump(char* type, int label)
{
    genCode(NULL, type, IntToChar(label,1), NULL, NULL);
}
//--- Generate instruction with a new label
void genCodeLabel(int label)
{
    genCode(IntToChar(label,1), "NOOP", NULL, NULL, NULL);
}
//--- Generate instruction for Enter Label
void genCodeENLB(int label, int level, int n)
{
    genCode(IntToChar(label,1), "ENLB", IntToChar(level,0), IntToChar(n,0), NULL);
}
//--- Generate instruction for Load Generalized Address and Call Function
void genCodeLGAD_CFUN(int label, int level, bool flag)
{
    if(flag) genCode(NULL, "LGAD", IntToChar(label,1), IntToChar(level,0), NULL);
    else genCode(NULL, "CFUN", IntToChar(label,1), IntToChar(level,0), NULL);
    
}
//--- Create new instance of a specific type structure
TypeDescrPtr newTypeDescr(TypeConstr constr, int size)
{
    TypeDescrPtr type = (TypeDescrPtr)calloc(1, sizeof(TypeDescr));
    type->constr = constr;
    type->size = size;
    return type;
}

SymbEntryPtr newSymbEntry(SymbCateg categ, char *id, int level)
{
    SymbEntryPtr st = (SymbEntryPtr)calloc(1, sizeof(SymbEntry));
    st->categ = categ;
    st->ident = id;
    st->level = level;
    return st;
}

ParamDescrPtr newParamDescr(TypeDescrPtr type, Passage pass)
{
    ParamDescrPtr paramDescr = (ParamDescrPtr)calloc(1, sizeof(ParamDescr));
    paramDescr->type = type;
    paramDescr->pass = pass;
    return paramDescr;
}
//--- increase by one the label
int newLabel(){ 
    return currentLabel=currentLabel+1;
}
//--- insert a new element to the symbol table
void insertSymbTable(SymbEntryPtr st)
{
    st->next = symbolTable;
    symbolTable = st;
}
//--- search "identifier" in the symbol table
SymbEntryPtr searchSte(char* identifier)
{
    SymbEntryPtr st = symbolTable;
    while (true) {
        if(st==NULL) return st;
        if (strcmp(st->ident, identifier) == 0) return st;
        st = st->next;
    }
}
//--- return the type of a specific leaf of the tree 
TypeDescrPtr getType(TreeNodePtr np)
{
    SymbEntryPtr st = searchSte(np->str);
    if(st == NULL || st->categ != S_TYPE) SemanticError(NULL); 
    return st->descr.type;
}
//--- Aux method: Change two list
SymbEntryPtr changeList(SymbEntryPtr l1, SymbEntryPtr l2, SymbEntryPtr ll){
    ll = l1;
    while (ll->next != NULL)
        ll = ll->next;
    ll->next = l2;
    return l1;
}
//--- Get the parameters descriptors
ParamDescrPtr getParamDescr(SymbEntryPtr formalParams)
{
    ParamDescrPtr paramList = NULL, param = NULL;
    while (true) {
        if(formalParams==NULL) return paramList;
        param = newParamDescr(formalParams->descr.formalParam.type, formalParams->descr.formalParam.pass);
        param->next = paramList;
        paramList = param;
        formalParams = formalParams->next;
    }
}
//--- Assign values for a new object SymbEntryPtr(S_PARAMETER)
SymbEntryPtr assignValue(SymbEntryPtr st, char *identifier, int *displacement, TypeDescrPtr type, Passage pass){
    st = newSymbEntry(S_PARAMETER, identifier, currentLevel);
    st->descr.formalParam.displ = *displacement;
    st->descr.formalParam.type = type;
    st->descr.formalParam.pass = pass;
    return st;
}
//--- Assign values for a new object SymbEntryPtr(S_VARIABLE)
SymbEntryPtr assignSymbolEntry(char* identifier, TypeDescrPtr type){
    SymbEntryPtr ste = newSymbEntry(S_VARIABLE, identifier, currentLevel);
    ste->descr.variable.displ = currentDisplacement;
    ste->descr.variable.type = type;
    return ste;
}
SymbEntryPtr assingLabel(char* identifier, bool flag){
    SymbEntryPtr st = newSymbEntry(S_LABEL, identifier, currentLevel);
    st->descr.label.mepaLabel = newLabel();
    st->descr.label.defined = flag;
    return st;
}
//--- Assign values for a new object TypeDescrPtr(T_FUNCTION)
TypeDescrPtr assingTypeDescr(TypeDescrPtr resultType, ParamDescrPtr param)
{
    TypeDescrPtr type = newTypeDescr(T_FUNCTION, 3);
    type->descr.function.result = resultType;
    type->descr.function.params = param;
    return type;
}
//--- Process expression parameters
SymbEntryPtr processExprPar(TreeNodePtr np, int *displacement, Passage pass, TypeDescrPtr type)
{
    if (np == NULL) return NULL;
    SymbEntryPtr l1, ll, st;
    
    l1 = processExprPar(np->next, displacement, pass, type);
    if (pass == P_VALUE) *displacement -= type->size;
    else *displacement -= 1;
    st = assignValue(st,np->str, displacement, type, pass);
    if (l1 == NULL) return st;
    else return changeList(l1, st, ll);
}
//--- Process Function as parameters
SymbEntryPtr processFunctAsPar(TreeNodePtr np, int *displacement)
{
    SymbEntryPtr st = NULL, formalParams = NULL;
    TypeDescrPtr type = NULL, resultType = NULL;
    int lastDispl = -4;
    if (np->comps[0] != NULL)
        resultType = getType(np->comps[0]);
    formalParams = processFormals(np->comps[2], &lastDispl);
    type = assingTypeDescr(resultType, getParamDescr(formalParams));
    *displacement -= type->size;
    st = assignValue(st,np->comps[1]->str, displacement, type, P_VARIABLE);
    return st;
}
//--- Process formal parameters 
SymbEntryPtr processFormals(TreeNodePtr np, int *displacement)
{
    if(np == NULL) return NULL;
    SymbEntryPtr l1,l2, ll;
    TypeDescrPtr type;
    l1 = processFormals(np->next,displacement);

    if(np->categ == C_PARAMETER || np->categ == C_REF_PARAMETER)
        type = getType(np->comps[1]);        
    if(np->categ == C_PARAMETER)l2 = processExprPar(np->comps[0],displacement, P_VALUE , type);
    if(np->categ == C_REF_PARAMETER) l2 = processExprPar(np->comps[0],displacement, P_VARIABLE , type);
    if(np->categ == C_FUCTION_PARAMETER) l2 = processFunctAsPar(np, displacement);

    if(l1 == NULL) return l2;
    else return changeList(l1, l2, ll);
}
//--- process labels
void processLabels(TreeNodePtr np)
{
    while (true) {
        if(np == NULL) break;
        SymbEntryPtr st = assingLabel(np->str,false);
        insertSymbTable(st);
        np = np->next;
    }
}
//--- process data of type array
TypeDescrPtr TypeArray(TreeNodePtr np, TypeDescrPtr parType)
{   
    TypeDescrPtr ansType = NULL, type = NULL;
    if (np != NULL){
        type = TypeArray(np->next, parType);
        ansType = newTypeDescr(T_ARRAY, atoi(np->str) * type->size);
        ansType->descr.array.dimen = atoi(np->str);
        ansType->descr.array.element = type;
        return ansType;
    }
    else return parType;
}
//--- determine the type of a specific leaf of the tree
TypeDescrPtr determineType(TreeNodePtr np)
{
    TreeNodePtr auxNode = np;
    if (np->comps[1] != NULL) {
        TypeDescrPtr type = NULL;
        np = np->comps[1];
        type = TypeArray(np, getType(auxNode->comps[0]));
        return type;
    }
    else return getType(auxNode->comps[0]);
}
//--- process the types declaration of a specific leaf of the tree
void processTypeDeclaration(TreeNodePtr np)
{
    TypeDescrPtr type = determineType(np->comps[1]);
    SymbEntryPtr st = newSymbEntry(S_TYPE, np->comps[0]->str, currentLevel);
    st->descr.type = type;
    insertSymbTable(st);
}
//--- process the type declaration
void processTypes(TreeNodePtr np)
{
    while (true) {
        if(np == NULL) break;
        processTypeDeclaration(np);
        np = np->next;
    }
}
//--- process the variables declaration of a specific leaf of the tree
void processVarDeclaration(TreeNodePtr np)
{
    TreeNodePtr var = np->comps[0];
    TypeDescrPtr type = determineType(np->comps[1]);
    while (true) {
        if(var == NULL) break;
        insertSymbTable(assignSymbolEntry(var->str, type));
        currentDisplacement = currentDisplacement + type->size;
        var = var->next;
    }
}
//--- process the variable declaration
void processVariables(TreeNodePtr np)
{
    while (true) {
        if(np == NULL) break;
        processVarDeclaration(np);
        np = np->next;
    }
}
//--- load formal symbol table
void loadFormalsSymbolTable(SymbEntryPtr list)
{
    if (list != NULL) {
        SymbEntryPtr table = list;
        while (table->next != NULL)
            table = table->next;
        table->next = symbolTable;
        symbolTable = list;
    }
}
//--- process functions declaration
void processFunctions(TreeNodePtr np)
{
    int label = newLabel();
    bool flagFunction = false;
    if (np != NULL) {
        flagFunction = true;
        genCodeJump("JUMP", label);
    }
    while (true) {
        if(np == NULL) break;
        processFunctionDecl(np, false);
        np = np->next;
    }
    if (flagFunction)
        genCodeLabel(label);
}
//--- determie if two type are compatible
bool CompatibleTypes(TypeDescrPtr t0, TypeDescrPtr t1)
{
    while ((t0->constr == T_ARRAY) && (t1->constr == T_ARRAY)) {
        if(t0->descr.array.dimen == t1->descr.array.dimen){
            t1 = t1->descr.array.element;
            t0 = t0->descr.array.element;
        }
        else return false;
    }
    if((t0->constr == T_ARRAY) || (t1->constr == T_ARRAY) || t0!=t1) return false;
    if (t0 == t1) return true;
}
//--- Relationar operators
bool relational(char* op)
{
    if (strcmp(op, "==") == 0 || strcmp(op, "!=") == 0 || strcmp(op, "<") == 0 || strcmp(op, "<=") == 0 || strcmp(op, ">") == 0 || strcmp(op, ">=") == 0)
        return true;
    return false;
}
//--- logical operations
bool logical(char* op)
{
    if (strcmp(op, "||") == 0 || strcmp(op, "&&") == 0)
        return true;
    return false;
}
//--- four operations(addition, subtraction, multiplication and division)
bool sumDifProDiv(char* op)
{
    if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0 || strcmp(op, "*") == 0 || strcmp(op, "/") == 0)
        return true;
    return false;
}
//--- determine if a operator is a unary operations
bool CompatibleTypesUnOp(char* op)
{
    if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0 || strcmp(op, "!") == 0) return true;
    return false;
}
//--- list of MEPA instruction for each binary operation
char* mepaBinInstr(char* op)
{
    if (strcmp(op, "+") == 0) return "ADDD";
    if (strcmp(op, "-") == 0) return "SUBT";
    if (strcmp(op, "||") == 0) return "LORR";
    if (strcmp(op, "*") == 0) return "MULT";
    if (strcmp(op, "/") == 0) return "DIVI";
    if (strcmp(op, "&&") == 0) return "LAND";
    if (strcmp(op, "==") == 0) return "EQUA";
    if (strcmp(op, "!=") == 0) return "DIFF";
    if (strcmp(op, "<") == 0) return "LESS";
    if (strcmp(op, "<=") == 0) return "LEQU";
    if (strcmp(op, ">") == 0) return "GRTR";
    if (strcmp(op, ">=") == 0) return "GEQU";
}
//--- list of MEPA instruction for each unary operation
char* mepaUnInstr(char* op)
{
    if (strcmp(op, "!") == 0) return "LNOT";
    if (strcmp(op, "+") == 0) return "NOOP";
    if (strcmp(op, "-") == 0) return "NEGT";
}
//--- check if two operator of a binary operation are compatible
bool CompatibleTypesBinOp(char* op)
{
    if (relational(op) || logical(op) || sumDifProDiv(op))
        return true;
    return false;
}
//--- process a binary expression
TypeDescrPtr processBinExpr(TreeNodePtr np)
{
    TypeDescrPtr t0 = processExpr(np->comps[0], false);
    TypeDescrPtr t1 = processExpr(np->comps[2], false);
    char* op = np->comps[1]->str;
    if (!CompatibleTypesBinOp(op))
        SemanticError(NULL);
    genCode0(mepaBinInstr(op));
    if (relational(op) || logical(op))
        return predefinedBoolean;
    else
        return predefinedInteger;
}
//--- process an unary expression
TypeDescrPtr processUnExpr(TreeNodePtr np)
{
    char* op = np->comps[0]->str;
    TypeDescrPtr t = processExpr(np->comps[1], false);
    if (!CompatibleTypesUnOp(op))
        SemanticError(NULL);
    genCode0(mepaUnInstr(op));
    return t;
}
//--- process integer
TypeDescrPtr processInteger(TreeNodePtr np)
{
    int value = atoi(np->str);
    genCode1("LDCT", value);
    return predefinedInteger;
}
//--- validate process variable
void validateProccesVaiable(int option, TreeNodePtr np, SymbEntryPtr ste, bool address){
    if(option==0 && (ste == NULL || ((ste->categ != S_VARIABLE) && (ste->categ != S_PARAMETER) && (ste->categ != S_CONST) && (ste->categ != S_FUNCTION)))) SemanticError(NULL); 
    if(option==1 && np->comps[1] != NULL) SemanticError(NULL);
    if(option==2 && (!address || np->comps[1] != NULL)) SemanticError(NULL);
}
TypeDescrPtr generateINDXcode(TreeNodePtr np, TypeDescrPtr resultType, TypeDescrPtr type, bool address){
    np = np->comps[1];
    while(np != NULL) {
      resultType = processExpr(np, false);
      if (resultType != predefinedInteger || type->constr != T_ARRAY)
         SemanticError(NULL);
      genCode1("INDX", type->descr.array.element->size);
      type = type->descr.array.element;
      np = np->next;
    }
    if (!address) {
       if (type->constr == T_ARRAY) genCode1("LDMV", type->size);
       else genCode0("CONT");
    }
    return type; 
}
//--- process variable
TypeDescrPtr processVariable(TreeNodePtr np, bool address)
{
    TypeDescrPtr type = NULL, resultType = NULL;
    SymbEntryPtr ste = searchSte(np->comps[0]->str);
    validateProccesVaiable(0,np,ste,address);
    if (ste->categ == S_CONST) {
        validateProccesVaiable(1,np,ste,address);
        type = ste->descr.constant.type;
        genCode1("LDCT", ste->descr.constant.value);
    }
    else if (ste->categ == S_FUNCTION) {
        validateProccesVaiable(2,np,ste,address);
        type = assingTypeDescr(ste->descr.funct.result, ste->descr.funct.params);
        genCodeLGAD_CFUN(ste->descr.funct.enterLabel, currentLevel, true);
    }
    else if ((ste->categ == S_PARAMETER) && (ste->descr.formalParam.pass == P_VARIABLE)) {
        type = ste->descr.formalParam.type;
        if (type->constr == T_ARRAY) {
            genCode2("LDVL", ste->level, ste->descr.formalParam.displ);
            type = generateINDXcode(np,resultType,type,address);
        }
        else {
            validateProccesVaiable(0,np,ste,address);
            genCode2("LDVI", ste->level, ste->descr.formalParam.displ);
        }
    }
    else if (ste->categ == S_PARAMETER) {
        type = ste->descr.formalParam.type;
        if (type->constr == T_ARRAY) {
            genCode2("LADR", ste->level, ste->descr.formalParam.displ);
            type = generateINDXcode(np,resultType,type,address);
        }
        else {
            validateProccesVaiable(0,np,ste,address);
            genCode2("LDVL", ste->level, ste->descr.formalParam.displ);
        }
    }
    else {
        type = ste->descr.variable.type;
        if (type->constr == T_ARRAY) {
            genCode2("LADR", ste->level, ste->descr.formalParam.displ);
            type = generateINDXcode(np,resultType,type,address);
        }
        else {
            validateProccesVaiable(0,np,ste,address);
            genCode2("LDVL", ste->level, ste->descr.variable.displ);
        }
    }
    return type;
}
//--- code generation for store value 
void codeGenerationAssign(int option, SymbEntryPtr ste){
    if(option==0) genCode2("STVI", ste->level, ste->descr.formalParam.displ);
    if(option==1) genCode2("STVL", ste->level, ste->descr.formalParam.displ);
    if(option==2) genCode2("STVL", ste->level, ste->descr.variable.displ);
}
//--- validate values for call function
void validFC(TreeNodePtr np, int option){
    if (option==0 && np==NULL) SemanticError(NULL);
    if (option==1 && np->categ != C_VARIABLE) SemanticError(NULL);
}
//--- process call funtion instruction
TypeDescrPtr processFunctionCall(TreeNodePtr np)
{
    SymbEntryPtr ste = searchSte(np->comps[0]->str);
    if (ste == NULL) SemanticError(NULL);    
    TypeDescrPtr resultType = NULL, returnType = NULL;
    ParamDescrPtr param = NULL;
    int level = ste->level;
    if (level >= 0) {
        returnType = (ste->categ == S_FUNCTION? ste->descr.funct.result:ste->descr.formalParam.type->descr.function.result);
        param = (ste->categ == S_FUNCTION? ste->descr.funct.params:ste->descr.formalParam.type->descr.function.params);
        if (returnType != NULL)
            genCode1("ALOC", 1);
        np = np->comps[1];
        while (true) {
            if (param==NULL) break;
            validFC(np,0);
            if (param->pass == P_VALUE)
                resultType = processExpr(np, false);
            else {
                validFC(np,1);
                resultType = processExpr(np, true);
            }
            param = param->next;
            np = np->next;
        }
        if (ste->categ == S_FUNCTION)
            genCodeLGAD_CFUN(ste->descr.funct.enterLabel, currentLevel, false);
        else 
            genCode3("CPFN", ste->level, ste->descr.variable.displ, currentLevel);
    }
    else {
        np = np->comps[1];
        if (strcmp(ste->ident, "read") == 0) {
            while (true) {
                if(np==NULL) break;
                genCode0("READ");
                validFC(np,1);
                ste = searchSte(np->comps[0]->str);
                if (ste == NULL || ((ste->categ != S_VARIABLE) && (ste->categ != S_PARAMETER)))
                    SemanticError(NULL);
                int option = ((ste->categ == S_PARAMETER) && (ste->descr.formalParam.pass == P_VARIABLE))?0:(ste->categ == S_PARAMETER?1:2);
                codeGenerationAssign(option,ste);
                np = np->next;
            }
        }
        else if (strcmp(ste->ident, "write") == 0) {
            while (true) {
                if(np==NULL) break;
                resultType = processExpr(np, false);
                genCode0("PRNT");
                np = np->next;
            }
        }
    }
    return returnType;
}
//--- process expression(type of expression:binary,unary,integer,variable and call function)
TypeDescrPtr processExpr(TreeNodePtr np, bool address)
{
    TypeDescrPtr expr;
    if(np->categ == C_BIN_EXPR) expr = processBinExpr(np);
    if(np->categ == C_UN_EXPR) expr = processUnExpr(np);
    if(np->categ == C_INTEGER) expr = processInteger(np);
    if(np->categ == C_VARIABLE) expr = processVariable(np, address);
    if(np->categ == C_FUNCTION_CALL) expr = processFunctionCall(np);
    return expr;
}
//-- process assing intruction
void processAssign(TreeNodePtr np)
{
    TreeNodePtr pvar = np->comps[0];
    TreeNodePtr pexpr = np->comps[1];
    SymbEntryPtr ste = searchSte(pvar->comps[0]->str);
    TypeDescrPtr tvar, texpr;
    if (ste == NULL || ((ste->categ != S_VARIABLE) && (ste->categ != S_PARAMETER)))
        SemanticError(NULL);
    if (ste->categ == S_VARIABLE) tvar = ste->descr.variable.type;
    else if (ste->categ == S_PARAMETER) tvar = ste->descr.formalParam.type;
    if (tvar->constr == T_ARRAY){
        tvar = processExpr(pvar, true);
        texpr = processExpr(pexpr, false);
        if (!CompatibleTypes(tvar, texpr))
            SemanticError(NULL);
        genCode1("STMV", tvar->size);
    }
    else {
        texpr = processExpr(pexpr, false);
        if (!CompatibleTypes(tvar, texpr))
            SemanticError(NULL);
        int option = ((ste->categ == S_PARAMETER) && (ste->descr.formalParam.pass == P_VARIABLE))?0:(ste->categ == S_PARAMETER?1:2);
        codeGenerationAssign(option,ste);
    }
}
//--- process goto instruction
void processGoto(TreeNodePtr np)
{
    SymbEntryPtr ste = searchSte(np->comps[0]->str);
    if (ste == NULL || ste->categ != S_LABEL)
        SemanticError(NULL);
    else 
        genCodeJump("JUMP", ste->descr.label.mepaLabel);
}
//--- process return instruction
void processReturn(TreeNodePtr np)
{
    TypeDescrPtr resultType = NULL;
    if (np->comps[0] != NULL) {
        resultType = processExpr(np->comps[0], false);
        if (!CompatibleTypes(currentFunction->descr.funct.result, resultType))
            SemanticError(NULL);
        genCode2("STVL", currentLevel, currentFunction->descr.funct.displ);
    }
    genCodeJump("JUMP", currentFunction->descr.funct.returnLabel);
}
//--- process compound statement
void processCompoundStatement(TreeNodePtr np)
{
    while(true) {
        if(np==NULL) break;
        processUnlabeledStatement(np);
        np = np->next;
    }
}
//--- code generation for intructions "while" and "if"
void codeGenerationIF(int option, int else_, int end_){
    if(option==0) genCodeJump("JMPF", end_);
    if(option==1) genCodeJump("JMPF", else_);
    if(option==2) genCodeJump("JUMP", end_);
}
void codeGenerationWhile(int option, int label){
    if(option==0) genCodeJump("JMPF", label);
    if(option==1) genCodeJump("JUMP", label);
}
//--- process conditional instruction(if and else)
void processIf(TreeNodePtr np)
{
    int else_,end_,flag=0;
    if (np->comps[2] != NULL) {
        flag = 1;
        else_ = newLabel();
    }
    end_ = newLabel(); 
    TypeDescrPtr resultType = processExpr(np->comps[0], false);
    if (resultType != predefinedBoolean)
        SemanticError(NULL);
    codeGenerationIF(flag,else_,end_);
    processCompoundStatement(np->comps[1]);
    if (flag) {
        codeGenerationIF(2,else_,end_);
        genCodeLabel(else_);
        processCompoundStatement(np->comps[2]);
    }
    genCodeLabel(end_);
}
//--- validate Statement
bool validateStatement(SymbEntryPtr st,TreeNodePtr np, int option){
    if(option==0) return (np->comps[1] == NULL) || (np->comps[1]->next == NULL);
    if(option==1) return st == NULL || st->categ != S_LABEL || st->descr.label.defined;
}
//--- process repetitive instruction(while)
void processWhile(TreeNodePtr np)
{
    TypeDescrPtr resultType;
    int init_label = newLabel(), end_label = newLabel();
    genCodeLabel(init_label);
    resultType = processExpr(np->comps[0], false);
    if (resultType != predefinedBoolean) SemanticError(NULL);
    codeGenerationWhile(0,end_label);
    processCompoundStatement(np->comps[1]);
    codeGenerationWhile(1,init_label);
    genCodeLabel(end_label);
}
//--- Process Unlabeled Statement
void processUnlabeledStatement(TreeNodePtr np)
{
    if(np!=NULL){
        switch(np->categ){
            case C_ASSIGN: processAssign(np); break;
            case C_FUNCTION_CALL: processFunctionCall(np); break;
            case C_GOTO: processGoto(np); break;
            case C_RETURN: processReturn(np); break;
            case C_IF: processIf(np); break;
            case C_WHILE: processWhile(np); break;
        }
    }
}
//--- Process estatements(Unlabeled and Compound Statement)
void processStatements(TreeNodePtr np)
{
    SymbEntryPtr st;
    while (true) {
        if(np==NULL) break;
        if (validateStatement(st,np,0)) {
            if (np->comps[0] != NULL) {
                st = searchSte(np->comps[0]->str);
                if (validateStatement(st,np,1))
                    SemanticError(NULL);
                else{
                    st->descr.label.defined = true;
                    genCodeENLB(st->descr.label.mepaLabel, currentLevel, currentDisplacement);
                }
            }
            processUnlabeledStatement(np->comps[1]);
        }
        else processCompoundStatement(np->comps[1]);
        np = np->next;
    }
}
//--- restore symbol table after of a function declaration
void restoreSymbolTable()
{
    SymbEntryPtr ste;
    while(symbolTable->level > currentLevel) {
        ste = symbolTable;
        symbolTable = symbolTable->next;
        free(ste);
    }
}
//--- process Labels, types and variables
void processHeader(TreeNodePtr p){
    processLabels(p->comps[3]->comps[0]);
    processTypes(p->comps[3]->comps[1]);
    processVariables(p->comps[3]->comps[2]);
}
//--- Function declaration 
void processFunctionDecl(TreeNodePtr p, bool ismain)
{
    char *fname = p->comps[1]->str;
    TypeDescrPtr resType = NULL, funType = NULL;
    int lastDispl = -4, lastUpDispl, entLabel, retLabel;
    SymbEntryPtr formals = NULL, func = NULL;
    currentLevel++;
    currentDisplacement = 0;
    if (p->comps[0] != NULL) resType = getType(p->comps[0]);
    
    formals = processFormals(p->comps[2], &lastDispl);

    if (resType != NULL) lastDispl -= resType->size;
    func = newSymbEntry(S_FUNCTION, fname, currentLevel - 1);
    func->descr.funct.result = resType;
    func->descr.funct.params = getParamDescr(formals);
    func->descr.funct.displ = lastDispl;
    insertSymbTable(func);
    currentFunction = func;
    if (resType != NULL) lastDispl += resType->size;
    
    if (ismain) genCode0("MAIN");
    else {
        entLabel = newLabel(); retLabel = newLabel();
        genCodeLabel1(entLabel,"ENFN", currentLevel);
        func->descr.funct.enterLabel = entLabel;
        func->descr.funct.returnLabel = retLabel;
    }

    processHeader(p);
    if (currentDisplacement > 0) genCode1("ALOC", currentDisplacement);
    loadFormalsSymbolTable(formals);
    lastUpDispl = currentDisplacement;
    processFunctions(p->comps[3]->comps[3]);
    currentDisplacement = lastUpDispl;
    currentFunction = func;
    processStatements(p->comps[3]->comps[4]);
   
    if (!ismain)
        genCodeLabel(retLabel);
    
    if (currentDisplacement > 0)
        genCode1("DLOC", currentDisplacement);
    
    if (ismain) {
        genCode0("STOP");
        genCode0("END ");
    }
    else
        genCode1("RTRN", -lastDispl - 4);
    
    currentLevel--;
    restoreSymbolTable();
}

//--- Main function, define values by default(integer, boolean, true, false, read and write)
void processProgram(void *p)
{
    SymbEntryPtr st = NULL;
    predefinedInteger = newTypeDescr(T_PREDEF, 1);
    predefinedBoolean = newTypeDescr(T_PREDEF, 1);
   
    st = newSymbEntry(S_TYPE, "integer", -1);
    st->descr.type = predefinedInteger;
    insertSymbTable(st);
    
    st = newSymbEntry(S_TYPE, "boolean", -1);
    st->descr.type = predefinedBoolean;
    insertSymbTable(st);
    
    st = newSymbEntry(S_CONST, "true", -1);
    st->descr.constant.value = 1;
    st->descr.constant.type = predefinedBoolean;
    insertSymbTable(st);
    
    st = newSymbEntry(S_CONST, "false", -1);
    st->descr.constant.value = 0;
    st->descr.constant.type = predefinedBoolean;
    insertSymbTable(st);
    
    st = newSymbEntry(S_FUNCTION, "read", -1);
    st->descr.funct.displ = 0;
    st->descr.funct.result = NULL;
    st->descr.funct.params = NULL;
    insertSymbTable(st);
    
    st = newSymbEntry(S_FUNCTION, "write", -1);
    st->descr.funct.displ = 0;
    st->descr.funct.result = NULL;
    st->descr.funct.params = NULL;
    insertSymbTable(st);
    
    processFunctionDecl((TreeNodePtr)p, true);
}
