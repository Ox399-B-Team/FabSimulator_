#pragma once
#include "afxdialogex.h"


// CFormLPM 대화 상자

class CFormLPM : public CDialogEx
{
	DECLARE_DYNAMIC(CFormLPM)

public:
	CFormLPM(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CFormLPM();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_LPM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	
	DECLARE_MESSAGE_MAP()
public:
	CSpinButtonCtrl m_spWaferMax;
	int m_nWaferMax;
	CString m_strObjName;

	virtual BOOL OnInitDialog();
};
