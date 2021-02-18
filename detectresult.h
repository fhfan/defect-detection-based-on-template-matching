#pragma once
#include "afxcmn.h"


// detectresult 对话框

class detectresult : public CDialogEx
{
	DECLARE_DYNAMIC(detectresult)

public:
	detectresult(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~detectresult();

// 对话框数据
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl dt;
};
extern detectresult odr;
