#pragma once
#include "afxdialogex.h"


//Thread에서 updateData 호출 시 에러를 위해 작성
#define UPDATE_MSG  WM_USER + 1

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

	//Thread에서 updateData 호출 시 에러를 위해 작성
	afx_msg LRESULT OnReceivedMsgFromThread(WPARAM w, LPARAM l);

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
