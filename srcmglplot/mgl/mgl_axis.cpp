/***************************************************************************
 * mgl_axis.cpp is part of Math Graphic Library                            *
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
#include <ctype.h>
#include <wchar.h>
#include <stdarg.h>
#include <stdlib.h>

#include "mgl/mgl.h"
#include "mgl/mgl_eval.h"
#include "mgl/mgl_c.h"
#define FLT_EPS	(1.+1.2e-07)
//-----------------------------------------------------------------------------
wchar_t *mgl_wcsdup(const wchar_t *s)
{
	wchar_t *r = (wchar_t *)malloc((wcslen(s)+1)*sizeof(wchar_t));
	memcpy(r,s,(wcslen(s)+1)*sizeof(wchar_t));
	return r;
}
//-----------------------------------------------------------------------------
char *mgl_strdup(const char *s)
{
	char *r = (char *)malloc((strlen(s)+1)*sizeof(char));
	memcpy(r,s,(strlen(s)+1)*sizeof(char));
	return r;
}
//-----------------------------------------------------------------------------
void wcstrim_mgl(wchar_t *str)
{
	wchar_t *c = mgl_wcsdup(str);
	unsigned long n=wcslen(str);
	long k;
	for(k=0;k<long(wcslen(str));k++)	if(!isspace(str[k]))	break;
	wcscpy(c,&(str[k]));
	n = wcslen(c);
	for(k=n-1;k>=0;k--)	if(!isspace(c[k]))	break;
	c[k+1] = 0;
	wcscpy(str,c);	free(c);
}
//-----------------------------------------------------------------------------
///	Round the number to k digits. For example, x = 0.01234 -> r = 0.0123 for k=3.
mreal mgl_okrugl(mreal x,int k)
{
	if(x<=0)	return 0;
	mreal r;
	long n = long(log10(x));
	r = floor(x*pow(10.,k-n))/pow(10.,k-n);
	return r;
}
//-----------------------------------------------------------------------------
void mglGraph::DrawXGridLine(mreal t, mreal y0, mreal z0)
{
	register int i;
	mreal *pp = new mreal[3*GridPnts+3],dd;
	bool *tt = new bool[GridPnts+1];
	dd = (Max.z-Min.z - (TernAxis&2 ? t:0))/GridPnts;
	for(i=0;i<GridPnts+1;i++)
	{
		pp[3*i]=t;	pp[3*i+1]=y0;	pp[3*i+2]=Min.z+dd*i;
		tt[i] = ScalePoint(pp[3*i],pp[3*i+1],pp[3*i+2]);
	}
	curv_plot(GridPnts+1,pp,tt);
	dd = (Max.y-Min.y - (TernAxis ? t-Min.x:0))/GridPnts;
	for(i=0;i<GridPnts+1;i++)
	{
		pp[3*i]=t;	pp[3*i+2]=z0;	pp[3*i+1]=Min.y+dd*i;
		tt[i] = ScalePoint(pp[3*i],pp[3*i+1],pp[3*i+2]);
	}
	curv_plot(GridPnts+1,pp,tt);
	delete [] tt;	delete [] pp;
}
//-----------------------------------------------------------------------------
void mglGraph::DrawYGridLine(mreal t, mreal x0, mreal z0)
{
	register int i;
	mreal *pp = new mreal[3*GridPnts+3],dd;
	bool *tt = new bool[GridPnts+1];
	if(!(TernAxis&2))
	{
		dd = (Max.z-Min.z)/GridPnts;
		for(i=0;i<GridPnts+1;i++)
		{
			pp[3*i]=TernAxis ? Max.x-t+Min.y:x0;
			pp[3*i+1]=t;	pp[3*i+2]=Min.z+dd*i;
			tt[i] = ScalePoint(pp[3*i],pp[3*i+1],pp[3*i+2]);
		}
		curv_plot(GridPnts+1,pp,tt);
	}
	dd = (Max.x-Min.x - (TernAxis ? t-Min.y:0))/GridPnts;
	for(i=0;i<GridPnts+1;i++)
	{
		pp[3*i+2]=z0;	pp[3*i+1]=t;	pp[3*i]=Min.x+dd*i;
		tt[i] = ScalePoint(pp[3*i],pp[3*i+1],pp[3*i+2]);
	}
	curv_plot(GridPnts+1,pp,tt);
	delete [] tt;	delete [] pp;
}
//-----------------------------------------------------------------------------
void mglGraph::DrawZGridLine(mreal t, mreal x0, mreal y0)
{
	register int i;
	mreal *pp = new mreal[3*GridPnts+3], dd;
	bool *tt = new bool[GridPnts+1];
	dd = (Max.x-Min.x - (TernAxis&2 ? t:0))/GridPnts;
	for(i=0;i<GridPnts+1;i++)
	{
		pp[3*i+1]=y0;	pp[3*i+2]=t;	pp[3*i]=Min.x+dd*i;
		tt[i] = ScalePoint(pp[3*i],pp[3*i+1],pp[3*i+2]);
	}
	curv_plot(GridPnts+1,pp,tt);
	dd = (Max.y-Min.y - (TernAxis&2 ? t:0))/GridPnts;
	for(i=0;i<GridPnts+1;i++)
	{
		pp[3*i]=x0;		pp[3*i+2]=t;	pp[3*i+1]=Min.y+dd*i;
		tt[i] = ScalePoint(pp[3*i],pp[3*i+1],pp[3*i+2]);
	}
	curv_plot(GridPnts+1,pp,tt);
	delete [] tt;	delete [] pp;
}
//-----------------------------------------------------------------------------
void mglGraph::DrawTGridLine(mreal t, mreal z0)
{
	if(!TernAxis)	return;
	register int i;
	mreal *pp = new mreal[3*GridPnts+3],dd;
	bool *tt = new bool[GridPnts+1];
	for(i=0;i<GridPnts+1;i++)
	{
		pp[3*i]= Min.x+t*i/GridPnts;
		pp[3*i+1]=Min.y+t-t*i/GridPnts;
		pp[3*i+2]=z0;
		tt[i] = ScalePoint(pp[3*i],pp[3*i+1],pp[3*i+2]);
	}
	curv_plot(GridPnts+1,pp,tt);
	dd = (Max.z-Min.z - (TernAxis&2 ? t:0))/GridPnts;
	for(i=0;i<GridPnts+1;i++)
	{
		pp[3*i]=Min.x;	pp[3*i+1]=Min.y+t;	pp[3*i+2]=Min.z+dd*i;
		tt[i] = ScalePoint(pp[3*i],pp[3*i+1],pp[3*i+2]);
	}
	curv_plot(GridPnts+1,pp,tt);
	delete [] tt;	delete [] pp;
}
//-----------------------------------------------------------------------------
void mglGraph::Grid(const char *dir, const char *pen)
{
	static int cgid=1;	StartGroup("AxisGrid",cgid++);
	mreal x0,y0,z0,ddx,ddy,ddz,t;
	if(pen)	SelectPen(pen);
	else	SelectPen(TranspType!=2 ? "B;1" : "b;1");
	ddx = dx>=0 ? dx : mgl_okrugl(-fabs(Min.x-Max.x)/(dx+1),3);
	ddy = dy>=0 ? dy : mgl_okrugl(-fabs(Min.y-Max.y)/(dy+1),3);
	ddz = dz>=0 ? dz : mgl_okrugl(-fabs(Min.z-Max.z)/(dz+1),3);
	if(strchr(dir,'x'))
	{
		x0 = GetOrgX('x');	y0 = GetOrgY('x');	z0 = GetOrgZ('x');
		x0 = isnan(OrgT.x) ? x0 : OrgT.x;
		x0 = x0 - ddx*floor((x0-(Max.x>Min.x?Min.x:Max.x))/ddx);
		float tmp=(Min.x<Max.x?Max.x:Min.x);
		if(xnum)	for(int i=0;i<xnum;i++)
			DrawXGridLine(xval[i],y0,z0);
		else if(dx)
		{	if(x0+ddx!=x0 && tmp+ddx!=tmp)	for(t=x0;t<=tmp;t+=ddx)	DrawXGridLine(t,y0,z0);	}
		else if(Min.x>0)
		{
			x0 = exp(M_LN10*floor(0.1+log10(Min.x)));
			for(t=x0;t<=Max.x*FLT_EPS;t*=10)	DrawXGridLine(t,y0,z0);
		}
		else if(Max.x<0)
		{
			x0 = -exp(M_LN10*floor(0.1+log10(-Min.x)));
			for(t=x0;t>=Min.x*FLT_EPS;t*=10)	DrawXGridLine(t,y0,z0);
		}
	}
	if(strchr(dir,'y'))
	{
		x0 = GetOrgX('y');	y0 = GetOrgY('y');	z0 = GetOrgZ('y');
		y0 = isnan(OrgT.y) ? y0 : OrgT.y;
		y0 = y0 - ddy*floor((y0-(Max.y>Min.y?Min.y:Max.y))/ddy);
		float tmp=(Max.y>Min.y?Max.y:Min.y);
		if(ynum)	for(int i=0;i<ynum;i++)
			DrawYGridLine(yval[i],x0,z0);
		else if(dy)
		{	if(y0+ddy!=y0 && tmp+ddy!=tmp)	for(t=y0;t<=tmp;t+=ddy)	DrawYGridLine(t,x0,z0);	}
		else if(Min.y>0)
		{
			y0 = exp(M_LN10*floor(0.1+log10(Min.y)));
			for(t=y0;t<=Max.y*FLT_EPS;t*=10)	DrawYGridLine(t,x0,z0);
		}
		else if(Max.y<0)
		{
			y0 = -exp(M_LN10*floor(0.1+log10(-Max.y)));
			for(t=y0;t>=Min.y*FLT_EPS;t*=10)	DrawYGridLine(t,x0,z0);
		}
	}
	if(strchr(dir,'z'))
	{
		x0 = GetOrgX('z');	y0 = GetOrgY('z');	z0 = GetOrgZ('z');
		z0 = isnan(OrgT.z) ? z0 : OrgT.z;
		z0 = z0 - ddz*floor((z0-(Max.z>Min.z?Min.z:Max.z))/ddz);
		float tmp=(Max.z>Min.z?Max.z:Min.z);
		if(znum)	for(int i=0;i<znum;i++)
			DrawZGridLine(zval[i],x0,y0);
		else if(dz)
		{	if(z0+ddz!=z0 && tmp+ddz!=tmp)	for(t=z0;t<=tmp;t+=ddz)	DrawZGridLine(t,x0,y0);	}
		else if(Min.z>0)
		{
			z0 = exp(M_LN10*floor(0.1+log10(Min.z)));
			for(t=z0;t<=Max.z*FLT_EPS;t*=10)	DrawZGridLine(t,x0,y0);
		}
		else if(Max.z<0)
		{
			z0 = -exp(M_LN10*floor(0.1+log10(-Max.z)));
			for(t=z0;t>=Min.z*FLT_EPS;t*=10)	DrawZGridLine(t,x0,y0);
		}
	}
	if(strchr(dir,'t') && TernAxis)
	{
		z0 = isnan(OrgT.z) ? GetOrgZ('z') : OrgT.z;
		z0 = z0 - ddz*floor((z0-Min.z)/ddz);
		if(xnum)	for(int i=0;i<xnum;i++)		DrawTGridLine(xval[i],z0);
		else if(dx && Max.x+ddx!=Max.x)	for(t=Min.x;t<=Max.x;t+=ddx)	DrawTGridLine(t-Min.x,z0);
		else if(dy && Max.y+ddy!=Max.y)	for(t=Min.y;t<=Max.y;t+=ddy)	DrawTGridLine(t-Min.y,z0);
	}
	SelectPen("k-1");
	EndGroup();
}
//-----------------------------------------------------------------------------
void mglGraph::Labelw(char dir,const wchar_t *text,mreal pos,mreal size,mreal shift)
{
	mreal t, x0, y0, z0;
	SelectPen("k-1");
	char font[33]="C";
	if(pos<-0.2)	font[0]='L';	if(pos>0.2)	font[0]='R';
	strcat(font,FontDef);
	x0 = GetOrgX(dir);	y0 = GetOrgY(dir);	z0 = GetOrgZ(dir);
	if(size<=0)	size = -size*FontSize;
	if(dir=='x')
	{
		if(dx)	t = (Min.x+Max.x+pos*(Max.x-Min.x))/2;
		else	t = Min.x*pow(Max.x/Min.x, (pos+1)/2);
		Putsw(mglPoint(t,y0,z0),text,font,size,'X',shift);
	}
	if(dir=='y')
	{
		if(TernAxis)
		{
			t = (Min.y+Max.y+pos*(Max.y-Min.y))/2;
			Putsw(mglPoint(Max.x+Min.y-t,t,z0),text,font,size,'T',shift);
		}
		else
		{
			if(dy)	t = (Min.y+Max.y+pos*(Max.y-Min.y))/2;
			else	t = Min.y*pow(Max.y/Min.y, (pos+1)/2);
			Putsw(mglPoint(x0,t,z0),text,font,size,'Y',shift);
		}
	}
	if(dir=='z')
	{
		if(dz)	t = (Min.z+Max.z+pos*(Max.z-Min.z))/2;
		else	t = Min.z*pow(Max.z/Min.z, (pos+1)/2);
		Putsw(mglPoint(x0,y0,t),text,font,size,'Z',shift);
	}
	if(dir=='t' && TernAxis)
	{
		t = (Min.y+Max.y-pos*(Max.y-Min.y))/2;
		Putsw(mglPoint(Min.x,t,Min.z),text,font,size,'Y',shift);
	}
}
//-----------------------------------------------------------------------------
int _mgl_tick_ext(mreal a, mreal b, wchar_t s[32], mreal &v)
{
	int kind = 0;
	if(fabs(a-b)<0.01*fabs(a))
	{
		kind = 1;
		v = fabs(a-b);
		if(v>10000.f)
		{
			int k=int(log10(v)-0.01);
			kind=3;		v=mgl_ipow(10,k);
			mglprintf(s, 32, L"(@{\\times{}10^{%d}})", k);
		}
		if(v<1e-4f)
		{
			int k=int(log10(v)-0.01)-1;
			kind=3;		v=mgl_ipow(10,k);
			mglprintf(s, 32, L"(@{\\times{}10^{%d}})", k);
		}
	}
	else
	{
		v = fabs(b)>fabs(a)?fabs(b):fabs(a);
		if(v>10000.f)
		{
			kind = 2;
			int k=int(log10(v)-0.01);
			v=mgl_ipow(10,k);
			mglprintf(s, 32, L"\\times 10^{%d}", k);
		}
		if(v<1e-4f)
		{
			kind = 2;
			int k=int(log10(v)-0.01)-1;
			v=mgl_ipow(10,k);
			mglprintf(s, 32, L"\\times 10^{%d}", k);
		}
	}
	return kind;
}
//-----------------------------------------------------------------------------
void _mgl_tick_text(mreal z, mreal z0, mreal d, mreal v, int kind, wchar_t str[64])
{
	mreal u = fabs(z)<d ? 0:z;
	if((kind&1) && z>z0)	u = fabs(z-z0)<d ? 0:(z-z0);
	if(kind==2 || (kind==3 && z>z0))	u /= v;
	if((kind&1) && z>z0)
	{
		int n1,n2;
		mglprintf(str, 64, fabs(u)<1 ? L"@{(+%.2g)}" : L"@{(+%.3g)}",u);
		n1=wcslen(str);	mglprintf(str, 64, L"@{(+%g)}",u);	n2=wcslen(str);
		if(n1<n2)	mglprintf(str, 64, fabs(u)<1 ? L"@{(+%.2g)}" : L"@{(+%.3g)}",u);
	}
	else
	{
		int n1,n2;
		mglprintf(str, 64, fabs(u)<1 ? L"%.2g" :  L"%.3g",u);
		n1=wcslen(str);	mglprintf(str, 64, L"%g",u);	n2=wcslen(str);
		if(n1<n2)	mglprintf(str, 64, fabs(u)<1 ? L"%.2g" :  L"%.3g",u);
	}
}
//-----------------------------------------------------------------------------
void mglGraph::DrawTick(mreal *pp,bool /*sub*/)
{
	ScalePoint(pp[0],pp[1],pp[2]);
	ScalePoint(pp[3],pp[4],pp[5]);
	ScalePoint(pp[6],pp[7],pp[8]);
	curv_plot(3,pp,0);
}
//-----------------------------------------------------------------------------
void mglGraph::DrawXTick(mreal x, mreal y0, mreal z0, mreal dy, mreal dz, int f)
{
	mreal pp[9],ff=TickLen/sqrt(1.+f*st_t);
	pp[0]=x;	pp[1]=y0+dy*ff;	pp[2]=z0;
	pp[3]=x;	pp[4]=y0;	pp[5]=z0;
	pp[6]=x;	pp[7]=y0;	pp[8]=z0+dz*ff;
	if(*TickStl && !f)	SelectPen(TickStl);
	if(*SubTStl && f)	SelectPen(SubTStl);
	DrawTick(pp,f!=0);
}
void mglGraph::AxisX(bool text, const char *stl)
{
	mreal pp[3*32];
	wchar_t str[64];
	long i;
	mreal x0,y0,z0,x,ddx,ddy,ddz,v=0;
	x0 = GetOrgX('x');	y0 = GetOrgY('x');	z0 = GetOrgZ('x');
	ddx = dx>=0 ? dx : mgl_okrugl(-fabs(Min.x-Max.x)/(dx+1),3);
	ddy = dy>=0 ? dy : mgl_okrugl(-fabs(Min.y-Max.y)/(dy+1),3);
	ddz = dz>=0 ? dz : mgl_okrugl(-fabs(Min.z-Max.z)/(dz+1),3);
	if(ddy==0)	ddy=y0*10>fabs(Max.y-Min.y) ? 0.3*fabs(Max.y-Min.y) : y0*3;
	if(ddz==0)	ddz=z0*10>fabs(Max.z-Min.z) ? 0.3*fabs(Max.z-Min.z) : z0*3;
	if(y0>(Max.y+Min.y)/2)	ddy = -ddy;
	if(z0>(Max.z+Min.z)/2)	ddz = -ddz;
	SelectPen(stl);
	for(i=0;i<31;i++)	// draw axis line
	{
		pp[3*i+1]=y0;	pp[3*i+2]=z0;
		pp[3*i]=Min.x+(Max.x-Min.x)*i/30.;
		ScalePoint(pp[3*i],pp[3*i+1],pp[3*i+2]);
	}
	if(stl[3])
	{
		pp[3*31+1]=y0;	pp[3*31+2]=z0;
		pp[3*31]=Min.x+(Max.x-Min.x)*1.05;
		ScalePoint(pp[3*31],pp[3*31+1],pp[3*31+2]);
		curv_plot(32,pp,0);
	}
	else	curv_plot(31,pp,0);

	if(xnum)	for(int i=0;i<xnum;i++)
	{
		DrawXTick(xval[i],y0,z0,ddy,ddz);
		if(text)	Putsw(mglPoint(xval[i],y0,z0),xstr[i],FontDef,-1,'x');
	}
	else if(ddx)	// ticks drawing
	{
		int kind=0;
		wchar_t s[32]=L"";
		if(xtt[0]==0) kind = _mgl_tick_ext(Max.x, Min.x, s, v);
		if(!TuneTicks)	kind = 0;

		NSx = NSx<0 ? 0 : NSx;
		x0 = isnan(OrgT.x) ? x0 : OrgT.x;
		double x1 = Max.x+(Max.x-Min.x)*1e-6;
		if(Max.x>Min.x)
			x0 = x0 - ddx*floor((x0-Min.x)/ddx+1e-3);
		else
		{
			x1 = Min.x-(Max.x-Min.x)*1e-6;
			x0 = x0 - ddx*floor((x0-Max.x)/ddx+1e-3);
		}
		if(x0+ddx!=x0 && x1+ddx!=x1)	for(x=x0;x<=x1;x+=ddx)
		{
			DrawXTick(x,y0,z0,ddy,ddz);
			if(text)
			{
				if(xtt[0])	mglprintf(str, 64, xtt, fabs(x)<ddx/100 ? 0 : x);
				else	_mgl_tick_text(x,x0,ddx/100,v,kind,str);
				wcstrim_mgl(str);
				Putsw(mglPoint(x,y0,z0),str,FontDef,-1,'x');
			}
		}
		if(text && (kind&2))
			Putsw(mglPoint(FactorPos*(Max.x-Min.x)+Min.x,y0,z0), s, FontDef, -1, 'x');
		if(NSx>0)
		{
			ddx /= (NSx+1);	// subticks drawing
			x0 = GetOrgX('x');	x0 = isnan(OrgT.x) ? x0 : OrgT.x;
			if(Max.x>Min.x)
			{	x1=Max.x;	x0 = x0 - ddx*floor((x0-Min.x)/ddx+1e-3);	}
			else
			{	x1=Min.x;	x0 = x0 - ddx*floor((x0-Max.x)/ddx+1e-3);	}
			if(x0+ddx!=x0 && x1+ddx!=x1)	for(x=x0;x<=x1;x+=ddx)	DrawXTick(x,y0,z0,ddy,ddz,1);
		}
	}
	else if(Min.x>0)
	{
		int k=1+int(log10(Max.x/Min.x)/5), cur=0;
		x0 = exp(M_LN10*floor(0.1+log10(Min.x)));
		for(x=x0;x<=Max.x*FLT_EPS;x*=10.f)
		{
			if(x*FLT_EPS>=Min.x && x<=Max.x*FLT_EPS)
			{
				DrawXTick(x,y0,z0,ddy,ddz);
				mglprintf(str,64,L"10^{%d}",int(floor(0.1+log10(x))));
				if(text && cur%k==0)	Putsw(mglPoint(x,y0,z0),str,FontDef,-1,'x');
				cur++;
			}
			// subticks drawing
			for(v=x;v<10*x;v+=x)	if(v>Min.x && v<Max.x)	DrawXTick(v,y0,z0,ddy,ddz,1);
		}
	}
	else if(Max.x<0)
	{
		int k=1+int(log10(Min.x/Max.x)/5), cur=0;
		x0 = -exp(M_LN10*floor(0.1+log10(-Max.x)));
		for(x=x0;x>=Min.x*FLT_EPS;x*=10)
		{
			if(x*FLT_EPS<=Max.x && x>=Min.x*FLT_EPS)
			{
				DrawXTick(x,y0,z0,ddy,ddz);
				mglprintf(str,64,L"-10^{%d}",int(floor(0.1+log10(-x))));
				if(text && cur%k==0)	Putsw(mglPoint(x,y0,z0),str,FontDef,-1,'x');
				cur++;
			}
			// subticks drawing
			for(v=x;v>10*x;v+=x)	if(v<Max.x && v>Min.x)	DrawXTick(v,y0,z0,ddy,ddz,1);
		}
	}
}
//-----------------------------------------------------------------------------
void mglGraph::DrawYTick(mreal y, mreal x0, mreal z0, mreal dx, mreal dz, int f)
{
	mreal pp[9],ff=TickLen/sqrt(1.+f*st_t);
	if(TernAxis)
	{	pp[0]=x0+dx*ff;	pp[1]=y-dx*ff;	pp[2]=z0;	}
	else
	{	pp[0]=x0+dx*ff;	pp[1]=y;	pp[2]=z0;	}
	pp[3]=x0;		pp[4]=y;	pp[5]=z0;
	pp[6]=x0;		pp[7]=y;	pp[8]=z0+dz*ff;
	if(*TickStl && !f)	SelectPen(TickStl);
	if(*SubTStl && f)	SelectPen(SubTStl);
	DrawTick(pp,f!=0);
}
void mglGraph::AxisY(bool text, const char *stl)
{
	mreal pp[3*32];
	wchar_t str[64];
	long i;
	mreal x0,y0,z0,y,ddx,ddy,ddz,v=0;
	x0 = GetOrgX('y');	y0 = GetOrgY('y');	z0 = GetOrgZ('y');
	ddx = dx>=0 ? dx : mgl_okrugl(-fabs(Min.x-Max.x)/(dx+1),3);
	ddy = dy>=0 ? dy : mgl_okrugl(-fabs(Min.y-Max.y)/(dy+1),3);
	ddz = dz>=0 ? dz : mgl_okrugl(-fabs(Min.z-Max.z)/(dz+1),3);
	if(ddx==0)	ddx=x0*10>fabs(Max.x-Min.x) ? 0.3*fabs(Max.x-Min.x) : x0*3;
	if(ddz==0)	ddz=z0*10>fabs(Max.z-Min.z) ? 0.3*fabs(Max.z-Min.z) : z0*3;
	if(x0>(Max.x+Min.x)/2)	ddx = -ddx;
	if(z0>(Max.z+Min.z)/2)	ddz = -ddz;
	SelectPen(stl);
	for(i=0;i<31;i++)	// сама ось
	{
		pp[3*i]=x0;	pp[3*i+2]=z0;
		pp[3*i+1]=Min.y+(Max.y-Min.y)*i/30.;
		ScalePoint(pp[3*i],pp[3*i+1],pp[3*i+2]);
	}
	if(stl[3])
	{
		pp[3*31]=x0;	pp[3*31+2]=z0;
		pp[3*31+1]=Min.y+(Max.y-Min.y)*1.05;
		ScalePoint(pp[3*31],pp[3*31+1],pp[3*31+2]);
		curv_plot(32,pp,0);
	}
	else	curv_plot(31,pp,0);

	if(ynum)	for(int i=0;i<ynum;i++)
	{
		DrawYTick(yval[i],x0,z0,ddx,ddz);
		if(text)	Putsw(mglPoint(x0,yval[i],z0),ystr[i],FontDef,-1,'y');
	}
	else if(ddy)	// ticks drawing
	{
		int kind=0;
		wchar_t s[32]=L"";
		if(ytt[0]==0) kind = _mgl_tick_ext(Max.y, Min.y, s, v);
		if(!TuneTicks)	kind = 0;

		NSy = NSy<0 ? 0 : NSy;
		y0 = isnan(OrgT.y) ? y0 : OrgT.y;
		double y1 = Max.y+(Max.y-Min.y)*1e-6;
		if(Max.y>Min.y)
			y0 = y0 - ddy*floor((y0-Min.y)/ddy+1e-3);
		else
		{
			y1 = Min.y-(Max.y-Min.y)*1e-6;
			y0 = y0 - ddy*floor((y0-Max.y)/ddy+1e-3);
		}
		if(y0+ddy!=y0 && y1+ddy!=y1)	for(y=y0;y<=y1;y+=ddy)
		{
			DrawYTick(y,x0,z0,ddx,ddz);
			if(text)
			{
				if(ytt[0])	mglprintf(str, 64, ytt, fabs(y)<ddy/100 ? 0 : y);
				else	_mgl_tick_text(y,y0,ddy/100,v,kind,str);
				wcstrim_mgl(str);
				Putsw(mglPoint(x0,y,z0),str,FontDef,-1,'y');
			}
		}
		if(text && (kind&2))
			Putsw(mglPoint(x0,FactorPos*(Max.y-Min.y)+Min.y,z0), s, FontDef, -1, 'y');
		if(NSy>0)
		{
			ddy /= (NSy+1);	// subticks drawing
			y0 = GetOrgY('y');	y0 = isnan(OrgT.y) ? y0 : OrgT.y;
			if(Max.y>Min.y)
			{	y1=Max.y;	y0 = y0 - ddy*floor((y0-Min.y)/ddy+1e-3);	}
			else
			{	y1=Min.y;	y0 = y0 - ddy*floor((y0-Max.y)/ddy+1e-3);	}
			if(y0+ddy!=y0 && y1+ddy!=y1)	for(y=y0;y<=y1;y+=ddy)	DrawYTick(y,x0,z0,ddx,ddz,1);
		}
	}
	else if(Min.y>0)
	{
		int k=1+int(log10(Max.y/Min.y)/5), cur=0;
		y0 = exp(M_LN10*floor(0.1+log10(Min.y)));
		for(y=y0;y<=Max.y*FLT_EPS;y*=10)
		{
			if(y*FLT_EPS>=Min.y && y<=Max.y*FLT_EPS)
			{
				DrawYTick(y,x0,z0,ddx,ddz);
				mglprintf(str,64,L"10^{%d}",int(floor(0.1+log10(y))));
				if(text && cur%k==0)	Putsw(mglPoint(x0,y,z0),str,FontDef,-1,'y');
				cur++;
			}
			// subticks drawing
			for(v=y;v<10*y;v+=y)	if(v>Min.y && v<Max.y)	DrawYTick(v,x0,z0,ddx,ddz,1);
		}
	}
	else if(Max.y<0)
	{
		int k=1+int(log10(Min.y/Max.y)/5), cur=0;
		y0 = -exp(M_LN10*floor(0.1+log10(-Max.y)));
		for(y=y0;y>=Min.y*FLT_EPS;y*=10)
		{
			if(y*FLT_EPS<=Max.y && y>=Min.y*FLT_EPS)
			{
				DrawYTick(y,x0,z0,ddx,ddz);
				mglprintf(str,64,L"-10^{%d}",int(floor(0.1+log10(-y))));
				if(text && cur%k==0)	Putsw(mglPoint(x0,y,z0),str,FontDef,-1,'y');
				cur++;
			}
			// subticks drawing
			for(v=y;v>10*y;v+=y)	if(v<Max.y && v>Min.y)	DrawYTick(v,x0,z0,ddx,ddz,1);
		}
	}
}
//-----------------------------------------------------------------------------
void mglGraph::DrawZTick(mreal z, mreal x0, mreal y0, mreal dx, mreal dy, int f)
{
	mreal pp[9],ff=TickLen/sqrt(1.+f*st_t);
	pp[0]=x0;	pp[1]=y0+dy*ff;	pp[2]=z;
	pp[3]=x0;	pp[4]=y0;		pp[5]=z;
	pp[6]=x0+dx*ff;	pp[7]=y0;	pp[8]=z;
	if(*TickStl && !f)	SelectPen(TickStl);
	if(*SubTStl && f)	SelectPen(SubTStl);
	DrawTick(pp,f!=0);
}
void mglGraph::AxisZ(bool text, const char *stl)
{
	mreal pp[3*32];
	wchar_t str[64];
	long i;
	mreal x0,y0,z0,z,ddx,ddy,ddz,v=0;
	x0 = GetOrgX('z');	y0 = GetOrgY('z');	z0 = GetOrgZ('z');
	ddx = dx>=0 ? dx : mgl_okrugl(-fabs(Min.x-Max.x)/(dx+1),3);
	ddy = dy>=0 ? dy : mgl_okrugl(-fabs(Min.y-Max.y)/(dy+1),3);
	ddz = dz>=0 ? dz : mgl_okrugl(-fabs(Min.z-Max.z)/(dz+1),3);
	if(ddx==0)	ddx=x0*10>fabs(Max.x-Min.x) ? 0.3*fabs(Max.x-Min.x) : x0*3;
	if(ddy==0)	ddy=y0*10>fabs(Max.y-Min.y) ? 0.3*fabs(Max.y-Min.y) : y0*3;
	if(x0>(Max.x+Min.x)/2)	ddx = -ddx;
	if(y0>(Max.y+Min.y)/2)	ddy = -ddy;
	SelectPen(stl);
	for(i=0;i<31;i++)	// сама ось
	{
		pp[3*i]=x0;	pp[3*i+1]=y0;
		pp[3*i+2]=Min.z+(Max.z-Min.z)*i/30.;
		ScalePoint(pp[3*i],pp[3*i+1],pp[3*i+2]);
	}
	if(stl[3])
	{
		pp[3*31]=x0;	pp[3*31+1]=y0;
		pp[3*31+2]=Min.z+(Max.z-Min.z)*1.05;
		ScalePoint(pp[3*31],pp[3*31+1],pp[3*31+2]);
		curv_plot(32,pp,0);
	}
	else	curv_plot(31,pp,0);

	if(znum)	for(int i=0;i<znum;i++)
	{
		DrawZTick(zval[i],x0,y0,ddx,ddy);
		if(text)	Putsw(mglPoint(x0,y0,zval[i]),zstr[i],FontDef,-1,'z');
	}
	else if(ddz)	// ticks drawing
	{
		int kind=0;
		wchar_t s[32]=L"";
		if(ztt[0]==0) kind = _mgl_tick_ext(Max.z, Min.z, s, v);
		if(!TuneTicks)	kind = 0;

		NSz = NSz<0 ? 0 : NSz;
		z0 = isnan(OrgT.z) ? z0 : OrgT.z;
		double z1 = Max.z+(Max.z-Min.z)*1e-6;
		if(Max.z>Min.z)		z0 = z0 - ddz*floor((z0-Min.z)/ddz+1e-3);
		else
		{
			z1 = Min.z-(Max.z-Min.z)*1e-6;
			z0 = z0 - ddz*floor((z0-Max.z)/ddz+1e-3);
		}
		if(z0+ddz!=z0 && z1+ddz!=z1)	for(z=z0;z<=z1;z+=ddz)
		{
			DrawZTick(z,x0,y0,ddx,ddy);
			if(text)
			{
				if(ztt[0])	mglprintf(str,64,ztt,fabs(z)<ddz/100 ? 0 : z);
				else	_mgl_tick_text(z,z0,ddz/100,v,kind,str);
				wcstrim_mgl(str);
				Putsw(mglPoint(x0,y0,z),str,FontDef,-1,'z');
			}
		}
		if(text && (kind&2))
			Putsw(mglPoint(x0,y0,FactorPos*(Max.z-Min.z)+Min.z), s, FontDef, -1, 'z');
		if(NSz>0)
		{
			ddz /= (NSz+1);	// subticks drawing
			z0 = GetOrgZ('z');	z0 = isnan(OrgT.z) ? z0 : OrgT.z;
			if(Max.z>Min.z)
			{	z1=Max.z;	z0 = z0 - ddz*floor((z0-Min.z)/ddz+1e-3);	}
			else
			{	z1=Min.z;	z0 = z0 - ddz*floor((z0-Max.z)/ddz+1e-3);	}
			if(z0+ddz!=z0 && z1+ddz!=z1)	for(z=z0;z<=z1;z+=ddz)	DrawZTick(z,x0,y0,ddx,ddy,1);
		}
	}
	else if(Min.z>0)
	{
		int k=1+int(log10(Max.z/Min.z)/5), cur=0;
		z0 = exp(M_LN10*floor(0.1+log10(Min.z)));
		for(z=z0;z<=Max.z*FLT_EPS;z*=10)
		{
			if(z*FLT_EPS>=Min.z && z<=Max.z*FLT_EPS)
			{
				DrawZTick(z,x0,y0,ddx,ddy);
				mglprintf(str,64,L"10^{%d}",int(floor(0.1+log10(z))));
				if(text && cur%k==0)	Putsw(mglPoint(x0,y0,z),str,FontDef,-1,'z');
				cur++;
			}
			// subticks drawing
			for(v=z;v<10*z;v+=z)	if(v>Min.z && v<Max.z)	DrawZTick(v,x0,y0,ddx,ddy,1);
		}
	}
	else if(Max.z<0)
	{
		int k=1+int(log10(Min.z/Max.z)/5), cur=0;
		z0 = -exp(M_LN10*floor(0.1+log10(-Max.z)));
		for(z=z0;z>=Min.z*FLT_EPS;z*=10)
		{
			if(z*FLT_EPS<=Max.z && z>=Min.z*FLT_EPS)
			{
				DrawZTick(z,x0,y0,ddx,ddy);
				mglprintf(str,64,L"-10^{%d}",int(floor(0.1+log10(-z))));
				if(text && cur%k==0)	Putsw(mglPoint(x0,y0,z),str,FontDef,-1,'z');
				cur++;
			}
			// subticks drawing
			for(v=z;v>10*z;v+=z)	if(v<Max.z && v>Min.z)	DrawZTick(v,x0,y0,ddx,ddy,1);
		}
	}
}
//-----------------------------------------------------------------------------
void mglGraph::Axis(const char *dir, bool adjust)
{
	if(adjust)	AdjustTicks(dir);
	static int cgid=1;	StartGroup("Axis",cgid++);
	bool text = strchr(dir,'_')==NULL;
	const char *arr = "AKDTVISO";
	char stl[5]={TranspType!=2?'k':'w', '-', '1', 0, 0};
	for(unsigned i=0;i<strlen(arr);i++)
		if(strchr(dir,arr[i]))	{	stl[3]=arr[i];	break;	}

	if(TernAxis)
	{
		_sx = _sy = _st = 1;
		if(fabs(Max.x-Min.x) != fabs(Max.y-Min.y))
			SetWarn(mglWarnTern,"Axis");
		AxisX(text, stl);
		AxisT(text, stl);
	}
	else
	{
		if(strchr(dir,'x'))	{	_sx = 1;	AxisX(text, stl);	}
		if(strchr(dir,'X'))	{	_sx = -1;	AxisX(text, stl);	}
		if(strchr(dir,'y'))	{	_sy = 1;	AxisY(text, stl);	}
		if(strchr(dir,'Y'))	{	_sy = -1;	AxisY(text, stl);	}
	}
	if(strchr(dir,'z'))	{	_sz = 1;	AxisZ(text, stl);	}
	if(strchr(dir,'Z'))	{	_sz = -1;	AxisZ(text, stl);	}
	EndGroup();
}
//-----------------------------------------------------------------------------
void mglGraph::TickBox()
{
	mreal x0,y0,z0,x,y,z,ddx,ddy,ddz;
	bool d_x,d_y,d_z;
	x0 = GetOrgX(0);	y0 = GetOrgY(0);	z0 = GetOrgZ(0);
	ddx = dx>=0 ? dx : mgl_okrugl(-fabs(Min.x-Max.x)/(dx+1),3);
	ddy = dy>=0 ? dy : mgl_okrugl(-fabs(Min.y-Max.y)/(dy+1),3);
	ddz = dz>=0 ? dz : mgl_okrugl(-fabs(Min.z-Max.z)/(dz+1),3);
	d_x=(ddx!=0);	d_y=(ddy!=0);	d_z=(ddz!=0);
	if(!d_x)	ddx=x0*10>fabs(Max.x-Min.x) ? 0.3*fabs(Max.x-Min.x) : x0*3;
	if(!d_y)	ddy=y0*10>fabs(Max.y-Min.y) ? 0.3*fabs(Max.y-Min.y) : y0*3;
	if(!d_z)	ddz=z0*10>fabs(Max.z-Min.z) ? 0.3*fabs(Max.z-Min.z) : z0*3;

	if(xnum)	for(int i=0;i<xnum;i++)
	{
		x = xval[i];
		DrawXTick(x,Min.y,Min.z,ddy,ddz);
		DrawXTick(x,Min.y,Max.z,ddy,-ddz);
		DrawXTick(x,Max.y,Min.z,-ddy,ddz);
		DrawXTick(x,Max.y,Max.z,-ddy,-ddz);
	}
	else if(d_x)	// рисуем метки по х
	{
		x0 = isnan(OrgT.x) ? x0 : OrgT.x;
		x0 = x0 - ddx*floor((x0-(Max.x>Min.x?Min.x:Max.x))/ddx);
		float tmp=(Max.x>Min.x?Max.x:Min.x);
		if(x0+ddx!=x0 && tmp+ddx!=tmp)	for(x=x0;x<tmp;x+=ddx)
		{
			DrawXTick(x,Min.y,Min.z,ddy,ddz);
			DrawXTick(x,Min.y,Max.z,ddy,-ddz);
			DrawXTick(x,Max.y,Min.z,-ddy,ddz);
			DrawXTick(x,Max.y,Max.z,-ddy,-ddz);
		}
	}
	else if(Min.x>0)
	{
		x0 = exp(M_LN10*floor(0.1+log10(Min.x)));
		for(x=x0;x<Max.x;x*=10)
		{
			if(x<Min.x)	continue;
			DrawXTick(x,Min.y,Min.z,ddy,ddz);
			DrawXTick(x,Min.y,Max.z,ddy,-ddz);
			DrawXTick(x,Max.y,Min.z,-ddy,ddz);
			DrawXTick(x,Max.y,Max.z,-ddy,-ddz);
		}
	}
	else if(Max.x<0)
	{
		x0 = -exp(M_LN10*floor(0.1+log10(-Max.x)));
		for(x=x0;x>Min.x;x*=10)
		{
			if(x>Max.x)	continue;
			DrawXTick(x,Min.y,Min.z,ddy,ddz);
			DrawXTick(x,Min.y,Max.z,ddy,-ddz);
			DrawXTick(x,Max.y,Min.z,-ddy,ddz);
			DrawXTick(x,Max.y,Max.z,-ddy,-ddz);
		}
	}
	x0 = GetOrgX(0);
	if(ynum)	for(int i=0;i<ynum;i++)
	{
		y = yval[i];
		DrawYTick(y,Min.x,Min.z,ddx,ddz);
		DrawYTick(y,Min.x,Max.z,ddx,-ddz);
		DrawYTick(y,Max.x,Min.z,-ddx,ddz);
		DrawYTick(y,Max.x,Max.z,-ddx,-ddz);
	}
	else if(d_y)	// рисуем метки по y
	{
		y0 = isnan(OrgT.y) ? y0 : OrgT.y;
		y0 = y0 - ddy*floor((y0-(Max.y>Min.y?Min.y:Max.y))/ddy);
		float tmp=(Max.y>Min.y?Max.y:Min.y);
		if(y0+ddy!=y0 && tmp+ddy!=tmp)	for(y=y0;y<tmp;y+=ddy)
		{
			DrawYTick(y,Min.x,Min.z,ddx,ddz);
			DrawYTick(y,Min.x,Max.z,ddx,-ddz);
			DrawYTick(y,Max.x,Min.z,-ddx,ddz);
			DrawYTick(y,Max.x,Max.z,-ddx,-ddz);
		}
	}
	else if(Min.y>0)
	{
		y0 = exp(M_LN10*floor(0.1+log10(Min.y)));
		for(y=y0;y<Max.y;y*=10)
		{
			if(y<Min.y)	continue;
			DrawYTick(y,Min.x,Min.z,ddx,ddz);
			DrawYTick(y,Min.x,Max.z,ddx,-ddz);
			DrawYTick(y,Max.x,Min.z,-ddx,ddz);
			DrawYTick(y,Max.x,Max.z,-ddx,-ddz);
		}
	}
	else if(Max.y<0)
	{
		y0 = -exp(M_LN10*floor(0.1+log10(-Max.y)));
		for(y=y0;y>Min.y;y*=10)
		{
			if(y>Max.y)	continue;
			DrawYTick(y,Min.x,Min.z,ddx,ddz);
			DrawYTick(y,Min.x,Max.z,ddx,-ddz);
			DrawYTick(y,Max.x,Min.z,-ddx,ddz);
			DrawYTick(y,Max.x,Max.z,-ddx,-ddz);
		}
	}
	y0 = GetOrgY(0);
	if(znum)	for(int i=0;i<znum;i++)
	{
		z = zval[i];
		DrawZTick(z,Min.x,Min.y,ddx,ddy);
		DrawZTick(z,Min.x,Max.y,ddx,-ddy);
		DrawZTick(z,Max.x,Min.y,-ddx,ddy);
		DrawZTick(z,Max.x,Max.y,-ddx,-ddy);
	}
	else if(d_z)	// рисуем метки
	{
		z0 = isnan(OrgT.z) ? z0 : OrgT.z;
		z0 = z0 - ddz*floor((z0-(Max.z>Min.z?Min.z:Max.z))/ddz);
		float tmp=(Max.z>Min.z?Max.z:Min.z);
		if(z0+ddz!=z0 && tmp+ddz!=tmp)	for(z=z0;z<tmp;z+=ddz)
		{
			DrawZTick(z,Min.x,Min.y,ddx,ddy);
			DrawZTick(z,Min.x,Max.y,ddx,-ddy);
			DrawZTick(z,Max.x,Min.y,-ddx,ddy);
			DrawZTick(z,Max.x,Max.y,-ddx,-ddy);
		}
	}
	else if(Min.z>0)
	{
		z0 = exp(M_LN10*floor(0.1+log10(Min.z)));
		for(z=z0;z<Max.z;z*=10)
		{
			if(z<Min.z)	continue;
			DrawZTick(z,Min.x,Min.y,ddx,ddy);
			DrawZTick(z,Min.x,Max.y,ddx,-ddy);
			DrawZTick(z,Max.x,Min.y,-ddx,ddy);
			DrawZTick(z,Max.x,Max.y,-ddx,-ddy);
		}
	}
	else if(Max.z<0)
	{
		z0 = -exp(M_LN10*floor(0.1+log10(-Max.z)));
		for(z=z0;z>Min.z;z*=10)
		{
			if(z>Max.z)	continue;
			DrawZTick(z,Min.x,Min.y,ddx,ddy);
			DrawZTick(z,Min.x,Max.y,ddx,-ddy);
			DrawZTick(z,Max.x,Min.y,-ddx,ddy);
			DrawZTick(z,Max.x,Max.y,-ddx,-ddy);
		}
	}
}
//-----------------------------------------------------------------------------
void mglGraph::Box(const char *col,bool ticks)
{
	mglColor c(NC);
	if(col && col[0])	c.Set(col[0]);
	Box(c,ticks);
}
//-----------------------------------------------------------------------------
void mglGraph::Box(mglColor p,bool ticks)
{
	static int cgid=1;	StartGroup("Box",cgid++);
	mreal x1=Min.x,x2=Max.x,y1=Min.y,y2=Max.y,z1=Min.z,z2=Max.z;
	Arrow1 = Arrow2 = '_';
	if(p.Valid())	Pen(p,'-',BaseLineWidth);
	else			Pen(TranspType!=2 ? BC:WC,'-',BaseLineWidth);
	mglFormula *tt = fc;	fc = 0;
	if((TernAxis&3)==1)			// usual ternary axis
	{
		Line(mglPoint(x1,y1,z1), mglPoint(x1,y1,z2), 0, 100);
		Line(mglPoint(x1,y2,z1), mglPoint(x1,y2,z2), 0, 100);
		Line(mglPoint(x2,y1,z1), mglPoint(x2,y1,z2), 0, 100);
		
		Line(mglPoint(x1,y1,z1), mglPoint(x1,y2,z1), 0, 100);
		Line(mglPoint(x1,y1,z1), mglPoint(x2,y1,z1), 0, 100);
		Line(mglPoint(x1,y2,z1), mglPoint(x2,y1,z1), 0, 100);
		
		Line(mglPoint(x1,y1,z2), mglPoint(x1,y2,z2), 0, 100);
		Line(mglPoint(x1,y1,z2), mglPoint(x2,y1,z2), 0, 100);
		Line(mglPoint(x1,y2,z2), mglPoint(x2,y1,z2), 0, 100);
	}
	else if((TernAxis&3)==2)	// quaternary axis
	{
		Line(mglPoint(x1,y1,z1), mglPoint(x1,y2,z1), 0, 100);
		Line(mglPoint(x1,y1,z1), mglPoint(x2,y1,z1), 0, 100);
		Line(mglPoint(x1,y2,z1), mglPoint(x2,y1,z1), 0, 100);
		
		Line(mglPoint(x1,y1,z1), mglPoint(x1,y1,z2), 0, 100);
		Line(mglPoint(x1,y2,z1), mglPoint(x1,y1,z2), 0, 100);
		Line(mglPoint(x2,y1,z1), mglPoint(x1,y1,z2), 0, 100);
	}
	else
	{
		Line(mglPoint(x1,y1,z1), mglPoint(x1,y1,z2), 0, 100);
		Line(mglPoint(x1,y2,z1), mglPoint(x1,y2,z2), 0, 100);
		Line(mglPoint(x2,y1,z1), mglPoint(x2,y1,z2), 0, 100);
		Line(mglPoint(x2,y2,z1), mglPoint(x2,y2,z2), 0, 100);
		
		Line(mglPoint(x1,y1,z1), mglPoint(x1,y2,z1), 0, 100);
		Line(mglPoint(x1,y1,z1), mglPoint(x2,y1,z1), 0, 100);
		Line(mglPoint(x1,y2,z1), mglPoint(x2,y2,z1), 0, 100);
		Line(mglPoint(x2,y1,z1), mglPoint(x2,y2,z1), 0, 100);
		
		Line(mglPoint(x1,y1,z2), mglPoint(x1,y2,z2), 0, 100);
		Line(mglPoint(x1,y1,z2), mglPoint(x2,y1,z2), 0, 100);
		Line(mglPoint(x1,y2,z2), mglPoint(x2,y2,z2), 0, 100);
		Line(mglPoint(x2,y1,z2), mglPoint(x2,y2,z2), 0, 100);
	}
	if(ticks && !TernAxis)	TickBox();
	fc = tt;
	EndGroup();
}
//-----------------------------------------------------------------------------
void mglGraph::AddLegend(const wchar_t *text,const char *style)
{
	if(!text)	return;
	if(NumLeg>=100)	{	SetWarn(mglWarnLegA);	return;	}
	LegStr[NumLeg] = mgl_wcsdup(text);
	if(style)	LegStl[NumLeg] = mgl_strdup(style);
	else		LegStl[NumLeg] = mgl_strdup(last_style);
	NumLeg++;
}
//-----------------------------------------------------------------------------
void mglGraph::Legend(mreal x, mreal y, const char *font, mreal size, mreal llen)
{	Legend(NumLeg,LegStr,LegStl,x,y,font,size,llen);	}
//-----------------------------------------------------------------------------
void mglGraph::Legend(int where, const char *font, mreal size, mreal llen)
{	Legend(NumLeg,LegStr,LegStl,where,font,size,llen);	}
//-----------------------------------------------------------------------------
void mglGraph::ClearLegend()
{
	for(long i=0;i<NumLeg;i++)	{	free(LegStr[i]);	free(LegStl[i]);	}
	NumLeg = 0;
}
//-----------------------------------------------------------------------------
void mglGraph::Legend(int n, wchar_t **text,char **style, int where,
					const char *font, mreal size, mreal llen)
{
	if(n<1)	{	SetWarn(mglWarnLeg);	return;	}
	if(isnan(llen))	llen=0.1;
	if(size<0)	size = -size*FontSize;
	mreal w=0, r=GetRatio(), rh, rw;

	rh=(r<1?r:1.)*size/6.;	// 6 = 12/2 = (16/1.4)/2
	rw=(r>1?1./r:1.)*size/8.;
	mreal h=fnt->Height(font)*rh, x, y, dx = 0.06, dy = 0.06;
	for(long i=0;i<n;i++)		// find text length
	{
		x = fnt->Width(text[i],font)*rw;
		if(style[i][0]==0)	x -= llen;
		w = w>x ? w:x;
	}
	w = (w + 1.1f*llen*1.5);	// add space for lines
  // Add margins
  w += 2*w*0.1f;
  h += 2*h*0.1f;

	switch(where)
	{
	case 0:		x = dx-1;	y = dy-1;		break;
	case 1:		x = 1-w-dx;	y = dy-1;		break;
	case 2:		x = dx-1;	y = 1-h*n-dy;	break;
	default:	x = 1-w-dx;	y = 1-h*n-dy;	break;
	}
	Legend(n,text,style,(x+1)/2,(y+1)/2,font,size,llen);
}
//-----------------------------------------------------------------------------
void mglGraph::Ternary(int t)
{
	static mglPoint x1(-1,-1,-1),x2(1,1,1),o(NAN,NAN,NAN);
	static bool c = true;
	TernAxis = t;
	if(t)
	{
		x1 = Min;	x2 = Max;	o = Org;	c = Cut;
		Cut = false;	OrgT = mglPoint(NAN,NAN,NAN);
		Axis(mglPoint(0,0,0),mglPoint(1,1,t==2?1:0),mglPoint(0,0,0));
	}
	else	{	Axis(x1,x2,o);	Cut = c;	}
}
//-----------------------------------------------------------------------------
void mglGraph::DrawTTick(mreal y, mreal x0, mreal z0, mreal dx, mreal dz, int f)
{
	mreal pp[9],ff=TickLen/sqrt(1.+f*st_t);
	pp[0]=x0-y+Min.y-dx*ff;		pp[1]=y;	pp[2]=z0;
	pp[3]=x0-y+Min.y;			pp[4]=y;	pp[5]=z0;
	pp[6]=x0-y+Min.y;			pp[7]=y;	pp[8]=z0+dz*ff;
	if(*TickStl && !f)	SelectPen(TickStl);
	if(*SubTStl && f)	SelectPen(SubTStl);
	DrawTick(pp,f!=0);
}
void mglGraph::AxisT(bool text, const char *stl)
{
	mreal pp[3*32];
	wchar_t str[64];
	long i;
	mreal z0=GetOrgZ('z'), y, ddx, ddy, ddz;
	ddx = dx>=0 ? dx : mgl_okrugl((Min.x-Max.x)/(dx+1),3);
	ddy = dy>=0 ? dy : mgl_okrugl((Min.y-Max.y)/(dy+1),3);
	ddz = dz>=0 ? dz : mgl_okrugl((Min.z-Max.z)/(dz+1),3);
	if(ddx==0)	ddx=(Max.x-Min.x)/2;
	if(ddy==0)	ddy=(Max.y-Min.y)/2;
	if(ddz==0)	ddz=(Max.z-Min.z)/2;
	SelectPen(stl);

	for(i=0;i<31;i++)	// сама ось
	{
		pp[3*i]=Max.x+(Min.x-Max.x)*i/30.;
		pp[3*i+2]=z0;
		pp[3*i+1]=Min.y+(Max.y-Min.y)*i/30.;
		ScalePoint(pp[3*i],pp[3*i+1],pp[3*i+2]);
	}
	if(stl[3])
	{
		pp[3*31]=Max.x+(Min.x-Max.x)*1.05;
		pp[3*31+1]=Min.y+(Max.y-Min.y)*1.05;
		pp[3*31+2]=z0;
		ScalePoint(pp[3*31],pp[3*31+1],pp[3*31+2]);
		curv_plot(32,pp,0);
	}
	else	curv_plot(31,pp,0);
	for(i=0;i<31;i++)	// сама ось
	{
		pp[3*i]=Min.x;
		pp[3*i+1]=Max.y+(Min.y-Max.y)*i/30.;
		pp[3*i+2]=z0;
		ScalePoint(pp[3*i],pp[3*i+1],pp[3*i+2]);
	}
	if(stl[3])
	{
		pp[3*31]=Min.x;	pp[3*31+2]=z0;
		pp[3*31+1]=Max.y+(Min.y-Max.y)*1.05;
		ScalePoint(pp[3*31],pp[3*31+1],pp[3*31+2]);
		curv_plot(32,pp,0);
	}
	else	curv_plot(31,pp,0);

	NSy = NSy<0 ? 0 : NSy;
	float tmp=Max.y+(Max.y-Min.y)*1e-6;
	if(Min.y+ddy!=Min.y && tmp+ddy!=tmp)	for(y=Min.y;y<=tmp;y+=ddy)
	{
		if(y>Min.y && y<Max.y)	DrawTTick(y,Max.x,z0,ddx,ddz);
		mglprintf(str,64,L"%.2g",y);		wcstrim_mgl(str);
		if(text)	Putsw(mglPoint(Max.x+Min.y-y,y,z0),str,FontDef,FontSize,'t');

		if(y>Min.y && y<Max.y)	DrawYTick(Max.y+Min.y-y,Min.x,z0,ddx,ddz);
		mreal yy = 1-Min.x-Min.y-Max.y+y;
		yy = fabs(yy)<(Max.y-Min.y)*1e-6 ? 0:yy;
		mglprintf(str,64,L"%.2g",yy);	wcstrim_mgl(str);
		if(text)	Putsw(mglPoint(Min.x,Max.y+Min.y-y,z0),str,FontDef,FontSize,'y');
	}
	if(NSy>0)
	{
		ddy /= (NSy+1);	// subticks drawing
		if(Min.y+ddy!=Min.y && Max.y+ddy!=Max.y)	for(y=Min.y;y<=Max.y;y+=ddy)
		{
			DrawTTick(y,Max.x,z0,ddx,ddz,1);
			DrawYTick(Max.y+Min.y-y,Min.x,z0,ddx,ddz,1);
		}
	}
}
//-----------------------------------------------------------------------------
void mglGraph::SetTicks(char dir, mreal d, int ns, mreal org)
{
	if(dir=='x')
	{	dx = d;	NSx = ns;	OrgT.x = org;	if(xnum) delete []xbuf;	xnum=0;	}
	else if(dir=='y')
	{	dy = d;	NSy = ns;	OrgT.y = org;	if(ynum) delete []ybuf;	ynum=0;	}
	else if(dir=='z')
	{	dz = d;	NSz = ns;	OrgT.z = org;	if(znum) delete []zbuf;	znum=0;	}
	else if(dir=='c' || dir=='a')	{	dc = d;	OrgC = org;	}
}
//-----------------------------------------------------------------------------
void mglGraph::SetTicksVal(char dir, int n, mreal *val, const wchar_t **lbl)
{
	long len=0;
	register int i;
	if(dir=='x')
	{
		if(xnum)	delete []xbuf;
		if(n<1 || !val || !lbl)	{	xnum=0;	return;	}
		if(n>50)	n=50;
		for(i=0;i<n;i++)	len += wcslen(lbl[i])+1;
		xbuf = new wchar_t[len];	xnum = n;	len = 0;
		for(i=0;i<n;i++)
		{
			wcscpy(xbuf+len, lbl[i]);	xval[i] = val[i];
			xstr[i] = xbuf+len;			len += wcslen(lbl[i])+1;
		}
	}
	else if(dir=='y')
	{
		if(ynum)	delete []ybuf;
		if(n<1 || !val || !lbl)	{	ynum=0;	return;	}
		if(n>50)	n=50;
		for(i=0;i<n;i++)	len += wcslen(lbl[i])+1;
		ybuf = new wchar_t[len];	ynum = n;	len = 0;
		for(i=0;i<n;i++)
		{
			wcscpy(ybuf+len, lbl[i]);	yval[i] = val[i];
			ystr[i] = ybuf+len;			len += wcslen(lbl[i])+1;
		}
	}
	else if(dir=='z')
	{
		if(znum)	delete []zbuf;
		if(n<1 || !val || !lbl)	{	znum=0;	return;	}
		if(n>50)	n=50;
		for(i=0;i<n;i++)	len += wcslen(lbl[i])+1;
		zbuf = new wchar_t[len];	znum = n;	len = 0;
		for(i=0;i<n;i++)
		{
			wcscpy(zbuf+len, lbl[i]);	zval[i] = val[i];
			zstr[i] = zbuf+len;			len += wcslen(lbl[i])+1;
		}
	}
}
//-----------------------------------------------------------------------------
void mglGraph::SetTicksVal(char dir, int n, mreal *val, const char **lbl)
{
	long len=0;
	register int i,ll;
	if(dir=='x')
	{
		if(xnum)	delete []xbuf;
		if(n<1 || !val || !lbl)	{	xnum=0;	return;	}
		if(n>50)	n=50;
		for(i=0;i<n;i++)	len += strlen(lbl[i])+1;
		xbuf = new wchar_t[len];	xnum = n;	len = 0;
		for(i=0;i<n;i++)
		{
			ll = strlen(lbl[i])+1;		xval[i] = val[i];
			mbstowcs(xbuf+len,lbl[i],ll);	xstr[i] = xbuf+len;
			len += ll;
		}
	}
	if(dir=='y')
	{
		if(ynum)	delete []ybuf;
		if(n<1 || !val || !lbl)	{	ynum=0;	return;	}
		if(n>50)	n=50;
		for(i=0;i<n;i++)	len += strlen(lbl[i])+1;
		ybuf = new wchar_t[len];	ynum = n;	len = 0;
		for(i=0;i<n;i++)
		{
			ll = strlen(lbl[i])+1;		yval[i] = val[i];
			mbstowcs(ybuf+len,lbl[i],ll);	ystr[i] = ybuf+len;
			len += ll;
		}
	}
	if(dir=='z')
	{
		if(znum)	delete []zbuf;
		if(n<1 || !val || !lbl)	{	znum=0;	return;	}
		if(n>50)	n=50;
		for(i=0;i<n;i++)	len += strlen(lbl[i])+1;
		zbuf = new wchar_t[len];	znum = n;	len = 0;
		for(i=0;i<n;i++)
		{
			ll = strlen(lbl[i])+1;		zval[i] = val[i];
			mbstowcs(zbuf+len,lbl[i],ll);	zstr[i] = zbuf+len;
			len += ll;
		}
	}
}
//-----------------------------------------------------------------------------
void mglGraph::SetTicksVal(char dir, int n, double val, const char *lbl, ...)
{
	if(n<1)	return;
	mreal *v = new mreal[n];
	const char **l = (const char **)malloc(n*sizeof(const char *));
	v[0] = val;	l[0] = lbl;
	va_list ap;
	va_start(ap, lbl);
	for(int i=1;i<n;i++)
	{
		v[i] = va_arg(ap, double);
		l[i] = va_arg(ap, char *);
	}
	SetTicksVal(dir,n,v,l);
	va_end(ap);
	delete []v;		free(l);
}
//-----------------------------------------------------------------------------
void mgl_set_ticks_val(HMGL gr, char dir, int n, double val, const char *lbl, ...)
{	// NOTE: I have to repeat the function because I don't know how to pass variable arguments to C++ function
	if(n<1)	return;
	mreal *v = new mreal[n];
	const char **l = (const char **)malloc(n*sizeof(const char *));
	v[0] = val;	l[0] = lbl;
	va_list ap;
	va_start(ap, lbl);
	for(int i=1;i<n;i++)
	{
		v[i] = va_arg(ap, double);
		l[i] = va_arg(ap, char *);
	}
	gr->SetTicksVal(dir,n,v,l);
	va_end(ap);
	delete []v;		free(l);
}
//-----------------------------------------------------------------------------
void mglGraph::adjust(char dir, mreal d)
{
	mreal n;
	d = fabs(d);	n = floor(log10(d));	d = floor(d*pow(10,-n));
	if(d<4)		SetTicks(dir,0.5*pow(10,n),4,0);
	else if(d<7)SetTicks(dir,pow(10,n),4,0);
	else		SetTicks(dir,2*pow(10,n),3,0);
}
bool mgl_islog(mreal a, mreal b)
{	return (a>0 && b>10*a) || (b<0 && a<10*b);	}
void mglGraph::AdjustTicks(const char *dir)
{
	if(strchr(dir,'x') || strchr(dir,'X'))
	{	if(fx && mgl_islog(Min.x,Max.x))	SetTicks ('x',0);	else	adjust('x',Max.x-Min.x);	}
	if(strchr(dir,'y') || strchr(dir,'Y'))
	{	if(fy && mgl_islog(Min.y,Max.y))	SetTicks ('y',0);	else	adjust('y',Max.y-Min.y);	}
	if(strchr(dir,'z') || strchr(dir,'Z'))
	{	if(fz && mgl_islog(Min.z,Max.z))	SetTicks ('z',0);	else	adjust('z',Max.z-Min.z);	}
	if(strchr(dir,'c') || strchr(dir,'C') || strchr(dir,'a') || strchr(dir,'A'))
	{	if(fa && mgl_islog(Cmin,Cmax))	SetTicks ('c',0);	else	adjust('c',Cmax-Cmin);	}
	TuneTicks = true;
}
//-----------------------------------------------------------------------------
