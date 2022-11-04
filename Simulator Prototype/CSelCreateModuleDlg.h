#pragma once
#include "afxdialogex.h"
#include "CFormLPM.h"
#include "CFormATM.h"
#include "CFormLL.h"
#include "CFormVAC.h"
#include "CFormPM.h"


// CSelCreateModuleDlg 대화 상자

class CSelCreateModuleDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSelCreateModuleDlg)

public:
	CTabCtrl m_ctrlTab;
	CFormLPM* m_pDlgLPM;
	CFormATM* m_pDlgATM;
	CFormLL* m_pDlgLL;
	CFormVAC* m_pDlgVAC;
	CFormPM* m_pDlgPM;
	CPoint m_ptPos;

	// 리스트컨트롤이 가지고 있음.. 고민중..
	int m_nRow;
	int m_nCol;
	
public:
	CSelCreateModuleDlg(CWnd* pParent, int nRow, int nCol);   // 표준 생성자입니다.
	virtual ~CSelCreateModuleDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SEL_CREATEMODULE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnTcnSelchangeTabModule(NMHDR* pNMHDR, LRESULT* pResult);
};
