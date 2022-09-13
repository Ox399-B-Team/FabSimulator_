// CSelUpdateModuleDlg.cpp: 구현 파일
//

#include "pch.h"
#include "Simulator Prototype.h"
#include "afxdialogex.h"
#include "CSelUpdateModuleDlg.h"


// CSelUpdateModuleDlg 대화 상자

IMPLEMENT_DYNAMIC(CSelUpdateModuleDlg, CDialogEx)

CSelUpdateModuleDlg::CSelUpdateModuleDlg(CWnd* pParent, int nRow, int nCol)
	: CDialogEx(IDD_SEL_UPDATEMODULE, pParent)
{
	m_nModuleIdx = -1;
	m_nTabIdx = -1;
	m_nRow = nRow;
	m_nCol = nCol;
	m_pDlgLPM = NULL;
	m_pDlgATM = NULL;
	m_pDlgLL = NULL;
	m_pDlgVAC = NULL;
	m_pDlgPM = NULL;
}

CSelUpdateModuleDlg::~CSelUpdateModuleDlg()
{
	delete m_pDlgLPM;
	delete m_pDlgATM;
	delete m_pDlgLL;
	delete m_pDlgVAC;
	delete m_pDlgPM;
}

void CSelUpdateModuleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_UPDATEMODULE, m_ctrlUpdateTab);
}


BEGIN_MESSAGE_MAP(CSelUpdateModuleDlg, CDialogEx)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_UPDATEMODULE, &CSelUpdateModuleDlg::OnTcnSelchangeTabUpdatemodule)
	ON_BN_CLICKED(IDOK, &CSelUpdateModuleDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CSelUpdateModuleDlg 메시지 처리기

#include "CFabController.h"
BOOL CSelUpdateModuleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CRect rectTab;
	m_ctrlUpdateTab.GetWindowRect(&rectTab);
	m_nTabIdx = CFabController::GetInstance().SelectModule(m_nRow, m_nCol, m_nModuleIdx);
	
	switch (m_nTabIdx)
	{
	case 0:
		m_ctrlUpdateTab.InsertItem(0, _T("LPM"));
		m_pDlgLPM = new CFormLPM;
		m_pDlgLPM->Create(IDD_DIALOG_LPM, &m_ctrlUpdateTab);
		m_pDlgLPM->MoveWindow(20, 20, rectTab.Width(), rectTab.Height());
		m_pDlgLPM->ShowWindow(SW_SHOW);
		
		// Controller의 vector<ModuleBase*>에 접근 후 m_pForm의 컨트롤에 현재 모듈 필드 값 출력.
		CFabController::GetInstance().InitUpdateDlg(m_pDlgLPM, m_nModuleIdx);
		m_pDlgLPM->UpdateData(0);

		break;

	case 1:
		m_ctrlUpdateTab.InsertItem(0, _T("ATM"));
		m_pDlgATM = new CFormATM;
		m_pDlgATM->Create(IDD_DIALOG_ATM, &m_ctrlUpdateTab);
		m_pDlgATM->MoveWindow(20, 20, rectTab.Width(), rectTab.Height());
		m_pDlgATM->ShowWindow(SW_SHOW);

		// Controller의 vector<ModuleBase*>에 접근 후 m_pForm의 컨트롤에 현재 모듈 필드 값 출력.
		CFabController::GetInstance().InitUpdateDlg(m_pDlgATM, m_nModuleIdx);
		m_pDlgATM->UpdateData(0);

		break;

	case 2:
		m_ctrlUpdateTab.InsertItem(0, _T("LL"));
		m_pDlgLL = new CFormLL;
		m_pDlgLL->Create(IDD_DIALOG_LL, &m_ctrlUpdateTab);
		m_pDlgLL->MoveWindow(20, 20, rectTab.Width(), rectTab.Height());
		m_pDlgLL->ShowWindow(SW_SHOW);

		// Controller의 vector<ModuleBase*>에 접근 후 m_pForm의 컨트롤에 현재 모듈 필드 값 출력.
		CFabController::GetInstance().InitUpdateDlg(m_pDlgLL, m_nModuleIdx);
		m_pDlgLL->UpdateData(0);

		break;

	case 3:
		m_ctrlUpdateTab.InsertItem(0, _T("VAC"));
		m_pDlgVAC = new CFormVAC;
		m_pDlgVAC->Create(IDD_DIALOG_VAC, &m_ctrlUpdateTab);
		m_pDlgVAC->MoveWindow(20, 20, rectTab.Width(), rectTab.Height());
		m_pDlgVAC->ShowWindow(SW_SHOW);

		// Controller의 vector<ModuleBase*>에 접근 후 m_pForm의 컨트롤에 현재 모듈 필드 값 출력.
		CFabController::GetInstance().InitUpdateDlg(m_pDlgVAC, m_nModuleIdx);
		m_pDlgVAC->UpdateData(0);

		break;

	case 4:
		m_ctrlUpdateTab.InsertItem(0, _T("PM"));
		m_pDlgPM = new CFormPM;
		m_pDlgPM->Create(IDD_DIALOG_PM, &m_ctrlUpdateTab);
		m_pDlgPM->MoveWindow(20, 20, rectTab.Width(), rectTab.Height());
		m_pDlgPM->ShowWindow(SW_SHOW);

		// Controller의 vector<ModuleBase*>에 접근 후 m_pForm의 컨트롤에 현재 모듈 필드 값 출력.
		CFabController::GetInstance().InitUpdateDlg(m_pDlgPM, m_nModuleIdx);
		m_pDlgPM->UpdateData(0);

		break;

	default:
		return FALSE;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

// 확인 버튼
void CSelUpdateModuleDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	switch (m_nTabIdx)
	{
	case 0:
	{
		m_pDlgLPM->UpdateData(1);

		// Controller에 접근 후 Update 수행
		CFabController::GetInstance().UpdateModule(m_pDlgLPM, m_nModuleIdx);
		break;
	}
	case 1:
	{
		m_pDlgATM->UpdateData(1);

		// Controller에 접근 후 Update 수행
		CFabController::GetInstance().UpdateModule(m_pDlgATM, m_nModuleIdx);

		break;
	}
	case 2:
	{
		m_pDlgLL->UpdateData(1);

		// Controller에 접근 후 Update 수행
		CFabController::GetInstance().UpdateModule(m_pDlgLL, m_nModuleIdx);

		break;
	}
	case 3:
	{
		m_pDlgVAC->UpdateData(1);

		// Controller에 접근 후 Update 수행
		CFabController::GetInstance().UpdateModule(m_pDlgVAC, m_nModuleIdx);

		break;
	}
	case 4:
	{
		m_pDlgPM->UpdateData(1);

		// Controller에 접근 후 Update 수행
		CFabController::GetInstance().UpdateModule(m_pDlgPM, m_nModuleIdx);
		
		break;
	}
	default:
		break;
	}


	CDialogEx::OnOK();
}

// 모듈 타입 변경 기능(탭 Sel Change) 보류.. 추후 심화 때 구현
void CSelUpdateModuleDlg::OnTcnSelchangeTabUpdatemodule(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;
}