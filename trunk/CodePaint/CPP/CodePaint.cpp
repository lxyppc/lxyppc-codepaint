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

static void    cpLoadSettingFromIni();
/** 
  Initial color map with default value
 */
void    cpInitialColorMap()
{
    cpLoadSettingFromIni();
    //for(int i=0;i<sizeof(colorMap)/sizeof(colorMap[0]);i++){
    //    cpColorMap[colorMap[i].name] = colorMap[i].color;
    //}
    //for(int i=0;i<sizeof(colorFormat)/sizeof(colorFormat[0]);i++){
    //    cpColorSF sf;
    //    sf.set = colorFormat[i].set;
    //    sf.reset = colorFormat[i].reset;
    //    sf.bNeedReplaceHtml = colorFormat[i].bNeedReplaceHtml;
    //    if(colorFormat[i].startTag){
    //        sf.startTag = colorFormat[i].startTag;
    //    }
    //    if(colorFormat[i].endTag){
    //        sf.endTag = colorFormat[i].endTag;
    //    }
    //    cpColorFormat[colorFormat[i].name] = sf;
    //}
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

char    IniName[1024] = "";
#define ArrSize(x)      (sizeof(x)/sizeof(x[0]))

static void    cpWriteDefaultStyle()
{
    GetModuleFileNameA(NULL,IniName,1024);
    size_t len = strlen(IniName);
    IniName[len-1] = _T('i');
    IniName[len-2] = _T('n');
    IniName[len-3] = _T('i');
        BOOL res;
        char tmpStr[256] = "";
        itoa(ArrSize(colorFormat),tmpStr,10);
        res = WritePrivateProfileStringA(
            "StyleSettings",
            "Count",
            tmpStr,IniName
            );
        for(int i=0;i<ArrSize(colorFormat);i++){
            char sName[256] = "Setting";
            itoa(i,sName+7,10);
            res = WritePrivateProfileStringA(
                "StyleSettings",
                sName,
                colorFormat[i].name,IniName
                );
            res = WritePrivateProfileStringA(
                colorFormat[i].name,
                "ColorSet",
                colorFormat[i].set,IniName
                );
            res = WritePrivateProfileStringA(
                colorFormat[i].name,
                "ColorReset",
                colorFormat[i].reset,IniName
                );
            res = WritePrivateProfileStringA(
                colorFormat[i].name,
                "StartTag",
                colorFormat[i].startTag,IniName
                );
            res = WritePrivateProfileStringA(
                colorFormat[i].name,
                "EndTag",
                colorFormat[i].endTag,IniName
                );
            res = WritePrivateProfileStringA(
                colorFormat[i].name,
                "ReplaceHtmlTag",
                colorFormat[i].bNeedReplaceHtml ? "1" : "0",IniName
                );
        }
}

static void    cpWriteDefaultColor()
{
    GetModuleFileNameA(NULL,IniName,1024);
    size_t len = strlen(IniName);
    IniName[len-1] = _T('i');
    IniName[len-2] = _T('n');
    IniName[len-3] = _T('i');
        BOOL res;
        char tmpStr[256] = "";
        itoa(ArrSize(colorMap),tmpStr,10);
        res = WritePrivateProfileStringA(
            "ColorSettings",
            "Count",
            tmpStr,IniName
            );
        for(int i=0;i<ArrSize(colorMap);i++){
            char sName[256] = "ColorNo";
            itoa(i,sName+7,10);
            res = WritePrivateProfileStringA(
                "ColorSettings",
                sName,
                colorMap[i].name,IniName
                );
            itoa(GetRValue(colorMap[i].color),sName,10);
            res = WritePrivateProfileStringA(
                colorMap[i].name,
                "R",
                sName,IniName
                );
            itoa(GetGValue(colorMap[i].color),sName,10);
            res = WritePrivateProfileStringA(
                colorMap[i].name,
                "G",
                sName,IniName
                );
            itoa(GetBValue(colorMap[i].color),sName,10);
            res = WritePrivateProfileStringA(
                colorMap[i].name,
                "B",
                sName,IniName
                );
        }
}

static void    cpLoadSettingFromIni()
{
    GetModuleFileNameA(NULL,IniName,1024);
    size_t len = strlen(IniName);
    IniName[len-1] = _T('i');
    IniName[len-2] = _T('n');
    IniName[len-3] = _T('i');
    FILE* fp = fopen(IniName,"r");
    if(fp){
        fclose(fp);
    }else{
        /// Write default value to ini file
        cpWriteDefaultStyle();
        cpWriteDefaultColor();
    }
    // Load settings from the ini file
    BOOL res;
    char tmpStr[256] = "";
    // Load style settings
    cpColorFormat.clear();
    res = GetPrivateProfileStringA(
            "StyleSettings",
            "Count",
            "0",tmpStr,256,IniName
            );
    int tmpCnt = atoi(tmpStr);
    if(!tmpCnt){
        cpWriteDefaultStyle();
        res = GetPrivateProfileStringA(
            "StyleSettings",
            "Count",
            "0",tmpStr,256,IniName
            );
        tmpCnt = atoi(tmpStr);
    }
    for(int i=0;i<tmpCnt;i++){
        char sName[256] = "Setting";
        itoa(i,sName+7,10);
        res = GetPrivateProfileStringA(
            "StyleSettings",
            sName,
            "",tmpStr,256,IniName);
        if(string(tmpStr) != ""){
            cpColorSF sf;

            res = GetPrivateProfileStringA(
            tmpStr,
            "ColorSet",
            "",sName,256,IniName);
            sf.set = sName;

            res = GetPrivateProfileStringA(
            tmpStr,
            "ColorReset",
            "",sName,256,IniName);
            sf.reset = sName;

            res = GetPrivateProfileStringA(
            tmpStr,
            "StartTag",
            "",sName,256,IniName);
            sf.startTag = sName;

            res = GetPrivateProfileStringA(
            tmpStr,
            "EndTag",
            "",sName,256,IniName);
            sf.endTag = sName;

            res = GetPrivateProfileStringA(
            tmpStr,
            "ReplaceHtmlTag",
            "",sName,256,IniName);
            sf.bNeedReplaceHtml = atoi(sName) ? TRUE : FALSE;
            cpColorFormat[tmpStr] = sf;
        }
    }

    // Load color settings
    cpColorMap.clear();
    res = GetPrivateProfileStringA(
            "ColorSettings",
            "Count",
            "0",tmpStr,256,IniName
            );
    tmpCnt = atoi(tmpStr);
    if(!tmpCnt){
        cpWriteDefaultColor();
        res = GetPrivateProfileStringA(
            "ColorSettings",
            "Count",
            "0",tmpStr,256,IniName
            );
       tmpCnt = atoi(tmpStr);
    }
    for(int i=0;i<tmpCnt;i++){
        char sName[256] = "ColorNo";
        itoa(i,sName+7,10);
        res = GetPrivateProfileStringA(
            "ColorSettings",
            sName,
            "",tmpStr,256,IniName);
        if(string(tmpStr) != ""){
            int r,g,b;
            res = GetPrivateProfileStringA(
                tmpStr,
                "R",
                "0",sName,256,IniName);
            r = atoi(sName);
            res = GetPrivateProfileStringA(
                tmpStr,
                "G",
                "0",sName,256,IniName);
            g = atoi(sName);
            res = GetPrivateProfileStringA(
                tmpStr,
                "B",
                "0",sName,256,IniName);
            b = atoi(sName);
            cpColorMap[tmpStr] = RGB(r,g,b);
        }
    }
            //for(int i=0;i<sizeof(colorMap)/sizeof(colorMap[0]);i++){
        //    cpColorMap[colorMap[i].name] = colorMap[i].color;
        //}
}

/**
  Get UI settings from the ini file
 */
void    cpGetUISetting(cpUISetting& uiSetting)
{
    GetModuleFileNameA(NULL,IniName,1024);
    size_t len = strlen(IniName);
    IniName[len-1] = _T('i');
    IniName[len-2] = _T('n');
    IniName[len-3] = _T('i');
    char tmpStr[256] = "";
    GetPrivateProfileStringA(
              "UISettings",
              "NeedLineNumber",
              "0",tmpStr,256,IniName);
    uiSetting.bNeedLineNumber = atoi(tmpStr) ? TRUE:FALSE;
    GetPrivateProfileStringA(
              "UISettings",
              "AutoCopy",
              "1",tmpStr,256,IniName);
    uiSetting.bAutoCopy = atoi(tmpStr) ? TRUE:FALSE;
    GetPrivateProfileStringA(
              "UISettings",
              "DefaultStyle",
              "pic16",tmpStr,256,IniName);
    uiSetting.defaultStyle  = tmpStr;
}

/**
  Save UI settings tp the ini file
 */
void    cpSetUISetting(const cpUISetting& uiSetting)
{
    GetModuleFileNameA(NULL,IniName,1024);
    size_t len = strlen(IniName);
    IniName[len-1] = _T('i');
    IniName[len-2] = _T('n');
    IniName[len-3] = _T('i');
    BOOL
    res = WritePrivateProfileStringA(
            "UISettings",
            "NeedLineNumber",
            uiSetting.bNeedLineNumber ? "1":"0",IniName
            );
    res = WritePrivateProfileStringA(
            "UISettings",
            "AutoCopy",
            uiSetting.bAutoCopy ? "1":"0",IniName
            );
    res = WritePrivateProfileStringA(
            "UISettings",
            "DefaultStyle",
            uiSetting.defaultStyle.c_str(),IniName
            );
}
