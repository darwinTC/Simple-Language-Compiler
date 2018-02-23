%{
#include "parser.h"

int fileno(FILE *stream);

int line_num=1;
int lexerror=0;
void yyerror(char *s);
extern char *tok_val;
%}

%x comment

%{
/* Identifiers and integers */
%}

DIGIT [0-9][0-9]*
ID [a-z][a-z0-9]*
SINGLELINE_COMMENT ("//".*)

%%

"/*"         BEGIN(comment);

<comment>[^*\n]*        /* eat anything that's not a '*' */
<comment>"*"+[^*/\n]*   /* eat up '*'s not followed by '/'s */
<comment>\n             ++line_num;
<comment>"*"+"/"        BEGIN(INITIAL);

\n      		line_num++;
[ \t]			;
{SINGLELINE_COMMENT} ;

%{
/* Reserved words */
%}

else  			return(ELSE); 
if  			return(IF); 
while 			return(WHILE);

%{
/* Symbols */
%}

"=="			return(EQUAL);
"="			return(ASSIGN);
"!="			return(DIFFERENT);
"!"			return(NOT);
"<"			return(LESS);
"<="			return(LESS_OR_EQUAL);
">"			return(GREATER);
">="			return(GREATER_OR_EQUAL);
"||"			return(OR);
"&&"			return(AND);
","			return(COMMA);
":"			return(COLON);
var			return(VAR);
vars			return(VARS);
void			return(VOID);
goto			return(GOTO);
types			return(TYPES);
labels			return(LABELS);
functions		return(FUNCTIONS);
return 			return(RETURN);
"("			return(OPEN_PAREN);
")"			return(CLOSE_PAREN);
"{"                     return(OPEN_BRACE);
"}"                     return(CLOSE_BRACE);
"["			return(OPEN_BRACKET);
"]"			return(CLOSE_BRACKET);
{ID} 			{strcpy((tok_val=malloc(yyleng+1)),yytext);return IDENTIFIER;}
{DIGIT}			{strcpy((tok_val=malloc(yyleng+1)),yytext); return INTEGER;}
"+"			return(PLUS);
"-"			return(MINUS);
"*"			return(MULTIPLY);
"/"                     return(DIV);
";"			return(SEMI_COLON);
<comment><<EOF>> 	return(UNFINISHED_COMMENT);
<<EOF>>  		return(END_OF_FILE);
.       		{lexerror=1;return(LEXICAL_ERROR);}

%%

int yywrap(){
        return 1;
        }

