#pragma once


#include <map>
#include <string>
#include <vector>
using   std::map;
using   std::string;
using   std::vector;


struct cpColorSF{
    string set;
    string reset;
    cpColorSF& operator=(const cpColorSF& rhs){
        set = rhs.set;
        reset = rhs.reset;
        return *this;
    }
    cpColorSF(){}
    cpColorSF(const cpColorSF& rhs){
        set = rhs.set;
        reset = rhs.reset;
    }
};

struct  cpCodeColor{
    COLORREF    color;
    int         start;
    int         end;
};

typedef     map<string,COLORREF>    cpColorMap_t;
typedef     map<string,cpColorSF>   cpColorFormat_t;

extern  void    OutputString(const char* str, int len);
extern  int     curPos;

void    cpInitialColorMap();
void    cpResetColorStack();
void    cpGetColorStack(vector<cpCodeColor>& colorStack);
void    cpGetStyleList(vector<string>& styleList);
void    cpSetColor(const string& colorName, COLORREF color);

void    cpSetCodeColor(const string& formatName, const string& colorName, BOOL bNeedTrack = TRUE);
void    cpResetCodeColor(const string& formatName, BOOL bNeedTrack = TRUE);
void    cpGetNewline(const string& formatName);
