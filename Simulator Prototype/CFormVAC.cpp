// CFormVAC.cpp: 구현 파일
//

#include "pch.h"
#include "Simulator Prototype.h"
#include "afxdialogex.h"
#include "CFormVAC.h"


// CFormVAC 대화 상자

IMPLEMENT_DYNAMIC(CFormVAC, CDialogEx)

CFormVAC::CFormVAC(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_VAC, pParent)
	, m_nPickTime(0)
	, m_nPlaceTime(0)
	, m_nRotateTime(0)
	, m_strObjName(_T(""))
{

}

CFormVAC::~CFormVAC()
{
}

void CFormVAC::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_VAC_ARM, m_ctrlArm);
	DDX_Control(pDX, IDC_SPIN_VAC_PICKTIME, m_spPickTime);
	DDX_Control(pDX, IDC_SPIN_VAC_PLACETIME, m_spPlaceTime);
	DDX_Control(pDX, IDC_SPIN_VAC_ROTATETIME, m_spRotateTime);
	DDX_Text(pDX, IDC_EDIT_VAC_PICKTIME, m_nPickTime);
	DDV_MinMaxInt(pDX, m_nPickTime, 1, 30);
	DDX_Text(pDX, IDC_EDIT_VAC_PLACETIME, m_nPlaceTime);
	DDV_MinMaxInt(pDX, m_nPlaceTime, 1, 30);
	DDX_Text(pDX, IDC_EDIT_VAC_ROTATETIME, m_nRotateTime);
	DDV_MinMaxInt(pDX, m_nRotateTime, 1, 10);
	DDX_Text(pDX, IDC_EDIT_VAC_OBJNAME, m_strObjName);
}


BEGIN_MESSAGE_MAP(CFormVAC, CDialogEx)
END_MESSAGE_MAP()


// CFormVAC 메시지 처리기


BOOL CFormVAC::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	SetBackgroundColor(RGB(255, 255, 255));

	// Spin Ctrl 초기화	
	m_spPickTime.SetRange(1, 30);
	m_spPlaceTime.SetRange(1, 30);
	m_spRotateTime.SetRange(1, 10);

	m_spPickTime.SetPos(5);
	m_spPlaceTime.SetPos(5);
	m_spRotateTime.SetPos(5);

	// ComboBox 초기화
	m_ctrlArm.AddString(_T("Dual Arm"));
	m_ctrlArm.AddString(_T("Quad Arm"));
	m_ctrlArm.SetCurSel(1);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
