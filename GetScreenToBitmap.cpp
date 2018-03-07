// GetScreenToBitmap.cpp: implementation of the CGetScreenToBitmap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyServer.h"
#include "GetScreenToBitmap.h"
//#include "jpeglib.H"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGetScreenToBitmap::CGetScreenToBitmap()
{
	m_lpBmpData = NULL;
	m_dwBmpSize = 0;
	m_dwBmpInfoSize = 0;
}

CGetScreenToBitmap::~CGetScreenToBitmap()
{
   ResetVariable();
}


void CGetScreenToBitmap::ResetVariable()
{
    if(m_lpBmpData) GlobalFree(m_lpBmpData);
	m_lpBmpData = NULL;
    m_dwBmpSize = 0;
    m_dwBmpInfoSize = 0;
}


LPSTR FAR CGetScreenToBitmap::GetImage()
{
	 return m_lpBmpData;
}

DWORD CGetScreenToBitmap::GetImageSize()
{
	 return m_dwBmpSize;
}

BOOL CGetScreenToBitmap::GetScreen(CRect rcArea,int nBits, int nArea)
{
	HDC	hScrDC, hMemDC;		// 屏幕和内存设备描述表
	HBITMAP	hBitmap, hOldBitmap;		// 位图句柄
	HDC	hDC;				//设备描述表
	int	iBits;				//当前显示分辨率下每个像素所占字节数
	WORD wBitCount;			//位图中每个像素所占字节数
	//定义调色板大小， 位图中像素字节大小 ，位图文件大小 ， 写入文件字节数
	DWORD dwPaletteSize=0, dwBmBitsSize;
	BITMAP Bitmap;			//位图属性结构
	BITMAPINFOHEADER bi;		//位图信息头结构 
	LPBITMAPINFOHEADER lpbi;		//指向位图信息头结构

	//定义文件，分配内存句柄，调色板句柄  
	HPALETTE hPal,hOldPal=NULL;
    //释放原有资源
	ResetVariable();
	// 获得屏幕分辨率
	SetArea(rcArea, nArea); //取图象区域
	//为屏幕创建设备描述表
	hScrDC = CreateDC("DISPLAY", NULL, NULL, NULL);
	//为屏幕设备描述表创建兼容的内存设备描述表
	hMemDC = CreateCompatibleDC(hScrDC);
	// 创建一个与屏幕设备描述表兼容的位图
	hBitmap = CreateCompatibleBitmap (hScrDC, 
		      m_rcArea.Width(), m_rcArea.Height());
	// 把新位图选到内存设备描述表中
	hOldBitmap = (HBITMAP) SelectObject(hMemDC, 
		          hBitmap);
	// 把屏幕设备描述表拷贝到内存设备描述表中
	BitBlt(hMemDC, 0, 0, m_rcArea.Width(), 
		   m_rcArea.Height(), hScrDC, 
		   m_rcArea.left, m_rcArea.top, SRCCOPY);
	//得到屏幕位图的句柄
	hBitmap = (HBITMAP) SelectObject(hMemDC, hOldBitmap);
	//计算位图每个像素所占字节数
	iBits = GetDeviceCaps(hScrDC, BITSPIXEL);
	iBits = iBits * GetDeviceCaps(hScrDC, PLANES);
	if (iBits <= 1)
		wBitCount = 1;
	else if (iBits <= 4)
		wBitCount = 4;
	else if (iBits <= 8)
		wBitCount = 8;
	else// if (iBits <= 24)
		wBitCount = 24;

	if (wBitCount > nBits)
		wBitCount = nBits;
	//计算调色板大小
	if (wBitCount <= 8)
		dwPaletteSize = (1 << wBitCount) * sizeof(RGBQUAD);

	//设置位图信息头结构
	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&Bitmap);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = Bitmap.bmWidth;
	bi.biHeight = Bitmap.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = wBitCount;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = dwPaletteSize/sizeof(RGBQUAD);
	bi.biClrImportant = 0;

	dwBmBitsSize = ((Bitmap.bmWidth * wBitCount+31)/32)* 4 *Bitmap.bmHeight ;
	m_dwBmpInfoSize = dwPaletteSize + sizeof(BITMAPINFOHEADER);
	m_dwBmpSize = dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER);
	m_lpBmpData  = (LPSTR)GlobalAlloc(GMEM_FIXED,m_dwBmpSize);
	if (m_lpBmpData == NULL) goto Exit01;

    lpbi = (LPBITMAPINFOHEADER)m_lpBmpData;
	*lpbi = bi;
	// 处理调色板   
	hPal = (HPALETTE) GetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{
		hDC = ::GetDC (NULL);
		hOldPal = ::SelectPalette(hDC, hPal, FALSE);
		RealizePalette(hDC);
	}
	// 获取该调色板下新的像素值
	::GetDIBits(
			hDC,
			hBitmap,
			0, 
			(UINT) Bitmap.bmHeight,
			(LPSTR)lpbi + sizeof(BITMAPINFOHEADER) + dwPaletteSize,
			//(BITMAPINFOHEADER *)lpbi,
			(BITMAPINFO *)lpbi,
			DIB_RGB_COLORS
		  );
	//恢复调色板
	if (hOldPal)
	{
		SelectPalette(hDC, hOldPal, TRUE);
		RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
	}
//DEL	m_lpBmpData = (char *)lpbi;
	//清除
	DeleteObject (hBitmap);
	DeleteObject (hOldBitmap);
	DeleteDC(hScrDC);
	DeleteDC(hMemDC);
	return TRUE;

Exit01:
	ResetVariable();
	DeleteObject (hBitmap);
	DeleteDC(hScrDC);
	DeleteDC(hMemDC);
	return FALSE;
}

void CGetScreenToBitmap::SetArea(CRect rcArea, int nArea)
{
	//为屏幕创建设备描述表
	HWND hFocus;
	int		xScrn, yScrn;			// 屏幕分辨率 
	HDC hScrDC = CreateDC("DISPLAY", NULL, NULL, NULL);
	// 获得屏幕分辨率

	xScrn = GetDeviceCaps(hScrDC, HORZRES);
	yScrn = GetDeviceCaps(hScrDC, VERTRES);
	DeleteDC(hScrDC);
    m_rcArea.left = 0;
    m_rcArea.top = 0;
    m_rcArea.right = xScrn;
    m_rcArea.bottom = yScrn;
    if (nArea == 1 &&  //1)当前窗口2)指定区域0)全屏
        (hFocus = ::GetForegroundWindow()) != NULL)
	{
	    GetWindowRect(hFocus, &m_rcArea);
	}
	else if(nArea == 2)
	{
    m_rcArea =  rcArea;
	}
	if (m_rcArea.left < 0)
		m_rcArea.left = 0;
	if (m_rcArea.top < 0)
		m_rcArea.top = 0;
	if (m_rcArea.right > xScrn)
		m_rcArea.right = xScrn;
	if (m_rcArea.bottom > yScrn)
		m_rcArea.bottom = yScrn;
	//确保选定区域是可见的
}
