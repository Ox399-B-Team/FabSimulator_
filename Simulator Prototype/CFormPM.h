#pragma once
#include "afxdialogex.h"


// CFormPM 대화 상자

class CFormPM : public CDialogEx
{
	DECLARE_DYNAMIC(CFormPM)

public:
	CFormPM(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CFormPM();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_PM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CSpinButtonCtrl m_spWaferMax;
	CSpinButtonCtrl m_spCleanTime;
	CSpinButtonCtrl m_spCleanCount;
	CSpinButtonCtrl m_spSlotOpen;
	CSpinButtonCtrl m_spSlotClose;
	CSpinButtonCtrl m_spProcessTime;
	int m_nWaferMax;
	int m_nCleanTime;
	int m_nSlotOpen;
	int m_nSlotClose;
	int m_nProcessTime;
	int m_nCleanCount;

	virtual BOOL OnInitDialog();
	CString m_strObjName;
};
