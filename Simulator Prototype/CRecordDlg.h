#pragma once
#include "afxdialogex.h"


// CRecordDlg 대화 상자

class CRecordDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRecordDlg)

public:
	CRecordDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	CRecordDlg(CString Created_date, CString Total_Runtime, CString Total_Output, CString Total_Input, CString Total_Throughput);
	virtual ~CRecordDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_RECORD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strCreatedDate;
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
	afx_msg void OnBnClickedOk();
	afx_msg void OnClose();
	CString m_strTotalOutput;
	CString m_strRuntime;
	CString m_strTotalInput;
	CListCtrl m_ctrlLayout;
	CString m_strThroughput;
	virtual void PostNcDestroy();
	afx_msg void OnBnClickedButtonSaveresult();
};
