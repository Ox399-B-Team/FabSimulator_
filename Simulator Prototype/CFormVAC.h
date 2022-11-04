#pragma once
#include "afxdialogex.h"


// CFormVAC 대화 상자

class CFormVAC : public CDialogEx
{
	DECLARE_DYNAMIC(CFormVAC)

public:
	CFormVAC(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	CFormVAC(CString strModuleName, CWnd* pParent = nullptr);
	virtual ~CFormVAC();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_VAC };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_ctrlArm;
	CSpinButtonCtrl m_spPickTime;
	CSpinButtonCtrl m_spPlaceTime;
	CSpinButtonCtrl m_spRotateTime;
	int m_nPickTime;
	int m_nPlaceTime;
	int m_nRotateTime;
	virtual BOOL OnInitDialog();
	CString m_strObjName;
};
