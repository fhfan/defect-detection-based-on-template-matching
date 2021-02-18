// detectresult.cpp : 实现文件
//

#include "stdafx.h"
#include "ROI.h"
#include "detectresult.h"
#include "afxdialogex.h"
#include "ADO.h"

detectresult odr;
ADO m_Ado1;

// detectresult 对话框

IMPLEMENT_DYNAMIC(detectresult, CDialogEx)

detectresult::detectresult(CWnd* pParent /*=NULL*/)
	: CDialogEx(detectresult::IDD, pParent)
{

}

detectresult::~detectresult()
{
}

void detectresult::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, dt);
}


BEGIN_MESSAGE_MAP(detectresult, CDialogEx)
END_MESSAGE_MAP()


// detectresult 消息处理程序


BOOL detectresult::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CRect rect;

	// 获取检测结果列表视图控件的位置和大小   
	odr.dt.GetClientRect(&rect);

	// 为列表视图控件添加8列   
	odr.dt.InsertColumn(0, _T("检测日期"), LVCFMT_CENTER, rect.Width() / 8, 0);
	odr.dt.InsertColumn(1, _T("检测数"), LVCFMT_CENTER, rect.Width() / 8, 1);
	odr.dt.InsertColumn(2, _T("正常数"), LVCFMT_CENTER, rect.Width() / 8, 2);
	odr.dt.InsertColumn(3, _T("缺陷数"), LVCFMT_CENTER, rect.Width() / 8, 3);
	odr.dt.InsertColumn(4, _T("未匹配数"), LVCFMT_CENTER, rect.Width() / 8, 4);
	odr.dt.InsertColumn(5, _T("正常数占比"), LVCFMT_CENTER, rect.Width() / 8, 5);
	odr.dt.InsertColumn(6, _T("缺陷数占比"), LVCFMT_CENTER, rect.Width() / 8, 6);
	odr.dt.InsertColumn(7, _T("未匹配数占比"), LVCFMT_CENTER, rect.Width() / 8, 7);

	UpdateData(true);
	CString strTime;
	CString totalnum;
	CString normalnum;
	CString defectnum;
	CString whitenum;
	CString zb;
	CString qb;
	CString nb;
	int idx = 0;
	try
	{
		CString search_sql;
		search_sql.Format(_T("select * from detectInfo"));
		m_Ado1.m_pRecordset = m_Ado1.OpenRecordset(search_sql);
		while (!m_Ado1.m_pRecordset->adoEOF)
		{
			strTime = m_Ado1.m_pRecordset->GetCollect("检测日期").bstrVal;
			totalnum = m_Ado1.m_pRecordset->GetCollect("检测数").bstrVal;
			normalnum = m_Ado1.m_pRecordset->GetCollect("正常数").bstrVal;
			defectnum = m_Ado1.m_pRecordset->GetCollect("缺陷数").bstrVal;
			whitenum = m_Ado1.m_pRecordset->GetCollect("未匹配数").bstrVal;
			zb = m_Ado1.m_pRecordset->GetCollect("正常数占比").bstrVal;
			qb = m_Ado1.m_pRecordset->GetCollect("缺陷数占比").bstrVal;
			nb = m_Ado1.m_pRecordset->GetCollect("未匹配数占比").bstrVal;
			odr.dt.InsertItem(idx, strTime);
			odr.dt.SetItemText(idx, 1, totalnum);
			odr.dt.SetItemText(idx, 2, normalnum);
			odr.dt.SetItemText(idx, 3, defectnum);
			odr.dt.SetItemText(idx, 4, whitenum);
			odr.dt.SetItemText(idx, 5, zb);
			odr.dt.SetItemText(idx, 6, qb);
			odr.dt.SetItemText(idx, 7, nb);
			idx++;
			m_Ado1.m_pRecordset->MoveNext();
		}
		m_Ado1.m_pRecordset->Close();
	}
	catch (_com_error e)
	{
		AfxMessageBox(_T("搜索失败！"));
		return TRUE;
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}
