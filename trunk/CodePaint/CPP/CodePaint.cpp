#include "StdAfx.h"
#include "CodePaint.h"


cpColorMap_t        cpColorMap;
cpColorFormat_t     cpColorFormat;

struct {
    char        *name;
    COLORREF    color;
}colorMap[] = 
{
    {"comment",RGB(0,200,0)},
    {"string",RGB(200,0,0)},
    {"number",RGB(0,0,0)},
    {"keyword",RGB(0,0,255)},
    {"linenumber",RGB(100,100,0)},
};

struct {
    char        *name;
    char        *set;
    char        *reset;
}colorFormat[] = 
{
    {"ouravr","<font color=#%06X>","</font>"},
};



void    cpInitialColorMap()
{
    for(int i=0;i<sizeof(colorMap)/sizeof(colorMap[0]);i++){
        cpColorMap[colorMap[i].name] = colorMap[i].color;
    }
    for(int i=0;i<sizeof(colorFormat)/sizeof(colorFormat[0]);i++){
        cpColorSF sf;
        sf.set = colorFormat[i].set;
        sf.reset = colorFormat[i].reset;
        cpColorFormat[colorFormat[i].name] = sf;
    }
}

void    cpSetColor(const string& colorName, COLORREF color)
{
    cpColorMap[colorName] = color;
}

void    cpSetCodeColor(const string& formatName, const string& colorName)
{
    cpColorFormat_t::iterator it = cpColorFormat.find(formatName);
    if(it == cpColorFormat.end()){
        return;
    }
    cpColorMap_t::iterator it1 = cpColorMap.find(colorName);
    if(it1 == cpColorMap.end()){
        return;
    }
    union {
        COLORREF ref;
        struct {
            BYTE b;
            BYTE g;
            BYTE r;
        };
    }myColor;
    myColor.ref = (unsigned int)it1->second;
    BYTE tmp = myColor.r;
    myColor.r = myColor.b;
    myColor.b = tmp;
    char buf[1024] = "";
    sprintf(buf,it->second.set.c_str(),myColor.ref);
    OutputString(buf,0);
}

void    cpResetCodeColor(const string& formatName)
{
    cpColorFormat_t::iterator it = cpColorFormat.find(formatName);
    if(it == cpColorFormat.end()){
        return;
    }
    char buf[1024] = "";
    sprintf(buf,it->second.reset.c_str());
    OutputString(buf,0);
}