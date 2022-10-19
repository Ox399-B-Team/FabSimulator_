#pragma once
#include "afxdialogex.h"


// CFormInfoLL 대화 상자

class CFormInfoLL : public CDialogEx
{
	DECLARE_DYNAMIC(CFormInfoLL)

public:
	CFormInfoLL(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CFormInfoLL();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_TIMEINFO_LL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strPumpTime;
	CString m_strVentTime;
	CString m_strSlotOpenTime;
	CString m_strDoorOpenTime;
	CString m_strPumpStableTime;
	CString m_strVentStableTime;
	CString m_strSlotCloseTime;
	CString m_strDoorCloseTime;
	virtual BOOL OnInitDialog();
};
