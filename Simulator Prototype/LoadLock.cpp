#include "pch.h"
#include "EFEM.h"
#include "LoadLock.h"

int LoadLock::s_nTotalSendWaferFromLL = 0;
int LoadLock::s_nRequiredDummyWaferCntLpmToPM = 0;
int LoadLock::s_nRequiredDummyWaferCntPMToLpm = 0;
int LoadLock::s_nCount = 0;

vector<LoadLock*> LoadLock::s_pLL;

#pragma region 생성자/소멸자
LoadLock::LoadLock(ModuleType _Type, CString _Name, int _WaferCount, int _WaferMax, int _Row, int _Col,
	int _PumpTime, int _PumpStableTime, int _VentTime, int _VentStableTime, int _SlotOpenTime, int _SlotCloseTime, int _DoorOpenTime, int _DoorCloseTime)
	: ModuleBase(_Type, _Name, _WaferCount, _WaferMax, _Row, _Col)
{
	m_nPumpTime = _PumpTime;
	m_nPumpStableTime = _PumpStableTime;
	m_nVentTime = _VentTime;
	m_nVentStableTime = _VentStableTime;
	m_nSlotValveOpenTime = _SlotOpenTime;
	m_nSlotValveCloseTime = _SlotCloseTime;
	m_nDoorValveOpenTime = _DoorOpenTime;
	m_nDoorValveCloseTime = _DoorCloseTime;

	m_bSlotValveOpen = true;
	m_bIsInputWafer = true;
	s_nCount++;

	m_hLLWaferCntChangeEvent = CreateEvent(NULL, TRUE, TRUE, NULL);

	s_pLL.push_back(this);
}

LoadLock::~LoadLock()
{
	CloseHandle(m_hLLWaferCntChangeEvent);
	s_nCount--;

	for (int i = 0; i < s_pLL.size(); i++)
	{
		if (s_pLL[i] == this)
		{
			s_pLL.erase(s_pLL.begin() + i);
			break;
		}
	}
}
#pragma endregion

#pragma region Get/Set 메서드
void LoadLock::SetPumpTime(int _value)
{
	m_nPumpTime = _value;
}

int LoadLock::GetPumpTime() const
{
	return m_nPumpTime;
}

void LoadLock::SetPumpStableTime(int _value)
{
	m_nPumpStableTime = _value;
}

int LoadLock::GetPumpStableTime() const
{
	return m_nPumpStableTime;
}

void LoadLock::SetVentTime(int _value)
{
	m_nVentTime = _value;
}

int LoadLock::GetVentTime() const
{
	return m_nVentTime;
}

void LoadLock::SetVentStableTime(int _value)
{
	m_nVentStableTime = _value;
}

int LoadLock::GetVentStableTime() const
{
	return m_nVentStableTime;
}

void LoadLock::SetSlotValveOpenTime(int _value)
{
	m_nSlotValveOpenTime = _value;
}

int LoadLock::GetSlotValveOpenTime() const
{
	return m_nSlotValveOpenTime;
}

void LoadLock::SetSlotValveCloseTime(int _value)
{
	m_nSlotValveCloseTime = _value;
}

int LoadLock::GetSlotValveCloseTime() const
{
	return m_nSlotValveCloseTime;
}

void LoadLock::SetDoorValveOpenTime(int _value)
{
	m_nDoorValveOpenTime = _value;
}

int LoadLock::GetDoorValveOpenTime() const
{
	return m_nDoorValveOpenTime;
}

void LoadLock::SetDoorValveCloseTime(int _value)
{
	m_nDoorValveCloseTime = _value;
}

int LoadLock::GetDoorValveCloseTime() const
{
	return m_nDoorValveCloseTime;
}
void LoadLock::SetIsInputWafer(bool _value) 
{
	m_bIsInputWafer = _value;
}
bool LoadLock::GetIsInputWafer() const
{
	return m_bIsInputWafer;
}
#pragma endregion

#pragma region 메서드
void LoadLock::SaveConfigModule(int nIdx, CString strFilePath)
{
	// 기본 ModuleBase의 필드
	CString strIdx, strRow, strCol, strWaferMax;
	strIdx.Format(_T("%d"), nIdx);
	strRow.Format(_T("%d"), m_nRow);
	strCol.Format(_T("%d"), m_nCol);
	strWaferMax.Format(_T("%d"), m_nWaferMax);

	// LoadLock 추가 필드
	CString strPumpTime, strPumpStableTime, strVentTime, strVentStableTime, strSlotOpenTime, strSlotCloseTime, strDoorOpenTime, strDoorCloseTime;

	strPumpTime.Format(_T("%d"), m_nPumpTime);
	strPumpStableTime.Format(_T("%d"), m_nPumpStableTime);
	strVentTime.Format(_T("%d"), m_nVentTime);
	strVentStableTime.Format(_T("%d"), m_nVentStableTime);
	strSlotOpenTime.Format(_T("%d"), m_nSlotValveOpenTime);
	strSlotCloseTime.Format(_T("%d"), m_nSlotValveCloseTime);
	strDoorOpenTime.Format(_T("%d"), m_nDoorValveOpenTime);
	strDoorCloseTime.Format(_T("%d\n"), m_nDoorValveCloseTime);

	WritePrivateProfileString(strIdx, _T("ModuleType"), _T("TYPE_LOADLOCK"), strFilePath);			// 타입
	WritePrivateProfileString(strIdx, _T("ModuleName"), m_strModuleName, strFilePath);				// 모듈명
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
}
void LoadLock::WorkThread()
{
	while (m_bStopFlag == false)
	{	
		if (m_nWaferCount == 0)
		//if(m_nWaferCount < m_nWaferMax)
		{
			if (ModuleBase::s_bDirect == false)
			{
				m_bDoorValveOpen = true;
				m_bSlotValveOpen = false;
			}

			else
			{
				m_bDoorValveOpen = false;
				m_bSlotValveOpen = true;
			}
		}
		
		WaitForSingleObject(m_hLLWaferCntChangeEvent, INFINITE);

		if (m_nWaferCount == m_nWaferMax)
			//|| (LPM::s_nTotalSendWafer == LPM::s_nTotalInitWafer && LPM::s_bLPMWaferPickBlock == false && m_nWaferCount == LPM::s_nTotalInitWafer - LPM::s_nTotalOutputWafer))
		{
			m_bIsWorking = true;
			//순방향일 때, Pump(대기->진공)
			if (ModuleBase::s_bDirect == false)
			{
				m_bDoorValveOpen = false;

				Sleep(m_nDoorValveCloseTime / ModuleBase::s_dSpeed);

				CString tmp = _T("");
				for (int i = 1; i <= 10; i++)
				{
					Sleep(m_nPumpTime / (ModuleBase::s_dSpeed * 10));
					Sleep(m_nPumpStableTime / (ModuleBase::s_dSpeed * 10));

					tmp.Format(_T("%s\n<Pump>\n[%d%%]"), m_strModuleName, i * 10);
					m_pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);
				}

				Sleep(m_nDoorValveOpenTime / ModuleBase::s_dSpeed);

				m_bSlotValveOpen = true;
			}

			//역방향일 때, Vent(진공->대기)
			else
			{
				m_bSlotValveOpen = false;

				Sleep(m_nSlotValveCloseTime / ModuleBase::s_dSpeed);

				CString tmp = _T("");
				for (int i = 1; i <= 10; i++)
				{
					Sleep(m_nVentTime / (ModuleBase::s_dSpeed * 10));
					Sleep(m_nVentStableTime / (ModuleBase::s_dSpeed * 10));

					tmp.Format(_T("%s\n<Vent>\n[%d%%]"), m_strModuleName, i * 10);
					m_pClistCtrl->SetItemText(m_nRow, AXIS - m_nCol, tmp);
				}

				Sleep(m_nSlotValveOpenTime / ModuleBase::s_dSpeed);

				m_bDoorValveOpen = true;

			}

			m_bIsWorking = false;
		}
		ResetEvent(m_hLLWaferCntChangeEvent);
	}
	SetEvent(m_hThreadCloseSignal);
}

void LoadLock::Run(vector<ModuleBase*> vPickModules, vector<ModuleBase*> vPlaceModules, CListCtrl* pClist) //LL <--> EFEM
{
	m_pClistCtrl = pClist;
	m_th = thread(&LoadLock::WorkThread, this);
}
#pragma endregion