#pragma once
#include "ModuleBase.h"

#pragma once
class LPM : public ModuleBase
{
public:
	static int s_nTotalInitWafer;
	static int s_nTotalSendWafer;
	static int s_nTotalUsedDummyWafer;
	static bool s_bLPMWaferPickBlock;

private:
	int m_nOutputWaferCount;

#pragma region ������/�Ҹ���
public:
	LPM(ModuleType _Type, CString _Name, int _WaferCount, int _WaferMax, int _Row, int _Col);
	virtual ~LPM();
#pragma endregion

#pragma region �޼���
private:
	void WorkThread();

public:
	bool SetOutputWaferCount(int _value);
	int GetOutputWaferCount() const;
	virtual void SaveConfigModule(int nIdx, CString strFilePath);

	void Run(vector<ModuleBase*> vPickModules, vector<ModuleBase*> vPlaceModules, CListCtrl* pClist);
#pragma endregion
};

class ATMRobot : public ModuleBase
{
#pragma region �ʵ�
private:
	int m_nPickTime;					// RobotArm���� Wafer�� �������� �ð�
	int m_nPlaceTime;					// RobotArm�� Wafer�� ��� Module�� ���� �ð�
	int m_nRotateTime;					// Robot�� ȸ���ð�
	int m_nRotateZCoordinateTime;		// Robot�� Z�� ȸ�� �ð�
	
	bool m_bIsInputWafer;				// TRUE : FOUP >> PM	FALSE : PM >> FOUP

	vector<ModuleBase*> m_vLPMModule;
	vector<ModuleBase*> m_vLLModule;

	int m_nDummyWaferReminder;

public:
	static HANDLE s_hEventOutputWaferAndUsedDummyWaferChange;
	static HANDLE s_hEventSendWaferChange;

	static int s_nTotalWaferCntFromLPM;
	static int s_nRequiredDummyWaferCntLpmToPM;
	static int s_nRequiredDummyWaferCntPMToLpm;

#pragma endregion
	
#pragma region ������/�Ҹ���
public:
	ATMRobot(ModuleType _Type, CString _Name, int _WaferCount, int _WaferMax, int _Row, int _Col, int _PickTime, int _PlaceTime, int _RotateTime, int RoteteZTime);
	virtual ~ATMRobot();
#pragma endregion
	
#pragma region Get/Set �޼���
public:
	void SetPickTime(int _value);
	int GetPickTime() const;
	void SetPlaceTime(int _value);
	int GetPlaceTime() const;
	void SetRotateTime(int _value);
	int GetRotateTime() const;
	void SetRotateZTime(int _value);
	int GetRotateZTime() const;
	void SetIsInputWafer(bool _value);
	bool GetIsInputWafer() const;
#pragma endregion

#pragma region �޼���
private:
	void WorkThread();
	bool PickWafer(ModuleBase* pM);
	bool PlaceWafer(ModuleBase* pM);

public:
	virtual void SaveConfigModule(int nIdx, CString strFilePath);
	void Run(vector<ModuleBase*> _vPickModules, vector<ModuleBase*> _vPlaceModules, CListCtrl* _pClistCtrl);
#pragma endregion
};