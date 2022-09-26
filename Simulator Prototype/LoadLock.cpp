#include "pch.h"
#include "EFEM.h"
#include "LoadLock.h"

int LoadLock::s_nTotalSendWaferFromLL = 0;

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

	m_hLLWaferCntChangeEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
}

LoadLock::~LoadLock()
{
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
int LoadLock::Pump()
{
	//double result = m_nSlotValveCloseTime + m_nVentTime + m_nVentStableTime + m_nDoorValveOpenTime;			
	// // LL을 진공 >> 대기 상태로 변환하는 시간 (DoorValve 여는 시간 = ATMRobot 회전 시간)
	//return result;

	return 0;
}

int LoadLock::vent()
{
	//double result = m_nDoorValveCloseTime + m_nPumpStableTime + m_nPumpTime + m_nSlotValveOpenTime;			// LL을 대기 >> 진공 상태로 변환하는 시간 (SlotValve 여는 시간 = VACRobot 회전 시간)
	//return result;

	return 0;
}

void LoadLock::work()
{
	while (1)
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

				Sleep(m_nDoorValveCloseTime / SPEED);
				Sleep(m_nPumpTime / SPEED);
				Sleep(m_nPumpStableTime / SPEED);
				Sleep(m_nDoorValveOpenTime / SPEED);

				m_bSlotValveOpen = true;
			}

			//역방향일 때, Vent(진공->대기)
			else
			{
				m_bSlotValveOpen = false;

				Sleep(m_nSlotValveCloseTime / SPEED);
				Sleep(m_nVentTime / SPEED);
				Sleep(m_nVentStableTime / SPEED);
				Sleep(m_nSlotValveOpenTime / SPEED);

				m_bDoorValveOpen = true;

			}

			m_bIsWorking = false;
		}
		ResetEvent(m_hLLWaferCntChangeEvent);
	}
}

void LoadLock::Run() //LL <--> EFEM
{
	m_th = thread(&LoadLock::work, this);
}
#pragma endregion