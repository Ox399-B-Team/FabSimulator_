#include "pch.h"
#include "EFEM.h"
#include "LoadLock.h"
#include "Simulator PrototypeDlg.h"
#include "CFabController.h"

#pragma region LPM

#pragma region 생성자/소멸자

int LPM::s_nTotalInitWafer = 0;
int LPM::s_nTotalSendWafer = 0;
int LPM::s_nTotalOutputWafer = 0;
bool LPM::s_bLPMWaferPickBlock = false;
int LPM::s_nTotalInputWafer = 0;

LPM::LPM(ModuleType _Type, CString _Name, int _WaferCount, int _WaferMax, int _Row, int _Col)
	: ModuleBase(_Type, _Name, _WaferCount, _WaferMax, _Row, _Col)
{
	s_nTotalInitWafer += m_nWaferCount;
	m_nOutputWaferCount = 0;
}

LPM::~LPM()
{
	m_th.~thread();
}

#pragma endregion	

bool LPM::SetOutputWaferCount(int _value)
{
	if (m_nOutputWaferCount > m_nWaferMax || _value > m_nWaferMax || _value < 0) // WaferMax 수치 제한
	{
		return false;
	}
	
	else
	{
		m_nOutputWaferCount = _value;
		return true;
	}
}

int LPM::GetOutputWaferCount() const
{
	return m_nOutputWaferCount;
}

void LPM::Run() //LL <--> EFEM
{
	m_th = thread(&LPM::work, this);
}
void LPM::work()
{
	int nInitialWafer = m_nWaferCount;

	while (1)
	{
		Sleep(1);

		//if (s_nTotalSendWafer > 0 && s_nTotalSendWafer % s_nTotalInitWafer == 0)
		if(m_nWaferCount == 0 && m_nOutputWaferCount == m_nWaferMax)
		{
			m_nWaferCount = m_nWaferMax;
			m_nOutputWaferCount = 0;
		}
	}
}
#pragma endregion

#pragma region ATMRobot
HANDLE ATMRobot::s_hEventOutputWaferChange = CreateEvent(NULL, FALSE, TRUE, NULL);
HANDLE ATMRobot::s_hEventBlockATMRobot = CreateEvent(NULL, FALSE, TRUE, NULL);

int ATMRobot::s_nTotalWaferCntFromLPM;

#pragma region 생성자/소멸자
ATMRobot::ATMRobot(ModuleType _Type, CString _Name, int _WaferCount, int _WaferMax, int _Row, int _Col, int _PickTime, int _PlaceTime, int _RotateTime, int RoteteZTime)
	: ModuleBase(_Type, _Name, _WaferCount, _WaferMax, _Row, _Col)
{
	m_nPickTime = _PickTime;
	m_nPlaceTime = _PlaceTime;
	m_nRotateTime = _RotateTime;
	m_nRotateZCoordinateTime = RoteteZTime;
	m_bIsInputWafer = true;

	SetEvent(ATMRobot::s_hEventBlockATMRobot);
}

ATMRobot::~ATMRobot()
{
}
#pragma endregion

#pragma region Get/Set 메서드
void ATMRobot::SetPickTime(int _value)
{
	m_nPickTime = _value;
}
int ATMRobot::GetPickTime() const
{
	return m_nPickTime;
}
void ATMRobot::SetPlaceTime(int _value)
{
	m_nPlaceTime = _value;
}
int ATMRobot::GetPlaceTime() const
{
	return m_nPlaceTime;
}
void ATMRobot::SetRotateTime(int _value)
{
	m_nRotateTime = _value;
}
int ATMRobot::GetRotateTime() const
{
	return m_nRotateTime;
}
void ATMRobot::SetRotateZTime(int _value)
{
	m_nRotateZCoordinateTime = _value;
}
int ATMRobot::GetRotateZTime() const
{
	return m_nRotateZCoordinateTime;
}
void ATMRobot::SetIsInputWafer(bool _value)
{
	m_bIsInputWafer = _value;
}
bool ATMRobot::GetIsInputWafer() const
{
	return m_bIsInputWafer;
}
#pragma endregion

#pragma region 메서드
bool ATMRobot::PickWafer(ModuleBase* pM, CListCtrl* pClistCtrl)
{
	if (pM->m_eModuleType == TYPE_LPM && LPM::s_bLPMWaferPickBlock == true)
		return false;


	while (pM->GetIsWorking() == false
		&& pM->GetWaferCount() > 0
		&& m_nWaferCount < m_nWaferMax)
	{
		m_bIsWorking = true;

		Sleep(m_nRotateTime / SPEED);
		Sleep(m_nPickTime / SPEED);

		WaitForSingleObject(pM->m_hMutex, INFINITE);

		if (pM->SetWaferCount(pM->GetWaferCount() - 1) == true)
		{
			Sleep(m_nRotateTime / SPEED);
			Sleep(m_nPickTime / SPEED);

			SetWaferCount(m_nWaferCount + 1);

			if (pM->m_eModuleType == TYPE_LPM)
			{
				LPM* pLPM = (LPM*)pM;
				CString tmp = _T("");

				LPM::s_nTotalSendWafer++;
				tmp.Format(_T("Input:\n(%d)"), LPM::s_nTotalSendWafer);
				pClistCtrl->SetItemText(3, 1, tmp);

				SetEvent(ATMRobot::s_hEventBlockATMRobot);

				//Aligner와 Exchange
				Sleep(m_nRotateTime / SPEED);

				//GUI에 찍어줌
				tmp.Format(_T("%s\n(ExChange)\n(%d)"), m_strModuleName, m_nWaferCount);
				pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);

				tmp.Format(_T("Aligner\n(ExChange)\n(%d)"), 1);
				pClistCtrl->SetItemText(m_nRow - 1, m_nCol, tmp);

				Sleep(max(m_nPickTime, m_nPlaceTime) / SPEED);

				tmp.Format(_T("%s\n(ExChange)\n(%d)"), m_strModuleName, m_nWaferCount + 1);
				pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);

				tmp.Format(_T("Aligner\n(ExChange)\n(%d)"), 0);
				pClistCtrl->SetItemText(m_nRow - 1, m_nCol, tmp);

				//GUI에 찍어줌
				//

				tmp.Format(_T("%s\n(%d)"), m_strModuleName, m_nWaferCount);
				pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);

				tmp = _T("");
				tmp.Format(_T("%s\n(전:%d)\n(후:%d)"), pM->GetModuleName(), pM->GetWaferCount(), pLPM->GetOutputWaferCount());
				pClistCtrl->SetItemText(pM->m_nRow, pM->m_nCol, tmp);

				//tmp.Format(_T("Output\n(%d)"), LPM::s_nTotalOutputWafer);
				//pClistCtrl->SetItemText(3, m_nCol + 2, tmp);

				m_bIsWorking = false;
				ReleaseMutex(pM->m_hMutex);

				//
			}

			//GUI에 찍어줌
			else
			{
				CString tmp;

				if (s_bDirect == false)
				{
					tmp.Format(_T("%s\n(%d)"), m_strModuleName, m_nWaferCount);
					pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);

					tmp = _T("");
					tmp.Format(_T("%s\n(%d)"), pM->GetModuleName(), pM->GetWaferCount());
					pClistCtrl->SetItemText(pM->m_nRow, pM->m_nCol, tmp);
				}

				else if (s_bDirect == true)
				{
					int axis = CFabController::GetInstance().m_pModule.back()->m_nCol;

					tmp.Format(_T("%s\n(%d)"), m_strModuleName, m_nWaferCount);
					pClistCtrl->SetItemText(m_nRow, 2 * axis - m_nCol, tmp);

					tmp = _T("");
					tmp.Format(_T("%s\n(%d)"), pM->GetModuleName(), pM->GetWaferCount());
					pClistCtrl->SetItemText(pM->m_nRow, 2 * axis - pM->m_nCol, tmp);
				}
			}

			///////////////////////////////////////////////////////////////////////////////
			if (pM->m_eModuleType == TYPE_LOADLOCK)
			{
				LoadLock* p = (LoadLock*)pM;
				SetEvent(p->m_hLLWaferCntChangeEvent);
			}

			m_bIsWorking = false;
			ReleaseMutex(pM->m_hMutex);
			return true;
		}
	}
	return false;
}

bool ATMRobot::PlaceWafer(ModuleBase* pM, CListCtrl* pClistCtrl)
{	
	if (pM->m_eModuleType == TYPE_LOADLOCK && pM->GetDoorValveOpen() == false)
	{
		return false;
	}

	if (pM->m_eModuleType == TYPE_LPM)
	{
		LPM* pLPM = (LPM*)pM;

		if (m_nWaferCount > 0 &&
			pLPM->GetOutputWaferCount() + pLPM->GetWaferCount() < pLPM->GetWaferMax())
		{
			m_bIsWorking = true;
			WaitForSingleObject(pM->m_hMutex, INFINITE);

			SetWaferCount(m_nWaferCount - 1);
			pLPM->SetOutputWaferCount(pLPM->GetOutputWaferCount() + 1);

			LPM::s_nTotalOutputWafer++;
			SetEvent(ATMRobot::s_hEventOutputWaferChange);

			//GUI에 찍어줌
			CString tmp;

			int axis = CFabController::GetInstance().m_pModule.back()->m_nCol;

			tmp.Format(_T("%s\n(%d)"), m_strModuleName, m_nWaferCount);
			pClistCtrl->SetItemText(m_nRow, 2 * axis - m_nCol, tmp);

			tmp = _T("");
			tmp.Format(_T("%s\n(전:%d)\n(후:%d)"), pM->GetModuleName(), pM->GetWaferCount(), pLPM->GetOutputWaferCount());
			pClistCtrl->SetItemText(pM->m_nRow, 2 * axis - pM->m_nCol, tmp);

			tmp.Format(_T("Output\n(%d)"), LPM::s_nTotalOutputWafer);
			pClistCtrl->SetItemText(3, 2 * axis - m_nCol + 2, tmp);

			m_bIsWorking = false;
			ReleaseMutex(pM->m_hMutex);
			return true;
		}

		else
			return false;
	}

	while (pM->GetIsWorking() == false &&
		pM->GetWaferCount() < pM->GetWaferMax() &&
		m_nWaferCount > 0)
	{
		m_bIsWorking = true;
		WaitForSingleObject(pM->m_hMutex, INFINITE);

		Sleep(m_nRotateTime / SPEED);
		Sleep(m_nRotateZCoordinateTime / SPEED);
		Sleep(m_nPlaceTime / SPEED);


		//int InitialWaferYou = pM->GetWaferCount();
		//int InitialWaferMe = m_nWaferCount;

		if (pM->SetWaferCount(pM->GetWaferCount() + 1) == true)
		{
			SetWaferCount(m_nWaferCount - 1);

			//if (pM->m_eModuleType == TYPE_LPM)
			//{
			//	LPM::s_nTotalOutputWafer += pM->GetWaferCount() - InitialWaferYou;
			//	pM->SetWaferCount(InitialWaferYou);
			//	LPM* pLPM = (LPM*)pM;
			//	SetEvent(ATMRobot::s_hEventBlockATMRobot);

			//	CString tmp = _T("");
			//	tmp.Format(_T("Output\n(%d)"), LPM::s_nTotalOutputWafer);
			//	pClistCtrl->SetItemText(pM->m_nRow + 1, CFabController::GetInstance().m_pModule.back()->m_nCol * 2 - m_nCol + 1, tmp);
			//}

			if (pM->m_eModuleType == TYPE_LOADLOCK)
			{
				LoadLock* p = (LoadLock*)pM;
				SetEvent(p->m_hLLWaferCntChangeEvent);
			}
			//GUI에 찍어줌
			CString tmp;
			if (pM->m_eModuleType != TYPE_LPM)
			{
				if (s_bDirect == false)
				{
					tmp.Format(_T("%s\n(%d)"), m_strModuleName, m_nWaferCount);
					pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);

					tmp = _T("");
					tmp.Format(_T("%s\n(%d)"), pM->GetModuleName(), pM->GetWaferCount());
					pClistCtrl->SetItemText(pM->m_nRow, pM->m_nCol, tmp);
				}

				else if (s_bDirect == true)
				{
					int axis = CFabController::GetInstance().m_pModule.back()->m_nCol;

					tmp.Format(_T("%s\n(%d)"), m_strModuleName, m_nWaferCount);
					pClistCtrl->SetItemText(m_nRow, 2 * axis - m_nCol, tmp);

					tmp = _T("");
					tmp.Format(_T("%s\n(%d)"), pM->GetModuleName(), pM->GetWaferCount());
					pClistCtrl->SetItemText(pM->m_nRow, 2 * axis - pM->m_nCol, tmp);
				}
			}
			/////////////////////////////////////////////////////////////////////////////
			if (pM->m_eModuleType == TYPE_LOADLOCK)
			{
				LoadLock* p = (LoadLock*)pM;
				SetEvent(p->m_hLLWaferCntChangeEvent);
			}

		}

		m_bIsWorking = false;
		ReleaseMutex(pM->m_hMutex);
		return true;
	}
}

void ATMRobot::work(Pick_PlaceM Pick_Place)
{
	vector<ModuleBase*> vPickModules;
	vector<ModuleBase*> vPlaceModules;

	ModuleBase* pSavePickModule = NULL;
	ModuleBase* pSavePlaceModule = NULL;

	CListCtrl* pClistCtrl = Pick_Place.m_pClistCtrl;

	ModuleBase* pM = NULL;

	while (1)
	{

		//PM이 다 차있는지 확인함
		if (s_bDirect == false)
		{
			vPickModules = Pick_Place.m_vPickModule;
			vPlaceModules = Pick_Place.m_vPlaceModule;
		}

		else if (s_bDirect == true)
		{
			vPickModules = Pick_Place.m_vPlaceModule;
			vPlaceModules = Pick_Place.m_vPickModule;
		}


		//Wafer을 가져올 모듈을 모니터링함
		for (int i = 0; i < vPickModules.size(); i++)
		{
			//if (pSavePickModule != NULL && pSavePickModule->GetWaferCount() < pSavePickModule->GetWaferMax())
			//{
			//	PickWafer(pSavePickModule, pClistCtrl);
			//}

			//else
			//{
			//	pM = (ModuleBase*)vPickModules[i];

			//	PickWafer(pM, pClistCtrl);

			//	pSavePickModule = pM;

			//	break;
			//}

			pM = (ModuleBase*)vPickModules[i];

			PickWafer(pM, pClistCtrl);

		}

		//Wafer을 보낼 모듈을 모니터링함
		for (int i = 0; i < vPlaceModules.size(); i++)
		{
			//if (pSavePlaceModule != NULL && pSavePlaceModule->GetWaferCount() < pSavePlaceModule->GetWaferMax())
			//{
			//	PlaceWafer(pSavePlaceModule, pClistCtrl);
			//}

			//else
			//{
			//	pM = (ModuleBase*)vPlaceModules[i];

			//	PlaceWafer(pM, pClistCtrl);

			//	pSavePlaceModule = pM;

			//}
			//if (i > 0 && vPlaceModules[i - 1]->GetWaferCount() < vPlaceModules[i - 1]->GetWaferMax())
				//pM = (ModuleBase*)vPlaceModules[i - 1];

			//else
			pM = (ModuleBase*)vPlaceModules[i];

			PlaceWafer(pM, pClistCtrl);

			//break;
		}
	}
}

void ATMRobot::Run(vector<ModuleBase*> _vPickModules, vector<ModuleBase*> _vPlaceModules, CListCtrl* _pClistCtrl)
{
	Pick_PlaceM pick_place;
	pick_place.m_vPickModule = _vPickModules;
	pick_place.m_vPlaceModule = _vPlaceModules;
	pick_place.m_pClistCtrl = _pClistCtrl;

	m_th = thread(&ATMRobot::work, this, pick_place);
}
#pragma endregion

#pragma endregion