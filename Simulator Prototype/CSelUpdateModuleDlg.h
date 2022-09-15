#pragma once
#include "afxdialogex.h"
#include "CFormATM.h"
#include "CFormLPM.h"
#include "CFormLL.h"
#include "CFormPM.h"
#include "CFormVAC.h"


// CSelUpdateModuleDlg 대화 상자

class CSelUpdateModuleDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSelUpdateModuleDlg)

public:
	CPoint m_ptPos;
	int m_nRow;
	int m_nCol;
	int m_nModuleIdx;
	int m_nTabIdx;

public:
	CSelUpdateModuleDlg(CWnd* pParent, int nRow, int nCol);
	virtual ~CSelUpdateModuleDlg();
	
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SEL_UPDATEMODULE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_ctrlUpdateTab;
	CFormLPM* m_pDlgLPM;
	CFormATM* m_pDlgATM;
	CFormLL* m_pDlgLL;
	CFormVAC* m_pDlgVAC;
	CFormPM* m_pDlgPM;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnTcnSelchangeTabUpdatemodule(NMHDR* pNMHDR, LRESULT* pResult);
};
