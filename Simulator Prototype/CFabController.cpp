#pragma once

#include "pch.h"
#include "CFabController.h"

vector<ModuleBase*> CFabController::s_pLPM;
vector<ModuleBase*> CFabController::s_pATMRobot;
vector<ModuleBase*> CFabController::s_pLL;
vector<ModuleBase*> CFabController::s_pVACRobot;
vector<ModuleBase*> CFabController::s_pPM;

bool CFabController::s_bAllWorkOver;
vector<HANDLE> CFabController::s_vhMoniteringThreads;

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

void CFabController::PrintModule(CDialogEx* pDlg, int nModuleIdx)
{
	
}


// 
//////////////////////////////////////////////////////////
//중앙감시 thread : 동작과정을 모니터링하고 제어해주는 thread
//////////////////////////////////////////////////////////
DWORD WINAPI MoniteringThread1(LPVOID p)
{
	SetEvent(VACRobot::s_hVACRobotExchangeOver);

	int const nMaxPMSlot = CFabController::s_pPM.size() * CFabController::s_pPM[0]->GetWaferMax();
	int const nMaxLLSlot = CFabController::s_pLL.size() * CFabController::s_pLL[0]->GetWaferMax();
	//목적. 모듈의 진행방향을 상황에 맞추어 바꾸어 줌
	while (1)
	{
		int nCntTotalPmWafer = 0;
		int nCntTotalLLWafer = 0;
		CFabController::s_bAllWorkOver = true;

		//bool bModuleEmpty = true;

		for (int i = 0; i < CFabController::GetInstance().m_pModule.size(); i++)
		{
			/*if (CFabController::GetInstance().m_pModule[i]->m_eModuleType != TYPE_LPM &&
				CFabController::GetInstance().m_pModule[i]->GetWaferCount() != 0)
				bModuleEmpty = false;*/

			if (i < CFabController::s_pPM.size())
				nCntTotalPmWafer += CFabController::s_pPM[i]->GetWaferCount();

			if (i < CFabController::s_pLL.size())
				nCntTotalLLWafer += CFabController::s_pLL[i]->GetWaferCount();

			//PM을 제외한 모든 모듈들이 일을 하고 있지 않은 경우
			if (CFabController::GetInstance().m_pModule[i]->m_eModuleType != TYPE_PROCESSCHAMBER &&
				CFabController::GetInstance().m_pModule[i]->GetIsWorking() == true)
				CFabController::s_bAllWorkOver = false;
		}

		//모듈들의 진행방향을 바꾸어 줌
		//if (ModuleBase::s_bDirect == false &&
		//	nCntTotalLLWafer == nMaxPMSlot &&
		//	nCntTotalPmWafer == nMaxPMSlot &&
		//	LoadLock::s_nTotalSendWaferFromLL == 0)
		//{
		//	//ModuleBase::s_bDirect = true;
		//}
		//WaitForSingleObject(VACRobot::s_hVACRobotExchangeOver, INFINITE);


		if (ModuleBase::s_bDirect == true &&
			ProcessChamber::s_nCntPMWorkOver == CFabController::s_pPM.size() &&
			LPM::s_nTotalOutputWafer > 0 && LPM::s_nTotalOutputWafer % nMaxPMSlot == 0)
		{
			ModuleBase::s_bDirect = false;

			for (int i = 0; i < CFabController::s_pLL.size(); i++)
			{
				LoadLock* pLL = (LoadLock*)CFabController::s_pLL[i];
				SetEvent(pLL->m_hLLWaferCntChangeEvent);
			}

			//Exchange가 끝났다는 조건
		
			for (int i = 0; i < CFabController::GetInstance().m_pModule.size(); i++)
			{
				CFabController::GetInstance().m_pModule[i]->m_bExchangeOver = false;
			}
			//ResetEvent(VACRobot::s_hVACRobotExchangeOver);
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

			while(1)
			{
				WaitForSingleObject(ATMRobot::s_hEventOutputWaferChange, INFINITE);
				if (LPM::s_nTotalOutputWafer > 0 && LPM::s_nTotalOutputWafer % nMaxPMSlot == 0)
				{
					LPM::s_bLPMWaferPickBlock = false;

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
			s_vhMoniteringThreads.push_back(CreateThread(NULL, NULL, MoniteringThread1, NULL, NULL, NULL));
			s_vhMoniteringThreads.push_back(CreateThread(NULL, NULL, MoniteringThread2, NULL, NULL, NULL));

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
		for (int i = 0; i < m_pModule.size(); i++)
		{
			m_pModule[i]->Resume();
		}

		for (int i = 0; i < s_vhMoniteringThreads.size(); i++)
		{
			ResumeThread(s_vhMoniteringThreads[i]);
		}
	}
}

void CFabController::SuspendModules()
{
	for (int i = 0; i < m_pModule.size(); i++)
	{
		m_pModule[i]->Suspend();
	}

	for (int i = 0; i < s_vhMoniteringThreads.size(); i++)
	{
		SuspendThread(s_vhMoniteringThreads[i]);
	}
}

#pragma endregion