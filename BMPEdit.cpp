// BMPEdit.cpp : implementation file
//

#include "stdafx.h"
#include "02test.h"
#include "BMPEdit.h"
#include "math.h"
using namespace std;

/////add20150108
#define PI 3.14159


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBMPEdit

CBMPEdit::CBMPEdit()
{
}

CBMPEdit::~CBMPEdit()
{
}



/////////////////////////////////////////////////////////////////////////////
// CBMPEdit message handlers




BOOL CBMPEdit::LoadBmpFile(const CString& sBmpFileName)
{
	m_bReady = FALSE;

	//释放之前读取的数据///////
	//释放图像数据
	if (m_hgImageData)
		GlobalFree(m_hgImageData);

	m_sBMPFileFullName = sBmpFileName;
	if (m_sBMPFileFullName.IsEmpty())
		return FALSE;
	CFile BmpFile;
	if (!BmpFile.Open(m_sBMPFileFullName, CFile::modeRead))
	{
		AfxMessageBox("文件打开时出错!");
		return FALSE;
	}
	//格式检查
	if (BmpFile.Read(&m_BmpFileHeader, sizeof(BITMAPFILEHEADER)) < sizeof(BITMAPFILEHEADER))
	{
		AfxMessageBox("文件数据已损坏!");
		BmpFile.Close();
		return FALSE;
	}
	if (m_BmpFileHeader.bfType != 0x4D42)
	{
		AfxMessageBox("非位图文件");
		BmpFile.Close();
		return FALSE;
	}
	if (BmpFile.Read(&m_BmpInfo.bmiHeader, sizeof(BITMAPINFOHEADER)) < sizeof(BITMAPINFOHEADER))
	{
		AfxMessageBox("文件数据已损坏!");
		BmpFile.Close();
		return FALSE;
	}
	//SetDIBitsToDevice()和StretchDIBits()会自行处理调色板，不需要再向当前DC中选入调色板了！！！

	//读取调色板
	if (m_BmpInfo.bmiHeader.biBitCount < 16)
		BmpFile.Read(&m_BmpInfo.bmiColors, GetColorNum(m_BmpInfo.bmiHeader) * sizeof(RGBQUAD));

	//读取图像数据
	nBytePerLine = (m_BmpInfo.bmiHeader.biWidth * m_BmpInfo.bmiHeader.biBitCount + 31) / 32 * 4;//图像每行所占实际字节数（须是4的倍数）
	m_hgImageData = GlobalAlloc(GHND, nBytePerLine * m_BmpInfo.bmiHeader.biHeight);

	if (m_hgImageData == NULL)
	{
		AfxMessageBox("内存不足");
		BmpFile.Close();
		return FALSE;
	}
	pImageData = (byte*)GlobalLock(m_hgImageData);
	BmpFile.Read(pImageData, nBytePerLine * m_BmpInfo.bmiHeader.biHeight);

	//add your code here
	////libin20150116///start////

	//彩色图像灰度化
	Gray();

	//Sobel锐化
	//Sharpening();

	//图像二值化
	//Binarization();

	///////////end/////////////


	GlobalUnlock(m_hgImageData);
	BmpFile.Close();
	m_bReady = TRUE;
	return TRUE;
}

int CBMPEdit::GetColorNum(const BITMAPINFOHEADER& BmpInf)
{
	if (BmpInf.biClrUsed > 0)
		return BmpInf.biClrUsed;
	else
		return 1 << BmpInf.biBitCount;
}

BOOL CBMPEdit::PlayBmp(CDC* pDC, const CPoint& StartPoint)
{
	if (!m_bReady)
		return FALSE;

	/*byte *pData = (byte *)GlobalLock(m_hgImageData);*/
	//if(!SetDIBitsToDevice(pDC->GetSafeHdc(),
	//					StartPoint.x,StartPoint.y,
	//					m_BmpInfo.bmiHeader.biWidth,m_BmpInfo.bmiHeader.biHeight,
	//					0,0,
	//					0,m_BmpInfo.bmiHeader.biHeight,
	//					pData,
	//				//	pBmpInf,
	//				    (BITMAPINFO*)&m_BmpInfo,
	//					DIB_RGB_COLORS
	//					))
	// {
	//	 GlobalUnlock(m_hgImageData);
	//	 return FALSE;
	// }
	//GlobalUnlock(m_hgImageData);


/////libin///20150118////start////////
	byte* pData = (byte*)GlobalLock(hDIB);
	if (!SetDIBitsToDevice(pDC->GetSafeHdc(),
		StartPoint.x, StartPoint.y,
		m_BmpInfo.bmiHeader.biWidth, m_BmpInfo.bmiHeader.biHeight,
		0, 0,
		0, m_BmpInfo.bmiHeader.biHeight,
		pData,
		//	pBmpInf,
		(BITMAPINFO*)&m_BmpInfo,
		DIB_RGB_COLORS
	))
	{
		GlobalUnlock(hDIB);
		return FALSE;
	}
	GlobalUnlock(hDIB);

	//////////////end////////////////////////


	return TRUE;

}
void CBMPEdit::Gray()
{
	pImageData = (byte*)GlobalLock(m_hgImageData);
	lWidth = m_BmpInfo.bmiHeader.biWidth;
	lHeight = m_BmpInfo.bmiHeader.biHeight;

	//add your code here
	for (int j = 0; j < lHeight; j++)
	{
		for (int i = 0; i < lWidth; i++)
		{
			//灰度化临时值
			temp = double(*(pImageData + j * nBytePerLine + i * 3 + 0)) * 0.229 +
				double(*(pImageData + j * nBytePerLine + i * 3 + 1)) * 0.587 +
				double(*(pImageData + j * nBytePerLine + i * 3 + 2)) * 0.114;
			//三通道图转到单通道
			*(pImageData + j * nBytePerLine + i * 3 + 0) = int(temp);
			*(pImageData + j * nBytePerLine + i * 3 + 1) = int(temp);
			*(pImageData + j * nBytePerLine + i * 3 + 2) = int(temp);
		}
	}
	GlobalUnlock(m_hgImageData);
}



int CBMPEdit::Sharpening()
{
	lWidth = m_BmpInfo.bmiHeader.biWidth;
	lHeight = m_BmpInfo.bmiHeader.biHeight;

	// 分配内存，以保存新DIB
	hDIB = GlobalAlloc(GHND, nBytePerLine * lHeight);

	// 判断是否内存分配失败
	if (hDIB == NULL)
	{
		// 分配内存失败
		return NULL;
	}
	pImageDataNew = (byte*)GlobalLock(hDIB);

	//锐化算法
	//add your code here
	for (int j = 1; j < lHeight - 1; j++)
	{
		for (int i = 1; i < lWidth - 1; i++)
		{
			gx = gy = 0;
			//取模板位置的各点灰度值
			val11 = *(pImageData + (j - 1) * nBytePerLine + (i - 1) * 3);
			val12 = *(pImageData + (j - 1) * nBytePerLine + i * 3);
			val13 = *(pImageData + (j - 1) * nBytePerLine + (i + 1) * 3);
			val21 = *(pImageData + j * nBytePerLine + (i - 1) * 3);
			val22 = *(pImageData + j * nBytePerLine + i * 3);
			val23 = *(pImageData + j * nBytePerLine + (i + 1) * 3);
			val31 = *(pImageData + nBytePerLine + i * 3);
			val32 = *(pImageData + (j + 1) * nBytePerLine + (i + 1) * 3);
			val33 = *(pImageData + (j + 1) * nBytePerLine + (i - 1) * 3);

			//图像的每一个像素的横向及纵向梯度近似值。
			gx = (-1.0) * val31 + 0 * val32 + 1.0 * val33
				+ (-2.0) * val21 + 0 * val22 + 2.0 * val23
				+ (-1.0) * val11 + 0 * val12 + 1.0 * val13;

			gy = (-1.0) * val31 + (-2.0) * val32 + (-1.0) * val33
				+ 0 * val21 + 0 * val22 + 0 * val32
				+ 1.0 * val11 + 2.0 * val12 + 1.0 * val13;

			Sobel = sqrt(gx * gx + gy * gy);
			*(pImageDataNew + j * nBytePerLine + i * 3 + 0) = int(Sobel);
			*(pImageDataNew + j * nBytePerLine + i * 3 + 1) = int(Sobel);
			*(pImageDataNew + j * nBytePerLine + i * 3 + 2) = int(Sobel);
		}
	}

	GlobalUnlock(hDIB);

}
void CBMPEdit::Binarization()
{
	//像素值大于阈值为255，小于等于阈值为0
	//add your code here
//"像素值大于复制为，小于等于复制为 
	for (int j = 0; j < lHeight; j++)
	{
		for (int i = 0; i < lWidth; i++) {

			if (int(*(pImageDataNew + j * nBytePerLine + i * 3)) > 80)
			{
				*(pImageDataNew + j * nBytePerLine + i * 3 + 0) = 255;
				*(pImageDataNew + j * nBytePerLine + i * 3 + 1) = 255;
				*(pImageDataNew + j * nBytePerLine + i * 3 + 2) = 255;
			}
			else {
				*(pImageDataNew + j * nBytePerLine + i * 3 + 0) = 0;
				*(pImageDataNew + j * nBytePerLine + i * 3 + 1) = 0;
				*(pImageDataNew + j * nBytePerLine + i * 3 + 2) = 0;
			}
		}
	}
}




