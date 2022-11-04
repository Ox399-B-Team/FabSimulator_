#pragma once
#include "ModuleBase.h"

class LoadLock : public ModuleBase
{
#pragma region �ʵ�
private:
	int m_nPumpTime;				// ��� >> ���� ���� ��ȯ �ð�
	int m_nPumpStableTime;			// ���� ���� ��ȯ ��/�� �ҿ� �ð�
	int m_nVentTime;				// ���� >> ��� ���� ��ȯ �ð�
	int m_nVentStableTime;			// ��� ���� ��ȯ ��/�� �ҿ� �ð�
	int m_nSlotValveOpenTime;		// TM ���� VALVE ���� �ð�
	int m_nSlotValveCloseTime;		// TM ���� VALVE �ݴ� �ð�
	int m_nDoorValveOpenTime;		// EFEM ���� VALVE ���� �ð�
	int m_nDoorValveCloseTime;		// EFEM ���� VALVE �ݴ� �ð�

	bool m_bIsInputWafer;			// TRUE : FOUP >> PM	FALSE : PM >> FOUP

public: 
	HANDLE m_hLLWaferCntChangeEvent;
	static int s_nTotalSendWaferFromLL;
	static int s_nRequiredDummyWaferCntLpmToPM;
	static int	s_nRequiredDummyWaferCntPMToLpm;
	static int s_nCount;
	static vector<LoadLock*> s_pLL;

#pragma endregion
	
#pragma region ������/�Ҹ���
public:
	LoadLock(ModuleType _Type, CString _Name, int _WaferCount, int _WaferMax, int _Row, int _Col, int _PumpTime, int _PumpStableTime, int _VentTime, int _VentStableTime,
		int _SlotOpenTime, int _SlotCloseTime, int _DoorOpenTime, int _DoorCloseTime);
	virtual ~LoadLock();
#pragma endregion

#pragma region Get/Set �޼���
public:
	void SetPumpTime(int _value);
	int GetPumpTime() const;
	void SetPumpStableTime(int _value);
	int GetPumpStableTime() const;
	void SetVentTime(int _value);
	int GetVentTime() const;
	void SetVentStableTime(int _value);
	int GetVentStableTime() const;
	void SetSlotValveOpenTime(int _value);
	int GetSlotValveOpenTime() const;
	void SetSlotValveCloseTime(int _value);
	int GetSlotValveCloseTime() const;
	void SetDoorValveOpenTime(int _value);
	int GetDoorValveOpenTime() const;
	void SetDoorValveCloseTime(int _value);
	int GetDoorValveCloseTime() const;
	void SetIsInputWafer(bool _value);
	bool GetIsInputWafer() const;
#pragma endregion

#pragma region �޼���
private:
	void WorkThread();
public:
	virtual void SaveConfigModule(int nIdx, CString strFilePath);
	void Run(vector<ModuleBase*> vPickModules, vector<ModuleBase*> vPlaceModules, CListCtrl* pClist);
#pragma endregion
};