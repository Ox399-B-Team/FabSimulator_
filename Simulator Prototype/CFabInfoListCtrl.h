#pragma once
#include <afxcmn.h>
class CFabInfoListCtrl : public CListCtrl
{
#pragma region 필드
public:
	int m_nCurRow;
	int m_nCurCol;
	CString m_strModuleName;
	
	
#pragma endregion

#pragma region 생성자/소멸자
public:
	CFabInfoListCtrl();
	~CFabInfoListCtrl();
#pragma endregion

#pragma region 메서드
public:
	BOOL InitListCtrl();
	DECLARE_MESSAGE_MAP()

#pragma endregion
	afx_msg void OnNMClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMCustomdraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMRClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMenuChangemodule();
	afx_msg void OnMenuDeletemodule();
};