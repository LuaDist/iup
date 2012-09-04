/***************************************************************************
 * mgl_combi.cpp is part of Math Graphic Library
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
//-----------------------------------------------------------------------------
//
//	DensX, DensY, DensZ series
//
//-----------------------------------------------------------------------------
void mglGraph::DensX(const mglData &a, const char *sch,mreal sVal)
{
	register long i,j,i0,k,n=a.nx,m=a.ny;
	mglColor col;
	if(isnan(sVal))	sVal = GetOrgX('x');
	if(a.nx<2 || a.ny<2)		{	SetWarn(mglWarnLow,"DensX");	return;	}
	if(sVal<Min.x || sVal>Max.x){	SetWarn(mglWarnSlc,"DensX");	return;	}
	static int cgid=1;	StartGroup("DensX",cgid++);
	SetScheme(sch);
	mreal *b = a.a, d;
	bool bb = false;
	if(a.nz>1)
	{
		bb = true;
		n = a.nz;
		d = (a.nx-1)*(sVal - Min.x)/(Max.x - Min.x);
		k = long(d);		d = d - k;
		b = new mreal[n*m];
		if(d>0) for(i=0;i<n;i++)	for(j=0;j<m;j++)
			b[i+n*j] = a.a[k+a.nx*(j+m*i)]*(1-d) + d*a.a[k+1+a.nx*(j+m*i)];
		else	for(i=0;i<n;i++)	for(j=0;j<m;j++)
			b[i+n*j] = a.a[k+a.nx*(j+m*i)];
	}

	mreal *pp = new mreal[3*n*m], *cc = new mreal[4*n*m];
	for(i=0;i<n;i++)	for(j=0;j<m;j++)	// создаем массив точек
	{
		i0 = i+n*j;
		pp[3*i0+0] = sVal;
		pp[3*i0+2] = Min.y + (Max.y-Min.y)*mreal(j)/(m-1.);
		pp[3*i0+1] = Min.z + (Max.z-Min.z)*mreal(i)/(n-1.);
		col = GetC(b[i+n*j]);
		cc[4*i0+0] = col.r;	cc[4*i0+1] = col.g;
		cc[4*i0+2] = col.b;	cc[4*i0+3] = Transparent ? AlphaDef : 1;
		ScalePoint(pp[3*i0+0],pp[3*i0+1],pp[3*i0+2]);
	}
	surf_plot(n, m, pp, cc, 0);
	EndGroup();
	if(a.nz>1)	delete []b;
	delete []pp;	delete []cc;	if(bb)	delete []b;
}
//-----------------------------------------------------------------------------
void mglGraph::DensY(const mglData &a, const char *sch,mreal sVal)
{
	register long i,j,i0,k,n=a.nx,m=a.ny;
	mglColor col;
	if(isnan(sVal))	sVal = GetOrgY('y');
	if(a.nx<2 || a.ny<2)		{	SetWarn(mglWarnLow,"DensY");	return;	}
	if(sVal<Min.y || sVal>Max.y){	SetWarn(mglWarnSlc,"DensY");	return;	}
	static int cgid=1;	StartGroup("DensY",cgid++);
	SetScheme(sch);
	mreal *b = a.a, d;
	bool bb = false;
	if(a.nz>1)
	{
		bb = true;
		m = a.nz;
		d = (a.ny-1)*(sVal - Min.y)/(Max.y - Min.y);
		k = long(d);		d = d - k;
		b = new mreal[n*m];
		if(d>0) for(i=0;i<n;i++)	for(j=0;j<m;j++)
			b[i+n*j] = a.a[i+n*(k+a.ny*j)]*(1-d) + d*a.a[i+n*(k+1+a.ny*j)];
		else	for(i=0;i<n;i++)	for(j=0;j<m;j++)
			b[i+n*j] = a.a[i+n*(k+a.ny*j)];
	}

	mreal *pp = new mreal[3*n*m], *cc = new mreal[4*n*m];
	for(i=0;i<n;i++)	for(j=0;j<m;j++)	// создаем массив точек
	{
		i0 = i+n*j;
		pp[3*i0+0] = Min.x + (Max.x-Min.x)*mreal(i)/(n-1.);
		pp[3*i0+1] = sVal;
		pp[3*i0+2] = Min.z + (Max.z-Min.z)*mreal(j)/(m-1.);
		col = GetC(b[i+n*j]);
		cc[4*i0+0] = col.r;	cc[4*i0+1] = col.g;
		cc[4*i0+2] = col.b;	cc[4*i0+3] = Transparent ? AlphaDef : 1;
		ScalePoint(pp[3*i0+0],pp[3*i0+1],pp[3*i0+2]);
	}
	surf_plot(n, m, pp, cc, 0);
	EndGroup();
	if(a.nz>1)	delete []b;
	delete []pp;	delete []cc;	if(bb)	delete []b;
}
//-----------------------------------------------------------------------------
void mglGraph::DensZ(const mglData &a, const char *sch,mreal sVal)
{
	register long i,j,i0,k,n=a.nx,m=a.ny;
	mglColor col;
	if(isnan(sVal))	sVal = GetOrgZ('z');
	if(a.nx<2 || a.ny<2)		{	SetWarn(mglWarnLow,"DensZ");	return;	}
	if(sVal<Min.z || sVal>Max.z){	SetWarn(mglWarnSlc,"DensZ");	return;	}
	static int cgid=1;	StartGroup("DensZ",cgid++);
	SetScheme(sch);
	mreal *b = a.a, d;
	bool bb = false;
	if(a.nz>1)
	{
		bb = true;
		d = (a.nz-1)*(sVal - Min.z)/(Max.y - Min.z);
		k = long(d);		d = d - k;
		b = new mreal[n*m];
		if(d>0) for(i=0;i<n;i++)	for(j=0;j<m;j++)
			b[i+n*j] = a.a[i+n*(j+m*k)]*(1-d) + d*a.a[i+n*(j+m*k+m)];
		else	for(i=0;i<n;i++)	for(j=0;j<m;j++)
			b[i+n*j] = a.a[i+n*(j+m*k)];
	}

	mreal *pp = new mreal[3*n*m], *cc = new mreal[4*n*m];
	for(i=0;i<n;i++)	for(j=0;j<m;j++)	// создаем массив точек
	{
		i0 = i+n*j;
		pp[3*i0+0] = Min.x + (Max.x-Min.x)*mreal(i)/(n-1.);
		pp[3*i0+1] = Min.y + (Max.y-Min.y)*mreal(j)/(m-1.);
		pp[3*i0+2] = sVal;
		col = GetC(b[i+n*j]);
		cc[4*i0+0] = col.r;	cc[4*i0+1] = col.g;
		cc[4*i0+2] = col.b;	cc[4*i0+3] = Transparent ? AlphaDef : 1;
		ScalePoint(pp[3*i0+0],pp[3*i0+1],pp[3*i0+2]);
	}
	surf_plot(n, m, pp, cc, 0);
	EndGroup();
	if(a.nz>1)	delete []b;
	delete []pp;	delete []cc;	if(bb)	delete []b;
}
//-----------------------------------------------------------------------------
//
//	ContX, ContY, ContZ series
//
//-----------------------------------------------------------------------------
void mglGraph::ContX(const mglData &z, const char *sch, mreal zVal, int Num)
{
	if(Num<1)	{	SetWarn(mglWarnCnt,"ContX");	return;	}
	mglData v(Num);
	for(long i=0;i<Num;i++)	v.a[i] = Cmin + (Cmax-Cmin)*mreal(i+1)/(Num+1);
	ContX(v,z,sch,zVal);
}
//-----------------------------------------------------------------------------
void mglGraph::ContY(const mglData &z, const char *sch, mreal zVal, int Num)
{
	if(Num<1)	{	SetWarn(mglWarnCnt,"ContY");	return;	}
	mglData v(Num);
	for(long i=0;i<Num;i++)	v.a[i] = Cmin + (Cmax-Cmin)*mreal(i+1)/(Num+1);
	ContY(v,z,sch,zVal);
}
//-----------------------------------------------------------------------------
void mglGraph::ContZ(const mglData &z, const char *sch, mreal zVal, int Num)
{
	if(Num<1)	{	SetWarn(mglWarnCnt,"ContZ");	return;	}
	mglData v(Num);
	for(long i=0;i<Num;i++)	v.a[i] = Cmin + (Cmax-Cmin)*mreal(i+1)/(Num+1);
	ContZ(v,z,sch,zVal);
}
//-----------------------------------------------------------------------------
void mglGraph::ContX(const mglData &v, const mglData &a, const char *sch, mreal sVal)
{
	register long i,j,k,n=a.nx,m=a.ny;
	mglColor col;
	if(isnan(sVal))	sVal = GetOrgX('x');
	if(a.nx<2 || a.ny<2)		{	SetWarn(mglWarnLow,"ContX");	return;	}
	if(sVal<Min.x || sVal>Max.x){	SetWarn(mglWarnSlc,"ContX");	return;	}
	static int cgid=1;	StartGroup("ContX",cgid++);
	long text=0;
	if(sch && strchr(sch,'t'))	text=-1;
	if(sch && strchr(sch,'T'))	text=+1;
	SetScheme(sch);
	mreal *b = a.a, d;
	bool bb = false;
	if(a.nz>1)
	{
		bb = true;
		n = a.nz;
		d = (a.ny-1)*(sVal - Min.x)/(Max.x - Min.x);
		k = long(d);		d = d - k;
		b = new mreal[n*m];
		if(d>0)	for(j=0;j<m;j++) for(i=0;i<n;i++)
			b[i+n*j] = a.a[k+a.nx*(j+m*i)]*(1-d) + d*a.a[k+1+a.nx*(j+m*i)];
		else	for(i=0;i<n;i++)	for(j=0;j<m;j++)
			b[i+n*j] = a.a[k+a.nx*(j+m*i)];
	}
	mreal *xx = new mreal[n*m], *yy = new mreal[n*m], *zz = new mreal[n*m];
	for(i=0;i<n;i++)	for(j=0;j<m;j++)
	{
		k = i+n*j;		yy[k] = Min.y + (Max.y-Min.y)*i/(n-1.);
		xx[k] = sVal;	zz[k] = Min.z + (Max.z-Min.z)*j/(m-1.);
	}
	for(long i=0;i<v.nx;i++)
	{
		Color(v.a[i]);
		cont_plot(v.a[i],n,m,b,xx,yy,zz,sVal,false,false,text);
	}
	EndGroup();
	if(a.nz>1)	delete []b;
	delete []xx;	delete []yy;	delete []zz;	if(bb)	delete []b;
}
//-----------------------------------------------------------------------------
void mglGraph::ContY(const mglData &v, const mglData &a, const char *sch, mreal sVal)
{
	register long i,j,k,n=a.nx,m=a.ny;
	mglColor col;
	if(isnan(sVal))	sVal = GetOrgY('y');
	if(a.nx<2 || a.ny<2)		{	SetWarn(mglWarnLow,"ContY");	return;	}
	if(sVal<Min.y || sVal>Max.y){	SetWarn(mglWarnSlc,"ContY");	return;	}
	static int cgid=1;	StartGroup("ContY",cgid++);
	long text=0;
	if(sch && strchr(sch,'t'))	text=-1;
	if(sch && strchr(sch,'T'))	text=+1;
	SetScheme(sch);
	mreal *b = a.a, d;
	bool bb = false;
	if(a.nz>1)
	{
		bb = true;
		m = a.nz;
		d = (a.ny-1)*(sVal - Min.y)/(Max.y - Min.y);
		k = long(d);		d = d - k;
		b = new mreal[n*m];
		if(d>0) for(i=0;i<n;i++)	for(j=0;j<m;j++)
			b[i+n*j] = a.a[i+n*(k+a.ny*j)]*(1-d) + d*a.a[i+n*(k+1+a.ny*j)];
		else	for(i=0;i<n;i++)	for(j=0;j<m;j++)
			b[i+n*j] = a.a[i+n*(k+a.ny*j)];
	}
	mreal *xx = new mreal[n*m], *yy = new mreal[n*m], *zz = new mreal[n*m];
	for(i=0;i<n;i++)	for(j=0;j<m;j++)
	{
		k = i+n*j;		zz[k] = Min.z + (Max.z-Min.z)*j/(m-1.);
		yy[k] = sVal;	xx[k] = Min.x + (Max.x-Min.x)*i/(n-1.);
	}
	for(long i=0;i<v.nx;i++)
	{
		Color(v.a[i]);
		cont_plot(v.a[i],n,m,b,xx,yy,zz,sVal,false,false,text);
	}
	EndGroup();
	if(a.nz>1)	delete []b;
	delete []xx;	delete []yy;	delete []zz;	if(bb)	delete []b;
}
//-----------------------------------------------------------------------------
void mglGraph::ContZ(const mglData &v, const mglData &a, const char *sch, mreal sVal)
{
	register long i,j,k,n=a.nx,m=a.ny;
	mglColor col;
	if(isnan(sVal))	sVal = GetOrgZ('z');
	if(a.nx<2 || a.ny<2)		{	SetWarn(mglWarnLow,"ContZ");	return;	}
	if(sVal<Min.z || sVal>Max.z){	SetWarn(mglWarnSlc,"ContZ");	return;	}
	static int cgid=1;	StartGroup("ContZ",cgid++);
	long text=0;
	if(sch && strchr(sch,'t'))	text=-1;
	if(sch && strchr(sch,'T'))	text=+1;
	SetScheme(sch);
	mreal *b = a.a, d;
	bool bb = false;
	if(a.nz>1)
	{
		bb = true;
		d = (a.nz-1)*(sVal - Min.z)/(Max.y - Min.z);
		k = long(d);		d = d - k;
		b = new mreal[n*m];
		if(d>0) for(i=0;i<n;i++)	for(j=0;j<m;j++)
			b[i+n*j] = a.a[i+n*(j+m*k)]*(1-d) + d*a.a[i+n*(j+m*k+m)];
		else	for(i=0;i<n;i++)	for(j=0;j<m;j++)
			b[i+n*j] = a.a[i+n*(j+m*k)];
	}
	mreal *xx = new mreal[n*m], *yy = new mreal[n*m], *zz = new mreal[n*m];
	for(i=0;i<n;i++)	for(j=0;j<m;j++)
	{
		k = i+n*j;		yy[k] = Min.y + (Max.y-Min.y)*j/(m-1.);
		zz[k] = sVal;	xx[k] = Min.x + (Max.x-Min.x)*i/(n-1.);
	}
	for(long i=0;i<v.nx;i++)
	{
		Color(v.a[i]);
		cont_plot(v.a[i],n,m,b,xx,yy,zz,sVal,false,false,text);
	}
	EndGroup();
	if(a.nz>1)	delete []b;
	delete []xx;	delete []yy;	delete []zz;	if(bb)	delete []b;
}
//-----------------------------------------------------------------------------
//
//	ContFX, ContFY, ContFZ series
//
//-----------------------------------------------------------------------------
void mglGraph::ContFX(const mglData &z, const char *sch, mreal zVal, int Num)
{
	if(Num<1)	{	SetWarn(mglWarnCnt,"ContFX");	return;	}
	mglData v(Num);
	for(long i=0;i<Num;i++)	v.a[i] = Cmin + (Cmax-Cmin)*mreal(i+1)/(Num+1);
	ContFX(v,z,sch,zVal);
}
//-----------------------------------------------------------------------------
void mglGraph::ContFY(const mglData &z, const char *sch, mreal zVal, int Num)
{
	if(Num<1)	{	SetWarn(mglWarnCnt,"ContFYX");	return;	}
	mglData v(Num);
	for(long i=0;i<Num;i++)	v.a[i] = Cmin + (Cmax-Cmin)*mreal(i+1)/(Num+1);
	ContFY(v,z,sch,zVal);
}
//-----------------------------------------------------------------------------
void mglGraph::ContFZ(const mglData &z, const char *sch, mreal zVal, int Num)
{
	if(Num<1)	{	SetWarn(mglWarnCnt,"ContFZ");	return;	}
	mglData v(Num);
	for(long i=0;i<Num;i++)	v.a[i] = Cmin + (Cmax-Cmin)*mreal(i+1)/(Num+1);
	ContFZ(v,z,sch,zVal);
}
//-----------------------------------------------------------------------------
void mglGraph::ContFX(const mglData &v, const mglData &a, const char *sch, mreal sVal)
{
	register long i,j,k,n=a.nx,m=a.ny;
	mglColor col;
	if(isnan(sVal))	sVal = GetOrgX('x');
	if(a.nx<2 || a.ny<2)		{	SetWarn(mglWarnLow,"ContFX");	return;	}
	if(sVal<Min.x || sVal>Max.x){	SetWarn(mglWarnSlc,"ContFX");	return;	}
	static int cgid=1;	StartGroup("ContFX",cgid++);
	SetScheme(sch);
	mreal *b = a.a, d;
	bool bb = false;
	if(a.nz>1)
	{
		bb = true;
		n = a.nz;
		d = (a.ny-1)*(sVal - Min.x)/(Max.x - Min.x);
		k = long(d);		d = d - k;
		b = new mreal[n*m];
		if(d>0)	for(j=0;j<m;j++) for(i=0;i<n;i++)
			b[i+n*j] = a.a[k+a.nx*(j+m*i)]*(1-d) + d*a.a[k+1+a.nx*(j+m*i)];
		else	for(i=0;i<n;i++)	for(j=0;j<m;j++)
			b[i+n*j] = a.a[k+a.nx*(j+m*i)];
	}
	mreal *xx = new mreal[n*m], *yy = new mreal[n*m], *zz = new mreal[n*m];
	for(i=0;i<n;i++)	for(j=0;j<m;j++)
	{
		k = i+n*j;		yy[k] = Min.y + (Max.y-Min.y)*i/(n-1.);
		xx[k] = sVal;	zz[k] = Min.z + (Max.z-Min.z)*j/(m-1.);
	}
	for(long i=0;i<v.nx-1;i++)
	{
		Color(v.a[i]);
		contf_plot(v.a[i],v.a[i+1],n,m,b,xx,yy,zz,sVal);
	}
	EndGroup();
	if(a.nz>1)	delete []b;
	delete []xx;	delete []yy;	delete []zz;	if(bb)	delete []b;
}
//-----------------------------------------------------------------------------
void mglGraph::ContFY(const mglData &v, const mglData &a, const char *sch, mreal sVal)
{
	register long i,j,k,n=a.nx,m=a.ny;
	mglColor col;
	if(isnan(sVal))	sVal = GetOrgY('y');
	if(a.nx<2 || a.ny<2)		{	SetWarn(mglWarnLow,"ContFY");	return;	}
	if(sVal<Min.y || sVal>Max.y){	SetWarn(mglWarnSlc,"ContFY");	return;	}
	static int cgid=1;	StartGroup("ContFY",cgid++);
	SetScheme(sch);
	mreal *b = a.a, d;
	bool bb = false;
	if(a.nz>1)
	{
		bb = true;
		m = a.nz;
		d = (a.ny-1)*(sVal - Min.y)/(Max.y - Min.y);
		k = long(d);		d = d - k;
		b = new mreal[n*m];
		if(d>0) for(i=0;i<n;i++)	for(j=0;j<m;j++)
			b[i+n*j] = a.a[i+n*(k+a.ny*j)]*(1-d) + d*a.a[i+n*(k+1+a.ny*j)];
		else	for(i=0;i<n;i++)	for(j=0;j<m;j++)
			b[i+n*j] = a.a[i+n*(k+a.ny*j)];
	}
	mreal *xx = new mreal[n*m], *yy = new mreal[n*m], *zz = new mreal[n*m];
	for(i=0;i<n;i++)	for(j=0;j<m;j++)
	{
		k = i+n*j;		zz[k] = Min.z + (Max.z-Min.z)*j/(m-1.);
		yy[k] = sVal;	xx[k] = Min.x + (Max.x-Min.x)*i/(n-1.);
	}
	for(long i=0;i<v.nx-1;i++)
	{
		Color(v.a[i]);
		contf_plot(v.a[i],v.a[i+1],n,m,b,xx,yy,zz,sVal);
	}
	EndGroup();
	if(a.nz>1)	delete []b;
	delete []xx;	delete []yy;	delete []zz;	if(bb)	delete []b;
}
//-----------------------------------------------------------------------------
void mglGraph::ContFZ(const mglData &v, const mglData &a, const char *sch, mreal sVal)
{
	register long i,j,k,n=a.nx,m=a.ny;
	mglColor col;
	if(isnan(sVal))	sVal = GetOrgZ('z');
	if(a.nx<2 || a.ny<2)		{	SetWarn(mglWarnLow,"ContFZ");	return;	}
	if(sVal<Min.z || sVal>Max.z){	SetWarn(mglWarnSlc,"ContFZ");	return;	}
	static int cgid=1;	StartGroup("ContFZ",cgid++);
	SetScheme(sch);
	mreal *b = a.a, d;
	bool bb = false;
	if(a.nz>1)
	{
		bb = true;
		d = (a.nz-1)*(sVal - Min.z)/(Max.y - Min.z);
		k = long(d);		d = d - k;
		b = new mreal[n*m];
		if(d>0) for(i=0;i<n;i++)	for(j=0;j<m;j++)
			b[i+n*j] = a.a[i+n*(j+m*k)]*(1-d) + d*a.a[i+n*(j+m*k+m)];
		else	for(i=0;i<n;i++)	for(j=0;j<m;j++)
			b[i+n*j] = a.a[i+n*(j+m*k)];
	}
	mreal *xx = new mreal[n*m], *yy = new mreal[n*m], *zz = new mreal[n*m];
	for(i=0;i<n;i++)	for(j=0;j<m;j++)
	{
		k = i+n*j;		yy[k] = Min.y + (Max.y-Min.y)*j/(m-1.);
		zz[k] = sVal;	xx[k] = Min.x + (Max.x-Min.x)*i/(n-1.);
	}
	for(long i=0;i<v.nx-1;i++)
	{
		Color(v.a[i]);
		contf_plot(v.a[i],v.a[i+1],n,m,b,xx,yy,zz,sVal);
	}
	EndGroup();
	if(a.nz>1)	delete []b;
	delete []xx;	delete []yy;	delete []zz;	if(bb)	delete []b;
}
//-----------------------------------------------------------------------------
//		Combined plotting functions
//-----------------------------------------------------------------------------
/// Draw density plot for data a at x = sVal
void mgl_dens_x(HMGL gr, const HMDT a, const char *sch, mreal sVal)
{	if(gr && a)	gr->DensX(*a, sch, sVal);	}
/// Draw density plot for data a at y = sVal
void mgl_dens_y(HMGL gr, const HMDT a, const char *sch, mreal sVal)
{	if(gr && a)	gr->DensY(*a, sch, sVal);	}
/// Draw density plot for data a at z = sVal
void mgl_dens_z(HMGL gr, const HMDT a, const char *sch, mreal sVal)
{	if(gr && a)	gr->DensZ(*a, sch, sVal);	}
/// Draw several contour plots for data a at x = sVal
void mgl_cont_x(HMGL gr, const HMDT a, const char *sch, mreal sVal, int Num)
{	if(gr && a)	gr->ContX(*a, sch, sVal, Num);	}
/// Draw several contour plots for data a at y = sVal
void mgl_cont_y(HMGL gr, const HMDT a, const char *sch, mreal sVal, int Num)
{	if(gr && a)	gr->ContY(*a, sch, sVal, Num);	}
/// Draw several contour plots for data a at z = sVal
void mgl_cont_z(HMGL gr, const HMDT a, const char *sch, mreal sVal, int Num)
{	if(gr && a)	gr->ContZ(*a, sch, sVal, Num);	}
/// Draw contour plots for data a at x = sVal
void mgl_cont_x_val(HMGL gr, const HMDT v, const HMDT a, const char *sch, mreal sVal)
{	if(gr && a && v)	gr->ContX(*v, *a, sch, sVal);	}
/// Draw contour plots for data a at y = sVal
void mgl_cont_y_val(HMGL gr, const HMDT v, const HMDT a, const char *sch, mreal sVal)
{	if(gr && a && v)	gr->ContY(*v, *a, sch, sVal);	}
/// Draw contour plots for data a at z = sVal
void mgl_cont_z_val(HMGL gr, const HMDT v, const HMDT a, const char *sch, mreal sVal)
{	if(gr && a && v)	gr->ContZ(*v, *a, sch, sVal);	}
/// Draw several contour plots for data a at x = sVal
void mgl_contf_x(HMGL gr, const HMDT a, const char *sch, mreal sVal, int Num)
{	if(gr && a)	gr->ContFX(*a, sch, sVal, Num);	}
/// Draw several contour plots for data a at y = sVal
void mgl_contf_y(HMGL gr, const HMDT a, const char *sch, mreal sVal, int Num)
{	if(gr && a)	gr->ContFY(*a, sch, sVal, Num);	}
/// Draw several contour plots for data a at z = sVal
void mgl_contf_z(HMGL gr, const HMDT a, const char *sch, mreal sVal, int Num)
{	if(gr && a)	gr->ContFZ(*a, sch, sVal, Num);	}
/// Draw contour plots for data a at x = sVal
void mgl_contf_x_val(HMGL gr, const HMDT v, const HMDT a, const char *sch, mreal sVal)
{	if(gr && a && v)	gr->ContFX(*v, *a, sch, sVal);	}
/// Draw contour plots for data a at y = sVal
void mgl_contf_y_val(HMGL gr, const HMDT v, const HMDT a, const char *sch, mreal sVal)
{	if(gr && a && v)	gr->ContFY(*v, *a, sch, sVal);	}
/// Draw contour plots for data a at z = sVal
void mgl_contf_z_val(HMGL gr, const HMDT v, const HMDT a, const char *sch, mreal sVal)
{	if(gr && a && v)	gr->ContFZ(*v, *a, sch, sVal);	}
//-----------------------------------------------------------------------------
//		Combined plotting functions (Fortran)
//-----------------------------------------------------------------------------
/// Draw density plot for data a at x = *sVal
void mgl_dens_x_(uintptr_t *gr, uintptr_t *a, const char *sch, mreal *sVal,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a)	_GR_->DensX(_D_(a), s, *sVal);
	delete []s;
}
/// Draw density plot for data a at y = *sVal
void mgl_dens_y_(uintptr_t *gr, uintptr_t *a, const char *sch, mreal *sVal,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a)	_GR_->DensY(_D_(a), s, *sVal);
	delete []s;
}
/// Draw density plot for data a at z = *sVal
void mgl_dens_z_(uintptr_t *gr, uintptr_t *a, const char *sch, mreal *sVal,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a)	_GR_->DensZ(_D_(a), s, *sVal);
	delete []s;
}
/// Draw several contour plots for data a at x = *sVal
void mgl_cont_x_(uintptr_t *gr, uintptr_t *a, const char *sch, mreal *sVal, int *Num,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a)	_GR_->ContX(_D_(a), s, *sVal, *Num);
	delete []s;
}
/// Draw several contour plots for data a at y = *sVal
void mgl_cont_y_(uintptr_t *gr, uintptr_t *a, const char *sch, mreal *sVal, int *Num,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a)	_GR_->ContY(_D_(a), s, *sVal, *Num);
	delete []s;
}
/// Draw several contour plots for data a at z = *sVal
void mgl_cont_z_(uintptr_t *gr, uintptr_t *a, const char *sch, mreal *sVal, int *Num,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a)	_GR_->ContZ(_D_(a), s, *sVal, *Num);
	delete []s;
}
/// Draw contour plots for data a at x = *sVal
void mgl_cont_x_val_(uintptr_t *gr, uintptr_t *v, uintptr_t *a, const char *sch, mreal *sVal,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a && v)	_GR_->ContX(_D_(v), _D_(a), s, *sVal);
	delete []s;
}
/// Draw contour plots for data a at y = *sVal
void mgl_cont_y_val_(uintptr_t *gr, uintptr_t *v, uintptr_t *a, const char *sch, mreal *sVal,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a && v)	_GR_->ContY(_D_(v), _D_(a), s, *sVal);
	delete []s;
}
/// Draw contour plots for data a at z = *sVal
void mgl_cont_z_val_(uintptr_t *gr, uintptr_t *v, uintptr_t *a, const char *sch, mreal *sVal,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a && v)	_GR_->ContZ(_D_(v), _D_(a), s, *sVal);
	delete []s;
}
/// Draw several contour plots for data a at x = *sVal
void mgl_contf_x_(uintptr_t *gr, uintptr_t *a, const char *sch, mreal *sVal, int *Num,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a)	_GR_->ContFX(_D_(a), s, *sVal, *Num);
	delete []s;
}
/// Draw several contour plots for data a at y = *sVal
void mgl_contf_y_(uintptr_t *gr, uintptr_t *a, const char *sch, mreal *sVal, int *Num,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a)	_GR_->ContFY(_D_(a), s, *sVal, *Num);
	delete []s;
}
/// Draw several contour plots for data a at z = *sVal
void mgl_contf_z_(uintptr_t *gr, uintptr_t *a, const char *sch, mreal *sVal, int *Num,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a)	_GR_->ContFZ(_D_(a), s, *sVal, *Num);
	delete []s;
}
/// Draw contour plots for data a at x = *sVal
void mgl_contf_x_val_(uintptr_t *gr, uintptr_t *v, uintptr_t *a, const char *sch, mreal *sVal,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a && v)	_GR_->ContFX(_D_(v), _D_(a), s, *sVal);
	delete []s;
}
/// Draw contour plots for data a at y = *sVal
void mgl_contf_y_val_(uintptr_t *gr, uintptr_t *v, uintptr_t *a, const char *sch, mreal *sVal,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a && v)	_GR_->ContFY(_D_(v), _D_(a), s, *sVal);
	delete []s;
}
/// Draw contour plots for data a at z = *sVal
void mgl_contf_z_val_(uintptr_t *gr, uintptr_t *v, uintptr_t *a, const char *sch, mreal *sVal,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a && v)	_GR_->ContFZ(_D_(v), _D_(a), s, *sVal);
	delete []s;
}
//-----------------------------------------------------------------------------
