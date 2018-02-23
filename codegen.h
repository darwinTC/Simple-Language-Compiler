#ifndef TREE_H
        #include "tree.h"
#endif 

#include <stdbool.h>
/* category of a entry*/
typedef enum {
    S_CONST = 1,
    S_VARIABLE,
    S_PARAMETER,
    S_FUNCTION,
    S_LABEL,
    S_TYPE
} SymbCateg;

typedef struct _typeDescr           *TypeDescrPtr;
typedef struct _paramDescr          *ParamDescrPtr;
typedef struct _symbEntry           *SymbEntryPtr;

/*Type of constructor*/
typedef enum {
    T_PREDEF = 1,
    T_ARRAY,
    T_FUNCTION
} TypeConstr;

/* desciption of array*/
typedef struct _CArray {
    TypeDescrPtr      element;
    int               dimen; 
} CArray;

/*
 * Desciption of function type constuction
 */
typedef struct _CFunction {
    TypeDescrPtr      result;
    ParamDescrPtr     params;
} CFunction;

typedef struct _typeDescr {
    TypeConstr       constr;
    int              size;
    union {
        CArray    array;
        CFunction function;
    }descr;
} TypeDescr;

/* type of passaging */
typedef enum { P_VALUE = 1, P_VARIABLE } Passage;

/* description of the constant*/
typedef struct _constant {
    int          value;
    TypeDescrPtr type;
} Constant;

/* description of a variable*/
typedef struct _variable {
    int          displ;
    TypeDescrPtr type;
} Variable;

/* description of formal parameter*/
typedef struct _formalParameter{
    int          displ;
    TypeDescrPtr type;
    Passage      pass;
} FormalParameter;

/* description of a function*/
typedef struct _function {
    int            enterLabel;
    int            returnLabel;
    int            displ;
    TypeDescrPtr   result;
    ParamDescrPtr  params;
} Function;

/* description of a label*/
typedef struct _label{
    int  mepaLabel; 
    bool defined;
} Label;

typedef struct _paramDescr {
    TypeDescrPtr      type;
    Passage           pass;
    ParamDescrPtr     next;
} ParamDescr;

typedef struct _symbEntry {
    SymbCateg      categ;
    char          *ident;
    int            level;
    SymbEntryPtr   next;
    union {
        Constant          constant;
        Variable          variable;
        FormalParameter   formalParam;
        Function          funct;
        Label             label;
        TypeDescrPtr      type;
    }descr;
} SymbEntry;

int stackHeight();
void processProgram(void *p);
SymbEntryPtr processFormals(TreeNodePtr np, int *displacement);
void processFunctionDecl(TreeNodePtr p, bool ismain);
TypeDescrPtr processExpr(TreeNodePtr np, bool address);
void processUnlabeledStatement(TreeNodePtr np);
