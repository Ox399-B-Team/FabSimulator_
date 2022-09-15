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

#pragma region 생성자/소멸자
CFabInfoListCtrl::CFabInfoListCtrl()
{
	m_nCurRow = -1;
	m_nCurCol = -1;
	m_strModuleName = _T("");
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
	SetExtendedStyle(GetExtendedStyle() | LVS_EX_SIMPLESELECT | LVS_EX_FULLROWSELECT);
	
	int nMaxCell = 21;
	
	// 리스트컨트롤 Item 행 높이 조절
	CImageList cImagelist;
	cImagelist.Create(1, 68, ILC_COLORDDB, 1, 0);
	SetImageList(&cImagelist, LVSIL_SMALL);

	CString strColumn;

	// 컬럼
	for (int i = 0; i < nMaxCell; i++)
	{
		strColumn.Format(_T("%d"), i);
		InsertColumn(i+1, strColumn, LVCFMT_CENTER, 68, 0);
	}

	// 로우
	for (int i = 1; i < 7; i++)
	{
		strColumn.Format(_T("%d"), i);
		InsertItem(i, strColumn, 0);
	}

	CFabController::GetInstance().m_pModule.push_back(new LPM(TYPE_LPM, _T("LPM01"), 25, 25, 2, 1));
	CFabController::GetInstance().m_pModule.push_back(new LPM(TYPE_LPM, _T("LPM02"), 25, 25, 3, 1));
	CFabController::GetInstance().m_pModule.push_back(new LPM(TYPE_LPM, _T("LPM03"), 25, 25, 4, 1));
	CFabController::GetInstance().m_pModule.push_back(new ATMRobot(TYPE_ATMROBOT, _T("ATMRobot"), 0, 2, 3, 2, 4, 4, 1, 1));
	CFabController::GetInstance().m_pModule.push_back(new LoadLock(TYPE_LOADLOCK, _T("LL1"), 0, 6, 2, 3, 15, 5, 15, 5, 2, 2, 2, 2));
	CFabController::GetInstance().m_pModule.push_back(new LoadLock(TYPE_LOADLOCK, _T("LL2"), 0, 6, 4, 3, 15, 5, 15, 5, 2, 2, 2, 2));
	CFabController::GetInstance().m_pModule.push_back(new VACRobot(TYPE_VACROBOT, _T("TM"), 0, 4, 3, 4, 4, 4, 4));
	CFabController::GetInstance().m_pModule.push_back(new ProcessChamber(TYPE_PROCESSCHAMBER, _T("PM01"), 0, 6, 2, 5, 600, 1200, 2, 2, 2));
	CFabController::GetInstance().m_pModule.push_back(new ProcessChamber(TYPE_PROCESSCHAMBER, _T("PM02"), 0, 6, 4, 5, 600, 1200, 2, 2, 2));

	for (int i = 0; i < CFabController::GetInstance().m_pModule.size(); i++)
	{
		ModuleBase* pM = CFabController::GetInstance().m_pModule[i];

		if (pM->m_eModuleType == TYPE_LPM)
			CFabController::s_pLPM.push_back(pM);
		else if (pM->m_eModuleType == TYPE_ATMROBOT)
			CFabController::s_pATMRobot.push_back(pM);
		else if (pM->m_eModuleType == TYPE_LOADLOCK)
			CFabController::s_pLL.push_back(pM);
		else if (pM->m_eModuleType == TYPE_VACROBOT)
			CFabController::s_pVACRobot.push_back(pM);
		else if (pM->m_eModuleType == TYPE_PROCESSCHAMBER)
			CFabController::s_pPM.push_back(pM);
	}

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
		
		if ((m_nCurRow == pNMCD->dwItemSpec) && (m_nCurCol == pDraw->iSubItem) && (pDraw->iSubItem != 0))	// 현재 클릭된 Sel일 시 파란색 표시
		{
			pDraw->clrText = RGB(255, 255, 255);
			pDraw->clrTextBk = RGB(0, 0, 200);
		}
		else if (!text.IsEmpty() && pDraw->iSubItem != 0)		// Sel에 모듈이 존재할 시
		{
			NMLVDISPINFO* pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

			pDraw->clrText = RGB(255, 255, 255);
			pDraw->clrTextBk = RGB(200, 0, 0);
		}
		else
		{
			pDraw->clrText = 0x0;
			pDraw->clrTextBk = 0xffffff;
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

	if (m_nCurCol == 0)									// 1st 컬럼은 대화상자 처리 하지 않음
		return;

	if (GetItemText(m_nCurRow, m_nCurCol).IsEmpty())	// 셀 비어있는지 판단해서 셀에 데이터가 있을 때만 (모듈이 들어가 있을때만) 코드 수행
		return;

	// 모듈 정보 출력 구현
	int nModuleIdx;
	CFabController::GetInstance().SelectModule(m_nCurRow, m_nCurCol, nModuleIdx);
	CFabController::GetInstance().PrintModule((CDialogEx*)this->GetParent(), nModuleIdx);


	*pResult = 0;
	//*pResult = (LRESULT)CDRF_NOTIFYSUBITEMDRAW;
}

// 마우스 DB 클릭
void CFabInfoListCtrl::OnNMDblclk(NMHDR* pNMHDR, LRESULT* pResult)
{
	SetItemState(m_nCurRow, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	m_nCurRow = pNMItemActivate->iItem;
	m_nCurCol = pNMItemActivate->iSubItem;

	SetItemState(m_nCurRow, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

	if (m_nCurCol == 0)	return;									// 1st 컬럼은 대화상자 처리 하지 않음
	if (!(GetItemText(m_nCurRow, m_nCurCol).IsEmpty())) return;	// 셀 비어있는지 판단해서 셀에 데이터가 있을 때만 (모듈이 들어가 있을때만) 함수종료

	// CreateDlg 생성
	CSelCreateModuleDlg dlg(this);
	::GetCursorPos(&dlg.m_ptPos);

	int nSelOption = (int)dlg.DoModal();

	*pResult = 0;
}

// 마우스 R 클릭
void CFabInfoListCtrl::OnNMRClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	SetItemState(m_nCurRow, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	// 리스트컨트롤 클릭된 서브아이템 Row, Col idx가져오기
	
	m_nCurRow = pNMItemActivate->iItem;
	m_nCurCol = pNMItemActivate->iSubItem;

	SetItemState(m_nCurRow, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

	if (m_nCurCol == 0) return;
	if (GetItemText(m_nCurRow, m_nCurCol).IsEmpty()) return;

	// 팝업메뉴 ===================================================================	
	CMenu popMenu;
	popMenu.LoadMenu(IDR_MENU_MODULE);
	CMenu* pSubMenu = popMenu.GetSubMenu(0);
	CPoint pContext;
	GetCursorPos(&pContext);
	
	pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, pContext.x, pContext.y, this);

	// 먼저 현재 선택상태를 해제합니다
	//SetItemState(-1, 0, LVIS_SELECTED | LVIS_FOCUSED);

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
	if (IDYES == AfxMessageBox(_T("모듈을 삭제하시겠습니까?"), MB_YESNO))
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