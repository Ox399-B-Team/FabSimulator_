// CFormUnitATM.cpp: 구현 파일
//

#include "pch.h"
#include "Simulator Prototype.h"
#include "afxdialogex.h"
#include "CFormUnitATM.h"


// CFormUnitATM 대화 상자

IMPLEMENT_DYNAMIC(CFormUnitATM, CDialogEx)

CFormUnitATM::CFormUnitATM(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_UNIT_ATM, pParent)
{

}

CFormUnitATM::~CFormUnitATM()
{
}

void CFormUnitATM::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFormUnitATM, CDialogEx)
END_MESSAGE_MAP()


// CFormUnitATM 메시지 처리기
