// UIParserDlg.cpp : implementation file
//

#include "stdafx.h"
#include "UIParser.h"
#include "UIParserDlg.h"
#include ".\uiparserdlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CUIParserDlg dialog




CUIParserDlg::CUIParserDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CUIParserDlg::IDD, pParent)
    ,m_textSrc(NULL)
    ,m_textSrcLen(0)
    , m_tabSize(1)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CUIParserDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_RICHEDIT21, m_sourceView);
    DDX_Control(pDX, IDC_LOG, m_log);
    DDX_Control(pDX, IDC_EDIT1, m_resultView);
    DDX_Control(pDX, IDC_COMBO1, m_style);
}

BEGIN_MESSAGE_MAP(CUIParserDlg, CDialog)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    //}}AFX_MSG_MAP
    ON_EN_CHANGE(IDC_RICHEDIT21, &CUIParserDlg::OnEnChangeRichedit21)
    ON_BN_CLICKED(IDC_CONVERT, &CUIParserDlg::OnBnClickedConvert)
    ON_WM_SIZE()
    ON_BN_CLICKED(IDC_TAB2SPACE, OnBnClickedTab2space)
END_MESSAGE_MAP()


// CUIParserDlg message handlers

BOOL CUIParserDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);            // Set big icon
    SetIcon(m_hIcon, FALSE);        // Set small icon

    // TODO: Add extra initialization here
    m_sourceView.SetEventMask(m_sourceView.GetEventMask()|ENM_CHANGE);
    cpInitialColorMap();
    cpGetStyleList(m_styleList);
    cpUISetting uiSet;
    cpGetUISetting(uiSet);
    m_style.ResetContent();
    int sel = 0;
    for(size_t i=0;i<m_styleList.size();i++){
        m_style.AddString(CString(m_styleList[i].c_str()));
        if(uiSet.defaultStyle == m_styleList[i]){
            sel = i;
        }
    }
    if(m_styleList.size()){
        m_style.SetCurSel(sel);
    }
    ((CButton*)GetDlgItem(IDC_CLIPBOARD))->SetCheck(uiSet.bAutoCopy);
    ((CButton*)GetDlgItem(IDC_LINE_NUMBER))->SetCheck(uiSet.bNeedLineNumber);
    ((CButton*)GetDlgItem(IDC_TAB2SPACE))->SetCheck(uiSet.bTab2Space);
    ((CButton*)GetDlgItem(IDC_TABSIZE))->EnableWindow(uiSet.bTab2Space);
    m_tabSize = uiSet.tabSize;
    CString s;
    s.Format(_T("%d"),m_tabSize);
    ((CButton*)GetDlgItem(IDC_TABSIZE))->SetWindowText(s);
    SetWindowPos(NULL,uiSet.windowRect.left,uiSet.windowRect.top,
        uiSet.windowRect.right - uiSet.windowRect.left,
        uiSet.windowRect.bottom - uiSet.windowRect.top,SWP_NOZORDER);
    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CUIParserDlg::OnCancel()
{
    cpUISetting uiSet;
    UpdateData();
    uiSet.defaultStyle = m_styleList[m_style.GetCurSel()];
    uiSet.bAutoCopy = ((CButton*)GetDlgItem(IDC_CLIPBOARD))->GetCheck();
    uiSet.bNeedLineNumber = ((CButton*)GetDlgItem(IDC_LINE_NUMBER))->GetCheck();
    uiSet.bTab2Space = ((CButton*)GetDlgItem(IDC_TAB2SPACE))->GetCheck();
    CString s;
    ((CButton*)GetDlgItem(IDC_TABSIZE))->GetWindowText(s);
    uiSet.tabSize = m_tabSize = _ttoi(s);
    GetWindowRect(&uiSet.windowRect);
    cpSetUISetting(uiSet);
    __super::OnCancel();
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CUIParserDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialog::OnPaint();
    }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CUIParserDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CUIParserDlg::OnEnChangeRichedit21()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialog::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here
}

void CUIParserDlg::OnSize(UINT nType, int cx, int cy)
{
    if(!GetSafeHwnd())return;
    CWnd * p ;
    LONG yPos = 5;
    if(p=GetDlgItem(IDC_RICHEDIT21))p-> SetWindowPos(NULL,5,yPos,cx-10,(cy-50)/2,SWP_NOZORDER);
    yPos += ((cy-50)/2 + 5);
    if(p=GetDlgItem(IDC_EDIT1))p->      SetWindowPos(NULL,5,yPos,cx-10,(cy-50)/2,SWP_NOZORDER);
    yPos += ((cy-50)/2 + 15);
    if(p=GetDlgItem(IDC_CONVERT))p->    SetWindowPos(NULL,  5,yPos,80,20,SWP_NOZORDER);
    if(p=GetDlgItem(IDC_LINE_NUMBER))p->SetWindowPos(NULL, 90,yPos,80,20,SWP_NOZORDER);
    if(p=GetDlgItem(IDC_COMBO1))p->     SetWindowPos(NULL,180,yPos,80,20,SWP_NOZORDER);
    if(p=GetDlgItem(IDC_CLIPBOARD))p->  SetWindowPos(NULL,270,yPos,80,20,SWP_NOZORDER);
    if(p=GetDlgItem(IDC_TAB2SPACE))p->     SetWindowPos(NULL,360,yPos,120,20,SWP_NOZORDER);
    if(p=GetDlgItem(IDC_TABSIZE))p->  SetWindowPos(NULL,480,yPos,30,20,SWP_NOZORDER);


    if(p=GetDlgItem(IDC_LOG))p->        SetWindowPos(NULL,520,yPos,cx - 610,20,SWP_NOZORDER);
    if(p=GetDlgItem(IDCANCEL))p->       SetWindowPos(NULL,cx-85,yPos,80,20,SWP_NOZORDER);
    CDialog::OnSize(nType,cx,cy);
}


static int srcRemain = 0;   //< remain characters for the source text
static int srcLength = 0;   //< length of the source text
static char* lpSrc = NULL;  //< source buffer pointer
/* lex parser function */
int     yylex(void);
void    ParseStart(void);
CComboBox* pLog = NULL;     //< combo box pointer used for log data
string  result;             //< result generate by the parser
BOOL    bNeedLineNumber;    //< Flag used for output line number
char*   lnFormatComment;    //< line number format for block comment
char*   lnFormatNormal;     //< line number format for normal code
char*   currentFormat;      //< current format style, e.g. "ouravr"
int     curPos;             //< current source character position
BOOL    bNeedReplaceHtml;   //< replace html special character or not
BOOL    bTab2Space;         //< change tab to space
int     tabSize;            //< tab size
int     colPos;             //< column position, used for set tab
void CUIParserDlg::OnBnClickedConvert()
{
    // TODO: Add your control notification handler code here

    /** convert the Unicode string to mulit-byte for parser */
    CString str;
    m_sourceView.GetWindowText(str);
    if(m_textSrc)delete [] m_textSrc;
    m_textSrcLen = ::WideCharToMultiByte(CP_ACP,0,str,-1,NULL,0,NULL,NULL);
    m_textSrc = new char[m_textSrcLen+2];
    if(!m_textSrc){
        AfxMessageBox(_T("Fail to allocate buffer for the text!"));
        return;
    }
    ::WideCharToMultiByte(CP_ACP,0,str,-1,m_textSrc,(int)m_textSrcLen,NULL,NULL);
    if(m_textSrc[m_textSrcLen-2] != '\n'){
        m_textSrc[m_textSrcLen-1] = '\r';
        m_textSrc[m_textSrcLen] = '\n';
        m_textSrc[m_textSrcLen+1] = 0;
        m_textSrcLen+=2;
    }
    /* Prepare data for the lex parser */
    lpSrc = m_textSrc;                      //< Set source buffer
    srcLength = 
        srcRemain = (int)m_textSrcLen;      //< Set source iterator
    m_log.ResetContent();                   //< Reset log combo box
    pLog = &m_log;
    result = string("");                    //< Reset result string
    /* line number format for comment */
    lnFormatComment = "|*%04d*|  ";
    /* line number format for normal code */
    lnFormatNormal = "/*%04d*/  ";
    /* Need line number or not */
    if(((CButton*)GetDlgItem(IDC_LINE_NUMBER))->GetCheck()){
        bNeedLineNumber = TRUE;
    }else{
        bNeedLineNumber = FALSE;
    }
    /* replace tab to space or not */
    tabSize = 0;
    bTab2Space = FALSE;
    colPos = 0;
    if(((CButton*)GetDlgItem(IDC_TAB2SPACE))->GetCheck()){
        CString s;
        ((CButton*)GetDlgItem(IDC_TABSIZE))->GetWindowText(s);
        m_tabSize = _ttoi(s);
        if(m_tabSize>0 && m_tabSize<21){
            tabSize = m_tabSize;
            bTab2Space = TRUE;
        }else{
            AfxMessageBox(_T("Tab size error"));
        }
    }
    static char curStyle[1024] = "";
    if(m_style.GetCurSel() >=0 && m_style.GetCurSel() < m_styleList.size()){
        strcpy(curStyle,m_styleList[m_style.GetCurSel()].c_str());
    }else{
        AfxMessageBox(_T("Invalid selection!"));
        return;
    }
    bNeedReplaceHtml = cpIsNeedReplaceHtml(curStyle);
    currentFormat = curStyle;                 //< set color sytle

    BOOL    repeat = FALSE;
    if(strcmp(currentFormat,"replace") == 0){
        repeat = TRUE;
        cpEnableReplace(TRUE);
    }else{
        cpEnableReplace(FALSE);
    }
    do{
        cpStringScanMode(repeat);
        result += cpStartTag(curStyle);
        curPos = 0;                             //< Initial position
        cpResetColorStack();                    //< Reset color stack
        ParseStart();                           //< 
        yylex();
        result += cpEndTag(curStyle);
        if(repeat){
            repeat = FALSE;
            result = string("");
            lpSrc = m_textSrc;                      //< Set source buffer
            srcLength = 
                srcRemain = (int)m_textSrcLen;      //< Set source iterator
            continue;
        }else{
            cpStringReplaceOverView();
            break;
        }
    }while(1);


    m_resultView.SetWindowText(CString(result.c_str()));

    wstring wresult(toWString(result));

    /* Copy the result to clipboard */
    if(((CButton*)GetDlgItem(IDC_CLIPBOARD))->GetCheck()){
        if(OpenClipboard()){
            if(EmptyClipboard()){
                HGLOBAL hData = GlobalAlloc(GMEM_MOVEABLE, wresult.length()*2 + 2);
                memcpy(GlobalLock(hData), wresult.c_str(), wresult.length()*2 + 2);
                GlobalUnlock(hData);
                if (::SetClipboardData(CF_UNICODETEXT/*CF_TEXT*/, hData) == NULL){
                    AfxMessageBox(_T("Unable to set Clipboard data")); 
                } 

            }
            CloseClipboard();
        }
    }

    /* Reset source text's format */
    CHARFORMAT  cf = {sizeof(CHARFORMAT)};
    cf.dwMask = CFM_COLOR | CFM_SIZE | CFM_FACE;
    _tcscpy(cf.szFaceName,_T("Fixedsys"));
    cf.crTextColor = RGB(0,0,0);
    cf.yHeight = 180;
    m_sourceView.SetSel(0,m_sourceView.GetTextLength());
    m_sourceView.SetSelectionCharFormat(cf);

    /* Highlight source text */
    vector<cpCodeColor> colorStack;
    cpGetColorStack(colorStack);
    cf.dwMask = CFM_COLOR;
    for(size_t i=0;i<colorStack.size();i++){
        m_sourceView.SetSel(colorStack[i].start,colorStack[i].end);
        cf.crTextColor = colorStack[i].color;
        m_sourceView.SetSelectionCharFormat(cf);
    }
}

void CUIParserDlg::OnBnClickedTab2space()
{
    ((CButton*)GetDlgItem(IDC_TABSIZE))->EnableWindow(
        ((CButton*)GetDlgItem(IDC_TAB2SPACE))->GetCheck()
        );
}

/**
 Override the default YY_INPUT method, get the characters from the rich edit box
 */
int GetInput(char *buf, int maxlen)
{
    int c = '*', n;
    for ( n = 0; n < maxlen &&  srcRemain &&
        (c = lpSrc[srcLength-srcRemain--]) != 0 && c != '\n'; ++n ) {
        buf[n] = (char) c; 
    }
    if ( c == '\n' ){
        buf[n++] = (char) c;
    }
    return n;
}

/**
 Record the log to combo box
 */
void StringLog(const char* str)
{
    if(pLog){
        pLog->AddString(CString(str));
    }
}

void OutputString(const char* str, int len, BOOL bNeedTrack)
{
    cpGetString(str,len);
    char space[16] = "&nbsp;";
    char light[16] = "&lt;";
                  //12345678901234567890
    char tab[21] = "                    ";
    string htmlSpace;
    if(bNeedTrack){
        curPos += len;//(MultiByteToWideChar (CP_ACP, 0, str, -1, NULL, 0) - 1);
        if(*str == '\r'){
            curPos--;
        }
        colPos += len;
        //if(*str == '\n')colPos = 0;

        /* handle multi-byte character's leader byte*/
        static bool bNeedSub = true;
        if(*str<0){
            if(bNeedSub){
                curPos--;
                bNeedSub = false;
            }else{
                bNeedSub = true;
            }
        }else{
            if(!bNeedSub){
                bNeedSub = true;
            }
        }
        if(bNeedReplaceHtml){
            if(bNeedSub){
                switch (*str){
                    case ' ':
                        str = space;
                        break;
                    case '<':
                        str = light;
                        break;
                    default:
                        break;
                }
            }
        }
        if(bTab2Space && *str == '\t' && bNeedSub){
            int tabCnt = tabSize - (colPos-1)%tabSize;
            tab[tabSize - (colPos-1)%tabSize] = 0;
            colPos+= (tabCnt-1);
            str = tab;
            if(bNeedReplaceHtml){
                htmlSpace = "";
                for(int i = 0;i<tabCnt;i++){
                    htmlSpace += space;
                }
                str = htmlSpace.c_str();
            }
        }
    }
    result += str;
}

void    TrackPosition_noUse(const char* str, int len)
{
    curPos += len;//(MultiByteToWideChar (CP_ACP, 0, str, -1, NULL, 0) - 1);
    if(*str == '\r'){
        curPos--;
    }

    /* handle multi-byte character's leader byte*/
    static bool bNeedSub = true;
    if(*str<0){
        if(bNeedSub){
            curPos--;
            bNeedSub = false;
        }else{
            bNeedSub = true;
        }
    }else{
        if(!bNeedSub){
            bNeedSub = true;
        }
    }
}

