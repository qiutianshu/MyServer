////////////////////////////////////////////////////////////////////////////
//	mfcappView.cpp : implementation of the CMfcappView class
//
//
//	Note  : GIF code removed 9/23/97 pending Unisys licensing. 
//
//
//	This code copyright 1997 Chris Losinger, unless otherwise noted
//
//	CHRISDL@PAGESZ.NET
//
//	PLEASE!!! Tell me of any bugs you find!!!
//
//	This code contains examples of using my JpegFile class, how to
//	read and write 1,4,8 and 24-bit BMPs 
//
//	I find that this code works well for my purposes. Feel free to 
//	use it in your own code, but I can't assume any responsibility
//	if this code fails to do what you expect.
//
//	If you find any problems with this code, feel free to contact 
//	me for help.
//
//	24-bit to 8-bit color quantization code modified from Dennis Lee's
//	DL1Quant. His source is available at ...
//


// MfcAppView.cpp : implementation of the CMfcAppView class
//

#include "stdafx.h"
#include "MfcApp.h"

#include <math.h>

#include "MfcAppDoc.h"
#include "MfcAppView.h"

#include "JpegFile.h"

#include "BMPDlg.h"
#include "BMPFile.h"

#include "QuantDlg.h"
#include "dl1quant.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMfcAppView

IMPLEMENT_DYNCREATE(CMfcAppView, CView)

BEGIN_MESSAGE_MAP(CMfcAppView, CView)
	//{{AFX_MSG_MAP(CMfcAppView)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_FILE_SAVECOLORMAPPEDBMP, OnFileSavecolormappedbmp)
	ON_COMMAND(ID_FILE_SAVEGRAYAS, OnFileSavegrayas)
	ON_COMMAND(ID_FILE_GETDIMENSIONSJPG, OnFileGetdimensionsjpg)
	ON_COMMAND(ID_PLAY, OnPlay)
	ON_COMMAND(ID_STOP, OnStop)
	ON_COMMAND(ID_PAUSE, OnPause)
	ON_UPDATE_COMMAND_UI_RANGE(ID_PAUSE,ID_PAUSE, OnPause)
//	ON_COMMAND_RANGE(ID_PAUSE, ID_PAUSE, OnViewPause)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMfcAppView construction/destruction

CMfcAppView::CMfcAppView()
{
							// we keep a single global image in memory

	m_buf=NULL;				// where we keep our image data
	m_width=0;				// image dimensions
	m_height=0;
	m_widthDW=0;
	m_bPause = false;
}

CMfcAppView::~CMfcAppView()
{
	// clean up
	if (m_buf!=NULL) {
		delete [] m_buf;
		m_buf=NULL;
	}

}

BOOL CMfcAppView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMfcAppView drawing

void CMfcAppView::OnDraw(CDC* pDC)
{
	CMfcAppDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// draw the BMP we have in memory
	DrawBMP();

}

/////////////////////////////////////////////////////////////////////////////
// CMfcAppView diagnostics

#ifdef _DEBUG
void CMfcAppView::AssertValid() const
{
	CView::AssertValid();
}

void CMfcAppView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMfcAppDoc* CMfcAppView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMfcAppDoc)));
	return (CMfcAppDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMfcAppView message handlers

void CMfcAppView::OnFileOpen() 
{
	CString fileName;
	CString filt="JPG File (*.JPG)|*.JPG|BMP (*.BMP)|*.BMP|All files (*.*)|*.*||";
    
    // OPENFILENAME - so i can get to its Help page easily
	CFileDialog fileDlg(TRUE,"*.JPG","*.JPG",NULL,filt,this);

	fileDlg.m_ofn.Flags|=OFN_FILEMUSTEXIST;
	fileDlg.m_ofn.lpstrTitle="File to load";

	if (fileDlg.DoModal()==IDOK) {

		AfxGetApp()->DoWaitCursor(1);

		fileName=fileDlg.GetPathName();
		CString ext=fileName.Right(4);

		if (!ext.CompareNoCase(".JPG"))
			LoadJPG(fileName);

		if (!ext.CompareNoCase(".BMP"))
			LoadBMP(fileName);
		
		AfxGetApp()->DoWaitCursor(-1);

	}            

	// force a redraw
	Invalidate(TRUE);

}

void CMfcAppView::OnFileSaveAs() 
{
	CString fileName;
	CString filt="JPG File (*.JPG)|*.JPG|BMP (*.BMP)|*.BMP|All files (*.*)|*.*||";
    
    // OPENFILENAME - so i can get to its Help page easily
	CFileDialog fileDlg(FALSE,"*.JPG","*.JPG",NULL,filt,this);

	fileDlg.m_ofn.Flags|=OFN_FILEMUSTEXIST;
	fileDlg.m_ofn.lpstrTitle="File to save as";

	if (fileDlg.DoModal()==IDOK) {
		
		AfxGetApp()->DoWaitCursor(1);

		fileName=fileDlg.GetPathName();

		CString ext=fileName.Right(4);

		if (!ext.CompareNoCase(".JPG"))
			SaveJPG(fileName,TRUE);

		if (!ext.CompareNoCase(".BMP"))
			SaveBMP24(fileName);

		AfxGetApp()->DoWaitCursor(-1);
	}      
}

void CMfcAppView::OnFileSavecolormappedbmp() 
{

	if (m_buf==NULL) {
		AfxMessageBox("No Image!");
		return;
	}

	////////////////////////////////////////////////////////////////////////
	// get the filename
	CString fileName;
	CString filt="BMP (*.BMP)|*.BMP|All files (*.*)|*.*||";
    
    // OPENFILENAME - so i can get to its Help page easily
	CFileDialog fileDlg(FALSE,"*.BMP","*.BMP",NULL,filt,this);
	fileDlg.m_ofn.Flags|=OFN_FILEMUSTEXIST;
	fileDlg.m_ofn.lpstrTitle="File to save as";

	if (fileDlg.DoModal()!=IDOK)
		return;

	fileName=fileDlg.GetPathName();

	////////////////////////////////////////////////////////////////////////
	// fetch bits per pixel
	CBMPDlg theDlg;
	if (theDlg.DoModal()!=IDOK)
		return;
	int bitsperpixel = theDlg.m_bits;

	AfxGetApp()->DoWaitCursor(1);

	////////////////////////////////////////////////////////////////////////
	// prepare for color-mapping
	
	// our palette
	RGBQUAD colormap[256];

	// num colors
	int colors = (int)pow(2,bitsperpixel);
	
	BYTE *colorMappedBuffer = NULL;

	// if we can use the color quantizer, we will
	if (bitsperpixel==8) {
		CQuantDlg theDlg;
		if (theDlg.DoModal()!=IDOK) {
			return;
		}

		// color or grayscale?
		if (theDlg.m_color) {

			// color !

			// allocate a buffer to colormap to
			colorMappedBuffer = (BYTE *)  new BYTE[m_width* m_height];

			if (colorMappedBuffer==NULL) {
				AfxMessageBox("Memory Error in OnSaveColormappedbmp!");
				return;
			}

			BYTE tmpPal[3][256];

			// colormap it 
			// generates an 8-bit color-mapped image into colorMappedBuffer
			if (!dl1quant(m_buf, 
								colorMappedBuffer,		// buffers
								m_width,
								m_height,	
								theDlg.m_quantColors,
								TRUE,
								tmpPal)) {				// palette
				AfxMessageBox("Quantization error");
				delete [] colorMappedBuffer;
				return;
			}
			// copy our palette
			for (UINT col=0;col<256;col++) {
				if (col>theDlg.m_quantColors) {
					colormap[col].rgbRed=0;
					colormap[col].rgbBlue=0;
					colormap[col].rgbGreen=0;
				} else {
					colormap[col].rgbRed=tmpPal[0][col];
					colormap[col].rgbGreen=tmpPal[1][col];
					colormap[col].rgbBlue=tmpPal[2][col];
				}
			}

		} else {
			// gray :(
			// convert to 8-bit colormapped grayscale
			colorMappedBuffer = MakeColormappedGrayscale(m_buf,					// RGB
														(UINT)m_width,			// pixels
														(UINT)m_height, 
														(UINT)m_width * 3,		// bytes
														(UINT)colors,			// colors
														colormap);				// palette
		}
	} else {	// bitsperpixel!=8

		// based on bitsperpixel, create a colormapped image
		colorMappedBuffer = MakeColormappedGrayscale(m_buf, 
													(UINT)m_width, 
													(UINT)m_height, 
													(UINT)m_width * 3, 
													(UINT)colors,
													colormap);
	}
	
	////////////////////////////////////////////////////////////////////////
	// finally, save the thing
	if (colorMappedBuffer!=NULL) {

		// write the BMP using our colormapped image (one byte per pixel, packed),
		// number of bits, number of total colors and a colormap
		// pixel values must be in the range [0...colors-1]

		BMPFile theBmpFile;

		theBmpFile.SaveBMP(fileName,					// path
							colorMappedBuffer,			// image
							m_width,					// pixels
							m_height,
							bitsperpixel,				// 1,4,8
							colors,						// num colors
							colormap);					// palette

		if (theBmpFile.m_errorText!="OK") {
			AfxMessageBox(theBmpFile.m_errorText, MB_ICONSTOP);
		}else {
			// load what we just saved
			LoadBMP(fileName);
			Invalidate(TRUE);
		}


		// toss our buffer...
		delete [] colorMappedBuffer;

	} else {
		AfxMessageBox("Failed to allocate space for RGB buffer");
	}

	AfxGetApp()->DoWaitCursor(-1);

}

void CMfcAppView::OnFileSavegrayas() 
{
	// note, because i'm lazy, most image data in this app
	// is handled as 24-bit images. this makes the DIB
	// conversion easier. 1,4,8, 15/16 and 32 bit DIBs are
	// significantly more difficult to handle.
	
	CString fileName;
	CString filt="JPG File (*.JPG)|*.JPG|All files (*.*)|*.*||";
    
    // OPENFILENAME - so i can get to its Help page easily
	CFileDialog fileDlg(FALSE,"*.JPG","*.JPG",NULL,filt,this);
	fileDlg.m_ofn.Flags|=OFN_FILEMUSTEXIST;
	fileDlg.m_ofn.lpstrTitle="File to save as grayscale";

	if (fileDlg.DoModal()==IDOK) {
	
		fileName=fileDlg.GetPathName();

		AfxGetApp()->DoWaitCursor(1);

		CString ext;
		ext=fileName.Right(4);

		if (!ext.CompareNoCase(".JPG"))
			SaveJPG(fileName,FALSE);

		AfxGetApp()->DoWaitCursor(-1);
	}

}

void CMfcAppView::DrawBMP()
{
	// if we don't have an image, get out of here
	if (m_buf==NULL) return;

	CDC *theDC = GetDC();

	if (theDC!=NULL) {

		CRect clientRect;
		GetClientRect(clientRect);

		// Center It
		int left = max(clientRect.left, ((clientRect.Width() - (int)m_width) / 2));
		int top = max(clientRect.top, ((clientRect.Height() - (int)m_height) / 2));

		// a 24-bit DIB is DWORD-aligned, vertically flipped and 
		// has Red and Blue bytes swapped. we already did the 
		// RGB->BGR and the flip when we read the images, now do
		// the DWORD-align

		BYTE *tmp;
		// DWORD-align for display
		tmp = JpegFile::MakeDwordAlignedBuf(m_buf,
										 m_width,
										 m_height,
										 &m_widthDW);

		// set up a DIB 
		BITMAPINFOHEADER bmiHeader;
		bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmiHeader.biWidth = m_width;
		bmiHeader.biHeight = m_height;
		bmiHeader.biPlanes = 1;
		bmiHeader.biBitCount = 24;
		bmiHeader.biCompression = BI_RGB;
		bmiHeader.biSizeImage = 0;
		bmiHeader.biXPelsPerMeter = 0;
		bmiHeader.biYPelsPerMeter = 0;
		bmiHeader.biClrUsed = 0;
		bmiHeader.biClrImportant = 0;


		// now blast it to the CDC passed in.
		// lines returns the number of lines actually displayed
		int lines = StretchDIBits(theDC->m_hDC,
									left, top,
									bmiHeader.biWidth,
									bmiHeader.biHeight,
									0,0,
									bmiHeader.biWidth,
									bmiHeader.biHeight,
									tmp,
									(LPBITMAPINFO)&bmiHeader,
									DIB_RGB_COLORS,
									SRCCOPY);

		delete [] tmp;

		CString info;
		info.Format("(%d x %d)", m_width, m_height);
		theDC->SetBkMode(TRANSPARENT);
		theDC->SetTextColor(RGB(0,0,0));
		theDC->TextOut(10,5, info);

		ReleaseDC(theDC);
	}
}

////////////////////////////////////////////////////////////////////////////
//	read a JPG to our global buffer
//

void CMfcAppView::LoadJPG(CString fileName)
{

	// m_buf is the global buffer
	if (m_buf!=NULL) {
		delete [] m_buf;
		m_buf=NULL;
	}

	// read to buffer tmp
	m_buf=JpegFile::JpegFileToRGB(fileName, &m_width, &m_height);

	//////////////////////
	// set up for display

	// do this before DWORD-alignment!!!
	// this works on packed (not DWORD-aligned) buffers
	// swap red and blue for display
	JpegFile::BGRFromRGB(m_buf, m_width, m_height);

	// vertical flip for display
	JpegFile::VertFlipBuf(m_buf, m_width * 3, m_height);

}

////////////////////////////////////////////////////////////////////////////
//	read a BMP to our global buffer
//

void CMfcAppView::LoadBMP(CString fileName)
{
	if (m_buf!=NULL) {
		delete [] m_buf;
	}

	BMPFile theBmpFile;

	m_buf=theBmpFile.LoadBMP(fileName, &m_width, &m_height);

	if ((m_buf==NULL) || (theBmpFile.m_errorText!="OK")) 
	{
		AfxMessageBox(theBmpFile.m_errorText);
		m_buf=NULL;
		return;
	}

	//////////////////////
	// set up for display

	// do this before DWORD-alignment!!!
	// this works on packed (not DWORD-aligned) buffers
	// swap red and blue for display
	JpegFile::BGRFromRGB(m_buf, m_width, m_height);

	// vertical flip for display
	JpegFile::VertFlipBuf(m_buf, m_width * 3, m_height);
}




////////////////////////////////////////////////////////////////////////////
//	save functions are generally more complex than reading functions.
//	there are many more decisions to be made for writing than for reading.


////////////////////////////////////////////////////////////////////////////
//	save a JPG

void CMfcAppView::SaveJPG(CString fileName, BOOL color)
{
	// note, because i'm lazy, most image data in this app
	// is handled as 24-bit images. this makes the DIB
	// conversion easier. 1,4,8, 15/16 and 32 bit DIBs are
	// significantly more difficult to handle.

	if (m_buf==NULL) {
		AfxMessageBox("No Image!");
		return;
	}

	// we vertical flip for display. undo that.
	JpegFile::VertFlipBuf(m_buf, m_width * 3, m_height);

	// we swap red and blue for display, undo that.
	JpegFile::BGRFromRGB(m_buf, m_width, m_height);


	// save RGB packed buffer to JPG
	BOOL ok=JpegFile::RGBToJpegFile(fileName, 
									m_buf,
									m_width,
									m_height,
									color, 
									75);			// quality value 1-100.
	if (!ok) {
		AfxMessageBox("Write Error");
	} else {
		// load what we just saved
		LoadJPG(fileName);
		Invalidate(TRUE);
	}
}

////////////////////////////////////////////////////////////////////////////
//
//	use the BMPFile class to write a 24-bit BMP file
//

void CMfcAppView::SaveBMP24(CString filename)
{
	// note, because i'm lazy, most image data in this app
	// is handled as 24-bit images. this makes the DIB
	// conversion easier. 1,4,8, 15/16 and 32 bit DIBs are
	// significantly more difficult to handle.

	if (m_buf==NULL) {
		AfxMessageBox("No Image!");
		return;
	}

	// image in m_buf is already BGR and vertically flipped, so we don't need
	// to do that for this function.

	// i really should make an RGB to BMP fn.

	BMPFile theBmpFile;
	theBmpFile.SaveBMP(filename,
						m_buf,
						m_width,
						m_height);

	if (theBmpFile.m_errorText!="OK") 
		AfxMessageBox(theBmpFile.m_errorText, MB_ICONSTOP);
	else {
		// load what we just saved
		LoadBMP(filename);
		Invalidate(TRUE);
	}


}


//	get JPG dimensions

void CMfcAppView::OnFileGetdimensionsjpg() 
{
	CString fileName;
	CString filt="JPG File (*.JPG)|*.JPG|All files (*.*)|*.*||";
    
    // OPENFILENAME - so i can get to its Help page easily
	CFileDialog fileDlg(TRUE,"*.JPG","*.JPG",NULL,filt,this);

	fileDlg.m_ofn.Flags|=OFN_FILEMUSTEXIST;
	fileDlg.m_ofn.lpstrTitle="File to examine";

	if (fileDlg.DoModal()==IDOK) {
		fileName=fileDlg.GetPathName();

		UINT width, height;
		if (JpegFile::GetJPGDimensions(fileName,
									&width,
									&height)) {
			char buf[200];
			sprintf(buf,"%d %d",width,height);
			AfxMessageBox(buf);
		} else
			AfxMessageBox("JPEG Error");
	}            
}


////////////////////////////////////////////////////////////////////////
//	instead of creating a good palette for the colormapped images
//	this just graymaps them.
//

BYTE * CMfcAppView::MakeColormappedGrayscale(BYTE *inBuf,
											  UINT inWidth,
											  UINT inHeight,
											  UINT inWidthBytes,
											  UINT colors,
											  RGBQUAD* colormap)
{
	////////////////////////////////////////////////////////////////////////	
	// allocate a buffer to colormap
	BYTE *tmp = (BYTE *)  new BYTE[inWidth * inHeight];
	if (tmp==NULL)
		return NULL;

	// force our image to use a stupid gray scale
	UINT color;
	for (color = 0;color < colors; color++) {
		colormap[color].rgbRed = color * 256 / colors;
		colormap[color].rgbGreen = color * 256 / colors;
		colormap[color].rgbBlue = color * 256 / colors;
	}


	UINT col, row;
	for (row =0; row < inHeight; row++) {

		for (col=0;col <inWidth; col++) {
			
			BYTE inRed, inBlue, inGreen;

			// src pixel
			long in_offset = row * inWidthBytes + col * 3;
			inRed = *(inBuf + in_offset + 0);
			inGreen = *(inBuf + in_offset + 1);
			inBlue = *(inBuf + in_offset + 2);

			// luminance
			int lum = (int)(.299 * (double)(inRed) + 
							.587 * (double)(inGreen) + 
							.114 * (double)(inBlue));

			// force luminance value into our range of colors
			lum = colors * lum / 256;

			// dest pixel
			long out_offset = row * inWidth + col;
			*(tmp+out_offset) = (BYTE)lum;
		}
	}

	return tmp;
}

void CMfcAppView::OnPlay() 
{
}
void CMfcAppView::OnStop() 
{
}
void CMfcAppView::OnPause() 
{
    if(m_bPause) 
		m_bPause = FALSE;
	else
		m_bPause = TRUE;
}
void CMfcAppView::OnPause(CCmdUI* pCmdUI) 
{
	if (m_bPause)
	  pCmdUI->SetCheck(pCmdUI->m_nID == ID_PAUSE);
	else
	  pCmdUI->SetCheck(pCmdUI->m_nID == 0);

}
/*
void CMfcAppView::OnViewPause(UINT nID)
{
    if(m_bPause) 
		m_bPause = FALSE;
	else
		m_bPause = TRUE;

	OnInitialUpdate();
}
*/