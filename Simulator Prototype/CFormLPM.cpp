// CFormLPM.cpp: 구현 파일
//

#include "pch.h"
#include "Simulator Prototype.h"
#include "afxdialogex.h"
#include "CFormLPM.h"


// CFormLPM 대화 상자

IMPLEMENT_DYNAMIC(CFormLPM, CDialogEx)

CFormLPM::CFormLPM(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_LPM, pParent)
	, m_nWaferMax(25)
	, m_strObjName(_T(""))
{
}

CFormLPM::CFormLPM(CString strModuleName, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_LPM, pParent)
	, m_nWaferMax(25)
	, m_strObjName(strModuleName)
{

}

CFormLPM::~CFormLPM()
{
}

void CFormLPM::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SPIN_LPM_WAFERMAX, m_spWaferMax);
	DDX_Text(pDX, IDC_EDIT_LPM_WAFERMAX, m_nWaferMax);
	DDV_MinMaxInt(pDX, m_nWaferMax, 1, 25);
	DDX_Text(pDX, IDC_EDIT_LPM_OBJNAME, m_strObjName);
}


BEGIN_MESSAGE_MAP(CFormLPM, CDialogEx)
END_MESSAGE_MAP()


// CFormLPM 메시지 처리기


BOOL CFormLPM::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	SetBackgroundColor(RGB(255, 255, 255));
	
	// Spin Ctrl 초기화 (LPM Wafer Max 제한 25)
	m_spWaferMax.SetRange(1, 25);
	m_spWaferMax.SetPos(25);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}