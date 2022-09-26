#include "pch.h"
#include "ProcessChamber.h"

#pragma region 생성자/소멸자

int ProcessChamber::s_nCntPMWorkOver = 0;

ProcessChamber::ProcessChamber(ModuleType _Type, CString _Name, int _WaferCount, int _WaferMax, int _Row, int _Col, 
	int _ProcessTime, int _CleanTime, int _SlotOpenTime, int _SlotCloseTime, int _CleanCount)
	: ModuleBase(_Type, _Name, _WaferCount, _WaferMax, _Row, _Col)
{
	m_nProcessTime = _ProcessTime;
	m_nCleanTime = _CleanTime;
	m_nSlotValveOpenTime = _SlotOpenTime;
	m_nSlotValveCloseTime = _SlotCloseTime;
	m_nCleanCount = _CleanCount;
	m_nProcessCount = 0;

	m_hPmWaferCntChangeEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	s_nCntPMWorkOver++;
}

ProcessChamber::~ProcessChamber()
{
}
#pragma endregion

#pragma region Get/Set 메서드
void ProcessChamber::SetProcessTime(int _value)
{
	m_nProcessTime = _value;
}

int ProcessChamber::GetProcessTime() const
{
	return m_nProcessTime;
}

void ProcessChamber::SetCleanTime(int _value)
{
	m_nCleanTime = _value;
}

int ProcessChamber::GetCleanTime() const
{
	return m_nCleanTime;
}

void ProcessChamber::SetCleanCount(int _value)
{
	m_nCleanCount = _value;
}

int ProcessChamber::GetCleanCount() const
{
	return m_nCleanCount;
}

int ProcessChamber::GetProcessCount() const
{
	return m_nProcessCount;
}

void ProcessChamber::SetSlotValveOpenTime(int _value)
{
	m_nSlotValveOpenTime = _value;
}

int ProcessChamber::GetSlotValveOpenTime() const
{
	return m_nSlotValveOpenTime;
}

void ProcessChamber::SetSlotValveCloseTime(int _value)
{
	m_nSlotValveCloseTime = _value;
}

int ProcessChamber::GetSlotValveCloseTime() const
{
	return m_nSlotValveCloseTime;
}
#pragma endregion

#pragma region 메서드
void ProcessChamber::work()
{
	while (1)
	{
		WaitForSingleObject(m_hPmWaferCntChangeEvent, INFINITE);
		if (m_nProcessCount > 0 && m_nProcessCount % m_nCleanCount == 0 && m_nWaferCount == 0)
		{
			/*int nInitWaferMax = m_nWaferMax;
			m_nWaferMax = 0;*/

			m_bIsWorking = true;
			
			Sleep(m_nSlotValveCloseTime / SPEED);

			Sleep(m_nCleanTime / SPEED);

			Sleep(m_nSlotValveOpenTime / SPEED);

			//m_bIsWorking = false;

			//m_nWaferMax = nInitWaferMax;
		}
		//ResetEvent(m_hPmWaferCntMinusEvent);

		//WaitForSingleObject(m_hPmWaferCntPlusEvent, INFINITE);
		if (m_nWaferCount == m_nWaferMax)
		{
			s_nCntPMWorkOver--;
			m_bIsWorking = true;

			Sleep(m_nSlotValveCloseTime / SPEED);

			Sleep(m_nProcessTime / SPEED);

			Sleep(m_nSlotValveOpenTime / SPEED);

			s_nCntPMWorkOver++;
			m_nProcessCount++;

		}

		m_bIsWorking = false;
		ResetEvent(m_hPmWaferCntChangeEvent);

	}
}

void ProcessChamber::Run()
{
	m_th = thread(&ProcessChamber::work, this);
}
#pragma endregion