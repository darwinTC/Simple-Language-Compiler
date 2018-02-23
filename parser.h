/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_PARSER_H_INCLUDED
# define YY_YY_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    EQUAL = 258,
    DIFFERENT = 259,
    LESS = 260,
    LESS_OR_EQUAL = 261,
    GREATER = 262,
    GREATER_OR_EQUAL = 263,
    PLUS = 264,
    MINUS = 265,
    OR = 266,
    MULTIPLY = 267,
    DIV = 268,
    AND = 269,
    CLOSE_BRACE = 270,
    CLOSE_BRACKET = 271,
    CLOSE_PAREN = 272,
    COLON = 273,
    COMMA = 274,
    ELSE = 275,
    END_OF_FILE = 276,
    FUNCTIONS = 277,
    GOTO = 278,
    IDENTIFIER = 279,
    ASSIGN = 280,
    IF = 281,
    INTEGER = 282,
    LABELS = 283,
    NOT = 284,
    OPEN_BRACE = 285,
    OPEN_BRACKET = 286,
    OPEN_PAREN = 287,
    RETURN = 288,
    SEMI_COLON = 289,
    TYPES = 290,
    VAR = 291,
    VARS = 292,
    VOID = 293,
    WHILE = 294,
    UNFINISHED_COMMENT = 295,
    LEXICAL_ERROR = 296
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_PARSER_H_INCLUDED  */
