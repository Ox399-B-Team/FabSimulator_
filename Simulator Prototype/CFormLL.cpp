// CFormLL.cpp: 구현 파일
//

#include "pch.h"
#include "Simulator Prototype.h"
#include "afxdialogex.h"
#include "CFormLL.h"


// CFormLL 대화 상자

IMPLEMENT_DYNAMIC(CFormLL, CDialogEx)

CFormLL::CFormLL(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_LL, pParent)
	, m_nWaferMax(0)
	, m_nPumpTime(0)
	, m_nVentTime(0)
	, m_nPumpStable(0)
	, m_nVentStable(0)
	, m_nSlotOpen(0)
	, m_nDoorOpen(0)
	, m_nSlotClose(0)
	, m_nDoorClose(0)
	, m_strObjName(_T(""))
{

}

CFormLL::~CFormLL()
{
}

void CFormLL::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SPIN_LL_WAFERMAX, m_spWaferMax);
	DDX_Control(pDX, IDC_SPIN_LL_PUMPTIME, m_spPumpTime);
	DDX_Control(pDX, IDC_SPIN_LL_VENTTIME, m_spVentTime);
	DDX_Control(pDX, IDC_SPIN_LL_PUMPSTABLE, m_spPumpStable);
	DDX_Control(pDX, IDC_SPIN_LL_VENTSTABLE, m_spVentStable);
	DDX_Control(pDX, IDC_SPIN_LL_SLOTOPEN, m_spSlotOpen);
	DDX_Control(pDX, IDC_SPIN_LL_DOOROPEN, m_spDoorOpen);
	DDX_Control(pDX, IDC_SPIN_LL_SLOTCLOSE, m_spSlotClose);
	DDX_Control(pDX, IDC_SPIN_LL_DOORCLOSE, m_spDoorClose);
	DDX_Text(pDX, IDC_EDIT_LL_WAFERMAX, m_nWaferMax);
	DDV_MinMaxInt(pDX, m_nWaferMax, 1, 6);
	DDX_Text(pDX, IDC_EDIT_LL_PUMPTIME, m_nPumpTime);
	DDV_MinMaxInt(pDX, m_nPumpTime, 1, 30);
	DDX_Text(pDX, IDC_EDIT_LL_VENTTIME, m_nVentTime);
	DDV_MinMaxInt(pDX, m_nVentTime, 1, 30);
	DDX_Text(pDX, IDC_EDIT_LL_PUMPSTABLE, m_nPumpStable);
	DDV_MinMaxInt(pDX, m_nPumpStable, 1, 20);
	DDX_Text(pDX, IDC_EDIT_LL_VENTSTABLE, m_nVentStable);
	DDV_MinMaxInt(pDX, m_nVentStable, 1, 20);
	DDX_Text(pDX, IDC_EDIT_LL_SLOTOPEN, m_nSlotOpen);
	DDV_MinMaxInt(pDX, m_nSlotOpen, 1, 10);
	DDX_Text(pDX, IDC_EDIT_LL_DOOROPEN, m_nDoorOpen);
	DDV_MinMaxInt(pDX, m_nDoorOpen, 1, 10);
	DDX_Text(pDX, IDC_EDIT_LL_SLOTCLOSE, m_nSlotClose);
	DDV_MinMaxInt(pDX, m_nSlotClose, 1, 10);
	DDX_Text(pDX, IDC_EDIT_LL_DOORCLOSE, m_nDoorClose);
	DDV_MinMaxInt(pDX, m_nDoorClose, 1, 10);
	DDX_Text(pDX, IDC_EDIT_LL_OBJNAME, m_strObjName);
}


BEGIN_MESSAGE_MAP(CFormLL, CDialogEx)
END_MESSAGE_MAP()


// CFormLL 메시지 처리기


BOOL CFormLL::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	SetBackgroundColor(RGB(255, 255, 255));

	// Spin Control 초기화
	m_spWaferMax.SetRange(1, 6);
	m_spPumpTime.SetRange(1, 99);
	m_spVentTime.SetRange(1, 99);
	m_spPumpStable.SetRange(1, 99);
	m_spVentStable.SetRange(1, 99);
	m_spSlotOpen.SetRange(1, 20);
	m_spDoorOpen.SetRange(1, 20);
	m_spSlotClose.SetRange(1, 20);
	m_spDoorClose.SetRange(1, 20);

	m_spWaferMax.SetPos(6);
	m_spPumpTime.SetPos(15);
	m_spVentTime.SetPos(15);
	m_spPumpStable.SetPos(5);
	m_spVentStable.SetPos(5);
	m_spSlotOpen.SetPos(2);
	m_spDoorOpen.SetPos(2);
	m_spSlotClose.SetPos(2);
	m_spDoorClose.SetPos(2);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
