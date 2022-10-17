// CSelCreateModuleDlg.cpp: 구현 파일
//

#pragma once
#include "pch.h"
#include "Simulator Prototype.h"
#include "afxdialogex.h"
#include "CSelCreateModuleDlg.h"


// CSelCreateModuleDlg 대화 상자

IMPLEMENT_DYNAMIC(CSelCreateModuleDlg, CDialogEx)

CSelCreateModuleDlg::CSelCreateModuleDlg(CWnd* pParent, int nRow, int nCol)
	: CDialogEx(IDD_SEL_CREATEMODULE, pParent)
{
	m_pDlgLPM = NULL;
	m_pDlgATM = NULL;
	m_pDlgLL = NULL;
	m_pDlgVAC = NULL;
	m_pDlgPM = NULL;
	m_nRow = nRow;
	m_nCol = nCol;
}

CSelCreateModuleDlg::~CSelCreateModuleDlg()
{
	delete m_pDlgLPM;
	delete m_pDlgATM;
	delete m_pDlgLL;
	delete m_pDlgVAC;
	delete m_pDlgPM;
}

void CSelCreateModuleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_MODULE, m_ctrlTab);
}

BEGIN_MESSAGE_MAP(CSelCreateModuleDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CSelCreateModuleDlg::OnBnClickedOk)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_MODULE, &CSelCreateModuleDlg::OnTcnSelchangeTabModule)
END_MESSAGE_MAP()


// CSelCreateModuleDlg 메시지 처리기
#include "CFabController.h"

BOOL CSelCreateModuleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CRect rect;
	m_ctrlTab.GetWindowRect(rect);

	switch (m_nCol)
	{
	case 2:
		m_ctrlTab.InsertItem(0, _T("LPM"));
		m_pDlgLPM = new CFormLPM;
		m_pDlgLPM->Create(IDD_DIALOG_LPM, &m_ctrlTab);
		m_pDlgLPM->MoveWindow(20, 20, rect.Width(), rect.Height());
		m_pDlgLPM->ShowWindow(SW_SHOW);
		break;
		
	case 3:
		m_ctrlTab.InsertItem(0, _T("ATMRobot"));
		m_pDlgATM = new CFormATM;
		m_pDlgATM->Create(IDD_DIALOG_ATM, &m_ctrlTab);
		m_pDlgATM->MoveWindow(20, 20, rect.Width(), rect.Height());
		m_pDlgATM->ShowWindow(SW_SHOW);
		break;

	case 4:
		m_ctrlTab.InsertItem(0, _T("LL"));
		m_pDlgLL = new CFormLL;
		m_pDlgLL->Create(IDD_DIALOG_LL, &m_ctrlTab);
		m_pDlgLL->MoveWindow(20, 20, rect.Width(), rect.Height());
		m_pDlgLL->ShowWindow(SW_SHOW);
		break;

	case 5:
		m_ctrlTab.InsertItem(0, _T("VACRobot"));
		m_pDlgVAC = new CFormVAC;
		m_pDlgVAC->Create(IDD_DIALOG_VAC, &m_ctrlTab);
		m_pDlgVAC->MoveWindow(20, 20, rect.Width(), rect.Height());
		m_pDlgVAC->ShowWindow(SW_SHOW);
		break;

	case 6:
		m_ctrlTab.InsertItem(0, _T("PM"));
		m_pDlgPM = new CFormPM;
		m_pDlgPM->Create(IDD_DIALOG_PM, &m_ctrlTab);
		m_pDlgPM->MoveWindow(20, 20, rect.Width(), rect.Height());
		m_pDlgPM->ShowWindow(SW_SHOW);
		break;
	}
	
	m_ctrlTab.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CSelCreateModuleDlg::OnBnClickedOk()
{
	int nCurSel = m_ctrlTab.GetCurSel();

	// 모듈 인스턴스 생성 (컨트롤러 접근)

	switch (m_nCol)
	{
	case 2:
		m_pDlgLPM->UpdateData(1);
		CFabController::GetInstance().CreateModule(m_pDlgLPM, 0);
		break;

	case 3:
		m_pDlgATM->UpdateData(1);
		CFabController::GetInstance().CreateModule(m_pDlgATM, 1);

		break;

	case 4:
		m_pDlgLL->UpdateData(1);
		CFabController::GetInstance().CreateModule(m_pDlgLL, 2);

		break;

	case 5:
		m_pDlgVAC->UpdateData(1);
		CFabController::GetInstance().CreateModule(m_pDlgVAC, 3);

		break;

	case 6:
		m_pDlgPM->UpdateData(1);
		CFabController::GetInstance().CreateModule(m_pDlgPM, 4);

		break;
	}

	CDialogEx::OnOK();
}

void CSelCreateModuleDlg::OnTcnSelchangeTabModule(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	switch (m_nCol)
	{
	case 2:
		m_pDlgLPM->ShowWindow(SW_SHOW);
		break;

	case 3:
		m_pDlgATM->ShowWindow(SW_SHOW);
		break;

	case 4:
		m_pDlgLL->ShowWindow(SW_SHOW);
		break;

	case 5:
		m_pDlgVAC->ShowWindow(SW_SHOW);
		break;

	case 6:
		m_pDlgPM->ShowWindow(SW_SHOW);
		break;
	}
	
	*pResult = 0;
}
