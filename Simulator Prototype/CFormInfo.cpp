// CFormInfo.cpp: 구현 파일
//

#include "pch.h"
#include "Simulator Prototype.h"
#include "Simulator PrototypeDlg.h"
#include "afxdialogex.h"
#include "CFormInfo.h"


// CFormInfo 대화 상자

IMPLEMENT_DYNAMIC(CFormInfo, CDialogEx)

CFormInfo::CFormInfo(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_INFO, pParent)
	, m_strModuleType(_T("-"))
	, m_strModuleName(_T("-"))
	, m_strWaferMax(_T("-"))
	, m_dModuleThruput(0.0)
	, m_nModuleInputCnt(0)
	, m_nModuleOutputCnt(0)
	, m_dFabThroughput(0.0)
	, m_nFabInputCnt(0)
	, m_nFabOutputCnt(0)
	
{

}

CFormInfo::~CFormInfo()
{
}

void CFormInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_UNIT_TYPE_VALUE, m_strModuleType);
	DDX_Text(pDX, IDC_STATIC_UNIT_NAME_VALUE, m_strModuleName);
	DDX_Text(pDX, IDC_STATIC_UNIT_THRUPUT_VALUE, m_dModuleThruput);
	DDX_Text(pDX, IDC_STATIC_UNIT_INPUT_VALUE, m_nModuleInputCnt);
	DDX_Text(pDX, IDC_STATIC_UNIT_OUTPUT_VALUE, m_nModuleOutputCnt);
	DDX_Text(pDX, IDC_STATIC_FAB_THROUGHPUT_VALUE, m_dFabThroughput);
	DDX_Text(pDX, IDC_STATIC_FAB_INPUT_VALUE, m_nFabInputCnt);
	DDX_Text(pDX, IDC_STATIC_FAB_OUTPUT_VALUE, m_nFabOutputCnt);
	DDX_Text(pDX, IDC_STATIC_UNIT_WAFERMAX_VALUE, m_strWaferMax);
}


BEGIN_MESSAGE_MAP(CFormInfo, CDialogEx)
END_MESSAGE_MAP()


// CFormInfo 메시지 처리기


BOOL CFormInfo::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	SetBackgroundColor(RGB(255, 255, 255));
	
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CFormInfo::InitializeFormInfo(bool bFlag)
{
	m_strModuleType = _T("-");
	m_strModuleName = _T("-");
	m_strWaferMax = (_T("-"));
	m_dModuleThruput = 0.0;
	m_nModuleInputCnt = 0;
	m_nModuleOutputCnt = 0;
	
	if (bFlag)
	{
		UpdateData(false);
	}
	else
	{
		m_dFabThroughput = 0.0;
		m_nFabInputCnt = 0;
		m_nFabOutputCnt = 0;

		::PostMessage(AfxGetApp()->GetMainWnd()->m_hWnd, UPDATE_MSG, 0, 0);
	}
}
