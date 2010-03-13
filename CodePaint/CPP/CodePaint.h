#pragma once


#include <map>
#include <string>
#include <vector>
using   std::map;
using   std::string;
using   std::wstring;
using   std::vector;

wstring toWString(const string& str);
string toString(const wstring& str);

struct cpColorSF{
    string set;
    string reset;
    string startTag;
    string endTag;
    BOOL   bNeedReplaceHtml;
    cpColorSF& operator=(const cpColorSF& rhs){
        set = rhs.set;
        reset = rhs.reset;
        startTag = rhs.startTag;
        endTag = rhs.endTag;
        bNeedReplaceHtml = rhs.bNeedReplaceHtml;
        return *this;
    }
    cpColorSF():bNeedReplaceHtml(FALSE){}
    cpColorSF(const cpColorSF& rhs){
        set = rhs.set;
        reset = rhs.reset;
        startTag = rhs.startTag;
        endTag = rhs.endTag;
        bNeedReplaceHtml = rhs.bNeedReplaceHtml;
    }
};

struct  cpCodeColor{
    COLORREF    color;
    int         start;
    int         end;
};

struct  cpUISetting
{
    BOOL        bNeedLineNumber;
    BOOL        bAutoCopy;
    string      defaultStyle;
    BOOL        bTab2Space;
    int         tabSize;
    RECT        windowRect;
};

typedef     map<string,COLORREF>    cpColorMap_t;
typedef     map<string,cpColorSF>   cpColorFormat_t;

extern  void    OutputString(const char* str, int len, BOOL bNeedTrack = FALSE);
extern  int     curPos;

void    cpInitialColorMap();
void    cpGetUISetting(cpUISetting& uiSetting);
void    cpSetUISetting(const cpUISetting& uiSetting);
void    cpResetColorStack();
void    cpGetColorStack(vector<cpCodeColor>& colorStack);
void    cpGetStyleList(vector<string>& styleList);
void    cpSetColor(const string& colorName, COLORREF color);

void    cpSetCodeColor(const string& formatName, const string& colorName, BOOL bNeedTrack = TRUE);
void    cpResetCodeColor(const string& formatName, BOOL bNeedTrack = TRUE);
void    cpGetNewline(const string& formatName);

BOOL    cpIsNeedReplaceHtml(const string& formatName);
string  cpStartTag(const string& formatName);
string  cpEndTag(const string& formatName);


void    cpStringScanMode(BOOL bScanMode);
void    cpBeginString();
void    cpEndString();
void    cpGetString(const char* str, int len);
void    cpStringReplaceOverView();
void    cpEnableReplace(BOOL bEnable);