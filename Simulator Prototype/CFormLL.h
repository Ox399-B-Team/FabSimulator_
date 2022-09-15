#pragma once
#include "afxdialogex.h"


// CFormLL 대화 상자

class CFormLL : public CDialogEx
{
	DECLARE_DYNAMIC(CFormLL)

public:
	CFormLL(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CFormLL();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_LL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CSpinButtonCtrl m_spWaferMax;
	CSpinButtonCtrl m_spPumpTime;
	CSpinButtonCtrl m_spVentTime;
	CSpinButtonCtrl m_spPumpStable;
	CSpinButtonCtrl m_spVentStable;
	CSpinButtonCtrl m_spSlotOpen;
	CSpinButtonCtrl m_spDoorOpen;
	CSpinButtonCtrl m_spSlotClose;
	CSpinButtonCtrl m_spDoorClose;
	int m_nWaferMax;
	int m_nPumpTime;
	int m_nVentTime;
	int m_nPumpStable;
	int m_nVentStable;
	int m_nSlotOpen;
	int m_nDoorOpen;
	int m_nSlotClose;
	int m_nDoorClose;
	CString m_strObjName;
};
