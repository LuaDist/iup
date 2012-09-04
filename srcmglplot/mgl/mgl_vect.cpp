/***************************************************************************
 * mgl_vect.cpp is part of Math Graphic Library
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
//	Vect series
//
//-----------------------------------------------------------------------------
void mglGraph::Traj(const mglData &x, const mglData &y, const mglData &z, 
					const mglData &ax, const mglData &ay, const mglData &az,
					const char *sch, mreal len)
{
	if(ax.nx<2)	{	SetWarn(mglWarnLow,"Traj");	return;	}
	if(ax.nx!=x.nx || z.nx!=x.nx || y.nx!=x.nx || ay.nx!=x.nx || az.nx!=x.nx)
	{	SetWarn(mglWarnDim,"Traj");	return;	}
	long n = ax.nx, m, i, j, nc=0;
	mglColor cm;
	// find maximum
	i = ax.ny>ay.ny ? ax.ny:ay.ny;	j = z.ny>az.ny ? z.ny:az.ny;
	m = x.ny>y.ny ? x.ny:y.ny;		if(i>m)	m=i;	if(j>m)	m=j;
	cm = cmap[0];	nc=NumCol;	NumCol=1;
	SetPal(sch);
	mreal *pp=new mreal[6*n*m], *cc=new mreal[n*m];
	bool *tt=new bool[2*n*m];
	Pen(NC,'-',BaseLineWidth);
//	char st[2]={0,0};	// temporary color scheme
	static int cgid=1;	StartGroup("Traj",cgid++);
	mreal dx,dy,dz,dd,da,xm=0;
	register long ix,iy,iz,jx,jy,jz;
	for(j=0;j<m;j++)	for(i=0;i<n;i++)	// find maximal amplitude of vector field
	{
		jx = i+ax.ny*(j%ax.ny);	jy = i+ay.ny*(j%ay.ny);	jz = i+az.ny*(j%az.ny);
		da = sqrt(ax.a[jx]*ax.a[jx]+ay.a[jy]*ay.a[jy]+az.a[jz]*az.a[jz]);
		xm = xm>da ? xm : da;
	}
	xm = 1./(xm ? sqrt(xm):1);
	for(j=0;j<m;j++) // start prepare arrows
	{
		cmap[0]=GetPal();
		for(i=0;i<n;i++)
		{
			ix = i + n*(j<x.ny?j:0);	jx = i + n*(j<ax.ny?j:0);
			iy = i + n*(j<y.ny?j:0);	jy = i + n*(j<ay.ny?j:0);
			iz = i + n*(j<z.ny?j:0);	jz = i + n*(j<az.ny?j:0);
			da = sqrt(ax.a[jx]*ax.a[jx]+ay.a[jy]*ay.a[jy]+az.a[jz]*az.a[jz]);
			if(len==0)
			{
				if(i<n-1)
				{	dx=x.a[ix+1]-x.a[ix];	dy=y.a[iy+1]-y.a[iy];	dz=z.a[iz+1]-z.a[iz];	}
				else
				{	dx=x.a[ix]-x.a[ix-1];	dy=y.a[iy]-y.a[iy-1];	dz=z.a[iz]-z.a[iz-1];	}
				dd = da ? 1/da : 0;		dd *= sqrt(dx*dx+dy*dy+dz*dz);
			}
			else dd = len;
			cc[i] = da*xm*1.5 - 0.5;
			pp[6*i+0] = x.a[ix];	pp[6*i+3] = x.a[ix]+dd*ax.a[jx];
			pp[6*i+1] = y.a[iy];	pp[6*i+4] = y.a[iy]+dd*ay.a[jy];
			pp[6*i+2] = z.a[iz];	pp[6*i+5] = z.a[iz]+dd*az.a[jz];
			tt[2*i]   = ScalePoint(pp[6*i],pp[6*i+1],pp[6*i+2]);
			tt[2*i+1] = ScalePoint(pp[6*i+3],pp[6*i+4],pp[6*i+5]);
		}
		vects_plot(n,pp,cc,tt,false);
	}
	cmap[0]=cm;	NumCol=nc;
	SetPal(0);	EndGroup();
	delete []pp;	delete []tt;	delete []cc;
}
//-----------------------------------------------------------------------------
void mglGraph::Traj(const mglData &x, const mglData &y, const mglData &ax, 
					const mglData &ay, const char *sch, mreal zVal, mreal len)
{
	if(ax.nx<2)	{	SetWarn(mglWarnLow,"Traj");	return;	}
	if(x.nx!=ax.nx || y.nx!=ax.nx || ay.nx!=ax.nx)
	{	SetWarn(mglWarnDim,"Traj");	return;	}
	mglData z(x.nx), az(x.nx);
	if(isnan(zVal))	zVal = Min.z;
	z.Fill(zVal,zVal);
	Traj(x,y,z,ax,ay,az,sch,len);
}
//-----------------------------------------------------------------------------
//
//	Vect series
//
//-----------------------------------------------------------------------------
void mglGraph::Vect(const mglData &x, const mglData &y, const mglData &ax, const mglData &ay, const char *sch, mreal zVal, int flag)
{
	long i,j,n=ax.nx,m=ax.ny,k,ix,iy,jj,i0;
	if(n*m*ax.nz!=ay.nx*ay.ny*ay.nz)	{	SetWarn(mglWarnDim,"Vect");	return;	}
	if(n<2 || m<2)						{	SetWarn(mglWarnLow,"Vect");	return;	}
	bool both = x.nx==n && y.nx==n && x.ny==m && y.ny==m;
	if(!(both || (x.nx==n && y.nx==m)))	{	SetWarn(mglWarnDim,"Vect");	return;	}
	static int cgid=1;	StartGroup("Vect",cgid++);

	SetScheme(sch);
	if(isnan(zVal))	zVal = Min.z;
	Pen(cmap[0],'-',BaseLineWidth);
	long tx=1,ty=1;
	if(MeshNum>1)	{	tx=(n-1)/(MeshNum-1);	ty=(m-1)/(MeshNum-1);	}
	if(tx<1)	tx=1;	if(ty<1)	ty=1;
	long nn=n/tx, mm=m/ty;
	mreal xm,ym,dx,dy,*pp=new mreal[6*nn*mm],*cc=new mreal[nn*mm];
	mreal dd,dm=(fabs(Cmax)+fabs(Cmin))*1e-5;
	bool *tt=new bool[2*nn*mm];

	for(i=0,xm=0;i<m*n*ax.nz;i++)
	{
		ym = (ax.a[i])*(ax.a[i])+(ay.a[i])*(ay.a[i]);
		xm = xm>ym ? xm : ym;
	}
	xm = 1./(xm==0 ? 1:sqrt(xm));
	long s = both ? n : 1, ni,nj;

	for(k=0;k<ax.nz;k++)
	{
		if(ax.nz>1)	zVal = Min.z+(Max.z-Min.z)*mreal(k)/(ax.nz-1);
		for(ni=0;ni<nn;ni++)	for(nj=0;nj<mm;nj++)
		{
			i = ni*tx;	j = nj*ty;	jj = i+n*(j+m*k);	i0 = ni+nn*nj;
			if(both)
			{	ix = x.nz>k ? jj : i+n*j;	iy = y.nz>k ? jj : i+n*j;	}
			else
			{	ix = x.ny>k ? i+n*k : i;	iy = y.ny>k ? j+m*k : j;	}
			dx = i<n-1 ? (x.a[ix+1]-x.a[ix]) : (x.a[ix]-x.a[ix-1]);
			dy = j<m-1 ? (y.a[iy+s]-y.a[iy]) : (y.a[iy]-y.a[iy-s]);
			dx *= tx;	dy *= ty;
			dd = hypot(ax.a[jj],ay.a[jj]);
			cc[i0] = dd*xm*1.5 - 0.5;

			pp[6*i0+2] = pp[6*i0+5] = zVal;
			dx *= (flag&MGL_VEC_LEN) ? (dd>dm ? ax.a[jj]/dd : 0) : ax.a[jj]*xm;
			dy *= (flag&MGL_VEC_LEN) ? (dd>dm ? ay.a[jj]/dd : 0) : ay.a[jj]*xm;
			if(flag & MGL_VEC_END)
			{
				pp[6*i0+0] = x.a[ix]-dx;	pp[6*i0+3] = x.a[ix];
				pp[6*i0+1] = y.a[iy]-dy;	pp[6*i0+4] = y.a[iy];
			}
			else if(flag & MGL_VEC_MID)
			{
				pp[6*i0+0] = x.a[ix]-dx/2;	pp[6*i0+3] = x.a[ix]+dx/2;
				pp[6*i0+1] = y.a[iy]-dy/2;	pp[6*i0+4] = y.a[iy]+dy/2;
			}
			else
			{
				pp[6*i0+0] = x.a[ix];		pp[6*i0+3] = x.a[ix]+dx;
				pp[6*i0+1] = y.a[iy];		pp[6*i0+4] = y.a[iy]+dy;
			}
			tt[2*i0]  = ScalePoint(pp[6*i0],pp[6*i0+1],pp[6*i0+2]);
			tt[2*i0+1]= ScalePoint(pp[6*i0+3],pp[6*i0+4],pp[6*i0+5]);
		}
		if(flag & MGL_VEC_DOT)
			lines_plot(nn*mm,pp,(flag&MGL_VEC_COL)?0:cc,tt,true,flag&MGL_VEC_GRD);
		else
			vects_plot(nn*mm,pp,(flag&MGL_VEC_COL)?0:cc,tt,flag&MGL_VEC_GRD);
	}
	EndGroup();
	delete []pp;	delete []tt;	delete []cc;
}
//-----------------------------------------------------------------------------
void mglGraph::Vect(const mglData &ax, const mglData &ay, const char *sch, mreal zVal, int flag)
{
	if(ax.nx<2 || ax.ny<2)	{	SetWarn(mglWarnLow,"Vect");	return;	}
	mglData x(ax.nx), y(ax.ny);
	x.Fill(Min.x,Max.x);
	y.Fill(Min.y,Max.y);
	Vect(x,y,ax,ay,sch,zVal,flag);
}
//-----------------------------------------------------------------------------
//
//	VectL series
//
//-----------------------------------------------------------------------------
void mglGraph::VectL(const mglData &x, const mglData &y, const mglData &ax, const mglData &ay, const char *sch, mreal zVal)
{	Vect(x,y,ax,ay,sch,zVal,MGL_VEC_COL|MGL_VEC_DOT|MGL_VEC_GRD);	}
//-----------------------------------------------------------------------------
void mglGraph::VectL(const mglData &ax, const mglData &ay, const char *sch, mreal zVal)
{
	if(ax.nx<2 || ax.ny<2)	{	SetWarn(mglWarnLow,"Vect");	return;	}
	mglData x(ax.nx), y(ax.ny);
	x.Fill(Min.x,Max.x);
	y.Fill(Min.y,Max.y);
	VectL(x,y,ax,ay,sch,zVal);
}
//-----------------------------------------------------------------------------
//
//	VectC series
//
//-----------------------------------------------------------------------------
void mglGraph::VectC(const mglData &x, const mglData &y, const mglData &ax, const mglData &ay, const char *sch, mreal zVal)
{	Vect(x,y,ax,ay,sch,zVal,MGL_VEC_LEN|MGL_VEC_DOT|MGL_VEC_GRD);	}
//-----------------------------------------------------------------------------
void mglGraph::VectC(const mglData &ax, const mglData &ay, const char *sch, mreal zVal)
{
	if(ax.nx<2 || ax.ny<2)	{	SetWarn(mglWarnLow,"Vect");	return;	}
	mglData x(ax.nx), y(ax.ny);
	x.Fill(Min.x,Max.x);
	y.Fill(Min.y,Max.y);
	VectC(x,y,ax,ay,sch,zVal);
}
//-----------------------------------------------------------------------------
//
//	Vect3 series
//
//-----------------------------------------------------------------------------
void mglGraph::Vect(const mglData &x, const mglData &y, const mglData &z, const mglData &ax, const mglData &ay, const mglData &az, const char *sch, int flag)
{
	register long i,j,n=ax.nx,m=ax.ny,l=ax.nz,k,jj,i0;
	if(n*m*l!=ay.nx*ay.ny*ay.nz || ax.nx*ax.ny*ax.nz!=az.nx*az.ny*az.nz)
	{	SetWarn(mglWarnDim,"Vect");	return;	}
	if(n<2 || m<2 || l<2)	{	SetWarn(mglWarnLow,"Vect");	return;	}
	bool both = x.nx*x.ny*x.nz==n*m*l && y.nx*y.ny*y.nz==n*m*l && z.nx*z.ny*z.nz==n*m*l;
	if(!(both || (x.nx==n && y.nx==m && z.nx==l)))
	{	SetWarn(mglWarnDim,"Vect");	return;	}
	static int cgid=1;	StartGroup("Vect3",cgid++);

	long ix,iy,iz;
	mreal xm,ym,dx,dy,dz,dd,dm=(fabs(Cmax)+fabs(Cmin))*1e-5;
	SetScheme(sch);
	Pen(cmap[0],'-',BaseLineWidth);
	long tx=1,ty=1,tz=1;
	if(MeshNum>1)
	{	tx=(n-1)/(MeshNum-1);	ty=(m-1)/(MeshNum-1);	tz=(l-1)/(MeshNum-1);}
	if(tx<1)	tx=1;	if(ty<1)	ty=1;	if(tz<1)	tz=1;
	long nn=n/tx, mm=m/ty, ll=l/tz;

	for(i=0,xm=0;i<m*n*l;i++)
	{
		ym = (ax.a[i])*(ax.a[i])+(ay.a[i])*(ay.a[i])+(az.a[i])*(az.a[i]);
		xm = xm>ym ? xm : ym;
	}
	xm = 1./(xm==0 ? 1:sqrt(xm));

	mreal *pp=new mreal[6*nn*mm*ll],*cc=new mreal[nn*mm*ll];
	bool *tt=new bool[2*nn*mm*ll];
	long s = both ? n:1, t = both ? n*m : 1, ni,nj,nk;

	if(both || (x.nx==n && y.nx==m && z.nx==l))
	{
		for(nk=0;nk<ll;nk++)	for(ni=0;ni<nn;ni++)	for(nj=0;nj<mm;nj++)
		{
			i = ni*tx;	j = nj*ty;	k = nk*tz;
			jj = i+n*(j+m*k);	i0 = ni+nn*(nj+mm*nk);
			if(both)
			{	ix = iy = iz = jj;	}
			else
			{	ix = i;	iy = j;	iz = k;	}
			dx = i<n-1 ? (x.a[ix+1]-x.a[ix]) : (x.a[ix]-x.a[ix-1]);
			dy = j<m-1 ? (y.a[iy+s]-y.a[iy]) : (y.a[iy]-y.a[iy-s]);
			dz = k<l-1 ? (z.a[iz+t]-y.a[iz]) : (y.a[iz]-y.a[iz-t]);
			dx *= tx;	dy *= ty;	dz *= tz;
			dd = sqrt(ax.a[jj]*ax.a[jj]+ay.a[jj]*ay.a[jj]+az.a[jj]*az.a[jj]);
			cc[jj] = xm*1.5*dd - 0.5;	dd *= 1.1;

			dx *= (flag&MGL_VEC_LEN) ? (dd>dm ? ax.a[jj]/dd : 0) : ax.a[jj]*xm;
			dy *= (flag&MGL_VEC_LEN) ? (dd>dm ? ay.a[jj]/dd : 0) : ay.a[jj]*xm;
			dz *= (flag&MGL_VEC_LEN) ? (dd>dm ? az.a[jj]/dd : 0) : az.a[jj]*xm;
			if(flag & MGL_VEC_END)
			{
				pp[6*i0+0] = x.a[ix]-dx;	pp[6*i0+3] = x.a[ix];
				pp[6*i0+1] = y.a[iy]-dy;	pp[6*i0+4] = y.a[iy];
				pp[6*i0+2] = z.a[iz]-dz;	pp[6*i0+5] = z.a[iz];
			}
			else if(flag & MGL_VEC_MID)
			{
				pp[6*i0+0] = x.a[ix]-dx/2;	pp[6*i0+3] = x.a[ix]+dx/2;
				pp[6*i0+1] = y.a[iy]-dy/2;	pp[6*i0+4] = y.a[iy]+dy/2;
				pp[6*i0+2] = z.a[iz]-dz/2;	pp[6*i0+5] = z.a[iz]+dz/2;
			}
			else
			{
				pp[6*i0+0] = x.a[ix];	pp[6*i0+3] = x.a[ix]+dx;
				pp[6*i0+1] = y.a[iy];	pp[6*i0+4] = y.a[iy]+dy;
				pp[6*i0+2] = z.a[iz];	pp[6*i0+5] = z.a[iz]+dz;
			}
			tt[2*i0]  = ScalePoint(pp[6*i0],pp[6*i0+1],pp[6*i0+2]);
			tt[2*i0+1]= ScalePoint(pp[6*i0+3],pp[6*i0+4],pp[6*i0+5]);
		}
		if(flag & MGL_VEC_DOT)
			lines_plot(nn*mm*ll,pp,(flag&MGL_VEC_COL)?0:cc,tt,true,flag&MGL_VEC_GRD);
		else
			vects_plot(nn*mm*ll,pp,(flag&MGL_VEC_COL)?0:cc,tt,flag&MGL_VEC_GRD);
	}
	EndGroup();
	delete []pp;	delete []tt;	delete []cc;
}
//-----------------------------------------------------------------------------
void mglGraph::Vect(const mglData &ax, const mglData &ay, const mglData &az, const char *sch, int flag)
{
	if(ax.nx<2 || ax.ny<2 || ax.nz<2)
	{	SetWarn(mglWarnLow,"Vect");	return;	}
	mglData x(ax.nx), y(ax.ny), z(ax.nz);
	x.Fill(Min.x,Max.x);
	y.Fill(Min.y,Max.y);
	z.Fill(Min.z,Max.z);
	Vect(x,y,z,ax,ay,az,sch,flag);
}
//-----------------------------------------------------------------------------
//
//	VectL3 series
//
//-----------------------------------------------------------------------------
void mglGraph::VectL(const mglData &x, const mglData &y, const mglData &z, const mglData &ax, const mglData &ay, const mglData &az, const char *sch)
{	Vect(x,y,z,ax,ay,az,sch,MGL_VEC_COL|MGL_VEC_DOT|MGL_VEC_GRD);	}
//-----------------------------------------------------------------------------
void mglGraph::VectL(const mglData &ax, const mglData &ay, const mglData &az, const char *sch)
{
	if(ax.nx<2 || ax.ny<2 || ax.nz<2)
	{	SetWarn(mglWarnLow,"Vect");	return;	}
	mglData x(ax.nx), y(ax.ny), z(ax.nz);
	x.Fill(Min.x,Max.x);
	y.Fill(Min.y,Max.y);
	z.Fill(Min.z,Max.z);
	VectL(x,y,z,ax,ay,az,sch);
}
//-----------------------------------------------------------------------------
//
//	VectC3 series
//
//-----------------------------------------------------------------------------
void mglGraph::VectC(const mglData &x, const mglData &y, const mglData &z,
					const mglData &ax, const mglData &ay, const mglData &az, const char *sch)
{	Vect(x,y,z,ax,ay,az,sch,MGL_VEC_LEN|MGL_VEC_DOT|MGL_VEC_GRD);	}
//-----------------------------------------------------------------------------
void mglGraph::VectC(const mglData &ax, const mglData &ay, const mglData &az, const char *sch)
{
	if(ax.nx<2 || ax.ny<2 || ax.nz<2)
	{	SetWarn(mglWarnLow,"VectC");	return;	}
	mglData x(ax.nx), y(ax.ny), z(ax.nz);
	x.Fill(Min.x,Max.x);
	y.Fill(Min.y,Max.y);
	z.Fill(Min.z,Max.z);
	VectC(x,y,z,ax,ay,az,sch);
}
//-----------------------------------------------------------------------------
//
//	Map series
//
//-----------------------------------------------------------------------------
void mglGraph::Map(const mglData &x, const mglData &y, const mglData &ax, const mglData &ay,
					const char *sch, int ks, bool pnts)
{
	register long i,j,i0,jj,ix,iy,n=ax.nx,m=ax.ny;
	if(n*m!=ay.nx*ay.ny)	{	SetWarn(mglWarnDim,"Map");	return;	}
	if(n<2 || m<2)			{	SetWarn(mglWarnLow,"Map");	return;	}
	bool both = x.nx==n && y.nx==n && x.ny==m && y.ny==m;
	if(!(both || (x.nx==n && y.nx==m)))
	{	SetWarn(mglWarnDim,"Map");	return;	}
	static int cgid=1;	StartGroup("Map",cgid++);

	SetScheme(sch);
	mglColor c;
	if(ks<0 || ks>=ay.nz || ks>=ax.nz)	ks = 0;
	long s = both ? n:1, s1, s2, n1, n2;

	mreal *pp = new mreal[3*n*m],*cc = new mreal[4*n*m];
	bool *tt = new bool[n*m];
	mreal xdy,xdx,ydx,ydy;

	for(i=0;i<n;i++)	for(j=0;j<m;j++)
	{
		jj = i+n*(j+m*ks);	i0 = i+n*j;
		if(both)
		{	ix = x.nz>ks ? jj : i0;	iy = y.nz>ks ? jj : i0;	}
		else
		{	ix = x.ny>ks ? i+n*ks : i;	iy = y.ny>ks ? j+m*ks : j;	}
		s1 = i>0 ? 1:0;		s2 = i<n-1 ? 1:0;
		xdx = (ax.a[jj+s2]-ax.a[jj-s1])/(x.a[ix+s2]-x.a[ix-s1]);
		ydx = (ay.a[jj+s2]-ay.a[jj-s1])/(x.a[ix+s2]-x.a[ix-s1]);
		s1 = j>0 ? s:0;		s2 = j<m-1 ? s:0;
		n1 = j>0 ? n:0;		n2 = j<m-1 ? n:0;
		xdy = (ax.a[jj+n2]-ax.a[jj-n1])/(y.a[iy+s2]-y.a[iy-s1]);
		ydy = (ay.a[jj+n2]-ay.a[jj-n1])/(y.a[iy+s2]-y.a[iy-s1]);
		xdx = xdx*ydy - xdy*ydx;	// �������-1
		pp[3*i0+0] = ax.a[jj];	pp[3*i0+1] = ay.a[jj];	pp[3*i0+2] = xdx;
		tt[i0] = ScalePoint(pp[3*i0],pp[3*i0+1],pp[3*i0+2]);
		if(tt[i0])
		{
			c = GetC2(x.a[ix],y.a[iy]);
			cc[4*i0] = c.r;	cc[4*i0+1] = c.g;	cc[4*i0+2] = c.b;
			cc[4*i0+3] = AlphaDef;
		}
	}
	if(pnts)
	{
		for(i=0;i<n*m;i++)
			if(tt[i])
				Ball(pp[3*i],pp[3*i+1],pp[3*i+2],
						mglColor(cc[4*i],cc[4*i+1],cc[4*i+2]),-cc[4*i+3]);
	}
	else	surf_plot(n, m, pp, cc, tt);
	EndGroup();
	delete []pp;	delete []cc;	delete[]tt;
}
//-----------------------------------------------------------------------------
void mglGraph::Map(const mglData &ax, const mglData &ay, const char *sch, int ks, bool pnts)
{
	if(ax.nx<2 || ax.ny<2)	{	SetWarn(mglWarnLow,"Map");	return;	}
	mglData x(ax.nx), y(ax.ny);
	x.Fill(Min.x,Max.x);
	y.Fill(Min.y,Max.y);
	Map(x,y,ax,ay,sch,ks,pnts);
}
//-----------------------------------------------------------------------------
//
//	Dew series
//
//-----------------------------------------------------------------------------
void mglGraph::Dew(const mglData &x, const mglData &y, const mglData &ax, const mglData &ay, const char *sch, mreal zVal)
{
	long i,j,n=ax.nx,m=ax.ny,k,ix,iy,jj;
	if(n*m*ax.nz!=ay.nx*ay.ny*ay.nz)	{	SetWarn(mglWarnDim,"Dew");	return;	}
	if(n<2 || m<2)						{	SetWarn(mglWarnLow,"Dew");	return;	}
	bool both = x.nx==n && y.nx==n && x.ny==m && y.ny==m;
	if(!(both || (x.nx==n && y.nx==m)))	{	SetWarn(mglWarnDim,"Dew");	return;	}
	static int cgid=1;	StartGroup("Dew",cgid++);

	mreal xm,ym,dx,dy,dd,x_,y_,z_;
	SetScheme(sch);
	bool inv = sch && strchr(sch,'A');
	if(isnan(zVal))	zVal = Min.z;
	long tx=1,ty=1;
	if(MeshNum>1)	{	tx=(n-1)/(MeshNum-1);	ty=(m-1)/(MeshNum-1);	}
	if(tx<1)	tx=1;	if(ty<1)	ty=1;

	for(i=0,xm=0;i<m*n*ax.nz;i++)
	{
		ym = (ax.a[i])*(ax.a[i])+(ay.a[i])*(ay.a[i]);
		xm = xm>ym ? xm : ym;
	}
	xm = 1./(xm==0 ? 1:sqrt(xm));

	long s = both ? n : 1;
	mglPoint p,q;

	for(k=0;k<ax.nz;k++)
	{
		if(ax.nz>1)	zVal = Min.z+(Max.z-Min.z)*mreal(k)/(ax.nz-1);
		for(i=0;i<n;i+=tx)	for(j=0;j<m;j+=ty)
		{
			jj = i+n*(j+m*k);
			if(both)
			{	ix = x.nz>k ? jj : i+n*j;	iy = y.nz>k ? jj : i+n*j;	}
			else
			{	ix = x.ny>k ? i+n*k : i;	iy = y.ny>k ? j+m*k : j;	}
			dx = i<n-1 ? (x.a[ix+1]-x.a[ix]) : (x.a[ix]-x.a[ix-1]);
			dy = j<m-1 ? (y.a[iy+s]-y.a[iy]) : (y.a[iy]-y.a[iy-s]);
			dx *= tx;	dy *= ty;

			p = mglPoint(x.a[ix], y.a[iy], zVal);
			x_ = p.x;	y_ = p.y;	z_ = p.z;
			if(!ScalePoint(x_,y_,z_))	continue;
			q = inv ? mglPoint(-ax.a[jj],-ay.a[jj],0) : mglPoint(ax.a[jj],ay.a[jj],0);
			dd = hypot(ax.a[jj],ay.a[jj]);
			Drop(p,q,(dx<dy?dx:dy)/2,GetC(2*dd*xm-1,false),dd*xm);
		}
	}
	EndGroup();
}
//-----------------------------------------------------------------------------
void mglGraph::Dew(const mglData &ax, const mglData &ay, const char *sch, mreal zVal)
{
	if(ax.nx<2 || ax.ny<2)	{	SetWarn(mglWarnLow,"Vect");	return;	}
	mglData x(ax.nx), y(ax.ny);
	x.Fill(Min.x,Max.x);
	y.Fill(Min.y,Max.y);
	Dew(x,y,ax,ay,sch,zVal);
}
//-----------------------------------------------------------------------------
//		Dual plotting functions
//-----------------------------------------------------------------------------
/// Plot dew drops for vector field {ax,ay} parametrically depended on coordinate {x,y}
void mgl_dew_xy(HMGL gr, const HMDT x, const HMDT y, const HMDT ax, const HMDT ay, const char *sch,mreal zVal)
{	if(gr && ay && ax && x && y)	gr->Dew(*x, *y, *ax, *ay, sch, zVal);	}
/// Plot dew drops for vector field {ax,ay}
void mgl_dew_2d(HMGL gr, const HMDT ax, const HMDT ay, const char *sch,mreal zVal)
{	if(gr && ay && ax)	gr->Dew(*ax, *ay, sch, zVal);	}
/// Plot arrows {ax,ay,az} along a curve {x,y,y}
void mgl_traj_xy(HMGL gr, const HMDT x, const HMDT y, const HMDT ax, const HMDT ay, const char *sch, mreal zVal, mreal len)
{	if(gr && ay && ax && x && y)	gr->Traj(*x, *y, *ax, *ay, sch, zVal, len);	}
void mgl_traj_xyz(HMGL gr, const HMDT x, const HMDT y, const HMDT z, const HMDT ax, const HMDT ay, const HMDT az, const char *sch, mreal len)
{	if(gr && ay && ax && az && z && x && y)	gr->Traj(*x, *y, *z, *ax, *ay, *az, sch, len);	}
/// Plot vector field {ax,ay} parametrically depended on coordinate {x,y} with length proportional to value |a|
void mgl_vect_xy(HMGL gr, const HMDT x, const HMDT y, const HMDT ax, const HMDT ay, const char *sch,mreal zVal,int flag)
{	if(gr && ay && ax && x && y)	gr->Vect(*x, *y, *ax, *ay, sch, zVal,flag);	}
void mgl_vectl_xy(HMGL gr, const HMDT x, const HMDT y, const HMDT ax, const HMDT ay, const char *sch,mreal zVal)
{	if(gr && ay && ax && x && y)	gr->VectL(*x, *y, *ax, *ay, sch, zVal);	}
/// Plot vector field {ax,ay} with length proportional to value |a|
void mgl_vect_2d(HMGL gr, const HMDT ax, const HMDT ay, const char *sch,mreal zVal,int flag)
{	if(gr && ay && ax)	gr->Vect(*ax, *ay, sch, zVal, flag);	}
void mgl_vectl_2d(HMGL gr, const HMDT ax, const HMDT ay, const char *sch,mreal zVal)
{	if(gr && ay && ax)	gr->VectL(*ax, *ay, sch, zVal);	}
/// Plot vector field {ax,ay} parametrically depended on coordinate {x,y} with color proportional to value |a|
void mgl_vectc_xy(HMGL gr, const HMDT x, const HMDT y, const HMDT ax, const HMDT ay, const char *sch,mreal zVal)
{	if(gr && ay && ax && x && y)	gr->VectC(*x, *y, *ax, *ay, sch, zVal);	}
/// Plot vector field {ax,ay} with color proportional to value |a|
void mgl_vectc_2d(HMGL gr, const HMDT ax, const HMDT ay, const char *sch,mreal zVal)
{	if(gr && ay && ax)	gr->VectC(*ax, *ay, sch, zVal);	}
/// Plot 3d vector field {ax,ay,ay} parametrically depended on coordinate {x,y,z} with length proportional to value |a|
void mgl_vect_xyz(HMGL gr, const HMDT x, const HMDT y, const HMDT z, const HMDT ax, const HMDT ay, const HMDT az, const char *sch,int flag)
{	if(gr && ay && ax && az && z && x && y)	gr->Vect(*x, *y, *z, *ax, *ay, *az, sch,flag);	}
void mgl_vectl_xyz(HMGL gr, const HMDT x, const HMDT y, const HMDT z, const HMDT ax, const HMDT ay, const HMDT az, const char *sch)
{	if(gr && ay && ax && az && z && x && y)	gr->VectL(*x, *y, *z, *ax, *ay, *az, sch);	}
/// Plot 3d vector field {ax,ay,ay} with length proportional to value |a|
void mgl_vect_3d(HMGL gr, const HMDT ax, const HMDT ay, const HMDT az, const char *sch,int flag)
{	if(gr && ay && ax && az)	gr->Vect(*ax, *ay, *az, sch,flag);	}
void mgl_vectl_3d(HMGL gr, const HMDT ax, const HMDT ay, const HMDT az, const char *sch)
{	if(gr && ay && ax && az)	gr->VectL(*ax, *ay, *az, sch);	}
/// Plot 3d vector field {ax,ay,ay} parametrically depended on coordinate {x,y,z} with color proportional to value |a|
void mgl_vectc_xyz(HMGL gr, const HMDT x, const HMDT y, const HMDT z, const HMDT ax, const HMDT ay, const HMDT az, const char *sch)
{	if(gr && ay && ax && az && z && x && y)	gr->VectC(*x, *y, *z, *ax, *ay, *az, sch);	}
/// Plot 3d vector field {ax,ay,ay} with color proportional to value |a|
void mgl_vectc_3d(HMGL gr, const HMDT ax, const HMDT ay, const HMDT az, const char *sch)
{	if(gr && ay && ax && az)	gr->VectC(*ax, *ay, *az, sch);	}
/// Color map of matrix a to matrix b, both matrix parametrically depend on coordinates
void mgl_map_xy(HMGL gr, const HMDT x, const HMDT y, const HMDT a, const HMDT b, const char *sch, int ks, int pnts)
{	if(gr && a && b && x && y)	gr->Map(*x, *y, *a, *b, sch, ks, pnts);	}
/// Color map of matrix a to matrix b
void mgl_map(HMGL gr, const HMDT a, const HMDT b, const char *sch, int ks, int pnts)
{	if(gr && a && b)	gr->Map(*a, *b, sch, ks, pnts);	}
//-----------------------------------------------------------------------------
//		Dual plotting functions (FORTRAN)
//-----------------------------------------------------------------------------
/// Plot dew drops for vector field {ax,ay} parametrically depended on coordinate {x,y}
void mgl_dew_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *ax, uintptr_t *ay, const char *sch, mreal *zVal,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && ay && ax && x && y)	_GR_->Dew(_D_(x), _D_(y), _D_(ax), _D_(ay), s, *zVal);
	delete []s;
}
/// Plot dew drops for vector field {ax,ay}
void mgl_dew_2d_(uintptr_t *gr, uintptr_t *ax, uintptr_t *ay, const char *sch, mreal *zVal,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && ay && ax)	_GR_->Dew(_D_(ax), _D_(ay), s, *zVal);
	delete []s;
}
/// Plot arrows {ax,ay,az} along a curve {x,y,y}
void mgl_traj_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *ax, uintptr_t *ay, const char *sch, mreal *zVal, mreal *len,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && ay && ax && x && y)
		_GR_->Traj(_D_(x), _D_(y), _D_(ax), _D_(ay), s, *zVal, *len);
	delete []s;
}
void mgl_traj_xyz_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *ax, uintptr_t *ay, uintptr_t *az, const char *sch,mreal *len,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && ay && ax && az && z && x && y)
		_GR_->Traj(_D_(x), _D_(y), _D_(z), _D_(ax), _D_(ay), _D_(az), s, *len);
	delete []s;
}
/// Plot vector field {ax,ay} parametrically depended on coordinate {x,y} with length proportional to value |a|
void mgl_vect_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *ax, uintptr_t *ay, const char *sch, mreal *zVal,int *flag,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && ay && ax && x && y)
		_GR_->Vect(_D_(x), _D_(y), _D_(ax), _D_(ay), s, *zVal, *flag);
	delete []s;
}
void mgl_vectl_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *ax, uintptr_t *ay, const char *sch, mreal *zVal,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && ay && ax && x && y)	_GR_->VectL(_D_(x), _D_(y), _D_(ax), _D_(ay), s, *zVal);
	delete []s;
}
/// Plot vector field {ax,ay} with length proportional to value |a|
void mgl_vect_2d_(uintptr_t *gr, uintptr_t *ax, uintptr_t *ay, const char *sch, mreal *zVal,int *flag,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && ay && ax)	_GR_->Vect(_D_(ax), _D_(ay), s, *zVal,*flag);
	delete []s;
}
void mgl_vectl_2d_(uintptr_t *gr, uintptr_t *ax, uintptr_t *ay, const char *sch, mreal *zVal,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && ay && ax)	_GR_->VectL(_D_(ax), _D_(ay), s, *zVal);
	delete []s;
}
/// Plot vector field {ax,ay} parametrically depended on coordinate {x,y} with color proportional to value |a|
void mgl_vectc_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *ax, uintptr_t *ay, const char *sch, mreal *zVal,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && ay && ax && x && y)	_GR_->VectC(_D_(x), _D_(y), _D_(ax), _D_(ay), s, *zVal);
	delete []s;
}
/// Plot vector field {ax,ay} with color proportional to value |a|
void mgl_vectc_2d_(uintptr_t *gr, uintptr_t *ax, uintptr_t *ay, const char *sch,mreal *zVal,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && ay && ax)	_GR_->VectC(_D_(ax), _D_(ay), s, *zVal);
	delete []s;
}
/// Plot 3d vector field {ax,ay,ay} parametrically depended on coordinate {x,y,z} with length proportional to value |a|
void mgl_vect_xyz_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *ax, uintptr_t *ay, uintptr_t *az, const char *sch,int *flag,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && ay && ax && az && z && x && y)
		_GR_->Vect(_D_(x), _D_(y), _D_(z), _D_(ax), _D_(ay), _D_(az), s, *flag);
	delete []s;
}
void mgl_vectl_xyz_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *ax, uintptr_t *ay, uintptr_t *az, const char *sch,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && ay && ax && az && z && x && y)	_GR_->VectL(_D_(x), _D_(y), _D_(z), _D_(ax), _D_(ay), _D_(az), s);
	delete []s;
}
/// Plot 3d vector field {ax,ay,ay} with length proportional to value |a|
void mgl_vect_3d_(uintptr_t *gr, uintptr_t *ax, uintptr_t *ay, uintptr_t *az, const char *sch, int *flag, int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && ay && ax && az)	_GR_->Vect(_D_(ax), _D_(ay), _D_(az), s, *flag);
	delete []s;
}
void mgl_vectl_3d_(uintptr_t *gr, uintptr_t *ax, uintptr_t *ay, uintptr_t *az, const char *sch, int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && ay && ax && az)	_GR_->VectL(_D_(ax), _D_(ay), _D_(az), s);
	delete []s;
}
/// Plot 3d vector field {ax,ay,ay} parametrically depended on coordinate {x,y,z} with color proportional to value |a|
void mgl_vectc_xyz_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *ax, uintptr_t *ay, uintptr_t *az, const char *sch,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && ay && ax && az && z && x && y)	_GR_->VectC(_D_(x), _D_(y), _D_(z), _D_(ax), _D_(ay), _D_(az), s);
	delete []s;
}
/// Plot 3d vector field {ax,ay,ay} with color proportional to value |a|
void mgl_vectc_3d_(uintptr_t *gr, uintptr_t *ax, uintptr_t *ay, uintptr_t *az, const char *sch,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && ay && ax && az)	_GR_->VectC(_D_(ax), _D_(ay), _D_(az), s);
	delete []s;
}
/// Color map of matrix a to matrix b, both matrix parametrically depend on coordinates
void mgl_map_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *a, uintptr_t *b, const char *sch, int *ks, int *pnts,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a && b && x && y)	_GR_->Map(_D_(x), _D_(y), _D_(a), _D_(b), s, *ks, *pnts);
	delete []s;
}
/// Color map of matrix a to matrix b
void mgl_map_(uintptr_t *gr, uintptr_t *a, uintptr_t *b, const char *sch, int *ks, int *pnts,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a && b)	_GR_->Map(_D_(a), _D_(b), s, *ks, *pnts);
	delete []s;
}
//-----------------------------------------------------------------------------
