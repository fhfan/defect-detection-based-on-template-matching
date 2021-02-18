#include "stdafx.h"
#include "ADO.h"


// ADO.cpp: implementation of the ADO class.
//
//////////////////////////////////////////////////////////////////////
#define MAX_FILE_LEN 300


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ADO::ADO()
{
	////读取数据库的路径
	////===============
	//CFile file;

	//file.Open(_T("data.txt"), CFile::modeRead);
	//int len = min(file.GetLength(), MAX_FILE_LEN);
	//char buf[MAX_FILE_LEN] = { 0 };
	//file.Read(buf, len);

	////提取文件的结构
	//CString st;
	//st.Format(_T("%s"), buf);

	//filename = st.Mid(3);
	////filename.Format("%s",buf);
	//file.Close();


}

ADO::~ADO()
{

}


void ADO::OnInitADOConn()
{
	CoInitialize(NULL);
	m_pConnection = _ConnectionPtr(__uuidof(Connection));
	m_pConnection->ConnectionString = "Provider=Microsoft.ACE.OLEDB.12.0; Data Source=New.accdb;";
	m_pConnection->Open("", "", "", adConnectUnspecified);


}


_RecordsetPtr&  ADO::OpenRecordset(CString sql)
{
	ASSERT(!sql.IsEmpty());											//SQL语句不能为空
	try
	{
		m_pRecordset.CreateInstance(__uuidof(Recordset));			//创建记录集对象实例
		m_pRecordset->Open(_bstr_t(sql), m_pConnection.GetInterfacePtr(),
			adOpenDynamic, adLockOptimistic, adCmdText); 			//执行SQL得到记录集
	}
	catch (_com_error e)											//捕获可能的异常
	{
		AfxMessageBox(e.Description());
	}
	return m_pRecordset;
}

void ADO::CloseRecordset()
{
	if (m_pRecordset->GetState() == adStateOpen)					//判断当前的记录集状态
		m_pRecordset->Close();										//关闭记录集
}

void ADO::CloseConn()
{
	m_pConnection->Close();											//关闭数据库连接
	::CoUninitialize();												//释放COM环境
}

UINT ADO::GetRecordCount(_RecordsetPtr pRecordset)
{
	int nCount = 0;													//声明保存记录数的变量
	try{
		pRecordset->MoveFirst();									//将记录集指针移动到第一条记录
	}
	catch (...)														//捕捉可能出现的错误
	{
		return 0;													//产生错误时返回0
	}
	if (pRecordset->adoEOF)											//判断记录集中是否没有记录
		return 0;													//无记录时返回0
	while (!pRecordset->adoEOF)										//当记录集指针没有指向最后时
	{
		pRecordset->MoveNext();										//将记录集指针移动到下一条记录
		nCount = nCount + 1;										//记录个数的变量加1
	}
	pRecordset->MoveFirst();										//将记录集指针移动到第一条记录
	return nCount;													//返回记录数
}




void ADO::CreateNew(CString filename)
{
	_variant_t RecordsAffected;
	CString sqlstringconnection;
	sqlstringconnection.Format(_T("CREATE TABLE %s(编号 INTEGER,流量 FLOAT,能量 FLOAT,效率 FLOAT, 压力 FLOAT)"), filename);
	_bstr_t sq = sqlstringconnection;
	//m_pConnection->Execute(sqlstringconnection,&RecordsAffected,adCmdText);  
	m_pConnection->Execute(sq, &RecordsAffected, adCmdText);
	//m_pConnection->Execute("CREATE TABLE new(编号 INTEGER,流量系数 FLOAT,能量头系数 FLOAT,多变内效率 FLOAT, 压比 FLOAT)",&RecordsAffected,adCmdText);  

}



void ADO::OnADOConn(CString s)
{
	::CoInitialize(NULL);			//创建COM运行环境

	CString strname;
	/////////////////////////////////////////////////
	///////////////////////////////////////////////////
	strname.Format(_T("Provider=Microsoft.ACE.OLEDB.12.0; Data Source=%s"), s);

	//strname.Format(_T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=%s" ), s);
	//m_pConnection->Open("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=MyAccess.mdb", "", "", adModeUnknown);

	m_pConnection = _ConnectionPtr(__uuidof(Connection));


	try
	{
		//::MessageBox(NULL, _T("执行1"), _T(" "), MB_OK);

		m_pConnection->ConnectionString = (_bstr_t)strname;
		m_pConnection->Open("", "", "", adConnectUnspecified);
	}
	catch (_com_error e)
	{
		//MessageBox(NULL, _T("执行2"), _T(" "), MB_OK);
		AfxMessageBox(e.Description()); //弹出错误处理
	}

}



void ADO::setfilename(CString datafile)
{
	filename = datafile;

}
