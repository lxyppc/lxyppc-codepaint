#pragma once


#include <map>
#include <string>
using   std::map;
using   std::string;

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

typedef     map<string,COLORREF>    cpColorMap_t;
typedef     map<string,cpColorSF>   cpColorFormat_t;

extern  void OutputString(const char* str, int len);

void    cpInitialColorMap();
void    cpSetColor(const string& colorName, COLORREF color);
void    cpSetCodeColor(const string& formatName, const string& colorName);
void    cpResetCodeColor(const string& formatName);
