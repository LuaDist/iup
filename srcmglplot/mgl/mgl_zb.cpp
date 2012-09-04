/***************************************************************************
 * mgl_zb.cpp is part of Math Graphic Library
 * Copyright (C) 2007 Alexey Balakin <balakin@appl.sci-nnov.ru>            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <stdlib.h>
#include "mgl/mgl_zb.h"
#include "mgl/mgl_c.h"
#include "mgl/mgl_f.h"
#define imax(a,b)	(a)>(b) ? (a) : (b)
#define imin(a,b)	(a)<(b) ? (a) : (b)

//#define MGL_ABUF_8

//#define MGL_SIMPLE_LINE
//-----------------------------------------------------------------------------
mglGraphZB::mglGraphZB(int w,int h) : mglGraphAB(w,h)
{	FastNoFace = true;	C = 0;	SetSize(w,h);	}
//-----------------------------------------------------------------------------
mglGraphZB::~mglGraphZB()
{	if(C)	{	delete []C;	delete []Z;	}	}
//-----------------------------------------------------------------------------
void mglGraphAB::SetDrawReg(int nx, int ny, int m)
{
	int mx = m%nx, my = m/nx;
	nx1 = Width*mx/nx;		ny1 = Height-Height*(my+1)/ny;
	nx2 = Width*(mx+1)/nx;	ny2 = Height-Height*my/ny;
}
//-----------------------------------------------------------------------------
void mglGraphAB::PutDrawReg(int , int , int , mglGraphAB *)	{}
//-----------------------------------------------------------------------------
void mglGraphZB::PutDrawReg(int nx, int ny, int m, mglGraphAB *g)
{
	mglGraphZB *gr=dynamic_cast<mglGraphZB *>(g);
	if(!gr)	return;
	int mx = m%nx, my = m/nx, x1, x2, y1, y2;
	x1 = Width*mx/nx;		y1 = Height-Height*(my+1)/ny;
	x2 = Width*(mx+1)/nx;	y2 = Height-Height*my/ny;
	register long i,j;
	for(j=y1;j<y2;j++)
	{
		i = x1+Width*(Height-1-j);
		memcpy(Z+8*i,gr->Z+8*i,8*(x2-x1)*sizeof(mreal));
		memcpy(C+32*i,gr->C+32*i,32*(x2-x1));
	}
}
//-----------------------------------------------------------------------------
void mglGraphZB::pnt_plot(long x,long y,mreal z,unsigned char ci[4])
{
	long i0=x+Width*(Height-1-y);
	if(x<nx1 || x>=nx2 || y<ny1 || y>=ny2)	return;
	unsigned char *cc = C+32*i0, c[4];
	memcpy(c,ci,4);
	mreal *zz = Z+8*i0, zf = FogDist*(z/Depth-0.5-FogDz);
	if(zf<0)
	{
		int d = int(255.f-255.f*exp(5.f*zf));
		unsigned char cb[4] = {BDef[0], BDef[1], BDef[2], d};
		if(d==255)	return;
		combine(c,cb);
	}
	if(DrawFace || !FastNoFace)
	{
#ifdef MGL_ABUF_8
		if(z>zz[3])
		{
#endif
			if(z>zz[1])
			{
#ifdef MGL_ABUF_8
				zz[7] = zz[6];	zz[6] = zz[5];	zz[5] = zz[4];
				zz[4] = zz[3];	zz[3] = zz[2];
				combine(cc+28,cc+24);	memcpy(cc+24,cc+20,4);
				memcpy(cc+20,cc+16,4);	memcpy(cc+16,cc+12,4);
				memcpy(cc+12,cc+8,4);
#else
				zz[3] = zz[2];	combine(cc+12,cc+8);
#endif
				zz[2] = zz[1];	memcpy(cc+8,cc+4,4);
				if(z>zz[0])	// shift point on slice down and paste new point
				{
					zz[1] = zz[0];	zz[0] = z;	OI[i0]=ObjId;
					memcpy(cc+4,cc,4);	memcpy(cc,c,4);
				}
				else	// shift point on slice down and paste new point
				{	zz[1] = z;	memcpy(cc+4,c,4);	}
			}
			else
			{
				if(z>zz[2])	// shift point on slice down and paste new point
				{
#ifdef MGL_ABUF_8
					zz[7] = zz[6];	zz[6] = zz[5];	zz[5] = zz[4];
					zz[4] = zz[3];	zz[3] = zz[2];
					combine(cc+28,cc+24);	memcpy(cc+24,cc+20,4);
					memcpy(cc+20,cc+16,4);	memcpy(cc+16,cc+12,4);
					memcpy(cc+12,cc+8,4);
#else
					zz[3] = zz[2];	combine(cc+12,cc+8);
#endif
					zz[2] = z;	memcpy(cc+8,c,4);
				}
				else	// shift point on slice down and paste new point
				{
#ifdef MGL_ABUF_8
					zz[7] = zz[6];	zz[6] = zz[5];	zz[5] = zz[4];
					zz[4] = zz[3];	zz[3] = z;
					combine(cc+28,cc+24);	memcpy(cc+24,cc+20,4);
					memcpy(cc+20,cc+16,4);	memcpy(cc+16,cc+12,4);
					memcpy(cc+12,c,4);
#else
					if(z>zz[3])	// point upper the background
					{	zz[3]=z;	combine(cc+12,c);	}
					else		// point below the background
					{	combine(c,cc+12);	memcpy(cc+12,c,4);	}
#endif
				}
			}
#ifdef MGL_ABUF_8
		}
		else
		{
			if(z>zz[5])
			{
				if(z>zz[4])	// shift point on slice down and paste new point
				{
					zz[7] = zz[6];	zz[6] = zz[5];	zz[5] = zz[4];	zz[4] = z;
					combine(cc+28,cc+24);	memcpy(cc+24,cc+20,4);
					memcpy(cc+20,cc+16,4);	memcpy(cc+16,c,4);
				}
				else	// shift point on slice down and paste new point
				{
					zz[7] = zz[6];	zz[6] = zz[5];	zz[5] = z;
					combine(cc+28,cc+24);	memcpy(cc+24,cc+20,4);
					memcpy(cc+20,c,4);
				}
			}
			else
			{
				if(z>zz[6])	// shift point on slice down and paste new point
				{
					zz[7] = zz[6];	zz[6] = z;
					combine(cc+28,cc+24);	memcpy(cc+24,c,4);
				}
				else	if(z>zz[7])	// point upper the background
				{	zz[7]=z;	combine(cc+28,c);	}
				else			// point below the background
				{	combine(c,cc+28);	memcpy(cc+28,c,4);	}
			}
		}
#endif
	}
	else
	{
		if(z>zz[0])	// point upper the background
		{	zz[0]=z;	memcpy(cc,c,4);		OI[i0]=ObjId;	}
	}
}
//-----------------------------------------------------------------------------
void mglGraphZB::Finish()
{
	register long i,i0;
	long n=Width*Height;
	unsigned char c[4],alf=TranspType!=2 ? 0:255,*cc;
	if(DrawFace || !FastNoFace)	for(i=0;i<n;i++)
	{
		i0 = 4*i;
		c[0]=BDef[0];	c[1]=BDef[1];	c[2]=BDef[2];	c[3]=alf;
		cc = C+32*i;
#ifdef MGL_ABUF_8
		combine(c,cc+28);		combine(c,cc+24);
		combine(c,cc+20);		combine(c,cc+16);
#endif
		combine(c,cc+12);		combine(c,cc+8);
		combine(c,cc+4);		combine(c,cc);
		memcpy(G4+i0,c,4);
		c[0]=BDef[0];	c[1]=BDef[1];	c[2]=BDef[2];	c[3]=255;
		combine(c,G4+i0);	memcpy(G+3*i,c,3);
	}
	else 	for(i=0;i<n;i++)
	{
		memcpy(G4+4*i,C+32*i,4);
		c[0]=BDef[0];	c[1]=BDef[1];	c[2]=BDef[2];	c[3]=255;
		combine(c,G4+4*i);	memcpy(G+3*i,c,3);
	}
	Finished = true;
}
//-----------------------------------------------------------------------------
void mglGraphZB::Clf(mglColor Back)
{
	Fog(0);
	pPos = 0;	CurrPal = -1;	PDef = 0xffff;
	if(Back==NC)		Back = mglColor(1,1,1);
	if(TranspType==2)	Back = mglColor(0,0,0);
	col2int(Back,1,BDef);
	register long i,n=Width*Height;
	memset(C,0,32*n);	memset(OI,0,n*sizeof(int));
	for(i=0;i<8*n;i++)	Z[i] = -1e20f;	// TODO Parallelization ?!?
//	for(i=0;i<Width;i++)	Z[i] = -1e20f;
//	for(i=1;i<8*Height;i++)	memcpy(Z+i*Width,Z,Width*sizeof(mreal));
	Finished = false;
}
//-----------------------------------------------------------------------------
void mglGraphZB::SetSize(int w,int h)
{
	if(w<=0 || h<=0)	{	SetWarn(mglWarnSize);	return;	}
	if(C)	{	delete []C;	delete []Z;	}
	C = new unsigned char[w*h*32];		// ����� *1 ��� TranspType>0 !!!
	Z = new mreal[w*h*8];
	mglGraphAB::SetSize(w,h);
}
//-----------------------------------------------------------------------------
int mgl_pnga_save(const char *fname, int w, int h, unsigned char **p);
void mglGraphZB::WriteSlice(int n)
{
	unsigned char **p;
	char fname[32];
	sprintf(fname,"%d.png",n);

	p = (unsigned char **)malloc(Height * sizeof(unsigned char *));
	for(long i=0;i<Height;i++)	p[i] = C+4*Width*i + n*4*Width*Height;

	mgl_pnga_save(fname, Width, Height, p);
	free(p);
}
//-----------------------------------------------------------------------------
void mglGraphZB::Glyph(mreal x, mreal y, mreal f, int s, long j, char col)
{
	int ss=s&3;
	f /= fnt->GetFact(ss);
	mglColor cc = mglColor(col);
	if(!cc.Valid())	cc = mglColor(CDef[0],CDef[1],CDef[2]);
	mreal c[4]={cc.r,cc.g,cc.b,CDef[3]};
	if(s&8)
	{
		if(!(s&4))	glyph_line(x,y,f,c,true);
		glyph_line(x,y,f,c,false);
	}
	else
	{
		if(!(s&4))	glyph_fill(x,y,f,fnt->GetNt(ss,j),fnt->GetTr(ss,j),c);
		glyph_wire(x,y,f,fnt->GetNl(ss,j),fnt->GetLn(ss,j),c);
	}
}
//-----------------------------------------------------------------------------
/// Create mglGraph object in ZBuffer mode.
HMGL mgl_create_graph_zb(int width, int height)
{    return new mglGraphZB(width,height);	}
/// Create mglGraph object in ZBuffer mode.
uintptr_t mgl_create_graph_zb_(int *width, int *height)
{    return uintptr_t(new mglGraphZB(*width,*height));	}
//-----------------------------------------------------------------------------
//
//		now these functions from mglGraphAB
//
//-----------------------------------------------------------------------------
void mglGraphAB::Ball(mreal x,mreal y,mreal z,mglColor col,mreal alpha)
{
	if(alpha==0)	return;
	if(alpha<0)	{	alpha = -alpha;	}
	else		{	if(!ScalePoint(x,y,z))	return;	}
	if(!col.Valid())	col = mglColor(1.,0.,0.);
	unsigned char r[4];
	Finished = false;
	alpha = Transparent ? alpha : 1;
	mreal p[3] = {x,y,z}, v, u = 1./(9*PenWidth*PenWidth);
	PostScale(p,1);
	r[0] = (unsigned char)(255.f*col.r);
	r[1] = (unsigned char)(255.f*col.g);
	r[2] = (unsigned char)(255.f*col.b);

	bool aa=UseAlpha;	UseAlpha = true;
	register long i,j;
	long w = long(5.5+fabs(PenWidth));
	for(i=-w;i<=w;i++)	for(j=-w;j<=w;j++)
	{
		v = (i*i+j*j)*u;
		r[3] = (unsigned char)(255.f*alpha*exp(-6.f*v));
		if(r[3]==0)	continue;
		pnt_plot(long(p[0])+i,long(p[1])+j,p[2],r);
	}
	UseAlpha = aa;
}
//-----------------------------------------------------------------------------
void mglGraphAB::ball(mreal *p,mreal *c)
{
	unsigned char r[4];
	Finished = false;
	r[0] = (unsigned char)(255.f*c[0]);
	r[1] = (unsigned char)(255.f*c[1]);
	r[2] = (unsigned char)(255.f*c[2]);

	bool aa=UseAlpha;	UseAlpha = true;
	register long i,j;
	long w = long(5.5+fabs(PenWidth));
	mreal v, u = 1./(4*PenWidth*PenWidth);
	for(i=-w;i<=w;i++)	for(j=-w;j<=w;j++)
	{
		v = (i*i+j*j)*u;
		r[3] = (unsigned char)(255.f*c[3]*exp(-6.f*v));
		if(r[3]==0)	continue;
		pnt_plot(long(p[0])+i,long(p[1])+j,p[2],r);
	}
	UseAlpha = aa;
}
//-----------------------------------------------------------------------------
unsigned char* mglGraphAB::col2int(mglColor c,mreal a,unsigned char *r)
{
	mreal cc[4] = {c.r,c.g,c.b,a};
	return col2int(cc,0,r);
}
//-----------------------------------------------------------------------------
unsigned char* mglGraphAB::col2int(mreal *c,mreal *n,unsigned char *r)
{
	register long i,j;
	static unsigned char u[4];
	register mreal b0=c[0],b1=c[1],b2=c[2];
	if(r==0) r = u;
	if(c[3]<=0)	{	memset(r,0,4*sizeof(unsigned char));	return r;	}
	if(UseLight && n)
	{
		mreal d0,d1,d2,nn;
		b0 *= AmbBr;		b1 *= AmbBr;		b2 *= AmbBr;
		for(i=0;i<10;i++)
		{
			if(!nLight[i])	continue;
			j = 3*i;
			nn = 2*(n[0]*pLight[j]+n[1]*pLight[j+1]+n[2]*pLight[j+2]) /
					(n[0]*n[0]+n[1]*n[1]+n[2]*n[2]+1e-6);
			d0 = pLight[j] - n[0]*nn;
			d1 = pLight[j+1]-n[1]*nn;
			d2 = pLight[j+2]-n[2]*nn;
			nn = 1 + d2/sqrt(d0*d0+d1*d1+d2*d2+1e-6);

			nn = exp(-aLight[i]*nn)*bLight[i]*2;
			b0 += nn*cLight[j];
			b1 += nn*cLight[j+1];
			b2 += nn*cLight[j+2];
		}
		b0 = b0<1 ? b0 : 1;
		b1 = b1<1 ? b1 : 1;
		b2 = b2<1 ? b2 : 1;
	}
	r[0] = (unsigned char)(255.f*b0);	r[1] = (unsigned char)(255.f*b1);
	r[2] = (unsigned char)(255.f*b2);
	// c[3] should be <1 but I additionally check it here
	r[3] = UseAlpha && c[3]<1 ? (unsigned char)(255.f*c[3]) : 255;
	return r;
}
//-----------------------------------------------------------------------------
/* Bilinear interpolation r(u,v) = r0 + (r1-r0)*u + (r2-20)*v + (r3+30-r1-r2)*u*v is used (where r is one of {x,y,z,R,G,B,A}. Variables u,v are determined for each point (x,y) and selected one pair which 0<u<1 and 0<v<1.*/
void mglGraphAB::quad_plot(mreal *pp0,mreal *pp1,mreal *pp2,mreal *pp3,mreal *cc0,mreal *cc1,mreal *cc2,mreal *cc3)
{	quad_draw(pp0,pp1,pp2,pp3,cc0,cc1,cc2,cc3);	}
void mglGraphAB::quad_draw(mreal *pp0,mreal *pp1,mreal *pp2,mreal *pp3,
					mreal *cc0,mreal *cc1,mreal *cc2,mreal *cc3)
{
	Finished = false;
	unsigned char r[4];
	long y1,x1,y2,x2;
	mreal d1[3],d2[3],d3[3],c1[4],c2[4],c3[4];
	mreal cs[4],ns[3],dd,dsx,dsy;

	d1[0] = pp1[0]-pp0[0];	d2[0] = pp2[0]-pp0[0];	d3[0] = pp3[0]+pp0[0]-pp1[0]-pp2[0];
	d1[1] = pp1[1]-pp0[1];	d2[1] = pp2[1]-pp0[1];	d3[1] = pp3[1]+pp0[1]-pp1[1]-pp2[1];
	d1[2] = pp1[2]-pp0[2];	d2[2] = pp2[2]-pp0[2];	d3[2] = pp3[2]+pp0[2]-pp1[2]-pp2[2];
	c1[0] = cc1[0]-cc0[0];	c2[0] = cc2[0]-cc0[0];	c3[0] = cc3[0]+cc0[0]-cc1[0]-cc2[0];
	c1[1] = cc1[1]-cc0[1];	c2[1] = cc2[1]-cc0[1];	c3[1] = cc3[1]+cc0[1]-cc1[1]-cc2[1];
	c1[2] = cc1[2]-cc0[2];	c2[2] = cc2[2]-cc0[2];	c3[2] = cc3[2]+cc0[2]-cc1[2]-cc2[2];
	c1[3] = cc1[3]-cc0[3];	c2[3] = cc2[3]-cc0[3];	c3[3] = cc3[3]+cc0[3]-cc1[3]-cc2[3];

	x1 = imin(imin(long(pp0[0]),long(pp3[0])),imin(long(pp1[0]),long(pp2[0])));	// bounding box
	y1 = imin(imin(long(pp0[1]),long(pp3[1])),imin(long(pp1[1]),long(pp2[1])));
	x2 = imax(imax(long(pp0[0]),long(pp3[0])),imax(long(pp1[0]),long(pp2[0])));
	y2 = imax(imax(long(pp0[1]),long(pp3[1])),imax(long(pp1[1]),long(pp2[1])));
	x1=x1>nx1?x1:nx1;	x2=x2<nx2?x2:nx2-1;	y1=y1>ny1?y1:ny1;	y2=y2<ny2?y2:ny2-1;
	if(x1>x2 || y1>y2)	return;

	dd=d1[0]*d2[1]-d1[1]*d2[0];
	dsx =-4*(d2[1]*d3[0] - d2[0]*d3[1])*d1[1];
	dsy = 4*(d2[1]*d3[0] - d2[0]*d3[1])*d1[0];

	if((d1[0]==0 && d1[1]==0) || (d2[0]==0 && d2[1]==0) || (pp1[0]==pp3[0] && pp1[1]==pp3[1]) || (pp2[0]==pp3[0] && pp2[1]==pp3[1]))
	{
		mreal ps[3];
		ps[0] = (pp0[0]+pp1[0]+pp2[0]+pp3[0])/4;
		ps[1] = (pp0[1]+pp1[1]+pp2[1]+pp3[1])/4;
		ps[2] = (pp0[2]+pp1[2]+pp2[2]+pp3[2])/4;
		cs[0] = (cc0[0]+cc1[0]+cc2[0]+cc3[0])/4;
		cs[1] = (cc0[1]+cc1[1]+cc2[1]+cc3[1])/4;
		cs[2] = (cc0[2]+cc1[2]+cc2[2]+cc3[2])/4;
		cs[3] = (cc0[3]+cc1[3]+cc2[3]+cc3[3])/4;
		trig_plot(ps,pp0,pp1,cs,cc0,cc1);
		trig_plot(ps,pp3,pp1,cs,cc3,cc1);
		trig_plot(ps,pp2,pp3,cs,cc2,cc3);
		trig_plot(ps,pp0,pp2,cs,cc0,cc2);
		return;
	}

	register long i,j,g;
	register mreal u,v,s,xx,yy,q;
	for(i=x1;i<=x2;i++)	for(j=y1;j<=y2;j++)
	{
		xx = (i-pp0[0]);	yy = (j-pp0[1]);
		s = dsx*xx + dsy*yy + (dd+d3[1]*xx-d3[0]*yy)*(dd+d3[1]*xx-d3[0]*yy);
		if(s<0)	continue;	// no solution
		s = sqrt(s);
		q = d3[0]*yy - d3[1]*xx + dd + s;
		u = q ? 2.f*(d2[1]*xx - d2[0]*yy)/q : -1.f;
		q = d3[1]*xx - d3[0]*yy + dd + s;
		v = q ? 2.f*(d1[0]*yy - d1[1]*xx)/q : -1.f;
		g = u<0.f || u>1.f || v<0.f || v>1.f;
		if(g)	// first root bad
		{
			q = d3[0]*yy - d3[1]*xx + dd - s;
			u = q ? 2.f*(d2[1]*xx - d2[0]*yy)/q : -1.f;
			q = d3[1]*xx - d3[0]*yy + dd - s;
			v = q ? 2.f*(d1[0]*yy - d1[1]*xx)/q : -1.f;
			g = u<0.f || u>1.f || v<0.f || v>1.f;
			if(g)	continue;	// second root bad
		}
		s = u*v;
		cs[0] = cc0[0]+c1[0]*u+c2[0]*v+c3[0]*s;
		cs[1] = cc0[1]+c1[1]*u+c2[1]*v+c3[1]*s;
		cs[2] = cc0[2]+c1[2]*u+c2[2]*v+c3[2]*s;
		cs[3] = cc0[3]+c1[3]*u+c2[3]*v+c3[3]*s;
		ns[0]=(d2[2]+d3[2]*u)*(d1[1]+d3[1]*v)-(d2[1]+d3[1]*u)*(d1[2]+d3[2]*v);	// normal vector
		ns[1]=(d2[0]+d3[0]*u)*(d1[2]+d3[2]*v)-(d2[2]+d3[2]*u)*(d1[0]+d3[0]*v);
		ns[2]=(d2[1]+d3[1]*u)*(d1[0]+d3[0]*v)-(d2[0]+d3[0]*u)*(d1[1]+d3[1]*v);
		pnt_plot(i,j,pp0[2]+d1[2]*u+d2[2]*v+d3[2]*s,col2int(cs,ns,r));
	}
}
//-----------------------------------------------------------------------------
/* Bilinear interpolation r(u,v) = r0 + (r1-r0)*u + (r2-20)*v + (r3+30-r1-r2)*u*v is used (where r is one of {x,y,z,R,G,B,A}. Variables u,v are determined 	for each point (x,y) and selected one pair which 0<u<1 and 0<v<1. 	In difference mglGraphZB::quad_plot the normal is also interpolated.*/
void mglGraphAB::quad_plot_n(mreal *pp0,mreal *pp1,mreal *pp2,mreal *pp3,
					mreal *cc0,mreal *cc1,mreal *cc2,mreal *cc3,
					mreal *nn0,mreal *nn1,mreal *nn2,mreal *nn3)
{
	Finished = false;
	unsigned char r[4];
	long y1,x1,y2,x2;
	mreal d1[3],d2[3],d3[3],c1[4],c2[4],c3[4],n1[3],n2[3],n3[3];
	mreal cs[4],ns[3],dd,dsx,dsy;

	d1[0] = pp1[0]-pp0[0];	d2[0] = pp2[0]-pp0[0];	d3[0] = pp3[0]+pp0[0]-pp1[0]-pp2[0];
	d1[1] = pp1[1]-pp0[1];	d2[1] = pp2[1]-pp0[1];	d3[1] = pp3[1]+pp0[1]-pp1[1]-pp2[1];
	d1[2] = pp1[2]-pp0[2];	d2[2] = pp2[2]-pp0[2];	d3[2] = pp3[2]+pp0[2]-pp1[2]-pp2[2];
	c1[0] = cc1[0]-cc0[0];	c2[0] = cc2[0]-cc0[0];	c3[0] = cc3[0]+cc0[0]-cc1[0]-cc2[0];
	c1[1] = cc1[1]-cc0[1];	c2[1] = cc2[1]-cc0[1];	c3[1] = cc3[1]+cc0[1]-cc1[1]-cc2[1];
	c1[2] = cc1[2]-cc0[2];	c2[2] = cc2[2]-cc0[2];	c3[2] = cc3[2]+cc0[2]-cc1[2]-cc2[2];
	c1[3] = cc1[3]-cc0[3];	c2[3] = cc2[3]-cc0[3];	c3[3] = cc3[3]+cc0[3]-cc1[3]-cc2[3];
	n1[0] = nn1[0]-nn0[0];	n2[0] = nn2[0]-nn0[0];	n3[0] = nn3[0]+nn0[0]-nn1[0]-nn2[0];
	n1[1] = nn1[1]-nn0[1];	n2[1] = nn2[1]-nn0[1];	n3[1] = nn3[1]+nn0[1]-nn1[1]-nn2[1];
	n1[2] = nn1[2]-nn0[2];	n2[2] = nn2[2]-nn0[2];	n3[2] = nn3[2]+nn0[2]-nn1[2]-nn2[2];

	x1 = imin(imin(long(pp0[0]),long(pp3[0])),imin(long(pp1[0]),long(pp2[0])));	// bounding box
	y1 = imin(imin(long(pp0[1]),long(pp3[1])),imin(long(pp1[1]),long(pp2[1])));
	x2 = imax(imax(long(pp0[0]),long(pp3[0])),imax(long(pp1[0]),long(pp2[0])));
	y2 = imax(imax(long(pp0[1]),long(pp3[1])),imax(long(pp1[1]),long(pp2[1])));
	x1=x1>nx1?x1:nx1;	x2=x2<nx2?x2:nx2-1;	y1=y1>ny1?y1:ny1;	y2=y2<ny2?y2:ny2-1;
	if(x1>x2 || y1>y2)	return;

	dd=d1[0]*d2[1]-d1[1]*d2[0];
	dsx =-4*(d2[1]*d3[0] - d2[0]*d3[1])*d1[1];
	dsy = 4*(d2[1]*d3[0] - d2[0]*d3[1])*d1[0];

	if((d1[0]==0 && d1[1]==0) || (d2[0]==0 && d2[1]==0) || (pp1[0]==pp3[0] && pp1[1]==pp3[1]) || (pp2[0]==pp3[0] && pp2[1]==pp3[1]))
	{
		mreal ps[3];
		ps[0] = (pp0[0]+pp1[0]+pp2[0]+pp3[0])/4;
		ps[1] = (pp0[1]+pp1[1]+pp2[1]+pp3[1])/4;
		ps[2] = (pp0[2]+pp1[2]+pp2[2]+pp3[2])/4;
		cs[0] = (cc0[0]+cc1[0]+cc2[0]+cc3[0])/4;
		cs[1] = (cc0[1]+cc1[1]+cc2[1]+cc3[1])/4;
		cs[2] = (cc0[2]+cc1[2]+cc2[2]+cc3[2])/4;
		cs[3] = (cc0[3]+cc1[3]+cc2[3]+cc3[3])/4;
		trig_plot(ps,pp0,pp1,cs,cc0,cc1);
		trig_plot(ps,pp3,pp1,cs,cc3,cc1);
		trig_plot(ps,pp2,pp3,cs,cc2,cc3);
		trig_plot(ps,pp0,pp2,cs,cc0,cc2);
		return;
	}

	register long i,j,g;
	register mreal u,v,s,xx,yy,q;
	for(i=x1;i<=x2;i++)	for(j=y1;j<=y2;j++)
	{
		xx = (i-pp0[0]);	yy = (j-pp0[1]);
		s = dsx*xx + dsy*yy + (dd+d3[1]*xx-d3[0]*yy)*(dd+d3[1]*xx-d3[0]*yy);
		if(s<0)	continue;	// no solution
		s = sqrt(s);
		q = d3[0]*yy - d3[1]*xx + dd + s;
		u = q ? 2.f*(d2[1]*xx - d2[0]*yy)/q : -1.f;
		q = d3[1]*xx - d3[0]*yy + dd + s;
		v = q ? 2.f*(d1[0]*yy - d1[1]*xx)/q : -1.f;
		g = u<0.f || u>1.f || v<0.f || v>1.f;
		if(g)	// first root bad
		{
			q = d3[0]*yy - d3[1]*xx + dd - s;
			u = q ? 2.f*(d2[1]*xx - d2[0]*yy)/q : -1.f;
			q = d3[1]*xx - d3[0]*yy + dd - s;
			v = q ? 2.f*(d1[0]*yy - d1[1]*xx)/q : -1.f;
			g = u<0.f || u>1.f || v<0.f || v>1.f;
			if(g)	continue;	// second root bad
		}
		s = u*v;
		cs[0] = cc0[0]+c1[0]*u+c2[0]*v+c3[0]*s;
		cs[1] = cc0[1]+c1[1]*u+c2[1]*v+c3[1]*s;
		cs[2] = cc0[2]+c1[2]*u+c2[2]*v+c3[2]*s;
		cs[3] = cc0[3]+c1[3]*u+c2[3]*v+c3[3]*s;
		ns[0] = nn0[0]+n1[0]*u+n2[0]*v+n3[0]*s;
		ns[1] = nn0[1]+n1[1]*u+n2[1]*v+n3[1]*s;
		ns[2] = nn0[2]+n1[2]*u+n2[2]*v+n3[2]*s;
		pnt_plot(i,j,pp0[2]+d1[2]*u+d2[2]*v+d3[2]*s,col2int(cs,ns,r));
	}
}
//-----------------------------------------------------------------------------
/* Bilinear interpolation r(u,v) = r0 + (r1-r0)*u + (r2-20)*v + (r3+30-r1-r2)*u*v is used (where r is one of {x,y,z,R,G,B,A}. Variables u,v are determined for each point (x,y) and selected one pair which 0<u<1 and 0<v<1.*/
void mglGraphAB::quad_plot_a(mreal *pp0,mreal *pp1,mreal *pp2,mreal *pp3,
					mreal aa0,mreal aa1,mreal aa2,mreal aa3, mreal alpha)
{
	Finished = false;
	unsigned char r[4];
	long y1,x1,y2,x2;
	mreal d1[3],d2[3],d3[3],a1,a2,a3;
	mreal dd,dsx,dsy;

	d1[0] = pp1[0]-pp0[0];	d2[0] = pp2[0]-pp0[0];	d3[0] = pp3[0]+pp0[0]-pp1[0]-pp2[0];
	d1[1] = pp1[1]-pp0[1];	d2[1] = pp2[1]-pp0[1];	d3[1] = pp3[1]+pp0[1]-pp1[1]-pp2[1];
	d1[2] = pp1[2]-pp0[2];	d2[2] = pp2[2]-pp0[2];	d3[2] = pp3[2]+pp0[2]-pp1[2]-pp2[2];
	a1 = aa1-aa0;			a2 = aa2-aa0;			a3 = aa3+aa0-aa1-aa2;

	x1 = imin(imin(long(pp0[0]),long(pp3[0])),imin(long(pp1[0]),long(pp2[0])));	// bounding box
	y1 = imin(imin(long(pp0[1]),long(pp3[1])),imin(long(pp1[1]),long(pp2[1])));
	x2 = imax(imax(long(pp0[0]),long(pp3[0])),imax(long(pp1[0]),long(pp2[0])));
	y2 = imax(imax(long(pp0[1]),long(pp3[1])),imax(long(pp1[1]),long(pp2[1])));
	x1=x1>nx1?x1:nx1;	x2=x2<nx2?x2:nx2-1;	y1=y1>ny1?y1:ny1;	y2=y2<ny2?y2:ny2-1;
	if(x1>x2 || y1>y2)	return;

	dd=d1[0]*d2[1]-d1[1]*d2[0];
	dsx =-4*(d2[1]*d3[0] - d2[0]*d3[1])*d1[1];
	dsy = 4*(d2[1]*d3[0] - d2[0]*d3[1])*d1[0];

	register long i,j,k;
	bool g;
	long n = NumCol-1;
	register mreal u,v,s,xx,yy,q;
	mglColor c;
	for(i=x1;i<=x2;i++)	for(j=y1;j<=y2;j++)
	{
		xx = (i-pp0[0]);	yy = (j-pp0[1]);
		s = dsx*xx + dsy*yy + (dd+d3[1]*xx-d3[0]*yy)*(dd+d3[1]*xx-d3[0]*yy);
		if(s<0)	continue;	// no solution
		s = sqrt(s);
		q = d3[0]*yy - d3[1]*xx + dd + s;
		u = q ? 2.f*(d2[1]*xx - d2[0]*yy)/q : -1.f;
		q = d3[1]*xx - d3[0]*yy + dd + s;
		v = q ? 2.f*(d1[0]*yy - d1[1]*xx)/q : -1.f;
		g = u<0.f || u>1.f || v<0.f || v>1.f;
		if(g)	// first root bad
		{
			q = d3[0]*yy - d3[1]*xx + dd - s;
			u = q ? 2.f*(d2[1]*xx - d2[0]*yy)/q : -1.f;
			q = d3[1]*xx - d3[0]*yy + dd - s;
			v = q ? 2.f*(d1[0]*yy - d1[1]*xx)/q : -1.f;
			g = u<0.f || u>1.f || v<0.f || v>1.f;
			if(g)	continue;	// second root bad
		}

		s = aa0 + a1*u + a2*v + a3*u*v;
		r[3] = (unsigned char)(255.f*alpha*(alpha>0 ? (s+1.f)*(s+1.f) : (1.f-s)*(s-1.f)));
		s = (s+1.f)/2.f;	s *= n;
		k = long(s);		s -= k;
		if(k<n)	c = cmap[k]*(1.f-s)+cmap[k+1]*s;
		else	c = cmap[n];
		r[0] = (unsigned char)(255.f*c.r);
		r[1] = (unsigned char)(255.f*c.g);
		r[2] = (unsigned char)(255.f*c.b);
		s = pp0[2]+d1[2]*u+d2[2]*v+d3[2]*u*v;
		pnt_plot(i,j, s, r);
	}
}
//-----------------------------------------------------------------------------
/* Linear interpolation r(u,v) = r0 + (r1-r0)*u + (r2-20)*v is used (where r is one of {x,y,z,R,G,B,A}. Variables u,v are determined for each point (x,y). Point plotted is u>0 and v>0 and u+v<1.*/
void mglGraphAB::trig_plot(mreal *pp0,mreal *pp1,mreal *pp2,mreal *cc0,mreal *cc1,mreal *cc2)
{	trig_draw(pp0,pp1,pp2,cc0,cc1,cc2);	}
void mglGraphAB::trig_draw(mreal *pp0,mreal *pp1,mreal *pp2,mreal *cc0,mreal *cc1,mreal *cc2)
{
	Finished = false;
	unsigned char r[4];
	long y1,x1,y2,x2;
	mreal d1[3],d2[3],c1[4],c2[4];
	mreal cs[4],ns[3],dxu,dxv,dyu,dyv;

	d1[0] = pp1[0]-pp0[0];	d2[0] = pp2[0]-pp0[0];
	d1[1] = pp1[1]-pp0[1];	d2[1] = pp2[1]-pp0[1];
	dxu = d2[0]*d1[1] - d1[0]*d2[1];
	if(fabs(dxu)<1e-5)	return;		// points lies on the same line

	d1[2] = pp1[2]-pp0[2];	d2[2] = pp2[2]-pp0[2];
	c1[0] = cc1[0]-cc0[0];	c2[0] = cc2[0]-cc0[0];
	c1[1] = cc1[1]-cc0[1];	c2[1] = cc2[1]-cc0[1];
	c1[2] = cc1[2]-cc0[2];	c2[2] = cc2[2]-cc0[2];
	c1[3] = cc1[3]-cc0[3];	c2[3] = cc2[3]-cc0[3];
	ns[0]=d2[2]*d1[1]-d2[1]*d1[2];					// normal vector
	ns[1]=d2[0]*d1[2]-d2[2]*d1[0];
	ns[2]=d2[1]*d1[0]-d2[0]*d1[1];

	dyv =-d1[0]/dxu;	dxv = d1[1]/dxu;
	dyu = d2[0]/dxu;	dxu =-d2[1]/dxu;

	x1 = imin(long(pp0[0]),imin(long(pp1[0]),long(pp2[0])));	// bounding box
	y1 = imin(long(pp0[1]),imin(long(pp1[1]),long(pp2[1])));
	x2 = imax(long(pp0[0]),imax(long(pp1[0]),long(pp2[0])));
	y2 = imax(long(pp0[1]),imax(long(pp1[1]),long(pp2[1])));
	x1=x1>nx1?x1:nx1;	x2=x2<nx2?x2:nx2-1;	y1=y1>ny1?y1:ny1;	y2=y2<ny2?y2:ny2-1;
	if(x1>x2 || y1>y2)	return;

	register mreal u,v,xx,yy;
	register long i,j,g;
	for(i=x1;i<=x2;i++)	for(j=y1;j<=y2;j++)
	{
		xx = (i-pp0[0]);	yy = (j-pp0[1]);
		u = dxu*xx+dyu*yy;	v = dxv*xx+dyv*yy;
		g = u<0 || v<0 || u+v>1;
		if(g)	continue;

		cs[0] = cc0[0]+c1[0]*u+c2[0]*v;
		cs[1] = cc0[1]+c1[1]*u+c2[1]*v;
		cs[2] = cc0[2]+c1[2]*u+c2[2]*v;
		cs[3] = cc0[3]+c1[3]*u+c2[3]*v;
		pnt_plot(i,j,pp0[2]+d1[2]*u+d2[2]*v,col2int(cs,ns,r));
	}
}
//-----------------------------------------------------------------------------
/* Linear interpolation r(u,v) = r0 + (r1-r0)*u + (r2-20)*v is used (where r is one of {x,y,z,R,G,B,A}. Variables u,v are determined for each point (x,y). Point plotted is u>0 and v>0 and u+v<1.*/
void mglGraphAB::trig_plot_n(mreal *pp0,mreal *pp1,mreal *pp2,
					mreal *cc0,mreal *cc1,mreal *cc2,
mreal *nn0,mreal *nn1,mreal *nn2)
{
	Finished = false;
	unsigned char r[4];
	long y1,x1,y2,x2;
	mreal d1[3],d2[3],c1[4],c2[4],n1[3],n2[3];
	mreal cs[4],ns[3],dxu,dxv,dyu,dyv;

	d1[0] = pp1[0]-pp0[0];	d2[0] = pp2[0]-pp0[0];
	d1[1] = pp1[1]-pp0[1];	d2[1] = pp2[1]-pp0[1];
	dxu = d2[0]*d1[1] - d1[0]*d2[1];
	if(fabs(dxu)<1e-5)	return;		// points lies on the same line

	d1[2] = pp1[2]-pp0[2];	d2[2] = pp2[2]-pp0[2];
	c1[0] = cc1[0]-cc0[0];	c2[0] = cc2[0]-cc0[0];
	c1[1] = cc1[1]-cc0[1];	c2[1] = cc2[1]-cc0[1];
	c1[2] = cc1[2]-cc0[2];	c2[2] = cc2[2]-cc0[2];
	c1[3] = cc1[3]-cc0[3];	c2[3] = cc2[3]-cc0[3];
	n1[0] = nn1[0]-nn0[0];	n2[0] = nn2[0]-nn0[0];
	n1[1] = nn1[1]-nn0[1];	n2[1] = nn2[1]-nn0[1];
	n1[2] = nn1[2]-nn0[2];	n2[2] = nn2[2]-nn0[2];

	dyv =-d1[0]/dxu;	dxv = d1[1]/dxu;
	dyu = d2[0]/dxu;	dxu =-d2[1]/dxu;

	x1 = imin(long(pp0[0]),imin(long(pp1[0]),long(pp2[0])));	// bounding box
	y1 = imin(long(pp0[1]),imin(long(pp1[1]),long(pp2[1])));
	x2 = imax(long(pp0[0]),imax(long(pp1[0]),long(pp2[0])));
	y2 = imax(long(pp0[1]),imax(long(pp1[1]),long(pp2[1])));
	x1=x1>nx1?x1:nx1;	x2=x2<nx2?x2:nx2-1;	y1=y1>ny1?y1:ny1;	y2=y2<ny2?y2:ny2-1;
	if(x1>x2 || y1>y2)	return;

	register mreal u,v,xx,yy;
	register long i,j,g;
	for(i=x1;i<=x2;i++)	for(j=y1;j<=y2;j++)
	{
		xx = (i-pp0[0]);	yy = (j-pp0[1]);
		u = dxu*xx+dyu*yy;	v = dxv*xx+dyv*yy;
		g = u<0 || v<0 || u+v>1;
		if(g)	continue;

		cs[0] = cc0[0]+c1[0]*u+c2[0]*v;
		cs[1] = cc0[1]+c1[1]*u+c2[1]*v;
		cs[2] = cc0[2]+c1[2]*u+c2[2]*v;
		cs[3] = cc0[3]+c1[3]*u+c2[3]*v;
		ns[0] = nn0[0]+n1[0]*u+n2[0]*v;
		ns[1] = nn0[1]+n1[1]*u+n2[1]*v;
		ns[2] = nn0[2]+n1[2]*u+n2[2]*v;
		pnt_plot(i,j,pp0[2]+d1[2]*u+d2[2]*v,col2int(cs,ns,r));
	}
}
//-----------------------------------------------------------------------------
/*void mglGraphAB::line_plot_s(mreal *x1,mreal *x2,mreal *y1,mreal *y2,bool )
{
	mreal kx,ky,kz,t,dd=1;
	mreal c10,c11,c12;

	register long i;
	kx = ky = kz = t = 0;
	if(fabs(x1[0]-x2[0])>fabs(x1[1]-x2[1]))
	{
		dd = 1./fabs(x2[0]-x1[0]);		t = fabs(x2[0]-x1[0]);
	}
	else if(fabs(x1[1]-x2[1])!=0)
	{
		dd = 1./fabs(x2[1]-x1[1]);		t = fabs(x2[1]-x1[1]);
	}
	kx=(x2[0]-x1[0])*dd;	ky=(x2[1]-x1[1])*dd;	kz=(x2[2]-x1[2])*dd;
	c10=(y2[0]-y1[0])*dd;	c11=(y2[1]-y1[1])*dd;	c12=(y2[2]-y1[2])*dd;
	unsigned char r[4]={255,255,255,255};
	for(i=0;i<=t;i++)
	{
		r[0] = (unsigned char)(255.f*(y1[0]+c10*i));
		r[1] = (unsigned char)(255.f*(y1[1]+c11*i));
		r[2] = (unsigned char)(255.f*(y1[2]+c12*i));
		pnt_plot(long(x1[0]+kx*i), long(x1[1]+ky*i), x1[2]+kz*i,r);
	}
}*/
//-----------------------------------------------------------------------------
void mglGraphAB::line_plot(mreal *pp0,mreal *pp1,mreal *cc0,mreal *cc1,bool all)
{	line_draw(pp0,pp1,cc0,cc1,all);	}
void mglGraphAB::line_draw(mreal *pp0,mreal *pp1,mreal *cc0,mreal *cc1,bool all)
{
//	if(!DrawFace && FastNoFace)	{	line_plot_s(pp0,pp1,cc0,cc1,all);	return;	}
	Finished = false;
	unsigned char r[4];
	long y1,x1,y2,x2;
	mreal dxu,dxv,dyu,dyv,dd,pw = fabs(PenWidth);
	bool hor = fabs(pp0[0]-pp1[0])>fabs(pp0[1]-pp1[1]);
	mreal d10,d11,d12, c10,c11,c12, b;

	d10 = pp1[0]-pp0[0];	d11 = pp1[1]-pp0[1];
	dd = sqrt(d10*d10 + d11*d11);
	if(dd<1e-5)	return;		// points lies on the vertical line
	b = PenWidth*PenWidth;

	d12 = pp1[2]-pp0[2];
	c10 = cc1[0]-cc0[0];	c11 = cc1[1]-cc0[1];
	c12 = cc1[2]-cc0[2];	//c13 = cc1[3]-cc0[3];

	dxv = d11/dd;	dyv =-d10/dd;
	dxu = d10/dd;	dyu = d11/dd;

	x1 = imin(long(pp0[0]),long(pp1[0]));	// bounding box
	y1 = imin(long(pp0[1]),long(pp1[1]));
	x2 = imax(long(pp0[0]),long(pp1[0]));
	y2 = imax(long(pp0[1]),long(pp1[1]));
	x1 -= int(pw+3.5);	y1 -= int(pw+3.5);
	x2 += int(pw+3.5);	y2 += int(pw+3.5);
	x1=x1>nx1?x1:nx1;	x2=x2<nx2?x2:nx2-1;	y1=y1>ny1?y1:ny1;	y2=y2<ny2?y2:ny2-1;
	if(x1>x2 || y1>y2)	return;

	bool aa=UseAlpha;
	register mreal u,v,xx,yy;
	register long i,j;
	register bool tt;
	UseAlpha = true;
	if(hor)	for(i=x1;i<=x2;i++)
	{
		y1 = int(pp0[1]+(pp1[1]-pp0[1])*(i-pp0[0])/(pp1[0]-pp0[0]) - pw - 3.5);
		y2 = int(pp0[1]+(pp1[1]-pp0[1])*(i-pp0[0])/(pp1[0]-pp0[0]) + pw + 3.5);
		y1=y1>ny1?y1:ny1;	y2=y2<ny2?y2:ny2-1;		if(y1>y2)	continue;
		for(j=y1;j<=y2;j++)
		{
			xx = (i-pp0[0]);	yy = (j-pp0[1]);
			u = dxu*xx+dyu*yy;	v = dxv*xx+dyv*yy;	v = v*v;
			if(u<0)			{	v += u*u;			u = 0;	}
			else if(u>dd)	{	v += (u-dd)*(u-dd);	u = dd;	}
			if(v>b)		continue;
			tt = all || (PDef & (1<<long(fmod(pPos+u/pw/1.5, 16))));
			if(!tt)		continue;
			u /= dd;
			r[0] = (unsigned char)(255.f*(cc0[0]+c10*u));
			r[1] = (unsigned char)(255.f*(cc0[1]+c11*u));
			r[2] = (unsigned char)(255.f*(cc0[2]+c12*u));
//			r[3] = (unsigned char)(255.f*exp(-6.f*v/b));
			r[3] = (unsigned char)(255.f/cosh(3.f*sqrt(v/b)));
			pnt_plot(i,j,pp0[2]+d12*u+pw,r);
		}
	}
	else	for(j=y1;j<=y2;j++)
	{
		x1 = int(pp0[0]+(pp1[0]-pp0[0])*(j-pp0[1])/(pp1[1]-pp0[1]) - pw - 3.5);
		x2 = int(pp0[0]+(pp1[0]-pp0[0])*(j-pp0[1])/(pp1[1]-pp0[1]) + pw + 3.5);
		x1=x1>nx1?x1:nx1;	x2=x2<nx2?x2:nx2-1;		if(x1>x2)	continue;

		for(i=x1;i<=x2;i++)
		{
			xx = (i-pp0[0]);	yy = (j-pp0[1]);
			u = dxu*xx+dyu*yy;	v = dxv*xx+dyv*yy;	v = v*v;
			if(u<0)			{	v += u*u;			u = 0;	}
			else if(u>dd)	{	v += (u-dd)*(u-dd);	u = dd;	}
			if(v>b)		continue;
			tt = all || (PDef & (1<<long(fmod(pPos+u/pw/1.5, 16))));
			if(!tt)		continue;
			u /= dd;
			r[0] = (unsigned char)(255.f*(cc0[0]+c10*u));
			r[1] = (unsigned char)(255.f*(cc0[1]+c11*u));
			r[2] = (unsigned char)(255.f*(cc0[2]+c12*u));
//			r[3] = (unsigned char)(255.f*exp(-6.f*v/b));
			r[3] = (unsigned char)(255.f/cosh(3.f*sqrt(v/b)));
			pnt_plot(i,j,pp0[2]+d12*u+pw,r);
		}
	}
	pPos = fmod(pPos+dd/pw/1.5, 16);
	UseAlpha = aa;
}
//-----------------------------------------------------------------------------
void mglGraphAB::mark_plot(mreal *pp, char type)
{
	unsigned char cs[4]={(unsigned char)(255.f*CDef[0]), (unsigned char)(255.f*CDef[1]),
						(unsigned char)(255.f*CDef[2]), (unsigned char)(255.f*CDef[3])};
	mreal p[12]={0,0,pp[2],0,0,pp[2],0,0,pp[2],0,0,pp[2]};
	mreal v, ss=MarkSize*0.35*font_factor;
	register long i,j,s;
	if(type=='.' || type=='C' || ss==0)
	{
		bool aa=UseAlpha;	UseAlpha = true;
		ss *= 1.1;	if(type=='C')	for(i=long(-4*ss);i<=long(4*ss);i++)
			pnt_plot(long(pp[0]+ss*cos(i*M_PI_4/ss)), long(pp[1]+ss*sin(i*M_PI_4/ss)),pp[2],cs);
		s = long(5.5+fabs(PenWidth));
		for(i=-s;i<=s;i++)	for(j=-s;j<=s;j++)
		{
			v = (i*i+j*j)/(9*PenWidth*PenWidth);
			cs[3] = (unsigned char)(255.f*exp(-6.f*v));
			if(cs[3]==0)	continue;
			pnt_plot(long(pp[0])+i,long(pp[1])+j,pp[2],cs);
		}
		UseAlpha = aa;
	}
	else
	{
		mreal pw = PenWidth;	PenWidth = BaseLineWidth;
		int pd = PDef;	PDef = 0xffff;
		register mreal zv = strchr("oOVDTS",type) ? pp[2]+BaseLineWidth : pp[2];
		if(!strchr("xsSoO",type))	ss *= 1.1;
		switch(type)
		{
		case 'P':
			p[0] = pp[0]-ss;	p[1] = pp[1]-ss;	p[3] = pp[0]+ss;	p[4] = pp[1]-ss;
			line_plot(p,p+3,CDef,CDef);
			p[0] = pp[0]-ss;	p[1] = pp[1]-ss;	p[3] = pp[0]-ss;	p[4] = pp[1]+ss;
			line_plot(p,p+3,CDef,CDef);
			p[0] = pp[0]-ss;	p[1] = pp[1]+ss;	p[3] = pp[0]+ss;	p[4] = pp[1]+ss;
			line_plot(p,p+3,CDef,CDef);
			p[0] = pp[0]+ss;	p[1] = pp[1]-ss;	p[3] = pp[0]+ss;	p[4] = pp[1]+ss;
			line_plot(p,p+3,CDef,CDef);
		case '+':
			p[0] = pp[0]-ss;	p[1] = pp[1];	p[3] = pp[0]+ss;	p[4] = pp[1];
			line_plot(p,p+3,CDef,CDef);
			p[0] = pp[0];	p[1] = pp[1]-ss;	p[3] = pp[0];	p[4] = pp[1]+ss;
			line_plot(p,p+3,CDef,CDef);
			break;
		case 'X':
			p[0] = pp[0]-ss;	p[1] = pp[1]-ss;	p[3] = pp[0]+ss;	p[4] = pp[1]-ss;
			line_plot(p,p+3,CDef,CDef);
			p[0] = pp[0]-ss;	p[1] = pp[1]-ss;	p[3] = pp[0]-ss;	p[4] = pp[1]+ss;
			line_plot(p,p+3,CDef,CDef);
			p[0] = pp[0]-ss;	p[1] = pp[1]+ss;	p[3] = pp[0]+ss;	p[4] = pp[1]+ss;
			line_plot(p,p+3,CDef,CDef);
			p[0] = pp[0]+ss;	p[1] = pp[1]-ss;	p[3] = pp[0]+ss;	p[4] = pp[1]+ss;
			line_plot(p,p+3,CDef,CDef);
		case 'x':
			p[0] = pp[0]-ss;	p[1] = pp[1]-ss;	p[3] = pp[0]+ss;	p[4] = pp[1]+ss;
			line_plot(p,p+3,CDef,CDef);
			p[0] = pp[0]+ss;	p[1] = pp[1]-ss;	p[3] = pp[0]-ss;	p[4] = pp[1]+ss;
			line_plot(p,p+3,CDef,CDef);
			break;
		case 'S':
			for(i=long(-ss-1);i<=long(ss+1);i++)	for(j=long(-ss-1);j<=long(ss+1);j++)
				pnt_plot(long(pp[0])+i,long(pp[1])+j,zv,cs);
		case 's':
			p[0] = pp[0]-ss;	p[1] = pp[1]-ss;	p[3] = pp[0]+ss;	p[4] = pp[1]-ss;
			line_plot(p,p+3,CDef,CDef);
			p[0] = pp[0]+ss;	p[1] = pp[1]-ss;	p[3] = pp[0]+ss;	p[4] = pp[1]+ss;
			line_plot(p,p+3,CDef,CDef);
			p[0] = pp[0]+ss;	p[1] = pp[1]+ss;	p[3] = pp[0]-ss;	p[4] = pp[1]+ss;
			line_plot(p,p+3,CDef,CDef);
			p[0] = pp[0]-ss;	p[1] = pp[1]+ss;	p[3] = pp[0]-ss;	p[4] = pp[1]-ss;
			line_plot(p,p+3,CDef,CDef);
			break;
		case 'D':
			for(i=long(-ss-1);i<=long(ss+1);i++)	for(j=long(-ss-1);j<=long(ss+1);j++)
				if(abs(i)+abs(j)<=long(ss+1))
					pnt_plot(long(pp[0])+i,long(pp[1])+j,zv,cs);
		case 'd':
			p[0] = pp[0];	p[1] = pp[1]-ss;	p[3] = pp[0]+ss;	p[4] = pp[1];
			line_plot(p,p+3,CDef,CDef);
			p[0] = pp[0]+ss;	p[1] = pp[1];	p[3] = pp[0];	p[4] = pp[1]+ss;
			line_plot(p,p+3,CDef,CDef);
			p[0] = pp[0];	p[1] = pp[1]+ss;	p[3] = pp[0]-ss;	p[4] = pp[1];
			line_plot(p,p+3,CDef,CDef);
			p[0] = pp[0]-ss;	p[1] = pp[1];	p[3] = pp[0];	p[4] = pp[1]-ss;
			line_plot(p,p+3,CDef,CDef);
			break;
		case 'Y':
			p[0] = pp[0];	p[1] = pp[1]-ss;	p[3] = pp[0];	p[4] = pp[1];
			line_plot(p,p+3,CDef,CDef);
			p[0] = pp[0]-0.8*ss;	p[1] = pp[1]+0.6*ss;	p[3] = pp[0];	p[4] = pp[1];
			line_plot(p,p+3,CDef,CDef);
			p[0] = pp[0]+0.8*ss;	p[1] = pp[1]+0.6*ss;	p[3] = pp[0];	p[4] = pp[1];
			line_plot(p,p+3,CDef,CDef);
			break;
		case '*':
			p[0] = pp[0]-ss;	p[1] = pp[1];	p[3] = pp[0]+ss;	p[4] = pp[1];
			line_plot(p,p+3,CDef,CDef);
			p[0] = pp[0]-0.6*ss;	p[1] = pp[1]-0.8*ss;	p[3] = pp[0]+0.6*ss;	p[4] = pp[1]+0.8*ss;
			line_plot(p,p+3,CDef,CDef);
			p[0] = pp[0]-0.6*ss;	p[1] = pp[1]+0.8*ss;	p[3] = pp[0]+0.6*ss;	p[4] = pp[1]-0.8*ss;
			line_plot(p,p+3,CDef,CDef);
			break;
		case 'T':
			for(i=long(-ss-1);i<=long(ss+1);i++)	for(j=long(-ss/2-1);j<=long(ss+1);j++)
				if(3*abs(i)+2*j<=long(2*ss+1))
					pnt_plot(long(pp[0])+i,long(pp[1])+j,zv,cs);
		case '^':
			p[0] = pp[0]-ss;	p[1] = pp[1]-ss/2;	p[3] = pp[0];	p[4] = pp[1]+ss;
			line_plot(p,p+3,CDef,CDef);
			p[0] = pp[0]-ss;	p[1] = pp[1]-ss/2;	p[3] = pp[0]+ss;	p[4] = pp[1]-ss/2;
			line_plot(p,p+3,CDef,CDef);
			p[0] = pp[0]+ss;	p[1] = pp[1]-ss/2;	p[3] = pp[0];	p[4] = pp[1]+ss;
			line_plot(p,p+3,CDef,CDef);
			break;
		case 'V':
			for(i=long(-ss-1);i<=long(ss+1);i++)	for(j=long(-ss-1);j<=long(ss/2+1);j++)
				if(3*abs(i)-2*j<=long(2*ss+1))
					pnt_plot(long(pp[0])+i,long(pp[1])+j,zv,cs);
		case 'v':
			p[0] = pp[0]-ss;	p[1] = pp[1]+ss/2;	p[3] = pp[0];	p[4] = pp[1]-ss;
			line_plot(p,p+3,CDef,CDef);
			p[0] = pp[0]-ss;	p[1] = pp[1]+ss/2;	p[3] = pp[0]+ss;	p[4] = pp[1]+ss/2;
			line_plot(p,p+3,CDef,CDef);
			p[0] = pp[0]+ss;	p[1] = pp[1]+ss/2;	p[3] = pp[0];	p[4] = pp[1]-ss;
			line_plot(p,p+3,CDef,CDef);
			break;
		case 'L':
			for(i=long(-ss-1);i<=long(ss+1);i++)	for(j=long(-ss-1);j<=long(ss/2+1);j++)
				if(3*abs(i)-2*j<=long(2*ss+1))
					pnt_plot(long(pp[0])+j,long(pp[1])+i,zv,cs);
		case '<':
			p[0] = pp[0]+ss/2;	p[1] = pp[1]-ss;	p[3] = pp[0]-ss;	p[4] = pp[1];
			line_plot(p,p+3,CDef,CDef);
			p[0] = pp[0]+ss/2;	p[1] = pp[1]+ss;	p[3] = pp[0]-ss;	p[4] = pp[1];
			line_plot(p,p+3,CDef,CDef);
			p[0] = p[3] = pp[0]+ss/2;	p[1] = pp[1]-ss;	p[4] = pp[1]+ss;
			line_plot(p,p+3,CDef,CDef);
			break;
		case 'R':
			for(i=long(-ss-1);i<=long(ss+1);i++)	for(j=long(-ss/2-1);j<=long(ss+1);j++)
				if(3*abs(i)+2*j<=long(2*ss+1))
					pnt_plot(long(pp[0])+j,long(pp[1])+i,zv,cs);
		case '>':
			p[0] = pp[0]-ss/2;	p[1] = pp[1]-ss;	p[3] = pp[0]+ss;	p[4] = pp[1];
			line_plot(p,p+3,CDef,CDef);
			p[0] = pp[0]-ss/2;	p[1] = pp[1]+ss;	p[3] = pp[0]+ss;	p[4] = pp[1];
			line_plot(p,p+3,CDef,CDef);
			p[0] = p[3] = pp[0]-ss/2;	p[1] = pp[1]-ss;	p[4] = pp[1]+ss;
			line_plot(p,p+3,CDef,CDef);
			break;
		case 'O':
			for(i=long(-ss);i<=long(ss);i++)	for(j=long(-ss);j<=long(ss);j++)
			{
				if(i*i+j*j>=ss*ss)	continue;
				pnt_plot(long(pp[0])+i,long(pp[1])+j,zv,cs);
			}
		case 'o':
			for(i=long(-4*ss);i<=long(4*ss);i++)
				pnt_plot(long(pp[0]+ss*cos(i*M_PI_4/ss)),
					long(pp[1]+ss*sin(i*M_PI_4/ss)),zv,cs);
			break;
		}
		PDef = pd;	PenWidth = pw;
	}
}
//-----------------------------------------------------------------------------
void mglGraphAB::pnt_plot(long,long,mreal,unsigned char [4]){}
//-----------------------------------------------------------------------------
