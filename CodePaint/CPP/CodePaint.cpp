#include "StdAfx.h"
#include "CodePaint.h"


cpColorMap_t            cpColorMap;
cpColorFormat_t         cpColorFormat;
vector<cpCodeColor>     cpPosStack;
vector<cpCodeColor>     cpColorStack;


struct {
    char        *name;
    COLORREF    color;
}colorMap[] = 
{
    {"comment",RGB(0,128,0)},
    {"string",RGB(200,0,0)},
    {"number",RGB(100,0,100)},
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

void    cpResetColorStack()
{
    cpPosStack.clear();
    cpColorStack.clear();
}

void    cpSetColor(const string& colorName, COLORREF color)
{
    cpColorMap[colorName] = color;
}

void    cpSetCodeColor(const string& formatName, const string& colorName, BOOL bNeedTrack)
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
    if(bNeedTrack){
        cpCodeColor cpColor;
        cpColor.color = it1->second;
        cpColor.start = curPos;
        cpPosStack.push_back(cpColor);
    }
}


void StringLog(const char* str);

void    cpResetCodeColor(const string& formatName, BOOL bNeedTrack)
{
    cpColorFormat_t::iterator it = cpColorFormat.find(formatName);
    if(it == cpColorFormat.end()){
        return;
    }
    char buf[1024] = "";
    sprintf(buf,it->second.reset.c_str());
    OutputString(buf,0);
    if(bNeedTrack && cpPosStack.size()){
        vector<cpCodeColor>::iterator it1 = cpPosStack.begin();
        it1 += (cpPosStack.size() - 1);
        if(it1 != cpPosStack.end()){
            it1->end = curPos;
            {
                char buf[1024] = "";
                sprintf(buf,"Color: 0x%06X, Start: %d, End: %d",
                    it1->color,it1->start, it1->end);
                StringLog(buf);
            }
            cpColorStack.push_back(*it1);
            cpPosStack.erase(it1);
        }
    }
}

void    cpGetColorStack(vector<cpCodeColor>& colorStack)
{
    colorStack.clear();
    for(size_t i=0;i<cpColorStack.size();i++){
        colorStack.push_back(cpColorStack[cpColorStack.size() - i - 1]);
    }
}
