#if !defined(AFX_BMPEDIT_H__CAD01C85_3CBA_4D33_AFDF_14A746B4BD41__INCLUDED_)
#define AFX_BMPEDIT_H__CAD01C85_3CBA_4D33_AFDF_14A746B4BD41__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BMPEdit.h : header file
//

//BITMAPINFO的扩展
struct EXT_BITMAPINFO
{
	BITMAPINFOHEADER    bmiHeader;
    RGBQUAD             bmiColors[256];//调色板最大为256
};


/////////////////////////////////////////////////////////////////////////////
// CBMPEdit window

class CBMPEdit
{
// Construction
public:
	CBMPEdit();

// Attributes
public:

// Operations
	//原图像每行的字节数
	long nBytePerLine;
	byte *pImageData;
public:
	BOOL PlayBmp(CDC *pDC, const CPoint &StartPoint);
	int GetColorNum(const BITMAPINFOHEADER &BmpInf);
	BOOL LoadBmpFile(const CString &sBMPFileName);
	virtual ~CBMPEdit();

/////libin20150118//////////////////
//图像灰度化
void Gray();

//图像锐化
int Sharpening();

//二值化
void Binarization();

private:
	BOOL m_bReady;  //数据是否已从文件读取
	CString m_sBMPFileFullName;  //完整路径名
	BITMAPFILEHEADER m_BmpFileHeader;
	EXT_BITMAPINFO m_BmpInfo;//存放 BITMAPINFOHEADER和调色板(非真彩色)
	HGLOBAL m_hgImageData; //图像数据句柄



	////libin20150118/////
	// 源图像的宽度和高度
	int	lWidth;
	int	lHeight;
	// 旋转后新DIB句柄
	HGLOBAL	hDIB;
	HGLOBAL hDIB_tmp;
	//新图像内存的地址

	byte *pImageDataNew;
	byte *pImageDataNew_tmp;
	//新灰度化像素临时值
	float temp;
	//Sobel算法参数
	double	gx , gy ;
	double val11,val12,val13,val21,val22,val23,val31,val32,val33;
	//Sobel因子
	double Sobel;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BMPEDIT_H__CAD01C85_3CBA_4D33_AFDF_14A746B4BD41__INCLUDED_)
