// CFormATM.cpp: 구현 파일
//

#include "pch.h"
#include "Simulator Prototype.h"
#include "afxdialogex.h"
#include "CFormATM.h"


// CFormATM 대화 상자

IMPLEMENT_DYNAMIC(CFormATM, CDialogEx)

CFormATM::CFormATM(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_ATM, pParent)
	, m_nPickTime(0)
	, m_nPlaceTime(0)
	, m_nRotateTime(0)
	, m_nZRotateTime(0)
	, m_strObjName(_T(""))
{

}

CFormATM::~CFormATM()
{
}

void CFormATM::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SPIN_ATM_PICKTIME, m_spPickTime);
	DDX_Control(pDX, IDC_SPIN_ATM_PLACETIME, m_spPlaceTime);
	DDX_Control(pDX, IDC_SPIN_ATM_ROTATETIME, m_spRotateTime);
	DDX_Control(pDX, IDC_SPIN_ATM_ZROTATETIME, m_spZRotateTime);
	DDX_Text(pDX, IDC_EDIT_ATM_PICKTIME, m_nPickTime);
	DDV_MinMaxInt(pDX, m_nPickTime, 1, 30);
	DDX_Text(pDX, IDC_EDIT_ATM_PLACETIME, m_nPlaceTime);
	DDV_MinMaxInt(pDX, m_nPlaceTime, 1, 30);
	DDX_Text(pDX, IDC_EDIT_ATM_ROTATETIME, m_nRotateTime);
	DDV_MinMaxInt(pDX, m_nRotateTime, 1, 10);
	DDX_Text(pDX, IDC_EDIT_ATM_ZROTATETIME, m_nZRotateTime);
	DDV_MinMaxInt(pDX, m_nZRotateTime, 1, 10);
	DDX_Control(pDX, IDC_COMBO_ATM_ARM, m_ctrlArm);
	DDX_Text(pDX, IDC_EDIT_ATM_OBJNAME, m_strObjName);
}


BEGIN_MESSAGE_MAP(CFormATM, CDialogEx)
END_MESSAGE_MAP()


// CFormATM 메시지 처리기


BOOL CFormATM::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	SetBackgroundColor(RGB(255, 255, 255));

	// Spin Ctrl 초기화
	m_spPickTime.SetRange(1, 30);
	m_spPlaceTime.SetRange(1, 30);
	m_spRotateTime.SetRange(1, 10);
	m_spZRotateTime.SetRange(1, 10);

	m_spPickTime.SetPos(4);
	m_spPlaceTime.SetPos(4);
	m_spRotateTime.SetPos(1);
	m_spZRotateTime.SetPos(1);
	
	// ComboBox 초기화
	m_ctrlArm.AddString(_T("Dual Arm"));
	m_ctrlArm.AddString(_T("Quad Arm"));
	m_ctrlArm.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
