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
    {"comment",     RGB(0,128,0)},
    {"string",      RGB(200,0,0)},
    {"number",      RGB(100,0,100)},
    {"keyword",     RGB(0,0,255)},
    {"linenumber",  RGB(100,100,0)},
};

struct {
    char        *name;
    char        *set;
    char        *reset;
    char        *startTag;
    char        *endTag;
    BOOL        bNeedReplaceHtml;
}colorFormat[] = 
{
    {"ouravr",  "<font color=#%06X>",   "</font>"   , "", "", FALSE},
    {"21ic",    "[color=#%06X]",        "[/color]"  , "", "", FALSE},
    {"pic16",   "<FONT color=#%06X>",   "</FONT>"   , "<DIV class=quote>", "</DIV>", TRUE},
};


/** 
  Initial color map with default value
 */
void    cpInitialColorMap()
{
    for(int i=0;i<sizeof(colorMap)/sizeof(colorMap[0]);i++){
        cpColorMap[colorMap[i].name] = colorMap[i].color;
    }
    for(int i=0;i<sizeof(colorFormat)/sizeof(colorFormat[0]);i++){
        cpColorSF sf;
        sf.set = colorFormat[i].set;
        sf.reset = colorFormat[i].reset;
        sf.bNeedReplaceHtml = colorFormat[i].bNeedReplaceHtml;
        if(colorFormat[i].startTag){
            sf.startTag = colorFormat[i].startTag;
        }
        if(colorFormat[i].endTag){
            sf.endTag = colorFormat[i].endTag;
        }
        cpColorFormat[colorFormat[i].name] = sf;
    }
}

/**
  Replace or add new color
 */
void    cpSetColor(const string& colorName, COLORREF color)
{
    cpColorMap[colorName] = color;
}

/**
  Reset the color stack
 */
void    cpResetColorStack()
{
    cpPosStack.clear();
    cpColorStack.clear();
}

/**
  set code color
 */
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
    COLORREF ref = RGB(
        GetBValue(it1->second),
        GetGValue(it1->second),
        GetRValue(it1->second)
        );
    char buf[1024] = "";
    sprintf(buf,it->second.set.c_str(),ref);
    OutputString(buf,0);
    if(bNeedTrack){
        cpCodeColor cpColor;
        cpColor.color = it1->second;
        cpColor.start = curPos;
        cpPosStack.push_back(cpColor);
    }
}


void StringLog(const char* str);
/**
  reset code color
 */
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
            if(0){
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

/**
  get color stack
 */
void    cpGetColorStack(vector<cpCodeColor>& colorStack)
{
    colorStack.clear();
    for(size_t i=0;i<cpColorStack.size();i++){
        colorStack.push_back(cpColorStack[cpColorStack.size() - i - 1]);
    }
}

void    cpGetStyleList(vector<string>& styleList)
{
    styleList.clear();
    for(cpColorFormat_t::iterator it = cpColorFormat.begin();
        it != cpColorFormat.end(); it++){
        styleList.push_back(it->first);
    }
}

void    cpGetNewline(const string& formatName)
{
    if(formatName == "pic16"){
        OutputString("<BR>",0);
    }
}

BOOL    cpIsNeedReplaceHtml(const string& formatName)
{
    return cpColorFormat[formatName].bNeedReplaceHtml;
}

string  cpStartTag(const string& formatName)
{
    return string(cpColorFormat[formatName].startTag);
}

string  cpEndTag(const string& formatName)
{
    return string(cpColorFormat[formatName].endTag);
}
