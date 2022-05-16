#if !defined(AFX_BMPEDIT_H__CAD01C85_3CBA_4D33_AFDF_14A746B4BD41__INCLUDED_)
#define AFX_BMPEDIT_H__CAD01C85_3CBA_4D33_AFDF_14A746B4BD41__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BMPEdit.h : header file
//

//BITMAPINFO����չ
struct EXT_BITMAPINFO
{
	BITMAPINFOHEADER    bmiHeader;
    RGBQUAD             bmiColors[256];//��ɫ�����Ϊ256
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
	//ԭͼ��ÿ�е��ֽ���
	long nBytePerLine;
	byte *pImageData;
public:
	BOOL PlayBmp(CDC *pDC, const CPoint &StartPoint);
	int GetColorNum(const BITMAPINFOHEADER &BmpInf);
	BOOL LoadBmpFile(const CString &sBMPFileName);
	virtual ~CBMPEdit();

/////libin20150118//////////////////
//ͼ��ҶȻ�
void Gray();

//ͼ����
int Sharpening();

//��ֵ��
void Binarization();

private:
	BOOL m_bReady;  //�����Ƿ��Ѵ��ļ���ȡ
	CString m_sBMPFileFullName;  //����·����
	BITMAPFILEHEADER m_BmpFileHeader;
	EXT_BITMAPINFO m_BmpInfo;//��� BITMAPINFOHEADER�͵�ɫ��(�����ɫ)
	HGLOBAL m_hgImageData; //ͼ�����ݾ��



	////libin20150118/////
	// Դͼ��Ŀ�Ⱥ͸߶�
	int	lWidth;
	int	lHeight;
	// ��ת����DIB���
	HGLOBAL	hDIB;
	HGLOBAL hDIB_tmp;
	//��ͼ���ڴ�ĵ�ַ

	byte *pImageDataNew;
	byte *pImageDataNew_tmp;
	//�»ҶȻ�������ʱֵ
	float temp;
	//Sobel�㷨����
	double	gx , gy ;
	double val11,val12,val13,val21,val22,val23,val31,val32,val33;
	//Sobel����
	double Sobel;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BMPEDIT_H__CAD01C85_3CBA_4D33_AFDF_14A746B4BD41__INCLUDED_)
