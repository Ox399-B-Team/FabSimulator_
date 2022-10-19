#pragma once
#include "ModuleBase.h"

class LoadLock : public ModuleBase
{
#pragma region 필드
private:
	int m_nPumpTime;				// 대기 >> 진공 상태 전환 시간
	int m_nPumpStableTime;			// 진공 상태 전환 전/후 소요 시간
	int m_nVentTime;				// 진공 >> 대기 상태 전환 시간
	int m_nVentStableTime;			// 대기 상태 전환 전/후 소요 시간
	int m_nSlotValveOpenTime;		// TM 연결 VALVE 여는 시간
	int m_nSlotValveCloseTime;		// TM 연결 VALVE 닫는 시간
	int m_nDoorValveOpenTime;		// EFEM 연결 VALVE 여는 시간
	int m_nDoorValveCloseTime;		// EFEM 연결 VALVE 닫는 시간

	bool m_bIsInputWafer;			// TRUE : FOUP >> PM	FALSE : PM >> FOUP

public: 
	HANDLE m_hLLWaferCntChangeEvent;
	static int s_nTotalSendWaferFromLL;
	static int s_nRequiredDummyWaferCntLpmToPM;
	static int	s_nRequiredDummyWaferCntPMToLpm;
	static int s_nCount;
	static vector<LoadLock*> s_pLL;

#pragma endregion
	
#pragma region 생성자/소멸자
public:
	LoadLock(ModuleType _Type, CString _Name, int _WaferCount, int _WaferMax, int _Row, int _Col, int _PumpTime, int _PumpStableTime, int _VentTime, int _VentStableTime,
		int _SlotOpenTime, int _SlotCloseTime, int _DoorOpenTime, int _DoorCloseTime);
	virtual ~LoadLock();
#pragma endregion

#pragma region Get/Set 메서드
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

#pragma region 메서드
private:
	void WorkThread();
public:
	virtual void SaveConfigModule(int nIdx, CString strFilePath);
	void Run(vector<ModuleBase*> vPickModules, vector<ModuleBase*> vPlaceModules, CListCtrl* pClist);
#pragma endregion
};