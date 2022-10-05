#include "pch.h"
#include "EFEM.h"
#include "LoadLock.h"
#include "VACRobot.h"
#include "ProcessChamber.h"
#include "CFabController.h"


HANDLE VACRobot::s_hVACRobotExchangeOver = CreateEvent(NULL, FALSE, TRUE, NULL);

#pragma region ������/�Ҹ���
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

#pragma region Get/Set �޼���
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

#pragma region �޼���
void VACRobot::SaveConfigModule(int nIdx, CString strFilePath)
{
	// �⺻ ModuleBase�� �ʵ�
	CString strIdx, strRow, strCol, strWaferMax;
	strIdx.Format(_T("%d"), nIdx);
	strRow.Format(_T("%d"), m_nRow);
	strCol.Format(_T("%d"), m_nCol);
	strWaferMax.Format(_T("%d"), m_nWaferMax);

	// VACRobot �߰� �ʵ�
	CString strPickTime, strPlaceTime, strRotateTime, strZRotatetime, strArmMode;
	strArmMode.Format(m_nWaferMax == 4 ? _T("QuadArm") : _T("DualArm"));					// WaferMax == 4 �� �� QuadArm | �ƴ� �� DualArm
	strPickTime.Format(_T("%d"), m_nPickTime);
	strPlaceTime.Format(_T("%d"), m_nPlaceTime);
	strRotateTime.Format(_T("%d\n"), m_nRotateTime);


	WritePrivateProfileString(strIdx, _T("ModuleType"), _T("TYPE_VACROBOT"), strFilePath);			// Ÿ��
	WritePrivateProfileString(strIdx, _T("ModuleName"), m_strModuleName, strFilePath);				// ����
	WritePrivateProfileString(strIdx, _T("PosX"), strCol, strFilePath);								// �÷�
	WritePrivateProfileString(strIdx, _T("PosY"), strRow, strFilePath);								// �ο�
	WritePrivateProfileString(strIdx, _T("ArmMode"), strArmMode, strFilePath);						// ArmMode(WaferMax)
	WritePrivateProfileString(strIdx, _T("PickTime"), strPickTime, strFilePath);					// PickTime
	WritePrivateProfileString(strIdx, _T("PlaceTime"), strPlaceTime, strFilePath);					// PlaceTime
	WritePrivateProfileString(strIdx, _T("RotateTime"), strRotateTime, strFilePath);				// RotateTime
}

bool VACRobot::PickWafer(ModuleBase* pM, CListCtrl* pClistCtrl)
{
	m_bIsWorking = true;
	WaitForSingleObject(pM->m_hMutex, INFINITE);

	Sleep(m_nRotateTime / ModuleBase::s_dSpeed);
	Sleep(m_nPickTime / ModuleBase::s_dSpeed);


	while (pM->GetIsWorking() == false)// ||
		//(pM->m_eModuleType != TYPE_LOADLOCK || pM->GetSlotValveOpen() == true))
	{
		int InitialWafer = pM->GetWaferCount();
		if (pM->SetWaferCount(pM->GetWaferCount() - m_nWaferMax / 2) == true)
		{
			SetWaferCount(m_nWaferCount + m_nWaferMax / 2);

			//if (pM->SetWaferCount(pM->GetWaferCount() - 1) == true)
			//{
			//	if (SetWaferCount(m_nWaferCount + 1) == false)
			//		pM->SetWaferCount(pM->GetWaferCount() + 1);
			//}


			//GUI�� �����
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
	m_bIsWorking = true;
	WaitForSingleObject(pM->m_hMutex, INFINITE);

	Sleep(m_nRotateTime / ModuleBase::s_dSpeed);
	Sleep(m_nPlaceTime / ModuleBase::s_dSpeed);     
	//pM->SetIsWorking(true);

	while (1)
	{
		if (pM->SetWaferCount(pM->GetWaferCount() + m_nWaferMax / 2) == true)
		{
			SetWaferCount(m_nWaferCount - m_nWaferMax / 2);

			//if (pM->SetWaferCount(pM->GetWaferCount() + 1) == true)
			//{
			//	if (SetWaferCount(m_nWaferCount - 1) == false)
			//		pM->SetWaferCount(pM->GetWaferCount() - 1);
			//}


			//GUI�� �����
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
	
	int nMaxPMSlot = CFabController::s_pPM.size() * CFabController::s_pPM[0]->GetWaferMax();
	int nMaxLLSlot = CFabController::s_pLL.size() * CFabController::s_pLL[0]->GetWaferMax();
	int nCntNeedToExchangeWafer = min(nMaxLLSlot, nMaxPMSlot) * 2;

	//1.
	while (m_bStopFlag == false)
	{
		Sleep(1);
		//1.1.1.1. Exchange�� �����ٴ� ����
		bool bIsThereTrue = false;
		for (int i = 0, j = 0; i < vPickModules.size() || j < vPlaceModules.size(); i++, j++)
		{
			if (vPickModules[i]->m_bExchangeOver == true || vPlaceModules[j]->m_bExchangeOver == true)
				bIsThereTrue = true;
		}

		bool bCheckATMRobotEmpty = true;
		for (int i = 0; i < CFabController::s_pATMRobot.size(); i++)
		{
			if (CFabController::s_pATMRobot[i]->GetWaferCount() > 0)
				bCheckATMRobotEmpty = false;
		}

		bool bCheckLLFull = true;
		for (int i = 0; i < CFabController::s_pLL.size(); i++)
		{
			if (CFabController::s_pLL[i]->GetWaferCount() != CFabController::s_pLL[i]->GetWaferMax())
				bCheckLLFull = false;
		}

		//1.1. Wafer Exchange�ϴ� ���
		if (LPM::s_nTotalSendWafer > nMaxLLSlot && LPM::s_nTotalSendWafer % nMaxLLSlot == 0
			&& s_bDirect == false
			&& bIsThereTrue == false
			&& bCheckATMRobotEmpty == true
			&& bCheckLLFull == true)
		{	

			//1.1.1.2. Exchange�� �ʿ��� ���ڵ�
			int nCntExchangedWaferPickModule = 0;
			int nCntExchangedWaferPlaceModule = 0;

			int nCntNeedToExchangeWaferPickModule = 0;
			int nCntNeedToExchangeWaferPlaceModule = 0;

			//1.1.2. �۾� ����

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

					//1. LL�� ���� wafer�� Pick

					if (SetWaferCount(m_nWaferCount + m_nWaferMax/2) == false)
						continue;


					vPickModules[i]->SetWaferCount(vPickModules[i]->GetWaferCount() - m_nWaferMax/2);
					
					// ��� ������ Thruput ���ϱ� ���� VAC Input, Output ++
					if (m_nWaferMax == 2)
					{
						m_nInputWafer++;
						m_nOutputWafer++;
					}
					else // QuadArm
					{
						m_nInputWafer = m_nInputWafer + 2;				
						m_nOutputWafer = m_nOutputWafer + 2;
					}
					// =====================================================
					
					Sleep(m_nPickTime / ModuleBase::s_dSpeed);
					nCntExchangedWaferPickModule++;

					//ȭ�鿡 ���
					CString tmp = _T("");
					tmp.Format(_T("%s\n(%d)"), m_strModuleName, m_nWaferCount);
					pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);

					tmp = _T("");
					tmp.Format(_T("%s\n(%d)"), vPickModules[i]->GetModuleName(), vPickModules[i]->GetWaferCount());
					pClistCtrl->SetItemText(vPickModules[i]->m_nRow, vPickModules[i]->m_nCol, tmp);



					for (int k = 0; k < min(nCntNeedToExchangeWaferPickModule, nCntNeedToExchangeWaferPlaceModule) * 2 - 2; k++)
					{
						//ȭ�鿡 ���
						tmp = _T("");
						tmp.Format(_T("%s\n(ExChange)\n(%d)"), m_strModuleName, m_nWaferCount);
						pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);

						tmp = _T("");
						tmp.Format(_T("%s\n(ExChange)\n(%d)"), vPickModules[i]->GetModuleName(), vPickModules[i]->GetWaferCount());
						pClistCtrl->SetItemText(vPickModules[i]->m_nRow, vPickModules[i]->m_nCol, tmp);

						tmp = _T("");
						tmp.Format(_T("%s\n(ExChange)\n(%d)"), vPlaceModules[j]->GetModuleName(), vPlaceModules[j]->GetWaferCount());
						pClistCtrl->SetItemText(vPlaceModules[j]->m_nRow, vPlaceModules[j]->m_nCol, tmp);

						//2. PM�� ���� Rotate
						Sleep(m_nRotateTime / ModuleBase::s_dSpeed);

						//3. PM�� wafer�� Exchange
						Sleep(max(m_nPlaceTime, m_nPickTime) / ModuleBase::s_dSpeed);

						if (k % 2 == 1) nCntExchangedWaferPickModule++;
						else nCntExchangedWaferPlaceModule++;

						//vPlaceModules[i]->SetWaferCount(vPlaceModules[i]->GetWaferCount() - 1);
						//m_nWaferCount++;

						//ȭ�鿡 ���
						tmp.Format(_T("%s\n(%d)"), m_strModuleName, m_nWaferCount);
						pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);

						tmp = _T("");
						tmp.Format(_T("%s\n(%d)"), vPickModules[i]->GetModuleName(), vPickModules[i]->GetWaferCount());
						pClistCtrl->SetItemText(vPickModules[i]->m_nRow, vPickModules[i]->m_nCol, tmp);

						tmp = _T("");
						tmp.Format(_T("%s\n(%d)"), vPlaceModules[j]->GetModuleName(), vPlaceModules[j]->GetWaferCount());
						pClistCtrl->SetItemText(vPlaceModules[j]->m_nRow, vPlaceModules[j]->m_nCol, tmp);
					}

					Sleep(m_nRotateTime / ModuleBase::s_dSpeed);
					if (SetWaferCount(m_nWaferCount - m_nWaferMax/2) == false)
						continue;
					vPickModules[i]->SetWaferCount(vPickModules[i]->GetWaferCount() + m_nWaferMax/2);

					Sleep(m_nPlaceTime / ModuleBase::s_dSpeed);
					nCntExchangedWaferPlaceModule++;


					//ȭ�鿡 ���
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

		//1.2. Wafer Move�ϴ� ���
		else if(LoadLock::s_nTotalSendWaferFromLL < nMaxPMSlot)
		{
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
				else if (pM->GetIsWorking() == false && pM->GetWaferCount() >= m_nWaferMax / 2 && m_nWaferCount < m_nWaferMax)
				{
					//!!!!!!!!!!!!!!!!//

					PickWafer(pM, pClistCtrl);

					//!!!!!!!!!!!!!!!!//
				}


			}

			//Wafer�� ���� ����� ����͸���
			for (int i = 0; i < vPlaceModules.size(); i++)
			{
				pM = (ModuleBase*)vPlaceModules[i];

				if (pM->GetIsWorking() == false &&
					m_nWaferCount >= m_nWaferMax / 2 &&
					pM->GetWaferCount() <= pM->GetWaferMax() - m_nWaferMax / 2)
				{
					//!!!!!!!!!!!!!!!!//

					PlaceWafer(pM, pClistCtrl);

					//!!!!!!!!!!!!!!!!//
				}
			}
		}


	}

 // 	while (m_nWaferMax == 2)
	//{
	//	//Wafer�� ������ ����� ����͸���
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

	//	//Wafer�� ���� ����� ����͸���
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

	////2. Quad Arm�� ��� ����
	//while (m_nWaferMax == 4)
	//{
	//	//PM�� �� ���ִ��� Ȯ����
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

	//	//Wafer�� ������ ����� ����͸���
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

	//	//Wafer�� ���� ����� ����͸���
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
