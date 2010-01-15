// UIParserDlg.cpp : implementation file
//

#include "stdafx.h"
#include "UIParser.h"
#include "UIParserDlg.h"
#include "CodePaint.h"

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
    DDX_Control(pDX, IDC_RICHEDIT22, m_resultView);
}

BEGIN_MESSAGE_MAP(CUIParserDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_EN_CHANGE(IDC_RICHEDIT21, &CUIParserDlg::OnEnChangeRichedit21)
    ON_BN_CLICKED(IDC_CONVERT, &CUIParserDlg::OnBnClickedConvert)
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

BOOL    parseStart = FALSE;

void CUIParserDlg::OnEnChangeRichedit21()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialog::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here
}

static int remain,max;
static char* tmpBuf = NULL;
int yylex(void);
void    ParseStart(void);
CComboBox* pLog = NULL;
string  result;
BOOL    bNeedLineNumber;
char*   lnFormatComment;
char*   lnFormatNormal;
char*   currentFormat;
int     curPos;
void CUIParserDlg::OnBnClickedConvert()
{
    // TODO: Add your control notification handler code here
    parseStart = TRUE;
    CString str;
    m_sourceView.GetWindowText(str);
    if(m_textSrc)delete [] m_textSrc;
    m_textSrcLen = ::WideCharToMultiByte(CP_ACP,0,str,-1,NULL,0,NULL,NULL);
	m_textSrc = new char[m_textSrcLen];
    if(!m_textSrc){
        AfxMessageBox(_T("Fail to allocate buffer for the text!"));
        return;
    }
    ::WideCharToMultiByte(CP_ACP,0,str,-1,m_textSrc,(int)m_textSrcLen,NULL,NULL);
    tmpBuf = m_textSrc;
    max = remain = (int)m_textSrcLen;
    m_log.ResetContent();
    pLog = &m_log;
    result = string("");
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
    currentFormat = "ouravr";
    curPos = 0;
    cpResetColorStack();
    ParseStart();
    yylex();
    m_resultView.SetWindowText(CString(result.c_str()));

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
    CHARFORMAT  cf = {sizeof(CHARFORMAT)};
    cf.dwMask = CFM_COLOR | CFM_SIZE | CFM_FACE;
    _tcscpy(cf.szFaceName,_T("Fixedsys"));
    cf.crTextColor = RGB(0,0,0);
    cf.yHeight = 180;
    m_sourceView.SetSel(0,m_sourceView.GetTextLength()-1);
    m_sourceView.SetSelectionCharFormat(cf);

    vector<cpCodeColor> colorStack;
    cpGetColorStack(colorStack);
    cf.dwMask = CFM_COLOR;
    for(size_t i=0;i<colorStack.size();i++){
        m_sourceView.SetSel(colorStack[i].start,colorStack[i].end);
        cf.crTextColor = colorStack[i].color;
        m_sourceView.SetSelectionCharFormat(cf);
    }
    parseStart = FALSE;
    //cf.crTextColor = RGB(0,0,0);
    //m_sourceView.SetSel(0,m_sourceView.GetTextLength()-1);
    //m_sourceView.SetSelectionCharFormat(cf);
}


/**
 Override the default YY_INPUT method, get the characters from the rich edit box
 */
int GetInput(char *buf, int maxlen)
{
    int c = '*', n;
    for ( n = 0; n < maxlen &&  remain &&
        (c = tmpBuf[max-remain--]) != 0 && c != '\n'; ++n ) {
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
