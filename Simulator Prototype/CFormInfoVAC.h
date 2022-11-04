#pragma once
#include "afxdialogex.h"


// CFormInfoVAC 대화 상자

class CFormInfoVAC : public CDialogEx
{
	DECLARE_DYNAMIC(CFormInfoVAC)

public:
	CFormInfoVAC(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CFormInfoVAC();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_TIMEINFO_VAC };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CString m_strPickTime;
	CString m_strPlaceTime;
	CString m_strRotateTime;
};
