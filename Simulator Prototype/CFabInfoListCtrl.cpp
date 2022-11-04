#pragma once
#include "pch.h"
#include "resource.h"
#include "CFabInfoListCtrl.h"
#include "CSelUpdateModuleDlg.h"
#include "CSelCreateModuleDlg.h"
#include "Simulator PrototypeDlg.h"

//////////임시 헤더///////////
#include "EFEM.h"
#include "LoadLock.h"
#include "VACRobot.h"
#include "ProcessChamber.h"
#include "CFabController.h"
/////////////////////////////

#define GREEN RGB(127, 185, 2)
#define ORANGE RGB(255, 152, 49) //(242, 79, 36)
#define BLUE RGB(0, 180, 242)
#define YELLOW RGB(255, 255, 0) //(255, 195, 0)
#define WHITE RGB(255, 255, 255)
#define BLACK RGB(0, 0, 0)
#define GRAY RGB(240, 240, 240)

#pragma region 생성자/소멸자
CFabInfoListCtrl::CFabInfoListCtrl()
{
	m_nCurRow = -1;
	m_nCurCol = -1;

	for (int i = 0; i < 14; i++)
	{
		for (int j = 0; j < 7; j++)
		{
			m_arrIsAvilable[i][j] = true;
}
	}

	// 생성 가능/불가능 추가 (셀렉션 포함)
	//for (int nRow = 0; nRow < 6; nRow++)
	//{
	//	for (int nCol = 0; nCol < 2; nCol++)
	//	{
	//		m_arrIsAvilable[nRow][nCol] = false;
	//	}
	//}
	
	// 모듈 최대 수량 제한 =============================================
	
	//m_arrIsAvilable[0][2] = false;
	//m_arrIsAvilable[0][3] = false;
	//m_arrIsAvilable[0][4] = false;
	//m_arrIsAvilable[5][2] = false;

	//m_arrIsAvilable[0][3] = false;
	//m_arrIsAvilable[1][3] = false;
	//m_arrIsAvilable[2][3] = false;
	//m_arrIsAvilable[4][3] = false;
	//m_arrIsAvilable[5][3] = false;

	//m_arrIsAvilable[0][4] = false;
	//m_arrIsAvilable[5][4] = false;

	//m_arrIsAvilable[0][5] = false;
	//m_arrIsAvilable[1][5] = false;
	//m_arrIsAvilable[2][5] = false;
	//m_arrIsAvilable[4][5] = false;
	//m_arrIsAvilable[5][5] = false;

	// ================================================================

}

CFabInfoListCtrl::~CFabInfoListCtrl()
{

}
#pragma endregion

#pragma region 메서드
BOOL CFabInfoListCtrl::InitListCtrl()
{
	ModifyStyle(LVS_OWNERDRAWFIXED, 0, 0);
	SetExtendedStyle(LVS_EX_GRIDLINES);
	SetExtendedStyle(GetExtendedStyle() | LVS_EX_SIMPLESELECT | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
	
	//int nMaxCell = 7;
	
	// 리스트컨트롤 Item 행 높이 조절
	CImageList cImagelist;
	cImagelist.Create(1, 70, ILC_COLORDDB, 1, 0);
	SetImageList(&cImagelist, LVSIL_SMALL);

	CString strColumn;

	// 컬럼
	InsertColumn(0, _T("-"), LVCFMT_CENTER, 52, 0);
	InsertColumn(1, _T("IN"), LVCFMT_CENTER, 83, 0);
	InsertColumn(2, _T("LPM"), LVCFMT_CENTER, 83, 0);
	InsertColumn(3, _T("ATMRobot"), LVCFMT_CENTER, 83, 0);
	InsertColumn(4, _T("LoadLock"), LVCFMT_CENTER, 83, 0);
	InsertColumn(5, _T("VACRobot"), LVCFMT_CENTER, 83, 0);
	InsertColumn(6, _T("PM"), LVCFMT_CENTER, 83, 0);
	InsertColumn(7, _T("VACRobot"), LVCFMT_CENTER, 83, 0);
	InsertColumn(8, _T("LoadLock"), LVCFMT_CENTER, 83, 0);
	InsertColumn(9, _T("ATMRobot"), LVCFMT_CENTER, 83, 0);
	InsertColumn(10, _T("LPM"), LVCFMT_CENTER, 83, 0);
	InsertColumn(11, _T("OUT"), LVCFMT_CENTER, 83, 0);
	
	// 로우
	for (int i = 1; i < 7; i++)
	{
		strColumn.Format(_T("%d"), i);
		InsertItem(i, strColumn, 0);
	}

	SetBkColor(GRAY);

	CFabController::GetInstance().m_pModule.push_back(new LPM(TYPE_LPM, _T("DummyStage"), 0, 12, 0, 2));
	
	CFabController::GetInstance().DrawModule();

	return TRUE;
}

#pragma endregion

BEGIN_MESSAGE_MAP(CFabInfoListCtrl, CListCtrl)
	ON_NOTIFY_REFLECT(NM_CLICK, &CFabInfoListCtrl::OnNMClick)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CFabInfoListCtrl::OnNMCustomdraw)
	ON_NOTIFY_REFLECT(NM_DBLCLK, &CFabInfoListCtrl::OnNMDblclk)
	ON_NOTIFY_REFLECT(NM_RCLICK, &CFabInfoListCtrl::OnNMRClick)
	ON_COMMAND(ID_MENU_CHANGEMODULE, &CFabInfoListCtrl::OnMenuChangemodule)
	ON_COMMAND(ID_MENU_DELETEMODULE, &CFabInfoListCtrl::OnMenuDeletemodule)
END_MESSAGE_MAP()

#pragma region ListCtrl

/* 색깔 저장
* pDraw->clrTextBk = RGB(127, 185, 2); 연두
* pDraw->clrTextBk = RGB(242, 79, 36); 주황
* pDraw->clrTextBk = RGB(0, 180, 242); 파랑
* pDraw->clrTextBk = RGB(255, 195, 0); 노랑
*/

// 리스트컨트롤 CustomDraw
void CFabInfoListCtrl::OnNMCustomdraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	SetItemState(-1, 0, LVIS_SELECTED | LVIS_FOCUSED);
	
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	NMLVCUSTOMDRAW* pDraw = (NMLVCUSTOMDRAW*)(pNMHDR);
	
	if (pNMCD->dwDrawStage == CDDS_PREPAINT)
	{
		*pResult = (LRESULT)CDRF_NOTIFYITEMDRAW;
		return; // 여기서 함수를 빠져 나가야 *pResult 값이 유지된다.
	}
	else if (pNMCD->dwDrawStage == CDDS_ITEMPREPAINT)
	{
		*pResult = (LRESULT)CDRF_NOTIFYSUBITEMDRAW;
		return;
	}
	else if (pNMCD->dwDrawStage == (CDDS_SUBITEM | CDDS_ITEMPREPAINT))
	{
		// sub-item이 그려지는 순간 위에서 *pResult 에 CDRF_NOTIFYSUBITEMDRAW 를 해서 여기로 옴
		CString text = GetItemText(pNMCD->dwItemSpec, pDraw->iSubItem);

		if (m_arrIsAvilable[pNMCD->dwItemSpec][pDraw->iSubItem] == false)
		{
			pDraw->clrText = BLACK;
			pDraw->clrTextBk = GRAY;
		}
		else if ((m_nCurRow == pNMCD->dwItemSpec) && (m_nCurCol == pDraw->iSubItem) && 
			m_arrIsAvilable[pNMCD->dwItemSpec][pDraw->iSubItem] == true)	// 현재 클릭된 Sel일 시 색 변경
		{
			pDraw->clrText = BLACK;
			pDraw->clrTextBk = YELLOW;
		}
		else if (!text.IsEmpty() && pDraw->iSubItem > 1)		// Sel에 모듈이 존재할 시
		{
			pDraw->clrText = WHITE;
			pDraw->clrTextBk = GREEN;

			//-----------------------------
			vector<ModuleBase*> pModule = CFabController::GetInstance().m_pModule;

			if (ModuleBase::s_bDirect == true)
				pDraw->clrTextBk = ORANGE;//주황색-역방향
			else
				pDraw->clrTextBk = GREEN;//연두색-정방향
		}
		else
		{
			pDraw->clrText = BLACK;
			pDraw->clrTextBk = WHITE;
		}

		*pResult = (LRESULT)CDRF_NEWFONT; // 이렇게 해야 설정한 대로 그려진다.
		return;
	}

	*pResult = 0;
}

// 마우스 L 클릭
void CFabInfoListCtrl::OnNMClick(NMHDR* pNMHDR, LRESULT* pResult)
{	
	// 이전 Row에 SetItemState 호출 (Row만 다시 NMCustomDraw 호출?)
	SetItemState(m_nCurRow, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	// 현재 Row, Col 저장
	m_nCurRow = pNMItemActivate->iItem;
	m_nCurCol = pNMItemActivate->iSubItem;

	// 리스트 컨트롤 전체에 NMCustomDraw 호출?
	SetItemState(m_nCurRow, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

	if (m_arrIsAvilable[m_nCurRow][m_nCurCol] == false) return;
	//if (GetItemText(m_nCurRow, m_nCurCol).IsEmpty()) return;
	
	// ListCtrl에서 Select 된 모듈 정보 불러오기
	int nModuleIdx;
	int nModuleType = CFabController::GetInstance().SelectModule(m_nCurRow, m_nCurCol, nModuleIdx);

	// Type 별 UpdateData 호출
	int nCurSel = ((CSimulatorPrototypeDlg*)AfxGetMainWnd())->m_ctrlInfoTab.GetCurSel();
	CFabController::GetInstance().PrintModuleInfo(nModuleIdx, nModuleType, nCurSel);

	*pResult = 0;
}

// 마우스 DB 클릭
void CFabInfoListCtrl::OnNMDblclk(NMHDR* pNMHDR, LRESULT* pResult)
{
	// Run 시 DBClick 제한
	if (((CSimulatorPrototypeDlg*)GetParent())->m_bIsRunning == true) return;

	SetItemState(m_nCurRow, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	m_nCurRow = pNMItemActivate->iItem;
	m_nCurCol = pNMItemActivate->iSubItem;

	SetItemState(m_nCurRow, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

	if (m_arrIsAvilable[m_nCurRow][m_nCurCol] == false)	return;
	if (!(GetItemText(m_nCurRow, m_nCurCol).IsEmpty())) return;	// 셀 비어있는지 판단해서 셀에 데이터가 있을 때만 (모듈이 들어가 있을때만) 함수종료
	if (CFabController::GetInstance().m_pModule[0]->IsRunning()) return;
	
	// CreateDlg 생성
	CSelCreateModuleDlg dlg(this, m_nCurRow, m_nCurCol);
	::GetCursorPos(&dlg.m_ptPos);

	int nSelOption = (int)dlg.DoModal();
	*pResult = 0;
}

// 마우스 R 클릭
void CFabInfoListCtrl::OnNMRClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	// Run 시 RClick 제한
	if (((CSimulatorPrototypeDlg*)GetParent())->m_bIsRunning == true) return;

	SetItemState(m_nCurRow, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	// 리스트컨트롤 클릭된 서브아이템 Row, Col idx가져오기
	
	m_nCurRow = pNMItemActivate->iItem;
	m_nCurCol = pNMItemActivate->iSubItem;

	SetItemState(m_nCurRow, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

	if (m_arrIsAvilable[m_nCurRow][m_nCurCol] == false) return;		
	if (GetItemText(m_nCurRow, m_nCurCol).IsEmpty()) return;
	if (CFabController::GetInstance().m_pModule[0]->IsRunning()) return;


	// 팝업메뉴 ===================================================================	
	CMenu popMenu;
	popMenu.LoadMenu(IDR_MENU_MODULE);
	CMenu* pSubMenu = popMenu.GetSubMenu(0);
	CPoint pContext;
	GetCursorPos(&pContext);
	
	pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, pContext.x, pContext.y, this);

	*pResult = 0;
}

#pragma endregion

#pragma region Menu

// 모듈 속성변경 서브메뉴 선택
void CFabInfoListCtrl::OnMenuChangemodule()
{
	CSelUpdateModuleDlg dlg(this, m_nCurRow, m_nCurCol);
	::GetCursorPos(&dlg.m_ptPos);
	
	int nSelOption = (int)dlg.DoModal();
}

// 모듈 삭제 서브메뉴 선택
void CFabInfoListCtrl::OnMenuDeletemodule()
{
	// 삭제 재확인 Dlg 캡션 변경을 위해 일시적으로 App의 m_pszAppName 변경
	LPCTSTR pAppNameTemp = AfxGetApp()->m_pszAppName;
	AfxGetApp()->m_pszAppName = _T("모듈 삭제");

	// 삭제 재확인 Dlg 호출..
	if (IDYES == MessageBox(_T("모듈을 삭제하시겠습니까?"), _T("모듈 삭제"), MB_YESNO))
	{
		// 컨트롤러의 vector<>에 접근 후 인스턴스 삭제..
		int nModuleIdx;
		CFabController::GetInstance().SelectModule(m_nCurRow, m_nCurCol, nModuleIdx);
		CFabController::GetInstance().DeleteModule(this, nModuleIdx);
	}

	// m_pszAppName 되돌려놓기
	AfxGetApp()->m_pszAppName = pAppNameTemp;
}

#pragma endregion