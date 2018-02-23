#ifndef TREE_H
#define TREE_H

#include <stdio.h>
#define MAX_COMPS 10000
#define MAX_STACK_SIZE 10000

typedef enum
{ C_FUNCTION = 1,
    C_BLOCK,
    C_TYPE,
    C_BIN_EXPR,
    C_UN_EXPR,
    C_ASSIGN,
    C_WHILE,
    C_IF,
    C_FUNCTION_CALL,
    C_VARS,
    C_TYPE_DATA,
    C_PARAMETER,
    C_FUCTION_PARAMETER,
    C_STATEMENT,
    C_VARIABLE,
    C_GOTO,
    C_RETURN,
    C_REF_PARAMETER,
    C_IDENT,
    C_INTEGER,
    C_OPERATOR
} Categ;

typedef struct _treeNode {
	Categ categ;
	struct _treeNode *next; // list
	struct _treeNode *comps[5];
	char * str; // IDENT, INTEGER
	// or operator

} TreeNode, *TreeNodePtr;

extern int top;
void genIdent(char *tok_val);
void genNode3(Categ cat, int n, char *s);
void genNode(Categ cat, int n);
void insertTopList();
void insertEndList();
void genEmpty();
void rotateList(int begin, int end);
char* NameType(int a);
void genEmpty2(int count);
int count(TreeNodePtr p, Categ cat);
void dumpTree(TreeNodePtr np, int number, char* space);
void *getTree();  // returns any pointer
void counts(void *p, int *functions, int *funcalls, int *whiles, int *ifs, int *bin);
#endif 
