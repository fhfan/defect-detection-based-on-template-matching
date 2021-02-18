#pragma once
// ADO.h: interface for the ADO class.
//
//////////////////////////////////////////////////////////////////////
#include"stdafx.h"

class ADO
{
public:

	_ConnectionPtr m_pConnection;					//连接对象指针
	_RecordsetPtr  m_pRecordset;					//记录集对象指针

public:
	CString filename;
	void setfilename(CString datafile);
	void OnADOConn(CString s);
	void CreateNew(CString filename);
	ADO();
	virtual ~ADO();
	void OnInitADOConn();							//连接数据库
	_RecordsetPtr&  OpenRecordset(CString sql);		//打开记录集
	void CloseRecordset();							//关闭记录集
	void CloseConn();								//关闭数据库连接
	UINT GetRecordCount(_RecordsetPtr pRecordset);	//获得记录数	
};

extern ADO m_Ado;
extern ADO m_Ado1;
