#pragma once
#include "ModuleBase.h"

class VACRobot : public ModuleBase
{
#pragma region 필드
private:
	int m_nPickTime;					// RobotArm으로 Wafer를 가져오는 시간
	int m_nPlaceTime;					// RobotArm의 Wafer를 대상 Module로 놓는 시간
	int m_nRotateTime;					// Robot의 회전시간

public:
	static HANDLE s_hVACRobotExchangeOver;
#pragma endregion

#pragma region 생성자/소멸자
public:
	VACRobot(ModuleType _Type, CString _Name, int _WaferCount, int _WaferMax, int _Row, int _Col, int _PickTime, int _PlaceTime, int _RotateTime);
	~VACRobot();
#pragma endregion

#pragma region Get/Set 메서드
public:
	void SetPickTime(int _value);
	int GetPickTime() const;
	void SetPlaceTime(int _value);
	int GetPlaceTime() const;
	void SetRotateTime(int _value);
	int GetRotateTime() const;
#pragma endregion

#pragma region 메서드
public:
	virtual void SaveConfigModule(int nIdx, CString strFilePath);
	bool PickWafer(ModuleBase* pM, CListCtrl* pClistCtrl);
	bool PlaceWafer(ModuleBase* pM, CListCtrl* pClistCtrl);
	void work(Pick_PlaceM Pick_Place);
	void Run(vector<ModuleBase*> m_vPickModules, vector<ModuleBase*> m_vPlaceModules, CListCtrl* pClist);

#pragma endregion
};