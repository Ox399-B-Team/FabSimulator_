#pragma once
#include "afxdialogex.h"


// CFormInfoPM 대화 상자

class CFormInfoPM : public CDialogEx
{
	DECLARE_DYNAMIC(CFormInfoPM)

public:
	CFormInfoPM(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CFormInfoPM();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_TIMEINFO_PM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strProcessTime;
	CString m_strProcessCount;
	CString m_strCleanTime;
	CString m_strCleanCount;
	CString m_strSlotOpenTime;
	CString m_strSlotCloseTime;
	virtual BOOL OnInitDialog();
};
