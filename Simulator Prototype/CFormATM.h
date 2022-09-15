#pragma once
#include "afxdialogex.h"


// CFormATM 대화 상자

class CFormATM : public CDialogEx
{
	DECLARE_DYNAMIC(CFormATM)

public:
	CFormATM(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CFormATM();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_ATM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CSpinButtonCtrl m_spPickTime;
	CSpinButtonCtrl m_spPlaceTime;
	CSpinButtonCtrl m_spRotateTime;
	CSpinButtonCtrl m_spZRotateTime;
	CComboBox m_ctrlArm;
	int m_nPickTime;
	int m_nPlaceTime;
	int m_nRotateTime;
	int m_nZRotateTime;
	CString m_strObjName;
};
