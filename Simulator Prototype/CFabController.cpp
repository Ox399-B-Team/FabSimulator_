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
			//	m_pMainDlg->m_pFormTimeInfoLL->ShowWindow(SW_HIDE);
			//	m_pMainDlg->m_pFormTimeInfoVAC->ShowWindow(SW_HIDE);
			//	m_pMainDlg->m_pFormTimeInfoPM->ShowWindow(SW_HIDE);
			
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
			strWaferMax.Format(_T("2 (DualArm)"));

			m_pMainDlg->m_pFormInfo->ShowWindow(SW_SHOW);
			m_pMainDlg->m_pFormTimeInfoATM->ShowWindow(SW_HIDE);
			//	m_pMainDlg->m_pFormTimeInfoLL->ShowWindow(SW_HIDE);
			//	m_pMainDlg->m_pFormTimeInfoVAC->ShowWindow(SW_HIDE);
			//	m_pMainDlg->m_pFormTimeInfoPM->ShowWindow(SW_HIDE);

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
			//	m_pMainDlg->m_pFormTimeInfoLL->ShowWindow(SW_HIDE);
			//	m_pMainDlg->m_pFormTimeInfoVAC->ShowWindow(SW_HIDE);
			//	m_pMainDlg->m_pFormTimeInfoPM->ShowWindow(SW_HIDE);
		}
		
		break;
	}

	case TYPE_LOADLOCK:
	{
		strModuleType.Format(_T("TYPE_LOADLOCK"));
		strWaferMax.Format(_T("%d"), m_pModule[nModuleIdx]->GetWaferMax());
		break;
	}

	case TYPE_VACROBOT:
	{
		strModuleType.Format(_T("TYPE_VACROBOT"));

		if ((m_pModule[nModuleIdx])->GetWaferMax() == 4)
			strWaferMax.Format(_T("4 (Quad Arm)"));
		else
			strWaferMax.Format(_T("2 (Dual Arm)"));

		break;
	}

	case TYPE_PROCESSCHAMBER:
	{
		strModuleType.Format(_T("TYPE_PROCESSCHAMBER"));
		strWaferMax.Format(_T("%d"), m_pModule[nModuleIdx]->GetWaferMax());
		break;
	}

	default:
	{
		m_pMainDlg->m_pFormInfo->ShowWindow(SW_SHOW);
		m_pMainDlg->m_pFormTimeInfoATM->ShowWindow(SW_HIDE);
		//	m_pMainDlg->m_pFormTimeInfoLL->ShowWindow(SW_HIDE);
		//	m_pMainDlg->m_pFormTimeInfoVAC->ShowWindow(SW_HIDE);
		//	m_pMainDlg->m_pFormTimeInfoPM->ShowWindow(SW_HIDE);

		m_pMainDlg->m_ctrlInfoTab.SetCurSel(0);
	}
	}



}







// ========================================================================================

//////////////////////////////////////////////////////////
//중앙감시 thread : 동작과정을 모니터링하고 제어해주는 thread
//////////////////////////////////////////////////////////
DWORD WINAPI MoniteringThread1(LPVOID p)
{
	int const nMaxPM = CFabController::s_pPM.size() * CFabController::s_pPM[0]->GetWaferMax();
	//<<<중앙감시 thread의 역할>>>///
	//1. 모듈의 진행방향을 상황에 맞추어 바꾸어 줌
	while (1)
	{
		int nCntTotalPmWafer = 0;
		//bool bPmIsEmpty = true;
		CFabController::s_bAllWorkOver = true;

		bool bModuleEmpty = true;

		for (int i = 0; i < CFabController::GetInstance().m_pModule.size(); i++)
		{
			if (CFabController::GetInstance().m_pModule[i]->m_eModuleType != TYPE_LPM &&
				CFabController::GetInstance().m_pModule[i]->GetWaferCount() != 0)
				bModuleEmpty = false;

			if (CFabController::s_pPM.size() > i)
				nCntTotalPmWafer += CFabController::s_pPM[i]->GetWaferCount();

			//PM을 제외한 모든 모듈들이 일을 하고 있지 않은 경우
			if (CFabController::GetInstance().m_pModule[i]->m_eModuleType != TYPE_PROCESSCHAMBER &&
				CFabController::GetInstance().m_pModule[i]->GetIsWorking() == true)
				CFabController::s_bAllWorkOver = false;
		}

		//모듈들의 진행방향을 바꾸어 줌
		//WaitForSingleObject(, INFINITE);
		if (ModuleBase::s_bDirect == false &&
			((nCntTotalPmWafer == nMaxPM && LPM::s_bLPMWaferPickBlock == true) || (LPM::s_nTotalSendWafer == LPM::s_nTotalInitWafer && LPM::s_bLPMWaferPickBlock == false && nCntTotalPmWafer != 0)) &&
			CFabController::s_bAllWorkOver == true)
		{
			ModuleBase::s_bDirect = true;
		}

		else if (ModuleBase::s_bDirect == true &&
			CFabController::s_bAllWorkOver == true &&
			LPM::s_bLPMWaferPickBlock == false &&
			bModuleEmpty == true)
		{
			ModuleBase::s_bDirect = false;
		}
	}

	return 0;
}

DWORD WINAPI MoniteringThread2(LPVOID p)
{
	int const nMaxPM = CFabController::s_pPM.size() * CFabController::s_pPM[0]->GetWaferMax();

	while (1)
	{
		WaitForSingleObject(ATMRobot::s_hEventBlockATMRobot, INFINITE);
		if (LPM::s_nTotalOutputWafer != LPM::s_nTotalSendWafer &&
			LPM::s_nTotalSendWafer > 0 && (LPM::s_nTotalSendWafer % nMaxPM) == 0)
		{
			LPM::s_bLPMWaferPickBlock = true;
			ResetEvent(ATMRobot::s_hEventBlockATMRobot);

			while (1)
			{
				WaitForSingleObject(ATMRobot::s_hEventBlockATMRobot, INFINITE);
				if (//ModuleBase::s_bDirect == true && CFabController::s_bAllWorkOver == true ||
					LPM::s_nTotalOutputWafer == LPM::s_nTotalSendWafer &&
					(LPM::s_nTotalOutputWafer > 0 && LPM::s_nTotalOutputWafer % nMaxPM == 0)
					&& CFabController::s_bAllWorkOver == true)
				{
					LPM::s_bLPMWaferPickBlock = false;
					ResetEvent(ATMRobot::s_hEventBlockATMRobot);
					break;
				}
			}
		}

	}
}

void CFabController::RunModules()
{
	//최초로 동작하는 경우 시작
	if (m_pModule[1]->IsRunning() == false)
	{
		CListCtrl* pListCtrl = (&(m_pMainDlg->m_ctrlListFabInfo));
		
		for (int i = 0; i < m_pModule.size(); i++)
		{
			ModuleBase* pM = m_pModule[i];

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


		if (CFabController::s_pLL.size() * CFabController::s_pLL[0]->GetWaferMax() > CFabController::s_pPM.size() * CFabController::s_pPM[0]->GetWaferMax())
		{
			AfxMessageBox(_T("LL들의 총 wafer 수가 PM들의 총 Wafer 수보다 많을 수 없습니다.\n"));
		}

		else
		{
			//중앙감시 thread 생성
			CFabController::s_hMoniteringThread = CreateThread(NULL, NULL, MoniteringThread1, m_pMainDlg, NULL, NULL);
			CloseHandle(CreateThread(NULL, NULL, MoniteringThread2, m_pMainDlg, NULL, NULL));

			//Process가 이미 진행중이 아닐 때 로직
			for (int i = 0; i < CFabController::GetInstance().m_pModule.size(); i++)
			{
				//Robot일 경우 동작 로직(TM, ATM) : Robot 앞뒤로 input, 설정해줌
				if (CFabController::GetInstance().m_pModule[i]->m_eModuleType == TYPE_ATMROBOT || CFabController::GetInstance().m_pModule[i]->m_eModuleType == TYPE_VACROBOT)
				{
					for (int j = 0; j < CFabController::GetInstance().m_pModule.size(); j++)
					{
						{
							if (CFabController::GetInstance().m_pModule[i]->m_nCol - 1 == CFabController::GetInstance().m_pModule[j]->m_nCol)
								CFabController::GetInstance().m_vPickModules.push_back(CFabController::GetInstance().m_pModule[j]);

							else if (CFabController::GetInstance().m_pModule[i]->m_nCol + 1 == CFabController::GetInstance().m_pModule[j]->m_nCol)
								CFabController::GetInstance().m_vPlaceModules.push_back(CFabController::GetInstance().m_pModule[j]);
						}
					}

					if (CFabController::GetInstance().m_pModule[i]->m_eModuleType == TYPE_ATMROBOT)
					{
						ATMRobot* p = (ATMRobot*)CFabController::GetInstance().m_pModule[i];
						p->Run(CFabController::GetInstance().m_vPickModules, CFabController::GetInstance().m_vPlaceModules, pListCtrl);
					}

					else if (CFabController::GetInstance().m_pModule[i]->m_eModuleType == TYPE_VACROBOT)
					{
						VACRobot* p = (VACRobot*)CFabController::GetInstance().m_pModule[i];
						p->Run(CFabController::GetInstance().m_vPickModules, CFabController::GetInstance().m_vPlaceModules, pListCtrl);
					}

					CFabController::GetInstance().m_vPickModules.clear();
					CFabController::GetInstance().m_vPlaceModules.clear();
				}

				//LPM, LL, PM일 경우 동작 로직
				else if (CFabController::GetInstance().m_pModule[i]->m_eModuleType == TYPE_LPM)
				{
					LPM* p;
					p = (LPM*)CFabController::GetInstance().m_pModule[i];
					p->Run();
				}

				else if (CFabController::GetInstance().m_pModule[i]->m_eModuleType == TYPE_LOADLOCK)
				{
					LoadLock* p;
					p = (LoadLock*)CFabController::GetInstance().m_pModule[i];
					p->Run();
				}


				else if (CFabController::GetInstance().m_pModule[i]->m_eModuleType == TYPE_PROCESSCHAMBER)
				{
					ProcessChamber* p;
					p = (ProcessChamber*)CFabController::GetInstance().m_pModule[i];
					p->Run();
				}
			}
		}
	}

	//이미 모듈들이 동작하고 있는 경우 일시정지
	else
	{
		for (int i = 0; i < CFabController::GetInstance().m_pModule.size(); i++)
		{
			CFabController::GetInstance().m_pModule[i]->Resume();
			ResumeThread(CFabController::GetInstance().s_hMoniteringThread);
		}
	}
}

void CFabController::SuspendModules()
{
	for (int i = 0; i < CFabController::GetInstance().m_pModule.size(); i++)
	{
		CFabController::GetInstance().m_pModule[i]->Suspend();
		SuspendThread(CFabController::GetInstance().s_hMoniteringThread);
	}
}

#pragma endregion