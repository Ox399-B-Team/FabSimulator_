#pragma once
#include <vector>
#include "Simulator PrototypeDlg.h"
#include "ModuleBase.h"
#include "EFEM.h"
#include "LoadLock.h"
#include "VACRobot.h"
#include "ProcessChamber.h"

#include "CSelCreateModuleDlg.h"

class CFabController
{
#pragma region 필드
public:
	vector<ModuleBase*> m_pModule;

	static vector<LPM*> s_pLPM;
	static vector<ATMRobot*> s_pATMRobot;
	static vector<LoadLock*> s_pLL;
	static vector<VACRobot*> s_pVACRobot;
	static vector<ProcessChamber*> s_pPM;

	vector<ModuleBase*> m_vPickModules;
	vector<ModuleBase*> m_vPlaceModules;

	static HANDLE s_hMoniteringThread1;
	static HANDLE s_hMoniteringThread2;
	CSimulatorPrototypeDlg* m_pMainDlg;

	BOOL m_bRunning;
	int m_nCurModuleIdx;
	
	CString tmp;

#pragma endregion

#pragma region 싱글톤 패턴
private:
	CFabController();
	CFabController(const CFabController& other);
	CFabController& operator=(const CFabController& ref) {};
	~CFabController();
	
public:
	static CFabController& GetInstance()
	{
		// 데이터 영역에 메모리 할당 (늦은 초기화?)
		static CFabController m_cFabController;
		
		return m_cFabController;
	}
	
#pragma endregion

#pragma region 메서드
public:
	int SelectModule(int nRow, int nCol, int& pModuleIdx);
	int SelectModuleCount(ModuleType eType);
	BOOL CompareModuleName(CString strModuleName);
	void DrawModule(bool bEmptyFlag = false);
	BOOL InitUpdateDlg(CDialogEx* pDlg, int nModuleIdx);
	void CreateModule(CDialogEx* pDlg, int nModuleIdx);
	void UpdateModule(CDialogEx* pDlg, int nModuleIdx);
	void DeleteModule(CFabInfoListCtrl* pCtrl, int nModuleIdx);
	thread ClearAllModule();
	void PrintModuleInfo(int nModuleIdx, int nModuleType, int nCurSel);
	void SetFabInfo(int nHour, int nMin, int nSec);
	void SetUnitInfo(int nModuleIdx);
	void SaveConfigFile(CString strFilePath);
	void LoadConfigFile(CString strFilePath);
	void SaveCSVFile(CString strFilePath);
	void ChangeTimeSpeed(int nCurSpeed);
	void RunGraph();
	bool RunModules();
	void SuspendModules();

#pragma endregion
};