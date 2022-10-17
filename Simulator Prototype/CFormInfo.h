#pragma once
#include "afxdialogex.h"


// CFormInfo 대화 상자

class CFormInfo : public CDialogEx
{
	DECLARE_DYNAMIC(CFormInfo)

public:
	CFormInfo(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CFormInfo();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_INFO };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void InitializeFormInfo(bool bFlag = true);
	CString m_strModuleType;
	CString m_strModuleName;
	CString m_strWaferMax;
	double m_dModuleThruput;
	int m_nModuleInputCnt;
	int m_nModuleOutputCnt;
	double m_dFabThroughput;
	int m_nFabInputCnt;
	int m_nFabOutputCnt;
};
