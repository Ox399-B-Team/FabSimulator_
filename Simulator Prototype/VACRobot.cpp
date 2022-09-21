#include "pch.h"
#include "EFEM.h"
#include "LoadLock.h"
#include "VACRobot.h"
#include "ProcessChamber.h"
#include "CFabController.h"


HANDLE VACRobot::s_hVACRobotExchangeOver = CreateEvent(NULL, TRUE, TRUE, NULL);

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
		int InitialWafer = pM->GetWaferCount();
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
				LoadLock::s_nTotalSendWaferFromLL += InitialWafer - pM->GetWaferCount();;
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
	vector<ModuleBase*> vPickModules;
	vector<ModuleBase*> vPlaceModules;

	CListCtrl* pClistCtrl = Pick_Place.m_pClistCtrl;
	ModuleBase* pM = NULL;

	vPickModules = Pick_Place.m_vPickModule;
	vPlaceModules = Pick_Place.m_vPlaceModule;
	
	bool bCheck = false;

	//1. DualArm일 경우 동작	
	while (m_nWaferMax == 2)
	{	
		int nMaxPMSlot = CFabController::s_pPM.size() * CFabController::s_pPM[0]->GetWaferMax();
		int nMaxLLSlot = CFabController::s_pLL.size() * CFabController::s_pLL[0]->GetWaferMax();
		int nCntNeedToExchangeWafer = min(nMaxLLSlot, nMaxPMSlot) * 2;

		//Wafer Exchange하는 경우
		if (LPM::s_nTotalSendWafer > 0 && LPM::s_nTotalSendWafer % nMaxLLSlot == 0 
			&& s_bDirect == false
			&& bCheck == true)
		{
			//1.작업 전 초기화 코드
			//1.1. Exchange가 끝났다는 조건
			for (int i = 0, j = 0; i < vPickModules.size() || j < vPlaceModules.size(); i++, j++)
			{
				vPickModules[i]->m_bExchangeOver = false;
				vPlaceModules[j]->m_bExchangeOver = false;
			}

			//1.2. Exchange에 필요한 인자들
			int nCntExchangedWaferPickModule = 0;
			int nCntExchangedWaferPlaceModule = 0;

			int nCntNeedToExchangeWaferPickModule = 0;
			int nCntNeedToExchangeWaferPlaceModule = 0;


			//2. 작업 시작
			for (int i = 0; i < vPickModules.size(); i++)
			{
				//
				if (vPickModules[i]->m_bExchangeOver == true)
					continue;

				nCntNeedToExchangeWaferPickModule = vPickModules[i]->GetWaferMax();

				for (int j = 0; j < vPlaceModules.size(); j++)
				{
					if (vPlaceModules[j]->m_bExchangeOver == true)
						continue;

					nCntNeedToExchangeWaferPlaceModule = vPlaceModules[j]->GetWaferMax();
					//1. LL로 부터 wafer를 Pick

					if (SetWaferCount(m_nWaferCount + 1) == false)
						continue;
					vPickModules[i]->SetWaferCount(vPickModules[i]->GetWaferCount() - 1);

					Sleep(m_nPickTime / SPEED);
					nCntExchangedWaferPickModule++;

					//화면에 출력
					CString tmp = _T("");
					tmp.Format(_T("%s\n(%d)"), m_strModuleName, m_nWaferCount);
					pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);

					tmp = _T("");
					tmp.Format(_T("%s\n(%d)"), vPickModules[i]->GetModuleName(), vPickModules[i]->GetWaferCount());
					pClistCtrl->SetItemText(vPickModules[i]->m_nRow, vPickModules[i]->m_nCol, tmp);


					for (int k = 0; k < min(nCntNeedToExchangeWaferPickModule, nCntNeedToExchangeWaferPlaceModule) * 2 - 2; k++)
					{
						//화면에 출력
						tmp = _T("");
						tmp.Format(_T("%s\n(ExChange)\n(%d)"), m_strModuleName, m_nWaferCount);
						pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);

						tmp = _T("");
						tmp.Format(_T("%s\n(ExChange)\n(%d)"), vPickModules[i]->GetModuleName(), vPickModules[i]->GetWaferCount());
						pClistCtrl->SetItemText(vPickModules[i]->m_nRow, vPickModules[i]->m_nCol, tmp);

						tmp = _T("");
						tmp.Format(_T("%s\n(ExChange)\n(%d)"), vPlaceModules[j]->GetModuleName(), vPlaceModules[j]->GetWaferCount());
						pClistCtrl->SetItemText(vPlaceModules[j]->m_nRow, vPlaceModules[j]->m_nCol, tmp);

						//2. PM을 향해 Rotate
						Sleep(m_nRotateTime / SPEED);

						//3. PM의 wafer와 Exchange
						Sleep(max(m_nPlaceTime, m_nPickTime) / SPEED);

						if (k % 2 == 1) nCntExchangedWaferPickModule++;
						else nCntExchangedWaferPlaceModule++;

						//vPlaceModules[i]->SetWaferCount(vPlaceModules[i]->GetWaferCount() - 1);
						//m_nWaferCount++;

						//화면에 출력
						tmp.Format(_T("%s\n(%d)"), m_strModuleName, m_nWaferCount);
						pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);

						tmp = _T("");
						tmp.Format(_T("%s\n(%d)"), vPickModules[i]->GetModuleName(), vPickModules[i]->GetWaferCount());
						pClistCtrl->SetItemText(vPickModules[i]->m_nRow, vPickModules[i]->m_nCol, tmp);

						tmp = _T("");
						tmp.Format(_T("%s\n(%d)"), vPlaceModules[j]->GetModuleName(), vPlaceModules[j]->GetWaferCount());
						pClistCtrl->SetItemText(vPlaceModules[j]->m_nRow, vPlaceModules[j]->m_nCol, tmp);
					}

					Sleep(m_nRotateTime / SPEED);
					if (SetWaferCount(m_nWaferCount - 1) == false)
						continue;
					vPickModules[i]->SetWaferCount(vPickModules[i]->GetWaferCount() + 1);

					Sleep(m_nPlaceTime / SPEED);
					nCntExchangedWaferPlaceModule++;


					//화면에 출력
					tmp = _T("");
					tmp.Format(_T("%s\n(%d)"), m_strModuleName, m_nWaferCount);
					pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);

					tmp = _T("");
					tmp.Format(_T("%s\n(%d)"), vPickModules[i]->GetModuleName(), vPickModules[i]->GetWaferCount());
					pClistCtrl->SetItemText(vPickModules[i]->m_nRow, vPickModules[i]->m_nCol, tmp);

					tmp = _T("");
					tmp.Format(_T("%s\n\n(%d)"), vPlaceModules[j]->GetModuleName(), vPlaceModules[j]->GetWaferCount());
					pClistCtrl->SetItemText(vPlaceModules[j]->m_nRow, vPlaceModules[j]->m_nCol, tmp);


					nCntNeedToExchangeWaferPickModule = vPickModules[i]->GetWaferMax();
					nCntNeedToExchangeWaferPlaceModule = vPlaceModules[j]->GetWaferMax();

					if (nCntExchangedWaferPickModule == nCntNeedToExchangeWaferPickModule)
					{
						nCntExchangedWaferPickModule = 0;

						LoadLock* pLL = (LoadLock*)(vPickModules[i]);
						pLL->m_bExchangeOver = true;

						if (nCntExchangedWaferPlaceModule == nCntNeedToExchangeWaferPlaceModule)
						{
							nCntExchangedWaferPlaceModule = 0;

							ProcessChamber* pPM = (ProcessChamber*)(vPlaceModules[j]);
							pPM->m_bExchangeOver = true;
							SetEvent(pPM->m_hPmWaferCntChangeEvent);

						}
						break;
					}
				}
			}
			LoadLock::s_nTotalSendWaferFromLL = 0;
			ModuleBase::s_bDirect = true;

			for (int i = 0; i < vPickModules.size(); i++)
			{
				LoadLock* pLL = (LoadLock*)vPickModules[i];
				SetEvent(pLL->m_hLLWaferCntChangeEvent);
			}
		}

		//Wafer Move하는 경우
		else
		{
			bCheck = true;

			int nCntTotalPMWafer = 0;
			for (int i = 0; i < vPlaceModules.size(); i++)
			{
				nCntTotalPMWafer += vPlaceModules[i]->GetWaferCount();
			}

			if (nCntTotalPMWafer == nMaxPMSlot)
				continue;

			for (int i = 0; i < vPickModules.size(); i++)
			{
				pM = (ModuleBase*)vPickModules[i];
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


			}

			//Wafer을 보낼 모듈을 모니터링함
			for (int i = 0; i < vPlaceModules.size(); i++)
			{
				pM = (ModuleBase*)vPlaceModules[i];

				if (pM->GetIsWorking() == false &&
					m_nWaferCount > 0 &&
					pM->GetWaferCount() < pM->GetWaferMax())
				{
					//!!!!!!!!!!!!!!!!//
					m_bIsWorking = true;

					PlaceWafer(pM, pClistCtrl);

					//!!!!!!!!!!!!!!!!//
				}
			}
		}
	}

 // 	while (m_nWaferMax == 2)
	//{
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

	////2. Quad Arm일 경우 동작
	//while (m_nWaferMax == 4)
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
	//			pM->GetWaferCount() > 1 &&
	//			m_nWaferCount + 1 < m_nWaferMax)
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
