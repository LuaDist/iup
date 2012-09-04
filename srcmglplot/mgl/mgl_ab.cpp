/***************************************************************************
 * mgl_ab.cpp is part of Math Graphic Library
 * Copyright (C) 2007 Alexey Balakin <balakin@appl.sci-nnov.ru>            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <stdlib.h>
#include <time.h>
#include <wchar.h>

#include "mgl/mgl_ab.h"
#define imax(a,b)	(a)>(b) ? (a) : (b)
#define imin(a,b)	(a)<(b) ? (a) : (b)
int mglNumThr;
char *mgl_strdup(const char *s);
//-----------------------------------------------------------------------------
void strtrim_mgl(char *str);
void wcstrim_mgl(wchar_t *str);
int _mgl_tick_ext(mreal a, mreal b, wchar_t s[32], mreal &v);
void _mgl_tick_text(mreal z, mreal z0, mreal d, mreal v, int kind, wchar_t str[64]);
//-----------------------------------------------------------------------------
mglGraphAB::mglGraphAB(int w,int h) : mglGraph()
{
	G = 0;	UseLight = false;	st_pos=-1;
	memset(stack,0,MGL_STACK_ENTRY*13*sizeof(mreal));
	SetSize(w,h);	ClfOnUpdate = true;	SetDrawReg(1,1,0);
	AutoClf=true;	Delay = 1;	ObjId=0;	pPos=0;
	NoAutoFactor = false;	ShowMousePos = true;
	BDef[0] = BDef[1] = BDef[2] = BDef[3] = 255;
	DefaultPlotParam();
}
//-----------------------------------------------------------------------------
mglGraphAB::~mglGraphAB()
{	if(G)	{	delete []G;	delete []G4;	delete []OI;	}	}
//-----------------------------------------------------------------------------
void mglGraphAB::DefaultPlotParam()
{
	mglGraph::DefaultPlotParam();
}
//-----------------------------------------------------------------------------
void mglGraphAB::Rotate(mreal TetX,mreal TetZ,mreal TetY)
{
	mglGraph::Rotate( TetX,TetZ,TetY );
}
//-----------------------------------------------------------------------------
void mglGraphAB::Pop()
{
	if(st_pos<0)	return;
	mreal *c=stack+13*st_pos;
	xPos=c[0];	yPos=c[1];	zPos=c[2];	Persp=c[3];
	memcpy(B,c+4,9*sizeof(mreal));
	st_pos--;
}
//-----------------------------------------------------------------------------
void mglGraphAB::Push()
{
	st_pos = st_pos<9 ? st_pos+1:9;
	if(st_pos<0)	st_pos=0;
	mreal *c=stack+13*st_pos;
	c[0]=xPos;	c[1]=yPos;	c[2]=zPos;	c[3]=Persp;
	memcpy(c+4,B,9*sizeof(mreal));
}
//-----------------------------------------------------------------------------
void mglGraphAB::SetPosScale(mreal xp, mreal yp, mreal zp, mreal scl)
{
	memset(B,0,9*sizeof(mreal));
	B[0] = B[4] = B[8] = scl;
	xPos=xp;	yPos=yp;	zPos=zp;
}
//-----------------------------------------------------------------------------
const unsigned char *mglGraphAB::GetBits()
{	if(!Finished)	Finish();	return G;	}
//-----------------------------------------------------------------------------
const unsigned char *mglGraphAB::GetRGBA()
{	if(!Finished)	Finish();	return G4;	}
//-----------------------------------------------------------------------------
void mglGraphAB::DefColor(mglColor c, mreal alpha)
{
	if(alpha<0 || alpha>1)	alpha = AlphaDef;
	CDef[3] = Transparent ? alpha : 1;
	if(c.Valid())
	{	CDef[0] = c.r;	CDef[1] = c.g;	CDef[2] = c.b;	}
}
//-----------------------------------------------------------------------------
void mglGraphAB::Pen(mglColor col, char style,mreal width)
{
	if(col.Valid())	DefColor(col,1);
	if(style==0)	return;
	switch(style)
	{
	case '-': PDef = 0xffff;	break;
	case '|': PDef = 0x00ff;	break;
	case ';': PDef = 0x0f0f;	break;
	case '=': PDef = 0x3333;	break;
	case ':': PDef = 0x1111;	break;
	case 'j': PDef = 0x087f;	break;
	case 'i': PDef = 0x2727;	break;
	case ' ': PDef = 0x0000;	break;
//	case 0:	  PDef = 0x0000;	break;
//	default:  PDef = 0xffff;
	}
	PenWidth = width;
}
//-----------------------------------------------------------------------------
void mglGraphAB::RotateN(mreal Tet,mreal x,mreal y,mreal z)
{
	mreal R[9],C[9],c=cos(Tet*M_PI/180),s=-sin(Tet*M_PI/180),r=1-c,n=sqrt(x*x+y*y+z*z);
	x/=n;	y/=n;	z/=n;
	C[0] = x*x*r+c;		C[1] = x*y*r-z*s;	C[2] = x*z*r+y*s;
	C[3] = x*y*r+z*s;	C[4] = y*y*r+c;		C[5] = y*z*r-x*s;
	C[6] = x*z*r-y*s;	C[7] = y*z*r+x*s;	C[8] = z*z*r+c;
	memcpy(R,B,9*sizeof(mreal));
	B[0] = C[0]*R[0] + C[3]*R[1] + C[6]*R[2];
	B[1] = C[1]*R[0] + C[4]*R[1] + C[7]*R[2];
	B[2] = C[2]*R[0] + C[5]*R[1] + C[8]*R[2];
	B[3] = C[0]*R[3] + C[3]*R[4] + C[6]*R[5];
	B[4] = C[1]*R[3] + C[4]*R[4] + C[7]*R[5];
	B[5] = C[2]*R[3] + C[5]*R[4] + C[8]*R[5];
	B[6] = C[0]*R[6] + C[3]*R[7] + C[6]*R[8];
	B[7] = C[1]*R[6] + C[4]*R[7] + C[7]*R[8];
	B[8] = C[2]*R[6] + C[5]*R[7] + C[8]*R[8];
	if(AutoPlotFactor && !NoAutoFactor)
	{
		mreal m=(fabs(B[3])+fabs(B[4])+fabs(B[5]))/inH;
		mreal n=(fabs(B[0])+fabs(B[1])+fabs(B[2]))/inW;
		PlotFactor = 1.55+0.6147*(m<n ? (n-1):(m-1));
	}
}
//-----------------------------------------------------------------------------
void mglGraphAB::Perspective(mreal a)	// I'm too lazy for using 4*4 matrix
{	Persp = fabs(a)/Depth;	}
//-----------------------------------------------------------------------------
void mglGraphAB::InPlot(mreal x1,mreal x2,mreal y1,mreal y2, bool rel)
{
	SelectPen("k-1");
	if(Width<=0 || Height<=0 || Depth<=0)	return;
	memset(B,0,9*sizeof(mreal));
	if(rel)
	{
		xPos = B1[9] + (x1+x2-1)/2*B1[0];
		yPos = B1[10]+ (y1+y2-1)/2*B1[4];
		B[0] = B1[0]*(x2-x1);	B[4] = B1[4]*(y2-y1);
		B[8] = sqrt(B[0]*B[4]);
		zPos = B1[11]+ (1.f-B[8]/(2*Depth))*B1[8];
	}
	else
	{
		B1[9] = xPos = (x1+x2)/2*Width;
		B1[10]= yPos = (y1+y2)/2*Height;
		B[0] = Width*(x2-x1);	B[4] = Height*(y2-y1);
		B[8] = sqrt(B[0]*B[4]);
		B1[11]= zPos = (1.f-B[8]/(2*Depth))*Depth;
		memcpy(B1,B,9*sizeof(mreal));
	}
	inW = B[0];	inH=B[4];
	font_factor = B[0] < B[4] ? B[0] : B[4];
	if(AutoPlotFactor) PlotFactor = 1.55;	// Automatically change plot factor !!!
	Persp = 0;
}
//-----------------------------------------------------------------------------
void mglGraphAB::Aspect(mreal Ax,mreal Ay,mreal Az)
{
	mreal a = fabs(Ax) > fabs(Ay) ? fabs(Ax) : fabs(Ay);
	a = a > fabs(Az) ? a : fabs(Az);
	if(a==0)	{	SetWarn(mglWarnZero,"Aspect");	return;	}
	Ax/=a;	Ay/=a;	Az/=a;
	B[0] *= Ax;		B[3] *= Ax;		B[6] *= Ax;
	B[1] *= Ay;		B[4] *= Ay;		B[7] *= Ay;
	B[2] *= Az;		B[5] *= Az;		B[8] *= Az;
}
//-----------------------------------------------------------------------------
void mglGraphAB::StickPlot(int num, int id, mreal tet, mreal phi)
{
	mreal dx,dy,w0,h0, p[6]={-1,0,0,1,0,0};
	InPlot(0,1,0,1,true);	Rotate(tet, phi);	PostScale(p,2);
	w0=1/(1+(num-1)*fabs(p[3]-p[0])/inW);		dx=(p[3]-p[0])*w0/inW;
	h0=1/(1+(num-1)*fabs(p[4]-p[1])/inH);		dy=(p[4]-p[1])*h0/inH;

	p[0]=-1;	p[3]=1;		p[1]=p[2]=p[4]=p[5]=0;	// extra widening
	InPlot(dx>0?0:1-w0, dx>0?w0:1, dy>0?0:1-h0, dy>0?h0:1, true);
	Rotate(tet,phi);	PostScale(p,2);
	w0=1/(1+(num-1)*fabs(p[3]-p[0])/inW);		dx=(p[3]-p[0])*w0/inW;
	h0=1/(1+(num-1)*fabs(p[4]-p[1])/inH);		dy=(p[4]-p[1])*h0/inH;

	mreal x1=dx>0?dx*id:1-w0+dx*id, x2=dx>0?w0+dx*id:1+dx*id;
	mreal y1=dy>0?dy*id:1-h0+dy*id, y2=dy>0?h0+dy*id:1+dy*id;
	InPlot(x1, x2, y1, y2, true);	Rotate(tet,phi);
}
//-----------------------------------------------------------------------------
void mglGraphAB::NormScale(mreal *p,long n)
{
	register long i;
	mreal x[3], *y;
	for(i=0;i<n;i++)
	{
		y = p+3*i;
		x[0] = (y[0]*B[0] + y[1]*B[1] + y[2]*B[2])/zoomx2;
		x[1] = (y[0]*B[3] + y[1]*B[4] + y[2]*B[5])/zoomy2;
		x[2] = (y[0]*B[6] + y[1]*B[7] + y[2]*B[8])/sqrt(zoomx2*zoomy2);
		memcpy(y,x,3*sizeof(mreal));
	}
}
//-----------------------------------------------------------------------------
void mglGraphAB::PostScale(mreal *p,long n)
{
	register mreal s3=2*PlotFactor;
	register long i;
	mreal x[3], *y;
	for(i=0;i<n;i++)
	{
		y = p+3*i;
		y[0] = y[0]/s3;		y[1] = y[1]/s3;		y[2] = y[2]/s3;
		x[0] = (xPos + y[0]*B[0] + y[1]*B[1] + y[2]*B[2] - zoomx1*Width)/zoomx2;
		x[1] = (yPos + y[0]*B[3] + y[1]*B[4] + y[2]*B[5] - zoomy1*Height)/zoomy2;
		x[2] = (zPos + y[0]*B[6] + y[1]*B[7] + y[2]*B[8])/sqrt(zoomx2*zoomy2);
		if(Persp)
		{
			register mreal d = (1-Persp*Depth/2)/(1-Persp*x[2]);
			x[0] = Width/2 + d*(x[0]-Width/2);
			x[1] = Height/2 + d*(x[1]-Height/2);
		}
		memcpy(y,x,3*sizeof(mreal));
	}
}
//-----------------------------------------------------------------------------
mglPoint mglGraphAB::CalcXYZ(int xs, int ys)
{
	mreal s3 = 2*PlotFactor, x, y, z;
	// NOTE: Perspective, transformation formulas and lists are not support just now !!! Also it use LAST InPlot parameters!!!
	ys = Height - ys;
	mreal xx = xs*zoomx2 - xPos + zoomx1*Width;
	mreal yy = ys*zoomy2 - yPos + zoomy1*Height;
	mreal d1=B[0]*B[4]-B[1]*B[3], d2=B[1]*B[5]-B[2]*B[4], d3=B[0]*B[5]-B[2]*B[3];
	if(fabs(d1) > fabs(d2) && fabs(d1) > fabs(d3))	// x-y plane
	{
		z = 0;
		x = s3*(B[4]*xx-B[1]*yy)/d1;
		y = s3*(B[0]*yy-B[3]*xx)/d1;
	}
	else if(fabs(d2) > fabs(d3))	// y-z
	{
		x = 0;
		y = s3*(B[5]*xx-B[2]*yy)/d2;
		z = s3*(B[1]*yy-B[4]*xx)/d2;
	}
	else	// x-z
	{
		y = 0;
		x = s3*(B[5]*xx-B[2]*yy)/d3;
		z = s3*(B[0]*yy-B[3]*xx)/d3;
	}
	LastMousePos = mglPoint(Min.x + (Max.x-Min.x)*(x+1)/2,
					Min.y + (Max.y-Min.y)*(y+1)/2,
					Min.z + (Max.z-Min.z)*(z+1)/2);
	return LastMousePos;
}
//-----------------------------------------------------------------------------
void mglGraphAB::CalcScr(mglPoint p, int *xs, int *ys)
{
	mreal pp[3]={p.x,p.y,p.z};
	ScalePoint(pp[0],pp[1],pp[2]);
	PostScale (pp,1);
	if(xs)	*xs=int(pp[0]);
	if(ys)	*ys=int(pp[1]);
}
//-----------------------------------------------------------------------------
mglPoint mglGraphAB::CalcScr(mglPoint p)
{	int x,y;	CalcScr(p,&x,&y);	return mglPoint(x,y);	}
//-----------------------------------------------------------------------------
void mglGraphAB::LightScale()
{
	mreal *x;
	register mreal nn;
	register long i,j;
	for(i=0;i<10;i++)
	{
		if(!nLight[i])	continue;
		j=3*i;		x = rLight+j;
		pLight[j] = (x[0]*B[0] + x[1]*B[1] + x[2]*B[2])/zoomx2;
		pLight[j+1] = (x[0]*B[3] + x[1]*B[4] + x[2]*B[5])/zoomy2;
		pLight[j+2] = (x[0]*B[6] + x[1]*B[7] + x[2]*B[8])/sqrt(zoomx2*zoomy2);
		nn=sqrt(pLight[j]*pLight[j]+pLight[j+1]*pLight[j+1]+pLight[j+2]*pLight[j+2]);
		pLight[j] /= nn;	pLight[j+1] /= nn;	pLight[j+2] /= nn;
	}
}
//-----------------------------------------------------------------------------
void mglGraphAB::Light(int n, bool enable)
{
	if(n<0 || n>9)	{	SetWarn(mglWarnLId);	return;	}
	nLight[n] = enable;
}
//-----------------------------------------------------------------------------
void mglGraphAB::Light(int n, mglPoint p, mglColor c, mreal br, bool inf, mreal ap)
{
	if(n<0 || n>9)	{	SetWarn(mglWarnLId);	return;	}
	nLight[n] = true;	aLight[n] = ap>0?ap*ap:3;
	bLight[n] = br;		iLight[n] = inf;
	rLight[3*n] = p.x;	rLight[3*n+1] = p.y;	rLight[3*n+2] = p.z;
	cLight[3*n] = c.r;	cLight[3*n+1] = c.g;	cLight[3*n+2] = c.b;
}
//-----------------------------------------------------------------------------
mreal mglGraphAB::Putsw(mglPoint p,mglPoint n,const wchar_t *str,char font,mreal size)
{
	static int cgid=1;	StartGroup("PutswL",cgid++);
	mreal pp[6] = {p.x,p.y,p.z,p.x+n.x,p.y+n.y,p.z+n.z};
	Arrow1 = Arrow2 = '_';

	if(size<0)	size = -size*FontSize;
	f_size = size;	Push();
	mreal shift = 0.07, fsize=size/8.*font_factor;
	mreal x1=zoomx1, x2=zoomx2, y1=zoomy1, y2=zoomy2;
	if(font=='t')	shift = -0.07;

	shift *= fsize/2;
	if(ScalePuts)
	{
		bool tt = Cut;	Cut = true;
		ScalePoint(pp[0],pp[1],pp[2]);
		ScalePoint(pp[3],pp[4],pp[5]);
		Cut = tt;
	}
	zoomx1=zoomy1=0;	zoomx2=zoomy2=1;
	PostScale(pp,2);
	zoomx1=x1;	zoomx2=x2;	zoomy1=y1;	zoomy2=y2;	Persp=0;

	mreal ll=(pp[3]-pp[0])*(pp[3]-pp[0])+(pp[4]-pp[1])*(pp[4]-pp[1]);
	if(ll==0)	{	Pop();	EndGroup();	return 0;	}
	mreal tet = 180*atan2(pp[4]-pp[1],pp[3]-pp[0])/M_PI;
//	if(fabs(tet)>90)	tet+=180;
	memset(B,0,9*sizeof(mreal));
	B[0] = B[4] = B[8] = fsize;
	fscl = fsize;	ftet = -tet;
	NoAutoFactor=true;	RotateN(-tet,0,0,1);	NoAutoFactor=false;
	xPos = pp[0]+shift*(pp[4]-pp[1])/sqrt(ll) - B[1]*0.02f;
	yPos = pp[1]-shift*(pp[3]-pp[0])/sqrt(ll) - B[4]*0.02f;
	zPos = pp[2];

	fsize = fnt->Puts(str,"rL",0)*size/8.;
	Pop();		EndGroup();
	return fsize;
}
//-----------------------------------------------------------------------------
void mglGraphAB::Putsw(mglPoint p, const wchar_t *wcs, const char *font, mreal size, char dir, mreal sh)
{
	static int nl_shift = 0;
	static int cgid=1;	StartGroup("Putsw",cgid++);
	if(font && strchr(font, 'A'))	{	Labelw(p.x, p.y, wcs,font,size,false);	return;	}
	if(font && strchr(font, 'a'))	{	Labelw(p.x, p.y, wcs,font,size,true);	return;	}

	wchar_t *wcl;
	unsigned wn=0;
	const wchar_t *wnl=0;
	for(wn=0;wn<wcslen(wcs);wn++)
		if(wcs[wn]=='\n' || (wcs[wn]=='\\' && wcs[wn+1]=='n' && strchr(" \t,.{[]1234567890",wcs[wn+2])))
		{	wnl = wcs+wn;	break;	}
	if(wnl)
	{
		wcl = new wchar_t[wn+1];	memcpy(wcl,wcs,wn*sizeof(wchar_t));
		wcl[wn]=0;	Putsw(p, wcl, font, size, dir, sh);
		wnl = (*wnl=='\n')?wnl+1:wnl+2;
		if(*wnl<=' ' && *wnl>0)	wnl++;
		nl_shift++;	Putsw(p, wnl, font, size, dir, sh);	nl_shift--;
		return;
	}

	bool upside = ( (((_sx==-1) ^ (Org.y==Max.y || Org.z==Max.z)) && (dir=='x' || dir=='X')) ||
					(((_sy==-1) ^ (Org.x==Max.x || Org.z==Max.z)) && (dir=='y' || dir=='Y')) ||
					(((_st==1)) && (dir=='t' || dir=='T')) ||
					(((_sz==-1) ^ (Org.y==Max.y || Org.x==Max.x)) && (dir=='z' || dir=='Z')) );
	mreal pp[6] = {p.x,p.y,p.z,p.x,p.y,p.z};
	Arrow1 = Arrow2 = '_';
	char *f, *font1;
	if(font)	{	font1=new char[strlen(font)+2];	strcpy(font1,font);	}
	else	{	font1=new char[strlen(FontDef)+2];	strcpy(font1,FontDef);	}
	char col=TranspType!=2 ? 'k':'w';
	f = (char*)strchr(font1,':');	if(f)	{	f[0]=0;	col=f[1];	}
//	stl[1] = col;	SelectPen(stl);

	if(size<0)	size = -size*FontSize;
	f_size = size;	Push();
	mreal shift = (sh/10+0.2)*2/PlotFactor, fsize=size/8.*font_factor;
	mreal x1=zoomx1, x2=zoomx2, y1=zoomy1, y2=zoomy2;
	zoomx1=zoomy1=0;	zoomx2=zoomy2=1;

	if(strchr("xyzt",dir))	shift = (sh/10+0.1)*2/PlotFactor;
//	shift *= size/7;
	switch(dir)
	{
	case 'x':
	case 'X':
//		pp[3] += pp[3] ? fabs(pp[3])*.001 :(Max.x - Min.x)/100;	break;
		pp[3] += (Max.x - Min.x)/1000;	break;
	case 'y':
	case 'Y':
		if(TernAxis)	upside = !upside;
//		pp[4] += pp[4] ? fabs(pp[4])*.001 : (Max.y - Min.y)/10;	break;
		pp[4] += (Max.y - Min.y)/1000;	break;
	case 'z':
	case 'Z':
		if(TernAxis)	upside = !upside;
//		pp[4] -= 0;		pp[5] += pp[5] ? fabs(pp[5])*.001 :(Max.z - Min.z)/100;	break;
		pp[5] += (Max.z - Min.z)/1000;	break;
	case 't':
	case 'T':
		pp[3] += (Max.x - Min.x)/1000;	pp[4] -= (Max.y - Min.y)/1000;	break;
	}
	if(upside)	shift = -shift;
	shift *= fsize/2;

	if(ScalePuts)
	{
		bool tt = Cut;	Cut = true;
		ScalePoint(pp[0],pp[1],pp[2]);
		ScalePoint(pp[3],pp[4],pp[5]);
		Cut = tt;
	}
	PostScale(pp,2);	Persp=0;

	if(dir==0)
	{
		xPos = pp[0];	yPos = pp[1];	zPos = pp[2];
		memset(B,0,9*sizeof(mreal));
		B[0] = B[4] = B[8] = fsize;
		fscl = fsize;	ftet = 0;
	}
	else if(RotatedText)
	{
		if(pp[4]==pp[1] && pp[3]==pp[0])
		{	zoomx1=x1;	zoomx2=x2;	zoomy1=y1;	zoomy2=y2;
			delete []font1;	Pop();	EndGroup();	return;	}
		mreal ll=(pp[3]-pp[0])*(pp[3]-pp[0])+(pp[4]-pp[1])*(pp[4]-pp[1]);
		mreal tet = 180*atan2(pp[4]-pp[1],pp[3]-pp[0])/M_PI;
		if(fabs(tet)>90)	tet+=180;
		memset(B,0,9*sizeof(mreal));
		B[0] = B[4] = B[8] = fsize;
		fscl = fsize;	ftet = -tet;
		NoAutoFactor=true;	RotateN(-tet,0,0,1);	NoAutoFactor=false;
		mreal ss = (pp[3]>pp[0] || tet==-90) ? 1 : -1;
		xPos = pp[0]+shift*ss*(pp[4]-pp[1])/sqrt(ll) - B[1]*0.02f;
		yPos = pp[1]-shift*ss*(pp[3]-pp[0])/sqrt(ll) - B[4]*0.02f;
		zPos = pp[2];
	}
	else
	{
		if(pp[4]==pp[1] && pp[3]==pp[0])
		{	zoomx1=x1;	zoomx2=x2;	zoomy1=y1;	zoomy2=y2;
			delete []font1;	Pop();	EndGroup();	return;	}
		mreal ll=(pp[3]-pp[0])*(pp[3]-pp[0])+(pp[4]-pp[1])*(pp[4]-pp[1]);
		mreal tet = atan2(pp[4]-pp[1],pp[3]-pp[0]);
//		if(fabs(tet)>90)	tet+=180;
		memset(B,0,9*sizeof(mreal));	B[0] = B[4] = B[8] = fsize;
		mreal ss = (pp[3]>pp[0] || tet==-M_PI/2) ? 1 : -1;
		fscl = fsize;	ftet = 0;
		xPos = pp[0]+shift*ss*(pp[4]-pp[1])/sqrt(ll) - B[1]*0.02f;
		yPos = pp[1]-shift*ss*(pp[3]-pp[0])/sqrt(ll) - B[4]*0.02f;
		zPos = pp[2];

		if(!strchr(font1,'R') && !strchr(font1,'C') && !strchr(font1,'L'))
		{
			char ch[2]="C";
//			if((pp[4]-pp[1])*(pp[3]-pp[0])>0) ch[0]=upside?'R':'L';	else ch[0]=upside?'L':'R';
			if(fabs(sin(tet))>0.1)	ch[0] = shift*ss*sin(tet)>0 ? 'L':'R';
			strcat(font1,ch);
		}
	}
	zoomx1=x1;	zoomx2=x2;	zoomy1=y1;	zoomy2=y2;
	yPos -= nl_shift*0.11/PlotFactor*fsize;
	fnt->Puts(wcs,font1,col);
	Pop();	delete []font1;	EndGroup();
}
//-----------------------------------------------------------------------------
void mglGraphAB::Legend(int n, wchar_t **text,char **style, mreal x, mreal y,
					const char *font, mreal size, mreal llen)
{
	if(n<1)	{	SetWarn(mglWarnLeg);	return;	}
	x = 2*x-1;	y = 2*y-1;
	if(isnan(llen))	llen=0.1;
	static int cgid=1;	StartGroup("Legend",cgid++);
	mreal pp[15], r=GetRatio(), rh, rw, s3=PlotFactor;
	if(size<=0)	size = -size*FontSize;
	if(!font || !(*font))	font="L";
	char *pA, *ff = new char[strlen(font)+1];	strcpy(ff,font);
	llen *= 1.5;

	rh=(r<1?r:1.)*size/6.;	rw=(r>1?1./r:1.)*size/8.;
//	rh=size/6.;	rw=size/24.;
	mreal w=0, h=fnt->Height(font)*rh, j;
	register long i;
	for(i=0;i<n;i++)		// find text length
	{
		j = fnt->Width(text[i],font)*rw;
		if(style[i][0]==0)	j -= llen;
		w = w>j ? w:j;
	}
	w = (w + llen*1.1f);	// add space for lines

	bool rel = true;
	if((pA=(char*)strchr(ff,'A')))
	{
		*pA = 'L';
		rel = false;
	}
	Push();	Identity(rel);	//	memcpy(B,B1,9*sizeof(mreal));
	if(LegendBox)	// draw bounding box
	{
    mreal mw = w*0.1f;
    mreal mh = h*0.1f;
		pp[2] = pp[5] = pp[8] = pp[11] = pp[14] = s3-0.01;
		pp[0] = pp[9] = pp[12] = x-mw;		pp[3] = pp[6] = x+w+2*mw;
		pp[1] = pp[4] = pp[13] = y-mh;		pp[7] = pp[10] = y+h*n+2*mh;
//		for(i=0;i<5;i++)	ScalePoint(pp[3*i],pp[3*i+1],pp[3*i+2]);
		SelectPen(TranspType!=2 ? "k-1":"w-1");
		curv_plot(5,pp,0);	// bounding rectangle
		pp[2] = pp[5] = pp[8] = pp[11] = s3-0.01;
		pp[0] = pp[6] = x-mw;			pp[3] = pp[9] = x+w+2*mw;
		pp[1] = pp[4] = y-mh;		pp[7] = pp[10] = y+h*n+2*mh;

    
		DefColor(mglColor(BDef[0]/255.0f,BDef[1]/255.0f,BDef[2]/255.0f),1);
//		DefColor(mglColor(1,1,1),1);
//		for(i=0;i<4;i++)	ScalePoint(pp[3*i],pp[3*i+1],pp[3*i+2]);
		surf_plot(2,2,pp,0,0);		// white rectangle below it
	}
	ScalePuts = false;

	for(i=0;i<n;i++)	// draw lines and legend
	{
		char m=SelectPen(style[i]);
		pp[2] = pp[5] = pp[8] = s3;
		pp[1] = pp[4] = pp[7] = y+i*h+0.45f*h;
		pp[0] = x+0.1f*llen;	pp[3] = x+0.9f*llen;	pp[6] = x+0.5f*llen;
//		ScalePoint(pp[0],pp[1],pp[2]);	ScalePoint(pp[3],pp[4],pp[5]);	ScalePoint(pp[6],pp[7],pp[8]);

		pPos=0;	curv_plot(2,pp,0);
		if(m)	for(int j=0;j<LegendMarks;j++)
		{
			pp[8] = s3;	pp[7] = y+i*h+0.45f*h;
			pp[6] = x+0.1f*llen + (j+1)*0.8f*llen/(1.+LegendMarks);
//			ScalePoint(pp[6],pp[7],pp[8]);
			Mark(pp[6],pp[7],pp[8],m);
		}
		SelectPen(TranspType!=2 ? "k-1":"w-1");
		Putsw(mglPoint(x+(style[i][0]!=0?llen:0), y+i*h+0.3f*h, s3), text[i], ff, size);
	}
	ScalePuts = true;
	Pop();	EndGroup();	delete []ff;
}
//-----------------------------------------------------------------------------
void mglGraphAB::colorbar(const mglData &vv, const mglColor *cs, int where, mreal x, mreal y, mreal w, mreal h)
{
	static int cgid=1;	StartGroup("Colorbar",cgid++);
	register long i,n=vv.nx;
	mreal *pp,*cc,d,s3=PlotFactor,ss=s3*0.9,v1=vv.Minimal();

	pp = new mreal[12*n];	cc = new mreal[16*n];
	x = 2*x-1;	y = 2*y-1;
	for(i=0;i<n-1;i++)
	{
		d = GetA(vv.a[i]);	//(vv.a[i]-v1)*dv-1;
		pp[12*i+0] = pp[12*i+3] = (ss*d+s3)*w+x*s3;
		pp[12*i+1] = pp[12*i+4] = (ss*d+s3)*h+y*s3;
		pp[12*i+2] = pp[12*i+5] = s3;
		switch(where)
		{
			case 1:	pp[12*i]  = x*s3;	pp[12*i+3] = (x+0.1*w)*s3;	break;
			case 2:	pp[12*i+1]= (y-0.1*h)*s3;	pp[12*i+4] = y*s3;	break;
			case 3:	pp[12*i+1]= y*s3;	pp[12*i+4] = (y+0.1*h)*s3;	break;
			default:pp[12*i]  = (x-0.1*w)*s3;	pp[12*i+3] = x*s3;	break;
		}
		d = GetA(vv.a[i+1]);	//(vv.a[i+1]-v1)*dv-1;
		pp[12*i+6] = pp[12*i+9] = (ss*d+s3)*w+x*s3;
		pp[12*i+7] = pp[12*i+10]= (ss*d+s3)*h+y*s3;
		pp[12*i+8] = pp[12*i+11]= s3;
		switch(where)
		{
			case 1:	pp[12*i+9] = x*s3;	pp[12*i+6] = (x+0.1*w)*s3;	break;
			case 2:	pp[12*i+10]= (y-0.1*h)*s3;	pp[12*i+7] = y*s3;	break;
			case 3:	pp[12*i+10]= y*s3;	pp[12*i+7] = (y+0.1*h)*s3;	break;
			default:pp[12*i+9] = (x-0.1*w)*s3;	pp[12*i+6] = x*s3;	break;
		}
		cc[16*i+0] = cc[16*i+4] = cc[16*i+8] = cc[16*i+12] = cs[i].r;
		cc[16*i+1] = cc[16*i+5] = cc[16*i+9] = cc[16*i+13] = cs[i].g;
		cc[16*i+2] = cc[16*i+6] = cc[16*i+10]= cc[16*i+14] = cs[i].b;
		cc[16*i+3] = cc[16*i+7] = cc[16*i+11]= cc[16*i+15] = 1;
	}
	Push();	memcpy(B,B1,9*sizeof(mreal));
	bool ll = UseLight;			UseLight = false;
//	surf_plot(2, n-1, pp, cc, 0);	UseLight = ll;
	quads_plot(n-1, pp, cc, 0);	UseLight = ll;

	const char *a="rC";
	if(where==0)	a = "rR";
	if(where==1)	a = "rL";
	if(where==2)	a = "rDC";
	mglPoint p;
	ScalePuts = false;
	SelectPen(TranspType!=2 ? "k-1":"w-1");

	mreal v=0,t;
	int kind=0;
	wchar_t s[32]=L"", str[64];
	if(ctt[0]==0 && TuneTicks) kind = _mgl_tick_ext(Cmax, Cmin, s, v);

	long m=n;
/*	if(n>20)	// adjust ticks
	{
		float d = fabs(vv.Maximal()-v1);
		d = floor(d*pow(10,-floor(log10(d))));
		if(d<4)	m = 1+int(2*d+0.5);
		else	m = 1+int(d+0.5);
		if(m<5)	m = 5;
	}*/
	if(dc<0) m=int(0.5-dc);
	if(dc>0) m=int(0.5+(Cmax-Cmin)/dc);
	if(dc==0 && Cmax*Cmin>0)	m = 1+int(0.5+fabs(log10(Cmax/Cmin)));
	if(m<2)	m=2;
	if(m>n)	m=n;

	mreal dv=(vv.Maximal()-v1)/m;
	for(i=0;i<m;i++)
	{
		t = vv.a[(n-1)*i/(m-1)];	d = GetA(t);	p.z = s3+1;
		p.x = (ss*d+s3)*w+x*s3;
		p.y = (ss*d+s3)*h+y*s3;
		switch(where)
		{
			case 1:	p.x = (x+0.13*w)*s3;	break;
			case 2:	p.y = (y-0.13*h)*s3;	break;
			case 3:	p.y = (y+0.13*h)*s3;	break;
			default:p.x = (x-0.13*w)*s3;	break;
		}
		if(ctt[0])	mglprintf(str, 64, ctt, t);
		else if(dc==0)
		{
			long kk=int(floor(0.1+log10(fabs(t))));
			mglprintf(str,64,L"%.2g\\cdot 10^{%d}",t/pow(10.,kk), kk);
		}
		else	_mgl_tick_text(t,v1,dv/100,v,kind,str);
		wcstrim_mgl(str);	Putsw(p,str,a,FontSize);
		// draw "grid" lines
		pp[0] = pp[3] = (ss*d+s3)*w+x*s3;
		pp[1] = pp[4] = (ss*d+s3)*h+y*s3;
		pp[2] = pp[5] = s3+1;
		switch(where)
		{
			case 1:	pp[0]  = x*s3;	pp[3] = (x+0.1*w)*s3;	break;
			case 2:	pp[1]= (y-0.1*h)*s3;	pp[4] = y*s3;	break;
			case 3:	pp[1]= y*s3;	pp[4] = (y+0.1*h)*s3;	break;
			default:pp[0]  = (x-0.1*w)*s3;	pp[3] = x*s3;	break;
		}
		memset(cc,0,6*sizeof(mreal));	PostScale (pp,2);
		line_plot(pp,pp+3,cc,cc+3,true);
	}
	switch(where)
	{
		case 1:	p = mglPoint((x+0.15*w)*s3, (y+1.75*h)*s3, s3+1);	break;
		case 2:	p = mglPoint((x+1.75*w)*s3, (y-0.15*h)*s3, s3+1);	break;
		case 3:	p = mglPoint((x+1.75*w)*s3, (y+0.15*h)*s3, s3+1);	break;
		default:p = mglPoint((x-0.15*w)*s3, (y+1.75*h)*s3, s3+1);	break;
	}
	if(kind&2)	Putsw(p,s,a,FontSize);
	ScalePuts = true;
	Pop();	EndGroup();
	delete []pp;	delete []cc;
}
//-----------------------------------------------------------------------------
void mglGraphAB::SetSize(int w,int h)
{
	if(w<=0 || h<=0)	{	SetWarn(mglWarnSize);	return;	}
	Width = w;	Height = h;	Depth = long(sqrt(mreal(w*h)));
	zPos = 0;//long(2*Depth*sqrt(3.));
	if(G)	{	delete []G;	delete []G4;	delete []OI;	}
	G = new unsigned char[w*h*3];
	G4= new unsigned char[w*h*4];
	OI= new int[w*h];
	Clf();
	InPlot(0,1,0,1);	SetDrawReg(1,1,0);	Persp = 0;
}
//-----------------------------------------------------------------------------
void mglGraphAB::WriteSVG(const char *fname, const char *descr)
{
	time_t now;
	time(&now);
	char *pname = new char[strlen(fname)+5];
	sprintf(pname,"%s.png",fname);
	WritePNG(pname,descr);

	FILE *fp = fopen(fname,"wt");
	if(!fp)		{	SetWarn(mglWarnOpen,fname);	return;	}
	fprintf(fp,"<?xml version=\"1.0\" standalone=\"no\"?>\n");
	fprintf(fp,"<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 20000303 Stylable//EN\" \"http://www.w3.org/TR/2000/03/WD-SVG-20000303/DTD/svg-20000303-stylable.dtd\">\n");
	fprintf(fp,"<svg width=\"%d\" height=\"%d\">\n",Width,Height);

	fprintf(fp,"<!--Creator: MathGL library-->\n");
	fprintf(fp,"<!--Title: %s-->\n<!--CreationDate: %s-->\n\n",descr?descr:fname,ctime(&now));
	fprintf(fp,"<g fill=\"none\" stroke=\"none\" stroke-width=\"0.5\">\n");

	fprintf(fp,"<image x=\"0\" y=\"0\" width=\"%d\" height=\"%d\" xlink:href=\"%s\"/>\n", Width, Height, pname);

	fprintf(fp,"</g></svg>");
	delete []pname;
}
//-----------------------------------------------------------------------------
void mglGraphAB::arrow_plot(mreal *p1,mreal *p2,char st)
{
	if(!strchr("AVKSDTIO",st))	return;
	mreal lx=p1[0]-p2[0], ly=p1[1]-p2[1], ll, kx,ky;
	mreal pp[12];
	ll = hypot(lx,ly)/(ArrowSize*0.35*font_factor);		if(ll==0)	return;
	lx /= ll;	ly /= ll;	kx = ly;	ky = -lx;
	pp[2] = pp[5] = pp[8] = pp[11] = p1[2];

	bool ul = UseLight;		UseLight = false;
	switch(st)
	{
	case 'I':
		pp[0] = p1[0]+kx;	pp[1] = p1[1]+ky;
		pp[3] = p1[0]-kx;	pp[4] = p1[1]-ky;
		line_plot(pp,pp+3,CDef,CDef);	break;
	case 'D':
		pp[0] = p1[0]+kx;	pp[1] = p1[1]+ky;
		pp[3] = p1[0]+lx;	pp[4] = p1[1]+ly;
		pp[6] = p1[0]-kx;	pp[7] = p1[1]-ky;
		pp[9] = p1[0]-lx;	pp[10] = p1[1]-ly;
		quad_plot(pp,pp+3,pp+9,pp+6,CDef,CDef,CDef,CDef);	break;
	case 'S':
		pp[0] = p1[0]+kx-lx;	pp[1] = p1[1]+ky-ly;
		pp[3] = p1[0]-kx-lx;	pp[4] = p1[1]-ky-ly;
		pp[6] = p1[0]-kx+lx;	pp[7] = p1[1]-ky+ly;
		pp[9] = p1[0]+kx+lx;	pp[10] = p1[1]+ky+ly;
		quad_plot(pp,pp+3,pp+9,pp+6,CDef,CDef,CDef,CDef);	break;
	case 'T':
		pp[0] = p1[0]-kx-lx;	pp[1] = p1[1]-ky-ly;
		pp[3] = p1[0]+kx-lx;	pp[4] = p1[1]+ky-ly;
		pp[6] = p1[0]+lx;		pp[7] = p1[1]+ly;
		trig_plot(pp,pp+3,pp+6,CDef,CDef,CDef);	break;
	case 'A':
		pp[0] = p1[0];			pp[1] = p1[1];
		pp[3] = p1[0]-kx-2*lx;	pp[4] = p1[1]-ky-2*ly;
		pp[6] = p1[0]-1.5*lx;	pp[7] = p1[1]-1.5*ly;
		pp[9] = p1[0]+kx-2*lx;	pp[10] = p1[1]+ky-2*ly;
		quad_plot(pp,pp+3,pp+9,pp+6,CDef,CDef,CDef,CDef);	break;
	case 'K':
		pp[0] = p1[0];			pp[1] = p1[1];
		pp[3] = p1[0]-kx-2*lx;	pp[4] = p1[1]-ky-2*ly;
		pp[6] = p1[0]-1.5*lx;	pp[7] = p1[1]-1.5*ly;
		pp[9] = p1[0]+kx-2*lx;	pp[10] = p1[1]+ky-2*ly;
		quad_plot(pp,pp+3,pp+9,pp+6,CDef,CDef,CDef,CDef);
		pp[0] = p1[0]+kx;	pp[1] = p1[1]+ky;
		pp[3] = p1[0]-kx;	pp[4] = p1[1]-ky;
		line_plot(pp,pp+3,CDef,CDef);	break;
	case 'V':
		pp[0] = p1[0];			pp[1] = p1[1];
		pp[3] = p1[0]-kx+2*lx;	pp[4] = p1[1]-ky+2*ly;
		pp[6] = p1[0]+1.5*lx;	pp[7] = p1[1]+1.5*ly;
		pp[9] = p1[0]+kx+2*lx;	pp[10] = p1[1]+ky+2*ly;
		quad_plot(pp,pp+3,pp+9,pp+6,CDef,CDef,CDef,CDef);	break;
	case 'O':
	{
		pp[0] = p1[0];	pp[1] = p1[1];
		double t;
		for(int i=0;i<16;i++)
		{
			t = M_PI*i/8.;
			pp[3] = p1[0] + kx*sin(t) + lx*cos(t);
			pp[4] = p1[1] + ky*sin(t) + ly*cos(t);
			t = M_PI*(i+1)/8.;
			pp[6] = p1[0] + kx*sin(t) + lx*cos(t);
			pp[7] = p1[1] + ky*sin(t) + ly*cos(t);
			trig_plot(pp,pp+3,pp+6,CDef,CDef,CDef);
		}
		break;
	}
/*		mreal ss = MarkSize;	MarkSize = ArrowSize;
		mark_plot(p1,'O');
		MarkSize = ss;*/
	}
	UseLight = ul;
}
//-----------------------------------------------------------------------------
unsigned char **mglGraphAB::GetRGBLines(long &w, long &h, unsigned char *&f, bool alpha)
{
	long d = alpha ? 4:3;
	unsigned char **p;
	if(!Finished)	Finish();
	p = (unsigned char **)malloc(Height * sizeof(unsigned char *));
	for(long i=0;i<Height;i++)	p[i] = (alpha?G4:G)+d*Width*i;
	w = Width;	h = Height;		f = 0;
	return p;
}
//-----------------------------------------------------------------------------
/* for 72pdi, FontSize=6 --> 26pt */
void mglGraphAB::SetFontSizePT(mreal pt, int dpi)
{	FontSize = 16.6154*pt/dpi;	}
//-----------------------------------------------------------------------------
void mglGraphAB::FindOptOrg(mreal ax[3], mreal ay[3], mreal az[3])
{
	static mreal px[3]={0,0,0}, py[3]={0,0,0}, pz[3]={0,0,0},
				bb[9]={1e30,0,0, 0,0,0, 0,0,0};
	static mglPoint m1, m2;
	mreal nn[24]={0,0,0, 0,0,1, 0,1,0, 0,1,1, 1,0,0, 1,0,1, 1,1,0, 1,1,1};
	mreal pp[24];
	memcpy(pp, nn, 24*sizeof(mreal));
	// do nothing if transformation matrix the same
	if(memcmp(B,bb,9*sizeof(mreal)) || m1!=Min || m2!=Max)
	{
		m1 = Min;	m2 = Max;
		memcpy(bb,B,9*sizeof(mreal));	PostScale(pp,8);
		// find point with minimal y
		register long i,j;
		for(i=j=1;i<24;i+=3)	if(pp[i]<pp[j])	j=i;
		j--;	memcpy(pp,pp+j,3*sizeof(mreal));
		// find max angle and left point
		// first select 3 closest points
		memcpy(pp+3,nn+j,3*sizeof(mreal));	pp[3]=1-pp[3];	// along x
		memcpy(pp+6,nn+j,3*sizeof(mreal));	pp[7]=1-pp[7];	// along y
		memcpy(pp+9,nn+j,3*sizeof(mreal));	pp[11]=1-pp[11];// along z
		PostScale(pp+3,3);
		pp[3] -= pp[0];	pp[4] -= pp[1];	pp[5] -= pp[2];
		pp[6] -= pp[0];	pp[7] -= pp[1];	pp[8] -= pp[2];
		pp[9] -= pp[0];	pp[10]-= pp[1];	pp[11]-= pp[2];
		// find cosine of axis projection
		mreal cxy, cxz, cyz, dx, dy, dz;
		dx = pp[3]*pp[3] + pp[4]*pp[4];
		dy = pp[6]*pp[6] + pp[7]*pp[7];
		dz = pp[9]*pp[9] + pp[10]*pp[10];
		cxy= pp[3]*pp[6] + pp[4]*pp[7];
		cxz= pp[3]*pp[9] + pp[4]*pp[10];
		cyz= pp[9]*pp[6] + pp[10]*pp[7];
		if(dx==0)		cxy = cxz = 1;
		else if(dy==0)	cxy = cyz = 1;
		else if(dz==0)	cyz = cxz = 1;
		else
		{	cxy /= sqrt(dx*dy);	cxz /= sqrt(dx*dz);	cyz /= sqrt(dz*dy);	}
		// find points for axis
		memcpy(px,nn+j,3*sizeof(mreal));
		memcpy(py,nn+j,3*sizeof(mreal));
		memcpy(pz,nn+j,3*sizeof(mreal));
		if(cxy<cxz && cxy<cyz)	// xy is lowest
		{	// px, py the same as pp
			if(pp[3]<pp[6])	pz[0] = 1-pz[0];
			else	pz[1] = 1-pz[1];
		}
		if(cxz<cxy && cxz<cyz)	// xz is lowest
		{	// px, pz the same as pp
			if(pp[3]<pp[9])	py[0] = 1-py[0];
			else	py[2] = 1-py[2];
		}
		if(cyz<cxz && cyz<cxy)	// yz is lowest
		{	// py, pz the same as pp
			if(pp[9]<pp[6])	px[2] = 1-px[2];
			else	px[1] = 1-px[1];
		}
		// return to normal variables
		// NOTE: this may not work in "inverse" curvilinear coordinates like x->1-x
		px[0] = Min.x + (Max.x-Min.x)*px[0];
		px[1] = Min.y + (Max.y-Min.y)*px[1];
		px[2] = Min.z + (Max.z-Min.z)*px[2];
		py[0] = Min.x + (Max.x-Min.x)*py[0];
		py[1] = Min.y + (Max.y-Min.y)*py[1];
		py[2] = Min.z + (Max.z-Min.z)*py[2];
		pz[0] = Min.x + (Max.x-Min.x)*pz[0];
		pz[1] = Min.y + (Max.y-Min.y)*pz[1];
		pz[2] = Min.z + (Max.z-Min.z)*pz[2];
	}
	// just copy saved values
	memcpy(ax,px,3*sizeof(mreal));
	memcpy(ay,py,3*sizeof(mreal));
	memcpy(az,pz,3*sizeof(mreal));
}
//-----------------------------------------------------------------------------
mreal mglGraphAB::GetOrgX(char dir)
{
	mreal res = Org.x;
	if(isnan(res))
	{
		mreal ax[3], ay[3], az[3];
		FindOptOrg(ax,ay,az);
		if(dir=='x')		res = ax[0];
		else if(dir=='y')	res = ay[0];
		else if(dir=='z')	res = az[0];
		else if(dir=='t')	res = Min.x;
		else res = B[6]>0 ? Max.x:Min.x;
	}
	return res;
}
//-----------------------------------------------------------------------------
mreal mglGraphAB::GetOrgY(char dir)
{
	mreal res = Org.y;
	if(isnan(res))
	{
		mreal ax[3], ay[3], az[3];
		FindOptOrg(ax,ay,az);
		if(dir=='x')		res = ax[1];
		else if(dir=='y')	res = ay[1];
		else if(dir=='z')	res = az[1];
		else if(dir=='t')	res = Min.y;
		else res = B[7]>0 ? Max.y:Min.y;
	}
	return res;
}
//-----------------------------------------------------------------------------
mreal mglGraphAB::GetOrgZ(char dir)
{
	mreal res = Org.z;
	if(isnan(res))
	{
		mreal ax[3], ay[3], az[3];
		FindOptOrg(ax,ay,az);
		if(dir=='x')		res = ax[2];
		else if(dir=='y')	res = ay[2];
		else if(dir=='z')	res = az[2];
		else if(dir=='t')	res = Min.z;
		else res = B[8]>0 ? Max.z:Min.z;
	}
	return res;
}
//-----------------------------------------------------------------------------
void mglGraphAB::DrawTick(mreal *pp,bool sub)
{
	ScalePoint(pp[0],pp[1],pp[2]);
	ScalePoint(pp[3],pp[4],pp[5]);
	ScalePoint(pp[6],pp[7],pp[8]);
	PostScale(pp,3);
	double dx,dy,dz,dd,ll=TickLen*(B1[0]<B1[4] ? B1[0]:B1[4])/3.5;
	if(sub)	ll/=sqrt(1.+st_t);
	dx=pp[0]-pp[3];	dy=pp[1]-pp[4];	dz=pp[2]-pp[5];
	dd=sqrt(dx*dx+dy*dy+dz*dz);	if(dd)	dd = ll/dd;
	pp[0]=pp[3]+dd*dx;	pp[1]=pp[4]+dd*dy;	pp[2]=pp[5]+dd*dz;
	dx=pp[6]-pp[3];	dy=pp[7]-pp[4];	dz=pp[8]-pp[5];
	dd=sqrt(dx*dx+dy*dy+dz*dz);	if(dd)	dd = ll/dd;
	pp[6]=pp[3]+dd*dx;	pp[7]=pp[4]+dd*dy;	pp[8]=pp[5]+dd*dz;
	line_plot(pp,pp+3,CDef,CDef);
	line_plot(pp+3,pp+6,CDef,CDef);
}
//-----------------------------------------------------------------------------
bool mglGraphAB::Alpha(bool enable)
{	bool t=UseAlpha;	UseAlpha=enable;	return t;	}
bool mglGraphAB::Light(bool enable)
{	bool t=UseLight;	UseLight=enable;	return t;	}
//-----------------------------------------------------------------------------
void mglGraphAB::Update(){}
void mglGraphAB::ToggleAlpha(){}
void mglGraphAB::ToggleLight(){}
void mglGraphAB::ToggleZoom(){}
void mglGraphAB::ToggleRotate(){}
void mglGraphAB::ToggleNo(){}
void mglGraphAB::ReLoad(bool){}
void mglGraphAB::Adjust(){}
void mglGraphAB::NextFrame(){}
void mglGraphAB::PrevFrame(){}
void mglGraphAB::Animation(){}
void mglGraphAB::Window(int , char **, int (*)(mglGraph *, void *), const char *, void *, void (*)(int , void *), bool ){}
//-----------------------------------------------------------------------------
int mgl_draw_class(mglGraph *gr, void *p)
{	return p ? ((mglDraw *)p)->Draw(gr) : 0;	}
void mgl_reload_class(int next, void *p)
{	if(p)	((mglDraw *)p)->Reload(next);	}
void mglGraphAB::Window(int argc, char **argv, const char *title, mglDraw *draw, bool maximize)
{
	Window(argc, argv, mgl_draw_class, title, draw, mgl_reload_class, maximize);
}
//-----------------------------------------------------------------------------

