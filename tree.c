#include <stdio.h> 
#include "tree.h"
#include <stdlib.h>

/*Number of instruction for each rule in the parser.y*/
int NumberComp[]={0,4,5,2,3,2,2,2,3,2,2,2,2,3,2,2,1,1,2,0,0,0};
/*Associate each index with its respective identifier(text)*/
char* NameType(int a)
{
    if(a==1) return "C_FUNCTION";    
    if(a==2) return "C_BLOCK"; 
    if(a==3) return "C_TYPE"; 
    if(a==4) return "C_BIN_EXPR"; 
    if(a==5) return "C_UN_EXPR"; 
    if(a==6) return "C_ASSIGN"; 
    if(a==7) return "C_WHILE"; 
    if(a==8) return "C_IF"; 
    if(a==9) return "C_FUNCTION_CALL"; 
    if(a==10) return "C_VARS"; 
    if(a==11) return "C_TYPE_DATA"; 
    if(a==12) return "C_PARAMETER"; 
    if(a==13) return "C_FUCTION_PARAMETER"; 
    if(a==14) return "C_STATEMENT"; 
    if(a==15) return "C_VARIABLE";
    if(a==16) return "C_GOTO";
    if(a==17) return "C_RETURN";
    if(a==18) return "C_REF_PARAMETER";
    if(a==19) return "C_IDENT";
    if(a==20) return "C_INTEGER";
    if(a==21) return "C_OPERATOR";
}

/*declaration of the tree with size MAX_STACK*/
TreeNodePtr stack[MAX_STACK_SIZE];
/*index of top of the tree*/
int top = 0;
/*return the root for the tree*/
void *getTree(){ 
    //dumpTree(stack[top-1], 0, "    ");
    return (void*)stack[top-1]; 
}
/*generate a new node associating the "n" nodes in the top, this new node will have with text "s"*/
void genNode3(Categ cat, int n, char *s) {
    int i;
    TreeNodePtr np = malloc(sizeof(TreeNode));
    np->categ = cat;
    np->next = NULL;
    np->str = s;
    for ( i=0; i<n; i++ )
        np->comps[i] = stack[top-n+i];
    top -= (n-1);
    //printf("%d : %s ==> %s \n",top, NameType(cat),s);
    stack[top-1] = np;
}
/*generate a new node associating the "n nodes in the top, this node do not have text*/
void genNode(Categ cat, int n) {
    genNode3(cat, n, NULL);
}
/*Insert the top node in "next" of the node of the previous one*/
void insertTopList() {
    // Reversed list!
    TreeNodePtr t = stack[--top];
    TreeNodePtr s = stack[top-1];
    t->next = s;
    stack[top-1] = t;
}
/*Insert the top node in the previous one*/
void insertEndList(){
    TreeNodePtr t = stack[--top];
    TreeNodePtr s = stack[top-1];
    if(t!=NULL)
    {
        TreeNodePtr n = t;
        while(n->next != NULL)
            n = n -> next;
        s->next = n->next;
        n->next = s;
        stack[top-1] = t;
    }
}
/*reverse the comps since the index idx to the beginning*/
void rotate(int idx)
{
    idx=idx-1;
    TreeNodePtr t = stack[top-1]->comps[idx];
    TreeNodePtr s = NULL;
    while(t != NULL) {
        TreeNodePtr n = t->next;
        t->next = s;
        s = t;
        t = n;
    }
    stack[top-1]->comps[idx] = s;
}
void rotateList(int begin, int end){
    for(int i=begin; i<=end; i++)
        rotate(i);
}
/*insert a node empty before of "count" position in the stack*/
void genEmpty2(int count)
{
    stack[top] = stack[top-1];
    stack[top-count] = NULL;
    top++;
}
/*insert a node empty in the top position of the stack*/
void genEmpty() {
    stack[top] = NULL;
    top++;
}
/*count the number of times of the "cat" element appears on the stack*/
int count(TreeNodePtr p, Categ cat){
    int cont=0;
    if(p==NULL) return cont;

    if(p->categ==cat) cont++; 

    if(p->next != NULL) 
        cont=cont+count(p->next,cat);  

    for(int i=0; i<NumberComp[p->categ] ; i++)
        cont=cont+count(p->comps[i],cat);

    return cont;
}
/*print the tree*/
void dumpTree(TreeNodePtr np, int number, char* space)
{
    int aux=number;
    while(aux--)
        fprintf(stderr, "%s", space);
    if (np == NULL) {
        fprintf(stderr, "Empty\n");
        fflush(stderr);
        return;
    }   
    fprintf(stderr, "%s: %s\n", NameType(np->categ), ((np->str != NULL)? np->str : ""));
    fflush(stderr);
    
    for(int i=0; i < NumberComp[np->categ]; i++)
        dumpTree(np->comps[i], number + 1, space);
    
    if (np->next != NULL)
        dumpTree(np->next, number, space);
}
/*count the number of times of the elements "C_FUNTION, C_FUNCTION_CALL, C_WHILE, C_IF and C_BIN_EXPR" appears on the stack*/
void counts(void *p, int *functions, int *funcalls, int *whiles, int *ifs, int *bin){
    TreeNodePtr np = (struct _treeNode *)p;
    *functions=count(np, C_FUNCTION);
    *funcalls=count(np, C_FUNCTION_CALL);
    *whiles=count(np, C_WHILE);
    *ifs=count(np, C_IF);
    *bin=count(np, C_BIN_EXPR);
}
