#pragma once
#include "afxdialogex.h"


// CFormInfoATM 대화 상자

class CFormInfoATM : public CDialogEx
{
	DECLARE_DYNAMIC(CFormInfoATM)

public:
	CFormInfoATM(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CFormInfoATM();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_TIMEINFO_ATM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CString m_strPickTime;
	CString m_strPlaceTime;
	CString m_strRotateTime;
	CString m_strZRotateTime;
};
