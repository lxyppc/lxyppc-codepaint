%{
/************************************************************
*  Company :    lxyppc
* 
*  Filename:    Parser.l
*  Summary :    Lexical compiler description file
* 
*  Version :    1.0
*  Author :     lxyppc
*  CreateDate:  2009-9-18
* 
*  All rights reserved.
*************************************************************/
#include "CodePaint.h"

/** Over ride the default input method
    here we get the input from rich edit box */
extern int GetInput(char *buf, int maxlen);
#undef YY_INPUT
#define YY_INPUT(buf, retval, maxlen) {retval = GetInput(buf, maxlen);}

/* When got a new line character, invoke this function */
void NewLine(BOOL bComment);

/* log related definitions */
extern void StringLog(const char* str);
#ifdef  _DEBUG
#define     PRINTF(_X_)     {char buf[1024]=""; sprintf _X_ ; StringLog(buf);}
#else
#define     PRINTF(_X_) 
#endif

/* output string to buffer */
extern  void OutputString(const char* str, int len);
/* track current position */
extern  void TrackPosition(const char* str, int len);
/* output string directly */
#define     DIRECT_OUTPUT       TrackPosition(yytext,yyleng);OutputString(yytext,yyleng);

extern  BOOL    bNeedLineNumber;
extern  char*   lnFormatComment;
extern  char*   lnFormatNormal;
extern  char*   currentFormat;

%}


hexNumber       0[xX][0-9abcdefABCDEF]+
intNumber       [+-]?[0-9]+[Ll]?
floatNumber     [+-]?[0-9]+"."[0-9]*[fF]?
eNumber         [+-]?[0-9]+"."?[0-9]*[eE][+-]?[0-9]+
number          {hexNumber}|{intNumber}|{floatNumber}|{eNumber}

identify        [a-zA-Z_][a-zA-Z0-9_]*

PreCompile      #[ \t]*(define|error|include|elif|if|line|else|ifdef|pragma|endif|ifndef|undef)

Keyword_C1      auto|break|bool|case|char|const|continue|default|do|double|defined|else|enum
Keyword_C2      extern|float|for|goto|if|int|long|register|return|short|signed|sizeof|static
Keyword_C3      struct|switch|typedef|union|unsigned|void|volatile|while
Keyword_C4      __asm|__based|__cdecl|__declspec|__except|__far|__fastcall|__finally|__fortran|__huge
Keyword_C5      __inline|__int16|__int32|__int64|__int8|__interrupt|__leave|__loadds|__near|__pascal
Keyword_C6      __saveregs|__segment|__segname|__self|__stdcall|__try|__uuidof
Keyword_C       {Keyword_C1}|{Keyword_C2}|{Keyword_C3}|{Keyword_C4}|{Keyword_C5}|{Keyword_C6}

Keyword_CPP1    catch|class|const_cast|delete|dynamic_cast|explicit|export|false|friend|inline|mutable
Keyword_CPP2    new|namespace|operator|private|protected|public|reinterpret_cast|static_cast|template
Keyword_CPP3    this|throw|true|try|typeid|typename|using|virtual|wchar_t|__multiple_inheritance
Keyword_CPP4    __single_inheritance|__virtual_inheritance
Keyword_CPP     {Keyword_CPP1}|{Keyword_CPP2}|{Keyword_CPP3}|{Keyword_CPP4}

Keyword_MSExt   dllexport|dllimport|naked|thread|uuid

/* Additional state declaration */
/* State for process line comment */
%x      LineComment
/* State for process block comment */
%x	BlockComment
/* State for process single quote strign */
%x	DQuoteString
/* State for process double quote strign */
%x	SQuoteString


%%
                    /* Line Comment process */
\/\/                { /* Begin line comment */
                        cpSetCodeColor(currentFormat,"comment");
                        DIRECT_OUTPUT;
                        BEGIN(LineComment);
                    }
<LineComment>{
\\\r\n              {
                        DIRECT_OUTPUT;
                        NewLine(TRUE);
                    }
\r\n                {
                        cpResetCodeColor(currentFormat);
                        DIRECT_OUTPUT;
                        NewLine(FALSE);
                        BEGIN(INITIAL);
                    }
.                   {
                        DIRECT_OUTPUT;
                    }
                }   /* Line Comment process end */

                    /* Block Comment process */
"/*"                {   /* Block Comment start */ 
                        cpSetCodeColor(currentFormat,"comment");
                        DIRECT_OUTPUT;
                        BEGIN(BlockComment);
                    }
<BlockComment>{
"*/"|\*\\\r\n\/     {   /* Block Comment end */   
                        DIRECT_OUTPUT;
                        cpResetCodeColor(currentFormat);
                        BEGIN(INITIAL); 
                    }
\n                  {   /* Block Comment meet newline */
                        DIRECT_OUTPUT;
                        NewLine(TRUE);
                    }
.                   {   /* Block Comment */ 
                        DIRECT_OUTPUT;
                    }
                }   /* Block Comment process end */


{PreCompile}        {   /* pre compile command */
                        PRINTF( (buf,"Get PreCompile: %s", yytext) ); 
                        cpSetCodeColor(currentFormat,"keyword");
                        DIRECT_OUTPUT;
                        cpResetCodeColor(currentFormat);
                    }


                    /* Single quote string process */
'                   {   /* Single quote start */
                        PRINTF( (buf,"SString Start") );
                        cpSetCodeColor(currentFormat,"string");
                        DIRECT_OUTPUT;
                        BEGIN(SQuoteString);}
<SQuoteString>{
\n                  { /* Single quote end */ 
                        PRINTF( (buf,"SString Error!") );
                        DIRECT_OUTPUT;
                        cpResetCodeColor(currentFormat);
                        BEGIN(INITIAL); 
                    }
'                   {
                        PRINTF( (buf,"SString End") );
                        DIRECT_OUTPUT;
                        cpResetCodeColor(currentFormat);
                        BEGIN(INITIAL);
                    }
\\'                 {
                        DIRECT_OUTPUT;
                        PRINTF( (buf,"%s",yytext) );
                    }
.                   {
                        DIRECT_OUTPUT;
                        PRINTF( (buf,"%s",yytext) );
                    }
                }   /* Single quote string process end*/


                    /* Double quote string process */
\"                  {   /* Double quote start */
                        PRINTF( (buf,"DString Start") );
                        cpSetCodeColor(currentFormat,"string");
                        DIRECT_OUTPUT;
                        BEGIN(DQuoteString);}
<DQuoteString>{
\n                  {   /* Double quote end */ 
                        PRINTF( (buf,"DString Error!") );
                        DIRECT_OUTPUT;
                        cpResetCodeColor(currentFormat);
                        BEGIN(INITIAL); 
                    }
\"                  {
                        PRINTF( (buf,"DString End") ); 
                        DIRECT_OUTPUT;
                        cpResetCodeColor(currentFormat);
                        BEGIN(INITIAL);
                    }
\\\"                {
                        PRINTF( (buf,"%s",yytext) );
                        DIRECT_OUTPUT;
                    }
.                   {
                        PRINTF( (buf,"%s",yytext) );
                        DIRECT_OUTPUT;
                    }
                }   /* Double quote string process end*/
           

\n                  {DIRECT_OUTPUT;NewLine(FALSE);}


{Keyword_C}         { /* Keywords for C*/  
                        PRINTF( (buf,"Get Keywords for C: %s", yytext) );
                        cpSetCodeColor(currentFormat,"keyword");
                        DIRECT_OUTPUT;
                        cpResetCodeColor(currentFormat);
                    }

{Keyword_CPP}       { /* Keywords for C++ */
                        PRINTF( (buf,"Get Keywords for C++: %s", yytext) );
                        cpSetCodeColor(currentFormat,"keyword");
                        DIRECT_OUTPUT;
                        cpResetCodeColor(currentFormat);
                    }

{Keyword_MSExt}     { /* Keywords for extension */ 
                        PRINTF( (buf,"Get Extension Keywords: %s", yytext) );
                        cpSetCodeColor(currentFormat,"keyword");
                        DIRECT_OUTPUT;
                        cpResetCodeColor(currentFormat);
                    }


{identify}          {/* Not used output directly */DIRECT_OUTPUT;}

{number}            {    /* Here we get a number! */
                        PRINTF( (buf,"Get Number: %s", yytext) );
                        cpSetCodeColor(currentFormat,"number");
                        DIRECT_OUTPUT;
                        cpResetCodeColor(currentFormat);
                    }

.                   {   /* Match nothing */
                        DIRECT_OUTPUT; 
                    }


%%


/*
 ************************************************
 * Helper functions used for identify the token *
 ************************************************
 */
int linesCount = 0;
/* increase the linesCount and output line number if need */
void NewLine(BOOL bComment)
{
    linesCount++;
    if(bNeedLineNumber){
        cpSetCodeColor(currentFormat,"linenumber",FALSE);
        char buf[1024]="";
        if(bComment){
            sprintf(buf,lnFormatComment,linesCount);
        }else{
            sprintf(buf,lnFormatNormal,linesCount);
        }
        OutputString(buf,0);
        cpResetCodeColor(currentFormat,FALSE);
    }
}

int  yywrap(void)
{
    return 1;
}

void    ParseStart(void)
{
    linesCount = 0;
    if(bNeedLineNumber){
    /* Output the first line number if need */
        NewLine(FALSE);
    }
}