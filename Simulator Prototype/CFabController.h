#pragma once
#include <vector>
#include "Simulator PrototypeDlg.h"
#include "ModuleBase.h"
#include "CSelCreateModuleDlg.h"

class CFabController
{
#pragma region 필드
public:
	vector<ModuleBase*> m_pModule;

	static vector<ModuleBase*> s_pLPM;
	static vector<ModuleBase*> s_pATMRobot;
	static vector<ModuleBase*> s_pLL;
	static vector<ModuleBase*> s_pVACRobot;
	static vector<ModuleBase*> s_pPM;

	vector<ModuleBase*> m_vPickModules;
	vector<ModuleBase*> m_vPlaceModules;

	static bool s_bAllWorkOver;
	static vector<HANDLE> s_vhMoniteringThreads;
	CSimulatorPrototypeDlg* m_pMainDlg;
	
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
	void DrawModule();
	BOOL InitUpdateDlg(CDialogEx* pDlg, int nModuleIdx);
	void CreateModule(CDialogEx* pDlg, int nModuleIdx);
	void UpdateModule(CDialogEx* pDlg, int nModuleIdx);
	void DeleteModule(CFabInfoListCtrl* pCtrl, int nModuleIdx);
	void ClearAllModule();
	void PrintModuleInfo(int nModuleIdx, int nModuleType, int nCurSel);
	void SaveConfigFile(CString strFilePath);
	void LoadConfigFile(CString strFilePath);
	void RunModules();
	void SuspendModules();
#pragma endregion
};