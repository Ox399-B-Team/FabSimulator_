#pragma once
#include "ModuleBase.h"

#pragma once
class LPM : public ModuleBase
{
public:
	static int s_nTotalInitWafer;
	static int s_nTotalSendWafer;
	static bool s_bLPMWaferPickBlock;


private:
	int m_nOutputWaferCount;

#pragma region ������/�Ҹ���
public:
	LPM(ModuleType _Type, CString _Name, int _WaferCount, int _WaferMax, int _Row, int _Col);
	~LPM();
#pragma endregion

#pragma region �޼���
	bool SetOutputWaferCount(int _value);
	int GetOutputWaferCount() const;
	virtual void SaveConfigModule(int nIdx, CString strFilePath);

	void Run();
	void work();
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

public:
	static HANDLE s_hEventOutputWaferChange;
	static HANDLE s_hEventSendWaferChange;

	static int s_nTotalWaferCntFromLPM;

#pragma endregion
	
#pragma region ������/�Ҹ���
public:
	ATMRobot(ModuleType _Type, CString _Name, int _WaferCount, int _WaferMax, int _Row, int _Col, int _PickTime, int _PlaceTime, int _RotateTime, int RoteteZTime);
	~ATMRobot();
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
public:
	virtual void SaveConfigModule(int nIdx, CString strFilePath);
	bool PickWafer(ModuleBase* pM, CListCtrl* pClistCtrl);
	bool PlaceWafer(ModuleBase* pM, CListCtrl* pClistCtrl);
	void work(Pick_PlaceM Pick_Place);
	void Run(vector<ModuleBase*> m_vPickModules, vector<ModuleBase*> m_vPlaceModules, CListCtrl* pClistCtrl);
#pragma endregion
};