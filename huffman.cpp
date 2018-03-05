// Huffman1.cpp: implementation of the CHuffman class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Huffman.h"

//#include <alloc.h>
#include <dos.h>
#include <fcntl.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHuffman::CHuffman()
{
_maskshuff[0] = &(_mask.chars.c3);
_maskshuff[1] = &(_mask.chars.c2);
_maskshuff[2] = &(_mask.chars.c1);
_maskshuff[3] = &(_mask.chars.c0);

}

CHuffman::~CHuffman()
{

}

void CHuffman::ntXORcode(char *outbuff, char *inbuff, long lSize )
{
	for(; lSize > 0; lSize--)
	{
     *outbuff ^= *inbuff;
	 outbuff++;
	 inbuff++;
	}
}
int CHuffman::ntDecode(char *outbuff, char *inbuff )
{
	int c, i, nchildren;
	int inleft;

	_eof = &_characters[0];
	if (inbuff[0] != US) return( 0 );
	if (inbuff[1] != RS) return( 0 );
	_inp = &inbuff[2];
	_origsize = 0;
	for (i=0; i<4; i++)
		_origsize = _origsize*256 + ((*_inp++) & 0377);
	inleft = _origsize;
	_dmaxlev = *_inp++ & 0377;

	if (_dmaxlev > 24) return( 0 );

	for (i=1; i<=_dmaxlev; i++)
		_intnodes[i] = *_inp++ & 0377;
	for (i=1; i<=_dmaxlev; i++) {
		_tree[i] = _eof;
		for (c=_intnodes[i]; c>0; c--) {
			if (_eof >= &_characters[255]) return( 0 );
			*_eof++ = *_inp++;
		}
	}
	*_eof++ = *_inp++;
	_intnodes[_dmaxlev] += 2;
	inleft -= _inp - &inbuff[0];

	if (inleft < 0) return( 0 );
	nchildren = 0;
	for (i=_dmaxlev; i>=1; i--) {
		c = _intnodes[i];
		_intnodes[i] = nchildren /= 2;
		nchildren += c;
	}
	return ( _decode( inleft,outbuff ));
}

int CHuffman::_decode( int inleft, char *outbuff )
{
	int bitsleft, c, i;
	int j, lev;
	char *p;

	_outp = &outbuff[0];
	lev = 1;
	i = 0;
	while (1) {
		if (--inleft < 0) return( 0 );
		c = *_inp++;
		bitsleft = 8;
		while (--bitsleft >= 0) {
			i *= 2;
			if (c & 0200)
				i++;
			c <<= 1;
			if ((j = i - _intnodes[lev]) >= 0) {
				p = &_tree[lev][j];
				if (p == _eof){
					c = _outp - &outbuff[0];
					_origsize -= c;
					if (_origsize != 0) return( 0 );
					return (1);
				}
				*_outp++ = *p;
				lev = 1;
				i = 0;
			}else
				lev++;
		}
	}
}
int CHuffman::ntIsarc(char *inbuff )
{
	if( inbuff[0] == US && inbuff[1] == RS )
		return( 1 );
	else
		return( 0 );
}
int CHuffman::ntGetorig(char *inbuff )
{
	int i;
	char *in;
	int size;

	in = inbuff+2;
	size = 0;
	for (i=0; i<4; i++)
		size = size*256 + ((*in++) & 0377);
	return( size );
}
int CHuffman::ntEncode(char *outbuff, char *inbuff, int buflen )
{
        register int c, i, p;
        long bitsout;

		_input( inbuff,buflen );

        _diffbytes = -1;
		_count[ END ] = 1;
		_insize.lint.lng = 0L;
		_n_ = 0;
        for (i=END; i>=0; i--) {
                _parent[i] = 0;
                if (_count[i] > 0) {
                        _diffbytes++;
                        _insize.lint.lng += _count[i];
                        _heap[++_n_].count = _count[i];
                        _heap[_n_].node = i;
                }
		}
		if (_diffbytes == 1) return( 0 );
        _insize.lint.lng >>= 1;
        for (i=_n_/2; i>=1; i--)
				_heapify(i);

        _lastnode = END;
        while (_n_ > 1) {
                _parent[_heap[1].node] = ++_lastnode;
                _inc = _heap[1].count;
                hmove (_heap[_n_], _heap[1]);
                _n_--;
				_heapify(1);
                _parent[_heap[1].node] = _lastnode;
                _heap[1].node = _lastnode;
                _heap[1].count += _inc;
				_heapify(1);
        }
        _parent[_lastnode] = 0;

        bitsout = _maxlev = 0;
        for (i=1; i<=24; i++)
                _levcount[i] = 0;
        for (i=0; i<=END; i++) {
                c = 0;
                for (p=_parent[i]; p!=0; p=_parent[p])
                        c++;
                _levcount[c]++;
                _length[i] = c;
                if (c > _maxlev)
                        _maxlev = c;
                bitsout += c*(_count[i]>>1);
        }
        if (_maxlev > 24) return( 0 );

        _inc = 1L << 24;
        _inc >>= _maxlev;
        _mask.lint.lng = 0;
        for (i=_maxlev; i>0; i--) {
                for (c=0; c<=END; c++)
                        if (_length[c] == i) {
                                _bits[c] = _mask.lint.lng;
                                _mask.lint.lng += _inc;
                        }
                _mask.lint.lng &= ~_inc;
                _inc <<= 1;
        }

		return( _output( outbuff,inbuff,buflen ));
}
void CHuffman::_input (char *inbuff, int buflen )
{
		register int i;

        for (i=0; i<END; i++)
                _count[i] = 0;
		while (buflen > 0)
				_count[inbuff[--buflen]&0377] += 2;
}
int CHuffman::_output(char *outbuff,char *inbuff, int buflen )
{
		int c, i;
        char *inp;
        register char **q, *outp;
        register int bitsleft;
        long temp;

        outbuff[0] = 037;       /* ascii US */
		outbuff[1] = 036;       /* ascii RS */

        temp = _insize.lint.lng;
        for (i=5; i>=2; i--) {
                outbuff[i] =  (char) (temp & 0377);
                temp >>= 8;
        }
        outp = outbuff+6;
        *outp++ = _maxlev;
        for (i=1; i<_maxlev; i++)
                *outp++ = _levcount[i];
        *outp++ = _levcount[_maxlev]-2;
        for (i=1; i<=_maxlev; i++)
                for (c=0; c<END; c++)
                        if (_length[c] == i)
                                *outp++ = c;

		inp = inbuff;
        bitsleft = 8;
        do {
                c = (--buflen < 0) ? END : (*inp++ & 0377);
                _mask.lint.lng = _bits[c]<<bitsleft;
                q = &_maskshuff[0];
                if (bitsleft == 8)
                        *outp = **q++;
                else
                        *outp |= **q++;
                bitsleft -= _length[c];
                while (bitsleft < 0) {
                        *++outp = **q++;
                        bitsleft += 8;
                }
        } while (c != END);
        if (bitsleft < 8)
                outp++;
        c = outp-outbuff;
		return (c);
}
void CHuffman::_heapify(int i )
{
        register int k;
        int lastparent;
		struct _heap heapsubi;

        hmove (_heap[i], heapsubi);
        lastparent = _n_/2;
        while (i <= lastparent) {
                k = 2*i;
                if (_heap[k].count > _heap[k+1].count && k < _n_)
                        k++;
                if (heapsubi.count < _heap[k].count)
                        break;
                hmove (_heap[k], _heap[i]);
                i = k;
        }
        hmove (heapsubi, _heap[i]);
}
