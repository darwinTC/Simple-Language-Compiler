/*The implementation of the rules of this code was based on the material of the course.*/
%{
#include <stdio.h>
#include "tree.h"
int yylex();
char *yytext;
char *tok_val;
void yyerror(char *s);

%}

%token EQUAL DIFFERENT LESS LESS_OR_EQUAL GREATER GREATER_OR_EQUAL PLUS MINUS OR
%token MULTIPLY DIV AND CLOSE_BRACE CLOSE_BRACKET CLOSE_PAREN COLON COMMA ELSE END_OF_FILE
%token FUNCTIONS GOTO IDENTIFIER ASSIGN IF INTEGER LABELS NOT OPEN_BRACE OPEN_BRACKET
%token OPEN_PAREN RETURN SEMI_COLON TYPES VAR VARS VOID WHILE UNFINISHED_COMMENT LEXICAL_ERROR

%%
    //--- END_OF_FILE, end of the program
    program : function END_OF_FILE {return 0;}
            ;

    void
            : VOID { genEmpty(); }
            ;

    //--- function = identifier + identifier + parameter + content
    function 
            : identifier identifier formal_parameters block {genNode(C_FUNCTION,4);rotateList(3,3);}
            | void identifier formal_parameters block {genNode(C_FUNCTION,4);rotateList(3,3);}
            ;
    //--- content = label + types + variables + function + content
    block  
            : labels types variables functions body {genNode(C_BLOCK,5);rotateList(1,5);}
            ;
    //--- define label, types and funtions
    labels 
            : LABELS identifier_list SEMI_COLON
            | empty
            ;
    types   
            : TYPES types_
            | empty
            ;

    //--- Aux method: List of types
    types_
            : identifier ASSIGN type SEMI_COLON {genNode(C_TYPE,2);}
            | types_ identifier ASSIGN type SEMI_COLON {genNode(C_TYPE,2); insertTopList();}
            ;

    variables
            : VARS variables_
            | empty
            ;

    //--- Aux method: List of variables
    variables_
            : identifier_list COLON type SEMI_COLON {genNode(C_VARS,2);rotateList(1,1);}
            | variables_ identifier_list COLON type SEMI_COLON {genNode(C_VARS,2); rotateList(1,1);insertTopList();}
            ;
	
    functions
            : FUNCTIONS functions_
            | empty
            ;
	
	//--- Aux method: List of functions
    functions_
            : function
            | functions_ function {insertTopList();}
            ;
    body
            : OPEN_BRACE body_ CLOSE_BRACE
            ;

	//--- Aux method: content of statements  
    body_   
            : body_ statement  {insertTopList();}
            | empty
            ;
    
    //--- define type of date
    type
            : identifier type_ {genNode(C_TYPE_DATA,2);rotateList(2,2);}
            ;

	//--- Aux method: bracketed integer 
    type_
            : type_ OPEN_BRACKET integer CLOSE_BRACKET {insertTopList();}
            | empty
            ;
    //--- define formal parameters
    formal_parameters
            : OPEN_PAREN empty CLOSE_PAREN
            | OPEN_PAREN formal_parameter formal_parameters_ CLOSE_PAREN {insertEndList();}
            ;

	//--- Aux method: formal declaration of parameters
    formal_parameters_
			: formal_parameters_ SEMI_COLON formal_parameter {insertTopList();}
            | empty
            ;

    //--- define a paremeter
    formal_parameter
            : expression_parameter
            | function_parameter
            ;
    
    //--- define expression parameter : VAR
    expression_parameter
            : VAR identifier_list COLON identifier {genNode(C_REF_PARAMETER,2);rotateList(1,1);}
            | identifier_list COLON identifier {genNode(C_PARAMETER,2);rotateList(1,1);}
            ;

    //--- define function parameter
    function_parameter
            : identifier identifier formal_parameters {genNode(C_FUCTION_PARAMETER,3);rotateList(3,3);}
            | void identifier formal_parameters {genNode(C_FUCTION_PARAMETER,3);rotateList(3,3);}
            ;
    //--- type of statement: unlabeled statemen and compound
    statement
            : identifier COLON unlabeled_statement {genNode(C_STATEMENT,2);}
            | unlabeled_statement {genEmpty2(1);genNode(C_STATEMENT,2);}
            | compound {genEmpty2(1);genNode(C_STATEMENT,2);}
            ;
    //--- define variable
    variable
            : identifier variable_ {genNode(C_VARIABLE,2);rotateList(2,2);}
            ;
    variable_
            : variable_ OPEN_BRACKET expression CLOSE_BRACKET {insertTopList();}
            | empty
            ;
    //--- define unlabeled statement: assignment, call function, goto, return, if, while and empty estatement
    unlabeled_statement
            : assignment
            | function_call_statement
            | goto
            | return 
            | conditional 
            | repetitive 
            | empty_statement 
            ;
    //--- define assignment
    assignment
            : variable ASSIGN expression SEMI_COLON {genNode(C_ASSIGN,2);}
            ;

	//--- function_call
    function_call_statement
            : function_call SEMI_COLON
            ;

    //--- define goto
    goto
            : GOTO identifier SEMI_COLON {genNode(C_GOTO,1);}
            ;

    //--- define return 
    return 
            : RETURN expression SEMI_COLON {genNode(C_RETURN,1);}
            | RETURN SEMI_COLON {genEmpty(); genNode(C_RETURN,1);}
            ;
    //--- define compound statement
    compound
            : OPEN_BRACE unlabeled_statement_ CLOSE_BRACE
            ;

    //--- Aux method
    unlabeled_statement_
            : unlabeled_statement
            | unlabeled_statement_ unlabeled_statement {insertTopList();}
            ;
    //--- define conditional(if and else case)
    conditional
            : IF OPEN_PAREN expression CLOSE_PAREN compound ELSE compound {genNode(C_IF,3);rotateList(2,3);}
            | IF OPEN_PAREN expression CLOSE_PAREN compound {genEmpty();genNode(C_IF,3);rotateList(2,2);}
            ;
    //--- define repetitive(while case)
    repetitive
            : WHILE OPEN_PAREN expression CLOSE_PAREN compound {genNode(C_WHILE,2);rotateList(2,2);}
            ;
    //--- define empty statement
    empty_statement
            : SEMI_COLON { genEmpty(); }
            ;
    //--- define expression
    expression
            : simple_expression relational_operator simple_expression {genNode(C_BIN_EXPR,3);}
            | simple_expression
            ;
    //--- define type relational operate for expressions
    relational_operator
            : EQUAL {genNode3(C_OPERATOR,0,"==");}
            | DIFFERENT {genNode3(C_OPERATOR,0,"!=");}
            | LESS {genNode3(C_OPERATOR,0,"<");}
            | LESS_OR_EQUAL {genNode3(C_OPERATOR,0,"<=");}
            | GREATER_OR_EQUAL {genNode3(C_OPERATOR,0,">=");}
            | GREATER {genNode3(C_OPERATOR,0,">");}
            ;
	//--- define simple expression
    simple_expression
            : header_simple simple_expression_
            ;

    header_simple
            : operation1 term  {genNode(C_UN_EXPR,2);}
            | term
            ;

    //--- Aux method
    operation1
            : PLUS {genNode3(C_OPERATOR,0,"+");}
            | MINUS {genNode3(C_OPERATOR,0,"-");}
            ;

    //--- Aux method
    simple_expression_ 
            : simple_expression_ additive_operator term {genNode(C_BIN_EXPR,3);}
            | /* empty */ 
            ;
    //--- define operator for a binary expression(additive operations)
    additive_operator
            : PLUS {genNode3(C_OPERATOR,0,"+");}
            | MINUS {genNode3(C_OPERATOR,0,"-");}
            | OR {genNode3(C_OPERATOR,0,"||");}
            ;
    //--- define term for multiplicative operator
    term
            : term multiplicative_operator factor {genNode(C_BIN_EXPR,3);}
            | factor 
            ;
    //--- multiplicative operator for a binary expression
    multiplicative_operator
            : MULTIPLY {genNode3(C_OPERATOR,0,"*");}
            | DIV {genNode3(C_OPERATOR,0,"/");}
            | AND {genNode3(C_OPERATOR,0,"&&");}
            ;
    //--- define type of factor(variable, integer, call funtion, etc)
    factor
            : variable 
            | integer 
            | function_call
            | OPEN_PAREN expression CLOSE_PAREN
            | not factor {genNode(C_UN_EXPR,2);}
            ;
    //--- define call function
    function_call
            : identifier OPEN_PAREN expression_list CLOSE_PAREN {genNode(C_FUNCTION_CALL,2);rotateList(2,2);}
            ;
    //--- define list of identifier 
    identifier_list
            : identifier_list COMMA identifier  {insertTopList();}
            | identifier
            ;
     //--- define expresion list 
    expression_list
            : expression expression_list_ {insertEndList();}
            | empty
            ;

    //--- Aux method: more expression for the rule above
    expression_list_
            : expression_list_ COMMA expression {insertTopList();}
            | empty			
            ;
    //--- define integer
    integer
            : INTEGER { genNode3(C_INTEGER,0,tok_val); }
            ;
    //--- define identifier
    identifier
            : IDENTIFIER { genNode3(C_IDENT,0,tok_val); }
            ;
    //--- empty rule
    empty
            : /* empty! */ { genEmpty(); }
            ;
    //--- define not operation
    not
            : NOT {genNode3(C_OPERATOR,0,"!");}
            ;
%%


