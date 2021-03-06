
// OpenCVPlatDoc.cpp : COpenCVPlatDoc 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "OpenCVPlat.h"
#endif

#include "OpenCVPlatDoc.h"

#include <propkey.h>

#include "Vignette.h"
#include "MainFrm.h"
#include "OpenCVPlatView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// COpenCVPlatDoc

IMPLEMENT_DYNCREATE(COpenCVPlatDoc, CDocument)

BEGIN_MESSAGE_MAP(COpenCVPlatDoc, CDocument)
END_MESSAGE_MAP()


// COpenCVPlatDoc 构造/析构

COpenCVPlatDoc::COpenCVPlatDoc()
{
	// TODO:  在此添加一次性构造代码

}

COpenCVPlatDoc::~COpenCVPlatDoc()
{
}

BOOL COpenCVPlatDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO:  在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// COpenCVPlatDoc 序列化

void COpenCVPlatDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO:  在此添加存储代码
	}
	else
	{
		// TODO:  在此添加加载代码
	}
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void COpenCVPlatDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 搜索处理程序的支持
void COpenCVPlatDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void COpenCVPlatDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// COpenCVPlatDoc 诊断

#ifdef _DEBUG
void COpenCVPlatDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void COpenCVPlatDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// COpenCVPlatDoc 命令


BOOL COpenCVPlatDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	// TODO:  在此添加您专用的创建代码
	CImage cimage;
	HRESULT hr = cimage.Load(lpszPathName);
	int nChannels = cimage.GetBPP() / 8;
	if ((1 != nChannels) && (3 != nChannels))
	{
		return FALSE;
	}
	int nWidth = cimage.GetWidth();
	int nHeight = cimage.GetHeight();
 
	if (1 == nChannels)
	{
		image.create(nHeight, nWidth, CV_8UC1);
	}
	else if (3 == nChannels)
	{
		image.create(nHeight, nWidth, CV_8UC3);
	}

	uchar* pucRow;
	uchar* pucImage = (uchar*)cimage.GetBits(); 
	int nStep = cimage.GetPitch();

	for (int nRow = 0; nRow < nHeight; nRow++)
	{
		pucRow = (image.ptr<uchar>(nRow));
		for (int nCol = 0; nCol < nWidth; nCol++)
		{
			if (1 == nChannels)
			{
				pucRow[nCol] = *(pucImage + nRow * nStep + nCol);
			}
			else if (3 == nChannels)
			{
				for (int nCha = 0; nCha < 3; nCha++)
				{
					pucRow[nCol * 3 + nCha] = *(pucImage + nRow * nStep + nCol * 3 + nCha);
				}
			}
		}
	}
	if (image.rows > 1000 || image.cols > 1600)
	{
		cv::Mat new_image;
		cv::resize(image, new_image, cv::Size(image.cols * 900.0 / image.rows, 900));
		image = new_image;
	}
	HWND hwnd = AfxGetApp()->GetMainWnd()->GetSafeHwnd();
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	COpenCVPlatView *pView = (COpenCVPlatView *)pMain->GetActiveView();
	CRect window_rect;
	GetWindowRect(hwnd, &window_rect);
	CRect client_rect;
	pView->GetClientRect(&client_rect);
	SetWindowPos(hwnd, NULL, 0, 0, image.cols + window_rect.Width() - client_rect.Width(),
		image.rows + window_rect.Height() - client_rect.Height(), SWP_NOMOVE);
	return TRUE;
}


BOOL COpenCVPlatDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	// TODO:  在此添加专用代码和/或调用基类
	int nChannels = image.channels();
	if ((1 != nChannels) && (3 != nChannels))
	{
		return FALSE;
	}
	int nWidth = image.cols;
	int nHeight = image.rows;

	CImage cimage;
	cimage.Destroy();
	cimage.Create(nWidth, nHeight, 8 * nChannels);

	uchar* pucRow;
	uchar* pucImage = (uchar*)cimage.GetBits();
	int nStep = cimage.GetPitch(); 

	if (1 == nChannels)
	{
		RGBQUAD* rgbquadColorTable;
		int nMaxColors = 256;
		rgbquadColorTable = new RGBQUAD[nMaxColors];
		cimage.GetColorTable(0, nMaxColors, rgbquadColorTable);
		for (int nColor = 0; nColor < nMaxColors; nColor++)
		{
			rgbquadColorTable[nColor].rgbBlue = (uchar)nColor;
			rgbquadColorTable[nColor].rgbGreen = (uchar)nColor;
			rgbquadColorTable[nColor].rgbRed = (uchar)nColor;
		}
		cimage.SetColorTable(0, nMaxColors, rgbquadColorTable);
		delete[]rgbquadColorTable;
	}

	for (int nRow = 0; nRow < nHeight; nRow++)
	{
		pucRow = (image.ptr<uchar>(nRow));
		for (int nCol = 0; nCol < nWidth; nCol++)
		{
			if (1 == nChannels)
			{
				*(pucImage + nRow * nStep + nCol) = pucRow[nCol];
			}
			else if (3 == nChannels)
			{
				for (int nCha = 0; nCha < 3; nCha++)
				{
					*(pucImage + nRow * nStep + nCol * 3 + nCha) = pucRow[nCol * 3 + nCha];
				}
			}
		}
	}
	HRESULT hr = cimage.Save(lpszPathName);
	return TRUE;
	//return CDocument::OnSaveDocument(lpszPathName);
}
