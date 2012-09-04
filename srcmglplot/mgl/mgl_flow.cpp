/***************************************************************************
 * mgl_flow.cpp is part of Math Graphic Library
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
#include "mgl/mgl.h"
#include "mgl/mgl_c.h"
#include "mgl/mgl_f.h"
//-----------------------------------------------------------------------------
//
//	Flow series
//
//-----------------------------------------------------------------------------
void mglGraph::flow(bool simple, mreal zVal, mreal u, mreal v,
					const mglData &x, const mglData &y, const mglData &ax, const mglData &ay)
{
	long n=10*(ax.nx+ax.ny);
	bool both = x.nx==ax.nx && y.nx==ax.nx && x.ny==ax.ny && y.ny==ax.ny;
	mreal *pp, *cc;
	bool *tt;
	pp = (mreal *)malloc(3*n*sizeof(mreal));
	cc = (mreal *)malloc(n*sizeof(mreal));
	tt = (bool  *)malloc(n*sizeof(bool));
	mreal dt = 0.5/(ax.nx > ax.ny ? ax.nx : ax.ny),e,f,g,ff[4],gg[4],h,s=1;
	if(u<0 || v<0)	{	dt = -dt;	u = -u;		v = -v;		s = -1;}
	register long k=0,m;
	bool end = false;
	do{
		if(k>=n)	// resize array if need
		{
			n += 10*(ax.nx+ax.ny);
			pp = (mreal *)realloc(pp,3*n*sizeof(mreal));
			cc = (mreal *)realloc(cc,n*sizeof(mreal));
			tt = (bool  *)realloc(tt,n*sizeof(bool));
		}
		// insert point
		pp[3*k] = simple ? Min.x + (Max.x-Min.x)*u :
				(both ? x.Spline1(u,v,0):x.Spline1(u,0,0));
		pp[3*k+1] = simple ? Min.y + (Max.y-Min.y)*v :
				(both ? y.Spline1(u,v,0):y.Spline1(v,0,0));
		pp[3*k+2] = zVal;
		tt[k]  = ScalePoint(pp[3*k],pp[3*k+1],pp[3*k+2]);
		for(m=0;m<k-1;m++)	// determines encircle
		{
			f = pp[3*k]-pp[3*m];
			g = pp[3*k+1]-pp[3*m+1];
			e = pp[3*k+2]-pp[3*m+2];
			if(f*f+g*g+e*e<dt*dt/4.)	{	end = true;	break;	}
		}
		f = ax.Linear1(u,v,0);	g = ay.Linear1(u,v,0);
//		f = ax.Spline1(u,v,0);	g = ay.Spline1(u,v,0);
		h = hypot(f,g);	cc[k] = s*h;	h+=1;
		k++;
		// find next point by midpoint method
		if(h<1e-7)	break;	// stationary point
		ff[0]=f*dt/h;	gg[0]=g*dt/h;
		e = u+ff[0]/2;	h = v+gg[0]/2;
		f = ax.Linear1(e,h,0);	g = ay.Linear1(e,h,0);	h = 1+hypot(f,g);
//		f = ax.Spline1(e,h,0);	g = ay.Spline1(e,h,0);	h = 1+hypot(f,g);
		if(h<1e-7)	break;	// stationary point
		ff[1]=f*dt/h;	gg[1]=g*dt/h;
		e = u+ff[1]/2;	h = v+gg[1]/2;
		f = ax.Linear1(e,h,0);	g = ay.Linear1(e,h,0);	h = 1+hypot(f,g);
//		f = ax.Spline1(e,h,0);	g = ay.Spline1(e,h,0);	h = 1+hypot(f,g);
		if(h<1e-7)	break;	// stationary point
		ff[2]=f*dt/h;	gg[2]=g*dt/h;
		e = u+ff[2];	h = v+gg[2];
		f = ax.Linear1(e,h,0);	g = ay.Linear1(e,h,0);	h = 1+hypot(f,g);
//		f = ax.Spline1(e,h,0);	g = ay.Spline1(e,h,0);	h = 1+hypot(f,g);
		if(h<1e-7)	break;	// stationary point
		ff[3]=f*dt/h;	gg[3]=g*dt/h;
		u += ff[0]/6+ff[1]/3+ff[2]/3+ff[3]/6;
		v += gg[0]/6+gg[1]/3+gg[2]/3+gg[3]/6;
		// condition of end
		end = end || u<0 || v<0 || u>1 || v>1;
	} while(!end);
	curv_plot(k,pp,cc,tt);
	free(pp);	free(cc);	free(tt);
}
//-----------------------------------------------------------------------------
void mglGraph::Flow(const mglData &x, const mglData &y, const mglData &ax, const mglData &ay, const char *sch, int num, bool cnt, mreal zVal)
{
	mreal u,v;
	long n=ax.nx, m=ax.ny;
	if(n*m*ax.nz!=ay.nx*ay.ny*ay.nz)	{	SetWarn(mglWarnDim,"Flow");	return;	}
	if(n<2 || m<2)						{	SetWarn(mglWarnLow,"Flow");	return;	}
	static int cgid=1;	StartGroup("Flow",cgid++);
	bool both = x.nx==n && y.nx==n && x.ny==m && y.ny==m;
	if(!(both || (x.nx==n && y.nx==m)))	{	SetWarn(mglWarnDim,"Flow");	return;	}

	Arrow1 = Arrow2 = '_';
	SetScheme(sch);
	// allocate memory
	if(isnan(zVal)) zVal = Min.z;
	cnt=(num>0);	num = abs(num);	// redefine central parater

	for(long k=0;k<ax.nz;k++)
	{
		if(ax.nz>1)	zVal = Min.z+(Max.z-Min.z)*mreal(k)/(ax.nz-1);
		for(long i=0;i<num;i++)
		{
			u = 0;	v = (i+1.)/(num+1.);
			flow(false, zVal, u, v, x, y, ax, ay);
			flow(false, zVal, -u, -v, x, y, ax, ay);
			u = 1;	v = (i+1.)/(num+1.);
			flow(false, zVal, u, v, x, y, ax, ay);
			flow(false, zVal, -u, -v, x, y, ax, ay);
			u = (i+1.)/(num+1.);	v = 0;
			flow(false, zVal, u, v, x, y, ax, ay);
			flow(false, zVal, -u, -v, x, y, ax, ay);
			u = (i+1.)/(num+1.);	v = 1;
			flow(false, zVal, u, v, x, y, ax, ay);
			flow(false, zVal, -u, -v, x, y, ax, ay);
			if(cnt)
			{
				u = 0.5;	v = (i+1.)/(num+1.);
				flow(false, zVal, u, v, x, y, ax, ay);
				flow(false, zVal, -u, -v, x, y, ax, ay);
				u = (i+1.)/(num+1.);	v = 0.5;
				flow(false, zVal, u, v, x, y, ax, ay);
				flow(false, zVal, -u, -v, x, y, ax, ay);
			}
		}
	}
	EndGroup();
}
//-----------------------------------------------------------------------------
void mglGraph::Flow(const mglData &ax, const mglData &ay, const char *sch, int num,
					bool cnt, mreal zVal)
{
	if(ax.nx*ax.ny!=ay.nx*ay.ny){	SetWarn(mglWarnDim,"Flow");	return;	}
	if(ax.nx<2 || ax.ny<2)		{	SetWarn(mglWarnLow,"Flow");	return;	}
	mglData x(ax.nx), y(ax.ny);
	x.Fill(Min.x,Max.x);
	y.Fill(Min.y,Max.y);
	Flow(x,y,ax,ay,sch,num,cnt,zVal);
}
//-----------------------------------------------------------------------------
void mglGraph::FlowP(mglPoint p, const mglData &x, const mglData &y, const mglData &ax, const mglData &ay, const char *sch)
{
	mreal u,v;
	long n=ax.nx, m=ax.ny;
	if(n*m*ax.nz!=ay.nx*ay.ny*ay.nz)	{	SetWarn(mglWarnDim,"Flow");	return;	}
	if(n<2 || m<2)						{	SetWarn(mglWarnLow,"Flow");	return;	}
	static int cgid=1;	StartGroup("FlowP",cgid++);
	bool both = x.nx==n && y.nx==n && x.ny==m && y.ny==m;
	if(!(both || (x.nx==n && y.nx==m)))	{	SetWarn(mglWarnDim,"Flow");	return;	}

	Arrow1 = Arrow2 = '_';
	SetScheme(sch);
	// find coordinates u, v
	register long i,j,ii;
	register mreal d, dm=1e7;
	long i0=0,j0=0;
	for(i=0;i<n;i++)	for(j=0;j<m;j++)	// first find closest
	{
		ii = i+n*j;
		d = both ? hypot(x.a[ii]-p.x,y.a[ii]-p.y) : hypot(x.a[i]-p.x,y.a[j]-p.y);
		if(d<dm)	{	i0=i;	j0=j;	dm=d;	}
	}
	if(dm==0)	{	u = i0/mreal(n);	v = j0/mreal(m);	}	// we find it
	else
	{
		mreal dxu,dxv,dyu,dyv, dx, dy;
		if(both)
		{
			ii = i0+n*j0;
			dx = x.a[ii]-p.x;	dy = y.a[ii]-p.y;
			if(i0<n-1)
			{	dxu = x.a[ii+1]-x.a[ii];	dyu = y.a[ii+1]-y.a[ii];	}
			else
			{	dxu = x.a[ii]-x.a[ii-1];	dyu = y.a[ii]-y.a[ii-1];	}
			if(j0<m-1)
			{	dxv = x.a[ii+n]-x.a[ii];	dyv = y.a[ii+n]-y.a[ii];	}
			else
			{	dxv = x.a[ii]-x.a[ii-n];	dyv = y.a[ii]-y.a[ii-n];	}
			d = dxv*dyu-dxu*dyv;
			u = (i0+(dxv*dy-dx*dyv)/d)/n;
			v = (j0-(dxu*dy-dx*dyu)/d)/m;
		}
		else
		{
			dx = x.a[i0]-p.x;	dy = y.a[j0]-p.y;
			if(i0<n-1)	dxu = x.a[i0+1]-x.a[i0];
			else		dxu = x.a[i0]-x.a[i0-1];
			if(j0<m-1)	dyv = y.a[j0+1]-y.a[j0];
			else		dyv = y.a[j0]-y.a[j0-1];
			u = (i0+dx/dxu)/n;	v = (j0+dy/dyv)/m;
		}
	}
	flow(false, p.z, u, v, x, y, ax, ay);
	EndGroup();
}
//-----------------------------------------------------------------------------
void mglGraph::FlowP(mglPoint p, const mglData &ax, const mglData &ay, const char *sch)
{
	if(ax.nx*ax.ny!=ay.nx*ay.ny){	SetWarn(mglWarnDim,"Flow");	return;	}
	if(ax.nx<2 || ax.ny<2)		{	SetWarn(mglWarnLow,"Flow");	return;	}
	mglData x(ax.nx), y(ax.ny);
	x.Fill(Min.x,Max.x);
	y.Fill(Min.y,Max.y);
	FlowP(p,x,y,ax,ay,sch);
}
//-----------------------------------------------------------------------------
void mglGraph::flow(bool simple, mreal u, mreal v, mreal w,
					const mglData &x, const mglData &y, const mglData &z,
					const mglData &ax, const mglData &ay, const mglData &az)
{
	static long n=10*(ax.nx+ax.ny);
	long nn = ax.nx*ax.ny*ax.nz;
	bool both = x.nx*x.ny*x.nz==nn && y.nx*y.ny*y.nz==nn && z.nx*z.ny*z.nz==nn;
	mreal *pp, *cc;
	bool *tt;
	pp = (mreal *)malloc(3*n*sizeof(mreal));
	cc = (mreal *)malloc(n*sizeof(mreal));
	tt = (bool  *)malloc(n*sizeof(bool));
	nn = (ax.nx > ax.ny ? ax.nx : ax.ny);
	nn = (nn > ax.nz ? nn : ax.nz);
	mreal dt = 0.2/nn, e,f,g,ee[4],ff[4],gg[4],h,s=1,u1,v1,w1;
	if(u<0 || v<0 || w<0)
	{	dt = -dt;	u = -u;		v = -v;		w = -w;		s = -1;}
	register long k=0,m;
	bool end = false;
	do{
		if(k>=n)	// resize array if need
		{
			n += 10*(ax.nx+ax.ny);
			pp = (mreal *)realloc(pp,3*n*sizeof(mreal));
			cc = (mreal *)realloc(cc,n*sizeof(mreal));
			tt = (bool  *)realloc(tt,n*sizeof(bool));
		}
		// insert point
		pp[3*k] = simple ? Min.x + (Max.x-Min.x)*u :
				(both ? x.Spline1(u,v,w):x.Spline1(u,0,0));
		pp[3*k+1] = simple ? Min.y + (Max.y-Min.y)*v :
				(both ? y.Spline1(u,v,w):y.Spline1(v,0,0));
		pp[3*k+2] = simple ? Min.z + (Max.z-Min.z)*w :
				(both ? z.Spline1(u,v,w):z.Spline1(w,0,0));
		tt[k]  = ScalePoint(pp[3*k],pp[3*k+1],pp[3*k+2]);
		for(m=0;m<k-1;m++)	// determines encircle
		{
			f = pp[3*k]-pp[3*m];
			g = pp[3*k+1]-pp[3*m+1];
			e = pp[3*k+2]-pp[3*m+2];
			if(f*f+g*g+e*e<dt*dt/4.)	{	end = true;	break;	}
		}
		e = ax.Linear1(u,v,w);	f = ay.Linear1(u,v,w);	g = az.Linear1(u,v,w);
//		e = ax.Spline1(u,v,w);	f = ay.Spline1(u,v,w);	g = az.Spline1(u,v,w);
		h = sqrt(e*e+f*f+g*g);	cc[k] = s*h;	h+=1;
		k++;
		// find next point by midpoint method
		if(h<1e-7)	break;	// stationary point
		ee[0]=e*dt/h;	ff[0]=f*dt/h;	gg[0]=g*dt/h;
		u1 = u+ee[0]/2;	v1 = v+ff[0]/2;	w1 = w+gg[0]/2;
		e = ax.Linear1(u1,v1,w1);	f = ay.Linear1(u1,v1,w1);
		g = az.Linear1(u1,v1,w1);	h = 1+sqrt(e*e+f*f+g*g);
//		e = ax.Spline1(u1,v1,w1);	f = ay.Spline1(u1,v1,w1);
//		g = az.Spline1(u1,v1,w1);	h = 1+sqrt(e*e+f*f+g*g);
		if(h<1e-7)	break;	// stationary point
		ee[1]=e*dt/h;	ff[1]=f*dt/h;	gg[1]=g*dt/h;
		u1 = u+ee[1]/2;	v1 = v+ff[1]/2;	w1 = w+gg[1]/2;
		e = ax.Linear1(u1,v1,w1);	f = ay.Linear1(u1,v1,w1);
		g = az.Linear1(u1,v1,w1);	h = 1+sqrt(e*e+f*f+g*g);
//		e = ax.Spline1(u1,v1,w1);	f = ay.Spline1(u1,v1,w1);
//		g = az.Spline1(u1,v1,w1);	h = 1+sqrt(e*e+f*f+g*g);
		if(h<1e-7)	break;	// stationary point
		ee[2]=e*dt/h;	ff[2]=f*dt/h;	gg[2]=g*dt/h;
		u1 = u+ee[2];	v1 = v+ff[2];	w1 = w+gg[2];
		e = ax.Linear1(u1,v1,w1);	f = ay.Linear1(u1,v1,w1);
		g = az.Linear1(u1,v1,w1);	h = 1+sqrt(e*e+f*f+g*g);
//		e = ax.Spline1(u1,v1,w1);	f = ay.Spline1(u1,v1,w1);
//		g = az.Spline1(u1,v1,w1);	h = 1+sqrt(e*e+f*f+g*g);
		if(h<1e-7)	break;	// stationary point
		ee[3]=e*dt/h;	ff[3]=f*dt/h;	gg[3]=g*dt/h;
		u += ee[0]/6+ee[1]/3+ee[2]/3+ee[3]/6;
		v += ff[0]/6+ff[1]/3+ff[2]/3+ff[3]/6;
		w += gg[0]/6+gg[1]/3+gg[2]/3+gg[3]/6;
		// condition of end
		end = end || u<0 || v<0 || u>1 || v>1 || w<0 || w>1;
	} while(!end);
	curv_plot(k,pp,cc,tt);
	free(pp);	free(cc);	free(tt);
}
//-----------------------------------------------------------------------------
void mglGraph::Flow(const mglData &x, const mglData &y, const mglData &z, const mglData &ax, const mglData &ay,
					const mglData &az, const char *sch, int num, bool cnt)
{
	mreal u,v,w;
	long i,j,n=ax.nx,m=ax.ny,l=ax.nz;
	if(ax.nx*ax.ny*ax.nz!=ay.nx*ay.ny*ay.nz || ax.nx*ax.ny*ax.nz!=az.nx*az.ny*az.nz)
	{	SetWarn(mglWarnDim,"Flow");	return;	}
	if(ax.nx<2 || ax.ny<2 || ax.nz<2)
	{	SetWarn(mglWarnLow,"Flow");	return;	}
	bool both = x.nx*x.ny*x.nz==n*m*l && y.nx*y.ny*y.nz==n*m*l && z.nx*z.ny*z.nz==n*m*l;
	if(!(both || (x.nx==n && y.nx==m && z.nx==l)))
	{	SetWarn(mglWarnDim,"Flow");	return;	}
	static int cgid=1;	StartGroup("Flow3",cgid++);
	cnt=(num>0);	num = abs(num);	// redefine central parater
	
	Arrow1 = Arrow2 = '_';
	SetScheme(sch);

	for(i=0;i<num;i++)	for(j=0;j<num;j++)
	{
		u = (i+1.)/(num+1.);	v = (j+1.)/(num+1.);	w = 0;
		flow(true, u, v, w, x, y, z, ax, ay, az);
		flow(true,-u,-v,-w, x, y, z, ax, ay, az);
		u = (i+1.)/(num+1.);	v = (j+1.)/(num+1.);	w = 1;
		flow(true, u, v, w, x, y, z, ax, ay, az);
		flow(true,-u,-v,-w, x, y, z, ax, ay, az);
		u = 0;	v = (j+1.)/(num+1.);	w = (i+1.)/(num+1.);
		flow(true, u, v, w, x, y, z, ax, ay, az);
		flow(true,-u,-v,-w, x, y, z, ax, ay, az);
		u = 1;	v = (j+1.)/(num+1.);	w = (i+1.)/(num+1.);
		flow(true, u, v, w, x, y, z, ax, ay, az);
		flow(true,-u,-v,-w, x, y, z, ax, ay, az);
		u = (i+1.)/(num+1.);	v = 0;	w = (j+1.)/(num+1.);
		flow(true, u, v, w, x, y, z, ax, ay, az);
		flow(true,-u,-v,-w, x, y, z, ax, ay, az);
		u = (i+1.)/(num+1.);	v = 1;	w = (j+1.)/(num+1.);
		flow(true, u, v, w, x, y, z, ax, ay, az);
		flow(true,-u,-v,-w, x, y, z, ax, ay, az);
		if(cnt)
		{
			u = (i+1.)/(num+1.);	v = (j+1.)/(num+1.);	w = 0.5;
			flow(true, u, v, w, x, y, z, ax, ay, az);
			flow(true,-u,-v,-w, x, y, z, ax, ay, az);
			u = 0.5;	v = (j+1.)/(num+1.);	w = (i+1.)/(num+1.);
			flow(true, u, v, w, x, y, z, ax, ay, az);
			flow(true,-u,-v,-w, x, y, z, ax, ay, az);
			u = (i+1.)/(num+1.);	v = 0.5;	w = (j+1.)/(num+1.);
			flow(true, u, v, w, x, y, z, ax, ay, az);
			flow(true,-u,-v,-w, x, y, z, ax, ay, az);
		}
	}
	EndGroup();
}
//-----------------------------------------------------------------------------
void mglGraph::Flow(const mglData &ax, const mglData &ay, const mglData &az,
					const char *sch, int num, bool cnt)
{
	if(ax.nx*ax.ny*ax.nz!=ay.nx*ay.ny*ay.nz || ax.nx*ax.ny*ax.nz!=az.nx*az.ny*az.nz)
	{	SetWarn(mglWarnDim,"Flow");	return;	}
	if(ax.nx<2 || ax.ny<2 || ax.nz<2)
	{	SetWarn(mglWarnLow,"Flow");	return;	}
	mglData x(ax.nx), y(ax.ny), z(ax.nz);
	x.Fill(Min.x,Max.x);
	y.Fill(Min.y,Max.y);
	z.Fill(Min.z,Max.z);
	Flow(x,y,z,ax,ay,az,sch,num,cnt);
}
//-----------------------------------------------------------------------------
void mglGraph::FlowP(mglPoint p, const mglData &x, const mglData &y, const mglData &z, const mglData &ax, const mglData &ay, const mglData &az, const char *sch)
{
	mreal u,v,w;
	long n=ax.nx,m=ax.ny,l=ax.nz;
	if(ax.nx*ax.ny*ax.nz!=ay.nx*ay.ny*ay.nz || ax.nx*ax.ny*ax.nz!=az.nx*az.ny*az.nz)
	{	SetWarn(mglWarnDim,"Flow");	return;	}
	if(ax.nx<2 || ax.ny<2 || ax.nz<2)
	{	SetWarn(mglWarnLow,"Flow");	return;	}
	bool both = x.nx*x.ny*x.nz==n*m*l && y.nx*y.ny*y.nz==n*m*l && z.nx*z.ny*z.nz==n*m*l;
	if(!(both || (x.nx==n && y.nx==m && z.nx==l)))
	{	SetWarn(mglWarnDim,"Flow");	return;	}
	static int cgid=1;	StartGroup("FlowP3",cgid++);
	Arrow1 = Arrow2 = '_';	SetScheme(sch);

	// find coordinates u, v, w
	register long i,j,k,ii;
	register mreal d, dm=1e7;
	long i0=0,j0=0,k0=0;
	mreal dx,dy,dz;
	for(i=0;i<n;i++)	for(j=0;j<m;j++)	for(k=0;k<l;k++)	// first find closest
	{
		ii = i+n*(j+m*k);
		if(both)
		{	dx = x.a[ii]-p.x;	dy = y.a[ii]-p.y;	dz = x.a[ii]-p.z;	}
		else
		{	dx = x.a[i]-p.x;	dy = y.a[j]-p.y;	dz = x.a[k]-p.z;	}
		d = sqrt(dx*dx+dy*dy+dz*dz);
		if(d<dm)	{	i0=i;	j0=j;	k0=k;	dm=d;	}
	}
	if(dm==0)	// we find it
	{	u=i0/mreal(n);	v=j0/mreal(m);	w=k0/mreal(l);	}
	else
	{
		mreal dxu,dxv,dxw,dyu,dyv,dyw,dzu,dzv,dzw;
		if(both)
		{
			ii = i0+n*j0;
			dx = x.a[ii]-p.x;	dy = y.a[ii]-p.y;	dz = z.a[ii]-p.z;
			if(i0<n-1)
			{	dxu = x.a[ii+1]-x.a[ii];	dyu = y.a[ii+1]-y.a[ii];	dzu = z.a[ii+1]-z.a[ii];	}
			else
			{	dxu = x.a[ii]-x.a[ii-1];	dyu = y.a[ii]-y.a[ii-1];	dzu = z.a[ii]-z.a[ii-1];	}
			if(j0<m-1)
			{	dxv = x.a[ii+n]-x.a[ii];	dyv = y.a[ii+n]-y.a[ii];	dzv = z.a[ii+n]-z.a[ii];	}
			else
			{	dxv = x.a[ii]-x.a[ii-n];	dyv = y.a[ii]-y.a[ii-n];	dzv = z.a[ii]-z.a[ii-n];	}
			if(k0<l-1)
			{	dxw = x.a[ii+n*m]-x.a[ii];	dyw = y.a[ii+n*m]-y.a[ii];	dzw = z.a[ii+n*m]-z.a[ii];	}
			else
			{	dxw = x.a[ii]-x.a[ii-n*m];	dyw = y.a[ii]-y.a[ii-n*m];	dzw = z.a[ii]-z.a[ii-n*m];	}
			d = dxu*(dyw*dzv-dyv*dzw)+dxv*(dyu*dzw-dyw*dzu)+dxw*(dyv*dzu-dyu*dzv);
			u = (i0+(dx*(dyw*dzv-dyv*dzw)+dxv*(dy*dzw-dyw*dz)+dxw*(dyv*dz-dy*dzv))/d)/n;
			v = (j0-(dx*(dyw*dzu-dyu*dzw)+dxu*(dy*dzw-dyw*dz)+dxw*(dyu*dz-dy*dzu))/d)/m;
			w = (i0+(dx*(dyv*dzu-dyu*dzv)+dxu*(dy*dzv-dyv*dz)+dxv*(dyu*dz-dy*dzu))/d)/l;
		}
		else
		{
			dx = x.a[i0]-p.x;	dy = y.a[j0]-p.y;
			if(i0<n-1)	dxu = x.a[i0+1]-x.a[i0];
			else		dxu = x.a[i0]-x.a[i0-1];
			if(j0<m-1)	dyv = y.a[j0+1]-y.a[j0];
			else		dyv = y.a[j0]-y.a[j0-1];
			if(k0<l-1)	dzw = z.a[k0+1]-z.a[k0];
			else		dzw = z.a[k0]-z.a[k0-1];
			u = (i0+dx/dxu)/n;	v = (j0+dy/dyv)/m;	w = (k0+dz/dzw)/m;
		}
	}
	flow(false, u, v, w, x, y, z, ax, ay, az);
	EndGroup();
}
//-----------------------------------------------------------------------------
void mglGraph::FlowP(mglPoint p, const mglData &ax, const mglData &ay, const mglData &az,
					const char *sch)
{
	if(ax.nx*ax.ny*ax.nz!=ay.nx*ay.ny*ay.nz || ax.nx*ax.ny*ax.nz!=az.nx*az.ny*az.nz)
	{	SetWarn(mglWarnDim,"Flow");	return;	}
	if(ax.nx<2 || ax.ny<2 || ax.nz<2)
	{	SetWarn(mglWarnLow,"Flow");	return;	}
	mglData x(ax.nx), y(ax.ny), z(ax.nz);
	x.Fill(Min.x,Max.x);
	y.Fill(Min.y,Max.y);
	z.Fill(Min.z,Max.z);
	FlowP(p, x,y,z,ax,ay,az,sch);
}
//-----------------------------------------------------------------------------
//
//	Grad series
//
//-----------------------------------------------------------------------------
void mglGraph::Grad(const mglData &x, const mglData &y, const mglData &z, const mglData &phi, const char *sch, int num)
{
	mglData ax(phi), ay(phi),az(phi),xx(phi),yy(phi),zz(phi);
	long nn = phi.nx*phi.ny*phi.nz;
	if(x.nx*x.ny*x.nz==nn && y.nx*y.ny*y.nz==nn && x.nx*x.ny*x.nz==nn)
	{	xx = x;	yy = y;	zz = z;	}	// nothing to do
	else if(x.nx==phi.nx && y.nx==phi.ny && z.nx==phi.nz)
	{	// prepare data
		register long i,j,k,i0;
		for(i=0;i<phi.nx;i++)	for(j=0;j<phi.ny;j++)	for(k=0;k<phi.nz;k++)
		{
			i0 = i+phi.nx*(j+phi.ny*k);	xx.a[i0] = x.a[i];
			yy.a[i0] = y.a[j];			zz.a[i0] = z.a[k];	}
	}
	else	{	SetWarn(mglWarnDim,"Grad");	return;	}
	ax.Diff(xx,yy,zz);	ay.Diff(yy,xx,zz);	az.Diff(zz,xx,yy);
	Flow(xx,yy,zz,ax,ay,az,sch,num,num>0);
}
//-----------------------------------------------------------------------------
void mglGraph::Grad(const mglData &x, const mglData &y, const mglData &phi, const char *sch, int num, mreal zVal)
{
	mglData ax(phi), ay(phi),xx(phi),yy(phi);
	long nn = phi.nx*phi.ny;
	if(x.nx*x.ny==nn && y.nx*y.ny==nn)	{	xx = x;	yy = y;	}
	else if(x.nx==phi.nx && y.nx==phi.ny)
	{
		register long i,j,i0;
		for(i=0;i<phi.nx;i++)	for(j=0;j<phi.ny;j++)
		{	i0 = i+phi.nx*j;	xx.a[i0] = x.a[i];	yy.a[i0] = y.a[j];	}
	}
	else	{	SetWarn(mglWarnDim,"Grad");	return;	}
	ax.Diff(xx,yy);	ay.Diff(yy,xx);
	Flow(xx,yy,ax,ay,sch,num,num>0,zVal);
}
//-----------------------------------------------------------------------------
void mglGraph::Grad(const mglData &phi, const char *sch, int num, mreal zVal)
{
	mglData x(phi.nx), y(phi.ny), z(phi.nz);
	x.Fill(Min.x,Max.x);	y.Fill(Min.y,Max.y);	z.Fill(Min.z,Max.z);
	if(phi.nz>2)	Grad(x,y,z,phi,sch,num?num:3);
	else			Grad(x,y,phi,sch,num?num:5,zVal);
}
//-----------------------------------------------------------------------------
//
//	Pipe series
//
//-----------------------------------------------------------------------------
void mglGraph::flowr(bool simple, mreal zVal, mreal u, mreal v, const mglData &x, const mglData &y, const mglData &ax, const mglData &ay, mreal r0)
{
	long n=10*(ax.nx+ax.ny);
	bool both = x.nx==ax.nx && y.nx==ax.nx && x.ny==ax.ny && y.ny==ax.ny;
	mreal *pp, *cc, *rr;

	pp = (mreal *)malloc(3*n*sizeof(mreal));
	cc = (mreal *)malloc(4*n*sizeof(mreal));
	rr = (mreal *)malloc(n*sizeof(mreal));
	mreal dt = 0.5/(ax.nx > ax.ny ? ax.nx : ax.ny),e,f,g,ff[4],gg[4],h,s=1,ss;
	if(u<0 || v<0)	{	dt = -dt;	u = -u;		v = -v;		s = -1;}
	ss = 4/((Cmax-Cmin)*(Cmax-Cmin));
	if(r0==0)	r0=0.05;
	register long k=0,m;
	bool end = false;
	do{
		if(k>=n)	// resize array if need
		{
			n += 10*(ax.nx+ax.ny);
			pp = (mreal *)realloc(pp,3*n*sizeof(mreal));
			cc = (mreal *)realloc(cc,4*n*sizeof(mreal));
			rr = (mreal *)realloc(rr,n*sizeof(mreal));
		}
		// insert point
		pp[3*k] = simple ? Min.x + (Max.x-Min.x)*u : (both ? x.Spline1(u,v,0):x.Spline1(u,0,0));
		pp[3*k+1] = simple ? Min.y + (Max.y-Min.y)*v : (both ? y.Spline1(u,v,0):y.Spline1(v,0,0));
		pp[3*k+2] = zVal;
		for(m=0;m<k-1;m++)	// determines encircle
		{
			f = pp[3*k]-pp[3*m];
			g = pp[3*k+1]-pp[3*m+1];
			e = pp[3*k+2]-pp[3*m+2];
			if(f*f+g*g+e*e<dt*dt/4.)	{	end = true;	}
		}
		f = ax.Linear1(u,v,0);	g = ay.Linear1(u,v,0);
//		f = ax.Spline1(u,v,0);	g = ay.Spline1(u,v,0);
		h = hypot(f,g);
		rr[k] = r0>0 ? r0*sqrt(1e-2+ss*h*h)/2 : -r0/sqrt(1e-2+ss*h*h)/5;
		{
			mreal a = (2*s*h-Cmin-Cmax)/(Cmax-Cmin);
			if(a>1)	a=1;	if(a<-1)	a=-1;
			mglColor c = GetC(a,false);
			cc[4*k] = c.r;	cc[4*k+1] = c.g;	cc[4*k+2] = c.b;	cc[4*k+3] = AlphaDef;
		}
		h+=1;	k++;
		// find next point by midpoint method
		ff[0]=f*dt/h;	gg[0]=g*dt/h;
		e = u+ff[0]/2;	h = v+gg[0]/2;
		f = ax.Linear1(e,h,0);	g = ay.Linear1(e,h,0);	h = 1+hypot(f,g);
//		f = ax.Spline1(e,h,0);	g = ay.Spline1(e,h,0);	h = 1+hypot(f,g);
		ff[1]=f*dt/h;	gg[1]=g*dt/h;
		e = u+ff[1]/2;	h = v+gg[1]/2;
		f = ax.Linear1(e,h,0);	g = ay.Linear1(e,h,0);	h = 1+hypot(f,g);
//		f = ax.Spline1(e,h,0);	g = ay.Spline1(e,h,0);	h = 1+hypot(f,g);
		ff[2]=f*dt/h;	gg[2]=g*dt/h;
		e = u+ff[2];	h = v+gg[2];
		f = ax.Linear1(e,h,0);	g = ay.Linear1(e,h,0);	h = 1+hypot(f,g);
//		f = ax.Spline1(e,h,0);	g = ay.Spline1(e,h,0);	h = 1+hypot(f,g);
		ff[3]=f*dt/h;	gg[3]=g*dt/h;
		u += ff[0]/6+ff[1]/3+ff[2]/3+ff[3]/6;
		v += gg[0]/6+gg[1]/3+gg[2]/3+gg[3]/6;
		// condition of end
		end = end || u<0 || v<0 || u>1 || v>1;
	} while(!end);
	tube_plot(k,pp,cc,rr);
	free(pp);	free(cc);	free(rr);
}
//-----------------------------------------------------------------------------
void mglGraph::Pipe(const mglData &x, const mglData &y, const mglData &ax, const mglData &ay, const char *sch, mreal r0, int num, bool cnt, mreal zVal)
{
	mreal u,v;
	long n=ax.nx, m=ax.ny;
	if(n*m*ax.nz!=ay.nx*ay.ny*ay.nz)	{	SetWarn(mglWarnDim,"Pipe");	return;	}
	if(n<2 || m<2)						{	SetWarn(mglWarnLow,"Pipe");	return;	}
	bool both = x.nx==n && y.nx==n && x.ny==m && y.ny==m;
	if(!(both || (x.nx==n && y.nx==m)))	{	SetWarn(mglWarnDim,"Pipe");	return;	}
	static int cgid=1;	StartGroup("Pipe",cgid++);

	Arrow1 = Arrow2 = '_';
	SetScheme(sch);
	// allocate memory
	if(isnan(zVal)) zVal = Min.z;
	cnt=(num>0);	num = abs(num);
	
	for(long k=0;k<ax.nz;k++)
	{
		if(ax.nz>1)	zVal = Min.z+(Max.z-Min.z)*mreal(k)/(ax.nz-1);
		for(long i=0;i<num;i++)
		{
			u = 0;	v = (i+1.)/(num+1.);
			flowr(false, zVal, u, v, x, y, ax, ay,r0);
			flowr(false, zVal, -u, -v, x, y, ax, ay,r0);
			u = 1;	v = (i+1.)/(num+1.);
			flowr(false, zVal, u, v, x, y, ax, ay,r0);
			flowr(false, zVal, -u, -v, x, y, ax, ay,r0);
			u = (i+1.)/(num+1.);	v = 0;
			flowr(false, zVal, u, v, x, y, ax, ay,r0);
			flowr(false, zVal, -u, -v, x, y, ax, ay,r0);
			u = (i+1.)/(num+1.);	v = 1;
			flowr(false, zVal, u, v, x, y, ax, ay,r0);
			flowr(false, zVal, -u, -v, x, y, ax, ay,r0);
			if(cnt)
			{
				u = 0.5;	v = (i+1.)/(num+1.);
				flowr(false, zVal, u, v, x, y, ax, ay,r0);
				flowr(false, zVal, -u, -v, x, y, ax, ay,r0);
				u = (i+1.)/(num+1.);	v = 0.5;
				flowr(false, zVal, u, v, x, y, ax, ay,r0);
				flowr(false, zVal, -u, -v, x, y, ax, ay,r0);
			}
		}
	}
	EndGroup();
}
//-----------------------------------------------------------------------------
void mglGraph::Pipe(const mglData &ax, const mglData &ay, const char *sch, mreal r0, int num, bool cnt, mreal zVal)
{
	if(ax.nx*ax.ny!=ay.nx*ay.ny){	SetWarn(mglWarnDim,"Pipe");	return;	}
	if(ax.nx<2 || ax.ny<2)		{	SetWarn(mglWarnLow,"Pipe");	return;	}
	mglData x(ax.nx), y(ax.ny);
	x.Fill(Min.x,Max.x);
	y.Fill(Min.y,Max.y);
	Pipe(x,y,ax,ay,sch,r0,num,cnt,zVal);
}
//-----------------------------------------------------------------------------
void mglGraph::flowr(bool simple, mreal u, mreal v, mreal w,
					const mglData &x, const mglData &y, const mglData &z,
					const mglData &ax, const mglData &ay, const mglData &az, mreal r0)
{
	static long n=10*(ax.nx+ax.ny);
	long nn = ax.nx*ax.ny*ax.nz;
	bool both = x.nx*x.ny*x.nz==nn && y.nx*y.ny*y.nz==nn && z.nx*z.ny*z.nz==nn;
	mreal *pp, *cc, *rr;

	pp = (mreal *)malloc(3*n*sizeof(mreal));
	cc = (mreal *)malloc(4*n*sizeof(mreal));
	rr = (mreal *)malloc(n*sizeof(mreal));
	nn = (ax.nx > ax.ny ? ax.nx : ax.ny);
	nn = (nn > ax.nz ? nn : ax.nz);
	mreal dt = 0.2/nn, e,f,g,ee[4],ff[4],gg[4],h,s=1,u1,v1,w1,ss;
	if(u<0 || v<0 || w<0)
	{	dt = -dt;	u = -u;		v = -v;		w = -w;		s = -1;}
	ss = 4/((Cmax-Cmin)*(Cmax-Cmin));
	if(r0==0)	r0=0.05;
	register long k=0,m;
	bool end = false;
	do{
		if(k>=n)	// resize array if need
		{
			n += 10*(ax.nx+ax.ny);
			pp = (mreal *)realloc(pp,3*n*sizeof(mreal));
			cc = (mreal *)realloc(cc,4*n*sizeof(mreal));
			rr = (mreal *)realloc(rr,n*sizeof(mreal));
		}
		// insert point
		pp[3*k] = simple ? Min.x + (Max.x-Min.x)*u : (both ? x.Spline1(u,v,w):x.Spline1(u,0,0));
		pp[3*k+1] = simple ? Min.y + (Max.y-Min.y)*v : (both ? y.Spline1(u,v,w):y.Spline1(v,0,0));
		pp[3*k+2] = simple ? Min.z + (Max.z-Min.z)*w : (both ? z.Spline1(u,v,w):z.Spline1(w,0,0));
		for(m=0;m<k-1;m++)	// determines encircle
		{
			f = pp[3*k]-pp[3*m];
			g = pp[3*k+1]-pp[3*m+1];
			e = pp[3*k+2]-pp[3*m+2];
			if(f*f+g*g+e*e<dt*dt/4.)	{	end = true;	break;	}
		}
		e = ax.Linear1(u,v,w);	f = ay.Linear1(u,v,w);	g = az.Linear1(u,v,w);
//		e = ax.Spline1(u,v,w);	f = ay.Spline1(u,v,w);	g = az.Spline1(u,v,w);
		h = sqrt(e*e+f*f+g*g);
		rr[k] = r0>0 ? r0*sqrt(1e-2+ss*h*h)/2 : -r0/sqrt(1e-2+ss*h*h)/5;
		{
			mreal a = (2*s*h-Cmin-Cmax)/(Cmax-Cmin);
			if(a>1)	a=1;	if(a<-1)	a=-1;
			mglColor c = GetC(a,false);
			cc[4*k] = c.r;	cc[4*k+1] = c.g;	cc[4*k+2] = c.b;	cc[4*k+3] = AlphaDef;
		}
		h+=1;	k++;
		// find next point by midpoint method
		ee[0]=e*dt/h;	ff[0]=f*dt/h;	gg[0]=g*dt/h;
		u1 = u+ee[0]/2;	v1 = v+ff[0]/2;	w1 = w+gg[0]/2;
		e = ax.Linear1(u1,v1,w1);	f = ay.Linear1(u1,v1,w1);
		g = az.Linear1(u1,v1,w1);	h = 1+sqrt(e*e+f*f+g*g);
//		e = ax.Spline1(u1,v1,w1);	f = ay.Spline1(u1,v1,w1);
//		g = az.Spline1(u1,v1,w1);	h = 1+sqrt(e*e+f*f+g*g);
		ee[1]=e*dt/h;	ff[1]=f*dt/h;	gg[1]=g*dt/h;
		u1 = u+ee[1]/2;	v1 = v+ff[1]/2;	w1 = w+gg[1]/2;
		e = ax.Linear1(u1,v1,w1);	f = ay.Linear1(u1,v1,w1);
		g = az.Linear1(u1,v1,w1);	h = 1+sqrt(e*e+f*f+g*g);
//		e = ax.Spline1(u1,v1,w1);	f = ay.Spline1(u1,v1,w1);
//		g = az.Spline1(u1,v1,w1);	h = 1+sqrt(e*e+f*f+g*g);
		ee[2]=e*dt/h;	ff[2]=f*dt/h;	gg[2]=g*dt/h;
		u1 = u+ee[2];	v1 = v+ff[2];	w1 = w+gg[2];
		e = ax.Linear1(u1,v1,w1);	f = ay.Linear1(u1,v1,w1);
		g = az.Linear1(u1,v1,w1);	h = 1+sqrt(e*e+f*f+g*g);
//		e = ax.Spline1(u1,v1,w1);	f = ay.Spline1(u1,v1,w1);
//		g = az.Spline1(u1,v1,w1);	h = 1+sqrt(e*e+f*f+g*g);
		ee[3]=e*dt/h;	ff[3]=f*dt/h;	gg[3]=g*dt/h;
		u += ee[0]/6+ee[1]/3+ee[2]/3+ee[3]/6;
		v += ff[0]/6+ff[1]/3+ff[2]/3+ff[3]/6;
		w += gg[0]/6+gg[1]/3+gg[2]/3+gg[3]/6;
		// condition of end
		end = end || u<0 || v<0 || u>1 || v>1 || w<0 || w>1;
	} while(!end);
	tube_plot(k,pp,cc,rr);
	free(pp);	free(cc);	free(rr);
}
//-----------------------------------------------------------------------------
void mglGraph::Pipe(const mglData &x, const mglData &y, const mglData &z, const mglData &ax, const mglData &ay, const mglData &az, const char *sch, mreal r0, int num, bool cnt)
{
	mreal u,v,w;
	long i,j,n=ax.nx,m=ax.ny,l=ax.nz;
	if(ax.nx*ax.ny*ax.nz!=ay.nx*ay.ny*ay.nz || ax.nx*ax.ny*ax.nz!=az.nx*az.ny*az.nz)
	{	SetWarn(mglWarnDim,"Pipe");	return;	}
	if(ax.nx<2 || ax.ny<2 || ax.nz<2)
	{	SetWarn(mglWarnLow,"Pipe");	return;	}
	bool both = x.nx*x.ny*x.nz==n*m*l && y.nx*y.ny*y.nz==n*m*l && z.nx*z.ny*z.nz==n*m*l;
	if(!(both || (x.nx==n && y.nx==m && z.nx==l)))
	{	SetWarn(mglWarnDim,"Pipe");	return;	}
	static int cgid=1;	StartGroup("Pipe3",cgid++);

	Arrow1 = Arrow2 = '_';
	SetScheme(sch);
	cnt=(num>0);	num = abs(num);
	
	for(i=0;i<num;i++)	for(j=0;j<num;j++)
	{
		u = (i+1.)/(num+1.);	v = (j+1.)/(num+1.);	w = 0;
		flowr(true, u, v, w, x, y, z, ax, ay, az,r0);
		flowr(true,-u,-v,-w, x, y, z, ax, ay, az,r0);
		u = (i+1.)/(num+1.);	v = (j+1.)/(num+1.);	w = 1;
		flowr(true, u, v, w, x, y, z, ax, ay, az,r0);
		flowr(true,-u,-v,-w, x, y, z, ax, ay, az,r0);
		u = 0;	v = (j+1.)/(num+1.);	w = (i+1.)/(num+1.);
		flowr(true, u, v, w, x, y, z, ax, ay, az,r0);
		flowr(true,-u,-v,-w, x, y, z, ax, ay, az,r0);
		u = 1;	v = (j+1.)/(num+1.);	w = (i+1.)/(num+1.);
		flowr(true, u, v, w, x, y, z, ax, ay, az,r0);
		flowr(true,-u,-v,-w, x, y, z, ax, ay, az,r0);
		u = (i+1.)/(num+1.);	v = 0;	w = (j+1.)/(num+1.);
		flowr(true, u, v, w, x, y, z, ax, ay, az,r0);
		flowr(true,-u,-v,-w, x, y, z, ax, ay, az,r0);
		u = (i+1.)/(num+1.);	v = 1;	w = (j+1.)/(num+1.);
		flowr(true, u, v, w, x, y, z, ax, ay, az,r0);
		flowr(true,-u,-v,-w, x, y, z, ax, ay, az,r0);
		if(cnt)
		{
			u = (i+1.)/(num+1.);	v = (j+1.)/(num+1.);	w = 0.5;
			flowr(true, u, v, w, x, y, z, ax, ay, az,r0);
			flowr(true,-u,-v,-w, x, y, z, ax, ay, az,r0);
			u = 0.5;	v = (j+1.)/(num+1.);	w = (i+1.)/(num+1.);
			flowr(true, u, v, w, x, y, z, ax, ay, az,r0);
			flowr(true,-u,-v,-w, x, y, z, ax, ay, az,r0);
			u = (i+1.)/(num+1.);	v = 0.5;	w = (j+1.)/(num+1.);
			flowr(true, u, v, w, x, y, z, ax, ay, az,r0);
			flowr(true,-u,-v,-w, x, y, z, ax, ay, az,r0);
		}
	}
	EndGroup();
}
//-----------------------------------------------------------------------------
void mglGraph::Pipe(const mglData &ax, const mglData &ay, const mglData &az,
					const char *sch, mreal r0, int num, bool cnt)
{
	if(ax.nx*ax.ny*ax.nz!=ay.nx*ay.ny*ay.nz || ax.nx*ax.ny*ax.nz!=az.nx*az.ny*az.nz)
	{	SetWarn(mglWarnDim,"Pipe");	return;	}
	if(ax.nx<2 || ax.ny<2 || ax.nz<2)
	{	SetWarn(mglWarnLow,"Pipe");	return;	}
	mglData x(ax.nx), y(ax.ny), z(ax.nz);
	x.Fill(Min.x,Max.x);
	y.Fill(Min.y,Max.y);
	z.Fill(Min.z,Max.z);
	Pipe(x,y,z,ax,ay,az,sch,r0,num,cnt);
}
//-----------------------------------------------------------------------------
/// Plot flows for vector field {ax,ay} parametrically depended on coordinate {x,y} with color proportional to value |a|
void mgl_flow_xy(HMGL gr, const HMDT x, const HMDT y, const HMDT ax, const HMDT ay, const char *sch, int num, int central, mreal zVal)
{	if(gr && ay && ax && x && y)	gr->Flow(*x, *y, *ax, *ay, sch, num, central, zVal);	}
/// Plot flows for vector field {ax,ay} with color proportional to value |a|
void mgl_flow_2d(HMGL gr, const HMDT ax, const HMDT ay, const char *sch, int num, int central, mreal zVal)
{	if(gr && ay && ax)	gr->Flow(*ax, *ay, sch, num, central, zVal);	}
/// Plot flows for 3d vector field {ax,ay,ay} parametrically depended on coordinate {x,y,z} with color proportional to value |a|
void mgl_flow_xyz(HMGL gr, const HMDT x, const HMDT y, const HMDT z, const HMDT ax, const HMDT ay, const HMDT az, const char *sch, int num, int central)
{	if(gr && ay && ax && az && x && y && z)	gr->Flow(*x, *y, *z, *ax, *ay, *az, sch, num, central);	}
/// Plot flows for 3d vector field {ax,ay,ay} with color proportional to value |a|
void mgl_flow_3d(HMGL gr, const HMDT ax, const HMDT ay, const HMDT az, const char *sch, int num, int central)
{	if(gr && ay && ax && az)	gr->Flow(*ax, *ay, *az, sch, num, central);	}
//-----------------------------------------------------------------------------
/// Plot flows for vector field {ax,ay} parametrically depended on coordinate {x,y} with color proportional to value |a|
void mgl_flowp_xy(HMGL gr, mreal x0, mreal y0, mreal z0, const HMDT x, const HMDT y, const HMDT ax, const HMDT ay, const char *sch)
{	if(gr && ay && ax && x && y)	gr->FlowP(mglPoint(x0,y0,z0), *x, *y, *ax, *ay, sch);	}
/// Plot flows for vector field {ax,ay} with color proportional to value |a|
void mgl_flowp_2d(HMGL gr, mreal x0, mreal y0, mreal z0, const HMDT ax, const HMDT ay, const char *sch)
{	if(gr && ay && ax)	gr->FlowP(mglPoint(x0,y0,z0), *ax, *ay, sch);	}
/// Plot flows for 3d vector field {ax,ay,ay} parametrically depended on coordinate {x,y,z} with color proportional to value |a|
void mgl_flowp_xyz(HMGL gr, mreal x0, mreal y0, mreal z0, const HMDT x, const HMDT y, const HMDT z, const HMDT ax, const HMDT ay, const HMDT az, const char *sch)
{	if(gr && ay && ax && az && x && y && z)	gr->FlowP(mglPoint(x0,y0,z0), *x, *y, *z, *ax, *ay, *az, sch);	}
/// Plot flows for 3d vector field {ax,ay,ay} with color proportional to value |a|
void mgl_flowp_3d(HMGL gr, mreal x0, mreal y0, mreal z0, const HMDT ax, const HMDT ay, const HMDT az, const char *sch)
{	if(gr && ay && ax && az)	gr->FlowP(mglPoint(x0,y0,z0), *ax, *ay, *az, sch);	}
//-----------------------------------------------------------------------------
/// Plot flow pipes for vector field {ax,ay} parametrically depended on coordinate {x,y} with color proportional to value |a|
void mgl_pipe_xy(HMGL gr, const HMDT x, const HMDT y, const HMDT ax, const HMDT ay, const char *sch, mreal r0, int num, int central, mreal zVal)
{	if(gr && ay && ax && x && y)	gr->Pipe(*x, *y, *ax, *ay, sch, r0, num, central, zVal);	}
/// Plot flow pipes for vector field {ax,ay} with color proportional to value |a|
void mgl_pipe_2d(HMGL gr, const HMDT ax, const HMDT ay, const char *sch, mreal r0, int num, int central, mreal zVal)
{	if(gr && ay && ax)	gr->Pipe(*ax, *ay, sch, r0, num, central, zVal);	}
/// Plot flow pipes for 3d vector field {ax,ay,ay} parametrically depended on coordinate {x,y,z} with color proportional to value |a|
void mgl_pipe_xyz(HMGL gr, const HMDT x, const HMDT y, const HMDT z, const HMDT ax, const HMDT ay, const HMDT az, const char *sch, mreal r0, int num, int central)
{	if(gr && ay && ax && az && x && y && z)	gr->Pipe(*x, *y, *z, *ax, *ay, *az, sch, r0, num, central);	}
/// Plot flow pipes for 3d vector field {ax,ay,ay} with color proportional to value |a|
void mgl_pipe_3d(HMGL gr, const HMDT ax, const HMDT ay, const HMDT az, const char *sch, mreal r0, int num, int central)
{	if(gr && ay && ax && az)	gr->Pipe(*ax, *ay, *az, sch, r0, num, central);	}
//-----------------------------------------------------------------------------
/// The gradient of the scalar field
void mgl_grad_xyz(HMGL gr, const HMDT x, const HMDT y, const HMDT z, const HMDT ph, const char *sch, int num)
{	if(gr&&x&&y&&z&&ph)	gr->Grad(*x,*y,*z,*ph,sch,num);	}
void mgl_grad_xy(HMGL gr, const HMDT x, const HMDT y, const HMDT ph, const char *sch, int num, mreal zVal)
{	if(gr&&x&&y&&ph)	gr->Grad(*x,*y,*ph,sch,num,zVal);	}
void mgl_grad(HMGL gr, const HMDT ph, const char *sch, int num, mreal zVal)
{	if(gr&&ph)	gr->Grad(*ph,sch,num,zVal);	}
//-----------------------------------------------------------------------------
//	Fortran interface
//-----------------------------------------------------------------------------
/// Plot flows for vector field {ax,ay} parametrically depended on coordinate {x,y} with color proportional to value |a|
void mgl_flow_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *ax, uintptr_t *ay, const char *sch,
					int *num, int *central, mreal *zVal,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && ay && ax && x && y)	_GR_->Flow(_D_(x), _D_(y), _D_(ax), _D_(ay), s, *num, *central, *zVal);
	delete []s;
}
/// Plot flows for vector field {ax,ay} with color proportional to value |a|
void mgl_flow_2d_(uintptr_t *gr, uintptr_t *ax, uintptr_t *ay, const char *sch, int *num, int *central, mreal *zVal,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && ay && ax)	_GR_->Flow(_D_(ax), _D_(ay), s, *num, *central, *zVal);
	delete []s;
}
/// Plot flows for 3d vector field {ax,ay,ay} parametrically depended on coordinate {x,y,z} with color proportional to value |a|
void mgl_flow_xyz_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *ax, uintptr_t *ay, uintptr_t *az,
					const char *sch, int *num, int *central,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && ay && ax && az && x && y && z)
		_GR_->Flow(_D_(x), _D_(y), _D_(z), _D_(ax), _D_(ay), _D_(az), s, *num, *central);
	delete []s;
}
/// Plot flows for 3d vector field {ax,ay,ay} with color proportional to value |a|
void mgl_flow_3d_(uintptr_t *gr, uintptr_t *ax, uintptr_t *ay, uintptr_t *az, const char *sch, int *num, int *central,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && ay && ax && az)	_GR_->Flow(_D_(ax), _D_(ay), _D_(az), s, *num, *central);
	delete []s;
}
//-----------------------------------------------------------------------------
/// Plot flows for vector field {ax,ay} parametrically depended on coordinate {x,y} with color proportional to value |a|
void mgl_flowp_xy_(uintptr_t *gr, mreal *x0, mreal *y0, mreal *z0, uintptr_t *x, uintptr_t *y, uintptr_t *ax, uintptr_t *ay, const char *sch, int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && ay && ax && x && y)
		_GR_->FlowP(mglPoint(*x0,*y0,*z0), _D_(x), _D_(y), _D_(ax), _D_(ay), s);
	delete []s;
}
/// Plot flows for vector field {ax,ay} with color proportional to value |a|
void mgl_flowp_2d_(uintptr_t *gr, mreal *x0, mreal *y0, mreal *z0, uintptr_t *ax, uintptr_t *ay, const char *sch, int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && ay && ax)
		_GR_->FlowP(mglPoint(*x0,*y0,*z0), _D_(ax), _D_(ay), s);
	delete []s;
}
/// Plot flows for 3d vector field {ax,ay,ay} parametrically depended on coordinate {x,y,z} with color proportional to value |a|
void mgl_flowp_xyz_(uintptr_t *gr, mreal *x0, mreal *y0, mreal *z0, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *ax, uintptr_t *ay, uintptr_t *az, const char *sch, int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && ay && ax && az && x && y && z)
		_GR_->FlowP(mglPoint(*x0,*y0,*z0), _D_(x), _D_(y), _D_(z), _D_(ax), _D_(ay), _D_(az), s);
	delete []s;
}
/// Plot flows for 3d vector field {ax,ay,ay} with color proportional to value |a|
void mgl_flowp_3d_(uintptr_t *gr, mreal *x0, mreal *y0, mreal *z0, uintptr_t *ax, uintptr_t *ay, uintptr_t *az, const char *sch, int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && ay && ax && az)
		_GR_->FlowP(mglPoint(*x0,*y0,*z0), _D_(ax), _D_(ay), _D_(az), s);
	delete []s;
}
//-----------------------------------------------------------------------------
/// Plot flow pipes for vector field {ax,ay} parametrically depended on coordinate {x,y} with color proportional to value |a|
void mgl_pipe_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *ax, uintptr_t *ay, const char *sch, mreal *r0,
					int *num, int *central, mreal *zVal,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && ay && ax && x && y)
		_GR_->Pipe(_D_(x), _D_(y), _D_(ax), _D_(ay), s, *r0, *num, *central, *zVal);
	delete []s;
}
/// Plot flow pipes for vector field {ax,ay} with color proportional to value |a|
void mgl_pipe_2d_(uintptr_t *gr, uintptr_t *ax, uintptr_t *ay, const char *sch, mreal *r0, int *num, int *central, mreal *zVal,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && ay && ax)	_GR_->Pipe(_D_(ax), _D_(ay), s, *r0, *num, *central, *zVal);
	delete []s;
}
/// Plot flow pipes for 3d vector field {ax,ay,ay} parametrically depended on coordinate {x,y,z} with color proportional to value |a|
void mgl_pipe_xyz_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *ax, uintptr_t *ay, uintptr_t *az,
					const char *sch, mreal *r0, int *num, int *central,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && ay && ax && az && x && y && z)
		_GR_->Pipe(_D_(x), _D_(y), _D_(z), _D_(ax), _D_(ay), _D_(az), s, *r0, *num, *central);
	delete []s;
}
/// Plot flow pipes for 3d vector field {ax,ay,ay} with color proportional to value |a|
void mgl_pipe_3d_(uintptr_t *gr, uintptr_t *ax, uintptr_t *ay, uintptr_t *az, const char *sch, mreal *r0, int *num, int *central,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && ay && ax && az)	_GR_->Pipe(_D_(ax), _D_(ay), _D_(az), s, *r0, *num, *central);
	delete []s;
}
//-----------------------------------------------------------------------------
/// Plot gradient for scalar field ph parametrically depended on coordinate {x,y} with color proportional to value |a|
void mgl_grad_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *ph, const char *sch,
					int *num, mreal *zVal,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && ph && x && y)	_GR_->Grad(_D_(x), _D_(y), _D_(ph), s, *num, *zVal);
	delete []s;
}
void mgl_grad_xyz_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *ph, const char *sch,
					int *num, int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && ph && x && y && z)	_GR_->Grad(_D_(x), _D_(y), _D_(z), _D_(ph), s, *num);
	delete []s;
}
/// Plot flows for vector field {ax,ay} with color proportional to value |a|
void mgl_grad_(uintptr_t *gr, uintptr_t *ph, const char *sch, int *num, mreal *zVal,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && ph)	_GR_->Flow(_D_(ph), s, *num, *zVal);
	delete []s;
}
//-----------------------------------------------------------------------------
