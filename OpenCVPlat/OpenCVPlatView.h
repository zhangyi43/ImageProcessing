
// OpenCVPlatView.h : COpenCVPlatView 类的接口
//

#pragma once


class COpenCVPlatView : public CView
{
protected: // 仅从序列化创建
	COpenCVPlatView();
	DECLARE_DYNCREATE(COpenCVPlatView)

// 特性
public:
	COpenCVPlatDoc* GetDocument() const;
	bool start_inpaint;
	cv::Mat nnf, last_nnf;
	cv::Mat dst;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~COpenCVPlatView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnVignette();
	afx_msg void OnMedianFilter();
	afx_msg void OnGaussianBlur();
	afx_msg void OnColorBalance();
	afx_msg void OnSaturation();
	afx_msg void OnSharpen();
	afx_msg void OnLiquify();
	afx_msg void OnSkin();
	afx_msg void OnLomo();
	afx_msg void OnImageInpaint();

	void inpaint();
};

#ifndef _DEBUG  // OpenCVPlatView.cpp 中的调试版本
inline COpenCVPlatDoc* COpenCVPlatView::GetDocument() const
   { return reinterpret_cast<COpenCVPlatDoc*>(m_pDocument); }
#endif

void pyramid_method(cv::Mat &image, int psize);
void optimize_start(cv::Mat &image, int psize);