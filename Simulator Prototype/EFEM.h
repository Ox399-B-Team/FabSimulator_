#pragma once
#include "ModuleBase.h"

#pragma once
class LPM : public ModuleBase
{
public:
	static int s_nTotalInitWafer;
	static int s_nTotalSendWafer;
	static bool s_bLPMWaferPickBlock;
	static int s_nTotalOutputWafer;
	int m_nOutputWafer;

#pragma region 생성자/소멸자
public:
	LPM(ModuleType _Type, CString _Name, int _WaferCount, int _WaferMax, int _Row, int _Col);
	~LPM();
#pragma endregion

#pragma region 메서드
	bool SetOutputWafer(int _value);
	void Run();
	void work();
#pragma endregion
};

class ATMRobot : public ModuleBase
{
#pragma region 필드
private:
	int m_nPickTime;					// RobotArm으로 Wafer를 가져오는 시간
	int m_nPlaceTime;					// RobotArm의 Wafer를 대상 Module로 놓는 시간
	int m_nRotateTime;					// Robot의 회전시간
	int m_nRotateZCoordinateTime;		// Robot의 Z축 회전 시간
	
	bool m_bIsInputWafer;				// TRUE : FOUP >> PM	FALSE : PM >> FOUP

public:
	static HANDLE s_hEventBlockATMRobot;
#pragma endregion
	
#pragma region 생성자/소멸자
public:
	ATMRobot(ModuleType _Type, CString _Name, int _WaferCount, int _WaferMax, int _Row, int _Col, int _PickTime, int _PlaceTime, int _RotateTime, int RoteteZTime);
	~ATMRobot();
#pragma endregion
	
#pragma region Get/Set 메서드
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

#pragma region 메서드
public:
	void Pick();
	void Place();
	void Rotate();
	bool PickWafer(ModuleBase* pM, CListCtrl* pClistCtrl);
	bool PlaceWafer(ModuleBase* pM, CListCtrl* pClistCtrl);
	void work(Pick_PlaceM Pick_Place);
	void Run(vector<ModuleBase*> m_vPickModules, vector<ModuleBase*> m_vPlaceModules, CListCtrl* pClistCtrl);
#pragma endregion
};