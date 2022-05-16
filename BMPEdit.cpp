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

	//�ͷ�֮ǰ��ȡ������///////
	//�ͷ�ͼ������
	if (m_hgImageData)
		GlobalFree(m_hgImageData);

	m_sBMPFileFullName = sBmpFileName;
	if (m_sBMPFileFullName.IsEmpty())
		return FALSE;
	CFile BmpFile;
	if (!BmpFile.Open(m_sBMPFileFullName, CFile::modeRead))
	{
		AfxMessageBox("�ļ���ʱ����!");
		return FALSE;
	}
	//��ʽ���
	if (BmpFile.Read(&m_BmpFileHeader, sizeof(BITMAPFILEHEADER)) < sizeof(BITMAPFILEHEADER))
	{
		AfxMessageBox("�ļ���������!");
		BmpFile.Close();
		return FALSE;
	}
	if (m_BmpFileHeader.bfType != 0x4D42)
	{
		AfxMessageBox("��λͼ�ļ�");
		BmpFile.Close();
		return FALSE;
	}
	if (BmpFile.Read(&m_BmpInfo.bmiHeader, sizeof(BITMAPINFOHEADER)) < sizeof(BITMAPINFOHEADER))
	{
		AfxMessageBox("�ļ���������!");
		BmpFile.Close();
		return FALSE;
	}
	//SetDIBitsToDevice()��StretchDIBits()�����д����ɫ�壬����Ҫ����ǰDC��ѡ���ɫ���ˣ�����

	//��ȡ��ɫ��
	if (m_BmpInfo.bmiHeader.biBitCount < 16)
		BmpFile.Read(&m_BmpInfo.bmiColors, GetColorNum(m_BmpInfo.bmiHeader) * sizeof(RGBQUAD));

	//��ȡͼ������
	nBytePerLine = (m_BmpInfo.bmiHeader.biWidth * m_BmpInfo.bmiHeader.biBitCount + 31) / 32 * 4;//ͼ��ÿ����ռʵ���ֽ���������4�ı�����
	m_hgImageData = GlobalAlloc(GHND, nBytePerLine * m_BmpInfo.bmiHeader.biHeight);

	if (m_hgImageData == NULL)
	{
		AfxMessageBox("�ڴ治��");
		BmpFile.Close();
		return FALSE;
	}
	pImageData = (byte*)GlobalLock(m_hgImageData);
	BmpFile.Read(pImageData, nBytePerLine * m_BmpInfo.bmiHeader.biHeight);

	//add your code here
	////libin20150116///start////

	//��ɫͼ��ҶȻ�
	Gray();

	//Sobel��
	//Sharpening();

	//ͼ���ֵ��
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
			//�ҶȻ���ʱֵ
			temp = double(*(pImageData + j * nBytePerLine + i * 3 + 0)) * 0.229 +
				double(*(pImageData + j * nBytePerLine + i * 3 + 1)) * 0.587 +
				double(*(pImageData + j * nBytePerLine + i * 3 + 2)) * 0.114;
			//��ͨ��ͼת����ͨ��
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

	// �����ڴ棬�Ա�����DIB
	hDIB = GlobalAlloc(GHND, nBytePerLine * lHeight);

	// �ж��Ƿ��ڴ����ʧ��
	if (hDIB == NULL)
	{
		// �����ڴ�ʧ��
		return NULL;
	}
	pImageDataNew = (byte*)GlobalLock(hDIB);

	//���㷨
	//add your code here
	for (int j = 1; j < lHeight - 1; j++)
	{
		for (int i = 1; i < lWidth - 1; i++)
		{
			gx = gy = 0;
			//ȡģ��λ�õĸ���Ҷ�ֵ
			val11 = *(pImageData + (j - 1) * nBytePerLine + (i - 1) * 3);
			val12 = *(pImageData + (j - 1) * nBytePerLine + i * 3);
			val13 = *(pImageData + (j - 1) * nBytePerLine + (i + 1) * 3);
			val21 = *(pImageData + j * nBytePerLine + (i - 1) * 3);
			val22 = *(pImageData + j * nBytePerLine + i * 3);
			val23 = *(pImageData + j * nBytePerLine + (i + 1) * 3);
			val31 = *(pImageData + nBytePerLine + i * 3);
			val32 = *(pImageData + (j + 1) * nBytePerLine + (i + 1) * 3);
			val33 = *(pImageData + (j + 1) * nBytePerLine + (i - 1) * 3);

			//ͼ���ÿһ�����صĺ��������ݶȽ���ֵ��
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
	//����ֵ������ֵΪ255��С�ڵ�����ֵΪ0
	//add your code here
//"����ֵ���ڸ���Ϊ��С�ڵ��ڸ���Ϊ 
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




