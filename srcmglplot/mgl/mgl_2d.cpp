/***************************************************************************
 * mgl_2d.cpp is part of Math Graphic Library
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
//	Plot by formulas series
//
//-----------------------------------------------------------------------------
void mglGraph::Surf(const char *eqZ, const char *sch, int n)
{
	// TODO Add strong function variation analisys ???
	if(eqZ==0 || eqZ[0]==0)	return;		// nothing to plot
	if(n<=0)	n=100;
	mglData z(n,n);
	mglFormula *eq = new mglFormula(eqZ);
	register int i,j;
	mreal dx = (Max.x - Min.x)/(n-1.), dy = (Max.y - Min.y)/(n-1.);
	for(i=0;i<n;i++)	for(j=0;j<n;j++)
		z.a[i+n*j] = eq->Calc(Min.x+i*dx, Min.y+j*dy);
	Surf(z, sch);
	delete eq;
}
//-----------------------------------------------------------------------------
void mglGraph::Surf(const char *eqX, const char *eqY, const char *eqZ, const char *sch, int n)
{
	// TODO Add strong function variation analisys ???
	mglData x(n,n), y(n,n), z(n,n);
	if(n<=0)	n=100;
	mglFormula *ex, *ey, *ez;
	ex = new mglFormula(eqX ? eqX : "u");
	ey = new mglFormula(eqY ? eqY : "v");
	ez = new mglFormula(eqZ ? eqZ : "0");
	register int i,j;
	register mreal u,v;
	for(i=0;i<n;i++)	for(j=0;j<n;j++)
	{
		v = i/(n-1.);	u = j/(n-1.);
		x.a[i+n*j] = ex->Calc(0,v,0,u);
		y.a[i+n*j] = ey->Calc(0,v,0,u);
		z.a[i+n*j] = ez->Calc(0,v,0,u);
	}
	Surf(x,y,z,sch);
	delete ex;	delete ey;	delete ez;
}
//-----------------------------------------------------------------------------
//
//	Primitives
//
//-----------------------------------------------------------------------------
void mglGraph::Face(mglPoint p1, mglPoint p2, mglPoint p3, mglPoint p4, const char *stl, int n,mreal val)
{
	static int cgid=1;	StartGroup("Face",cgid++);
	register long i,j,i0;
	n = (n<2) ? 2 : n;
	mglColor c1('w'),c2,c3,c4;
	mreal *pp = new mreal[3*n*n],u;
	mreal *cc = new mreal[4*n*n],v;
	bool cut = Cut;	Cut = true;

	SetScheme(stl);
	if(stl && *stl && NumCol)	c1 = cmap[0];
	bool all = stl && strlen(stl)>4 && NumCol>4;
	if(all)	{	c2=cmap[1];	c3=cmap[2];	c4=cmap[3];	}
	else if(!isnan(val))	c1 = GetC(val);

	for(i=0;i<n;i++)	for(j=0;j<n;j++)
	{
		u = i/(n-1.);	v = j/(n-1.);	i0 = i+n*j;		cc[4*i0+3] = AlphaDef;
		pp[3*i0+0] = p1.x+u*(p2.x-p1.x)+v*(p3.x-p1.x)+u*v*(p4.x+p1.x-p2.x-p3.x);
		pp[3*i0+1] = p1.y+u*(p2.y-p1.y)+v*(p3.y-p1.y)+u*v*(p4.y+p1.y-p2.y-p3.y);
		pp[3*i0+2] = p1.z+u*(p2.z-p1.z)+v*(p3.z-p1.z)+u*v*(p4.z+p1.z-p2.z-p3.z);
		ScalePoint(pp[3*i0+0],pp[3*i0+1],pp[3*i0+2]);
		if(all)
		{
			cc[4*i0+0] = c1.r+u*(c2.r-c1.r)+v*(c3.r-c1.r)+u*v*(c4.r+c1.r-c2.r-c3.r);
			cc[4*i0+1] = c1.g+u*(c2.g-c1.g)+v*(c3.g-c1.g)+u*v*(c4.g+c1.g-c2.g-c3.g);
			cc[4*i0+2] = c1.b+u*(c2.b-c1.b)+v*(c3.b-c1.b)+u*v*(c4.b+c1.b-c2.b-c3.b);
		}
		else
		{	cc[4*i0] = c1.r;	cc[4*i0+1] = c1.g;	cc[4*i0+2] = c1.b;	}
	}
	surf_plot(n,n,pp,cc,0);
	if(stl && strchr(stl,'#'))
	{	SelectPen("k-");	memset(cc,0,4*n*n*sizeof(mreal));	mesh_plot(n,n,pp,cc,0,-3);	}
	Cut = cut;
	delete []pp;	delete []cc;
	EndGroup();
}
//-----------------------------------------------------------------------------
void mglGraph::Sphere(mglPoint p, mreal r, const char *stl)
{
	Drop(p,mglPoint(1),2*r,stl,0,1);
}
//-----------------------------------------------------------------------------
void mglGraph::Drop(mglPoint p, mglPoint q, mreal r, const char *stl, mreal sh, mreal a)
{
	mglColor c('r');
	if(stl && stl[0])	c.Set(stl[0]);
	Drop(p,q,r,c,sh,a);
}
//-----------------------------------------------------------------------------
void mglGraph::Drop(mglPoint p, mglPoint q, mreal r, mglColor c, mreal sh, mreal a)
{
	static int cgid=1;	StartGroup("Drop",cgid++);
	register long i,j,i0;
	long n = CirclePnts;
	n = (n<3) ? 3 : n;
	mreal *pp = new mreal[3*n*n],u,v,x,y,z;

	if(q.x==0 && q.y==0 && q.z==0)	{	q = mglPoint(1,0,0);	sh=0;	}
	mglPoint p1,p2;
	q = q/sqrt(q*q);	p1 = !q;	p2 = q^p1;	r /= 2;
	DefColor(c,-1);

	bool cut = Cut;	Cut = true;
	for(i=0;i<n;i++)	for(j=0;j<n;j++)
	{
		u = i*M_PI/(n-1.);	v = 2*M_PI*j/(n-1.)-1;	i0 = i+n*j;
		x = r*cos(v)*a*sin(u)*(1.+sh*cos(u))/(1+sh);
		y = r*sin(v)*a*sin(u)*(1.+sh*cos(u))/(1+sh);
		z = r*(1+sh)*(cos(u)+sh);
		pp[3*i0+0] = p.x + p1.x*x + p2.x*y + q.x*z;
		pp[3*i0+1] = p.y + p1.y*x + p2.y*y + q.y*z;
		pp[3*i0+2] = p.z + p1.z*x + p2.z*y + q.z*z;
		ScalePoint(pp[3*i0+0],pp[3*i0+1],pp[3*i0+2]);
	}
	Cut = cut;
	int mnt=mglNumThr;	mglNumThr=1;	// too many faces
	surf_plot(n,n,pp,0,0);
	mglNumThr = mnt;
	EndGroup();
	delete []pp;
}
//-----------------------------------------------------------------------------
void mglGraph::Ellipse(mglPoint p, mglPoint r, const char *stl)
{
	if(r.x==0 || r.y==0 || r.z==0)	return;
	static int cgid=1;	StartGroup("Ellipse",cgid++);
	register long i,j,i0;
	long n = CirclePnts;
	n = (n<3) ? 3 : n;
	mreal *pp = new mreal[3*n*n],u,v;

	SetScheme(stl);

	bool cut = Cut;	Cut = true;
	for(i=0;i<n;i++)	for(j=0;j<n;j++)
	{
		u = i*M_PI/(n-1.);	v = 2*M_PI*j/(n-1.)-1;	i0 = i+n*j;
		pp[3*i0+0] = p.x + r.x*cos(v)*sin(u);
		pp[3*i0+1] = p.y + r.y*sin(v)*sin(u);
		pp[3*i0+2] = p.z + r.z*cos(u);
		ScalePoint(pp[3*i0+0],pp[3*i0+1],pp[3*i0+2]);
	}
	Cut = cut;
	surf_plot(n,n,pp,0,0);
	EndGroup();
	delete []pp;
}
//-----------------------------------------------------------------------------
//
//	Mesh series
//
//-----------------------------------------------------------------------------
void mglGraph::Mesh(const mglData &x, const mglData &y, const mglData &z, const char *sch)
{
	register long i,j,i0,k,n=z.nx,m=z.ny;
	mglColor c;
	if(x.nx!=n)		{	SetWarn(mglWarnDim,"Mesh");	return;	}
	if(n<2 || m<2)	{	SetWarn(mglWarnLow,"Mesh");	return;	}
	if(y.nx!=m && (x.ny!=m || y.nx!=n || y.ny!=m))
	{	SetWarn(mglWarnDim);	return;	};
	static int cgid=1;	StartGroup("Mesh",cgid++);
	SelectPen("k-");
	SetScheme(sch);

	mreal *pp = new mreal[3*n*m], *cc = new mreal[3*n*m];
	bool *tt = new bool[n*m];
	for(k=0;k<z.nz;k++)
	{
		for(i=0;i<n;i++)	for(j=0;j<m;j++)	// создаем массив точек
		{
			i0 = i+n*j;
			pp[3*i0+0] = GetX(x,i,j,k);
			pp[3*i0+1] = GetY(y,i,j,k);
			pp[3*i0+2] = z.a[i+n*(j+m*k)];
			c = GetC(pp[3*i0+0],pp[3*i0+1],pp[3*i0+2]);
			cc[3*i0+0] = c.r;	cc[3*i0+1] = c.g;	cc[3*i0+2] = c.b;
			tt[i0] = ScalePoint(pp[3*i0+0],pp[3*i0+1],pp[3*i0+2]);
		}
		mesh_plot(n, m, pp, cc, tt,3);
	}
	EndGroup();
	delete []pp;	delete []cc;	delete []tt;
}
//-----------------------------------------------------------------------------
void mglGraph::Mesh(const mglData &z, const char *sch)
{
	if(z.nx<2 || z.ny<2){	SetWarn(mglWarnLow,"Mesh");	return;	}
	mglData x(z.nx), y(z.ny);
	x.Fill(Min.x,Max.x);
	y.Fill(Min.y,Max.y);
	Mesh(x,y,z,sch);
}
//-----------------------------------------------------------------------------
//
//	Fall series
//
//-----------------------------------------------------------------------------
void mglGraph::Fall(const mglData &x, const mglData &y, const mglData &z, const char *sch)
{
	register long i,j,i0,k,n=z.nx,m=z.ny;
	mglColor c;
	if(x.nx!=z.nx)		{	SetWarn(mglWarnDim,"Fall");	return;	}
	if(z.nx<2 || z.ny<2){	SetWarn(mglWarnLow,"Fall");	return;	}
	if(y.nx!=z.ny && (x.ny!=z.ny || y.nx!=z.nx || y.ny!=z.ny))
	{	SetWarn(mglWarnDim);	return;	}
	static int cgid=1;	StartGroup("Fall",cgid++);
	SelectPen("k-");
	SetScheme(sch);

	long how = (sch && strchr(sch,'x')) ? 2:1;
	mreal *pp = new mreal[3*n*m], *cc = new mreal[3*n*m];
	bool *tt = new bool[n*m];
	for(k=0;k<z.nz;k++)
	{
		for(i=0;i<n;i++)	for(j=0;j<m;j++)	// создаем массив точек
		{
			i0 = i+n*j;
			pp[3*i0+0] = GetX(x,i,j,k);
			pp[3*i0+1] = GetY(y,i,j,k);
			pp[3*i0+2] = z.a[i+n*(j+m*k)];
			c = GetC(pp[3*i0+0],pp[3*i0+1],pp[3*i0+2]);
			cc[3*i0+0] = c.r;	cc[3*i0+1] = c.g;	cc[3*i0+2] = c.b;
			tt[i0] = ScalePoint(pp[3*i0+0],pp[3*i0+1],pp[3*i0+2]);
		}
		mesh_plot(n, m, pp, cc, tt,how);
	}
	EndGroup();
	delete []pp;	delete []cc;	delete []tt;
}
//-----------------------------------------------------------------------------
void mglGraph::Fall(const mglData &z, const char *sch)
{
	if(z.nx<2 || z.ny<2){	SetWarn(mglWarnLow,"Fall");	return;	}
	mglData x(z.nx), y(z.ny);
	x.Fill(Min.x,Max.x);
	y.Fill(Min.y,Max.y);
	Fall(x,y,z,sch);
}
//-----------------------------------------------------------------------------
//
//	Belt series
//
//-----------------------------------------------------------------------------
void mglGraph::Belt(const mglData &x, const mglData &y, const mglData &z, const char *sch)
{
	register long i,j,k,n=z.nx,m=z.ny;
	mglColor c;
	if(x.nx!=z.nx)		{	SetWarn(mglWarnDim,"Belt");	return;	}
	if(z.nx<2 || z.ny<2){	SetWarn(mglWarnLow,"Belt");	return;	}
	if(y.nx!=z.ny && (x.ny!=z.ny || y.nx!=z.nx || y.ny!=z.ny))
	{	SetWarn(mglWarnDim);	return;	}
	if(!DrawFace)	{	Fall(x,y,z,sch);	return;	}
	static int cgid=1;	StartGroup("Belt",cgid++);
	SetScheme(sch);

	bool how = !(sch && strchr(sch,'x'));
	mreal *pp = new mreal[6*(n>m?n:m)], *cc = new mreal[8*(n>m?n:m)];
	bool *tt = new bool[2*(n>m?n:m)];
	for(k=0;k<z.nz;k++)
	{
		if(how)	for(i=0;i<n;i++)
		{
			for(j=0;j<m;j++)	// создаем массив точек
			{
				if(i>0)
				{
					pp[6*j+0] = (GetX(x,i,j,k)+GetX(x,i-1,j,k))/2;
					pp[6*j+1] = (GetY(y,i,j,k)+GetY(y,i-1,j,k))/2;
				}
				else
				{
					pp[6*j+0] = GetX(x,i,j,k);
					pp[6*j+1] = GetY(y,i,j,k);
				}
				if(i<n-1)
				{
					pp[6*j+3] = (GetX(x,i,j,k)+GetX(x,i+1,j,k))/2;
					pp[6*j+4] = (GetY(y,i,j,k)+GetY(y,i+1,j,k))/2;
				}
				else
				{
					pp[6*j+3] = GetX(x,i,j,k);
					pp[6*j+4] = GetY(y,i,j,k);
				}
				pp[6*j+2] = pp[6*j+5] = z.a[i+n*(j+m*k)];
				c = GetC(pp[6*j+0],pp[6*j+1],pp[6*j+2]);
				cc[8*j+0] = c.r;	cc[8*j+1] = c.g;	cc[8*j+2] = c.b;
				c = GetC(pp[6*j+3],pp[6*j+4],pp[6*j+5]);
				cc[8*j+4] = c.r;	cc[8*j+5] = c.g;	cc[8*j+6] = c.b;
				cc[8*j+3] = cc[8*j+7] = Transparent ? AlphaDef : 1;
				tt[2*j] = ScalePoint(pp[6*j+0],pp[6*j+1],pp[6*j+2]);
				tt[2*j+1]=ScalePoint(pp[6*j+3],pp[6*j+4],pp[6*j+5]);
			}
			surf_plot(2,m,pp,cc,tt);
		}
		else	for(j=0;j<m;j++)
		{
			for(i=0;i<n;i++)	// создаем массив точек
			{
				if(j>0)
				{
					pp[6*i+0] = (GetX(x,i,j,k)+GetX(x,i,j-1,k))/2;
					pp[6*i+1] = (GetY(y,i,j,k)+GetY(y,i,j-1,k))/2;
				}
				else
				{
					pp[6*i+0] = GetX(x,i,j,k);
					pp[6*i+1] = GetY(y,i,j,k);
				}
				if(j<m-1)
				{
					pp[6*i+3] = (GetX(x,i,j,k)+GetX(x,i,j+1,k))/2;
					pp[6*i+4] = (GetY(y,i,j,k)+GetY(y,i,j+1,k))/2;
				}
				else
				{
					pp[6*i+3] = GetX(x,i,j,k);
					pp[6*i+4] = GetY(y,i,j,k);
				}
				pp[6*i+2] = pp[6*i+5] = z.a[i+n*(j+m*k)];
				c = GetC(pp[6*i+0],pp[6*i+1],pp[6*i+2]);
				cc[8*i+0] = c.r;	cc[8*i+1] = c.g;	cc[8*i+2] = c.b;
				c = GetC(pp[6*i+3],pp[6*i+4],pp[6*i+5]);
				cc[8*i+4] = c.r;	cc[8*i+5] = c.g;	cc[8*i+6] = c.b;
				cc[8*i+3] = cc[8*i+7] = Transparent ? AlphaDef : 1;
				tt[2*i] = ScalePoint(pp[6*i+0],pp[6*i+1],pp[6*i+2]);
				tt[2*i+1]=ScalePoint(pp[6*i+3],pp[6*i+4],pp[6*i+5]);
			}
			surf_plot(2,n,pp,cc,tt);
		}
	}
	EndGroup();
	delete []pp;	delete []cc;	delete []tt;
}
//-----------------------------------------------------------------------------
void mglGraph::Belt(const mglData &z, const char *sch)
{
	if(z.nx<2 || z.ny<2){	SetWarn(mglWarnLow,"Belt");	return;	}
	mglData x(z.nx), y(z.ny);
	x.Fill(Min.x,Max.x);
	y.Fill(Min.y,Max.y);
	Belt(x,y,z,sch);
}
//-----------------------------------------------------------------------------
//
//	Grid series
//
//-----------------------------------------------------------------------------
void mglGraph::Grid(const mglData &x, const mglData &y, const mglData &z, const char *sch, mreal zVal)
{
	register long i,j,i0,k,n=z.nx,m=z.ny;
	if(x.nx!=z.nx)		{	SetWarn(mglWarnDim,"Grid");	return;	}
	if(z.nx<2 || z.ny<2){	SetWarn(mglWarnLow,"Grid");	return;	}
	if(y.nx!=z.ny && (x.ny!=z.ny || y.nx!=z.nx || y.ny!=z.ny))
	{	SetWarn(mglWarnDim);	return;	}
	static int cgid=1;	StartGroup("Grid",cgid++);
	if(isnan(zVal))	zVal = Min.z;
	if(sch)	SelectPen(sch);
	else	SelectPen("k-");

	mreal *pp = new mreal[3*n*m];
	bool *tt = new bool[n*m];
	for(k=0;k<z.nz;k++)
	{
		if(z.nz>1)	zVal = Min.z+(Max.z-Min.z)*mreal(k)/(z.nz-1);
		for(i=0;i<n;i++)	for(j=0;j<m;j++)	// создаем массив точек
		{
			i0 = i+n*j;
			pp[3*i0+0] = GetX(x,i,j,k);
			pp[3*i0+1] = GetY(y,i,j,k);
			pp[3*i0+2] = zVal;
			tt[i0] = ScalePoint(pp[3*i0+0],pp[3*i0+1],pp[3*i0+2]);
		}
		mesh_plot(n, m, pp, 0, tt,3);
	}
	EndGroup();
	delete []pp;	delete []tt;
}
//-----------------------------------------------------------------------------
void mglGraph::Grid(const mglData &z, const char *sch, mreal zVal)
{
	if(z.nx<2 || z.ny<2){	SetWarn(mglWarnLow,"Grid");	return;	}
	mglData x(z.nx), y(z.ny);
	x.Fill(Min.x,Max.x);
	y.Fill(Min.y,Max.y);
	Grid(x,y,z,sch,zVal);
}
//-----------------------------------------------------------------------------
//
//	Surf series
//
//-----------------------------------------------------------------------------
void mglGraph::Surf(const mglData &x, const mglData &y, const mglData &z, const char *sch)
{
	register long i,j,i0,k,n=z.nx,m=z.ny;
	mglColor col;
	if(x.nx!=z.nx)		{	SetWarn(mglWarnDim,"Surf");	return;	}
	if(z.nx<2 || z.ny<2){	SetWarn(mglWarnLow,"Surf");	return;	}
	if(y.nx!=z.ny && (x.ny!=z.ny || y.nx!=z.nx || y.ny!=z.ny))
	{	SetWarn(mglWarnDim);	return;	}
	static int cgid=1;	StartGroup("Surf",cgid++);
	SetScheme(sch);

	mreal *pp = new mreal[3*n*m], *cc = new mreal[4*n*m];
	bool *tt = new bool[n*m];
	for(k=0;k<z.nz;k++)
	{
		for(i=0;i<n;i++)	for(j=0;j<m;j++)	// создаем массив точек
		{
			i0 = i+n*j;
			pp[3*i0+0] = GetX(x,i,j,k);
			pp[3*i0+1] = GetY(y,i,j,k);
			pp[3*i0+2] = z.a[i+n*(j+m*k)];
			col = GetC(pp[3*i0+0],pp[3*i0+1],pp[3*i0+2]);
			cc[4*i0+0] = col.r;	cc[4*i0+1] = col.g;
			cc[4*i0+2] = col.b;	cc[4*i0+3] = Transparent ? AlphaDef : 1;
			tt[i0] = ScalePoint(pp[3*i0+0],pp[3*i0+1],pp[3*i0+2]);
		}
		surf_plot(n, m, pp, cc, tt);
	}
	if(sch && strchr(sch,'#'))
	{	SelectPen("k-");	mesh_plot(n,m,pp,0,tt,-3);	}
	EndGroup();
	delete []pp;	delete []cc;	delete []tt;
}
//-----------------------------------------------------------------------------
void mglGraph::Surf(const mglData &z, const char *sch)
{
	if(z.nx<2 || z.ny<2){	SetWarn(mglWarnLow,"Surf");	return;	}
	mglData x(z.nx), y(z.ny);
	x.Fill(Min.x,Max.x);
	y.Fill(Min.y,Max.y);
	Surf(x,y,z,sch);
}
//-----------------------------------------------------------------------------
//
//	Dens series
//
//-----------------------------------------------------------------------------
void mglGraph::Dens(const mglData &x, const mglData &y, const mglData &z, const char *sch,mreal zVal)
{
	register long i,j,i0,k,n=z.nx,m=z.ny;
	mglColor col;
	if(x.nx!=z.nx)		{	SetWarn(mglWarnDim,"Dens");	return;	}
	if(z.nx<2 || z.ny<2){	SetWarn(mglWarnLow,"Dens");	return;	}
	if(y.nx!=z.ny && (x.ny!=z.ny || y.nx!=z.nx || y.ny!=z.ny))
	{	SetWarn(mglWarnDim);	return;	}
	static int cgid=1;	StartGroup("Dens",cgid++);
	if(isnan(zVal))	zVal = Min.z;
	SetScheme(sch);

	mreal *pp = new mreal[3*n*m], *cc = new mreal[4*n*m];
	bool *tt = new bool[n*m];
	for(k=0;k<z.nz;k++)
	{
		if(z.nz>1)	zVal = Min.z+(Max.z-Min.z)*mreal(k)/(z.nz-1);
		for(i=0;i<n;i++)	for(j=0;j<m;j++)	// создаем массив точек
		{
			i0 = i+n*j;
			pp[3*i0+0] = GetX(x,i,j,k);
			pp[3*i0+1] = GetY(y,i,j,k);
			pp[3*i0+2] = zVal;
			col = GetC(z.a[i+n*(j+m*k)]);
			cc[4*i0+0] = col.r;	cc[4*i0+1] = col.g;
			cc[4*i0+2] = col.b;	cc[4*i0+3] = Transparent ? AlphaDef : 1;
			tt[i0] = ScalePoint(pp[3*i0+0],pp[3*i0+1],pp[3*i0+2]);
			if(isnan(z.a[i+n*(j+m*k)]))	tt[i0] = false;
		}
		surf_plot(n, m, pp, cc, tt);
	}
	if(sch && strchr(sch,'#'))
	{	SelectPen("k-");	mesh_plot(n,m,pp,0,tt,-3);	}
	EndGroup();
	delete []pp;	delete []cc;	delete []tt;
}
//-----------------------------------------------------------------------------
void mglGraph::Dens(const mglData &z, const char *sch,mreal zVal)
{
	if(z.nx<2 || z.ny<2){	SetWarn(mglWarnLow,"Dens");	return;	}
	mglData x(z.nx), y(z.ny);
	x.Fill(Min.x,Max.x);
	y.Fill(Min.y,Max.y);
	Dens(x,y,z,sch,zVal);
}
//-----------------------------------------------------------------------------
//
//	STFA series
//
//-----------------------------------------------------------------------------
void mglGraph::STFA(const mglData &x, const mglData &y, const mglData &re, const mglData &im, int dn, const char *sch,mreal zVal)
{
	mglData z = mglSTFA(re,im,dn,'x');
	Dens(x,y,z,sch,zVal);
}
//-----------------------------------------------------------------------------
void mglGraph::STFA(const mglData &re, const mglData &im, int dn, const char *sch,mreal zVal)
{
	mglData z = mglSTFA(re,im,dn,'x');
	Dens(z,sch,zVal);
}
//-----------------------------------------------------------------------------
//
//	SurfC series
//
//-----------------------------------------------------------------------------
void mglGraph::SurfC(const mglData &x, const mglData &y, const mglData &z, const mglData &c, const char *sch)
{
	register long i,j,i0,k,n=z.nx,m=z.ny;
	mglColor col;
	if(x.nx!=z.nx)		{	SetWarn(mglWarnDim,"SurfC");	return;	}
	if(z.nx<2 || z.ny<2){	SetWarn(mglWarnLow,"SurfC");	return;	}
	if(z.nx*z.ny*z.nz!=c.nx*c.ny*c.nz)
	{	SetWarn(mglWarnDim);	return;	}
	if(y.nx!=z.ny && (x.ny!=z.ny || y.nx!=z.nx || y.ny!=z.ny))
	{	SetWarn(mglWarnDim);	return;	}
	static int cgid=1;	StartGroup("SurfC",cgid++);
	SetScheme(sch);

	mreal *pp = new mreal[3*n*m], *cc = new mreal[4*n*m];
	bool *tt = new bool[n*m];
	for(k=0;k<z.nz;k++)
	{
		for(i=0;i<n;i++)	for(j=0;j<m;j++)	// создаем массив точек
		{
			i0 = i+n*j;
			pp[3*i0+0] = GetX(x,i,j,k);
			pp[3*i0+1] = GetY(y,i,j,k);
			pp[3*i0+2] = z.a[i+n*(j+m*k)];
			col = GetC(c.a[i+n*(j+m*k)]);
			cc[4*i0+0] = col.r;	cc[4*i0+1] = col.g;
			cc[4*i0+2] = col.b;	cc[4*i0+3] = Transparent ? AlphaDef : 1;
			tt[i0] = ScalePoint(pp[3*i0+0],pp[3*i0+1],pp[3*i0+2]);
			if(isnan(c.a[i+n*(j+m*k)]))	tt[i0] = false;
		}
		surf_plot(n, m, pp, cc, tt);
	}
	if(sch && strchr(sch,'#'))
	{	SelectPen("k-");	mesh_plot(n,m,pp,0,tt,-3);	}
	EndGroup();
	delete []pp;	delete []cc;	delete []tt;
}
//-----------------------------------------------------------------------------
void mglGraph::SurfC(const mglData &z, const mglData &c,const char *sch)
{
	if(z.nx<2 || z.ny<2){	SetWarn(mglWarnLow,"SurfC");	return;	}
	mglData x(z.nx), y(z.ny);
	x.Fill(Min.x,Max.x);
	y.Fill(Min.y,Max.y);
	SurfC(x,y,z,c,sch);
}
//-----------------------------------------------------------------------------
//
//	SurfA series
//
//-----------------------------------------------------------------------------
void mglGraph::SurfA(const mglData &x, const mglData &y, const mglData &z, const mglData &c, const char *sch)
{
	register long i,j,i0,ii;
	long k,n=z.nx,m=z.ny;
	mglColor col;
	if(x.nx!=z.nx)		{	SetWarn(mglWarnDim,"SurfA");	return;	}
	if(z.nx<2 || z.ny<2){	SetWarn(mglWarnLow,"SurfA");	return;	}
	if(z.nx*z.ny*z.nz!=c.nx*c.ny*c.nz)
	{	SetWarn(mglWarnDim);	return;	}
	if(y.nx!=z.ny && (x.ny!=z.ny || y.nx!=z.nx || y.ny!=z.ny))
	{	SetWarn(mglWarnDim);	return;	}
	static int cgid=1;	StartGroup("SurfA",cgid++);
	SetScheme(sch);

	mreal *pp = new mreal[3*n*m], *cc = new mreal[4*n*m];
	bool *tt = new bool[n*m];
	for(k=0;k<z.nz;k++)
	{
		for(i=0;i<n;i++)	for(j=0;j<m;j++)	// создаем массив точек
		{
			i0 = i+n*j;
			ii = i+n*(j+m*k);
			pp[3*i0+0] = GetX(x,i,j,k);
			pp[3*i0+1] = GetY(y,i,j,k);
			pp[3*i0+2] = z.a[i+n*(j+m*k)];
			col = GetC(pp[3*i0+0],pp[3*i0+1],pp[3*i0+2]);
			cc[4*i0+0] = col.r;	cc[4*i0+1] = col.g;
			cc[4*i0+2] = col.b;	cc[4*i0+3] = (GetA(c.a[ii])+1)*(GetA(c.a[ii])+1)/4;
			tt[i0] = ScalePoint(pp[3*i0+0],pp[3*i0+1],pp[3*i0+2]);
			if(isnan(c.a[i+n*(j+m*k)]))	tt[i0] = false;
		}
		surf_plot(n, m, pp, cc, tt);
	}
	if(sch && strchr(sch,'#'))
	{	SelectPen("k-");	mesh_plot(n,m,pp,0,tt,-3);	}
	EndGroup();
	delete []pp;	delete []cc;	delete []tt;
}
//-----------------------------------------------------------------------------
void mglGraph::SurfA(const mglData &z, const mglData &c,const char *sch)
{
	if(z.nx<2 || z.ny<2){	SetWarn(mglWarnLow,"SurfA");	return;	}
	mglData x(z.nx), y(z.ny);
	x.Fill(Min.x,Max.x);
	y.Fill(Min.y,Max.y);
	SurfA(x,y,z,c,sch);
}
//-----------------------------------------------------------------------------
//
//	Boxs series
//
//-----------------------------------------------------------------------------
void mglGraph::Boxs(const mglData &x, const mglData &y, const mglData &z, const char *sch,mreal zVal)
{
	register long i,j,k,n=z.nx,m=z.ny,i0,i1,jj;
	if(x.nx!=z.nx)		{	SetWarn(mglWarnDim,"Boxs");	return;	}
	if(z.nx<2 || z.ny<2){	SetWarn(mglWarnLow,"Boxs");	return;	}
	if(y.nx!=z.ny && (x.ny!=z.ny || y.nx!=z.nx || y.ny!=z.ny))
	{	SetWarn(mglWarnDim);	return;	}
	static int cgid=1;	StartGroup("Boxs",cgid++);
	if(isnan(zVal))	zVal = GetOrgZ('x');
	SetScheme(sch);

	mreal *pp = new mreal[3*(n*m+4*(n-1)*(m-1))];
	mglColor *cc = new mglColor[n*m];
	bool *tt = new bool[n*m+4*(n-1)*(m-1)];

	for(k=0;k<z.nz;k++)
	{
		for(i=0;i<n;i++)	for(j=0;j<m;j++)
		{
			i0 = i+n*j;
			jj = i0 + n*m*k;
			i1 = 4*(i+(n-1)*j)+n*m;

			cc[i0] = GetC(z.a[jj]);
			pp[3*i0+0] = GetX(x,i,j,k);	pp[3*i0+1] = GetY(y,i,j,k);	pp[3*i0+2] = zVal;
			tt[i0] = ScalePoint(pp[3*i0+0],pp[3*i0+1],pp[3*i0+2]);
			if(i<n-1 && j<m-1)
			{
				pp[3*i1+2] = pp[3*i1+5] = pp[3*i1+8] = pp[3*i1+11] = z.a[jj];
				pp[3*i1+0] = GetX(x,i,j,k);	pp[3*i1+1] = GetY(y,i,j,k);
				tt[i1] = ScalePoint(pp[3*i1+0],pp[3*i1+1],pp[3*i1+2]);
				pp[3*i1+3] = GetX(x,i+1,j,k);	pp[3*i1+4] = GetY(y,i+1,j,k);
				tt[i1+1] = ScalePoint(pp[3*i1+3],pp[3*i1+4],pp[3*i1+5]);
				pp[3*i1+6] = GetX(x,i,j+1,k);	pp[3*i1+7] = GetY(y,i,j+1,k);
				tt[i1+2] = ScalePoint(pp[3*i1+6],pp[3*i1+7],pp[3*i1+8]);
				pp[3*i1+9] = GetX(x,i+1,j+1,k);	pp[3*i1+10] = GetY(y,i+1,j+1,k);
				tt[i1+3] = ScalePoint(pp[3*i1+9],pp[3*i1+10],pp[3*i1+11]);
			}
		}
		boxs_plot(n, m, pp, cc, tt, Transparent ? AlphaDef : 1,	sch && strchr(sch,'#'));
	}
	EndGroup();
	delete []pp;	delete []cc;	delete []tt;
}
//-----------------------------------------------------------------------------
void mglGraph::Boxs(const mglData &z, const char *sch,mreal zVal)
{
	if(z.nx<2 || z.ny<2){	SetWarn(mglWarnLow,"Boxs");	return;	}
	mglData x(z.nx), y(z.ny);
	x.Fill(Min.x,Max.x);
	y.Fill(Min.y,Max.y);
	Boxs(x,y,z,sch,zVal);
}
//-----------------------------------------------------------------------------
//
//	Tile series
//
//-----------------------------------------------------------------------------
void mglGraph::Tile(const mglData &x, const mglData &y, const mglData &z, const char *sch)
{
	register long i,j,k,n=z.nx,m=z.ny,i0,i1;
	if(x.nx!=z.nx)		{	SetWarn(mglWarnDim,"Tile");	return;	}
	if(z.nx<2 || z.ny<2){	SetWarn(mglWarnLow,"Tile");	return;	}
	if(y.nx!=z.ny && (x.ny!=z.ny || y.nx!=z.nx || y.ny!=z.ny))
	{	SetWarn(mglWarnDim);	return;	}
	static int cgid=1;	StartGroup("Tile",cgid++);
	SetScheme(sch);

	mreal *pp = new mreal[12*(n-1)*(m-1)];
	mreal *cc = new mreal[16*(n-1)*(m-1)];
	bool *tt = new bool[4*(n-1)*(m-1)];
	mglColor c;

	for(k=0;k<z.nz;k++)
	{
		for(i=0;i<n-1;i++)	for(j=0;j<m-1;j++)
		{
			i0 = i+(n-1)*j;
			i1 = i+n*j;

			c = GetC(z.a[i1 + n*m*k]);
			cc[16*i0] = cc[16*i0+4] = cc[16*i0+8] = cc[16*i0+12] = c.r;
			cc[16*i0+1] = cc[16*i0+5] = cc[16*i0+9] = cc[16*i0+13] = c.g;
			cc[16*i0+2] = cc[16*i0+6] = cc[16*i0+10] = cc[16*i0+14] = c.b;
			cc[16*i0+3] = cc[16*i0+7] = cc[16*i0+11] = cc[16*i0+15] = Transparent ? AlphaDef:1;
			pp[12*i0+2] = pp[12*i0+5] = pp[12*i0+8] = pp[12*i0+11] =
				(z.a[i1 + n*m*k]+z.a[i1+1 + n*m*k]+z.a[i1+n + n*m*k]+z.a[i1+1+n + n*m*k])/4;

			pp[12*i0+0] = GetX(x,i,j,k);	pp[12*i0+1] = GetY(y,i,j,k);
			pp[12*i0+3] = GetX(x,i+1,j,k);	pp[12*i0+4] = GetY(y,i+1,j,k);
			pp[12*i0+6] = GetX(x,i+1,j+1,k);	pp[12*i0+7] = GetY(y,i+1,j+1,k);
			pp[12*i0+9] = GetX(x,i,j+1,k);	pp[12*i0+10] = GetY(y,i,j+1,k);
			tt[4*i0] = ScalePoint(pp[12*i0+0],pp[12*i0+1],pp[12*i0+2]);
			tt[4*i0+1] = ScalePoint(pp[12*i0+3],pp[12*i0+4],pp[12*i0+5]);
			tt[4*i0+2] = ScalePoint(pp[12*i0+6],pp[12*i0+7],pp[12*i0+8]);
			tt[4*i0+3] = ScalePoint(pp[12*i0+9],pp[12*i0+10],pp[12*i0+11]);
		}
		quads_plot((n-1)*(m-1), pp, cc, tt);
	}
	EndGroup();
	delete []pp;	delete []cc;	delete []tt;
}
//-----------------------------------------------------------------------------
void mglGraph::Tile(const mglData &z, const char *sch)
{
	if(z.nx<2 || z.ny<2){	SetWarn(mglWarnLow,"Tile");	return;	}
	mglData x(z.nx), y(z.ny);
	x.Fill(Min.x,Max.x);
	y.Fill(Min.y,Max.y);
	Tile(x,y,z,sch);
}
//-----------------------------------------------------------------------------
void mglGraph::TileS(const mglData &x, const mglData &y, const mglData &z, const mglData &s, const char *sch)
{
	register long i,j,k,n=z.nx,m=z.ny,i0,i1;
	if(x.nx!=z.nx || s.nx*s.ny*s.nz!=z.nx*z.ny*z.nz)
	{	SetWarn(mglWarnDim,"Tile");	return;	}
	if(z.nx<2 || z.ny<2){	SetWarn(mglWarnLow,"Tile");	return;	}
	if(y.nx!=z.ny && (x.ny!=z.ny || y.nx!=z.nx || y.ny!=z.ny))
	{	SetWarn(mglWarnDim);	return;	}
	static int cgid=1;	StartGroup("TileS",cgid++);
	SetScheme(sch);

	mreal *pp = new mreal[12*(n-1)*(m-1)], ss;
	mreal *cc = new mreal[16*(n-1)*(m-1)];
	bool *tt = new bool[4*(n-1)*(m-1)];
	mreal x1,x2,x3,x4,y1,y2,y3,y4;
	mglColor c;

	for(k=0;k<z.nz;k++)
	{
		for(i=0;i<n-1;i++)	for(j=0;j<m-1;j++)
		{
			i0 = i+(n-1)*j;
			i1 = i+n*j;

			c = GetC(z.a[i1 + n*m*k]);
			ss = (s.a[i1 + n*m*k] + s.a[i1+1 + n*m*k] + s.a[i1+n + n*m*k] + s.a[i1+1+n + n*m*k])/4.f;
			ss = (1-GetA(ss))/4;
			cc[16*i0] = cc[16*i0+4] = cc[16*i0+8] = cc[16*i0+12] = c.r;
			cc[16*i0+1] = cc[16*i0+5] = cc[16*i0+9] = cc[16*i0+13] = c.g;
			cc[16*i0+2] = cc[16*i0+6] = cc[16*i0+10] = cc[16*i0+14] = c.b;
			cc[16*i0+3] = cc[16*i0+7] = cc[16*i0+11] = cc[16*i0+15] = Transparent ? AlphaDef:1;
			pp[12*i0+2] = pp[12*i0+5] = pp[12*i0+8] = pp[12*i0+11] =
				(z.a[i1 + n*m*k]+z.a[i1+1 + n*m*k]+z.a[i1+n + n*m*k]+z.a[i1+1+n + n*m*k])/4;

			x1 = GetX(x,i,j,k);			y1 = GetY(y,i,j,k);
			x2 = GetX(x,i+1,j,k)-x1;	y2 = GetY(y,i+1,j,k)-y1;
			x4 = GetX(x,i,j+1,k)-x1;	y4 = GetY(y,i,j+1,k)-y1;
			x3 = GetX(x,i+1,j+1,k)-x2-x4-x1;
			y3 = GetY(y,i+1,j+1,k)-y2-y4-y1;
			pp[12*i0+0] = x1+x2*ss+x4*ss+x3*ss*ss;
			pp[12*i0+1] = y1+y2*ss+y4*ss+y3*ss*ss;
			pp[12*i0+3] = x1+x2*(1-ss)+x4*ss+x3*ss*(1-ss);
			pp[12*i0+4] = y1+y2*(1-ss)+y4*ss+y3*ss*(1-ss);
			pp[12*i0+6] = x1+x2*(1-ss)+x4*(1-ss)+x3*(1-ss)*(1-ss);
			pp[12*i0+7] = y1+y2*(1-ss)+y4*(1-ss)+y3*(1-ss)*(1-ss);
			pp[12*i0+9] = x1+x2*ss+x4*(1-ss)+x3*ss*(1-ss);
			pp[12*i0+10]= y1+y2*ss+y4*(1-ss)+y3*ss*(1-ss);
			tt[4*i0] = ScalePoint(pp[12*i0+0],pp[12*i0+1],pp[12*i0+2]);
			tt[4*i0+1] = ScalePoint(pp[12*i0+3],pp[12*i0+4],pp[12*i0+5]);
			tt[4*i0+2] = ScalePoint(pp[12*i0+6],pp[12*i0+7],pp[12*i0+8]);
			tt[4*i0+3] = ScalePoint(pp[12*i0+9],pp[12*i0+10],pp[12*i0+11]);
		}
		quads_plot((n-1)*(m-1), pp, cc, tt);
	}
	EndGroup();
	delete []pp;	delete []cc;	delete []tt;
}
//-----------------------------------------------------------------------------
void mglGraph::TileS(const mglData &z, const mglData &s, const char *sch)
{
	if(z.nx<2 || z.ny<2){	SetWarn(mglWarnLow,"Tile");	return;	}
	mglData x(z.nx), y(z.ny);
	x.Fill(Min.x,Max.x);
	y.Fill(Min.y,Max.y);
	TileS(x,y,z,s,sch);
}
//-----------------------------------------------------------------------------
//		2D plotting functions
//-----------------------------------------------------------------------------
/// Draw grid lines for density plot of 2d data specified parametrically
void mgl_grid_xy(HMGL gr, const HMDT x, const HMDT y, const HMDT a, const char *sch,mreal zVal)
{	if(gr && a && x && y)	gr->Grid(*x, *y, *a, sch, zVal);	}
/// Draw grid lines for density plot of 2d data
void mgl_grid(HMGL gr, const HMDT a,const char *sch,mreal zVal)
{	if(gr && a)	gr->Grid(*a, sch, zVal);	}
/// Draw mesh lines for 2d data specified parametrically
void mgl_mesh_xy(HMGL gr, const HMDT x, const HMDT y, const HMDT a, const char *sch)
{	if(gr && a && x && y)	gr->Mesh(*x, *y, *a, sch);	}
/// Draw mesh lines for 2d data
void mgl_mesh(HMGL gr, const HMDT a, const char *sch)
{	if(gr && a)	gr->Mesh(*a, sch);	}
/// Draw fall lines for 2d data specified parametrically
void mgl_fall_xy(HMGL gr, const HMDT x, const HMDT y, const HMDT a, const char *sch)
{	if(gr && a && x && y)	gr->Fall(*x, *y, *a, sch);	}
/// Draw fall lines for 2d data
void mgl_fall(HMGL gr, const HMDT a, const char *sch)
{	if(gr && a)	gr->Fall(*a, sch);	}
/// Draw belt lines for 2d data specified parametrically
void mgl_belt_xy(HMGL gr, const HMDT x, const HMDT y, const HMDT a, const char *sch)
{	if(gr && a && x && y)	gr->Belt(*x, *y, *a, sch);	}
/// Draw belt lines for 2d data
void mgl_belt(HMGL gr, const HMDT a, const char *sch)
{	if(gr && a)	gr->Belt(*a, sch);	}
/// Draw surface for 2d data specified parametrically
void mgl_surf_xy(HMGL gr, const HMDT x, const HMDT y, const HMDT a, const char *sch)
{	if(gr && a && x && y)	gr->Surf(*x, *y, *a, sch);	}
/// Draw surface for 2d data
void mgl_surf(HMGL gr, const HMDT a, const char *sch)
{	if(gr && a)	gr->Surf(*a, sch);	}
/// Draw density plot for surface specified parametrically
void mgl_dens_xy(HMGL gr, const HMDT x, const HMDT y, const HMDT a, const char *sch,mreal zVal)
{	if(gr && a && x && y)	gr->Dens(*x, *y, *a, sch, zVal);	}
/// Draw density plot for 2d data
void mgl_dens(HMGL gr, const HMDT a, const char *sch,mreal zVal)
{	if(gr && a)	gr->Dens(*a, sch, zVal);	}
/// Draw vertical boxes for 2d data specified parametrically
void mgl_boxs_xy(HMGL gr, const HMDT x, const HMDT y, const HMDT a, const char *sch,mreal zVal)
{	if(gr && a && x && y)	gr->Boxs(*x, *y, *a, sch,zVal);	}
/// Draw vertical boxes for 2d data
void mgl_boxs(HMGL gr, const HMDT a, const char *sch,mreal zVal)
{	if(gr && a)	gr->Boxs(*a, sch,zVal);	}
/// Draw tiles for 2d data specified parametrically
void mgl_tile_xy(HMGL gr, const HMDT x, const HMDT y, const HMDT a, const char *sch)
{	if(gr && a && x && y)	gr->Tile(*x, *y, *a, sch);	}
/// Draw mesh lines for 2d data
void mgl_tile(HMGL gr, const HMDT a, const char *sch)
{	if(gr && a)	gr->Tile(*a, sch);	}
/// Draw variable-size tiles for 2d data specified parametrically
void mgl_tiles_xy(HMGL gr, const HMDT x, const HMDT y, const HMDT a, const HMDT r, const char *sch)
{	if(gr && a && x && y && r)	gr->TileS(*x, *y, *a, *r, sch);	}
/// Draw variable-size mesh lines for 2d data
void mgl_tiles(HMGL gr, const HMDT a, const HMDT r, const char *sch)
{	if(gr && a && r)	gr->TileS(*a, *r, sch);	}
//-----------------------------------------------------------------------------
/// Draw surface specified parametrically with coloring by other matrix
void mgl_surfc_xy(HMGL gr, const HMDT x, const HMDT y, const HMDT z, const HMDT a, const char *sch)
{	if(gr && a && z && x && y)	gr->SurfC(*x, *y, *z, *a, sch);	}
/// Draw surface specified by matrix with coloring by other matrix
void mgl_surfc(HMGL gr, const HMDT z, const HMDT a, const char *sch)
{	if(gr && a && z)	gr->SurfC(*z, *a, sch);	}
/// Draw surface specified parametrically which transparency is determined by other matrix
void mgl_surfa_xy(HMGL gr, const HMDT x, const HMDT y, const HMDT z, const HMDT a, const char *sch)
{	if(gr && a && z && x && y)	gr->SurfA(*x, *y, *z, *a, sch);	}
/// Draw surface specified by matrix which transparency is determined by other matrix
void mgl_surfa(HMGL gr, const HMDT z, const HMDT a, const char *sch)
{	if(gr && a && z)	gr->SurfA(*z, *a, sch);	}
/// Draw spectrogram for data specified parametrically which transparency is determined by other matrix
void mgl_stfa_xy(HMGL gr, const HMDT x, const HMDT y, const HMDT re, const HMDT im, int dn, const char *sch, mreal zval)
{	if(gr && re && im && x && y)	gr->STFA(*x, *y, *re, *im, dn, sch, zval);	}
/// Draw spectrogram for data specified by matrix which transparency is determined by other matrix
void mgl_stfa(HMGL gr, const HMDT re, const HMDT im, int dn, const char *sch, mreal zval)
{	if(gr && re && im)	gr->STFA(*re, *im, dn, sch, zval);	}
//-----------------------------------------------------------------------------
//		2D plotting functions (Fortran)
//-----------------------------------------------------------------------------
/// Draw grid lines for density plot of 2d data specified parametrically
void mgl_grid_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *a, const char *sch,mreal *zVal,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a && x && y)	_GR_->Grid(_D_(x), _D_(y), _D_(a), s, *zVal);
	delete []s;
}
/// Draw grid lines for density plot of 2d data
void mgl_grid_(uintptr_t *gr, uintptr_t *a,const char *sch,mreal *zVal,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a)	_GR_->Grid(_D_(a), s, *zVal);
	delete []s;
}
/// Draw mesh lines for 2d data specified parametrically
void mgl_mesh_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *a, const char *sch,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a && x && y)	_GR_->Mesh(_D_(x), _D_(y), _D_(a), s);
	delete []s;
}
/// Draw mesh lines for 2d data
void mgl_mesh_(uintptr_t *gr, uintptr_t *a, const char *sch,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a)	_GR_->Mesh(_D_(a), s);
	delete []s;
}
/// Draw fall lines for 2d data specified parametrically
void mgl_fall_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *a, const char *sch,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a && x && y)	_GR_->Fall(_D_(x), _D_(y), _D_(a), s);
	delete []s;
}
/// Draw fall lines for 2d data
void mgl_fall_(uintptr_t *gr, uintptr_t *a, const char *sch,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a)	_GR_->Fall(_D_(a), s);
	delete []s;
}
/// Draw belt lines for 2d data specified parametrically
void mgl_belt_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *a, const char *sch,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a && x && y)	_GR_->Belt(_D_(x), _D_(y), _D_(a), s);
	delete []s;
}
/// Draw belt lines for 2d data
void mgl_belt_(uintptr_t *gr, uintptr_t *a, const char *sch,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a)	_GR_->Belt(_D_(a), s);
	delete []s;
}
/// Draw surface for 2d data specified parametrically
void mgl_surf_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *a, const char *sch,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a && x && y)	_GR_->Surf(_D_(x), _D_(y), _D_(a), s);
	delete []s;
}
/// Draw surface for 2d data
void mgl_surf_(uintptr_t *gr, uintptr_t *a, const char *sch,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a)	_GR_->Surf(_D_(a), s);
	delete []s;
}
/// Draw density plot for surface specified parametrically
void mgl_dens_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *a, const char *sch,mreal *zVal,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a && x && y)	_GR_->Dens(_D_(x), _D_(y), _D_(a), s, *zVal);
	delete []s;
}
/// Draw density plot for 2d data
void mgl_dens_(uintptr_t *gr, uintptr_t *a, const char *sch,mreal *zVal,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a)	_GR_->Dens(_D_(a), s, *zVal);
	delete []s;
}
/// Draw vertical boxes for 2d data specified parametrically
void mgl_boxs_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *a, const char *sch,mreal *zVal,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a && x && y)	_GR_->Boxs(_D_(x), _D_(y), _D_(a), s,*zVal);
	delete []s;
}
/// Draw vertical boxes for 2d data
void mgl_boxs_(uintptr_t *gr, uintptr_t *a, const char *sch,mreal *zVal,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a)	_GR_->Boxs(_D_(a), s,*zVal);
	delete []s;
}
/// Draw tiles for 2d data specified parametrically
void mgl_tile_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *a, const char *sch,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a && x && y)	_GR_->Tile(_D_(x), _D_(y), _D_(a), s);
	delete []s;
}
/// Draw mesh lines for 2d data
void mgl_tile_(uintptr_t *gr, uintptr_t *a, const char *sch,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a)	_GR_->Tile(_D_(a), s);
	delete []s;
}
/// Draw tiles for 2d data specified parametrically
void mgl_tiles_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *a, uintptr_t *r, const char *sch,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a && x && y && r)
		_GR_->TileS(_D_(x), _D_(y), _D_(a), _D_(r), s);
	delete []s;
}
/// Draw mesh lines for 2d data
void mgl_tiles_(uintptr_t *gr, uintptr_t *a, uintptr_t *r, const char *sch,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a && r)	_GR_->TileS(_D_(a), _D_(r), s);
	delete []s;
}
//-----------------------------------------------------------------------------
/// Draw surface specified parametrically with coloring by other matrix
void mgl_surfc_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *a, const char *sch,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a && z && x && y)	_GR_->SurfC(_D_(x), _D_(y), _D_(z), _D_(a), s);
	delete []s;
}
/// Draw surface specified by matrix with coloring by other matrix
void mgl_surfc_(uintptr_t *gr, uintptr_t *z, uintptr_t *a, const char *sch,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a && z)	_GR_->SurfC(_D_(z), _D_(a), s);
	delete []s;
}
/// Draw surface specified parametrically which transparency is determined by other matrix
void mgl_surfa_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *a, const char *sch,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a && z && x && y)	_GR_->SurfA(_D_(x), _D_(y), _D_(z), _D_(a), s);
	delete []s;
}
/// Draw surface specified by matrix which transparency is determined by other matrix
void mgl_surfa_(uintptr_t *gr, uintptr_t *z, uintptr_t *a, const char *sch,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a && z)	_GR_->SurfA(_D_(z), _D_(a), s);
	delete []s;
}
/// Draw spectrogram for data specified parametrically which transparency is determined by other matrix
void mgl_stfa_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *re, uintptr_t *im, int *dn, const char *sch, mreal *zVal, int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && re && im && x && y)
		_GR_->STFA(_D_(x), _D_(y), _D_(re), _D_(im), *dn, s, *zVal);
	delete []s;
}
/// Draw spectrogram for data specified by matrix which transparency is determined by other matrix
void mgl_stfa_(uintptr_t *gr, uintptr_t *re, uintptr_t *im, int *dn, const char *sch, mreal *zVal, int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && re && im)	_GR_->STFA(_D_(re), _D_(im), *dn, s, *zVal);
	delete []s;
}
//-----------------------------------------------------------------------------
void mgl_fsurf(HMGL gr, const char *fy, const char *stl, int n)
{	if(gr)	gr->Surf(fy, stl, n);	}
void mgl_fsurf_xyz(HMGL gr, const char *fx, const char *fy, const char *fz, const char *stl, int n)
{	if(gr)	gr->Surf(fx,fy,fz,stl,n);	}
//-----------------------------------------------------------------------------
void mgl_fsurf_(uintptr_t *gr, const char *fy, const char *stl, int *n, int ly, int ls)
{
	char *s=new char[ly+1];	memcpy(s,fy,ly);	s[ly]=0;
	char *p=new char[ls+1];	memcpy(p,stl,ls);	p[ls]=0;
	if(gr)	_GR_->Surf(s, p, *n);
	delete []s;		delete []p;
}
void mgl_fsurf_xyz_(uintptr_t *gr, const char *fx, const char *fy, const char *fz, const char *stl, int *n, int lx, int ly, int lz, int ls)
{
	char *sx=new char[lx+1];	memcpy(sx,fx,lx);	sx[lx]=0;
	char *sy=new char[ly+1];	memcpy(sy,fy,ly);	sy[ly]=0;
	char *sz=new char[lz+1];	memcpy(sz,fz,lz);	sz[lz]=0;
	char *p=new char[ls+1];	memcpy(p,stl,ls);	p[ls]=0;
	if(gr)	_GR_->Plot(sx, sy, sz, p, *n);
	delete []sx;	delete []sy;	delete []sz;	delete []p;
}
//-----------------------------------------------------------------------------
