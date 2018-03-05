//	bmpops.cpp : implementation of the BMPFile class
//	
//	This handles the reading and writing of BMP files.
//
//

#include "stdafx.h"
#include "bmpfile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BMPFile::BMPFile()
{
	m_errorText="OK";
}

////////////////////////////////////////////////////////////////////////////
//	load a .BMP file - 1,4,8,24 bit
//
//	allocates and returns an RGB buffer containing the image.
//	modifies width and height accordingly - NULL, 0, 0 on error

BYTE * BMPFile::LoadBMP(CString fileName, 
						UINT *width, 
						UINT *height)
{
	BITMAP inBM;

	BYTE m1,m2;
    long filesize;
    short res1,res2;
    long pixoff;
    long bmisize;                    
    long compression;
    unsigned long sizeimage;
    long xscale, yscale;
    long colors;
    long impcol;
    

	BYTE *outBuf=NULL;
	
	// for safety
	*width=0; *height=0;

	// init
	m_errorText="OK";
	m_bytesRead=0;

	FILE *fp;
	
	fp=fopen(fileName,"rb");
	if (fp==NULL) {
		CString msg;                    
		msg="Can't open file for reading :\n"+fileName;
		m_errorText=msg;
		return NULL;
	} else {
	    long rc;
		rc=fread((BYTE  *)&(m1),1,1,fp); m_bytesRead+=1;
		if (rc==-1) {m_errorText="Read Error!"; fclose(fp); return NULL;}

		rc=fread((BYTE  *)&(m2),1,1,fp); m_bytesRead+=1;
		if (rc==-1) m_errorText="Read Error!";
		if ((m1!='B') || (m2!='M')) {
			m_errorText="Not a valid BMP File";
			fclose(fp);
			return NULL;
        }
        
		////////////////////////////////////////////////////////////////////////////
		//
		//	read a ton of header stuff

		rc=fread((long  *)&(filesize),4,1,fp); m_bytesRead+=4;
		if (rc!=1) {m_errorText="Read Error!"; fclose(fp); return NULL;}

		rc=fread((int  *)&(res1),2,1,fp); m_bytesRead+=2;
		if (rc!=1) {m_errorText="Read Error!"; fclose(fp); return NULL;}

		rc=fread((int  *)&(res2),2,1,fp); m_bytesRead+=2;
		if (rc!=1) {m_errorText="Read Error!"; fclose(fp); return NULL;}

		rc=fread((long  *)&(pixoff),4,1,fp); m_bytesRead+=4;
		if (rc!=1) {m_errorText="Read Error!"; fclose(fp); return NULL;}

		rc=fread((long  *)&(bmisize),4,1,fp); m_bytesRead+=4;
		if (rc!=1) {m_errorText="Read Error!"; fclose(fp); return NULL;}

		rc=fread((long  *)&(inBM.bmWidth),4,1,fp);	 m_bytesRead+=4;
		if (rc!=1) {m_errorText="Read Error!"; fclose(fp); return NULL;}

		rc=fread((long  *)&(inBM.bmHeight),4,1,fp); m_bytesRead+=4;
		if (rc!=1) {m_errorText="Read Error!"; fclose(fp); return NULL;}

		rc=fread((int  *)&(inBM.bmPlanes),2,1,fp); m_bytesRead+=2;
		if (rc!=1) {m_errorText="Read Error!"; fclose(fp); return NULL;}

		rc=fread((int  *)&(inBM.bmBitsPixel),2,1,fp); m_bytesRead+=2;
		if (rc!=1) {m_errorText="Read Error!"; fclose(fp); return NULL;}

		rc=fread((long  *)&(compression),4,1,fp); m_bytesRead+=4;
		if (rc!=1) {m_errorText="Read Error!"; fclose(fp); return NULL;}

		rc=fread((long  *)&(sizeimage),4,1,fp); m_bytesRead+=4;
		if (rc!=1) {m_errorText="Read Error!"; fclose(fp); return NULL;}

		rc=fread((long  *)&(xscale),4,1,fp); m_bytesRead+=4;
		if (rc!=1) {m_errorText="Read Error!"; fclose(fp); return NULL;}

		rc=fread((long  *)&(yscale),4,1,fp); m_bytesRead+=4;
		if (rc!=1) {m_errorText="Read Error!"; fclose(fp); return NULL;}

		rc=fread((long  *)&(colors),4,1,fp); m_bytesRead+=4;
		if (rc!=1) {m_errorText="Read Error!"; fclose(fp); return NULL;}

		rc=fread((long  *)&(impcol),4,1,fp); m_bytesRead+=4;
		if (rc!=1) {m_errorText="Read Error!"; fclose(fp); return NULL;}

		////////////////////////////////////////////////////////////////////////////
		//	i don't do RLE files

		if (compression!=BI_RGB) {
	    	m_errorText="This is a compressed file.";
	    	fclose(fp);
	    	return NULL;
	    }

		if (colors == 0) {
			colors = 1 << inBM.bmBitsPixel;
		}


		////////////////////////////////////////////////////////////////////////////
		// read colormap

		RGBQUAD *colormap = NULL;

		switch (inBM.bmBitsPixel) {
		case 24:
			break;
			// read pallete 
		case 1:
		case 4:
		case 8:
			colormap = new RGBQUAD[colors];
			if (colormap==NULL) {
				fclose(fp);
				m_errorText="Out of memory";
				return NULL;
			}

			int i;
			for (i=0;i<colors;i++) {
				BYTE r,g,b, dummy;

				rc=fread((BYTE *)&(b),1,1,fp);
				m_bytesRead++;
				if (rc!=1) {
					m_errorText="Read Error!";	
					delete [] colormap;
					fclose(fp);
					return NULL;
				}

				rc=fread((BYTE  *)&(g),1,1,fp); 
				m_bytesRead++;
				if (rc!=1) {
					m_errorText="Read Error!";	
					delete [] colormap;
					fclose(fp);
					return NULL;
				}

				rc=fread((BYTE  *)&(r),1,1,fp); 
				m_bytesRead++;
				if (rc!=1) {
					m_errorText="Read Error!";	
					delete [] colormap;
					fclose(fp);
					return NULL;
				}


				rc=fread((BYTE  *)&(dummy),1,1,fp); 
				m_bytesRead++;
				if (rc!=1) {
					m_errorText="Read Error!";	
					delete [] colormap;
					fclose(fp);
					return NULL;
				}

				colormap[i].rgbRed=r;
				colormap[i].rgbGreen=g;
				colormap[i].rgbBlue=b;
			}
			break;
		}


		if ((long)m_bytesRead>pixoff) {
			fclose(fp);
			m_errorText="Corrupt palette";
			delete [] colormap;
			fclose(fp);
			return NULL;
		}

		while ((long)m_bytesRead<pixoff) {
			char dummy;
			fread(&dummy,1,1,fp);
			m_bytesRead++;
		}

		int w=inBM.bmWidth;
		int h=inBM.bmHeight;

		// set the output params
		*width=w;
		*height=h;

		long row_size = w * 3;

		long bufsize = (long)w * 3 * (long)h;

		////////////////////////////////////////////////////////////////////////////
		// alloc our buffer

		outBuf=(BYTE *) new BYTE [bufsize];
		if (outBuf==NULL) {
			m_errorText="Memory alloc Failed";
		} else {

			////////////////////////////////////////////////////////////////////////////
			//	read it

			long row=0;
			long rowOffset=0;

			// read rows in reverse order
			for (row=inBM.bmHeight-1;row>=0;row--) {

				// which row are we working on?
				rowOffset=(long unsigned)row*row_size;						      

				if (inBM.bmBitsPixel==24) {

					for (int col=0;col<w;col++) {
						long offset = col * 3;
						char pixel[3];

						if (fread((void  *)(pixel),1,3,fp)==3) {
							// we swap red and blue here
							*(outBuf + rowOffset + offset + 0)=pixel[2];		// r
							*(outBuf + rowOffset + offset + 1)=pixel[1];		// g
							*(outBuf + rowOffset + offset + 2)=pixel[0];		// b
						}

					}

					m_bytesRead+=row_size;
					
					// read DWORD padding
					while ((m_bytesRead-pixoff)&3) {
						char dummy;
						if (fread(&dummy,1,1,fp)!=1) {
							m_errorText="Read Error";
							delete [] outBuf;
							fclose(fp);
							return NULL;
						}

						m_bytesRead++;
					}
 
					
				} else {	// 1, 4, or 8 bit image

					////////////////////////////////////////////////////////////////
					// pixels are packed as 1 , 4 or 8 bit vals. need to unpack them

					int bit_count = 0;
					UINT mask = (1 << inBM.bmBitsPixel) - 1;

					BYTE inbyte=0;

					for (int col=0;col<w;col++) {
						
						int pix=0;

						// if we need another byte
						if (bit_count <= 0) {
							bit_count = 8;
							if (fread(&inbyte,1,1,fp)!=1) {
								m_errorText="Read Error";
								delete [] outBuf;
								delete [] colormap;
								fclose(fp);
								return NULL;
							}
							m_bytesRead++;
						}

						// keep track of where we are in the bytes
						bit_count -= inBM.bmBitsPixel;
						pix = ( inbyte >> bit_count) & mask;

						// lookup the color from the colormap - stuff it in our buffer
						// swap red and blue
						*(outBuf + rowOffset + col * 3 + 2) = colormap[pix].rgbBlue;
						*(outBuf + rowOffset + col * 3 + 1) = colormap[pix].rgbGreen;
						*(outBuf + rowOffset + col * 3 + 0) = colormap[pix].rgbRed;
					}

					// read DWORD padding
					while ((m_bytesRead-pixoff)&3) {
						char dummy;
						if (fread(&dummy,1,1,fp)!=1) {
							m_errorText="Read Error";
							delete [] outBuf;
							if (colormap)
								delete [] colormap;
							fclose(fp);
							return NULL;
						}
						m_bytesRead++;
					}
				}
			}
		
		}

		if (colormap) {
			delete [] colormap;
		}

		fclose(fp);

    }

	return outBuf;
}

////////////////////////////////////////////////////////////////////////////
//	write a 24-bit BMP file
//
//	image MUST be a packed buffer (not DWORD-aligned)
//	image MUST be vertically flipped !
//	image MUST be BGR, not RGB !
//

void BMPFile::SaveBMP(CString fileName,		// output path
							BYTE * buf,				// BGR buffer
							UINT width,				// pixels
							UINT height)
{
	short res1=0;
    short res2=0;
    long pixoff=54;
    long compression=0;
    long cmpsize=0;
    long colors=0;
    long impcol=0;
	char m1='B';
	char m2='M';

	m_errorText="OK";

	DWORD widthDW = WIDTHBYTES(width * 24);

	long bmfsize=sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
  							widthDW * height;	
	long byteswritten=0;

	BITMAPINFOHEADER header;
  	header.biSize=40; 						// header size
	header.biWidth=width;
	header.biHeight=height;
	header.biPlanes=1;
	header.biBitCount=24;					// RGB encoded, 24 bit
	header.biCompression=BI_RGB;			// no compression
	header.biSizeImage=0;
	header.biXPelsPerMeter=0;
	header.biYPelsPerMeter=0;
	header.biClrUsed=0;
	header.biClrImportant=0;

	FILE *fp;	
	fp=fopen(fileName,"wb");
	if (fp==NULL) {
		m_errorText="Can't open file for writing";
		return;
	}

	// should probably check for write errors here...
	
	fwrite((BYTE  *)&(m1),1,1,fp); byteswritten+=1;
	fwrite((BYTE  *)&(m2),1,1,fp); byteswritten+=1;
	fwrite((long  *)&(bmfsize),4,1,fp);	byteswritten+=4;
	fwrite((int  *)&(res1),2,1,fp); byteswritten+=2;
	fwrite((int  *)&(res2),2,1,fp); byteswritten+=2;
	fwrite((long  *)&(pixoff),4,1,fp); byteswritten+=4;

	fwrite((BITMAPINFOHEADER *)&header,sizeof(BITMAPINFOHEADER),1,fp);
	byteswritten+=sizeof(BITMAPINFOHEADER);
	
	long row=0;
	long rowidx;
	long row_size;
	row_size=header.biWidth*3;
    long rc;
	for (row=0;row<header.biHeight;row++) {
		rowidx=(long unsigned)row*row_size;						      

		// write a row
		rc=fwrite((void  *)(buf+rowidx),row_size,1,fp);
		if (rc!=1) {
			m_errorText="fwrite error\nGiving up";
			break;
		}
		byteswritten+=row_size;	

		// pad to DWORD
		for (DWORD count=row_size;count<widthDW;count++) {
			char dummy=0;
			fwrite(&dummy,1,1,fp);
			byteswritten++;							  
		}

	}
           
	fclose(fp);
}


////////////////////////////////////////////////////////////////////////////////////
//
//	1,4,8 bit BMP stuff
//
//	if you have a color-mapped image and a color map...
//
//	the BMP saving code in SaveColorMappedBMP modified from Programming 
//	for Graphics Files in C and C++, by John Levine.

void BMPFile::SaveBMP(CString fileName, 			// output path
								 BYTE * colormappedbuffer,	// one BYTE per pixel colomapped image
								 UINT width,
								 UINT height,
 								 int bitsperpixel,			// 1, 4, 8
								 int colors,				// number of colors (number of RGBQUADs)
								 RGBQUAD *colormap)			// array of RGBQUADs 
{
	int datasize, cmapsize, byteswritten, row, col;

	m_errorText="OK";

	if (bitsperpixel == 24) {
		// the routines could be combined, but i don't feel like it
		m_errorText="We don't do 24-bit files in here, sorry";
		return;
	} else
		cmapsize = colors * 4;

	datasize = BMP_PIXELSIZE(width, height, bitsperpixel);

	long filesize = BMP_HEADERSIZE + cmapsize + datasize;
	int res1, res2;
	res1 = res2 = 0;

	long pixeloffset = BMP_HEADERSIZE + cmapsize;

	int bmisize = 40;
	long cols = width;
	long rows = height;
	WORD planes = 1;
	long compression =0;
	long cmpsize = datasize;
	long xscale = 0;
	long yscale = 0;
	long impcolors = colors;

	FILE *fp;
	fp = fopen(fileName, "wb");
	if (fp==NULL) {
		m_errorText="Can't Open";
		return;
	}
	char bm[2];
	bm[0]='B';
	bm[1]='M';

	// header stuff
	BITMAPFILEHEADER bmfh;
	bmfh.bfType=*(WORD *)&bm; 
    bmfh.bfSize= filesize; 
    bmfh.bfReserved1=0; 
    bmfh.bfReserved2=0; 
    bmfh.bfOffBits=pixeloffset; 

	fwrite(&bmfh, sizeof (BITMAPFILEHEADER), 1, fp);


	BITMAPINFOHEADER bmih;
	bmih.biSize = bmisize; 
	bmih.biWidth = cols; 
	bmih.biHeight = rows; 
	bmih.biPlanes = planes; 
	bmih.biBitCount =bitsperpixel;
	bmih.biCompression = compression; 
	bmih.biSizeImage = cmpsize; 
	bmih.biXPelsPerMeter = xscale; 
	bmih.biYPelsPerMeter = yscale; 
	bmih.biClrUsed = colors;
	bmih.biClrImportant = impcolors;
	
	fwrite(&bmih, sizeof (BITMAPINFOHEADER), 1, fp);

	if (cmapsize) {
		int i;
		for (i = 0; i< colors; i++) {
			putc(colormap[i].rgbRed, fp);
			putc(colormap[i].rgbGreen, fp);
			putc(colormap[i].rgbBlue, fp);
			putc(0, fp);	// dummy
		}
	}

	byteswritten = BMP_HEADERSIZE + cmapsize;

	for (row = 0; row< (int)height; row++) {
		int pixbuf;
		int nbits = 0;

		for (col =0 ; col < (int)width; col++) {
			int offset = row * width + col;	// offset into our color-mapped RGB buffer
			BYTE pval = *(colormappedbuffer + offset);

			pixbuf = (pixbuf << bitsperpixel) | pval;

			nbits += bitsperpixel;

			if (nbits > 8) {
				m_errorText="Error : nBits > 8????";
				fclose(fp);
				return;
			}

			if (nbits == 8) {
				putc(pixbuf, fp);
				pixbuf=0;
				nbits=0;
				byteswritten++;
			}
		} // cols

		if (nbits > 0) {
			putc(pixbuf, fp);		// write partially filled byte
			byteswritten++;
		}

		// DWORD align
		while ((byteswritten -pixeloffset) & 3) {
			putc(0, fp);
			byteswritten++;
		}

	}	//rows

	if (byteswritten!=filesize) {
		m_errorText="byteswritten != filesize";
	}

	fclose(fp);
}
