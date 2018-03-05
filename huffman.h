// Huffman1.h: interface for the CHuffman class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HUFFMAN1_H__5D5E262B_0CA7_42A9_A1CC_C907153A2CF8__INCLUDED_)
#define AFX_HUFFMAN1_H__5D5E262B_0CA7_42A9_A1CC_C907153A2CF8__INCLUDED_

#define US	037
#define RS	036

#define END     256
#define hmove(a,b) {(b).count = (a).count; (b).node = (a).node;}


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CHuffman  
{
public:
	CHuffman();
	virtual ~CHuffman();
//New

//Ω‚—π
char	*_inp;
char	*_outp;

int		_origsize;
short	_dmaxlev;
short	_intnodes[25];
char	*_tree[25];
char	_characters[256];
char	*_eof;
int ntIsarc(char *inbuff );
int _decode( int inleft, char *outbuff );
int ntDecode(char *outbuff, char *inbuff );
void ntXORcode(char *outbuff, char *inbuff, long lSize );

//—πÀı
struct  _heap {
        long count;
        int node;
} _heap[ END+2 ];
union FOUR {
	struct { long lng; } lint;
	struct { char c0, c1, c2, c3; } chars;
};


long    _count[ END+1 ];
union   FOUR _insize;
int     _diffbytes;

int     _maxlev;
int     _levcount[ 25 ];
int     _lastnode;
int     _parent[ 2*END+1 ];

char    _length[ END+1 ];
long    _bits[ END+1 ];
long    _inc;

union   FOUR _mask;
//char  *_maskshuff[4] = {&(_mask.chars.c3), &(_mask.chars.c2),&(_mask.chars.c1), &(_mask.chars.c0)};
char  *_maskshuff[4];

int _n_;


int ntGetorig(char *inbuff );
int ntEncode(char *outbuff, char *inbuff, int buflen );
void _input (char *inbuff, int buflen );
int _output(char *outbuff,char *inbuff, int buflen );
void _heapify(int i );


};

#endif // !defined(AFX_HUFFMAN1_H__5D5E262B_0CA7_42A9_A1CC_C907153A2CF8__INCLUDED_)
