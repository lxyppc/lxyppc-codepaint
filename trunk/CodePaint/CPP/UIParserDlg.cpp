// UIParserDlg.cpp : implementation file
//

#include "stdafx.h"
#include "UIParser.h"
#include "UIParserDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CUIParserDlg dialog




CUIParserDlg::CUIParserDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUIParserDlg::IDD, pParent)
    ,m_textSrc(NULL)
    ,m_textSrcLen(0)
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
END_MESSAGE_MAP()


// CUIParserDlg message handlers

BOOL CUIParserDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
    m_sourceView.SetEventMask(m_sourceView.GetEventMask()|ENM_CHANGE);
    cpInitialColorMap();
    cpGetStyleList(m_styleList);
    m_style.ResetContent();
    for(size_t i=0;i<m_styleList.size();i++){
        m_style.AddString(CString(m_styleList[i].c_str()));
    }
    if(m_styleList.size()){
        m_style.SetCurSel(0);
    }

	return TRUE;  // return TRUE  unless you set the focus to a control
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
    if(p=GetDlgItem(IDC_LOG))p->        SetWindowPos(NULL,270,yPos,cx - 360,20,SWP_NOZORDER);
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
    static char curStyle[1024] = "";
    if(m_style.GetCurSel() >=0 && m_style.GetCurSel() < m_styleList.size()){
        strcpy(curStyle,m_styleList[m_style.GetCurSel()].c_str());
    }else{
        AfxMessageBox(_T("Invalid selection!"));
        return;
    }
    currentFormat = curStyle;                 //< set color sytle
    curPos = 0;                             //< Initial position
    cpResetColorStack();                    //< Reset color stack
    ParseStart();                           //< 
    yylex();
    m_resultView.SetWindowText(CString(result.c_str()));

    /* Copy the result to clipboard */
    if(OpenClipboard()){
        if(EmptyClipboard()){
            HGLOBAL hData = GlobalAlloc(GMEM_MOVEABLE, result.length() + 1);
            memcpy(GlobalLock(hData), result.c_str(), result.length() + 1);
            GlobalUnlock(hData);
            if (::SetClipboardData(CF_TEXT, hData) == NULL){
                AfxMessageBox(_T("Unable to set Clipboard data")); 
            } 

        }
        CloseClipboard();
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

void OutputString(const char* str, int len)
{
    result += string(str);
}

void    TrackPosition(const char* str, int len)
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
