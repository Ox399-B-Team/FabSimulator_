#include "pch.h"
#include "ModuleBase.h"
#include "CFabController.h"

bool ModuleBase::s_bDirect = false;

ModuleBase::ModuleBase(ModuleType _Type, CString _Name, int _WaferCount, int _WaferMax, int _Row, int _Col )
{
	m_eModuleType = _Type;
	m_strModuleName = _Name;
	m_nRow = _Row;
	m_nCol = _Col;
	m_nWaferCount = _WaferCount;
	m_nWaferMax = _WaferMax;

	m_hMutex = CreateMutex(NULL, FALSE, NULL);
	m_bIsWorking = false;
}

ModuleBase::~ModuleBase()
{
	CloseHandle(m_hMutex);

	// 에러처리 필요 **
	if (m_th.joinable())
	{
		m_th.join();
	}
	else
	{

	}
}

void ModuleBase::SetModuleName(CString _value)
{
	m_strModuleName = _value;
}

CString ModuleBase::GetModuleName() const
{
	return m_strModuleName;
}

bool ModuleBase::SetWaferCount(int _value)
{
	if (m_nWaferCount > m_nWaferMax || _value > m_nWaferMax || _value < 0) // WaferMax 수치 제한
	{
		return false;
	}
	m_nWaferCount = _value;
	return true;
}

int ModuleBase::GetWaferCount() const
{
	return m_nWaferCount;
}

void ModuleBase::SetWaferMax(int _value)
{
	m_nWaferMax = _value;
}

int ModuleBase::GetWaferMax() const
{
	return m_nWaferMax;
}

bool ModuleBase::GetIsWorking() const
{
	return m_bIsWorking;
}

bool ModuleBase::SetIsWorking(bool _value)
{
	// bool 반환 필요? == 나중에 설명 필요
	m_bIsWorking = _value;
	return true;
}

void ModuleBase::Resume()
{
	if (m_th.joinable())
	{
		ResumeThread(m_th.native_handle());
	}
}

void ModuleBase::Suspend()
{
	if (m_th.joinable())
	{
		SuspendThread(m_th.native_handle());
	}
}

bool ModuleBase::IsRunning()
{
	if (m_th.joinable())
	{
		return true;
	}
	else
	{
		return false;
	}
}

HANDLE ModuleBase::hThread()
{
	if (m_th.joinable())
		return m_th.native_handle();
}

bool ModuleBase::GetSlotValveOpen() const
{
	return m_bSlotValveOpen;
}

bool ModuleBase::GetDoorValveOpen() const
{
	return m_bDoorValveOpen;
}