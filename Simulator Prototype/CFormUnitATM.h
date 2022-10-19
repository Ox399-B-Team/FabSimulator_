#pragma once
#include "afxdialogex.h"


// CFormUnitATM 대화 상자

class CFormUnitATM : public CDialogEx
{
	DECLARE_DYNAMIC(CFormUnitATM)

public:
	CFormUnitATM(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CFormUnitATM();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UNIT_ATM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
