// CFormPM.cpp: 구현 파일
//

#include "pch.h"
#include "Simulator Prototype.h"
#include "afxdialogex.h"
#include "CFormPM.h"


// CFormPM 대화 상자

IMPLEMENT_DYNAMIC(CFormPM, CDialogEx)

CFormPM::CFormPM(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_PM, pParent)
	, m_nWaferMax(0)
	, m_nCleanTime(0)
	, m_nSlotOpen(0)
	, m_nSlotClose(0)
	, m_nProcessTime(0)
	, m_nCleanCount(0)
	, m_strObjName(_T(""))
{

}

CFormPM::~CFormPM()
{
}

void CFormPM::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SPIN_PM_WAFERMAX, m_spWaferMax);
	DDX_Control(pDX, IDC_SPIN_PM_CELANTIME, m_spCleanTime);
	DDX_Control(pDX, IDC_SPIN_PM_CELANCOUNT, m_spCleanCount);
	DDX_Control(pDX, IDC_SPIN_PM_SLOTOPEN, m_spSlotOpen);
	DDX_Control(pDX, IDC_SPIN_PM_SLOTCLOSE, m_spSlotClose);
	DDX_Control(pDX, IDC_SPIN_PM_PROCESSTIME, m_spProcessTime);
	DDX_Text(pDX, IDC_EDIT_PM_WAFERMAX, m_nWaferMax);
	DDV_MinMaxInt(pDX, m_nWaferMax, 1, 6);
	DDX_Text(pDX, IDC_EDIT_PM_CELANTIME, m_nCleanTime);
	DDV_MinMaxInt(pDX, m_nCleanTime, 1, 5000);
	DDX_Text(pDX, IDC_EDIT_PM_SLOTOPEN, m_nSlotOpen);
	DDV_MinMaxInt(pDX, m_nSlotOpen, 1, 10);
	DDX_Text(pDX, IDC_EDIT_PM_SLOTCLOSE, m_nSlotClose);
	DDV_MinMaxInt(pDX, m_nSlotClose, 1, 10);
	DDX_Text(pDX, IDC_EDIT_PM_PROCESSTIME, m_nProcessTime);
	DDV_MinMaxInt(pDX, m_nProcessTime, 1, 5000);
	DDX_Text(pDX, IDC_EDIT_PM_CELANCOUNT, m_nCleanCount);
	DDV_MinMaxInt(pDX, m_nCleanCount, 1, 30);
	DDX_Text(pDX, IDC_EDIT_PM_OBJNAME, m_strObjName);
}


BEGIN_MESSAGE_MAP(CFormPM, CDialogEx)
END_MESSAGE_MAP()


// CFormPM 메시지 처리기


BOOL CFormPM::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	SetBackgroundColor(RGB(255, 255, 255));

	// Spin Control 초기화
	m_spWaferMax.SetRange(1, 6);
	m_spCleanTime.SetRange(1, 5000);
	m_spCleanCount.SetRange(1, 30);
	m_spSlotOpen.SetRange(1, 10);
	m_spSlotClose.SetRange(1, 10);
	m_spProcessTime.SetRange(1, 5000);

	m_spWaferMax.SetPos(6);
	m_spCleanTime.SetPos(1200);
	m_spCleanCount.SetPos(10);
	m_spSlotOpen.SetPos(2);
	m_spSlotClose.SetPos(2);
	m_spProcessTime.SetPos(600);

	// Spin Control 초기화

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
