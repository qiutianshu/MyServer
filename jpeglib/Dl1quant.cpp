//
//	This has been modified from Dennis Lee's original version
//	

/*
 * DL1 Quantization
 * ================
 *
 * File: dl1quant.c
 * Author: Dennis Lee   E-mail: denlee@ecf.utoronto.ca
 *
 * Copyright (C) 1993-1997 Dennis Lee
 *
 * C implementation of DL1 Quantization.
 * DL1 Quantization is a 2-pass color quantizer optimized for speed.
 * The method was designed around the steps required by a 2-pass
 * quantizer and constructing a model that would require the least
 * amount of extra work.  The resulting method is extremely fast --
 * about half the speed of a memcpy.  That should make DL1 Quant the
 * fastest 2-pass color quantizer.
 *
 * This quantizer's quality is also among the best, slightly
 * better than Wan et al's marginal variance based quantizer.  For
 * more on DL1 Quant's performance and other related information,
 * see DLQUANT.TXT included in this distribution.
 *
 *
 * NOTES
 * =====
 *
 * The dithering code is based on code from the IJG's jpeg library.
 *
 * This source code may be freely copied, modified, and redistributed,
 * provided this copyright notice is attached.
 * Compiled versions of this code, modified or not, are free for
 * personal use.  Compiled versions used in distributed software
 * is also free, but a notification must be sent to the author.
 * An e-mail to denlee@ecf.utoronto.ca will do.
 *
 */

#include "stdafx.h"

#ifdef __cplusplus
extern "C" {
#endif


#include <stdlib.h>
#include "dl1quant.h"

//#define FAST        /* improves speed but uses a lot of memory */
#define QUAL1       /* slightly improves quality */
//#define QUAL2       /* slightly improves quality */

/* define *one* of the following dither options */
//#define DITHER1     /* 1-val error diffusion dither */
#define DITHER2     /* 2-val error diffusion dither */
//#define DITHER4     /* 4-val error diffusion dither (Floyd-Steinberg) */

#define DITHER_MAX  20

LOCAL uchar palette[3][256];
LOCAL CUBE *rgb_table[6];
LOCAL ushort r_offset[256], g_offset[256], b_offset[256];
LOCAL CLOSEST_INFO c_info;
LOCAL int tot_colors, pal_index;
LOCAL ulong *squares;
LOCAL FCUBE *heap = NULL;
LOCAL short *dl_image = NULL;

/* returns 1 on success, 0 on failure */
GLOBAL int dl1quant(uchar *inbuf, 
					uchar *outbuf, 
					int width, 
					int height, 
					int quant_to,
					int dither, 
					uchar userpal[3][256]) {

	dlq_init();

    if (dlq_start() == 0) {
		dlq_finish();
		return 0;
	}

    if (build_table(inbuf, (ulong)width * (ulong)height) == 0) {
		dlq_finish();
		return 0;
    }
    
	reduce_table(quant_to);
    set_palette(0, 0);
    
	if (quantize_image(inbuf, outbuf, width, height, dither) == 0) {
		dlq_finish();
		return 0;
    }

    dlq_finish();
    copy_pal(userpal);

    return 1;
}

LOCAL void copy_pal(uchar userpal[3][256]) 
{
    int i;

    for (i = 0; i < 256; i++) {
		userpal[0][i] = palette[0][i];
		userpal[1][i] = palette[1][i];
		userpal[2][i] = palette[2][i];
    }
}

LOCAL void dlq_init(void) 
{
    int i;

	for (i=0;i<6;i++) {
		rgb_table[i]=NULL;
	}

	tot_colors=0;
	pal_index=0;

	heap = NULL;
	dl_image = NULL;

    for (i = 0; i < 256; i++) {
		r_offset[i] = (i & 128) << 7 | (i & 64) << 5 | (i & 32) << 3 |
				  (i & 16)  << 1 | (i & 8)  >> 1;
		g_offset[i] = (i & 128) << 6 | (i & 64) << 4 | (i & 32) << 2 |
				  (i & 16)  << 0 | (i & 8)  >> 2;
		b_offset[i] = (i & 128) << 5 | (i & 64) << 3 | (i & 32) << 1 |
				  (i & 16)  >> 1 | (i & 8)  >> 3;
    }


	c_info.palette_index=0;
	c_info.red=0;
	c_info.green=0;
	c_info.blue=0;
    c_info.distance=0;

    for (i = (-255); i <= 255; i++)
		c_info.squares[i+255] = i*i;

	for (i=0;i<256;i++) {
		palette[0][i]=0;
		palette[1][i]=0;
		palette[2][i]=0;
	}

    squares = c_info.squares + 255;
}

/* returns 1 on success, 0 on failure */
LOCAL int dlq_start(void) 
{
    int i;

    rgb_table[0] = (CUBE *) new CUBE(1);
    rgb_table[1] = (CUBE *) new CUBE(8);
    rgb_table[2] = (CUBE *) new CUBE(64);
    rgb_table[3] = (CUBE *) new CUBE(512);
    rgb_table[4] = (CUBE *) new CUBE(4096);
    rgb_table[5] = (CUBE *) new CUBE(32768);

    for (i = 0; i <= 5; i++)
		if (rgb_table[i] == NULL)
		    return 0;
   
    pal_index = 0;
    return 1;
}

LOCAL void dlq_finish(void) 
{
	int i;

	for (i=0;i<6;i++) {
		if (rgb_table[i] != NULL) {
			delete rgb_table[i];
			rgb_table[i]=NULL;
		}
	}

    if (heap != NULL) {
		delete heap;
		heap=NULL;
	}
    
	if (dl_image != NULL) {
		delete dl_image;
		dl_image=NULL;
	}

	memset(&c_info, 0, sizeof(CLOSEST_INFO));

	tot_colors=pal_index=0;

}

/* returns 1 on success, 0 on failure */
LOCAL int build_table(uchar *image, ulong pixels) 
{
    ulong i=0;
	ulong index=0;
	ulong cur_count=0;
	ulong head=0;
	ulong tail=0;
    slong j=0;

    heap = (FCUBE *) new BYTE(sizeof(FCUBE) * 32769);
    if (heap == NULL)
		return 0;

#ifdef FAST
    dl_image = new BYTE(sizeof(short) * pixels);
    if (dl_image == NULL)
		return 0;
#endif

    for (i = 0; i < pixels; i++) {
#ifdef FAST
		dl_image[i] = index = r_offset[image[0]] + g_offset[image[1]] + b_offset[image[2]];
#else
		index = r_offset[image[0]] + g_offset[image[1]] + b_offset[image[2]];
#endif
#ifdef QUAL1
		rgb_table[5][index].r += image[0];
		rgb_table[5][index].g += image[1];
		rgb_table[5][index].b += image[2];
#endif
		rgb_table[5][index].pixel_count++;
		image += 3;
    }

    tot_colors = 0;
		for (i = 0; i < 32768; i++) {
		cur_count = rgb_table[5][i].pixel_count;
		if (cur_count) {
			heap[++tot_colors].level = 5;
			heap[tot_colors].index = (ushort)i;
			rgb_table[5][i].pixels_in_cube = cur_count;
#ifndef QUAL1
			rgb_table[5][i].r = cur_count * (((i & 0x4000) >> 7 |
					(i & 0x0800) >> 5 | (i & 0x0100) >> 3 |
					(i & 0x0020) >> 1 | (i & 0x0004) << 1) + 4);
			rgb_table[5][i].g = cur_count * (((i & 0x2000) >> 6 |
					(i & 0x0400) >> 4 | (i & 0x0080) >> 2 |
					(i & 0x0010) >> 0 | (i & 0x0002) << 2) + 4);
			rgb_table[5][i].b = cur_count * (((i & 0x1000) >> 5 |
					(i & 0x0200) >> 3 | (i & 0x0040) >> 1 |
					(i & 0x0008) << 1 | (i & 0x0001) << 3) + 4);
#endif
			head = i;
			for (j = 4; j >= 0; j--) {
				tail = head & 0x7;
				head >>= 3;
				rgb_table[j][head].pixels_in_cube += cur_count;
				rgb_table[j][head].children |= 1 << tail;
			}
		}
    }

    for (i = tot_colors; i > 0; i--)
		fixheap(i);

    return 1;
}

LOCAL void fixheap(ulong id) 
{
    uchar thres_level = heap[id].level;
    ulong thres_index = heap[id].index;
	ulong index=0;
	ulong half_totc = tot_colors >> 1;
	ulong thres_val = rgb_table[thres_level][thres_index].pixels_in_cube;

    while (id <= half_totc) {
		index = id << 1;

		if (index < (ulong)tot_colors)
			if (rgb_table[heap[index].level][heap[index].index].pixels_in_cube
			  > rgb_table[heap[index+1].level][heap[index+1].index].pixels_in_cube)
			index++;

		if (thres_val <= rgb_table[heap[index].level][heap[index].index].pixels_in_cube)
			break;
		else {
			heap[id] = heap[index];
			id = index;
		}
    }
    heap[id].level = thres_level;
    heap[id].index = (ushort)thres_index;
}

LOCAL void reduce_table(int num_colors) 
{
	while (tot_colors > num_colors) {

		uchar tmp_level = heap[1].level, t_level = max(0,tmp_level - 1);
		ulong tmp_index = heap[1].index, t_index = tmp_index >> 3;

		if (rgb_table[t_level][t_index].pixel_count)
			heap[1] = heap[tot_colors--];
		else {
			heap[1].level = t_level;
			heap[1].index = (ushort)t_index;
		}

		rgb_table[t_level][t_index].pixel_count += rgb_table[tmp_level][tmp_index].pixel_count;
		rgb_table[t_level][t_index].r += rgb_table[tmp_level][tmp_index].r;
		rgb_table[t_level][t_index].g += rgb_table[tmp_level][tmp_index].g;
		rgb_table[t_level][t_index].b += rgb_table[tmp_level][tmp_index].b;
		rgb_table[t_level][t_index].children &= ~(1 << (tmp_index & 0x7));

		fixheap(1);
    }
}

LOCAL void set_palette(int index, int level) 
{
    int i;

    if (rgb_table[level][index].children) {
		for (i = 7; i >= 0; i--) {
			if (rgb_table[level][index].children & (1 << i)) {
				set_palette((index << 3) + i, level + 1);
			}
		}
	}

    if (rgb_table[level][index].pixel_count) {
		ulong r_sum, g_sum, b_sum, sum;

		rgb_table[level][index].palette_index = pal_index;
		
		r_sum = rgb_table[level][index].r;
		g_sum = rgb_table[level][index].g;
		b_sum = rgb_table[level][index].b;
		
		sum = rgb_table[level][index].pixel_count;

		palette[0][pal_index] = (BYTE)((r_sum + (sum >> 1)) / sum);
		palette[1][pal_index] = (BYTE)((g_sum + (sum >> 1)) / sum);
		palette[2][pal_index] = (BYTE)((b_sum + (sum >> 1)) / sum);

		pal_index++;
    }
}

LOCAL void closest_color(int index, int level) 
{
    int i;

    if (rgb_table[level][index].children) {
		for (i = 7; i >= 0; i--) {
			if (rgb_table[level][index].children & (1 << i)) {
				closest_color((index << 3) + i, level + 1);
			}
		}
	}

    if (rgb_table[level][index].pixel_count) {
		slong dist, r_dist, g_dist, b_dist;
		uchar pal_num = rgb_table[level][index].palette_index;

		/* Determine if this color is "closest". */
		r_dist = palette[0][pal_num] - c_info.red;
		g_dist = palette[1][pal_num] - c_info.green;
		b_dist = palette[2][pal_num] - c_info.blue;
		
		dist = squares[r_dist] + squares[g_dist] + squares[b_dist];

		if (dist < (slong)c_info.distance) {
			c_info.distance = dist;
			c_info.palette_index = pal_num;
		}
    }
}

/* returns 1 on success, 0 on failure */
LOCAL int quantize_image(uchar *in, uchar *out, int width, int height, int dither) 
{
	if (!dither) {
		ulong i=0;
		ulong pixels = width * height;
		ushort level=0;
		ushort index=0;
		uchar tmp_r=0;
		uchar tmp_g=0;
		uchar tmp_b=0;
		uchar cube=0;
		uchar *lookup=NULL;

		lookup = (BYTE*)new BYTE(sizeof(char) * 32768);
		if (lookup == NULL)
			return 0;

		for (i = 0; i < 32768; i++)
			if (rgb_table[5][i].pixel_count) {
			tmp_r = (BYTE)((i & 0x4000) >> 7 | (i & 0x0800) >> 5 |
				(i & 0x0100) >> 3 | (i & 0x0020) >> 1 |
				(i & 0x0004) << 1);
			tmp_g = (BYTE)((i & 0x2000) >> 6 | (i & 0x0400) >> 4 |
				(i & 0x0080) >> 2 | (i & 0x0010) >> 0 |
				(i & 0x0002) << 2);
			tmp_b = (BYTE)((i & 0x1000) >> 5 | (i & 0x0200) >> 3 |
				(i & 0x0040) >> 1 | (i & 0x0008) << 1 |
				(i & 0x0001) << 3);
	#ifdef QUAL2
			lookup[i] = bestcolor(tmp_r, tmp_g, tmp_b);
	#else
			c_info.red   = tmp_r + 4;
			c_info.green = tmp_g + 4;
			c_info.blue  = tmp_b + 4;
			level = 0;
			index = 0;
			for (;;) {
				cube = (tmp_r&128) >> 5 | (tmp_g&128) >> 6 | (tmp_b&128) >> 7;
				if ((rgb_table[level][index].children & (1 << cube)) == 0) {
				c_info.distance = (ULONG)~0L;
				closest_color(index, level);
				lookup[i] = c_info.palette_index;
				break;
				}
				level++;
				index = (index << 3) + cube;
				tmp_r <<= 1;
				tmp_g <<= 1;
				tmp_b <<= 1;
			}
	#endif
			}

		for (i = 0; i < pixels; i++) {
	#ifdef FAST
			out[i] = lookup[dl_image[i]];
	#else
			out[i] = lookup[r_offset[in[0]] + g_offset[in[1]] + b_offset[in[2]]];
			in += 3;
	#endif
		}

		delete lookup;

	} else { // dither

	#if defined(DITHER2) || defined(DITHER4)
		slong i=0;
		slong j=0;
		slong r_pix=0;
		slong g_pix=0;
		slong b_pix=0;
		slong offset=0;
		slong dir=0;
		slong two_val=0;
		slong odd_scanline = 0;
		slong err_len = (width + 2) * 3;

		uchar *range_tbl = NULL; 
		uchar *range = NULL;

		sshort *lookup = NULL;
		sshort *erowerr = NULL;
		sshort *orowerr = NULL;
		sshort *thisrowerr = NULL;
		sshort *nextrowerr = NULL;

		schar *dith_max_tbl = NULL;
		schar *dith_max = NULL;

		lookup = (sshort *)new BYTE(sizeof(short) * 32768);
		erowerr = (sshort *)new BYTE(sizeof(short) * err_len);
		orowerr = (sshort *)new BYTE(sizeof(short) * err_len);
		range_tbl = (BYTE*)new BYTE(3 * 256);
		range = range_tbl + 256;
		dith_max_tbl= (schar *)new BYTE(512);
		dith_max = dith_max_tbl + 256;

		if (range_tbl == NULL || lookup == NULL ||
			erowerr == NULL || orowerr == NULL || dith_max_tbl == NULL) {
			if (range_tbl != NULL) {
				delete range_tbl;
				range_tbl=NULL;
			}
			if (lookup != NULL) {
				delete lookup;
				lookup=NULL;
			}
			if (erowerr != NULL) {
				delete erowerr;
				erowerr=NULL;
			}
			if (orowerr != NULL) {
				delete orowerr;
				orowerr=NULL;
			}
			if (dith_max_tbl != NULL) {
				delete dith_max_tbl;
				dith_max_tbl=NULL;
			}
			return 0;
		}

		for (i = 0; i < err_len; i++)
			erowerr[i] = 0;

		for (i = 0; i < 32768; i++)
			lookup[i] = -1;

		for (i = 0; i < 256; i++) {
			range_tbl[i] = 0;
			range_tbl[i + 256] = (uchar) i;
			range_tbl[i + 512] = 255;
		}

		for (i = 0; i < 256; i++) {
			dith_max_tbl[i] = -DITHER_MAX;
			dith_max_tbl[i + 256] = DITHER_MAX;
		}
		for (i = -DITHER_MAX; i <= DITHER_MAX; i++)
			dith_max_tbl[i + 256] = (schar)i;

		for (i = 0 ; i < height; i++) {
			if (odd_scanline) {
				dir = -1;
				in  += (width - 1) * 3;
				out += (width - 1);
				thisrowerr = orowerr + 3;
				nextrowerr = erowerr + width * 3;
			} else {
				dir = 1;
				thisrowerr = erowerr + 3;
				nextrowerr = orowerr + width * 3;
			}

			nextrowerr[0] = nextrowerr[1] = nextrowerr[2] = 0;
			
			for (j = 0; j < width; j++) {

		#ifdef DITHER2
				r_pix = range[(thisrowerr[0] >> 1) + in[0]];
				g_pix = range[(thisrowerr[1] >> 1) + in[1]];
				b_pix = range[(thisrowerr[2] >> 1) + in[2]];
		#else
				r_pix = range[((thisrowerr[0] + 8) >> 4) + in[0]];
				g_pix = range[((thisrowerr[1] + 8) >> 4) + in[1]];
				b_pix = range[((thisrowerr[2] + 8) >> 4) + in[2]];
		#endif
				offset = (r_pix&248) << 7 | (g_pix&248) << 2 | b_pix >> 3;
				if (lookup[offset] < 0)
					lookup[offset] = bestcolor(r_pix, g_pix, b_pix);
				*out = (BYTE)lookup[offset];
				r_pix = dith_max[r_pix - palette[0][lookup[offset]]];
				g_pix = dith_max[g_pix - palette[1][lookup[offset]]];
				b_pix = dith_max[b_pix - palette[2][lookup[offset]]];

		#ifdef DITHER2
				nextrowerr[0  ]  = (short)r_pix;
				thisrowerr[0+3] += (short)r_pix;
				nextrowerr[1  ]  = (short)g_pix;
				thisrowerr[1+3] += (short)g_pix;
				nextrowerr[2  ]  = (short)b_pix;
				thisrowerr[2+3] += (short)b_pix;
		#else
				two_val = r_pix * 2;
				nextrowerr[0-3]  = r_pix;
				r_pix += two_val;
				nextrowerr[0+3] += r_pix;
				r_pix += two_val;
				nextrowerr[0  ] += r_pix;
				r_pix += two_val;
				thisrowerr[0+3] += r_pix;
				two_val = g_pix * 2;
				nextrowerr[1-3]  = g_pix;
				g_pix += two_val;
				nextrowerr[1+3] += g_pix;
				g_pix += two_val;
				nextrowerr[1  ] += g_pix;
				g_pix += two_val;
				thisrowerr[1+3] += g_pix;
				two_val = b_pix * 2;
				nextrowerr[2-3]  = b_pix;
				b_pix += two_val;
				nextrowerr[2+3] += b_pix;
				b_pix += two_val;
				nextrowerr[2  ] += b_pix;
				b_pix += two_val;
				thisrowerr[2+3] += b_pix;
		#endif
				thisrowerr += 3;
				nextrowerr -= 3;
				in  += dir * 3;
				out += dir;
			}

			if ((i % 2) == 1) {
				in  += (width + 1) * 3;
				out += (width + 1);
			}

			odd_scanline = !odd_scanline;
		}

		delete range_tbl;
		delete lookup;
		delete erowerr;
		delete orowerr;
		delete dith_max_tbl;
	#else
		slong i =0; 
		slong j=0; 
		slong r_pix=0;
		slong g_pix=0;
		slong b_pix=0;
		slong r_err=0;
		slong g_err=0;
		slong b_err=0;
		slong offset=0;

		uchar *range_tbl = (BYTE*)new BYTE(3 * 256), *range = range_tbl + 256;
		sshort *lookup = (sshort *)new BYTE(sizeof(short) * 32768);

		if (range_tbl == NULL || lookup == NULL) {
			if (range_tbl != NULL)
			delete range_tbl;
			if (lookup != NULL)
			delete lookup;
			return 0;
		}

		for (i = 0; i < 32768; i++)
			lookup[i] = -1;

		for (i = 0; i < 256; i++) {
			range_tbl[i] = 0;
			range_tbl[i + 256] = (uchar) i;
			range_tbl[i + 512] = 255;
		}

		for (i = 0; i < height; i++) {
			r_err = g_err = b_err = 0;
			for (j = width - 1; j >= 0; j--) {
				r_pix = range[(r_err >> 1) + in[0]];
				g_pix = range[(g_err >> 1) + in[1]];
				b_pix = range[(b_err >> 1) + in[2]];
				
				offset = (r_pix&248) << 7 | (g_pix&248) << 2 | b_pix >> 3;

				if (lookup[offset] < 0)
					lookup[offset] = bestcolor(r_pix, g_pix, b_pix);

				*out++ = (BYTE)lookup[offset];

				r_err = r_pix - palette[0][lookup[offset]];
				g_err = g_pix - palette[1][lookup[offset]];
				b_err = b_pix - palette[2][lookup[offset]];

				in += 3;
			}
		}

		delete range_tbl;
		delete lookup;
	#endif
    }

    return 1;
}

LOCAL int bestcolor(int r, int g, int b) 
{
    ulong i=0;
	ulong bestcolor=0;
	ulong curdist=0;
	ulong mindist=0;
    slong rdist=0;
	slong gdist=0;
	slong bdist=0;

    r = (r & 248) + 4;
    g = (g & 248) + 4;
    b = (b & 248) + 4;
    mindist = 200000;

	for (i = 0; i < (ulong)tot_colors; i++) {
		rdist = palette[0][i] - r;
		gdist = palette[1][i] - g;
		bdist = palette[2][i] - b;
		curdist = squares[rdist] + squares[gdist] + squares[bdist];

		if (curdist < mindist) {
			mindist = curdist;
			bestcolor = i;
		}
	}
    return bestcolor;
}

#ifdef __cplusplus
}
#endif
