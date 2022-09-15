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

LPM::LPM(ModuleType _Type, CString _Name, int _WaferCount, int _WaferMax, int _Row, int _Col)
	: ModuleBase(_Type, _Name, _WaferCount, _WaferMax, _Row, _Col)
{
	s_nTotalInitWafer += m_nWaferCount;
	m_nOutputWafer = 0;
}

LPM::~LPM()
{
}

#pragma endregion
bool LPM::SetOutputWafer(int _value)
{
	if (_value > m_nWaferMax ||
		_value < 0)			// WaferMax 수치 제한
	{
		return false;
	}

	else
	{
		m_nWaferCount = _value;
		return true;
	}
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

	}
}
#pragma endregion

#pragma region ATMRobot

HANDLE ATMRobot::s_hEventBlockATMRobot = CreateEvent(NULL, TRUE, TRUE, NULL);

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
void ATMRobot::Pick()
{
}

void ATMRobot::Place()
{
}

void ATMRobot::Rotate()
{
}

bool ATMRobot::PickWafer(ModuleBase* pM, CListCtrl* pClistCtrl)
{
	Sleep(m_nRotateTime / SPEED);
	Sleep(m_nPickTime / SPEED);
	WaitForSingleObject(pM->m_hMutex, INFINITE);
	//pM->SetIsWorking(true);

	while (pM->m_eModuleType != TYPE_LPM || LPM::s_bLPMWaferPickBlock == false)
	{
		//int InitialWaferYou = pM->GetWaferCount();
		//int InitialWaferMe = m_nWaferCount;
		if (pM->m_eModuleType == TYPE_LPM)
		{
			if (LPM::s_bLPMWaferPickBlock == true)
				break;
			LPM::s_nTotalSendWafer += 1;
			SetEvent(ATMRobot::s_hEventBlockATMRobot);
		}

		if (pM->SetWaferCount(pM->GetWaferCount() - 1) == true)
		{
			SetWaferCount(m_nWaferCount + 1);

			if ((pM->GetWaferCount() > 0 &&
				m_nWaferCount < m_nWaferMax))
			{
				if (pM->m_eModuleType == TYPE_LPM)
				{
					if (LPM::s_bLPMWaferPickBlock == true)
						break;
					LPM::s_nTotalSendWafer += 1;
					SetEvent(ATMRobot::s_hEventBlockATMRobot);
				}

				if (pM->SetWaferCount(pM->GetWaferCount() - 1) == true &&
					SetWaferCount(m_nWaferCount + 1) == false)
				{
					pM->SetWaferCount(pM->GetWaferCount() + 1);
				}
			}




			//GUI에 찍어줌
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
			break;
		}
	}

	//pM->SetIsWorking(false);
	m_bIsWorking = false;
	ReleaseMutex(pM->m_hMutex);
	return true;
}

bool ATMRobot::PlaceWafer(ModuleBase* pM, CListCtrl* pClistCtrl)
{
	Sleep(m_nRotateTime / SPEED);
	Sleep(m_nRotateZCoordinateTime / SPEED);
	Sleep(m_nPlaceTime / SPEED);
	WaitForSingleObject(pM->m_hMutex, INFINITE);
	//pM->SetIsWorking(true);

	while (1)
	{
		int InitialWafer = pM->GetWaferCount();
		if (pM->SetWaferCount(pM->GetWaferCount() + 1) == true)
		{
			SetWaferCount(m_nWaferCount - 1);

			if (pM->SetWaferCount(pM->GetWaferCount() + 1) == true)
			{
				if (SetWaferCount(m_nWaferCount - 1) == false)
					pM->SetWaferCount(pM->GetWaferCount() - 1);
			}

			if (pM->m_eModuleType == TYPE_LPM)
			{
				LPM::s_nTotalOutputWafer += pM->GetWaferCount() - InitialWafer;
				pM->SetWaferCount(InitialWafer);
				SetEvent(ATMRobot::s_hEventBlockATMRobot);

				CString tmp = _T("");
				tmp.Format(_T("Output\n(%d)"), LPM::s_nTotalOutputWafer);
				pClistCtrl->SetItemText(pM->m_nRow + 1, CFabController::GetInstance().m_pModule.back()->m_nCol * 2 - m_nCol + 1, tmp);

			}

			//GUI에 찍어줌
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
			///////////////////////////////////////////////////////////////////////////////
			if (pM->m_eModuleType == TYPE_LOADLOCK)
			{
				LoadLock* p = (LoadLock*)pM;
				SetEvent(p->m_hLLWaferCntChangeEvent);
			}
			break;
		}
	}
	//pM->SetIsWorking(false);
	m_bIsWorking = false;
	ReleaseMutex(pM->m_hMutex);
	return true;
}

void ATMRobot::work(Pick_PlaceM Pick_Place)
{
	vector<ModuleBase*> vPickModules;
	vector<ModuleBase*> vPlaceModules;
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
			pM = (ModuleBase*)vPickModules[i];

			//while (1)
			{
				if (pM->GetIsWorking() == false && pM->GetWaferCount() > 0 && m_nWaferCount < m_nWaferMax && (pM->m_eModuleType != TYPE_LPM || LPM::s_bLPMWaferPickBlock == false))
				{
					//!!!!!!!!!!!!!!!!//
					m_bIsWorking = true;

					//if (pM->GetModuleName().Compare(_T("LPM")) == 0 &&
					//	LPM::s_bBlock == true)
					//{
					//	m_bIsWorking = false;
					//	//ResetEvent(ATMRobot::hEventBlockATMRobot);
					//}
					//else

					PickWafer(pM, pClistCtrl);


					//ReleaseMutex(pM->hMutex);
					//!!!!!!!!!!!!!!!!//
				}

				//	else
					//	break;
			}
		}

		//Wafer을 보낼 모듈을 모니터링함
		for (int i = 0; i < vPlaceModules.size(); i++)
		{
			pM = (ModuleBase*)vPlaceModules[i];

			//while (1)
			//{
				//if (pM->GetModuleName().Compare(_T("LPM")))
				//{
				//	LPM* p = (LPM*)pM;
				//	if (p->GetIsWorking() == false &&
				//		p->m_nOutputWafer > 0 &&
				//		p->m_nOutputWafer < p->GetWaferMax())
				//	{

				//	}
				//}
			if (pM->m_eModuleType == TYPE_LOADLOCK &&
				pM->GetDoorValveOpen() == false)
			{

			}

			else if (pM->GetIsWorking() == false &&
				m_nWaferCount > 0 &&
				pM->GetWaferCount() < pM->GetWaferMax())
			{
				//!!!!!!!!!!!!!!!!//
				m_bIsWorking = true;

				PlaceWafer(pM, pClistCtrl);

				//!!!!!!!!!!!!!!!!//
			}

			//	else
				//	break;
		//	}
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