// UIParserDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "CodePaint.h"
// CUIParserDlg dialog
class CUIParserDlg : public CDialog
{
// Construction
public:
    CUIParserDlg(CWnd* pParent = NULL);    // standard constructor

// Dialog Data
    enum { IDD = IDD_UIPARSER_DIALOG };

    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support


// Implementation
protected:
    HICON m_hIcon;

    // Generated message map functions
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
public:
    CRichEditCtrl m_sourceView;
public:
    afx_msg void OnEnChangeRichedit21();
    virtual void OnOK(){}
    virtual void OnCancel();
public:
    CComboBox m_log;
public:
    afx_msg void OnBnClickedConvert();
public:
    char*       m_textSrc;
    size_t      m_textSrcLen;
public:
    CEdit       m_resultView;
public:
    CComboBox m_style;
public:
    vector<string>  m_styleList;
public:
    afx_msg void OnSize(UINT nType, int cx, int cy);
    int m_tabSize;
    afx_msg void OnBnClickedTab2space();
};
