
// ROIDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ROI.h"
#include "ROIDlg.h"
#include "afxdialogex.h"
#include <io.h>
#include <fcntl.h>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "ADO.h"
#include "detectresult.h"
#include "time.h"


using namespace std;
using namespace cv;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

ADO m_Ado;


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CROIDlg 对话框



CROIDlg::CROIDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CROIDlg::IDD, pParent)
	, minareamin(300)
	, minareamax(400)
	, minareastep(100)
	, maxareamin(50000)
	, maxareamax(60000)
	, maxareastep(10000)
	, absthresmin(20)
	, absthresmax(21)
	, absthresstep(1)
	, varthresmin(5)
	, varthresmax(6)
	, varthresstep(1)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CROIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, minareamin);
	DDX_Text(pDX, IDC_EDIT2, minareamax);
	DDX_Text(pDX, IDC_EDIT3, minareastep);
	DDX_Text(pDX, IDC_EDIT4, maxareamin);
	DDX_Text(pDX, IDC_EDIT5, maxareamax);
	DDX_Text(pDX, IDC_EDIT6, maxareastep);
	DDX_Text(pDX, IDC_EDIT7, absthresmin);
	DDX_Text(pDX, IDC_EDIT8, absthresmax);
	DDX_Text(pDX, IDC_EDIT9, absthresstep);
	DDX_Text(pDX, IDC_EDIT10, varthresmin);
	DDX_Text(pDX, IDC_EDIT11, varthresmax);
	DDX_Text(pDX, IDC_EDIT12, varthresstep);
	DDX_Control(pDX, IDC_COMBO1, combobox);
}

BEGIN_MESSAGE_MAP(CROIDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CROIDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CROIDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CROIDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CROIDlg::OnBnClickedButton4)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CROIDlg::OnCbnSelchangeCombo1)
	ON_BN_CLICKED(IDC_BUTTON5, &CROIDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CROIDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CROIDlg::OnBnClickedButton7)
END_MESSAGE_MAP()


// CROIDlg 消息处理程序

BOOL CROIDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	CreateImageWindow();
	InitConsoleWindow();
	combobox.AddString(_T("轴平行矩形"));
	combobox.AddString(_T("旋转矩形"));
	combobox.AddString(_T("圆形"));
	combobox.AddString(_T("椭圆"));
	::CoInitialize(NULL);
	try{

		m_Ado1.m_pConnection.CreateInstance("ADODB.Connection");  //创建连接对象实例		
		_bstr_t strConnect = "Provider=Microsoft.ACE.OLEDB.12.0;Data Source=test.mdb;";//连接字符串		
		m_Ado1.m_pConnection->Open(strConnect, "", "", adModeUnknown); //打开数据库
	}
	catch (_com_error e){

		AfxMessageBox(e.Description());
		return FALSE;
	}
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CROIDlg::InitConsoleWindow()
{
	AllocConsole();
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	int hCrt = _open_osfhandle((long)handle, _O_TEXT);
	FILE *hf = _fdopen(hCrt, "w");
	*stdout = *hf;
}

HTuple hvWindowID;
void CROIDlg::CreateImageWindow()
{
	HTuple HWindowID;
	CRect Rect;
	CWnd* pWnd = GetDlgItem(IDC_PIC);
	HWindowID = (Hlong)pWnd->m_hWnd;
	pWnd->GetWindowRect(&Rect);
	OpenWindow(0, 0, Rect.Width(), Rect.Height(), HWindowID, "visible", "", &hvWindowID);
}

void CROIDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CROIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CROIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


Mat HObject2Mat(HObject Hobj)
{
	Mat Image;
	HTuple htCh;

	HString cType;

	ConvertImageType(Hobj, &Hobj, "byte");

	CountChannels(Hobj, &htCh);

	Hlong wid = 0;

	Hlong hgt = 0;

	if (htCh[0].I() == 1)

	{

		HImage hImg(Hobj);

		void *ptr = hImg.GetImagePointer1(&cType, &wid, &hgt);//GetImagePointer1(Hobj, &ptr, &cType, &wid, &hgt);

		int W = (HTuple)wid;

		int H = (HTuple)hgt;

		Image.create(H, W, CV_8UC1);

		unsigned char *pdata = static_cast<unsigned char *>(ptr);

		memcpy(Image.data, pdata, W*H);

	}

	else if (htCh[0].I() == 3)

	{

		void *Rptr;

		void *Gptr;

		void *Bptr;

		HImage hImg(Hobj);

		hImg.GetImagePointer3(&Rptr, &Gptr, &Bptr, &cType, &wid, &hgt);

		int W = (HTuple)wid;

		int H = (HTuple)hgt;

		Image.create(H, W, CV_8UC3);

		vector<cv::Mat> VecM(3);

		VecM[0].create(H, W, CV_8UC1);

		VecM[1].create(H, W, CV_8UC1);

		VecM[2].create(H, W, CV_8UC1);

		unsigned char *R = (unsigned char *)Rptr;

		unsigned char *G = (unsigned char *)Gptr;

		unsigned char *B = (unsigned char *)Bptr;

		memcpy(VecM[2].data, R, W*H);

		memcpy(VecM[1].data, G, W*H);

		memcpy(VecM[0].data, B, W*H);

		merge(VecM, Image);

	}
	return Image;
}


bool j = true;
HObject hoImage;
Mat srcImage;
void CROIDlg::OnBnClickedButton1()
{
	// TODO:  在此添加控件通知处理程序代码
	HTuple hv_Files, m_ImageWidth, m_ImageHeight, hv_ImageOKDir = "./训练";
	j = true;
	ReadImage(&hoImage, (hv_ImageOKDir + "/") + "3");
	GetImagePointer1(hoImage, NULL, NULL, &m_ImageWidth, &m_ImageHeight);
	SetPart(hvWindowID, 0, 0, m_ImageHeight - 1, m_ImageWidth - 1);
	HDevWindowStack::Push(hvWindowID);
	if (HDevWindowStack::IsOpen())
		DispObj(hoImage, HDevWindowStack::GetActive());
	srcImage = HObject2Mat(hoImage);
}

HObject ho_RegionROI;
CString name;
void CROIDlg::OnBnClickedButton2()
{
	// TODO:  在此添加控件通知处理程序代码
	HTuple hv_Row1, hv_Column1, hv_Row2, hv_Column2, Radius1, Radius2, Phi, Length1, Length2;
	HObject ho_WindowROI, ho_RegionLines1, ho_RegionLines2, ho_RegionLines3, ho_RegionLines4;
	//if (HDevWindowStack::IsOpen())
	SetColor(HDevWindowStack::GetActive(), "red");
	if (name == "轴平行矩形")
	{
		DrawRectangle1(hvWindowID, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);
		//hv_Row1 = 0; hv_Column1 = 0; hv_Row2 = 1276; hv_Column2 = 443;
		GenRectangle1(&ho_WindowROI, hv_Row1, hv_Column1, hv_Row2, hv_Column2);
		//DispRectangle1(hvWindowID, hv_Row1, hv_Column1, hv_Row2, hv_Column2);
		GenRegionLine(&ho_RegionLines1, hv_Row1, hv_Column1, hv_Row1, hv_Column2);
		GenRegionLine(&ho_RegionLines2, hv_Row1, hv_Column2, hv_Row2, hv_Column2);
		GenRegionLine(&ho_RegionLines3, hv_Row2, hv_Column2, hv_Row2, hv_Column1);
		GenRegionLine(&ho_RegionLines4, hv_Row2, hv_Column1, hv_Row1, hv_Column1);
		HDevWindowStack::Push(hvWindowID);
		if (HDevWindowStack::IsOpen())
		{
			DispObj(ho_RegionLines1, HDevWindowStack::GetActive());
			DispObj(ho_RegionLines2, HDevWindowStack::GetActive());
			DispObj(ho_RegionLines3, HDevWindowStack::GetActive());
			DispObj(ho_RegionLines4, HDevWindowStack::GetActive());
		}
		Rect rect(hv_Column1.D(), hv_Row1.D(), hv_Column2.D() - hv_Column1.D(), hv_Row2.D() - hv_Row1.D());
		imwrite(".//fittinglines.jpg", srcImage(rect));
	}
	else if (name == "旋转矩形")
	{
		DrawRectangle2(hvWindowID, &hv_Row1, &hv_Column1, &Phi, &Length1, &Length2);
		GenRectangle2(&ho_WindowROI, hv_Row1, hv_Column1, Phi, Length1, Length2);
		DispRectangle2(hvWindowID, hv_Row1, hv_Column1, Phi, Length1, Length2);
	}
	else if (name == "圆形")
	{
		DrawCircle(hvWindowID, &hv_Row1, &hv_Column1, &Radius1);
		GenCircle(&ho_WindowROI, hv_Row1, hv_Column1, Radius1);
		DispCircle(hvWindowID, hv_Row1, hv_Column1, Radius1);
	}
	else
	{
		DrawEllipse(hvWindowID, &hv_Row1, &hv_Column1, &Phi, &Radius1, &Radius2);
		GenEllipse(&ho_WindowROI, hv_Row1, hv_Column1, Phi, Radius1, Radius2);
		DispEllipse(hvWindowID, hv_Row1, hv_Column1, Phi, Radius1, Radius2);
	}
	//cout << j << endl;
	if (j == true)
	{
		ho_RegionROI = ho_WindowROI;
		j = false;
	}
	else
		Difference(ho_RegionROI, ho_WindowROI, &ho_RegionROI);
}

int r;
void training(HTuple hv_ImagetrainDir, HTuple trainfile, HTuple trainfile1, HTuple *hv_ShapeModelID, HTuple *hv_VariationModelID, HTuple hv_FindOffset)
{
	// Local iconic variables
	HObject  ho_Image, ho_ImageReduced;
	HObject  ho_ModelImages, ho_ModelRegions, ho_Model;
	HObject  ho_ImageTrans, ho_MeanImage, ho_VarImage;

	// Local control variables
	//HTuple hv_VariationModelID;
	HTuple  hv_Width, hv_Height, hv_WindowHandle;
	HTuple  hv_Area, hv_RowRef, hv_ColumnRef;
	HTuple  hv_NumOKImages;
	HTuple  hv_I, hv_Row, hv_Column, hv_Angle, hv_Score, hv_HomMat2D;

	r = 1;
	ListFiles(hv_ImagetrainDir, (HTuple("files").Append("follow_links")), &hv_NumOKImages);
	TupleRegexpSelect(hv_NumOKImages, (HTuple("\\.(tif|tiff|gif|bmp|jpg|jpeg|jp2|png|pcx|pgm|ppm|pbm|xwd|ima|hobj)$").Append("ignore_case")),
		&hv_NumOKImages);
	ReadImage(&ho_Image, HTuple(hv_NumOKImages[0]));
	GetImageSize(ho_Image, &hv_Width, &hv_Height);
	ReduceDomain(ho_Image, ho_RegionROI, &ho_ImageReduced);
	InspectShapeModel(ho_ImageReduced, &ho_ModelImages, &ho_ModelRegions, 1, 20);
	GenContoursSkeletonXld(ho_ModelRegions, &ho_Model, 1, "filter");
	AreaCenter(ho_RegionROI, &hv_Area, &hv_RowRef, &hv_ColumnRef);
	/*CreateShapeModel(ho_ModelImages, 8, HTuple(0).TupleRad(), HTuple(360).TupleRad(),
		HTuple(0.0739).TupleRad(), (HTuple("point_reduction_high").Append("no_pregeneration")),
		"use_polarity", ((HTuple(19).Append(39)).Append(141)), 3, hv_ShapeModelID);*/
	CreateShapeModel(ho_ModelImages, "auto", -0.39, 0.79, "auto", "auto", "use_polarity",
		"auto", "auto", hv_ShapeModelID);
	CreateVariationModel(hv_Width, hv_Height, "byte", "standard", hv_VariationModelID);
	HTuple  hv_RegionRow1, hv_RegionCloumn1, hv_RegionRow2, hv_RegionCloumn2;
	SmallestRectangle1(ho_RegionROI, &hv_RegionRow1, &hv_RegionCloumn1, &hv_RegionRow2, &hv_RegionCloumn2);
	HObject ho_FindROI;
	GenRectangle1(&ho_FindROI, hv_RegionRow1 - hv_FindOffset, hv_RegionCloumn1 - hv_FindOffset, hv_RegionRow2 + hv_FindOffset,
		hv_RegionCloumn2 + hv_FindOffset);
	{
		HTuple end_val38 = (hv_NumOKImages.TupleLength()) - 1;
		HTuple step_val38 = 1;
		for (hv_I = 0; hv_I.Continue(end_val38, step_val38); hv_I += step_val38)
		{
			ReadImage(&ho_Image, HTuple(hv_NumOKImages[hv_I]));
			ReduceDomain(ho_Image, ho_FindROI, &ho_ImageReduced);
			/*FindShapeModel(ho_ImageReduced, *hv_ShapeModelID, HTuple(0).TupleRad(), HTuple(360).TupleRad(),
				0.6, 0, 0.5, "least_squares", (HTuple(8).Append(1)), 1, &hv_Row, &hv_Column,
				&hv_Angle, &hv_Score);*/
			FindShapeModel(ho_ImageReduced, *hv_ShapeModelID, -0.39, 0.78, 0.5, 1, 0.5, "least_squares",
				0, 0.9, &hv_Row, &hv_Column, &hv_Angle, &hv_Score);
			if (0 != ((hv_Score.TupleLength()) == 1))
			{
				VectorAngleToRigid(hv_Row, hv_Column, hv_Angle, hv_RowRef, hv_ColumnRef, 0,
					&hv_HomMat2D);
				AffineTransImage(ho_Image, &ho_ImageTrans, hv_HomMat2D, "bilinear", "false");
				TrainVariationModel(ho_ImageTrans, *hv_VariationModelID);
				if (HDevWindowStack::IsOpen())
				{
					DispObj(ho_ImageTrans, HDevWindowStack::GetActive());
				}
			}
		}
	}
	GetVariationModel(&ho_MeanImage, &ho_VarImage, *hv_VariationModelID);
	WriteVariationModel(*hv_VariationModelID, trainfile);
	WriteShapeModel(*hv_ShapeModelID, trainfile1);
	WriteRegion(ho_RegionROI, "region");
	AfxMessageBox(_T("训练完成"));
	cout << "训练数量："<<hv_NumOKImages.TupleLength().I() << endl;
}

int tnum, ngnum, oknum, nullnum, a, s;
HTuple hv_NumNGImages;
bool abc;
void testing(HTuple hv_ImagetestDir, HTuple trainfile, HTuple trainfile1, HTuple hv_ShapeModelID, HTuple hv_VariationModelID, HTuple hv_FindOffset, HTuple hv_ThresholdAreaErrorMin, HTuple hv_ThresholdAreaErrorMax, HTuple AbsThreshold, HTuple VarThreshold)
{
	// Local iconic variables
	HObject  ho_ImageReduced, ho_FindROI, ho_Image;
	HObject  ho_ImageTrans, ho_ImageReduced1, ho_Rectangle;
	HObject  ho_RegionDiff, ho_ConnectedRegions, ho_RegionsError;

	// Local control variables
	//HTuple hv_VariationModelID;
	HTuple  hv_Area, hv_RowRef, hv_ColumnRef;
	HTuple  hv_Row, hv_Column, hv_Angle, hv_Score, hv_HomMat2D;
	HTuple  hv_ThresholdAreaError, hv_I, hv_Mean, hv_Deviation;
	HTuple  hv_NumError, hv_AreaRef, m_ImageWidth, m_ImageHeight;
	tnum = 0;
	ngnum = 0;
	oknum = 0;
	nullnum = 0;
	a = 0;
	s = 0;

	if (r == 1)
	{
		ReadRegion(&ho_RegionROI, "region");
		ReadShapeModel(trainfile1, &hv_ShapeModelID);
		ReadVariationModel(trainfile, &hv_VariationModelID);
		r = 0;
	}
	PrepareVariationModel(hv_VariationModelID, AbsThreshold, VarThreshold);
	AreaCenter(ho_RegionROI, &hv_AreaRef, &hv_RowRef, &hv_ColumnRef);
	HTuple  hv_RegionRow1, hv_RegionCloumn1, hv_RegionRow2, hv_RegionCloumn2;
	SmallestRectangle1(ho_RegionROI, &hv_RegionRow1, &hv_RegionCloumn1, &hv_RegionRow2, &hv_RegionCloumn2);
	GenRectangle1(&ho_FindROI, hv_RegionRow1 - hv_FindOffset, hv_RegionCloumn1 - hv_FindOffset, hv_RegionRow2 + hv_FindOffset,
		hv_RegionCloumn2 + hv_FindOffset);
	ListFiles(hv_ImagetestDir, (HTuple("files").Append("follow_links")), &hv_NumNGImages);
	TupleRegexpSelect(hv_NumNGImages, (HTuple("\\.(tif|tiff|gif|bmp|jpg|jpeg|jp2|png|pcx|pgm|ppm|pbm|xwd|ima|hobj)$").Append("ignore_case")),
		&hv_NumNGImages);
	HTuple end_val38 = (hv_NumNGImages.TupleLength()) - 1;
	HTuple step_val38 = 1;
	for (hv_I = 0; hv_I.Continue(end_val38, step_val38); hv_I += step_val38)
	{
		tnum++;
		ReadImage(&ho_Image, HTuple(hv_NumNGImages[hv_I]));
		if (j == true)
		{
			GetImagePointer1(ho_Image, NULL, NULL, &m_ImageWidth, &m_ImageHeight);
			SetPart(hvWindowID, 0, 0, m_ImageHeight - 1, m_ImageWidth - 1);
			HDevWindowStack::Push(hvWindowID);
			//if (HDevWindowStack::IsOpen())
			SetColor(HDevWindowStack::GetActive(), "red");
			j = false;
		}
		ReduceDomain(ho_Image, ho_FindROI, &ho_ImageReduced);
		/*FindShapeModel(ho_ImageReduced, hv_ShapeModelID, HTuple(0).TupleRad(), HTuple(360).TupleRad(),
		0.6, 0, 0.5, "least_squares", (HTuple(8).Append(1)), 1, &hv_Row, &hv_Column,
		&hv_Angle, &hv_Score);*/
		FindShapeModel(ho_ImageReduced, hv_ShapeModelID, -0.39, 0.78, 0.5, 1, 0.5, "least_squares",
			0, 0.9, &hv_Row, &hv_Column, &hv_Angle, &hv_Score);
		if (0 != ((hv_Score.TupleLength()) == 1))
		{
			VectorAngleToRigid(hv_Row, hv_Column, hv_Angle, hv_RowRef, hv_ColumnRef, 0, &hv_HomMat2D);
			AffineTransImage(ho_Image, &ho_ImageTrans, hv_HomMat2D, "bilinear", "false");
			ReduceDomain(ho_ImageTrans, ho_RegionROI, &ho_ImageReduced1);
			CompareVariationModel(ho_ImageReduced1, &ho_RegionDiff, hv_VariationModelID);
			HalconCpp::Connection(ho_RegionDiff, &ho_ConnectedRegions);
			//ThresholdAreaError值与产品大小正相关
			SelectShape(ho_ConnectedRegions, &ho_RegionsError, "area", "and", hv_ThresholdAreaErrorMin, hv_ThresholdAreaErrorMax);
			CountObj(ho_RegionsError, &hv_NumError);
			if (0 != (hv_NumError == 0))
			{
				if (HDevWindowStack::IsOpen())
					DispObj(ho_ImageTrans, HDevWindowStack::GetActive());
				oknum++;
				//std::cout << "OK" << std::endl;
			}
			else
			{
				if (HDevWindowStack::IsOpen())
				{
					DispObj(ho_ImageTrans, HDevWindowStack::GetActive());
					DispObj(ho_RegionsError, HDevWindowStack::GetActive());
				}
				ngnum++;
				//std::cout << "NG" << std::endl;
			}
		}
		else
		{
			nullnum++;
			if (abc == true)
			{
				GenRectangle1(&ho_Rectangle, 0, 0, 50, 50);
				Intensity(ho_Rectangle, ho_Image, &hv_Mean, &hv_Deviation);
				if (hv_Deviation.D() == 0)
				{
					cout << "空白片" << endl;
					s++;
				}
				else
				{
					a = 1;
					break;
				}
			}
		}
	}
}

HTuple hv_ImagetrainDir = "./训练";
HTuple hv_ImagetestDir = "./调优";
HTuple hv_ImagetestDir1 = "./测试";
HTuple trainfile = "./training_variation.vam";
HTuple trainfile1 = "./training_shape.vam";
HTuple hv_FindOffset = 200;
HTuple hv_ShapeModelID, hv_VariationModelID;
void CROIDlg::OnBnClickedButton3()
{
	// TODO:  在此添加控件通知处理程序代码
	training(hv_ImagetrainDir, trainfile, trainfile1, &hv_ShapeModelID, &hv_VariationModelID, hv_FindOffset);
}



double AreaMin, AreaMax, AbsThres, VarThres;
double i, m, k, l;
HTuple hv_i1, hv_j1, hv_k1, hv_l1;
void CROIDlg::OnBnClickedButton4()
{
	// TODO:  在此添加控件通知处理程序代码
	//HTuple AbsThreshold = 10, VarThreshold = 5;
	//HTuple hv_ThresholdAreaErrorMin = 1000;
	//HTuple hv_ThresholdAreaErrorMax = 50000;
	abc = true;
	double cp = 0;
	UpdateData(TRUE);
	if (minareamin > 0 && minareamin <= minareamax && minareamax <= maxareamin && maxareamin <= maxareamax && absthresmin > 0 && absthresmin <= absthresmax && varthresmin > 0 &&
		varthresmin <= varthresmax && minareastep > 0 && maxareastep > 0 && absthresstep > 0 && varthresstep > 0)
	{
		for (i = minareamin; i <= minareamax; i += minareastep)
		{
			for (m = maxareamin; m <= maxareamax; m += maxareastep)
			{
				for (k = absthresmin; k <= absthresmax; k += absthresstep)
				{
					for (l = varthresmin; l <= varthresmax; l += varthresstep)
					{
						hv_i1 = i; hv_j1 = m; hv_k1 = k; hv_l1 = l;
						testing(hv_ImagetestDir, trainfile, trainfile1, hv_ShapeModelID, hv_VariationModelID, hv_FindOffset, hv_i1, hv_j1, hv_k1, hv_l1);
						if (a == 0)
						{
							if ((double)ngnum / (hv_NumNGImages.TupleLength() - s) > cp)
							{
								cp = (double)ngnum / (hv_NumNGImages.TupleLength() - s);
								AreaMin = i;
								AreaMax = m;
								AbsThres = k;
								VarThres = l;
							}
						}
						else
							break;
					}
					if (a == 1)
						break;
				}
				if (a == 1)
					break;
			}
			if (a == 1)
				break;
		}
		if (a == 0)
		{
			MessageBox(_T("参数已优化"));
			cout << "测试数量（不包括空白片）：" << hv_NumNGImages.TupleLength().I() - s << endl;
			cout << "最优检出率：" << cp << endl;
			cout << "最优参数：" << endl;
			cout << "最小面积：" << AreaMin << endl;
			cout << "最大面积：" << AreaMax << endl;
			cout << "绝对阈值：" << AbsThres << endl;
			cout << "变化阈值：" << VarThres << endl;
		}
		else
			AfxMessageBox(_T("匹配失败，重新取ROI"));
	}
	else
		AfxMessageBox(_T("参数输入错误，重新输入"));
}


void CROIDlg::OnCbnSelchangeCombo1()
{
	// TODO:  在此添加控件通知处理程序代码
	int nSel;
	// 获取组合框控件的列表框中选中项的索引   
	nSel = combobox.GetCurSel();
	// 根据选中项索引获取该项字符串   
	combobox.GetLBText(nSel, name);
}




void CROIDlg::OnBnClickedButton5()
{
	// TODO:  在此添加控件通知处理程序代码
	abc = false;
	hv_i1 = AreaMin; hv_j1 = AreaMax; hv_k1 = AbsThres; hv_l1 = VarThres;
	testing(hv_ImagetestDir1, trainfile, trainfile1, hv_ShapeModelID, hv_VariationModelID, hv_FindOffset, hv_i1, hv_j1, hv_k1, hv_l1);
}

bool judg = true;
void CROIDlg::OnBnClickedButton6()
{
	// TODO:  在此添加控件通知处理程序代码
	addDataTOAccess();          //保存access
}


void CROIDlg::addDataTOAccess()
{
	CString sql;
	sql.Format(_T("select * from detectInfo"));
	m_Ado1.m_pRecordset = m_Ado1.OpenRecordset(sql);
	//清空表格
	if (!m_Ado1.m_pRecordset->BOF)
	{
		m_Ado1.m_pRecordset->MoveFirst();
		while (!m_Ado1.m_pRecordset->adoEOF)
		{
			m_Ado1.m_pRecordset->MoveNext();
		}
		if (judg == false)
		{
			m_Ado1.m_pRecordset->MovePrevious();
			m_Ado1.m_pRecordset->Delete(adAffectCurrent);
			m_Ado1.m_pRecordset->Update(); //更新数据表记录
		}
	}
	try
	{
		CString  strTime = CTime::GetCurrentTime().Format("%Y%m%d");//获取当前时间
		m_Ado1.m_pRecordset->AddNew();
		m_Ado1.m_pRecordset->PutCollect("检测日期", (_bstr_t)strTime);
		m_Ado1.m_pRecordset->PutCollect("检测数", (_bstr_t)tnum);
		m_Ado1.m_pRecordset->PutCollect("正常数", (_bstr_t)oknum);
		m_Ado1.m_pRecordset->PutCollect("缺陷数", (_bstr_t)ngnum);
		m_Ado1.m_pRecordset->PutCollect("未匹配数", (_bstr_t)nullnum);
		double zb = (double)oknum / tnum;
		m_Ado1.m_pRecordset->PutCollect("正常数占比", (_bstr_t)zb);
		double qb = (double)ngnum / tnum;
		m_Ado1.m_pRecordset->PutCollect("缺陷数占比", (_bstr_t)qb);
		double nb = (double)nullnum / tnum;
		m_Ado1.m_pRecordset->PutCollect("未匹配数占比", (_bstr_t)nb);
		m_Ado1.m_pRecordset->Update();
		AfxMessageBox(_T("添加成功！"));
		judg = false;
	}
	catch (_com_error e){
		AfxMessageBox(_T("添加失败！"));
	}

}


void CROIDlg::OnBnClickedButton7()
{
	// TODO:  在此添加控件通知处理程序代码
	INT_PTR nRes;
	nRes = odr.DoModal();
	if (nRes == IDOK)
		return;
}
