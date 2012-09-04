/***************************************************************************
 * mgl_1d.cpp is part of Math Graphic Library
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
#include "mgl/mgl.h"
#include "mgl/mgl_c.h"
#include "mgl/mgl_f.h"
#include "mgl/mgl_eval.h"
//-----------------------------------------------------------------------------
//
//	Primitives
//
//-----------------------------------------------------------------------------
void mglGraph::Cone(mglPoint p1, mglPoint p2, mreal r1, mreal r2, const char *stl, bool edge)
{
	if(r2<0)	r2 = r1;
	if(r1==0 && r2==0)	return;		// zero radius is invisible
	int n = edge ? 4:2;
	mglData x(n), y(n), z(n), r(n);
	if(edge)
	{
		mreal d;
		d = (p2.x-p1.x)/1e3;	x.a[0] = p1.x-d;	x.a[1] = p1.x;	x.a[2] = p2.x;	x.a[3] = p2.x+d;
		d = (p2.y-p1.y)/1e3;	y.a[0] = p1.y-d;	y.a[1] = p1.y;	y.a[2] = p2.y;	y.a[3] = p2.y+d;
		d = (p2.z-p1.z)/1e3;	z.a[0] = p1.z-d;	z.a[1] = p1.z;	z.a[2] = p2.z;	z.a[3] = p2.z+d;
		r.a[0] = 0;	r.a[1] = r1;	r.a[2] = r2;	r.a[3] = 0;
	}
	else
	{
		x.a[0] = p1.x;	x.a[1] = p2.x;
		y.a[0] = p1.y;	y.a[1] = p2.y;
		z.a[0] = p1.z;	z.a[1] = p2.z;
		r.a[0] = r1;	r.a[1] = r2;
	}
	Tube(x,y,z,r,stl);
}
//-----------------------------------------------------------------------------
void mglGraph::Curve(mglPoint p1, mglPoint d1, mglPoint p2, mglPoint d2, const char *stl, int n)
{
	static int cgid=1;	StartGroup("Curve",cgid++);
	register long i;
	n = (n<2) ? 2 : n;
	mreal *pp = new mreal[3*n],a[3],b[3],s;
	bool cut = Cut;	Cut = true;
	SelectPen(stl);
	a[0] = 3*(p2.x-p1.x) - (d2.x+2*d1.x);	b[0] = d1.x+d2.x - 2*(p2.x-p1.x);
	a[1] = 3*(p2.y-p1.y) - (d2.y+2*d1.y);	b[1] = d1.y+d2.y - 2*(p2.y-p1.y);
	a[2] = 3*(p2.z-p1.z) - (d2.z+2*d1.z);	b[2] = d1.z+d2.z - 2*(p2.z-p1.z);

	for(i=0;i<n;i++)
	{
		s = i/(n-1.);
		pp[3*i+0] = p1.x+s*(d1.x+s*(a[0]+s*b[0]));
		pp[3*i+1] = p1.y+s*(d1.y+s*(a[1]+s*b[1]));
		pp[3*i+2] = p1.z+s*(d1.z+s*(a[2]+s*b[2]));
		ScalePoint(pp[3*i+0],pp[3*i+1],pp[3*i+2]);
	}
	curv_plot(n,pp,0);
	Cut = cut;
	EndGroup();
	delete []pp;
}
//-----------------------------------------------------------------------------
void mglGraph::Line(mglPoint p1, mglPoint p2, const char *stl, int n)
{
	register long i;
	n = (n<2) ? 2 : n;
	mreal *pp = new mreal[3*n],s;
	bool cut = Cut;	Cut = true;
	SelectPen(stl);

	for(i=0;i<n;i++)
	{
		s = i/mreal(n-1);
		pp[3*i+0] = p1.x+s*(p2.x-p1.x);
		pp[3*i+1] = p1.y+s*(p2.y-p1.y);
		pp[3*i+2] = p1.z+s*(p2.z-p1.z);
		ScalePoint(pp[3*i+0],pp[3*i+1],pp[3*i+2]);
	}
	curv_plot(n,pp,0);
	Cut = cut;
//	Flush();
	delete []pp;
}
//-----------------------------------------------------------------------------
void mglGraph::Error(mglPoint p,mglPoint e,const char *pen)
{
	static int cgid=1;	StartGroup("ErBox",cgid++);
	char mk=SelectPen(pen);
	ArrowSize /=2;
	Line(mglPoint(p.x+e.x,p.y,p.z),mglPoint(p.x-e.x,p.y,p.z),"-II");
	Line(mglPoint(p.x,p.y+e.y,p.z),mglPoint(p.x,p.y-e.y,p.z),"-II");
	Line(mglPoint(p.x,p.y,p.z+e.z),mglPoint(p.x,p.y,p.z-e.z),"-II");
	if(mk)	Mark(p,mk);
	ArrowSize *=2;
	EndGroup();
}
//-----------------------------------------------------------------------------
//
//	Plot by formulas series
//
//-----------------------------------------------------------------------------
void mglGraph::Plot(const char *eqY, const char *pen, mreal zVal, int n)
{
	if(eqY==0 || eqY[0]==0)	return;		// nothing to plot
	if(n<=0)	n=100;

	mreal *x = (mreal *)malloc(n*sizeof(mreal));
	mreal *y = (mreal *)malloc(n*sizeof(mreal));
	mglFormula *eq = new mglFormula(eqY);
	register int i;
	mreal d = (Max.x - Min.x)/(n-1.), xs, ys, yr, ym=fabs(Max.y - Min.y)/1000;
	for(i=0;i<n;i++)	// initial data filling
	{	x[i]=Min.x + i*d;	y[i]=eq->Calc(x[i]);	}

	for(i=0;i<n-1 && n<10000;)
	{
		xs=(x[i]+x[i+1])/2;
		ys=(y[i]+y[i+1])/2;	yr=eq->Calc(xs);
		if(fabs(yr-ys)>ym)	// bad approximation here
		{
			x = (mreal *)realloc(x,(n+1)*sizeof(mreal));
			y = (mreal *)realloc(y,(n+1)*sizeof(mreal));
			memmove(x+i+2,x+i+1,(n-i-1)*sizeof(mreal));
			memmove(y+i+2,y+i+1,(n-i-1)*sizeof(mreal));
			x[i+1] = xs;	y[i+1] = yr;	n++;
		}
		else i++;
	}
	
	delete eq;
	mglData yy,xx;
	xx.Set(x,n);	free(x);
	yy.Set(y,n);	free(y);
	Plot(xx,yy,pen, zVal);
}
//-----------------------------------------------------------------------------
void mglGraph::Plot(const char *eqX, const char *eqY, const char *eqZ, const char *pen, int n)
{
	if(n<=0)	n=100;
	mreal *x = (mreal *)malloc(n*sizeof(mreal));
	mreal *y = (mreal *)malloc(n*sizeof(mreal));
	mreal *z = (mreal *)malloc(n*sizeof(mreal));
	mreal *t = (mreal *)malloc(n*sizeof(mreal));
	mglFormula *ex, *ey, *ez;
	ex = new mglFormula(eqX ? eqX : "0");
	ey = new mglFormula(eqY ? eqY : "0");
	ez = new mglFormula(eqZ ? eqZ : "0");
	register int i;
	mreal ts, xs, ys, zs, xr, yr, zr, xm=fabs(Max.x - Min.x)/1000, ym=fabs(Max.y - Min.y)/1000, zm=fabs(Max.z - Min.z)/1000;
	for(i=0;i<n;i++)	// initial data filling
	{
		t[i] = i/(n-1.);
		x[i] = ex->Calc(0,0,t[i]);
		y[i] = ey->Calc(0,0,t[i]);
		z[i] = ez->Calc(0,0,t[i]);
	}

	for(i=0;i<n-1 && n<10000;)
	{
		ts=(t[i]+t[i+1])/2;
		xs=(x[i]+x[i+1])/2;	xr=ex->Calc(0,0,ts);
		ys=(y[i]+y[i+1])/2;	yr=ey->Calc(0,0,ts);
		zs=(z[i]+z[i+1])/2;	zr=ez->Calc(0,0,ts);
		if(fabs(xr-xs)>xm || fabs(yr-ys)>ym || fabs(zr-zs)>zm)	// bad approximation here
		{
			z = (mreal *)realloc(z,(n+1)*sizeof(mreal));
			t = (mreal *)realloc(t,(n+1)*sizeof(mreal));
			x = (mreal *)realloc(x,(n+1)*sizeof(mreal));
			y = (mreal *)realloc(y,(n+1)*sizeof(mreal));
			memmove(x+i+2,x+i+1,(n-i-1)*sizeof(mreal));
			memmove(y+i+2,y+i+1,(n-i-1)*sizeof(mreal));
			memmove(z+i+2,z+i+1,(n-i-1)*sizeof(mreal));
			memmove(t+i+2,t+i+1,(n-i-1)*sizeof(mreal));
			t[i+1]=ts;	x[i+1]=xr;	y[i+1]=yr;	z[i+1]=zr;	n++;
		}
		else i++;
	}
	delete ex;	delete ey;	delete ez;

	mglData xx,yy,zz;
	xx.Set(x,n);	yy.Set(y,n);	zz.Set(z,n);
	free(x);	free(y);	free(z);	free(t);
	Plot(xx,yy,zz,pen);
}
//-----------------------------------------------------------------------------
//
//	Radar series
//
//-----------------------------------------------------------------------------
void mglGraph::Radar(const mglData &a, const char *pen, mreal r)
{
	long n = a.nx;
	if(n<2)	{	SetWarn(mglWarnLow,"Radar");	return;	}
	mglData x(n+1,a.ny), y(n+1,a.ny);
	mreal m=a.Minimal();
	if(r<0)	r = m<0 ? -m:0;
	register long i,j;
	for(j=0;j<a.ny;j++)
	{
		for(i=0;i<n;i++)
		{
			x.a[i+(n+1)*j] = (r+a.a[i+n*j])*cos(2*i*M_PI/n);
			y.a[i+(n+1)*j] = (r+a.a[i+n*j])*sin(2*i*M_PI/n);
		}
		x.a[n+(n+1)*j] = r+a.a[n*j];	y.a[n+(n+1)*j] = 0;
	}
	Plot(x,y,pen);
	if(pen && strchr(pen,'#'))	// draw "grid"
	{
		m = 1.1*(a.Maximal()+r);
		for(i=0;i<n;i++)
			Line(mglPoint(0), mglPoint(m*cos(2*i*M_PI/n),m*sin(2*i*M_PI/n)),"k");
		if(r>0)
		{
			x.Create(101);	y.Create(101);
			for(i=0;i<101;i++)
			{	x.a[i]=r*cos(2*i*M_PI/100);	y.a[i]=r*sin(2*i*M_PI/100);	}
      SetPalColor(0, 0, 0, 0);
			Plot(x,y,"k", -1);	SetPal(0);
		}
	}
}
//-----------------------------------------------------------------------------
//
//	Plot series
//
//-----------------------------------------------------------------------------
void mglGraph::Plot(const mglData &x, const mglData &y, const mglData &z, const char *pen)
{
	long j,m,mx,my,mz,n=y.nx;
	Arrow1 = Arrow2 = '_';
	char mk=0;
	if(x.nx!=n || z.nx!=n)	{	SetWarn(mglWarnDim,"Plot");	return;	}
	if(n<2)					{	SetWarn(mglWarnLow,"Plot");	return;	}
	static int cgid=1;	StartGroup("Plot",cgid++);
	m = x.ny > y.ny ? x.ny : y.ny;	m = z.ny > m ? z.ny : m;
	mreal *pp = new mreal[6*n];
	bool *tt = new bool[2*n],inan,onan;
	if(pen && *pen)	mk=SelectPen(pen);	else	Pen(NC, '-', BaseLineWidth);
	SetPal(pen);
	for(j=0;j<m;j++)
	{
		Pen(GetPal(),0,0);
		mx = j<x.ny ? j:0;	my = j<y.ny ? j:0;	mz = j<z.ny ? j:0;
		register long i,k,jj;
		inan = onan = false;
		for(jj=i=0;i<n;i++,jj++)
		{
			k = 3*jj;
			pp[k+0] = x.a[i+mx*n];	pp[k+1] = y.a[i+my*n];	pp[k+2] = z.a[i+mz*n];
			inan = isnan(pp[k+0]) || isnan(pp[k+1]) || isnan(pp[k+2]);
			if(jj>0)	onan = isnan(pp[k-3]) || isnan(pp[k-2]) || isnan(pp[k-1]);
			tt[jj] = ScalePoint(pp[k],pp[k+1],pp[k+2]);
			if(mk && tt[jj])	Mark(pp[k],pp[k+1],pp[k+2],mk);
			if(jj>0 && ((tt[jj] && !tt[jj-1] && !onan) || (tt[jj-1] && !tt[jj] && !inan)))	// do smoothing
			{
				float i1=0, i2=1, ii;
				pp[k+3] = pp[k+0];	pp[k+4] = pp[k+1];	// copy current
				pp[k+5] = pp[k+2];	tt[jj+1] = tt[jj];
				do {
					ii = (i1+i2)/2;
					pp[k+0] = x.a[i+mx*n]*ii+x.a[i-1+mx*n]*(1-ii);
					pp[k+1] = y.a[i+my*n]*ii+y.a[i-1+my*n]*(1-ii);
					pp[k+2] = z.a[i+mz*n]*ii+z.a[i-1+mz*n]*(1-ii);
					tt[jj] = ScalePoint(pp[k],pp[k+1],pp[k+2]);
					if((tt[jj] && tt[jj+1]) || (tt[jj-1] && !tt[jj]))	i2 = ii;
					else	i1 = ii;
				} while(fabs(i2-i1)>1e-3);
				tt[jj] = true;	jj++;
			}
		}
		curv_plot(jj,pp,tt);
	}
	SetPal(0);	EndGroup();
	delete []tt;	delete []pp;
}
//-----------------------------------------------------------------------------
void mglGraph::Plot(const mglData &x, const mglData &y, const char *pen,mreal zVal)
{
	mglData z(y.nx);
	if(isnan(zVal))	zVal = Min.z;
	z.Fill(zVal,zVal);
	Plot(x,y,z,pen);
}
//-----------------------------------------------------------------------------
void mglGraph::Plot(const mglData &y, const char *pen,mreal zVal)
{
	if(y.nx<2)	{	SetWarn(mglWarnLow,"Plot");	return;	}
	mglData x(y.nx);
	x.Fill(Min.x,Max.x);
	Plot(x,y,pen,zVal);
}
//-----------------------------------------------------------------------------
//
//	Plot series
//
//-----------------------------------------------------------------------------
void mglGraph::Tens(const mglData &x, const mglData &y, const mglData &z, const mglData &c, const char *pen)
{
	long j,m,mx,my,mz,mc,n=y.nx;
	Arrow1 = Arrow2 = '_';
	char mk=0;
	if(x.nx!=n || z.nx!=n || c.nx!=n)
	{	SetWarn(mglWarnDim,"Tens");	return;	}
	if(n<2)					{	SetWarn(mglWarnLow,"Tens");	return;	}
	static int cgid=1;	StartGroup("Tens",cgid++);
	m = x.ny > y.ny ? x.ny : y.ny;	m = z.ny > m ? z.ny : m;
	mreal *pp = new mreal[6*n], *cc = new mreal[2*n];
	bool *tt = new bool[2*n];
	if(pen && *pen)	{	SetScheme(pen,false);	mk=SelectPen(pen);	}
	for(j=0;j<m;j++)
	{
		mx = j<x.ny ? j:0;	my = j<y.ny ? j:0;
		mz = j<z.ny ? j:0;	mc = j<c.ny ? j:0;
		register long i,jj,k;
		for(jj=i=0;i<n;i++,jj++)
		{
			k = 3*jj;
			pp[k+0] = x.a[i+mx*n];	pp[k+1] = y.a[i+my*n];
			pp[k+2] = z.a[i+mz*n];	cc[jj] = c.a[i+mc*n];
			tt[jj] = ScalePoint(pp[k],pp[k+1],pp[k+2]);
			if(mk && tt[jj])
			{
				DefColor(GetC(cc[jj]));
				Mark(pp[k],pp[k+1],pp[k+2],mk);
			}
			if(jj>0 && ((tt[jj] && !tt[jj-1]) || (tt[jj-1] && !tt[jj])))	// do smoothing
			{
				float i1=0, i2=1, ii;
				pp[k+3] = pp[k+0];	pp[k+4] = pp[k+1];	// copy current
				pp[k+5] = pp[k+2];	tt[jj+1] = tt[jj];
				do {
					ii = (i1+i2)/2;
					pp[k+0] = x.a[i+mx*n]*ii+x.a[i-1+mx*n]*(1-ii);
					pp[k+1] = y.a[i+my*n]*ii+y.a[i-1+my*n]*(1-ii);
					pp[k+2] = z.a[i+mz*n]*ii+z.a[i-1+mz*n]*(1-ii);
					tt[jj] = ScalePoint(pp[k],pp[k+1],pp[k+2]);
					if((tt[jj] && tt[jj+1]) || (tt[jj-1] && !tt[jj]))	i2 = ii;
					else	i1 = ii;
				} while(fabs(i2-i1)>1e-3);
				tt[jj] = true;	jj++;
			}
		}
		curv_plot(jj,pp,cc,tt);
	}
	EndGroup();
	delete []tt;	delete []pp;	delete []cc;
}
//-----------------------------------------------------------------------------
void mglGraph::Tens(const mglData &x, const mglData &y, const mglData &c, const char *pen,mreal zVal)
{
	mglData z(y.nx);
	if(isnan(zVal))	zVal = Min.z;
	z.Fill(zVal,zVal);
	Tens(x,y,z,c,pen);
}
//-----------------------------------------------------------------------------
void mglGraph::Tens(const mglData &y, const mglData &c, const char *pen,mreal zVal)
{
	if(y.nx<2)	{	SetWarn(mglWarnLow,"Tens");	return;	}
	mglData x(y.nx);
	x.Fill(Min.x,Max.x);
	Tens(x,y,c,pen,zVal);
}
//-----------------------------------------------------------------------------
//
//	Area series
//
//-----------------------------------------------------------------------------
void mglGraph::Area(const mglData &x, const mglData &y, const mglData &z, const char *pen)
{
	long i,j,n=y.nx,m,mx,my,mz;
	if(x.nx!=n || z.nx!=n)	{	SetWarn(mglWarnDim,"Area");	return;	}
	if(n<2)					{	SetWarn(mglWarnLow,"Area");	return;	}
	static int cgid=1;	StartGroup("Area3",cgid++);
	m = x.ny > y.ny ? x.ny : y.ny;	m = z.ny > m ? z.ny : m;
	mreal *pp = new mreal[6*n], *cc = new mreal[8*n], z0=GetOrgZ('x');
	bool *tt = new bool[2*n];
	mglColor c1,c2;
	SetPal(pen);
	for(j=0;j<m;j++)
	{
		c1 = GetPal();
		if(NumPal==2*m)	c2 = GetPal();	else	c2 = c1;
		mx = j<x.ny ? j:0;	my = j<y.ny ? j:0;	mz = j<z.ny ? j:0;

		for(i=0;i<y.nx;i++)
		{
			cc[8*i+3] = cc[8*i+7] = AlphaDef;
			cc[8*i]   = c2.r;	cc[8*i+1] = c2.g;	cc[8*i+2] = c2.b;
			cc[8*i+4] = c1.r;	cc[8*i+5] = c1.g;	cc[8*i+6] = c1.b;
			pp[6*i+3] = pp[6*i+0] = x.a[i+mx*n];
			pp[6*i+4] = pp[6*i+1] = y.a[i+my*n];
			pp[6*i+2] = z.a[i+mz*n];
			tt[2*i] = ScalePoint(pp[6*i+0],pp[6*i+1],pp[6*i+2]);
			pp[6*i+5] = z0;
			tt[2*i+1] = ScalePoint(pp[6*i+3],pp[6*i+4],pp[6*i+5]);
		}
		surf_plot(2,n,pp,cc,tt);
	}
	SetPal(0);	EndGroup();
	delete []pp;	delete []tt;	delete []cc;
}
//-----------------------------------------------------------------------------
void mglGraph::Area(const mglData &x, const mglData &y, const char *pen, mreal zVal)
{
	long i,j,n=y.nx,m=y.ny,mx;
	if(x.nx!=n)	{	SetWarn(mglWarnDim,"Area");	return;	}
	if(n<2)		{	SetWarn(mglWarnLow,"Area");	return;	}
	static int cgid=1;	StartGroup("Curve",cgid++);
	mreal *pp = new mreal[6*n], *cc = new mreal[8*n], y0=GetOrgY('x');
	bool *tt = new bool[2*n], znan = isnan(zVal);
	mglColor c1,c2;
	mglData f(y);
	bool sum = pen && strchr(pen,'a')!=0;
	if(sum)	f.CumSum("y");

	SetPal(pen);
	for(j=0;j<m;j++)
	{
		c1 = GetPal();
		if(NumPal==2*m)	c2 = GetPal();	else	c2 = c1;
		mx = j<x.ny ? j:0;	//my = j<y.ny ? j:0;
		for(i=0;i<n;i++)
		{
			cc[8*i+3] = cc[8*i+7] = AlphaDef;
			cc[8*i]   = c2.r;	cc[8*i+1] = c2.g;	cc[8*i+2] = c2.b;
			cc[8*i+4] = c1.r;	cc[8*i+5] = c1.g;	cc[8*i+6] = c1.b;

			pp[6*i+3] = pp[6*i+0] = x.a[i+mx*n];
			pp[6*i+1] = f.a[i+j*n];
			// NOTE: I use 'm' (not 'm-1') for placing area plots at z<Max.z
			pp[6*i+2] = znan ? Min.z + (m-1-j)*(Max.z-Min.z)/m : zVal;
			tt[2*i+0] = ScalePoint(pp[6*i+0],pp[6*i+1],pp[6*i+2]);
			pp[6*i+4] = sum && j>0 ? f.a[i+j*n-n] : y0;
			pp[6*i+5] = znan ? Min.z + (m-1-j)*(Max.z-Min.z)/m : zVal;
			tt[2*i+1] = ScalePoint(pp[6*i+3],pp[6*i+4],pp[6*i+5]);
		}
		surf_plot(2,n,pp,cc,tt);
	}
	SetPal(0);	EndGroup();
	delete []pp;	delete []tt;	delete []cc;
}
//-----------------------------------------------------------------------------
void mglGraph::Area(const mglData &y, const char *pen, mreal zVal)
{
	if(y.nx<2)		{	SetWarn(mglWarnLow,"Area");	return;	}
	mglData x(y.nx);
	x.Fill(Min.x,Max.x);
	Area(x,y,pen,zVal);
}
//-----------------------------------------------------------------------------
//
//	Region series
//
//-----------------------------------------------------------------------------
void mglGraph::Region(const mglData &x, const mglData &y1, const mglData &y2, const char *pen, mreal zVal, bool inside)
{
	long i,j, n=y1.nx, m=y1.ny, mx;
	if(x.nx!=n || y2.nx!=n || y2.ny!=m)
	{	SetWarn(mglWarnDim,"Region");	return;	}
	if(n<2)	{	SetWarn(mglWarnLow,"Region");	return;	}
	static int cgid=1;	StartGroup("Region",cgid++);
	mreal *pp = new mreal[6*n], *cc = new mreal[8*n], f1,f2;
	bool *tt = new bool[2*n];
	if(isnan(zVal))	zVal = Min.z;

	mglColor c1,c2;
	SetPal(pen);
	for(j=0;j<m;j++)
	{
		c1 = GetPal();
		if(NumPal==2*m)	c2 = GetPal();	else	c2 = c1;
		mx = j<x.ny ? j:0;
		for(i=0;i<n;i++)
		{
			cc[8*i+3] = cc[8*i+7] = AlphaDef;
			cc[8*i]   = c1.r;	cc[8*i+1] = c1.g;	cc[8*i+2] = c1.b;
			cc[8*i+4] = c2.r;	cc[8*i+5] = c2.g;	cc[8*i+6] = c2.b;
			pp[6*i+3] = pp[6*i+0] = x.a[i+mx*n];
			f1 = y1.a[i+j*n];	f2 = y2.a[i+j*n];
			pp[6*i+1] = f1;		pp[6*i+2] = zVal;
			tt[2*i+0] = ScalePoint(pp[6*i+0],pp[6*i+1],pp[6*i+2]);
			pp[6*i+4] = f2;		pp[6*i+5] = zVal;
			tt[2*i+1] = ScalePoint(pp[6*i+3],pp[6*i+4],pp[6*i+5]);
			if(f1>f2 && inside)	{	tt[2*i] = tt[2*i+1] = false;	}
		}
		surf_plot(2,n,pp,cc,tt);
	}
	SetPal(0);	EndGroup();
	delete []pp;	delete []tt;	delete []cc;
}
//-----------------------------------------------------------------------------
void mglGraph::Region(const mglData &y1, const mglData &y2, const char *pen, mreal zVal, bool inside)
{
	mglData x(y1.nx);
	x.Fill(Min.z, Max.z);
	Region(x,y1,y2,pen,zVal,inside);
}
//-----------------------------------------------------------------------------
//
//	Step series
//
//-----------------------------------------------------------------------------
void mglGraph::Step(const mglData &x, const mglData &y, const mglData &z, const char *pen)
{
	long i,j,m,mx,my,mz,n=y.nx;
	char mk=0;
	Arrow1 = Arrow2 = '_';
	if(x.nx!=n || z.nx!=n)	{	SetWarn(mglWarnDim,"Step");	return;	}
	if(n<2)					{	SetWarn(mglWarnLow,"Step");	return;	}
	static int cgid=1;	StartGroup("Step3",cgid++);
	m = x.ny > y.ny ? x.ny : y.ny;	m = z.ny > m ? z.ny : m;
	mreal *pp = new mreal[6*n];
	bool *tt = new bool[2*n];
	if(pen && *pen)	mk=SelectPen(pen);	else	Pen(NC, '-', BaseLineWidth);
	SetPal(pen);
	for(j=0;j<m;j++)
	{
		Pen(GetPal(),0,0);
		mx = j<x.ny ? j:0;	my = j<y.ny ? j:0;	mz = j<z.ny ? j:0;
		for(i=0;i<n;i++)
		{
			pp[6*i+0] = x.a[i+mx*n];
			pp[6*i+1] = y.a[i+my*n];
			pp[6*i+2] = z.a[i+mz*n];
			tt[2*i] = ScalePoint(pp[6*i+0],pp[6*i+1],pp[6*i+2]);
			if(mk && tt[2*i])	Mark(pp[6*i+0],pp[6*i+1],pp[6*i+2],mk);
			if(i<n-1)
			{
				pp[6*i+3] = x.a[i+1+mx*n];
				pp[6*i+4] = y.a[i+1+my*n];
				pp[6*i+5] = z.a[i+mz*n];
				tt[2*i+1] = ScalePoint(pp[6*i+3],pp[6*i+4],pp[6*i+5]);
			}
		}
		curv_plot(2*n-1,pp,tt);
	}
	SetPal(0);	EndGroup();
	delete []tt;	delete []pp;
}
//-----------------------------------------------------------------------------
void mglGraph::Step(const mglData &x, const mglData &y, const char *pen,mreal zVal)
{
	long i,j,m,mx,my,n=y.nx;
	char mk=0;
	Arrow1 = Arrow2 = '_';
	if(x.nx!=n)	{	SetWarn(mglWarnDim,"Step");	return;	}
	if(n<2)		{	SetWarn(mglWarnLow,"Step");	return;	}
	static int cgid=1;	StartGroup("Step",cgid++);
	m = x.ny > y.ny ? x.ny : y.ny;
	mreal *pp = new mreal[6*n];
	bool *tt = new bool[2*n];
	if(isnan(zVal))	zVal = Min.z;

	if(pen && *pen)	mk=SelectPen(pen);	else 	Pen(NC, '-', BaseLineWidth);
	SetPal(pen);
	for(j=0;j<m;j++)
	{
		Pen(GetPal(),0,0);
		mx = j<x.ny ? j:0;	my = j<y.ny ? j:0;
		for(i=0;i<n;i++)
		{
			pp[6*i+0] = x.a[i+mx*n];
			pp[6*i+1] = y.a[i+my*n];
			pp[6*i+2] = zVal;
			tt[2*i] = ScalePoint(pp[6*i+0],pp[6*i+1],pp[6*i+2]);
			if(mk && tt[2*i])	Mark(pp[6*i+0],pp[6*i+1],pp[6*i+2],mk);
			if(i<n-1)
			{
				pp[6*i+3] = x.a[i+1+mx*n];
				pp[6*i+4] = y.a[i+my*n];
				pp[6*i+5] = zVal;
				tt[2*i+1] = ScalePoint(pp[6*i+3],pp[6*i+4],pp[6*i+5]);
			}
		}
		curv_plot(2*n-1,pp,tt);
	}
	SetPal(0);	EndGroup();
	delete []tt;	delete []pp;
}
//-----------------------------------------------------------------------------
void mglGraph::Step(const mglData &y, const char *pen,mreal zVal)
{
	if(y.nx<2)	{	SetWarn(mglWarnLow,"Step");	return;	}
	mglData x(y.nx);
	x.Fill(Min.x,Max.x);
	Step(x,y,pen,zVal);
}
//-----------------------------------------------------------------------------
//
//	Stem series
//
//-----------------------------------------------------------------------------
void mglGraph::Stem(const mglData &x, const mglData &y, const mglData &z, const char *pen)
{
	long i,j,m,mx,my,mz,n=y.nx;
	char mk=0;
	Arrow1 = Arrow2 = '_';
	if(x.nx!=n || z.nx!=n)	{	SetWarn(mglWarnDim,"Stem");	return;	}
	if(n<2)					{	SetWarn(mglWarnLow,"Stem");	return;	}
	static int cgid=1;	StartGroup("Stem3",cgid++);
	m = x.ny > y.ny ? x.ny : y.ny;	m = z.ny > m ? z.ny : m;
	mreal *pp = new mreal[6*n],z0=GetOrgZ('x');
	long *nn = new long[2*n];
	bool *tt = new bool[2*n];
	if(pen && *pen)	mk=SelectPen(pen);	else 	Pen(NC, '-', BaseLineWidth);
	SetPal(pen);
	for(j=0;j<m;j++)
	{
		Pen(GetPal(),0,0);
		mx = j<x.ny ? j:0;	my = j<y.ny ? j:0;	mz = j<z.ny ? j:0;
		for(i=0;i<n;i++)
		{
			pp[6*i+3] = pp[6*i+0] = x.a[i+mx*n];
			pp[6*i+4] = pp[6*i+1] = y.a[i+my*n];
			pp[6*i+2] = z.a[i+mz*n];
			tt[2*i] = ScalePoint(pp[6*i+0],pp[6*i+1],pp[6*i+2]);
			if(mk && tt[2*i])	Mark(pp[6*i+0],pp[6*i+1],pp[6*i+2],mk);
			pp[6*i+5] = z0;
			tt[2*i+1] = ScalePoint(pp[6*i+3],pp[6*i+4],pp[6*i+5]);
			nn[2*i] = 2*i+1;	nn[2*i+1] = -1;
		}
		curv_plot(2*n,pp,tt,nn);
	}
	SetPal(0);	EndGroup();
	delete []tt;	delete []pp;	delete []nn;
}
//-----------------------------------------------------------------------------
void mglGraph::Stem(const mglData &x, const mglData &y, const char *pen,mreal zVal)
{
	long i,j,m,mx,my,n=y.nx;
	char mk=0;
	Arrow1 = Arrow2 = '_';
	if(x.nx!=n)	{	SetWarn(mglWarnDim,"Stem");	return;	}
	if(n<2)		{	SetWarn(mglWarnLow,"Stem");	return;	}
	static int cgid=1;	StartGroup("Stem",cgid++);
	m = x.ny > y.ny ? x.ny : y.ny;
	mreal *pp = new mreal[6*n],y0=GetOrgY('x');
	long *nn = new long[2*n];
	bool *tt = new bool[2*n];
	if(isnan(zVal))	zVal = Min.z;

	if(pen && *pen)	mk=SelectPen(pen);	else 	Pen(NC, '-', BaseLineWidth);
	SetPal(pen);
	for(j=0;j<m;j++)
	{
		Pen(GetPal(),0,0);
		mx = j<x.ny ? j:0;	my = j<y.ny ? j:0;
		for(i=0;i<n;i++)
		{
			pp[6*i+3] = pp[6*i+0] = x.a[i+mx*n];
			pp[6*i+1] = y.a[i+my*n];
			pp[6*i+5] = pp[6*i+2] = zVal;
			tt[2*i] = ScalePoint(pp[6*i+0],pp[6*i+1],pp[6*i+2]);
			if(mk && tt[2*i])	Mark(pp[6*i+0],pp[6*i+1],pp[6*i+2],mk);
			pp[6*i+4] = y0;
			tt[2*i+1] = ScalePoint(pp[6*i+3],pp[6*i+4],pp[6*i+5]);
			nn[2*i] = 2*i+1;	nn[2*i+1] = -1;
		}
		curv_plot(2*n,pp,tt,nn);
	}
	SetPal(0);	EndGroup();
	delete []tt;	delete []pp;	delete []nn;
}
//-----------------------------------------------------------------------------
void mglGraph::Stem(const mglData &y, const char *pen,mreal zVal)
{
	if(y.nx<2)	{	SetWarn(mglWarnLow,"Stem");	return;	}
	mglData x(y.nx);
	x.Fill(Min.x,Max.x);
	Stem(x,y,pen,zVal);
}
//-----------------------------------------------------------------------------
//
//	Bars series
//
//-----------------------------------------------------------------------------
void mglGraph::Bars(const mglData &x, const mglData &y, const mglData &z, const char *pen)
{
	long i,j,m,mx,my,mz,n=y.nx;
	if(x.nx!=n || z.nx!=n)	{	SetWarn(mglWarnDim,"Bars");	return;	}
	if(n<2)					{	SetWarn(mglWarnLow,"Bars");	return;	}
	static int cgid=1;	StartGroup("Bars3",cgid++);
	m = x.ny > y.ny ? x.ny : y.ny;	m = z.ny > m ? z.ny : m;
	mreal *pp = new mreal[12*n],*cc = new mreal[16*n],x1,x2,y1,y2,z0,zz,zp;
	bool *tt = new bool[4*n], wire = pen && strchr(pen,'#');
	bool above = pen && strchr(pen,'a')!=0, fall = pen && strchr(pen,'f')!=0;
	if(above)	fall = false;
	mglData dd(above ? n : 1);

	mglColor c1,c2,c;
	SetPal(pen);
	for(j=0;j<m;j++)
	{
		c1 = GetPal();
		if(NumPal==2*m)	c2 = GetPal();	else	c2 = c1;
		mx = j<x.ny ? j:0;	my = j<y.ny ? j:0;	mz = j<z.ny ? j:0;
		zp = z0 = GetOrgZ('x');
		for(i=0;i<n;i++)
		{
			if(i<n-1)
			{
				x2 = x.a[i+n*mx] + BarWidth*(x.a[i+1+n*mx]-x.a[i+n*mx])/2;
				y2 = y.a[i+n*my] + BarWidth*(y.a[i+1+n*my]-y.a[i+n*my])/2;
			}
			else	{	x2 = x.a[i+n*mx];	y2 = y.a[i+n*my];	}
			if(i>0)
			{
				x1 = x.a[i+n*mx] - BarWidth*(x.a[i+n*mx]-x.a[i-1+n*mx])/2;
				y1 = y.a[i+n*my] - BarWidth*(y.a[i+n*my]-y.a[i-1+n*my])/2;
			}
			else	{	x1 = x.a[i+n*mx];	y1 = y.a[i+n*my];	}
			zz = z.a[i+n*mz];
			if(!above)
			{
				x2 = (x2-x1)/m;		x1 += j*x2;		x2 += x1;
				y2 = (y2-y1)/m;		y1 += j*y2;		y2 += y1;
			}
			else
			{	z0 = GetOrgZ('x') + dd.a[i];	dd.a[i] += zz;	zz += z0;	}
			if(fall)	{	z0 = zp;	zz += z0;	zp = zz;	}

			c = (z.a[i+n*mz]<0) ? c1 : c2;
			cc[16*i]   = cc[16*i+4] = cc[16*i+8] = cc[16*i+12] = c.r;
			cc[16*i+1] = cc[16*i+5] = cc[16*i+9] = cc[16*i+13] = c.g;
			cc[16*i+2] = cc[16*i+6] = cc[16*i+10]= cc[16*i+14] = c.b;
			cc[16*i+3] = cc[16*i+7] = cc[16*i+11]= cc[16*i+15] = AlphaDef;

			pp[12*i+0] = x1;	pp[12*i+1] = y1;	pp[12*i+2] = zz;
			pp[12*i+3] = x1;	pp[12*i+4] = y1;	pp[12*i+5] = z0;
			pp[12*i+6] = x2;	pp[12*i+7] = y2;	pp[12*i+8] = z0;
			pp[12*i+9] = x2;	pp[12*i+10]= y2;	pp[12*i+11]= zz;
			tt[4*i]   = ScalePoint(pp[12*i+0],pp[12*i+1],pp[12*i+2]);
			tt[4*i+1] = ScalePoint(pp[12*i+3],pp[12*i+4],pp[12*i+5]);
			tt[4*i+2] = ScalePoint(pp[12*i+6],pp[12*i+7],pp[12*i+8]);
			tt[4*i+3] = ScalePoint(pp[12*i+9],pp[12*i+10],pp[12*i+11]);
		}
		quads_plot(n,pp,cc,tt);
		if(wire)	for(i=0;i<n;i++)
		{
			mreal ct[4]={0,0,0,1}, *pt = pp+12*i;
			if(tt[4*i] && tt[4*i+1])	line_plot(pt, pt+3, ct, ct);
			if(tt[4*i] && tt[4*i+3])	line_plot(pt, pt+9, ct, ct);
			if(tt[4*i+2] && tt[4*i+1])	line_plot(pt+6, pt+3, ct, ct);
			if(tt[4*i+2] && tt[4*i+3])	line_plot(pt+6, pt+9, ct, ct);
		}
	}
	SetPal(0);	EndGroup();
	delete []tt;	delete []pp;	delete []cc;
}
//-----------------------------------------------------------------------------
void mglGraph::Bars(const mglData &x, const mglData &y, const char *pen,mreal zVal)
{
	long i,j,m,mx,my,n=y.nx;
	if(x.nx!=n)	{	SetWarn(mglWarnDim,"Bars");	return;	}
	if(n<2)		{	SetWarn(mglWarnLow,"Bars");	return;	}
	static int cgid=1;	StartGroup("Bars",cgid++);
	m = x.ny > y.ny ? x.ny : y.ny;
	mreal *pp = new mreal[12*n],*cc = new mreal[16*n],x1,x2,yy,y0,yp;
	bool *tt = new bool[4*n], wire = pen && strchr(pen,'#');
	if(isnan(zVal))	zVal = Min.z;
	bool above = pen && strchr(pen,'a')!=0, fall = pen && strchr(pen,'f')!=0;
	if(above)	fall = false;
	mglData dd(above ? n : 1);

	mglColor c1,c2,c;
	SetPal(pen);
	for(j=0;j<m;j++)
	{
		c1 = GetPal();
		if(NumPal==2*m)	c2 = GetPal();	else	c2 = c1;
		mx = j<x.ny ? j:0;	my = j<y.ny ? j:0;	y0 = yp = GetOrgY('x');
		for(i=0;i<n;i++)
		{
			if(i<n-1)	x2 = x.a[i+n*mx] + BarWidth*(x.a[i+1+n*mx]-x.a[i+n*mx])/2;
			else		x2 = x.a[i+n*mx];
			if(i>0)		x1 = x.a[i+n*mx] - BarWidth*(x.a[i+n*mx]-x.a[i-1+n*mx])/2;
			else		x1 = x.a[i+n*mx];
			yy = y.a[i+n*my];
			if(!above)
			{	x2 = (x2-x1)/m;		x1 += j*x2;		x2 += x1;	}
			else
			{	y0 = GetOrgY('x') + dd.a[i];	dd.a[i] += yy;	yy += y0;	}
			if(fall)	{	y0 = yp;	yy += y0;	yp = yy;	}

			c = (y.a[i+n*my]<0) ? c1 : c2;
			cc[16*i]   = cc[16*i+4] = cc[16*i+8] = cc[16*i+12] = c.r;
			cc[16*i+1] = cc[16*i+5] = cc[16*i+9] = cc[16*i+13] = c.g;
			cc[16*i+2] = cc[16*i+6] = cc[16*i+10]= cc[16*i+14] = c.b;
			cc[16*i+3] = cc[16*i+7] = cc[16*i+11]= cc[16*i+15] = AlphaDef;

			pp[12*i+0] = x1;	pp[12*i+1] = yy;	pp[12*i+2] = zVal;
			pp[12*i+3] = x1;	pp[12*i+4] = y0;	pp[12*i+5] = zVal;
			pp[12*i+6] = x2;	pp[12*i+7] = y0;	pp[12*i+8] = zVal;
			pp[12*i+9] = x2;	pp[12*i+10]= yy;	pp[12*i+11]= zVal;
			tt[4*i]   = ScalePoint(pp[12*i+0],pp[12*i+1],pp[12*i+2]);
			tt[4*i+1] = ScalePoint(pp[12*i+3],pp[12*i+4],pp[12*i+5]);
			tt[4*i+2] = ScalePoint(pp[12*i+6],pp[12*i+7],pp[12*i+8]);
			tt[4*i+3] = ScalePoint(pp[12*i+9],pp[12*i+10],pp[12*i+11]);
		}
		quads_plot(n,pp,cc,tt);
		if(wire)	for(i=0;i<n;i++)
		{
			mreal ct[4]={0,0,0,1}, *pt = pp+12*i;
			if(tt[4*i] && tt[4*i+1])	line_plot(pt, pt+3, ct, ct);
			if(tt[4*i] && tt[4*i+3])	line_plot(pt, pt+9, ct, ct);
			if(tt[4*i+2] && tt[4*i+1])	line_plot(pt+6, pt+3, ct, ct);
			if(tt[4*i+2] && tt[4*i+3])	line_plot(pt+6, pt+9, ct, ct);
		}
	}
	SetPal(0);	EndGroup();
	delete []tt;	delete []pp;	delete []cc;
}
//-----------------------------------------------------------------------------
void mglGraph::Bars(const mglData &y, const char *pen,mreal zVal)
{
	if(y.nx<2)	{	SetWarn(mglWarnLow,"Bars");	return;	}
	mglData x(y.nx);
	x.Fill(Min.x,Max.x);
	Bars(x,y,pen,zVal);
}
//-----------------------------------------------------------------------------
//
//		Barh series
//
//-----------------------------------------------------------------------------
void mglGraph::Barh(const mglData &y, const mglData &v, const char *pen,mreal zVal)
{
	long i,j,m,mx,my,n=v.nx;
	if(y.nx!=n)	{	SetWarn(mglWarnDim,"Barh");	return;	}
	if(n<2)		{	SetWarn(mglWarnLow,"Barh");	return;	}
	static int cgid=1;	StartGroup("Barh",cgid++);
	m = y.ny > v.ny ? y.ny : v.ny;
	mreal *pp = new mreal[12*n],*cc = new mreal[16*n],y1,y2,xx,x0,xp;
	bool *tt = new bool[4*n], wire = pen && strchr(pen,'#');
	if(isnan(zVal))	zVal = Min.z;
	bool above = pen && strchr(pen,'a')!=0, fall = pen && strchr(pen,'f')!=0;
	if(above)	fall = false;
	mglData dd(above ? n : 1);

	mglColor c1,c2,c;
	SetPal(pen);
	for(j=0;j<m;j++)
	{
		c1 = GetPal();
		if(NumPal==2*m)	c2 = GetPal();	else	c2 = c1;
		my = j<y.ny ? j:0;	mx = j<v.ny ? j:0;	xp = x0 = GetOrgX('y');
		for(i=0;i<n;i++)
		{
			if(i<n-1)	y2 = y.a[i+n*my] + BarWidth*(y.a[i+1+n*my]-y.a[i+n*my])/2;
			else		y2 = y.a[i+n*my];
			if(i>0)		y1 = y.a[i+n*my] - BarWidth*(y.a[i+n*my]-y.a[i-1+n*my])/2;
			else		y1 = y.a[i+n*my];
			xx = v.a[i+n*mx];
			if(!above)
			{	y2 = (y2-y1)/m;		y1 += j*y2;		y2 += y1;	}
			else
			{	x0 = GetOrgX('y') + dd.a[i];	dd.a[i] += xx;	xx += x0;	}
			if(fall)	{	x0 = xp;	xx += x0;	xp = xx;	}

			c = (v.a[i+n*mx]<0) ? c1 : c2;
			cc[16*i]   = cc[16*i+4] = cc[16*i+8] = cc[16*i+12] = c.r;
			cc[16*i+1] = cc[16*i+5] = cc[16*i+9] = cc[16*i+13] = c.g;
			cc[16*i+2] = cc[16*i+6] = cc[16*i+10]= cc[16*i+14] = c.b;
			cc[16*i+3] = cc[16*i+7] = cc[16*i+11]= cc[16*i+15] = AlphaDef;

			pp[12*i+0] = xx;	pp[12*i+1] = y1;	pp[12*i+2] = zVal;
			pp[12*i+3] = xx;	pp[12*i+4] = y2;	pp[12*i+5] = zVal;
			pp[12*i+6] = x0;	pp[12*i+7] = y2;	pp[12*i+8] = zVal;
			pp[12*i+9] = x0;	pp[12*i+10]= y1;	pp[12*i+11]= zVal;
			tt[4*i]   = ScalePoint(pp[12*i+0],pp[12*i+1],pp[12*i+2]);
			tt[4*i+1] = ScalePoint(pp[12*i+3],pp[12*i+4],pp[12*i+5]);
			tt[4*i+2] = ScalePoint(pp[12*i+6],pp[12*i+7],pp[12*i+8]);
			tt[4*i+3] = ScalePoint(pp[12*i+9],pp[12*i+10],pp[12*i+11]);
		}
		quads_plot(n,pp,cc,tt);
		if(wire)	for(i=0;i<n;i++)
		{
			mreal ct[4]={0,0,0,1}, *pt = pp+12*i;
			if(tt[4*i] && tt[4*i+1])	line_plot(pt, pt+3, ct, ct);
			if(tt[4*i] && tt[4*i+3])	line_plot(pt, pt+9, ct, ct);
			if(tt[4*i+2] && tt[4*i+1])	line_plot(pt+6, pt+3, ct, ct);
			if(tt[4*i+2] && tt[4*i+3])	line_plot(pt+6, pt+9, ct, ct);
		}
	}
	SetPal(0);	EndGroup();
	delete []tt;	delete []pp;	delete []cc;
}
//-----------------------------------------------------------------------------
void mglGraph::Barh(const mglData &v, const char *pen,mreal zVal)
{
	if(v.nx<2)	{	SetWarn(mglWarnLow,"Barh");	return;	}
	mglData y(v.nx);
	y.Fill(Min.y,Max.y);
	Barh(y,v,pen,zVal);
}
//-----------------------------------------------------------------------------
//
//	Bars series
//
//-----------------------------------------------------------------------------
double sgn(double a);
int mgl_cmp_flt(const void *a, const void *b)
{
	const mreal *aa = (const mreal *)a;
	const mreal *bb = (const mreal *)b;
	return int(sgn(*aa-*bb));
}
//-----------------------------------------------------------------------------
void mglGraph::BoxPlot(const mglData &x, const mglData &y, const char *pen,mreal zVal)
{
	long n=y.nx, m=y.ny;
	if(x.nx!=n)	{	SetWarn(mglWarnDim,"BoxPlot");	return;	}
//	if(n<2)		{	SetWarn(mglWarnLow,"BoxPlot");	return;	}
	static int cgid=1;	StartGroup("BoxPlot",cgid++);
	mreal *b = new mreal[5*n], *pp = new mreal[6*9*n], *d = new mreal[m], x1, x2;
	bool *tt = new bool[2*9*n];
	if(isnan(zVal))	zVal = Min.z;
	register long i,j,i0;
	for(i=0;i<n;i++)	// find quartiles by itself
	{
		register long mm,k;
		for(mm=j=0;j<m;j++)	if(!isnan(y.a[i+n*j]))
		{	d[mm]=y.a[i+n*j];	mm++;	}
		if(m==0)	{	b[i]=NAN;	break;	}
		qsort(d, mm, sizeof(mreal), mgl_cmp_flt);
		b[i] = d[0];	b[i+4*n] = d[mm-1];		k = mm/4;
		b[i+n] = (mm%4) ? d[k] : (d[k]+d[k-1])/2.;
		b[i+2*n] = (mm%2) ? d[mm/2] : (d[mm/2]+d[mm/2-1])/2.;
		b[i+3*n] = (mm%4) ? d[mm-k-1] : (d[mm-k-1]+d[mm-k])/2.;
	}
	delete []d;

	SetPal(pen);//	DefColor(GetPal(), -1);
	mglColor c1=cmap[0], c2=cmap[1];
	cmap[0] = cmap[1] = GetPal();
	for(j=0;j<18*n;j++)	pp[2+3*j] = zVal;
	for(i=0;i<n;i++)
	{
		i0 = 54*i;
		if(i<n-1)	x2 = x.a[i] + BarWidth*(x.a[i+1]-x.a[i])/2;
		else		x2 = x.a[i];
		if(i>0)		x1 = x.a[i] - BarWidth*(x.a[i]-x.a[i-1])/2;
		else		x1 = x.a[i];
		// x-coordinates of lines
		pp[i0] = pp[i0+12]=pp[i0+18]=pp[i0+24]=pp[i0+30]=pp[i0+33]=pp[i0+48]=x1;
		pp[i0+3]=pp[i0+15]=pp[i0+21]=pp[i0+27]=pp[i0+36]=pp[i0+39]=pp[i0+51]=x2;
		pp[i0+6]=pp[i0+9] =pp[i0+42]=pp[i0+45]=x.a[i];
		// y-coordinates of lines
		pp[i0+1] = pp[i0+4] = pp[i0+7] = b[i];
		pp[i0+49] = pp[i0+52] = pp[i0+46] = b[i+4*n];
		pp[i0+19] = pp[i0+22] = b[i+2*n];
		pp[i0+10] = pp[i0+13] = pp[i0+16] = pp[i0+31] = pp[i0+37] = b[i+n];
		pp[i0+25] = pp[i0+28] = pp[i0+34] = pp[i0+40] = pp[i0+43] = b[i+3*n];
		if(isnan(b[i]) || isnan(b[i+n]) || isnan(b[i+2*n]) || isnan(b[i+3*n]) || isnan(b[i+4*n]))
			for(j=0;j<18;j++)	tt[j+18*i] = false;
		else	for(j=0;j<18;j++)
		{
			i0 = j+18*i;
			tt[i0] = ScalePoint(pp[3*i0],pp[3*i0+1],pp[3*i0+2]);
		}
	}
	lines_plot(9*n,pp,0,tt,false,false);
	cmap[0]=c1;	cmap[1]=c2;	SetPal(0);	EndGroup();
	delete []b;		delete []tt;	delete []pp;
}
//-----------------------------------------------------------------------------
void mglGraph::BoxPlot(const mglData &y, const char *pen,mreal zVal)
{
	if(y.nx<2)	{	SetWarn(mglWarnLow,"BoxPlot");	return;	}
	mglData x(y.nx);
	x.Fill(Min.x,Max.x);
	BoxPlot(x,y,pen,zVal);
}
//-----------------------------------------------------------------------------
//
//		Torus series
//
//-----------------------------------------------------------------------------
void mglGraph::Torus(const mglData &r, const mglData &z, const char *sch)
{
	long i,j,n=r.nx;
	if(r.nx*r.ny!=z.nx*z.ny){	SetWarn(mglWarnDim,"Torus");	return;	}
	if(n<2)					{	SetWarn(mglWarnLow,"Torus");	return;	}
	static int cgid=1;	StartGroup("Torus",cgid++);
	mreal *pp = new mreal[3*n];
	long *nn = new long[n];
	SetScheme(sch);
	if(sch)
	{
		if(strchr(sch,'x'))	AxialDir = 'x';
//		if(strchr(sch,'y'))	AxialDir = 'y';
		else if(strchr(sch,'z'))	AxialDir = 'z';
		else AxialDir = 'y';
	}

	Color(Cmin);
	memset(pp,0,3*n*sizeof(mreal));
	for(j=0;j<r.ny;j++)
	{
		for(i=0;i<n;i++)
		{
			nn[i] = i<n-1 ? i+1 : -1;
			pp[3*i]  = r.a[i+n*j];	pp[3*i+1]= z.a[i+n*j];
		}
		axial_plot(n,pp,nn,99,sch && strchr(sch,'#'));
	}
	EndGroup();
	delete []nn;	delete []pp;
}
//-----------------------------------------------------------------------------
void mglGraph::Torus(const mglData &y, const char *pen)
{
	if(y.nx<2)	{	SetWarn(mglWarnLow,"Torus");	return;	}
	mglData x(y.nx);
	x.Fill(Min.x,Max.x);
	Torus(x,y,pen);
}
//-----------------------------------------------------------------------------
//
//		Error series
//
//-----------------------------------------------------------------------------
void mglGraph::Error(const mglData &x, const mglData &y, const mglData &ex, const mglData &ey,
					const char *pen,mreal zVal)
{
	register long i,j,m=1,n=y.nx*y.ny*y.nz;
	if(ex.nx*ex.ny*ex.nz!=n || ey.nx*ey.ny*ey.nz!=n || y.nx!=x.nx)
	{	SetWarn(mglWarnDim,"Error");	return;	}
	static int cgid=1;	StartGroup("ErrorXY",cgid++);
	char mk=pen?SelectPen(pen):0;
	if(isnan(zVal))	zVal = Min.z;
	if(n!=x.nx*x.ny*x.nz)
	{	m = y.ny*y.nz;	n = y.nx;	}
	ArrowSize /=2;
	for(j=0;j<m;j++)
	{
		for(i=0;i<n;i++)
		{
			Line(mglPoint(x.a[i],y.a[i+n*j]+ey.a[i+n*j],zVal),
					mglPoint(x.a[i],y.a[i+n*j]-ey.a[i+n*j],zVal),"-II",100);
			Line(mglPoint(x.a[i]+ex.a[i+n*j],y.a[i+n*j],zVal),
					mglPoint(x.a[i]-ex.a[i+n*j],y.a[i+n*j],zVal),"-II",100);
			if(mk)	Mark(mglPoint(x.a[i],y.a[i+n*j],zVal),mk);
		}
	}
	ArrowSize *=2;
	EndGroup();
}
//-----------------------------------------------------------------------------
void mglGraph::Error(const mglData &x, const mglData &y, const mglData &ey, const char *pen,
					mreal zVal)
{
	if(y.nx*y.ny*y.nz!=ey.nx*ey.ny*ey.nz || y.nx!=x.nx)
	{	SetWarn(mglWarnDim,"Error");	return;	}
	static int cgid=1;	StartGroup("Error",cgid++);
	char mk=pen?SelectPen(pen):0;
	if(isnan(zVal))	zVal = Min.z;
	register long i,j,m,n;
	if(y.nx*y.ny*y.nz!=x.nx*x.ny*x.nz)
	{	m = y.ny*y.nz;	n = y.nx;	}
	else
	{	m = 1;	n = y.nx*y.ny*y.nz;	}
	ArrowSize /=2;
	for(j=0;j<m;j++)
	{
		for(i=0;i<n;i++)
		{
			Line(mglPoint(x.a[i],y.a[i+n*j]+ey.a[i+n*j],zVal),
					mglPoint(x.a[i],y.a[i+n*j]-ey.a[i+n*j],zVal),"-II",100);
			if(mk)	Mark(mglPoint(x.a[i],y.a[i+n*j],zVal),mk);
		}
	}
	ArrowSize *=2;
	EndGroup();
}
//-----------------------------------------------------------------------------
void mglGraph::Error(const mglData &y, const mglData &ey, const char *pen,mreal zVal)
{
	mglData x(y.nx);
	x.Fill(Min.x,Max.x);
	Error(x,y,ey,pen,zVal);
}
//-----------------------------------------------------------------------------
//
//		Chart series
//
//-----------------------------------------------------------------------------
void mglGraph::Chart(const mglData &a, const char *cols)
{
	if(a.Minimal()<0)	{	SetWarn(mglWarnNeg,"Chart");	return;	}
	static int cgid=1;	StartGroup("Chart",cgid++);
	bool wire = false;	// draw edges
	register long n=a.nx,i,j,k,l,m,i0;
	if(cols && !strcmp(cols,"#"))	{	wire = true;	cols = 0;	}
	if(!cols)	cols = MGL_DEF_PAL;
	mglColor *c = new mglColor[strlen(cols)+1];
	long nc=0;			// number of colors
	for(i=0;i<long(strlen(cols));i++)
	{
		if(strchr("wkrgbcymhRGBCYMHWlenuqpLENUQP ",cols[i]))
		{	c[nc].Set(cols[i]);	nc++;	}
		else if(cols[i]=='#')	wire = true;
	}

	mreal dy = (Max.y-Min.y)/a.ny, dx, ss, cs, x1, y1;
	mreal *pp = new mreal[3*40*81];
	for(j=0;j<a.ny;j++)
	{
		y1 = Min.y + dy*j;
		for(i=0,ss=0;i<n;i++)	ss += a.a[i+j*n];
		if(ss==0)	continue;
		for(cs=0,i=0;i<n;i++)
		{
			dx = a.a[i+j*n]/ss;	m = 2+long(38*dx+0.9);
			if(dx==0)	continue;
			x1 = Min.x + (Max.x-Min.x)*cs/ss;	dx *= (Max.x-Min.x);
			if(!c[i%nc].Valid())	{	cs += a.a[i+j*n];	continue;	}
			DefColor(c[i%nc],Transparent ? AlphaDef : 1);
			for(k=0;k<40;k++)	for(l=0;l<m;l++)
			{
				i0 = 3*(m*k+l);
				pp[i0]=x1+dx*l/(m-1.);	pp[i0+1]=y1+dy*k/39.;	pp[i0+2]=Max.z;
				ScalePoint(pp[i0],pp[i0+1],pp[i0+2]);
				i0 = 3*(m*(79-k)+l);
				pp[i0]=x1+dx*l/(m-1.);	pp[i0+1]=y1+dy*k/39.;	pp[i0+2]=Min.z;
				ScalePoint(pp[i0],pp[i0+1],pp[i0+2]);
			}
			memcpy(pp+3*80*m,pp,3*m*sizeof(mreal));
			surf_plot(m,81,pp,0,0);
			for(k=0;k<40;k++)
			{
				i0 = 6*k;
				pp[i0] = x1;	pp[i0+1] = y1+dy*k/39.;	pp[i0+2] = Max.z;
				ScalePoint(pp[i0],pp[i0+1],pp[i0+2]);
				pp[i0+3] = x1;	pp[i0+4] = y1+dy*k/39.;	pp[i0+5] = Min.z;
				ScalePoint(pp[i0+3],pp[i0+4],pp[i0+5]);
			}
			surf_plot(2,40,pp,0,0);
			for(k=0;k<40;k++)
			{
				i0 = 6*k;
				pp[i0] = x1+dx;		pp[i0+1] = y1+dy*k/39.;	pp[i0+2] = Max.z;
				ScalePoint(pp[i0],pp[i0+1],pp[i0+2]);
				pp[i0+3] = x1+dx;	pp[i0+4] = y1+dy*k/39.;	pp[i0+5] = Min.z;
				ScalePoint(pp[i0+3],pp[i0+4],pp[i0+5]);
			}
			surf_plot(2,40,pp,0,0);
			if(wire)
			{
				Line(mglPoint(x1,y1,Max.z),mglPoint(x1,y1,Min.z),"k-",2);
				Line(mglPoint(x1+dx,y1,Max.z),mglPoint(x1+dx,y1,Min.z),"k-",2);
				Line(mglPoint(x1,y1+dy,Max.z),mglPoint(x1,y1+dy,Min.z),"k-",2);
				Line(mglPoint(x1+dx,y1+dy,Max.z),mglPoint(x1+dx,y1+dy,Min.z),"k-",2);

				Line(mglPoint(x1,y1,Max.z),mglPoint(x1+dx,y1,Max.z),"k-",40);
				Line(mglPoint(x1,y1,Max.z),mglPoint(x1,y1+dy,Max.z),"k-",40);
				Line(mglPoint(x1+dx,y1+dy,Max.z),mglPoint(x1+dx,y1,Max.z),"k-",40);
				Line(mglPoint(x1+dx,y1+dy,Max.z),mglPoint(x1,y1+dy,Max.z),"k-",40);

				Line(mglPoint(x1,y1,Min.z),mglPoint(x1+dx,y1,Min.z),"k-",40);
				Line(mglPoint(x1,y1,Min.z),mglPoint(x1,y1+dy,Min.z),"k-",40);
				Line(mglPoint(x1+dx,y1+dy,Min.z),mglPoint(x1+dx,y1,Min.z),"k-",40);
				Line(mglPoint(x1+dx,y1+dy,Min.z),mglPoint(x1,y1+dy,Min.z),"k-",40);
			}
			cs += a.a[i+j*n];
		}
	}
	EndGroup();		delete []pp;	delete []c;
}
//-----------------------------------------------------------------------------
//
//	Mark series
//
//-----------------------------------------------------------------------------
void mglGraph::Mark(const mglData &x, const mglData &y, const mglData &z, const mglData &r, const char *pen)
{
	long j,m,mx,my,mz,mr,n=y.nx;
	char mk=0;
	if(x.nx!=n || z.nx!=n || r.nx!=n)
	{	SetWarn(mglWarnDim,"Mark");	return;	}
	if(n<2)	{	SetWarn(mglWarnLow,"Mark");	return;	}
	static int cgid=1;	StartGroup("Mark",cgid++);
	m = x.ny > y.ny ? x.ny : y.ny;	m = z.ny > m ? z.ny : m;
	mreal ms = MarkSize, xx,yy,zz;
	bool tt;

	if(pen && *pen)	mk=SelectPen(pen);	else mk='o';
	if(mk==0)	return;
	SetPal(pen);
	for(j=0;j<m;j++)
	{
		Pen(GetPal(),0,0);
		mx = j<x.ny ? j:0;	my = j<y.ny ? j:0;
		mz = j<z.ny ? j:0;	mr = j<r.ny ? j:0;
		register long i;
		for(i=0;i<n;i++)
		{
			xx = x.a[i+mx*n];	yy = y.a[i+my*n];	zz = z.a[i+mz*n];
			tt = ScalePoint(xx,yy,zz);
			MarkSize = ms*fabs(r.a[i+mr*n]);
			if(tt)	Mark(xx,yy,zz,mk);
		}
	}
	MarkSize = ms;	SetPal(0);	EndGroup();
}
//-----------------------------------------------------------------------------
void mglGraph::Mark(const mglData &x, const mglData &y, const mglData &r, const char *pen,mreal zVal)
{
	mglData z(y.nx);
	if(isnan(zVal))	zVal = Min.z;
	z.Fill(zVal,zVal);
	Mark(x,y,z,r,pen);
}
//-----------------------------------------------------------------------------
void mglGraph::Mark(const mglData &y, const mglData &r, const char *pen,mreal zVal)
{
	if(y.nx<2)	{	SetWarn(mglWarnLow,"Mark");	return;	}
	mglData x(y.nx);
	x.Fill(Min.x,Max.x);
	Mark(x,y,r,pen,zVal);
}
//-----------------------------------------------------------------------------
//
//	Mark series
//
//-----------------------------------------------------------------------------
void mglGraph::TextMark(const mglData &x, const mglData &y, const mglData &z, const mglData &r, const wchar_t *text, const char *fnt)
{
	long j,m,mx,my,mz,mr,n=y.nx;
	if(x.nx!=n || z.nx!=n || r.nx!=n)
	{	SetWarn(mglWarnDim,"Mark");	return;	}
	if(n<2)	{	SetWarn(mglWarnLow,"Mark");	return;	}
	static int cgid=1;	StartGroup("TextMark",cgid++);
	m = x.ny > y.ny ? x.ny : y.ny;	m = z.ny > m ? z.ny : m;

	for(j=0;j<m;j++)
	{
		mx = j<x.ny ? j:0;	my = j<y.ny ? j:0;
		mz = j<z.ny ? j:0;	mr = j<r.ny ? j:0;
		register long i;
		for(i=0;i<n;i++)
			Putsw(mglPoint(x.a[i+mx*n],y.a[i+my*n],z.a[i+mz*n]),text,
				  fnt?fnt:"rC",-25*MarkSize*fabs(r.a[i+mr*n]));
	}
	EndGroup();
}
//-----------------------------------------------------------------------------
void mglGraph::TextMark(const mglData &x, const mglData &y, const mglData &r, const wchar_t *text, const char *fnt,mreal zVal)
{
	mglData z(y.nx);
	if(isnan(zVal))	zVal = Min.z;
	z.Fill(zVal,zVal);
	TextMark(x,y,z,r,text,fnt);
}
//-----------------------------------------------------------------------------
void mglGraph::TextMark(const mglData &y, const mglData &r, const wchar_t *text, const char *fnt,mreal zVal)
{
	if(y.nx<2)	{	SetWarn(mglWarnLow,"Mark");	return;	}
	mglData x(y.nx);
	x.Fill(Min.x,Max.x);
	TextMark(x,y,r,text,fnt,zVal);
}
//-----------------------------------------------------------------------------
void mglGraph::TextMark(const mglData &y, const wchar_t *text, const char *fnt,mreal zVal)
{
	mglData r(y.nx);	r.Fill(1,1);
	TextMark(y,r,text,fnt,zVal);
}
//-----------------------------------------------------------------------------
//
//	Tube series
//
//-----------------------------------------------------------------------------
void mglGraph::tube_plot(long n,mreal *q,mreal *c,mreal *r)
{
	if(n<2 || CirclePnts<3)	return;
	mreal *pp = new mreal[3*CirclePnts*n], *cc = new mreal[4*CirclePnts*n];
	mreal lx,ly,lz,len,gx=0,gy=0,gz=1,hx,hy,hz;
	mreal xx, yy, zz, rr;
	bool *tt = new bool[CirclePnts*n],tl;
	register long i,k,j,i0;
	long m=n;
	for(j=-1,i=0;i<n;i++)
	{
		xx = q[i*3]; yy = q[i*3+1]; zz = q[i*3+2];
		tl = ScalePoint(xx,yy,zz);
		xx = q[i*3]; yy = q[i*3+1]; zz = q[i*3+2];	rr = r[i];
		if(i>0)
		{	lx = xx-q[i*3-3];	ly = yy-q[i*3-2];	lz = zz-q[i*3-1];	}
		else
		{	lx = q[i*3+3]-xx;	ly = q[i*3+4]-yy;	lz = q[i*3+5]-zz;	}
		len = sqrt(lx*lx+ly*ly+lz*lz);
		if(len==0)	{	m--;	continue;	}	else	j++;
		lx /= len;	ly /= len;	lz /= len;
		if(i>0)
		{
			len = gx*lx+gy*ly+gz*lz;
			gx -= lx*len;	gy -= ly*len;	gz -= lz*len;
			len = sqrt(gx*gx+gy*gy+gz*gz);
			gx /= len;		gy /= len;		gz /= len;
		}
		else
		{
			if(lx==0 && ly==0)
			{	gx = -lz/hypot(lx,lz);	gz = lx/hypot(lx,lz);	gy=0;	}
			else
			{	gx = ly/hypot(lx,ly);	gy = -lx/hypot(lx,ly);	gz=0;	}
		}
		hx = ly*gz-lz*gy;	hy = lz*gx-lx*gz;	hz = lx*gy-ly*gx;
		for(k=0;k<CirclePnts;k++)
		{
			i0 = k+CirclePnts*j;
			lx = rr*cos(2*M_PI*k/(CirclePnts-1.));
			ly = rr*sin(2*M_PI*k/(CirclePnts-1.));
			pp[3*i0] = xx + gx*lx + hx*ly;
			pp[3*i0+1]=yy + gy*lx + hy*ly;
			pp[3*i0+2]=zz + gz*lx + hz*ly;
			if(c)	memcpy(cc+4*i0,c+4*i,4*sizeof(mreal));
			tt[i0] = tl;
			ScalePoint(pp[3*i0],pp[3*i0+1],pp[3*i0+2]);
		}
	}
	int mnt=mglNumThr;	mglNumThr = 1;	// too many faces
	surf_plot(CirclePnts,m,pp,c?cc:0,tt);
	mglNumThr = mnt;
	delete []pp;	delete []cc;	delete []tt;
}
//-----------------------------------------------------------------------------
void mglGraph::Tube(const mglData &x, const mglData &y, const mglData &z, const mglData &r, const char *pen)
{
	long j,m,mx,my,mz,mr,n=y.nx;
	if(n<2)	{	SetWarn(mglWarnLow,"Tube");	return;	}
	if(x.nx!=n || z.nx!=n || r.nx!=n)
	{	SetWarn(mglWarnDim,"Tube");	return;	}
	static int cgid=1;	StartGroup("Tube",cgid++);
	m = x.ny > y.ny ? x.ny : y.ny;	m = z.ny > m ? z.ny : m;	m = r.ny > m ? r.ny : m;
	mreal *pp = new mreal[3*n], *rr = new mreal[n];

	SelectPen(pen);	SetPal(pen);
	for(j=0;j<m;j++)
	{
		DefColor(GetPal(), -1);
		mx = j<x.ny ? j:0;	my = j<y.ny ? j:0;
		mz = j<z.ny ? j:0;	mr = j<r.ny ? j:0;
		register long i;

		for(i=0;i<n;i++)
		{
			pp[3*i] = x.a[i+mx*n];		pp[3*i+1] = y.a[i+my*n];
			pp[3*i+2] = z.a[i+mz*n];	rr[i] = r.a[i+mr*n];
		}
		tube_plot(n,pp,0,rr);
	}
	SetPal(0);	EndGroup();
	delete []pp;	delete []rr;
}
//-----------------------------------------------------------------------------
void mglGraph::Tube(const mglData &x, const mglData &y, const mglData &r, const char *pen,mreal zVal)
{
	mglData z(y.nx);
	if(isnan(zVal))	zVal = Min.z;
	z.Fill(zVal,zVal);
	Tube(x,y,z,r,pen);
}
//-----------------------------------------------------------------------------
void mglGraph::Tube(const mglData &y, const mglData &r, const char *pen,mreal zVal)
{
	if(y.nx<2)	{	SetWarn(mglWarnLow,"Tube");	return;	}
	mglData x(y.nx);
	x.Fill(Min.x,Max.x);
	Tube(x,y,r,pen,zVal);
}
//-----------------------------------------------------------------------------
void mglGraph::Tube(const mglData &y, mreal rr, const char *pen,mreal zVal)
{
	if(y.nx<2)	{	SetWarn(mglWarnLow,"Tube");	return;	}
	mglData x(y.nx), r(y.nx);
	x.Fill(Min.x,Max.x);
	r.Fill(rr,rr);
	Tube(x,y,r,pen,zVal);
}
//-----------------------------------------------------------------------------
void mglGraph::Tube(const mglData &x, const mglData &y, mreal rr, const char *pen,mreal zVal)
{
	if(y.nx<2)	{	SetWarn(mglWarnLow,"Tube");	return;	}
	mglData r(y.nx);
	r.Fill(rr,rr);
	Tube(x,y,r,pen,zVal);
}
//-----------------------------------------------------------------------------
void mglGraph::Tube(const mglData &x, const mglData &y, const mglData &z, mreal rr, const char *pen)
{
	if(y.nx<2)	{	SetWarn(mglWarnLow,"Tube");	return;	}
	mglData r(y.nx);
	r.Fill(rr,rr);
	Tube(x,y,z,r,pen);
}
//-----------------------------------------------------------------------------
//		1D plotting functions
//-----------------------------------------------------------------------------
/// Draw line plot for points in arrays \a x, \a y, \a z.
void mgl_plot_xyz(HMGL gr, const HMDT x, const HMDT y, const HMDT z, const char *pen)
{	if(gr && x && y && z)	gr->Plot(*x,*y,*z,pen);	}
/// Draw line plot for points in arrays \a x, \a y.
void mgl_plot_xy(HMGL gr, const HMDT x, const HMDT y, const char *pen)
{	if(gr && x && y)	gr->Plot(*x,*y,pen);	}
/// Draw line plot for points in arrays \a y.
void mgl_plot(HMGL gr, const HMDT y, const char *pen)
{	if(gr && y)	gr->Plot(*y,pen);	}
/// Draw boxplot for points in arrays \a x, \a y.
void mgl_boxplot_xy(HMGL gr, const HMDT x, const HMDT y, const char *pen)
{	if(gr && x && y)	gr->BoxPlot(*x,*y,pen);	}
/// Draw bozplot for points in arrays \a y.
void mgl_boxplot(HMGL gr, const HMDT y, const char *pen)
{	if(gr && y)	gr->BoxPlot(*y,pen);	}
/// Draw radar plot for points in arrays \a y.
void mgl_radar(HMGL gr, const HMDT a, const char *pen, mreal r)
{	if(gr && a)	gr->Radar(*a,pen,r);	}
/// Draw line plot for points in arrays \a x, \a y, \a z.
void mgl_tens_xyz(HMGL gr, const HMDT x, const HMDT y, const HMDT z, const HMDT c, const char *pen)
{	if(gr && x && y && z)	gr->Tens(*x,*y,*z,*c,pen);	}
/// Draw line plot for points in arrays \a x, \a y.
void mgl_tens_xy(HMGL gr, const HMDT x, const HMDT y, const HMDT c, const char *pen)
{	if(gr && x && y)	gr->Tens(*x,*y,*c,pen);	}
/// Draw line plot for points in arrays \a y.
void mgl_tens(HMGL gr, const HMDT y, const HMDT c, const char *pen)
{	if(gr && y)	gr->Tens(*y,*c,pen);	}
/// Draw area plot for points in arrays \a x, \a y, \a z.
void mgl_area_xyz(HMGL gr, const HMDT x, const HMDT y, const HMDT z, const char *pen)
{	if(gr && x && y && z)	gr->Area(*x,*y,*z,pen);	}
/// Draw area plot for points in arrays \a x, \a y.
void mgl_area_xy(HMGL gr, const HMDT x, const HMDT y, const char *pen)
{	if(gr && x && y)	gr->Area(*x,*y,pen);	}
/// Draw area plot for points in arrays \a y.
void mgl_area(HMGL gr, const HMDT y, const char *pen)
{	if(gr && y)	gr->Area(*y,pen);	}
/// Fill area between curves \a y1, \a y2 parametrically dependent on \a x.
void mgl_region_xy(HMGL gr, const HMDT x, const HMDT y1, const HMDT y2, const char *pen, int inside)
{	if(gr && x && y1 && y2)	gr->Region(*x,*y1,*y2,pen,NAN,inside);	}
/// Fill area between curves \a y1, \a y2.
void mgl_region(HMGL gr, const HMDT y1, const HMDT y2, const char *pen, int inside)
{	if(gr && y1 && y2)	gr->Region(*y1,*y2,pen,NAN,inside);	}
/// draw mark with different type at position {x,y,z}
void mgl_mark(HMGL gr, mreal x,mreal y,mreal z,char mark)
{	gr->Mark(mglPoint(x,y,z),mark);	}
/// Draw vertical lines from points in arrays \a x, \a y, \a z to mglGraph::Org.
void mgl_stem_xyz(HMGL gr, const HMDT x, const HMDT y, const HMDT z, const char *pen)
{	if(gr && x && y && z)	gr->Stem(*x,*y,*z,pen);	}
/// Draw vertical lines from points in arrays \a x, \a y to mglGraph::Org.
void mgl_stem_xy(HMGL gr, const HMDT x, const HMDT y, const char *pen)
{	if(gr && x && y)	gr->Stem(*x,*y,pen);	}
/// Draw vertical lines from points in arrays \a y to mglGraph::Org.
void mgl_stem(HMGL gr, const HMDT y,	const char *pen)
{	if(gr && y)	gr->Stem(*y,pen);	}
/// Draw stairs for points in arrays \a x, \a y, \a z.
void mgl_step_xyz(HMGL gr, const HMDT x, const HMDT y, const HMDT z, const char *pen)
{	if(gr && x && y && z)	gr->Step(*x,*y,*z,pen);	}
/// Draw stairs for points in arrays \a x, \a y.
void mgl_step_xy(HMGL gr, const HMDT x, const HMDT y, const char *pen)
{	if(gr && x && y)	gr->Step(*x,*y,pen);	}
/// Draw line plot for points in arrays \a y.
void mgl_step(HMGL gr, const HMDT y,	const char *pen)
{	if(gr && y)	gr->Step(*y,pen);	}
/// Draw vertical bars from points in arrays \a x, \a y, \a z to mglGraph::Org.
void mgl_bars_xyz(HMGL gr, const HMDT x, const HMDT y, const HMDT z, const char *pen)
{	if(gr && x && y && z)	gr->Bars(*x,*y,*z,pen);	}
/// Draw vertical bars from points in arrays \a x, \a y to mglGraph::Org.
void mgl_bars_xy(HMGL gr, const HMDT x, const HMDT y, const char *pen)
{	if(gr && x && y)	gr->Bars(*x,*y,pen);	}
/// Draw vertical bars from points in arrays \a y to mglGraph::Org.
void mgl_bars(HMGL gr, const HMDT y,	const char *pen)
{	if(gr && y)	gr->Bars(*y,pen);	}
/// Draw vertical bars from points in arrays \a x, \a y to mglGraph::Org.
void mgl_barh_yx(HMGL gr, const HMDT y, const HMDT v, const char *pen)
{	if(gr && v && y)	gr->Barh(*y,*v,pen);	}
/// Draw vertical bars from points in arrays \a y to mglGraph::Org.
void mgl_barh(HMGL gr, const HMDT v,	const char *pen)
{	if(gr && v)	gr->Barh(*v,pen);	}
/// Draw surface of curve {\a r,\a z} rotatation around Z axis
void mgl_torus(HMGL gr, const HMDT r, const HMDT z, const char *pen)
{	if(gr && r && z)	gr->Torus(*r, *z, pen);	}
//-----------------------------------------------------------------------------
/// Draw chart for data a
void mgl_chart(HMGL gr, const HMDT a, const char *col)
{	if(gr&&a)	gr->Chart(*a, col);	}
/// Draw error boxes ey for data y
void mgl_error(HMGL gr, const HMDT y, const HMDT ey, const char *pen)
{	if(gr&&y&&ey)	gr->Error(*y,*ey,pen);	}
/// Draw error boxes ey for data {x,y}
void mgl_error_xy(HMGL gr, const HMDT x, const HMDT y, const HMDT ey, const char *pen)
{	if(gr&&x&&y&&ey)	gr->Error(*x,*y,*ey,pen);	}
/// Draw error boxes {ex,ey} for data {x,y}
void mgl_error_exy(HMGL gr, const HMDT x, const HMDT y, const HMDT ex, const HMDT ey, const char *pen)
{	if(gr&&x&&ex&&y&&ey)	gr->Error(*x,*y,*ex,*ey,pen);	}
/// Draw marks with diffenernt sizes \a r for points in arrays \a x, \a y, \a z.
void mgl_mark_xyz(HMGL gr, const HMDT x, const HMDT y, const HMDT z, const HMDT r, const char *pen)
{	if(gr&&x&&y&&z&&r)	gr->Mark(*x,*y,*z,*r,pen);	}
/// Draw marks with diffenernt sizes \a r for points in arrays \a x, \a y.
void mgl_mark_xy(HMGL gr, const HMDT x, const HMDT y, const HMDT r, const char *pen)
{	if(gr&&x&&y&&r)	gr->Mark(*x,*y,*r,pen);	}
/// Draw marks with diffenernt sizes \a r for points in arrays \a y.
void mgl_mark_y(HMGL gr, const HMDT y, const HMDT r, const char *pen)
{	if(gr&&y&&r)	gr->Mark(*y,*r,pen);	}
/// Draw tube with radial sizes \a r for points in arrays \a x, \a y, \a z.
void mgl_tube_xyzr(HMGL gr, const HMDT x, const HMDT y, const HMDT z, const HMDT r, const char *pen)
{	if(gr&&x&&y&&z&&r)	gr->Tube(*x,*y,*z,*r,pen);	}
/// Draw tube with radial sizes \a r for points in arrays \a x, \a y.
void mgl_tube_xyr(HMGL gr, const HMDT x, const HMDT y, const HMDT r, const char *pen)
{	if(gr&&x&&y&&r)	gr->Tube(*x,*y,*r,pen);	}
/// Draw tube with radial sizes \a r for points in arrays \a y.
void mgl_tube_r(HMGL gr, const HMDT y, const HMDT r, const char *pen)
{	if(gr&&y&&r)	gr->Tube(*y,*r,pen);	}
/// Draw tube with constant radial sizes \a r for points in arrays \a x, \a y, \a z.
void mgl_tube_xyz(HMGL gr, const HMDT x, const HMDT y, const HMDT z, mreal r, const char *pen)
{	if(gr&&x&&y&&z)	gr->Tube(*x,*y,*z,r,pen);	}
/// Draw tube with constant radial sizes \a r for points in arrays \a x, \a y.
void mgl_tube_xy(HMGL gr, const HMDT x, const HMDT y, mreal r, const char *pen)
{	if(gr&&x&&y)	gr->Tube(*x,*y,r,pen);	}
/// Draw tube with constant radial sizes \a r for points in arrays \a y.
void mgl_tube(HMGL gr, const HMDT y, mreal r, const char *pen)
{	if(gr&&y)	gr->Tube(*y,r,pen);	}
//-----------------------------------------------------------------------------
//		1D plotting functions (Fortran)
//-----------------------------------------------------------------------------
/// Draw line plot for points in arrays \a x, \a y, \a z.
void mgl_plot_xyz_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, const char *pen,int l)
{
	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	if(gr && x && y && z)	_GR_->Plot(_D_(x),_D_(y),_D_(z),s);
	delete []s;
}
/// Draw line plot for points in arrays \a x, \a y.
void mgl_plot_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, const char *pen,int l)
{
	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	if(gr && x && y)	_GR_->Plot(_D_(x),_D_(y),s);
	delete []s;
}
/// Draw line plot for points in arrays \a y.
void mgl_plot_(uintptr_t *gr, uintptr_t *y,	const char *pen,int l)
{
	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	if(gr && y)	_GR_->Plot(_D_(y),s);
	delete []s;
}
/// Draw boxplot for points in arrays \a x, \a y.
void mgl_boxplot_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, const char *pen,int l)
{
	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	if(gr && x && y)	_GR_->BoxPlot(_D_(x),_D_(y),s);
	delete []s;
}
/// Draw boxplot for points in arrays \a y.
void mgl_boxplot_(uintptr_t *gr, uintptr_t *y,	const char *pen,int l)
{
	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	if(gr && y)	_GR_->BoxPlot(_D_(y),s);
	delete []s;
}
/// Draw radar plot for points in arrays \a y.
void mgl_radar_(uintptr_t *gr, uintptr_t *a, const char *pen, mreal *r, int l)
{
	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	if(gr && a)	_GR_->Radar(_D_(a),s,*r);
	delete []s;
}
/// Draw line plot for points in arrays \a x, \a y, \a z.
void mgl_tens_xyz_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *c, const char *pen,int l)
{
	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	if(gr && x && y && z)	_GR_->Tens(_D_(x),_D_(y),_D_(z),_D_(c),s);
	delete []s;
}
/// Draw line plot for points in arrays \a x, \a y.
void mgl_tens_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *c, const char *pen,int l)
{
	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	if(gr && x && y)	_GR_->Tens(_D_(x),_D_(y),_D_(c),s);
	delete []s;
}
/// Draw line plot for points in arrays \a y.
void mgl_tens_(uintptr_t *gr, uintptr_t *y, uintptr_t *c, const char *pen,int l)
{
	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	if(gr && y)	_GR_->Tens(_D_(y),_D_(c),s);
	delete []s;
}

/// Draw area plot for points in arrays \a x, \a y, \a z.
void mgl_area_xyz_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, const char *pen,int l)
{
	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	if(gr && x && y && z)	_GR_->Area(_D_(x),_D_(y),_D_(z),s);
	delete []s;
}
/// Draw area plot for points in arrays \a x, \a y.
void mgl_area_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, const char *pen,int l)
{
	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	if(gr && x && y)	_GR_->Area(_D_(x),_D_(y),s);
	delete []s;
}
/// Draw area plot for points in arrays \a y.
void mgl_area_(uintptr_t *gr, uintptr_t *y, const char *pen,int l)
{
	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	if(gr && y)	_GR_->Area(_D_(y),s);
	delete []s;
}
/// Fill area between the curves \a y1, \a y2 parametrically dependent on \a x.
void mgl_region_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y1, uintptr_t *y2, const char *pen, int *inside, int l)
{
	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	if(gr && x && y1 && y2)	_GR_->Region(_D_(x),_D_(y1),_D_(y2),s,NAN,*inside);
	delete []s;
}
/// Fill area between the curves \a y1, \a y2.
void mgl_region_(uintptr_t *gr, uintptr_t *y1, uintptr_t *y2, const char *pen, int *inside, int l)
{
	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	if(gr && y1 && y2)	_GR_->Region(_D_(y1),_D_(y2),s,NAN,*inside);
	delete []s;
}
/// draw mark with different type at position {x,y,z}
void mgl_mark_(uintptr_t *gr, mreal *x,mreal *y,mreal *z,const char *mark,int)
{	_GR_->Mark(mglPoint(*x,*y,*z),*mark);	}
/// Draw vertical lines from points in arrays \a x, \a y, \a z to mglGraph::Org.
void mgl_stem_xyz_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, const char *pen,int l)
{
	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	if(gr && x && y && z)	_GR_->Stem(_D_(x),_D_(y),_D_(z),s);
	delete []s;
}
/// Draw vertical lines from points in arrays \a x, \a y to mglGraph::Org.
void mgl_stem_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, const char *pen,int l)
{
	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	if(gr && x && y)	_GR_->Stem(_D_(x),_D_(y),s);
	delete []s;
}
/// Draw vertical lines from points in arrays \a y to mglGraph::Org.
void mgl_stem_(uintptr_t *gr, uintptr_t *y,	const char *pen,int l)
{
	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	if(gr && y)	_GR_->Stem(_D_(y),s);
	delete []s;
}
/// Draw stairs for points in arrays \a x, \a y, \a z.
void mgl_step_xyz_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, const char *pen,int l)
{
	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	if(gr && x && y && z)	_GR_->Step(_D_(x),_D_(y),_D_(z),s);
	delete []s;
}
/// Draw stairs for points in arrays \a x, \a y.
void mgl_step_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, const char *pen,int l)
{
	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	if(gr && x && y)	_GR_->Step(_D_(x),_D_(y),s);
	delete []s;
}
/// Draw line plot for points in arrays \a y.
void mgl_step_(uintptr_t *gr, uintptr_t *y,	const char *pen,int l)
{
	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	if(gr && y)	_GR_->Step(_D_(y),s);
	delete []s;
}
/// Draw vertical bars from points in arrays \a x, \a y, \a z to mglGraph::Org.
void mgl_bars_xyz_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, const char *pen,int l)
{
	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	if(gr && x && y && z)	_GR_->Bars(_D_(x),_D_(y),_D_(z),s);
	delete []s;
}
/// Draw vertical bars from points in arrays \a x, \a y to mglGraph::Org.
void mgl_bars_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, const char *pen,int l)
{
	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	if(gr && x && y)	_GR_->Bars(_D_(x),_D_(y),s);
	delete []s;
}
/// Draw vertical bars from points in arrays \a y to mglGraph::Org.
void mgl_bars_(uintptr_t *gr, uintptr_t *y,	const char *pen,int l)
{
	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	if(gr && y)	_GR_->Bars(_D_(y),s);
	delete []s;
}
/// Draw vertical bars from points in arrays \a x, \a y to mglGraph::Org.
void mgl_barh_yx_(uintptr_t *gr, uintptr_t *y, uintptr_t *v, const char *pen,int l)
{
	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	if(gr && v && y)	_GR_->Barh(_D_(y),_D_(v),s);
	delete []s;
}
/// Draw vertical bars from points in arrays \a y to mglGraph::Org.
void mgl_barh_(uintptr_t *gr, uintptr_t *v,	const char *pen,int l)
{
	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	if(gr && v)	_GR_->Barh(_D_(v),s);
	delete []s;
}
/// Draw surface of curve {\a r,\a z} rotatation around Z axis
void mgl_torus_(uintptr_t *gr, uintptr_t *r, uintptr_t *z, const char *pen,int l)
{
	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	if(gr && r && z)	_GR_->Torus(_D_(r), _D_(z), s);
	delete []s;
}
//-----------------------------------------------------------------------------
/// Draw chart for data a
void mgl_chart_(uintptr_t *gr, uintptr_t *a, const char *col,int l)
{
	char *s=new char[l+1];	memcpy(s,col,l);	s[l]=0;
	if(gr&&a)	_GR_->Chart(_D_(a), s);
	delete []s;
}
/// Draw error boxes ey for data y
void mgl_error_(uintptr_t *gr, uintptr_t *y, uintptr_t *ey, const char *pen,int l)
{
	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	if(gr&&y&&ey)	_GR_->Error(_D_(y),_D_(ey),s);
	delete []s;
}
/// Draw error boxes ey for data {x,y}
void mgl_error_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *ey, const char *pen,int l)
{
	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	if(gr&&x&&y&&ey)	_GR_->Error(_D_(x),_D_(y),_D_(ey),s);
	delete []s;
}
/// Draw error boxes {ex,ey} for data {x,y}
void mgl_error_exy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *ex, uintptr_t *ey, const char *pen,int l)
{
	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	if(gr&&x&&ex&&y&&ey)	_GR_->Error(_D_(x),_D_(y),_D_(ex),_D_(ey),s);
	delete []s;
}
/// Draw marks with diffenernt sizes \a r for points in arrays \a x, \a y, \a z.
void mgl_mark_xyz_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *r, const char *pen,int l)
{
	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	if(gr&&x&&y&&z&&r)	_GR_->Mark(_D_(x),_D_(y),_D_(z), _D_(r),s);
	delete []s;
}
/// Draw marks with diffenernt sizes \a r for points in arrays \a x, \a y.
void mgl_mark_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *r, const char *pen,int l)
{
	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	if(gr&&x&&y&&r)	_GR_->Mark(_D_(x), _D_(y), _D_(r),s);
	delete []s;
}
/// Draw marks with diffenernt sizes \a r for points in arrays \a y.
void mgl_mark_y_(uintptr_t *gr, uintptr_t *y, uintptr_t *r, const char *pen,int l)
{
	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	if(gr&&y&&r)	_GR_->Mark(_D_(y),_D_(r),s);
	delete []s;
}
/// Draw tube with radial sizes \a r for points in arrays \a x, \a y, \a z.
void mgl_tube_xyzr_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *r, const char *pen,int l)
{
	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	if(gr&&x&&y&&z&&r)	_GR_->Tube(_D_(x),_D_(y),_D_(z), _D_(r),s);
	delete []s;
}
/// Draw tube with radial sizes \a r for points in arrays \a x, \a y.
void mgl_tube_xyr_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *r, const char *pen,int l)
{
	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	if(gr&&x&&y&&r)	_GR_->Tube(_D_(x),_D_(y),_D_(r),s);
	delete []s;
}
/// Draw tube with radial sizes \a r for points in arrays \a y.
void mgl_tube_r_(uintptr_t *gr, uintptr_t *y, uintptr_t *r, const char *pen,int l)
{
	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	if(gr&&y&&r)	_GR_->Tube(_D_(y),_D_(r),s);
	delete []s;
}
/// Draw tube with constant radial sizes \a r for points in arrays \a x, \a y, \a z.
void mgl_tube_xyz_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, mreal *r, const char *pen,int l)
{
	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	if(gr&&x&&y&&z)	_GR_->Tube(_D_(x),_D_(y),_D_(z),*r,s);
	delete []s;
}
/// Draw tube with constant radial sizes \a r for points in arrays \a x, \a y.
void mgl_tube_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, mreal *r, const char *pen,int l)
{
	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	if(gr&&x&&y)	_GR_->Tube(_D_(x),_D_(y),*r,s);
	delete []s;
}
/// Draw tube with constant radial sizes \a r for points in arrays \a y.
void mgl_tube_(uintptr_t *gr, uintptr_t *y, mreal *r, const char *pen,int l)
{
	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	if(gr&&y)	_GR_->Tube(_D_(y),*r,s);
	delete []s;
}
//-----------------------------------------------------------------------------
void mgl_textmark_xyzr(HMGL gr, const HMDT x, const HMDT y, const HMDT z, const HMDT r, const char *text, const char *fnt)
{	if(gr&&z&&x&&y&&r&&text)	gr->TextMark(*x,*y,*z,*r,text,fnt);	}
void mgl_textmark_xyr(HMGL gr, const HMDT x, const HMDT y, const HMDT r, const char *text, const char *fnt)
{	if(gr&&x&&y&&r&&text)	gr->TextMark(*x,*y,*r,text,fnt);	}
void mgl_textmark_yr(HMGL gr, const HMDT y, const HMDT r, const char *text, const char *fnt)
{	if(gr&&y&&r&&text)	gr->TextMark(*y,*r,text,fnt);	}
void mgl_textmark(HMGL gr, const HMDT y, const char *text, const char *fnt)
{	if(gr&&y&&text)	gr->TextMark(*y,text,fnt);	}
void mgl_textmarkw_xyzr(HMGL gr, const HMDT x, const HMDT y, const HMDT z, const HMDT r, const wchar_t *text, const char *fnt)
{	if(gr&&z&&x&&y&&r&&text)	gr->TextMark(*x,*y,*z,*r,text,fnt);	}
void mgl_textmarkw_xyr(HMGL gr, const HMDT x, const HMDT y, const HMDT r, const wchar_t *text, const char *fnt)
{	if(gr&&x&&y&&r&&text)	gr->TextMark(*x,*y,*r,text,fnt);	}
void mgl_textmarkw_yr(HMGL gr, const HMDT y, const HMDT r, const wchar_t *text, const char *fnt)
{	if(gr&&y&&r&&text)	gr->TextMark(*y,*r,text,fnt);	}
void mgl_textmarkw(HMGL gr, const HMDT y, const wchar_t *text, const char *fnt)
{	if(gr&&y&&text)	gr->TextMark(*y,text,fnt);	}
//-----------------------------------------------------------------------------
void mgl_textmark_xyzr_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *r, const char *text, const char *fnt, int l,int n)
{
	char *s=new char[l+1];	memcpy(s,text,l);	s[l]=0;
	char *f=new char[n+1];	memcpy(s,fnt,n);	f[n]=0;
	if(gr&&y&&x&&z&&r)	_GR_->TextMark(_D_(x),_D_(y),_D_(z),_D_(r),s,f);
	delete []s;		delete []f;
}
void mgl_textmark_xyr_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *r, const char *text, const char *fnt, int l,int n)
{
	char *s=new char[l+1];	memcpy(s,text,l);	s[l]=0;
	char *f=new char[n+1];	memcpy(s,fnt,n);	f[n]=0;
	if(gr&&y&&x&&r)	_GR_->TextMark(_D_(x),_D_(y),_D_(r),s,f);
	delete []s;		delete []f;
}
void mgl_textmark_yr_(uintptr_t *gr, uintptr_t *y, uintptr_t *r, const char *text, const char *fnt, int l,int n)
{
	char *s=new char[l+1];	memcpy(s,text,l);	s[l]=0;
	char *f=new char[n+1];	memcpy(s,fnt,n);	f[n]=0;
	if(gr&&y&&r)	_GR_->TextMark(_D_(y),_D_(r),s,f);
	delete []s;		delete []f;
}
void mgl_textmark_(uintptr_t *gr, uintptr_t *y, const char *text, const char *fnt, int l,int n)
{
	char *s=new char[l+1];	memcpy(s,text,l);	s[l]=0;
	char *f=new char[n+1];	memcpy(s,fnt,n);	f[n]=0;
	if(gr&&y)	_GR_->TextMark(_D_(y),s,f);
	delete []s;		delete []f;
}
//-----------------------------------------------------------------------------
void mgl_fplot(HMGL gr, const char *fy, const char *stl, int n)
{	if(gr)	gr->Plot(fy, stl, NAN, n);	}
void mgl_fplot_xyz(HMGL gr, const char *fx, const char *fy, const char *fz, const char *stl, int n)
{	if(gr)	gr->Plot(fx,fy,fz,stl,n);	}
//-----------------------------------------------------------------------------
void mgl_fplot_(uintptr_t *gr, const char *fy, const char *stl, int *n, int ly, int ls)
{
	char *s=new char[ly+1];	memcpy(s,fy,ly);	s[ly]=0;
	char *p=new char[ls+1];	memcpy(p,stl,ls);	p[ls]=0;
	if(gr)	_GR_->Plot(s, p, NAN, *n);
	delete []s;		delete []p;
}
void mgl_fplot_xyz_(uintptr_t *gr, const char *fx, const char *fy, const char *fz, const char *stl, int *n, int lx, int ly, int lz, int ls)
{
	char *sx=new char[lx+1];	memcpy(sx,fx,lx);	sx[lx]=0;
	char *sy=new char[ly+1];	memcpy(sy,fy,ly);	sy[ly]=0;
	char *sz=new char[lz+1];	memcpy(sz,fz,lz);	sz[lz]=0;
	char *p=new char[ls+1];	memcpy(p,stl,ls);	p[ls]=0;
	if(gr)	_GR_->Plot(sx, sy, sz, p, *n);
	delete []sx;	delete []sy;	delete []sz;	delete []p;
}
//-----------------------------------------------------------------------------
