%option noyywrap nodefault yylineno
%x IFILE CODEBLOCK COMMENT

NEWLINE (\r\n|\n|\r)

%{
#include <stdlib.h>
#include <string.h>
#if defined(_WIN32)
#include <io.h>
#define isatty _isatty
#define fileno _fileno
#endif
#include "idlcpp.y.h"
#include "yyfuncs.h"

YY_BUFFER_STATE createBufferState(FILE* file);
int yytokenno = 0;
int yycolumnno = 1;
int yyHasListProperty = 0;
%}

%%

"#import"[ \t]*\"	{BEGIN IFILE;}
<IFILE>[^\"\r\n]+		{
						{
							int c;
							while (0 != (c = yyinput()))
							{
								if ('\r' == c)
								{
									c = yyinput();
									if (0 != c && '\n' != c)
									{
										unput(c);
									}
									break;
								}
								if ('\n' == c)
								{
									break;
								}
							}
							addSourceFile(yytext);
							yycolumnno = 1;
							BEGIN INITIAL;
						}
					}
<IFILE>.|{NEWLINE}			{fprintf(stderr, "%d bad import line\n", yylineno); yyterminate();}

"#{"						{yycolumnno += 2; BEGIN CODEBLOCK;}
<CODEBLOCK>"#}"				{yycolumnno += 2; BEGIN INITIAL;}
<CODEBLOCK>[^#\r\n]+		{newCodeBlock(yytext); yycolumnno += strlen(yytext); yytokenno++;}
<CODEBLOCK>{NEWLINE}		{newCodeBlock("\n"); yycolumnno = 1; yytokenno++;}
<CODEBLOCK>.				{newCodeBlock(yytext); yycolumnno++; yytokenno++;}
<CODEBLOCK><<EOF>>			{unterminatedCode();yyterminate();}

"/*"						{yycolumnno += 2; BEGIN COMMENT;}
<COMMENT>"*/"				{yycolumnno += 2; BEGIN INITIAL;}
<COMMENT>[^*\r\n]+			{yycolumnno += strlen(yytext); }
<COMMENT>{NEWLINE}			{newCodeBlock("\n"); yycolumnno = 1; }
<COMMENT>.					{yycolumnno++;}
<COMMENT><<EOF>>			{unterminatedComment();yyterminate();}



"//".*{NEWLINE}				{newCodeBlock("\n"); yycolumnno = 1;}

<<EOF>>						{yyterminate();}

\#\#.*						{newCodeBlock(yytext + 2); yycolumnno += strlen(yytext); yytokenno++;}

\#[a-zA-Z_0-9]+				{newCodeBlock(yytext + 1); yycolumnno += strlen(yytext); yytokenno++;}

"%"	|
"^" |
"&" |
"*" |
"=" |
"<" |
">" |
"{"	|
"}"	|
"["	|
"]"	|
"("	|
")"	|
","	|
";"	|
":"	|
"?"												{yylval.sn = newToken(yytext[0]);					yycolumnno++; yytokenno++; return yytext[0];}

"nometa"										{yylval.sn = newToken(snt_keyword_nometa);			yycolumnno += strlen(yytext); yytokenno++; return NOMETA;}
"nocode"										{yylval.sn = newToken(snt_keyword_nocode);			yycolumnno += strlen(yytext); yytokenno++; return NOCODE;}
"void"											{yylval.sn = newToken(snt_keyword_void);			yycolumnno += strlen(yytext); yytokenno++; return VOID;}
"::"											{yylval.sn = newToken(snt_keyword_scope);			yycolumnno += strlen(yytext); yytokenno++; return SCOPE;}
"bool"											{yylval.sn = newToken(snt_keyword_bool);			yycolumnno += strlen(yytext); yytokenno++; return BOOL;}
"char"											{yylval.sn = newToken(snt_keyword_char);			yycolumnno += strlen(yytext); yytokenno++; return CHAR;}
"wchar_t"										{yylval.sn = newToken(snt_keyword_wchar_t);			yycolumnno += strlen(yytext); yytokenno++; return WCHAR_T;}
"short"											{yylval.sn = newToken(snt_keyword_short);			yycolumnno += strlen(yytext); yytokenno++; return SHORT;}
"long"											{yylval.sn = newToken(snt_keyword_long);			yycolumnno += strlen(yytext); yytokenno++; return LONG;}
"int"											{yylval.sn = newToken(snt_keyword_int);				yycolumnno += strlen(yytext); yytokenno++; return INT;}
"signed"										{yylval.sn = newToken(snt_keyword_signed);			yycolumnno += strlen(yytext); yytokenno++; return SIGNED;}
"unsigned"										{yylval.sn = newToken(snt_keyword_unsigned);		yycolumnno += strlen(yytext); yytokenno++; return UNSIGNED;}
"float"											{yylval.sn = newToken(snt_keyword_float);			yycolumnno += strlen(yytext); yytokenno++; return FLOAT;}
"double"										{yylval.sn = newToken(snt_keyword_double);			yycolumnno += strlen(yytext); yytokenno++; return DOUBLE;}
"string_t"										{yylval.sn = newToken(snt_keyword_string_t);		yycolumnno += strlen(yytext); yytokenno++; return STRING_T;}
"namespace"										{yylval.sn = newToken(snt_keyword_namespace);		yycolumnno += strlen(yytext); yytokenno++; return NAMESPACE;}
"enum"											{yylval.sn = newToken(snt_keyword_enum);			yycolumnno += strlen(yytext); yytokenno++; return ENUM;}
"class"											{yylval.sn = newToken(snt_keyword_class);			yycolumnno += strlen(yytext); yytokenno++; return CLASS;}
"struct"										{yylval.sn = newToken(snt_keyword_struct);			yycolumnno += strlen(yytext); yytokenno++; return STRUCT;}
"interface"										{yylval.sn = newToken(snt_keyword_interface);		yycolumnno += strlen(yytext); yytokenno++; return INTERFACE;}
"virtual"										{yylval.sn = newToken(snt_keyword_virtual);			yycolumnno += strlen(yytext); yytokenno++; return VIRTUAL;}
"static"										{yylval.sn = newToken(snt_keyword_static);			yycolumnno += strlen(yytext); yytokenno++; return STATIC;}
"typedef"										{yylval.sn = newToken(snt_keyword_typedef);			yycolumnno += strlen(yytext); yytokenno++; return TYPEDEF;}
"typename"										{yylval.sn = newToken(snt_keyword_typename);		yycolumnno += strlen(yytext); yytokenno++; return TYPENAME;}
"get"											{yylval.sn = newToken(snt_keyword_get);				yycolumnno += strlen(yytext); yytokenno++; return GET;}
"set"											{yylval.sn = newToken(snt_keyword_set);				yycolumnno += strlen(yytext); yytokenno++; return SET;}
"export"										{yylval.sn = newToken(snt_keyword_export);			yycolumnno += strlen(yytext); yytokenno++; return EXPORT;}
"template"										{yylval.sn = newToken(snt_keyword_template);		yycolumnno += strlen(yytext); yytokenno++; return TEMPLATE;}

[a-zA-Z_][a-zA-Z_0-9]*							{yylval.sn = newIdentifier(yytext);					yycolumnno += strlen(yytext); yytokenno++; return IDENTIFIER;}

\"[^\"]*\"										{yylval.sn = newString(yytext + 1);					yycolumnno += strlen(yytext); yytokenno++; return STRING;}
u8\"[^\"]*\"									{yylval.sn = newString(yytext + 3);					yycolumnno += strlen(yytext); yytokenno++; return U8STRING;}

[ \t]											{													++yycolumnno;}
{NEWLINE}										{newCodeBlock("\n");								yycolumnno = 1;}

[^ ,;:{}\[\]<>()&*^$"/+%^|!~=?\-\t\r\n]*		{invalidString(yytext);								yycolumnno += strlen(yytext);}
.												{invalidString(yytext);								yycolumnno += strlen(yytext);}

%%

YY_BUFFER_STATE createBufferState(FILE* file)
{
	YY_BUFFER_STATE bs;
	bs = yy_create_buffer(file, YY_BUF_SIZE);
	return bs;
}