#pragma once

#include "pch.h"
#include "CFabController.h"

vector<ModuleBase*> CFabController::s_pLPM;
vector<ModuleBase*> CFabController::s_pATMRobot;
vector<ModuleBase*> CFabController::s_pLL;
vector<ModuleBase*> CFabController::s_pVACRobot;
vector<ModuleBase*> CFabController::s_pPM;

bool CFabController::s_bAllWorkOver;
HANDLE CFabController::s_hMoniteringThread;
HANDLE CFabController::s_hMoniteringThread2;

#pragma region 생성자/소멸자

CFabController::CFabController()
{
	m_pMainDlg = (CSimulatorPrototypeDlg*)AfxGetMainWnd();
}

CFabController::CFabController(const CFabController& other)
{
	m_pMainDlg = (CSimulatorPrototypeDlg*)AfxGetMainWnd();
}

CFabController::~CFabController()
{
}

#pragma endregion

#include "EFEM.h"
#include "LoadLock.h"
#include "VACRobot.h"
#include "ProcessChamber.h"

#pragma region 메서드

int CFabController::SelectModule(int nRow, int nCol, int& pModuleIdx)
{
	for (int i = 0; i < m_pModule.size(); i++)
	{
		if (m_pModule[i]->m_nCol == nCol && m_pModule[i]->m_nRow == nRow)
		{
			pModuleIdx = i;
			switch (m_pModule[i]->m_eModuleType)
			{
			case TYPE_LPM:
				return (int)TYPE_LPM;

			case TYPE_ATMROBOT:
				return (int)TYPE_ATMROBOT;

			case TYPE_LOADLOCK:
				return (int)TYPE_LOADLOCK;

			case TYPE_VACROBOT:
				return (int)TYPE_VACROBOT;

			case TYPE_PROCESSCHAMBER:
				return (int)TYPE_PROCESSCHAMBER;
			}
		}
	}

	return -1;
}

void CFabController::DrawModule()
{
	for (int i = 0; i < (int)m_pModule.size(); i++)
	{
		m_pMainDlg->m_ctrlListFabInfo.SetItemText(m_pModule[i]->m_nRow, m_pModule[i]->m_nCol, m_pModule[i]->GetModuleName());
	}
}

BOOL CFabController::InitUpdateDlg(CDialogEx* pDlg, int nModuleIdx)
{
	switch (m_pModule[nModuleIdx]->m_eModuleType)
	{
	case TYPE_LPM:
	{
		LPM* pModule = (LPM*)m_pModule[nModuleIdx];
		CFormLPM* pForm = (CFormLPM*)pDlg;

		pForm->m_strObjName = pModule->GetModuleName();
		pForm->m_nWaferMax = pModule->GetWaferMax();

		return TRUE;
	}
	case TYPE_ATMROBOT:
	{
		ATMRobot* pModule = (ATMRobot*)m_pModule[nModuleIdx];
		CFormATM* pForm = (CFormATM*)pDlg;

		pForm->m_strObjName = pModule->GetModuleName();

		if (pModule->GetWaferMax() == 2) pForm->m_ctrlArm.SetCurSel(0);
		else pForm->m_ctrlArm.SetCurSel(1);

		pForm->m_nPickTime = pModule->GetPickTime();
		pForm->m_nPlaceTime = pModule->GetPlaceTime();
		pForm->m_nRotateTime = pModule->GetRotateTime();
		pForm->m_nZRotateTime = pModule->GetRotateZTime();

		return TRUE;
	}
	case TYPE_LOADLOCK:
	{
		LoadLock* pModule = (LoadLock*)m_pModule[nModuleIdx];
		CFormLL* pForm = (CFormLL*)pDlg;

		pForm->m_strObjName = pModule->GetModuleName();
		pForm->m_nWaferMax = pModule->GetWaferMax();

		pForm->m_nPumpTime = pModule->GetPumpTime();
		pForm->m_nPumpStable = pModule->GetPumpStableTime();
		pForm->m_nVentTime = pModule->GetVentTime();
		pForm->m_nVentStable = pModule->GetVentStableTime();
		pForm->m_nDoorOpen = pModule->GetDoorValveOpenTime();
		pForm->m_nDoorClose = pModule->GetDoorValveCloseTime();
		pForm->m_nSlotOpen = pModule->GetSlotValveOpenTime();
		pForm->m_nSlotClose = pModule->GetSlotValveCloseTime();

		return TRUE;
	}
	case TYPE_VACROBOT:
	{
		VACRobot* pModule = (VACRobot*)m_pModule[nModuleIdx];
		CFormVAC* pForm = (CFormVAC*)pDlg;

		pForm->m_strObjName = pModule->GetModuleName();

		if (pModule->GetWaferMax() == 2) pForm->m_ctrlArm.SetCurSel(0);
		else pForm->m_ctrlArm.SetCurSel(1);

		pForm->m_nPickTime = pModule->GetPickTime();
		pForm->m_nPlaceTime = pModule->GetPlaceTime();
		pForm->m_nRotateTime = pModule->GetRotateTime();

		return TRUE;
	}
	case TYPE_PROCESSCHAMBER:
	{
		ProcessChamber* pModule = (ProcessChamber*)m_pModule[nModuleIdx];
		CFormPM* pForm = (CFormPM*)pDlg;

		pForm->m_strObjName = pModule->GetModuleName();
		pForm->m_nWaferMax = pModule->GetWaferMax();
		pForm->m_nProcessTime = pModule->GetProcessTime();
		pForm->m_nCleanTime = pModule->GetCleanTime();
		pForm->m_nCleanCount = pModule->GetCleanCount();
		pForm->m_nSlotOpen = pModule->GetSlotValveOpenTime();
		pForm->m_nSlotClose = pModule->GetSlotValveCloseTime();

		return TRUE;
	}
	default:
		return FALSE;
	}

}

void CFabController::CreateModule(CDialogEx* pDlg, int nModuleIdx)
{
	// Type
	ModuleType eType = (ModuleType)nModuleIdx;

	// 모듈 Row, Col 값
	int nCol = m_pMainDlg->m_ctrlListFabInfo.m_nCurCol;
	int nRow = m_pMainDlg->m_ctrlListFabInfo.m_nCurRow;

	switch (eType)
	{
	case TYPE_LPM:
	{
		CFormLPM* pForm = (CFormLPM*)pDlg;

		m_pModule.push_back(new LPM(eType, pForm->m_strObjName, pForm->m_nWaferMax, pForm->m_nWaferMax, nRow, nCol));

		break;
	}
	case TYPE_ATMROBOT:
	{
		CFormATM* pForm = (CFormATM*)pDlg;
		
		int nPickTime = pForm->m_nPickTime;
		int nPlaceTime = pForm->m_nPlaceTime;
		int nRotateTime = pForm->m_nRotateTime;
		int nZRotateTime = pForm->m_nZRotateTime;

		m_pModule.push_back(new ATMRobot(
			eType, pForm->m_strObjName, 0, 2, nRow, nCol, 
			nPickTime, nPlaceTime, nRotateTime, nZRotateTime));

		break;
	}
	case TYPE_LOADLOCK:
	{
		CFormLL* pForm = (CFormLL*)pDlg;

		int nWaferMax = pForm->m_nWaferMax;

		// 설정시간 값
		int nPumpTime = pForm->m_nPumpTime;
		int nPumpStable = pForm->m_nPumpStable;
		int nVentTime = pForm->m_nVentTime;
		int nVentStable = pForm->m_nVentStable;
		int nSlotOpen = pForm->m_nSlotOpen;
		int nSlotClose = pForm->m_nSlotClose;
		int nDoorOpen = pForm->m_nDoorOpen;
		int nDoorClose = pForm->m_nDoorClose;

		m_pModule.push_back(new LoadLock(
			eType, pForm->m_strObjName, 0, nWaferMax, nRow, nCol,
			nPumpTime, nPumpStable, nVentTime, nVentStable, nSlotOpen, 
			nSlotClose, nDoorOpen, nDoorClose));

		break;
	}
	case TYPE_VACROBOT:
	{
		CFormVAC* pForm = (CFormVAC*)pDlg;

		// Arm
		int nWaferMax = pForm->m_ctrlArm.GetCurSel() == 0 ? 4 : 2;
		
		// 설정 시간 값
		int nPickTime = pForm->m_nPickTime;
		int nPlaceTime = pForm->m_nPlaceTime;
		int nRotateTime = pForm->m_nRotateTime;

		m_pModule.push_back(new VACRobot(eType, pForm->m_strObjName, 0, nWaferMax, nRow, nCol, 
			nPickTime, nPlaceTime, nRotateTime));
		
		break;
	}
	case TYPE_PROCESSCHAMBER:
	{
		CFormPM* pForm = (CFormPM*)pDlg;

		// 설정시간 값
		int nProcessTime = pForm->m_nProcessTime;
		int nCleanTime = pForm->m_nCleanTime;
		int nSlotOpen = pForm->m_nSlotOpen;
		int nSlotClose = pForm->m_nSlotClose;
		int nCleanCount = pForm->m_nCleanCount;
		
		m_pModule.push_back(new ProcessChamber(eType, pForm->m_strObjName, 0, pForm->m_nWaferMax, nRow, nCol, 
			nProcessTime, nCleanTime, nSlotOpen, nSlotClose, nCleanCount));

		break;
	}
	}

	DrawModule();
}

void CFabController::UpdateModule(CDialogEx* pDlg, int nModuleIdx)
{
	switch (m_pModule[nModuleIdx]->m_eModuleType)
	{
	case TYPE_LPM:
	{
		LPM* pModule = (LPM*)m_pModule[nModuleIdx];
		CFormLPM* pForm = (CFormLPM*)pDlg;

		pModule->SetModuleName(pForm->m_strObjName);
		pModule->SetWaferMax(pForm->m_nWaferMax);
		
		m_pMainDlg->m_ctrlListFabInfo.SetItemText(pModule->m_nRow, pModule->m_nCol, pModule->GetModuleName());

		return;
	}
	case TYPE_ATMROBOT:
	{
		ATMRobot* pModule = (ATMRobot*)m_pModule[nModuleIdx];
		CFormATM* pForm = (CFormATM*)pDlg;

		pModule->SetModuleName(pForm->m_strObjName);
		pModule->SetWaferMax(pForm->m_ctrlArm.GetCurSel() == 0? 2 : 4);
		pModule->SetPickTime(pForm->m_nPickTime);
		pModule->SetPlaceTime(pForm->m_nPlaceTime);
		pModule->SetRotateTime(pForm->m_nRotateTime);
		pModule->SetRotateZTime(pForm->m_nZRotateTime);
		
		m_pMainDlg->m_ctrlListFabInfo.SetItemText(pModule->m_nRow, pModule->m_nCol, pModule->GetModuleName());

		return;
	}
	case TYPE_LOADLOCK:
	{
		LoadLock* pModule = (LoadLock*)m_pModule[nModuleIdx];
		CFormLL* pForm = (CFormLL*)pDlg;
		
		pModule->SetModuleName(pForm->m_strObjName);
		pModule->SetWaferMax(pForm->m_nWaferMax);
		pModule->SetPumpTime(pForm->m_nPumpTime);
		pModule->SetPumpStableTime(pForm->m_nPumpStable);
		pModule->SetVentTime(pForm->m_nVentTime);
		pModule->SetVentStableTime(pForm->m_nVentStable);
		pModule->SetDoorValveOpenTime(pForm->m_nDoorOpen);
		pModule->SetDoorValveCloseTime(pForm->m_nDoorClose);
		pModule->SetSlotValveOpenTime(pForm->m_nSlotOpen);
		pModule->SetSlotValveCloseTime(pForm->m_nSlotClose);

		m_pMainDlg->m_ctrlListFabInfo.SetItemText(pModule->m_nRow, pModule->m_nCol, pModule->GetModuleName());

		return;
	}
	case TYPE_VACROBOT:
	{
		VACRobot* pModule = (VACRobot*)m_pModule[nModuleIdx];
		CFormVAC* pForm = (CFormVAC*)pDlg;

		pModule->SetModuleName(pForm->m_strObjName);
		pModule->SetWaferMax(pForm->m_ctrlArm.GetCurSel() == 0 ? 2 : 4);
		pModule->SetPickTime(pForm->m_nPickTime);
		pModule->SetPlaceTime(pForm->m_nPlaceTime);
		pModule->SetRotateTime(pForm->m_nRotateTime);

		m_pMainDlg->m_ctrlListFabInfo.SetItemText(pModule->m_nRow, pModule->m_nCol, pModule->GetModuleName());

		return;
	}
	case TYPE_PROCESSCHAMBER:
	{
		ProcessChamber* pModule = (ProcessChamber*)m_pModule[nModuleIdx];
		CFormPM* pForm = (CFormPM*)pDlg;

		pModule->SetModuleName(pForm->m_strObjName);
		pModule->SetWaferMax(pForm->m_nWaferMax);
		pModule->SetProcessTime(pForm->m_nProcessTime);
		pModule->SetCleanTime(pForm->m_nCleanTime);
		pModule->SetCleanCount(pForm->m_nCleanCount);
		pModule->SetSlotValveOpenTime(pForm->m_nSlotOpen);
		pModule->SetSlotValveCloseTime(pForm->m_nSlotClose);

		m_pMainDlg->m_ctrlListFabInfo.SetItemText(pModule->m_nRow, pModule->m_nCol, pModule->GetModuleName());
		
		return;
	}
	default:
		return;
	}
}

void CFabController::DeleteModule(CFabInfoListCtrl* pCtrl, int nModuleIdx)
{
	delete m_pModule[nModuleIdx];

	m_pModule.erase(m_pModule.begin() + nModuleIdx);
	pCtrl->SetItemText(pCtrl->m_nCurRow, pCtrl->m_nCurCol, _T(""));

	// 추가 삭제 필요 (모니터링 스레드에서 사용되는? >> 협의 필요)
}

// 모듈 전체 삭제
void CFabController::ClearAllModule()
{
	for (int i = 0; i < m_pModule.size(); i++)
	{
		//m_pModule[i]->m_th
		delete m_pModule[i];
	}

	s_pLPM.clear();
	s_pATMRobot.clear();
	s_pLL.clear();
	s_pVACRobot.clear();
	s_pPM.clear();

	m_vPickModules.clear();
	m_vPlaceModules.clear();

	s_hMoniteringThread = NULL;
	s_hMoniteringThread2 = NULL;

	s_bAllWorkOver = FALSE;
	s_hMoniteringThread = NULL;

	m_pModule.clear();

	DrawModule();

	m_pMainDlg->SetWindowText(_T("주성 Fab Simulator"));

	// 추가 삭제 필요 (모니터링 스레드에서 사용되는? >> 협의 필요)
}

// Info에 모듈 정보 출력
void CFabController::PrintModuleInfo(int nModuleIdx, int nModuleType, int nCurSel)
{
	CString strModuleType;
	CString strWaferMax;

	switch ((ModuleType)nModuleType)
	{
	case TYPE_LPM:
	{
		if (nCurSel != 0) // MainDlg의 InfoTab의 Index가 0일 시 (Informations가 골라져 있을 시)
		{
			m_pMainDlg->m_pFormInfo->ShowWindow(SW_SHOW);
			m_pMainDlg->m_pFormTimeInfoATM->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoLL->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoVAC->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoPM->ShowWindow(SW_HIDE);
			
			m_pMainDlg->m_ctrlInfoTab.SetCurSel(0);
		}
		
		strModuleType.Format(_T("TYPE_LPM"));
		strWaferMax.Format(_T("%d"), m_pModule[nModuleIdx]->GetWaferMax());

		m_pMainDlg->m_pFormInfo->m_strModuleType = strModuleType;
		m_pMainDlg->m_pFormInfo->m_strModuleName = m_pModule[nModuleIdx]->GetModuleName();
		m_pMainDlg->m_pFormInfo->m_strWaferMax = strWaferMax;
		m_pMainDlg->m_pFormInfo->UpdateData(0);

		break;
	}

	case TYPE_ATMROBOT:
	{
		ATMRobot* pModule = (ATMRobot*)m_pModule[nModuleIdx];

		if (nCurSel == 0) // MainDlg의 InfoTab의 Index가 0일 시 (Informations가 골라져 있을 시)
		{
			strModuleType.Format(_T("TYPE_ATMROBOT"));
			strWaferMax.Format(_T("2 (Dual Arm)"));

			m_pMainDlg->m_pFormInfo->ShowWindow(SW_SHOW);
			m_pMainDlg->m_pFormTimeInfoATM->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoLL->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoVAC->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoPM->ShowWindow(SW_HIDE);

			m_pMainDlg->m_pFormInfo->m_strModuleType = strModuleType;
			m_pMainDlg->m_pFormInfo->m_strModuleName = m_pModule[nModuleIdx]->GetModuleName();
			m_pMainDlg->m_pFormInfo->m_strWaferMax = strWaferMax;
			m_pMainDlg->m_pFormInfo->UpdateData(0);
		}

		else if (nCurSel == 1) // MainDlg의 InfoTab의 Index가 1일 시 (Time Info가 골라져 있을 시)
		{
			CString strTime;

			strTime.Format(_T("%d (sec)"), pModule->GetPickTime());
			m_pMainDlg->m_pFormTimeInfoATM->m_strPickTime = strTime;

			strTime.Format(_T("%d (sec)"), pModule->GetPlaceTime());
			m_pMainDlg->m_pFormTimeInfoATM->m_strPlaceTime = strTime;

			strTime.Format(_T("%d (sec)"), pModule->GetRotateTime());
			m_pMainDlg->m_pFormTimeInfoATM->m_strRotateTime = strTime;

			strTime.Format(_T("%d (sec)"), pModule->GetRotateZTime());
			m_pMainDlg->m_pFormTimeInfoATM->m_strZRotateTime = strTime;

			m_pMainDlg->m_pFormTimeInfoATM->UpdateData(0);
			
			// Form SHOW / HIDE ===================================================
			m_pMainDlg->m_pFormInfo->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoATM->ShowWindow(SW_SHOW);
			m_pMainDlg->m_pFormTimeInfoLL->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoVAC->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoPM->ShowWindow(SW_HIDE);
		}
		
		break;
	}

	case TYPE_LOADLOCK:
	{
		LoadLock* pModule = (LoadLock*)m_pModule[nModuleIdx];

		if (nCurSel == 0) // MainDlg의 InfoTab의 Index가 0일 시 (Informations가 골라져 있을 시)
		{
			strModuleType.Format(_T("TYPE_LOADLOCK"));
			strWaferMax.Format(_T("%d"), m_pModule[nModuleIdx]->GetWaferMax());

			m_pMainDlg->m_pFormInfo->ShowWindow(SW_SHOW);
			m_pMainDlg->m_pFormTimeInfoATM->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoLL->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoVAC->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoPM->ShowWindow(SW_HIDE);

			m_pMainDlg->m_pFormInfo->m_strModuleType = strModuleType;
			m_pMainDlg->m_pFormInfo->m_strModuleName = m_pModule[nModuleIdx]->GetModuleName();
			m_pMainDlg->m_pFormInfo->m_strWaferMax = strWaferMax;
			m_pMainDlg->m_pFormInfo->UpdateData(0);
		}

		else if (nCurSel == 1) // MainDlg의 InfoTab의 Index가 1일 시 (Time Info가 골라져 있을 시)
		{
			CString strTime;

			strTime.Format(_T("%d (sec)"), pModule->GetPumpTime());
			m_pMainDlg->m_pFormTimeInfoLL->m_strPumpTime = strTime;

			strTime.Format(_T("%d (sec)"), pModule->GetPumpStableTime());
			m_pMainDlg->m_pFormTimeInfoLL->m_strPumpStableTime = strTime;

			strTime.Format(_T("%d (sec)"), pModule->GetVentTime());
			m_pMainDlg->m_pFormTimeInfoLL->m_strVentTime = strTime;

			strTime.Format(_T("%d (sec)"), pModule->GetVentStableTime());
			m_pMainDlg->m_pFormTimeInfoLL->m_strVentStableTime = strTime;

			strTime.Format(_T("%d (sec)"), pModule->GetDoorValveOpenTime());
			m_pMainDlg->m_pFormTimeInfoLL->m_strDoorOpenTime = strTime;

			strTime.Format(_T("%d (sec)"), pModule->GetDoorValveCloseTime());
			m_pMainDlg->m_pFormTimeInfoLL->m_strDoorCloseTime = strTime;

			strTime.Format(_T("%d (sec)"), pModule->GetSlotValveOpenTime());
			m_pMainDlg->m_pFormTimeInfoLL->m_strSlotOpenTime = strTime;

			strTime.Format(_T("%d (sec)"), pModule->GetSlotValveCloseTime());
			m_pMainDlg->m_pFormTimeInfoLL->m_strSlotCloseTime = strTime;

			m_pMainDlg->m_pFormTimeInfoLL->UpdateData(0);


			// Form SHOW / HIDE ===================================================
			m_pMainDlg->m_pFormInfo->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoATM->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoLL->ShowWindow(SW_SHOW);
			m_pMainDlg->m_pFormTimeInfoVAC->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoPM->ShowWindow(SW_HIDE);
		}

		break;
	}

	case TYPE_VACROBOT:
	{
		VACRobot* pModule = (VACRobot*)m_pModule[nModuleIdx];

		if (nCurSel == 0) // MainDlg의 InfoTab의 Index가 0일 시 (Informations가 골라져 있을 시)
		{
			strModuleType.Format(_T("TYPE_VACROBOT"));

			if ((m_pModule[nModuleIdx])->GetWaferMax() == 4)
				strWaferMax.Format(_T("4 (Quad Arm)"));
			else
				strWaferMax.Format(_T("2 (Dual Arm)"));
			
			m_pMainDlg->m_pFormInfo->ShowWindow(SW_SHOW);
			m_pMainDlg->m_pFormTimeInfoATM->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoLL->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoVAC->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoPM->ShowWindow(SW_HIDE);

			m_pMainDlg->m_pFormInfo->m_strModuleType = strModuleType;
			m_pMainDlg->m_pFormInfo->m_strModuleName = m_pModule[nModuleIdx]->GetModuleName();
			m_pMainDlg->m_pFormInfo->m_strWaferMax = strWaferMax;
			m_pMainDlg->m_pFormInfo->UpdateData(0);
		}

		else if (nCurSel == 1) // MainDlg의 InfoTab의 Index가 1일 시 (Time Info가 골라져 있을 시)
		{
			CString strTime;

			strTime.Format(_T("%d (sec)"), pModule->GetPickTime());
			m_pMainDlg->m_pFormTimeInfoVAC->m_strPickTime = strTime;

			strTime.Format(_T("%d (sec)"), pModule->GetPlaceTime());
			m_pMainDlg->m_pFormTimeInfoVAC->m_strPlaceTime = strTime;

			strTime.Format(_T("%d (sec)"), pModule->GetRotateTime());
			m_pMainDlg->m_pFormTimeInfoVAC->m_strRotateTime = strTime;

			m_pMainDlg->m_pFormTimeInfoVAC->UpdateData(0);
			
			// Form SHOW / HIDE ===================================================
			m_pMainDlg->m_pFormInfo->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoATM->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoLL->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoVAC->ShowWindow(SW_SHOW);
			m_pMainDlg->m_pFormTimeInfoPM->ShowWindow(SW_HIDE);
		}

		break;
	}

	case TYPE_PROCESSCHAMBER:
	{
		ProcessChamber* pModule = (ProcessChamber*)m_pModule[nModuleIdx];
		
		if (nCurSel == 0) // MainDlg의 InfoTab의 Index가 0일 시 (Informations가 골라져 있을 시)
		{
			strModuleType.Format(_T("TYPE_PROCESSCHAMBER"));
			strWaferMax.Format(_T("%d"), pModule->GetWaferMax());

			m_pMainDlg->m_pFormInfo->ShowWindow(SW_SHOW);
			m_pMainDlg->m_pFormTimeInfoATM->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoLL->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoVAC->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoPM->ShowWindow(SW_HIDE);

			m_pMainDlg->m_pFormInfo->m_strModuleType = strModuleType;
			m_pMainDlg->m_pFormInfo->m_strModuleName = pModule->GetModuleName();
			m_pMainDlg->m_pFormInfo->m_strWaferMax = strWaferMax;
			m_pMainDlg->m_pFormInfo->UpdateData(0);
		}

		else if (nCurSel == 1)
		{
			CString strTime;

			strTime.Format(_T("%d (sec)"), pModule->GetProcessTime());
			m_pMainDlg->m_pFormTimeInfoPM->m_strProcessTime = strTime;

			strTime.Format(_T("%d (cnt)"), pModule->GetProcessCount());
			m_pMainDlg->m_pFormTimeInfoPM->m_strProcessCount = strTime;

			strTime.Format(_T("%d (sec)"), pModule->GetCleanTime());
			m_pMainDlg->m_pFormTimeInfoPM->m_strCleanTime = strTime;

			strTime.Format(_T("%d (cnt)"), pModule->GetCleanCount());
			m_pMainDlg->m_pFormTimeInfoPM->m_strCleanCount = strTime;

			strTime.Format(_T("%d (sec)"), pModule->GetSlotValveOpenTime());
			m_pMainDlg->m_pFormTimeInfoPM->m_strSlotOpenTime = strTime;

			strTime.Format(_T("%d (sec)"), pModule->GetSlotValveCloseTime());
			m_pMainDlg->m_pFormTimeInfoPM->m_strSlotCloseTime = strTime;

			m_pMainDlg->m_pFormTimeInfoPM->UpdateData(0);

			// Form SHOW / HIDE ===================================================
			m_pMainDlg->m_pFormInfo->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoATM->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoLL->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoVAC->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoPM->ShowWindow(SW_SHOW);
		}

		break;
	}

	default:
	{
		m_pMainDlg->m_pFormInfo->ShowWindow(SW_SHOW);
		m_pMainDlg->m_pFormTimeInfoATM->ShowWindow(SW_HIDE);
		m_pMainDlg->m_pFormTimeInfoLL->ShowWindow(SW_HIDE);
		m_pMainDlg->m_pFormTimeInfoVAC->ShowWindow(SW_HIDE);
		m_pMainDlg->m_pFormTimeInfoPM->ShowWindow(SW_HIDE);

		m_pMainDlg->m_ctrlInfoTab.SetCurSel(0);
	}
	}
}

// ConfigFile 저장
void CFabController::SaveConfigFile(CString strFilePath)
{
	//INI 파일에 정보 쓰기
	int nModuleSize = (int)m_pModule.size();
	CString strSize;
	strSize.Format(_T("%d\n"), nModuleSize);
	
	WritePrivateProfileString(_T("Common"),//섹션명 
		_T("ModuleCount"), //키명
		strSize, //키 값
		strFilePath);

	CString strIdx, strModuleName, strRow, strCol, strWaferMax;
	for (int i = 0; i < nModuleSize; i++)
	{
		// 공통 데이터 CString으로 변환
		strIdx.Format(_T("%d"), i);
		strModuleName.Format(m_pModule[i]->GetModuleName());
		strRow.Format(_T("%d"), m_pModule[i]->m_nRow);
		strCol.Format(_T("%d"), m_pModule[i]->m_nCol);
		strWaferMax.Format(_T("%d"), m_pModule[i]->GetWaferMax());
		
		switch ((ModuleType)(m_pModule[i]->m_eModuleType))
		{
		case TYPE_LPM:
		{
			WritePrivateProfileString(strIdx, _T("ModuleType"), _T("TYPE_LPM"), strFilePath);				// 타입
			WritePrivateProfileString(strIdx, _T("ModuleName"), strModuleName, strFilePath);				// 모듈명
			WritePrivateProfileString(strIdx, _T("PosX"), strCol, strFilePath);								// 컬럼
			WritePrivateProfileString(strIdx, _T("PosY"), strRow, strFilePath);								// 로우
			strWaferMax.Format(_T("%d\n"), m_pModule[i]->GetWaferMax());									// 한줄 추가
			WritePrivateProfileString(strIdx, _T("WaferMax"), strWaferMax, strFilePath);					// WaferMax
			
			break;
		}

		case TYPE_ATMROBOT:
		{
			ATMRobot* pModule = (ATMRobot*)m_pModule[i];

			CString strPickTime, strPlaceTime, strRotateTime, strZRotatetime, strArmMode;
			strPickTime.Format(_T("%d"), pModule->GetPickTime());
			strPlaceTime.Format(_T("%d"), pModule->GetPlaceTime());
			strRotateTime.Format(_T("%d"), pModule->GetRotateTime());
			strZRotatetime.Format(_T("%d\n"), pModule->GetRotateZTime());
			strArmMode.Format(_T("DualArm"));		// << 추후 개선할 수 있으면 개선 (Robot Arm 표현)
			
			WritePrivateProfileString(strIdx, _T("ModuleType"), _T("TYPE_ATMROBOT"), strFilePath);			// 타입
			WritePrivateProfileString(strIdx, _T("ModuleName"),	strModuleName, strFilePath);				// 모듈명
			WritePrivateProfileString(strIdx, _T("PosX"), strCol, strFilePath);								// 컬럼
			WritePrivateProfileString(strIdx, _T("PosY"), strRow, strFilePath);								// 로우
			WritePrivateProfileString(strIdx, _T("ArmMode"), strArmMode, strFilePath);						// ArmMode(WaferMax)
			WritePrivateProfileString(strIdx, _T("PickTime"), strPickTime, strFilePath);					// PickTime
			WritePrivateProfileString(strIdx, _T("PlaceTime"), strPlaceTime, strFilePath);					// PlaceTime
			WritePrivateProfileString(strIdx, _T("StationMoveTime"), strRotateTime, strFilePath);			// RotateTime
			WritePrivateProfileString(strIdx, _T("Z-MoveTime"), strZRotatetime, strFilePath);				// ZRotateTime

			break;
		}

		case TYPE_LOADLOCK:
		{
			LoadLock* pModule = (LoadLock*)m_pModule[i];
			
			CString strPumpTime, strPumpStableTime, strVentTime, strVentStableTime, strSlotOpenTime, strSlotCloseTime, strDoorOpenTime, strDoorCloseTime;
			
			strPumpTime.Format(_T("%d"), pModule->GetPumpTime());
			strPumpStableTime.Format(_T("%d"), pModule->GetPumpStableTime());
			strVentTime.Format(_T("%d"), pModule->GetVentTime());
			strVentStableTime.Format(_T("%d"), pModule->GetVentStableTime());
			strSlotOpenTime.Format(_T("%d"), pModule->GetSlotValveOpenTime());
			strSlotCloseTime.Format(_T("%d"), pModule->GetSlotValveCloseTime());
			strDoorOpenTime.Format(_T("%d"), pModule->GetDoorValveOpenTime());
			strDoorCloseTime.Format(_T("%d\n"), pModule->GetDoorValveCloseTime());

			WritePrivateProfileString(strIdx, _T("ModuleType"), _T("TYPE_LOADLOCK"), strFilePath);			// 타입
			WritePrivateProfileString(strIdx, _T("ModuleName"), strModuleName, strFilePath);				// 모듈명
			WritePrivateProfileString(strIdx, _T("PosX"), strCol, strFilePath);								// 컬럼
			WritePrivateProfileString(strIdx, _T("PosY"), strRow, strFilePath);								// 로우
			WritePrivateProfileString(strIdx, _T("WaferMax"), strWaferMax, strFilePath);					// WaferMax
			WritePrivateProfileString(strIdx, _T("PumpTime"), strPumpTime, strFilePath);					// PumpTime
			WritePrivateProfileString(strIdx, _T("PumpStableTime"), strPumpStableTime, strFilePath);		// PumpStableTime
			WritePrivateProfileString(strIdx, _T("VentTime"), strVentTime, strFilePath);					// VentTime
			WritePrivateProfileString(strIdx, _T("VentStableTime"), strVentStableTime, strFilePath);		// VentStableTime
			WritePrivateProfileString(strIdx, _T("SlotValveOpenTime"), strSlotOpenTime, strFilePath);		// SlotOpenTime
			WritePrivateProfileString(strIdx, _T("SlotValveCloseTime"), strSlotCloseTime, strFilePath);		// SlotCloseTime
			WritePrivateProfileString(strIdx, _T("DoorValveOpenTime"), strDoorOpenTime, strFilePath);		// DoorOpenTime
			WritePrivateProfileString(strIdx, _T("DoorValveCloseTime"), strDoorCloseTime, strFilePath);		// DoorCloseTime

			break;
		}

		case TYPE_VACROBOT:
		{
			VACRobot* pModule = (VACRobot*)m_pModule[i];

			CString strPickTime, strPlaceTime, strRotateTime, strZRotatetime, strArmMode;
			strArmMode.Format(pModule->GetWaferMax() == 4 ? _T("QuadArm") : _T("DualArm"));					// WaferMax == 4 일 시 QuadArm | 아닐 시 DualArm
			strPickTime.Format(_T("%d"), pModule->GetPickTime());
			strPlaceTime.Format(_T("%d"), pModule->GetPlaceTime());
			strRotateTime.Format(_T("%d\n"), pModule->GetRotateTime());
			

			WritePrivateProfileString(strIdx, _T("ModuleType"), _T("TYPE_VACROBOT"), strFilePath);			// 타입
			WritePrivateProfileString(strIdx, _T("ModuleName"), strModuleName, strFilePath);				// 모듈명
			WritePrivateProfileString(strIdx, _T("PosX"), strCol, strFilePath);								// 컬럼
			WritePrivateProfileString(strIdx, _T("PosY"), strRow, strFilePath);								// 로우
			WritePrivateProfileString(strIdx, _T("ArmMode"), strArmMode, strFilePath);						// ArmMode(WaferMax)
			WritePrivateProfileString(strIdx, _T("PickTime"), strPickTime, strFilePath);					// PickTime
			WritePrivateProfileString(strIdx, _T("PlaceTime"), strPlaceTime, strFilePath);					// PlaceTime
			WritePrivateProfileString(strIdx, _T("RotateTime"), strRotateTime, strFilePath);					// RotateTime

			break;
		}

		case TYPE_PROCESSCHAMBER:
		{
			ProcessChamber* pModule = (ProcessChamber*)m_pModule[i];

			CString strProcessTime, strCleanTime, strCleanCount, strSlotOpenTime, strSlotCloseTime;

			strProcessTime.Format(_T("%d"), pModule->GetProcessTime());
			strCleanTime.Format(_T("%d"), pModule->GetCleanTime());
			strCleanCount.Format(_T("%d"), pModule->GetCleanCount());
			strSlotOpenTime.Format(_T("%d"), pModule->GetSlotValveOpenTime());
			strSlotCloseTime.Format(_T("%d\n"), pModule->GetSlotValveCloseTime());

			WritePrivateProfileString(strIdx, _T("ModuleType"), _T("TYPE_PROCESSCHAMBER"), strFilePath);	// 타입
			WritePrivateProfileString(strIdx, _T("ModuleName"), strModuleName, strFilePath);				// 모듈명
			WritePrivateProfileString(strIdx, _T("PosX"), strCol, strFilePath);								// 컬럼
			WritePrivateProfileString(strIdx, _T("PosY"), strRow, strFilePath);								// 로우
			WritePrivateProfileString(strIdx, _T("WaferMax"), strWaferMax, strFilePath);					// WaferMax
			WritePrivateProfileString(strIdx, _T("ProcessTime"), strProcessTime, strFilePath);				// ProcessTime
			WritePrivateProfileString(strIdx, _T("CleanTime"), strCleanTime, strFilePath);					// CleanTime
			WritePrivateProfileString(strIdx, _T("CleanCount"), strCleanCount, strFilePath);				// CleanCount
			WritePrivateProfileString(strIdx, _T("SlotValveOpenTime"), strSlotOpenTime, strFilePath);		// SlotOpenTime
			WritePrivateProfileString(strIdx, _T("SlotValveCloseTime"), strSlotCloseTime, strFilePath);		// SlotCloseTime

			break;
		}
		default:
		{
			break;
		}
		}

	}

}

// ConfigFile 불러오기
void CFabController::LoadConfigFile(CString strFilePath)
{
	// Common 섹션 먼저 읽기 (ModuleCount)
	int nModuleSize = GetPrivateProfileInt(_T("Common"), _T("ModuleCount"), -1, strFilePath);
	
	for (int i = 0; i < nModuleSize; i++)
	{
		TCHAR szCount[100] = { NULL };
		TCHAR szName[100] = { NULL };
		CString strIdx, strModuleType, strModuleName;
		strIdx.Format(_T("%d"), i);
		GetPrivateProfileString(strIdx, _T("ModuleType"), NULL, szCount, sizeof(szCount), strFilePath);

		strModuleType.Format(_T("%s"), szCount);
		
		GetPrivateProfileString(strIdx, _T("ModuleName"), NULL, szName, sizeof(szName), strFilePath);
		strModuleName.Format(_T("%s"), szName);

		int nRow = GetPrivateProfileInt(strIdx, _T("PosY"), -1, strFilePath);
		int nCol = GetPrivateProfileInt(strIdx, _T("PosX"), -1, strFilePath);

		if (strModuleType == _T("TYPE_LPM"))
		{
			ModuleType eType = TYPE_LPM;
			int nWaferMax = GetPrivateProfileInt(strIdx, _T("WaferMax"), -1, strFilePath);

			m_pModule.push_back(new LPM(eType, strModuleName, nWaferMax, nWaferMax, nRow, nCol));
		}
		else if (strModuleType == _T("TYPE_ATMROBOT"))
		{
			ModuleType eType = TYPE_ATMROBOT;

			TCHAR szArmMode[100] = { NULL };
			GetPrivateProfileString(strIdx, _T("ArmMode"), NULL, szArmMode, sizeof(szArmMode), strFilePath);

			CString strArmMode;
			strArmMode.Format(_T("%s"), szArmMode);
			int nWaferMax = strArmMode == _T("DualArm") ? 2 : 4;

			int nPickTime = GetPrivateProfileInt(strIdx, _T("PickTime"), -1, strFilePath);
			int nPlaceTime = GetPrivateProfileInt(strIdx, _T("PlaceTime"), -1, strFilePath);
			int nRotateTime = GetPrivateProfileInt(strIdx, _T("StationMoveTime"), -1, strFilePath);
			int nZRotateTime = GetPrivateProfileInt(strIdx, _T("Z-MoveTime"), -1, strFilePath);
			
			m_pModule.push_back(new ATMRobot(eType, strModuleName, 0, nWaferMax, nRow, nCol, nPickTime, nPlaceTime, nRotateTime, nZRotateTime));
		}
		else if (strModuleType == _T("TYPE_LOADLOCK"))
		{
			ModuleType eType = TYPE_LOADLOCK;
			int nWaferMax = GetPrivateProfileInt(strIdx, _T("WaferMax"), -1, strFilePath);
			int nPumpTime = GetPrivateProfileInt(strIdx, _T("PumpTime"), -1, strFilePath);
			int nPumpStableTime = GetPrivateProfileInt(strIdx, _T("PumpStableTime"), -1, strFilePath);
			int nVentTime = GetPrivateProfileInt(strIdx, _T("VentTime"), -1, strFilePath);
			int nVentStableTime = GetPrivateProfileInt(strIdx, _T("VentStableTime"), -1, strFilePath);
			int nSlotOpenTime = GetPrivateProfileInt(strIdx, _T("SlotValveOpenTime"), -1, strFilePath);
			int nSlotCloseTime = GetPrivateProfileInt(strIdx, _T("SlotValveCloseTime"), -1, strFilePath);
			int nDoorOpenTime = GetPrivateProfileInt(strIdx, _T("DoorValveOpenTime"), -1, strFilePath);
			int nDoorCloseTime = GetPrivateProfileInt(strIdx, _T("DoorValveCloseTime"), -1, strFilePath);

			m_pModule.push_back(new LoadLock(eType, strModuleName, 0, nWaferMax, nRow, nCol, 
				nPumpTime, nPumpStableTime, nVentTime, nVentStableTime, nSlotOpenTime, nSlotCloseTime, nDoorOpenTime, nDoorCloseTime));
		}
		else if (strModuleType == _T("TYPE_VACROBOT"))
		{
			ModuleType eType = TYPE_VACROBOT;

			TCHAR szArmMode[100] = { NULL };
			GetPrivateProfileString(strIdx, _T("ArmMode"), NULL, szArmMode, sizeof(szArmMode), strFilePath);
			
			CString strArmMode;
			strArmMode.Format(_T("%s"), szArmMode);
			int nWaferMax = strArmMode == _T("DualArm") ? 2 : 4;

			int nPickTime = GetPrivateProfileInt(strIdx, _T("PickTime"), -1, strFilePath);
			int nPlaceTime = GetPrivateProfileInt(strIdx, _T("PlaceTime"), -1, strFilePath);
			int nRotateTime = GetPrivateProfileInt(strIdx, _T("RotateTime"), -1, strFilePath);

			m_pModule.push_back(new VACRobot(eType, strModuleName, 0, nWaferMax, nRow, nCol, nPickTime, nPlaceTime, nRotateTime));
		}
		else if (strModuleType == _T("TYPE_PROCESSCHAMBER"))
		{
			ModuleType eType = TYPE_PROCESSCHAMBER;
			int nWaferMax = GetPrivateProfileInt(strIdx, _T("WaferMax"), -1, strFilePath);
			int nProcessTime = GetPrivateProfileInt(strIdx, _T("ProcessTime"), -1, strFilePath);
			int nCleanTime = GetPrivateProfileInt(strIdx, _T("CleanTime"), -1, strFilePath);
			int nCleanCount = GetPrivateProfileInt(strIdx, _T("CleanCount"), -1, strFilePath);
			int nSlotOpenTime = GetPrivateProfileInt(strIdx, _T("SlotValveOpenTime"), -1, strFilePath);
			int nSlotCloseTime = GetPrivateProfileInt(strIdx, _T("SlotValveCloseTime"), -1, strFilePath);
			
			m_pModule.push_back(new ProcessChamber(eType, strModuleName, 0, nWaferMax, nRow, nCol, nProcessTime, nCleanTime, nSlotOpenTime, nSlotCloseTime, nCleanCount));
		}

	}
	
	DrawModule();
}


// ========================================================================================

//////////////////////////////////////////////////////////
//중앙감시 thread : 동작과정을 모니터링하고 제어해주는 thread
//////////////////////////////////////////////////////////
DWORD WINAPI MoniteringThread1(LPVOID p)
{
	int const nMaxPMSlot = CFabController::s_pPM.size() * CFabController::s_pPM[0]->GetWaferMax();
	int const nMaxLLSlot = CFabController::s_pLL.size() * CFabController::s_pLL[0]->GetWaferMax();
	//목적. 모듈의 진행방향을 상황에 맞추어 바꾸어 줌
	while (1)
	{
		int nCntTotalPmWafer = 0;
		int nCntTotalLLWafer = 0;
		CFabController::s_bAllWorkOver = true;

		bool bModuleEmpty = true;

		for (int i = 0; i < CFabController::GetInstance().m_pModule.size(); i++)
		{
			if (CFabController::GetInstance().m_pModule[i]->m_eModuleType != TYPE_LPM &&
				CFabController::GetInstance().m_pModule[i]->GetWaferCount() != 0)
				bModuleEmpty = false;

			if (i < CFabController::s_pPM.size())
				nCntTotalPmWafer += CFabController::s_pPM[i]->GetWaferCount();

			if (i < CFabController::s_pLL.size())
				nCntTotalLLWafer += CFabController::s_pLL[i]->GetWaferCount();
		}

		//모듈들의 진행방향을 바꾸어 줌
		//if (ModuleBase::s_bDirect == false &&
		//	nCntTotalLLWafer == nMaxPMSlot &&
		//	nCntTotalPmWafer == nMaxPMSlot &&
		//	LoadLock::s_nTotalSendWaferFromLL == 0)
		//{
		//	//ModuleBase::s_bDirect = true;
		//}

		if (ModuleBase::s_bDirect == true &&
			LPM::s_nTotalOutputWafer > 0 && LPM::s_nTotalOutputWafer % nMaxPMSlot == 0)
		{
			ModuleBase::s_bDirect = false;

			for (int i = 0; i < CFabController::s_pLL.size(); i++)
			{
				LoadLock* pLL = (LoadLock*)CFabController::s_pLL[i];
				SetEvent(pLL->m_hLLWaferCntChangeEvent);
			}


		}
	}

	return 0;
}

DWORD WINAPI MoniteringThread2(LPVOID p)
{
	int const nMaxPMSlot = CFabController::s_pPM.size() * CFabController::s_pPM[0]->GetWaferMax();
	int const nMaxLLSlot = CFabController::s_pLL.size() * CFabController::s_pLL[0]->GetWaferMax();

	while (1)
	{
		WaitForSingleObject(ATMRobot::s_hEventBlockATMRobot, INFINITE);
		if (LPM::s_nTotalSendWafer == nMaxPMSlot + nMaxLLSlot ||
			(LPM::s_nTotalSendWafer > nMaxPMSlot + nMaxLLSlot && (LPM::s_nTotalSendWafer - (nMaxPMSlot + nMaxLLSlot)) % nMaxLLSlot == 0))
		{
			LPM::s_bLPMWaferPickBlock = true;
			ResetEvent(ATMRobot::s_hEventBlockATMRobot);

			while (1)
			{
				WaitForSingleObject(ATMRobot::s_hEventBlockATMRobot, INFINITE);
				if (LPM::s_nTotalOutputWafer > 0 && LPM::s_nTotalOutputWafer % nMaxPMSlot == 0)
				{
					LPM::s_bLPMWaferPickBlock = false;
					ResetEvent(ATMRobot::s_hEventBlockATMRobot);
					break;
				}
			}
		}

	}

	return 0;
}

void CFabController::RunModules()
{
	//최초로 동작하는 경우 시작
	if (m_pModule[1]->IsRunning() == false)		// 사용자가 LPM을 제외한 다른 모듈을 먼저 만들고 실행 가능?
	{
		CListCtrl* pListCtrl = (&(m_pMainDlg->m_ctrlListFabInfo));
		
		for (int i = 0; i < m_pModule.size(); i++)		// 모듈 타입별 구분
		{
			ModuleBase* pM = m_pModule[i];

			if (pM->m_eModuleType == TYPE_LPM)
				s_pLPM.push_back(pM);
			else if (pM->m_eModuleType == TYPE_ATMROBOT)
				s_pATMRobot.push_back(pM);
			else if (pM->m_eModuleType == TYPE_LOADLOCK)
				s_pLL.push_back(pM);
			else if (pM->m_eModuleType == TYPE_VACROBOT)
				s_pVACRobot.push_back(pM);
			else if (pM->m_eModuleType == TYPE_PROCESSCHAMBER)
				s_pPM.push_back(pM);
		}

		// LL 총 WaferMax < PM 총 WaferMax 제한
		if (s_pLL.size() * s_pLL[0]->GetWaferMax() > s_pPM.size() * s_pPM[0]->GetWaferMax())
		{
			AfxMessageBox(_T("LL들의 총 wafer 수가 PM들의 총 Wafer 수보다 많을 수 없습니다.\n"));
		}

		else
		{
			//중앙감시 thread 생성
			s_hMoniteringThread = CreateThread(NULL, NULL, MoniteringThread1, NULL, NULL, NULL);
			s_hMoniteringThread2 = CreateThread(NULL, NULL, MoniteringThread2, NULL, NULL, NULL);
			
			//Process가 이미 진행중이 아닐 때 로직
			for (int i = 0; i < m_pModule.size(); i++)
			{
				//Robot일 경우 동작 로직(TM, ATM) : Robot 앞뒤로 input, 설정해줌
				if (m_pModule[i]->m_eModuleType == TYPE_ATMROBOT || m_pModule[i]->m_eModuleType == TYPE_VACROBOT)
				{
					for (int j = 0; j < m_pModule.size(); j++)
					{
						{
							if (m_pModule[i]->m_nCol - 1 == m_pModule[j]->m_nCol) 
								m_vPickModules.push_back(m_pModule[j]);

							else if (m_pModule[i]->m_nCol + 1 == m_pModule[j]->m_nCol)
								m_vPlaceModules.push_back(m_pModule[j]);
						}
					}

					if (m_pModule[i]->m_eModuleType == TYPE_ATMROBOT)
					{
						ATMRobot* p = (ATMRobot*)m_pModule[i];
						p->Run(m_vPickModules, m_vPlaceModules, pListCtrl);
					}

					else if (m_pModule[i]->m_eModuleType == TYPE_VACROBOT)
					{
						VACRobot* p = (VACRobot*)m_pModule[i];
						p->Run(m_vPickModules, m_vPlaceModules, pListCtrl);
					}

					m_vPickModules.clear();
					m_vPlaceModules.clear();
				}

				//LPM, LL, PM일 경우 동작 로직
				else if (m_pModule[i]->m_eModuleType == TYPE_LPM)
				{
					LPM* p;
					p = (LPM*)m_pModule[i];
					p->Run();
				}

				else if (m_pModule[i]->m_eModuleType == TYPE_LOADLOCK)
				{
					LoadLock* p;
					p = (LoadLock*)m_pModule[i];
					p->Run();
				}


				else if (m_pModule[i]->m_eModuleType == TYPE_PROCESSCHAMBER)
				{
					ProcessChamber* p;
					p = (ProcessChamber*)m_pModule[i];
					p->Run();
				}
			}
		}
	}

	//이미 모듈들이 동작하고 있는 경우 일시정지
	else
	{
		for (int i = 0; i < m_pModule.size(); i++)
		{
			m_pModule[i]->Resume();
			ResumeThread(s_hMoniteringThread);
		}
	}
}

void CFabController::SuspendModules()
{
	for (int i = 0; i < m_pModule.size(); i++)
	{
		m_pModule[i]->Suspend();
		SuspendThread(s_hMoniteringThread);
	}
}

#pragma endregion