#include "pch.h"
#include "LoadLock.h"
#include "VACRobot.h"
#include "ProcessChamber.h"
#include "CFabController.h"

#pragma region 생성자/소멸자
VACRobot::VACRobot(ModuleType _Type, CString _Name, int _WaferCount, int _WaferMax, int _Row, int _Col, int _PickTime, int _PlaceTime, int _RotateTime)
	: ModuleBase(_Type, _Name, _WaferCount, _WaferMax, _Row, _Col)
{
	m_nPickTime = _PickTime;
	m_nPlaceTime = _PlaceTime;
	m_nRotateTime = _RotateTime;
}
VACRobot::~VACRobot()
{
}
#pragma endregion

#pragma region Get/Set 메서드
void VACRobot::SetPickTime(int _value)
{
	m_nPickTime = _value;
}

int VACRobot::GetPickTime() const
{
	return m_nPickTime;
}

void VACRobot::SetPlaceTime(int _value)
{
	m_nPlaceTime = _value;
}

int VACRobot::GetPlaceTime() const
{
	return m_nPlaceTime;
}

void VACRobot::SetRotateTime(int _value)
{
	m_nRotateTime = _value;
}

int VACRobot::GetRotateTime() const
{
	return m_nRotateTime;
}
#pragma endregion

#pragma region 메서드
void VACRobot::Pick()
{
}
void VACRobot::Place()
{
}
void VACRobot::Rotate()
{
}

bool VACRobot::PickWafer(ModuleBase* pM, CListCtrl* pClistCtrl)
{
	Sleep(m_nRotateTime / SPEED);
	Sleep(m_nPickTime / SPEED);
	WaitForSingleObject(pM->m_hMutex, INFINITE);


	while (pM->GetIsWorking() == false)// ||
		//(pM->m_eModuleType != TYPE_LOADLOCK || pM->GetSlotValveOpen() == true))
	{
		if (pM->SetWaferCount(pM->GetWaferCount() - 1) == true)
		{
			SetWaferCount(m_nWaferCount + 1);

			if (pM->SetWaferCount(pM->GetWaferCount() - 1) == true)
			{
				if (SetWaferCount(m_nWaferCount + 1) == false)
					pM->SetWaferCount(pM->GetWaferCount() + 1);
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
				pClistCtrl->SetItemText(m_nRow, axis*2 - m_nCol, tmp);

				tmp = _T("");
				tmp.Format(_T("%s\n(%d)"), pM->GetModuleName(), pM->GetWaferCount());
				pClistCtrl->SetItemText(pM->m_nRow, axis*2 - pM->m_nCol, tmp);
			}

			if (pM->m_eModuleType == TYPE_PROCESSCHAMBER)
			{
				ProcessChamber* p = (ProcessChamber*)pM;
				SetEvent(p->m_hPmWaferCntChangeEvent);
			}

			if (pM->m_eModuleType == TYPE_LOADLOCK)
			{
				LoadLock* p = (LoadLock*)pM;
				SetEvent(p->m_hLLWaferCntChangeEvent);
			}
			//pM->SetIsWorking(false);
			break;
		}
	}
	m_bIsWorking = false;
	ReleaseMutex(pM->m_hMutex);
	return true;
}

bool VACRobot::PlaceWafer(ModuleBase* pM, CListCtrl* pClistCtrl)
{
	Sleep(m_nRotateTime / SPEED);
	Sleep(m_nPlaceTime / SPEED);
	WaitForSingleObject(pM->m_hMutex, INFINITE);
	//pM->SetIsWorking(true);

	while (1)
	{
		if (pM->SetWaferCount(pM->GetWaferCount() + 1) == true)
		{
			SetWaferCount(m_nWaferCount - 1);

			if (pM->SetWaferCount(pM->GetWaferCount() + 1) == true)
			{
				if (SetWaferCount(m_nWaferCount - 1) == false)
					pM->SetWaferCount(pM->GetWaferCount() - 1);
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
				pClistCtrl->SetItemText(m_nRow, axis*2 - m_nCol, tmp);

				tmp = _T("");
				tmp.Format(_T("%s\n(%d)"), pM->GetModuleName(), pM->GetWaferCount());
				pClistCtrl->SetItemText(pM->m_nRow, axis*2 - pM->m_nCol, tmp);
			}

			if (pM->m_eModuleType == TYPE_PROCESSCHAMBER)
			{
				ProcessChamber* p = (ProcessChamber*)pM;
				SetEvent(p->m_hPmWaferCntChangeEvent);
			}
			break;
		}
	}
	//pM->SetIsWorking(false);
	m_bIsWorking = false;
	ReleaseMutex(pM->m_hMutex);
	return true;
}

void VACRobot::work(Pick_PlaceM Pick_Place)
{
	vector<ModuleBase*> m_vPickModules;
	vector<ModuleBase*> m_vPlaceModules;

	CListCtrl* pClistCtrl = Pick_Place.m_pClistCtrl;
	ModuleBase* pM = NULL;

	//1. DualArm일 경우 동작
	//while (m_nWaferMax == 2)
	//{
	//	//PM이 다 차있는지 확인함
	//	if (s_bDirect == false)
	//	{
	//		m_vPickModules = Pick_Place.m_vPickModule;
	//		m_vPlaceModules = Pick_Place.m_vPlaceModule;
	//	}

	//	else if (s_bDirect == true)
	//	{
	//		m_vPickModules = Pick_Place.m_vPlaceModule;
	//		m_vPlaceModules = Pick_Place.m_vPickModule;
	//	}

	//	//Wafer을 가져올 모듈을 모니터링함
	//	for (int i = 0; i < m_vPickModules.size(); i++)
	//	{
	//		pM = (ModuleBase*)m_vPickModules[i];
	//		//while (1)
	//		//{
	//			//if (pM->GetModuleName().Compare(_T("LL")) == 0 &&
	//			//	pM->GetWaferMax() != pM->GetWaferCount())
	//			//{
	//				//continue;
	//			//}

	//		if (pM->m_eModuleType == TYPE_LOADLOCK &&
	//			pM->GetSlotValveOpen() == false)
	//		{
	//		}

	//		else if (pM->GetIsWorking() == false &&
	//			pM->GetWaferCount() > 0 &&
	//			m_nWaferCount < m_nWaferMax)
	//		{
	//			//!!!!!!!!!!!!!!!!//
	//			m_bIsWorking = true;

	//			PickWafer(pM, pClistCtrl);

	//			//ReleaseMutex(pM->hMutex);
	//			//!!!!!!!!!!!!!!!!//
	//		}
	//		//else
	//			//break;
	//	//}
	//	}

	//	//Wafer을 보낼 모듈을 모니터링함
	//	for (int i = 0; i < m_vPlaceModules.size(); i++)
	//	{
	//		pM = (ModuleBase*)m_vPlaceModules[i];

	//		//while (1)
	//		{
	//			if (pM->GetIsWorking() == false &&
	//				m_nWaferCount > 0 &&
	//				pM->GetWaferCount() < pM->GetWaferMax())
	//			{
	//				//!!!!!!!!!!!!!!!!//
	//				m_bIsWorking = true;

	//				PlaceWafer(pM, pClistCtrl);


	//				//!!!!!!!!!!!!!!!!//
	//			}

	//			//else
	//				//break;
	//		}
	//	}
	//}

  	while (m_nWaferMax == 2)
	{
		//Wafer을 가져올 모듈을 모니터링함
		for (int i = 0; i < m_vPickModules.size(); i++)
		{
			pM = (ModuleBase*)m_vPickModules[i];
			//while (1)
			//{
				//if (pM->GetModuleName().Compare(_T("LL")) == 0 &&
				//	pM->GetWaferMax() != pM->GetWaferCount())
				//{
					//continue;
				//}

			if (pM->m_eModuleType == TYPE_LOADLOCK &&
				pM->GetSlotValveOpen() == false)
			{
			}

			else if (pM->GetIsWorking() == false &&
				pM->GetWaferCount() > 0 &&
				m_nWaferCount < m_nWaferMax)
			{
				//!!!!!!!!!!!!!!!!//
				m_bIsWorking = true;

				PickWafer(pM, pClistCtrl);

				//ReleaseMutex(pM->hMutex);
				//!!!!!!!!!!!!!!!!//
			}
			//else
				//break;
		//}
		}

		//Wafer을 보낼 모듈을 모니터링함
		for (int i = 0; i < m_vPlaceModules.size(); i++)
		{
			pM = (ModuleBase*)m_vPlaceModules[i];

			//while (1)
			{
				if (pM->GetIsWorking() == false &&
					m_nWaferCount > 0 &&
					pM->GetWaferCount() < pM->GetWaferMax())
				{
					//!!!!!!!!!!!!!!!!//
					m_bIsWorking = true;

					PlaceWafer(pM, pClistCtrl);


					//!!!!!!!!!!!!!!!!//
				}

				//else
					//break;
			}
		}
	}

	//2. Quad Arm일 경우 동작
	while (m_nWaferMax == 4)
	{
		//PM이 다 차있는지 확인함
		if (s_bDirect == false)
		{
			m_vPickModules = Pick_Place.m_vPickModule;
			m_vPlaceModules = Pick_Place.m_vPlaceModule;
		}

		else if (s_bDirect == true)
		{
			m_vPickModules = Pick_Place.m_vPlaceModule;
			m_vPlaceModules = Pick_Place.m_vPickModule;
		}

		//Wafer을 가져올 모듈을 모니터링함
		for (int i = 0; i < m_vPickModules.size(); i++)
		{
			pM = (ModuleBase*)m_vPickModules[i];
			//while (1)
			//{
				//if (pM->GetModuleName().Compare(_T("LL")) == 0 &&
				//	pM->GetWaferMax() != pM->GetWaferCount())
				//{
					//continue;
				//}

			if (pM->m_eModuleType == TYPE_LOADLOCK &&
				pM->GetSlotValveOpen() == false)
			{
			}

			else if (pM->GetIsWorking() == false &&
				pM->GetWaferCount() > 1 &&
				m_nWaferCount + 1 < m_nWaferMax)
			{
				//!!!!!!!!!!!!!!!!//
				m_bIsWorking = true;

				PickWafer(pM, pClistCtrl);

				//ReleaseMutex(pM->hMutex);
				//!!!!!!!!!!!!!!!!//
			}
			//else
				//break;
		//}
		}

		//Wafer을 보낼 모듈을 모니터링함
		for (int i = 0; i < m_vPlaceModules.size(); i++)
		{
			pM = (ModuleBase*)m_vPlaceModules[i];

			//while (1)
			{
				if (pM->GetIsWorking() == false &&
					m_nWaferCount > 0 &&
					pM->GetWaferCount() < pM->GetWaferMax())
				{
					//!!!!!!!!!!!!!!!!//
					m_bIsWorking = true;

					PlaceWafer(pM, pClistCtrl);


					//!!!!!!!!!!!!!!!!//
				}

				//else
					//break;
			}
		}
	}
}

void VACRobot::Run(vector<ModuleBase*> m_vPickModules, vector<ModuleBase*> m_vPlaceModules, CListCtrl* pClist)
{
	Pick_PlaceM pick_place;
	pick_place.m_vPickModule = m_vPickModules;
	pick_place.m_vPlaceModule = m_vPlaceModules;
	pick_place.m_pClistCtrl = pClist;

	m_th = thread(&VACRobot::work, this, pick_place);
}
#pragma endregion
