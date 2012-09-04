/***************************************************************************
 * mgl_3d.cpp is part of Math Graphic Library
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
//	CloudQ series
//
//-----------------------------------------------------------------------------
void mglGraph::Cloud(const mglData &x, const mglData &y, const mglData &z, const mglData &a,
					const char *sch, mreal alpha)
{
	long i,j,k,n=a.nx,m=a.ny,l=a.nz;
	register int i0,i1;
	if(n<2 || m<2 || l<2)	{	SetWarn(mglWarnLow,"CloudQ");	return;	}
	bool both = x.nx*x.ny*x.nz==n*m*l && y.nx*y.ny*y.nz==n*m*l && z.nx*z.ny*z.nz==n*m*l;
	if(!(both || (x.nx==n && y.nx==m && z.nx==l)))
	{	SetWarn(mglWarnDim);	return;	}
	static int cgid=1;	StartGroup("CloudQ",cgid++);

	int tx=1,ty=1,tz=1;
	if(MeshNum>1)
	{	tx=(n-1)/(MeshNum-1);	ty=(m-1)/(MeshNum-1);	tz=(l-1)/(MeshNum-1);}
	if(tx<1)	tx=1;	if(ty<1)	ty=1;	if(tz<1)	tz=1;

	if(alpha<0)	alpha = AlphaDef;
	if(sch && strchr(sch,'-'))	alpha = -alpha;
	alpha /= pow(n/tx*m/ty*l/tz,1./3)/CloudFactor/5;
	if(alpha>1)	alpha = 1;
	SetScheme(sch);
	mreal *pp = new mreal[3*n*m*l];
	if(!pp)	{	SetWarn(mglWarnMem);	return;	}
	mreal *aa = new mreal[n*m*l];

	// x, y -- матрицы как и z
	if(both)	for(i=0;i<n/tx;i++)	for(j=0;j<m/ty;j++)	for(k=0;k<l/tz;k++)
	{
		i0 = i+(n/tx)*(j+(m/ty)*k);
		i1 = i*tx+n*(j*ty+m*k*tz);
		pp[3*i0]  = x.a[i1];
		pp[3*i0+1]= y.a[i1];
		pp[3*i0+2]= z.a[i1];
		aa[i0] = a.a[i1];
	}
	// x, y -- вектора
	else	for(i=0;i<n/tx;i++)	for(j=0;j<m/ty;j++)	for(k=0;k<l/tz;k++)
	{
		i0 = i+(n/tx)*(j+(m/ty)*k);
		pp[3*i0]  = x.a[i*tx];
		pp[3*i0+1]= y.a[j*ty];
		pp[3*i0+2]= z.a[k*tz];
		aa[i0] = a.a[i*tx+n*(j*ty+m*k*tz)];
	}
	bool al=Alpha(true);
	cloud_plot(n/tx,m/ty,l/tz,pp,aa,alpha);
	Alpha(al);
	EndGroup();
	delete []pp;	delete []aa;
}
//-----------------------------------------------------------------------------
void mglGraph::Cloud(const mglData &a, const char *sch, mreal alpha)
{
	if(a.nx<2 || a.ny<2 || a.nz<2)
	{	SetWarn(mglWarnLow,"CloudQ");	return;	}
	mglData x(a.nx), y(a.ny),z(a.nz);
	x.Fill(Min.x,Max.x);
	y.Fill(Min.y,Max.y);
	z.Fill(Min.z,Max.z);
	Cloud(x,y,z,a,sch,alpha);
}
//-----------------------------------------------------------------------------
//
//	Surf3 series
//
//-----------------------------------------------------------------------------
long mglGraph::add_spoint(long &pc,mreal **p,mreal **k,mreal **b,mreal **n,
			mreal x,mreal y,mreal z,mreal nx,mreal ny,mreal nz,
			mreal k1,mreal k2,mreal k3,mreal a)
{
	static long pMax=0;
	if(!ScalePoint(x,y,z) || a==0)	return -1;
	mglColor c;
	if(OnCoord)	c = GetC(x,y,z);
	else		c = Pal[100];
	if(!(*p))
	{
		pMax = 100;	pc = 0;
		*p = (mreal *)malloc(3*pMax*sizeof(mreal));
		*n = (mreal *)malloc(3*pMax*sizeof(mreal));
		*k = (mreal *)malloc(3*pMax*sizeof(mreal));
		*b = (mreal *)malloc(4*pMax*sizeof(mreal));
	}
	b[0][4*pc] = c.r;	b[0][4*pc+1] = c.g;	b[0][4*pc+2] = c.b;
	b[0][4*pc+3] = Transparent ? a : 1;
	p[0][3*pc] = x;		p[0][3*pc+1] = y;	p[0][3*pc+2] = z;
	n[0][3*pc] = nx;	n[0][3*pc+1] = ny;	n[0][3*pc+2] = nz;
	k[0][3*pc] = k1;	k[0][3*pc+1] = k2;	k[0][3*pc+2] = k3;
	pc++;
	if(pc>=pMax)
	{
		pMax += 100;
		*p = (mreal *)realloc(*p,3*pMax*sizeof(mreal));
		*n = (mreal *)realloc(*n,3*pMax*sizeof(mreal));
		*k = (mreal *)realloc(*k,3*pMax*sizeof(mreal));
		*b = (mreal *)realloc(*b,4*pMax*sizeof(mreal));
	}
	return pc-1;
}
//-----------------------------------------------------------------------------
void normal_3d(const mglData &a,mreal x,mreal y,mreal z,mreal *nx,mreal *ny,
			mreal *nz,bool inv)
{
	long n=a.nx, m=a.ny, l=a.nz, o=n*m;
	register long i,j,k,i0;
//	i=x<n-1?long(x):n-2;	j=y<m-1?long(y):m-2;
//	k=z<l-1?long(z):l-2;	i0=i+n*(j+m*k);
	i=long(x);	j=long(y);	k=long(z);
	i = i<n-1 ? i:n-2;	j = j<m-1 ? j:m-2;	k = k<l-1 ? k:l-2;
	x-=i;	y-=j;	z-=k;	i0=i+n*(j+m*k);


	if(n==2)	*nx = a.a[i0+1] - a.a[i0];
	else if(i==0)	*nx = (a.a[i0+1] - a.a[i0])*(1-x) + (a.a[i0+2] - a.a[i0])*x/2;
	else if(i==n-2)	*nx = (a.a[i0+1] - a.a[i0-1])*(1-x)/2 + (a.a[i0+1] - a.a[i0])*x;
	else	*nx = ((a.a[i0+1] - a.a[i0-1])*(1-x) + (a.a[i0+2] - a.a[i0])*x)/2;

	if(m==2)	*ny = a.a[i0+n] - a.a[i0];
	else if(j==0)	*ny = (a.a[i0+n] - a.a[i0])*(1-y) + (a.a[i0+2*n] - a.a[i0])*y/2;
	else if(j==m-2)	*ny = (a.a[i0+n] - a.a[i0-n])*(1-y)/2 + (a.a[i0+n] - a.a[i0])*y;
	else	*ny = ((a.a[i0+n] - a.a[i0-n])*(1-y) + (a.a[i0+2*n] - a.a[i0])*y)/2;

	if(l==2)	*nz = a.a[i0+o] - a.a[i0];
	else if(k==0)	*nz = (a.a[i0+o] - a.a[i0])*(1-z) + (a.a[i0+2*o] - a.a[i0])*z/2;
	else if(k==l-2)	*nz = (a.a[i0+o] - a.a[i0-o])*(1-z)/2 + (a.a[i0+o] - a.a[i0])*z;
	else	*nz = ((a.a[i0+o] - a.a[i0-o])*(1-z) + (a.a[i0+2*o] - a.a[i0])*z)/2;
	if(!inv)	{	*nx = -*nx;	*ny = -*ny;	*nz = -*nz;		}
}
//-----------------------------------------------------------------------------
void normal_1d(const mglData &a,mreal x,mreal *nx,bool inv)
{
	register long n=a.nx, i=long(x);

	if(n==2 || x<=1)	*nx = a.a[i+1] - a.a[i];
	else if(x>=n-2)		*nx = a.a[i] - a.a[i-1];
	else	*nx = ((a.a[i+1] - a.a[i-1])*(i+1-x) + (a.a[i+2] - a.a[i])*(x-i))/2;
	if(!inv)	*nx = -*nx;
	if(*nx==0)	*nx=1;
}
//-----------------------------------------------------------------------------
void mglGraph::Surf3(mreal val, const mglData &x, const mglData &y, const mglData &z, const mglData &a,
					const char *sch)
{
	long i,j,k,i0,i1,n=a.nx,m=a.ny,l=a.nz;
	long *kx1,*kx2,*ky1,*ky2,*kz,pc;
	bool both, wire = sch && strchr(sch,'#');
	mreal *pp=0,*cc=0,*kk=0,*nn=0;
	mreal d,xx,yy,zz,nx,ny,nz,dx,dy,dz,tx,ty,tz;
	if(n<2 || m<2 || l<2)	{	SetWarn(mglWarnLow,"Surf3");	return;	}
	both = x.nx*x.ny*x.nz==n*m*l && y.nx*y.ny*y.nz==n*m*l && z.nx*z.ny*z.nz==n*m*l;
	if(!(both || (x.nx==n && y.nx==m && z.nx==l)))
	{	SetWarn(mglWarnDim,"Surf3");	return;	}
	static int cgid=1;	StartGroup("Surf3",cgid++);

	bool inv = (sch && strchr(sch,'-'));
	SetScheme(sch);
	kx1 = new long[n*m];	kx2 = new long[n*m];
	ky1 = new long[n*m];	ky2 = new long[n*m];
	kz  = new long[n*m];
	Pal[100]=GetC(val);
//	DefColor(c,AlphaDef);

	for(k=0;k<l;k++)
	{
		memcpy(kx1,kx2,n*m*sizeof(long));	memset(kx2,-1,n*m*sizeof(long));
		memcpy(ky1,ky2,n*m*sizeof(long));	memset(ky2,-1,n*m*sizeof(long));
		memset(kz ,-1,n*m*sizeof(long));
		for(i=0;i<n;i++)	for(j=0;j<m;j++)
		{
			i1 = i+n*j;	i0 = i1+n*m*k;
			if(i<n-1)
			{
				d = _d(val,a.a[i0],a.a[i0+1]);
				if(d>=0 && d<1)
				{
					xx = both ? x.a[i0]*(1.-d)+x.a[i0+1]*d : x.a[i]*(1.-d)+x.a[i+1]*d;
					yy = both ? y.a[i0]*(1.-d)+y.a[i0+1]*d : y.a[j];
					zz = both ? z.a[i0]*(1.-d)+z.a[i0+1]*d : z.a[k];
					normal_3d(a,i+d,j,k,&dx,&dy,&dz,inv);
					if(both)
					{
						normal_3d(x,i+d,j,k,&tx,&ty,&tz,true);
						nx = (dx*tx+dy*ty+dz*tz)/(tx*tx+ty*ty+tz*tz);
						normal_3d(y,i+d,j,k,&tx,&ty,&tz,true);
						ny = (dx*tx+dy*ty+dz*tz)/(tx*tx+ty*ty+tz*tz);
						normal_3d(z,i+d,j,k,&tx,&ty,&tz,true);
						nz = (dx*tx+dy*ty+dz*tz)/(tx*tx+ty*ty+tz*tz);
					}
					else
					{
						normal_1d(x,i+d,&tx,true);		nx = dx/tx;
						normal_1d(y,j,&ty,true);		ny = dy/ty;
						normal_1d(z,k,&tz,true);		nz = dz/tz;
					}
					kx2[i1] = add_spoint(pc,&pp,&kk,&cc,&nn,xx,yy,zz,nx,ny,nz,
										i+d,j,k,Transparent ? AlphaDef : 1);
				}
			}
			if(j<m-1)
			{
				d = _d(val,a.a[i0],a.a[i0+n]);
				if(d>=0 && d<1)
				{
					xx = both ? x.a[i0]*(1.-d)+x.a[i0+n]*d : x.a[i];
					yy = both ? y.a[i0]*(1.-d)+y.a[i0+n]*d : y.a[j]*(1.-d)+y.a[j+1]*d;
					zz = both ? z.a[i0]*(1.-d)+z.a[i0+n]*d : z.a[k];
					normal_3d(a,i,j+d,k,&dx,&dy,&dz,inv);
					if(both)
					{
						normal_3d(x,i,j+d,k,&tx,&ty,&tz,true);
						nx = (dx*tx+dy*ty+dz*tz)/(tx*tx+ty*ty+tz*tz);
						normal_3d(y,i,j+d,k,&tx,&ty,&tz,true);
						ny = (dx*tx+dy*ty+dz*tz)/(tx*tx+ty*ty+tz*tz);
						normal_3d(z,i,j+d,k,&tx,&ty,&tz,true);
						nz = (dx*tx+dy*ty+dz*tz)/(tx*tx+ty*ty+tz*tz);
					}
					else
					{
						normal_1d(x,i,&tx,true);		nx = dx/tx;
						normal_1d(y,j+d,&ty,true);		ny = dy/ty;
						normal_1d(z,k,&tz,true);		nz = dz/tz;
					}
					ky2[i1] = add_spoint(pc,&pp,&kk,&cc,&nn,xx,yy,zz,nx,ny,nz,
										i,j+d,k,Transparent ? AlphaDef : 1);
				}
			}
			if(k>0)
			{
				d = _d(val,a.a[i0-n*m],a.a[i0]);
				if(d>=0 && d<1)
				{
					xx = both ? x.a[i0-n*m]*(1.-d)+x.a[i0]*d : x.a[i];
					yy = both ? y.a[i0-n*m]*(1.-d)+y.a[i0]*d : y.a[j];
					zz = both ? z.a[i0-n*m]*(1.-d)+z.a[i0]*d : z.a[k-1]*(1.-d)+z.a[k]*d;
					normal_3d(a,i,j,k+d-1,&dx,&dy,&dz,inv);
					if(both)
					{
						normal_3d(x,i,j,k+d-1,&tx,&ty,&tz,true);
						nx = (dx*tx+dy*ty+dz*tz)/(tx*tx+ty*ty+tz*tz);
						normal_3d(y,i,j,k+d-1,&tx,&ty,&tz,true);
						ny = (dx*tx+dy*ty+dz*tz)/(tx*tx+ty*ty+tz*tz);
						normal_3d(z,i,j,k+d-1,&tx,&ty,&tz,true);
						nz = (dx*tx+dy*ty+dz*tz)/(tx*tx+ty*ty+tz*tz);
					}
					else
					{
						normal_1d(x,i,&tx,true);		nx = dx/tx;
						normal_1d(y,j,&ty,true);		ny = dy/ty;
						normal_1d(z,k+d-1,&tz,true);	nz = dz/tz;
					}
					kz[i1] = add_spoint(pc,&pp,&kk,&cc,&nn,xx,yy,zz,nx,ny,nz,
									i,j,k+d-1,Transparent ? AlphaDef : 1);
				}
			}
		}
		if(k>0)	surf3_plot(n,m,kx1,kx2,ky1,ky2,kz,pp,cc,kk,nn,wire);
	}
	EndGroup();
	delete []kx1;	delete []kx2;	delete []ky1;	delete []ky2;	delete []kz;
	if(pp)	{	free(pp);	free(kk);	free(cc);	free(nn);	}
}
//-----------------------------------------------------------------------------
void mglGraph::Surf3(mreal val, const mglData &a, const char *sch)
{
	if(a.nx<2 || a.ny<2 || a.nz<2)	{	SetWarn(mglWarnLow,"Surf3");	return;	}
	mglData x(a.nx), y(a.ny),z(a.nz);
	x.Fill(Min.x,Max.x);
	y.Fill(Min.y,Max.y);
	z.Fill(Min.z,Max.z);
	Surf3(val,x,y,z,a,sch);
}
//-----------------------------------------------------------------------------
void mglGraph::Surf3(const mglData &x, const mglData &y, const mglData &z, const mglData &a,
					const char *stl, int num)
{
	mreal v;
	num = num<=0 ? 1 : num;
	for(long i=0;i<num;i++)
	{
		v = Cmax + (Cmin-Cmax)*(i+1.)/(num+1);
		Surf3(v,x,y,z,a,stl);
	}
}
//-----------------------------------------------------------------------------
void mglGraph::Surf3(const mglData &a, const char *stl, int num)
{
	mreal v;
	num = num<=0 ? 1 : num;
	for(long i=0;i<num;i++)
	{
		v = Cmax + (Cmin-Cmax)*(i+1.)/(num+1);
		Surf3(v,a,stl);
	}
}
//-----------------------------------------------------------------------------
//
//	Surf3A series
//
//-----------------------------------------------------------------------------
void mglGraph::Surf3A(mreal val, const mglData &x, const mglData &y, const mglData &z, const mglData &a,
					const mglData &b, const char *sch)
{
	long i,j,k,i0,i1,n=a.nx,m=a.ny,l=a.nz;
	long *kx1,*kx2,*ky1,*ky2,*kz,pc;
	bool both, wire = sch && strchr(sch,'#');
	mreal *pp=0,*cc=0,*kk=0,*nn=0;
	mreal d,xx,yy,zz,nx,ny,nz,dx,dy,dz,tx,ty,tz,alpha;
	if(n<2 || m<2 || l<2)	{	SetWarn(mglWarnLow,"Surf3A");	return;	}
	both = x.nx*x.ny*x.nz==n*m*l && y.nx*y.ny*y.nz==n*m*l && z.nx*z.ny*z.nz==n*m*l;
	if(!(both || (x.nx==n && y.nx==m && z.nx==l)))
	{	SetWarn(mglWarnDim,"Surf3A");	return;	}
	if(b.nx*b.ny*b.nz!=n*m*l)	{	SetWarn(mglWarnDim,"Surf3A");	return;	}
	static int cgid=1;	StartGroup("Surf3A",cgid++);

	bool inv = (sch && strchr(sch,'-'));
	SetScheme(sch);
	kx1 = new long[n*m];	kx2 = new long[n*m];
	ky1 = new long[n*m];	ky2 = new long[n*m];
	kz  = new long[n*m];
	Pal[100]=GetC(val);
//	DefColor(c,AlphaDef);

	for(k=0;k<l;k++)
	{
		memcpy(kx1,kx2,n*m*sizeof(long));	memset(kx2,-1,n*m*sizeof(long));
		memcpy(ky1,ky2,n*m*sizeof(long));	memset(ky2,-1,n*m*sizeof(long));
		memset(kz ,-1,n*m*sizeof(long));
		for(i=0;i<n;i++)	for(j=0;j<m;j++)
		{
			i1 = i+n*j;	i0 = i1+n*m*k;
			if(i<n-1)
			{
				d = _d(val,a.a[i0],a.a[i0+1]);
				if(d>=0 && d<1)
				{
					xx = both ? x.a[i0]*(1.-d)+x.a[i0+1]*d : x.a[i]*(1.-d)+x.a[i+1]*d;
					yy = both ? y.a[i0]*(1.-d)+y.a[i0+1]*d : y.a[j];
					zz = both ? z.a[i0]*(1.-d)+z.a[i0+1]*d : z.a[k];
					alpha = b.a[i0]*(1.-d)+b.a[i0+1]*d;
					normal_3d(a,i+d,j,k,&dx,&dy,&dz,inv);
					if(both)
					{
						normal_3d(x,i+d,j,k,&tx,&ty,&tz,true);
						nx = (dx*tx+dy*ty+dz*tz)/(tx*tx+ty*ty+tz*tz);
						normal_3d(y,i+d,j,k,&tx,&ty,&tz,true);
						ny = (dx*tx+dy*ty+dz*tz)/(tx*tx+ty*ty+tz*tz);
						normal_3d(z,i+d,j,k,&tx,&ty,&tz,true);
						nz = (dx*tx+dy*ty+dz*tz)/(tx*tx+ty*ty+tz*tz);
					}
					else
					{
						normal_1d(x,i+d,&tx,true);		nx = dx/tx;
						normal_1d(y,j,&ty,true);		ny = dy/ty;
						normal_1d(z,k,&tz,true);		nz = dz/tz;
					}
					kx2[i1] = add_spoint(pc,&pp,&kk,&cc,&nn,xx,yy,zz,nx,ny,nz,
							i+d,j,k,(GetA(alpha)+1)*(GetA(alpha)+1)/4);
				}
			}
			if(j<m-1)
			{
				d = _d(val,a.a[i0],a.a[i0+n]);
				if(d>=0 && d<1)
				{
					xx = both ? x.a[i0]*(1.-d)+x.a[i0+n]*d : x.a[i];
					yy = both ? y.a[i0]*(1.-d)+y.a[i0+n]*d : y.a[j]*(1.-d)+y.a[j+1]*d;
					zz = both ? z.a[i0]*(1.-d)+z.a[i0+n]*d : z.a[k];
					alpha = b.a[i0]*(1.-d)+b.a[i0+n]*d;
					normal_3d(a,i,j+d,k,&dx,&dy,&dz,inv);
					if(both)
					{
						normal_3d(x,i,j+d,k,&tx,&ty,&tz,true);
						nx = (dx*tx+dy*ty+dz*tz)/(tx*tx+ty*ty+tz*tz);
						normal_3d(y,i,j+d,k,&tx,&ty,&tz,true);
						ny = (dx*tx+dy*ty+dz*tz)/(tx*tx+ty*ty+tz*tz);
						normal_3d(z,i,j+d,k,&tx,&ty,&tz,true);
						nz = (dx*tx+dy*ty+dz*tz)/(tx*tx+ty*ty+tz*tz);
					}
					else
					{
						normal_1d(x,i,&tx,true);		nx = dx/tx;
						normal_1d(y,j+d,&ty,true);		ny = dy/ty;
						normal_1d(z,k,&tz,true);		nz = dz/tz;
					}
					ky2[i1] = add_spoint(pc,&pp,&kk,&cc,&nn,xx,yy,zz,nx,ny,nz,
							i,j+d,k,(GetA(alpha)+1)*(GetA(alpha)+1)/4);
				}
			}
			if(k>0)
			{
				d = _d(val,a.a[i0-n*m],a.a[i0]);
				if(d>=0 && d<1)
				{
					xx = both ? x.a[i0-n*m]*(1.-d)+x.a[i0]*d : x.a[i];
					yy = both ? y.a[i0-n*m]*(1.-d)+y.a[i0]*d : y.a[j];
					zz = both ? z.a[i0-n*m]*(1.-d)+z.a[i0]*d : z.a[k-1]*(1.-d)+z.a[k]*d;
					alpha = b.a[i0-n*m]*(1.-d)+b.a[i0]*d;
					normal_3d(a,i,j,k+d-1,&dx,&dy,&dz,inv);
					if(both)
					{
						normal_3d(x,i,j,k+d-1,&tx,&ty,&tz,true);
						nx = (dx*tx+dy*ty+dz*tz)/(tx*tx+ty*ty+tz*tz);
						normal_3d(y,i,j,k+d-1,&tx,&ty,&tz,true);
						ny = (dx*tx+dy*ty+dz*tz)/(tx*tx+ty*ty+tz*tz);
						normal_3d(z,i,j,k+d-1,&tx,&ty,&tz,true);
						nz = (dx*tx+dy*ty+dz*tz)/(tx*tx+ty*ty+tz*tz);
					}
					else
					{
						normal_1d(x,i,&tx,true);		nx = dx/tx;
						normal_1d(y,j,&ty,true);		ny = dy/ty;
						normal_1d(z,k+d-1,&tz,true);	nz = dz/tz;
					}
					kz[i1] = add_spoint(pc,&pp,&kk,&cc,&nn,xx,yy,zz,nx,ny,nz,
							i,j,k+d-1,(GetA(alpha)+1)*(GetA(alpha)+1)/4);
				}
			}
		}
		if(k>0)	surf3_plot(n,m,kx1,kx2,ky1,ky2,kz,pp,cc,kk,nn,wire);
	}
	EndGroup();
	delete []kx1;	delete []kx2;	delete []ky1;	delete []ky2;	delete []kz;
	if(pp)	{	free(pp);	free(kk);	free(cc);	free(nn);	}
}
//-----------------------------------------------------------------------------
void mglGraph::Surf3A(mreal val, const mglData &a, const mglData &b, const char *sch)
{
	if(a.nx<2 || a.ny<2 || a.nz<2)	{	SetWarn(mglWarnLow,"Surf3A");	return;	}
	mglData x(a.nx), y(a.ny),z(a.nz);
	x.Fill(Min.x,Max.x);
	y.Fill(Min.y,Max.y);
	z.Fill(Min.z,Max.z);
	Surf3A(val,x,y,z,a,b,sch);
}
//-----------------------------------------------------------------------------
void mglGraph::Surf3A(const mglData &x, const mglData &y, const mglData &z, const mglData &a, const mglData &b,
					const char *stl, int num)
{
	mreal v;
	num = num<=0 ? 1 : num;
	for(long i=0;i<num;i++)
	{
		v = Cmax + (Cmin-Cmax)*(i+1.)/(num+1);
		Surf3A(v,x,y,z,a,b,stl);
	}
}
//-----------------------------------------------------------------------------
void mglGraph::Surf3A(const mglData &a, const mglData &b, const char *stl, int num)
{
	mreal v;
	num = num<=0 ? 1 : num;
	for(long i=0;i<num;i++)
	{
		v = Cmax + (Cmin-Cmax)*(i+1.)/(num+1);
		Surf3A(v,a,b,stl);
	}
}
//-----------------------------------------------------------------------------
//
//	Surf3C series
//
//-----------------------------------------------------------------------------
void mglGraph::Surf3C(mreal val, const mglData &x, const mglData &y, const mglData &z, const mglData &a, const mglData &b, const char *sch)
{
	long i,j,k,i0,i1,n=a.nx,m=a.ny,l=a.nz;
	long *kx1,*kx2,*ky1,*ky2,*kz,pc;
	bool both, wire = sch && strchr(sch,'#');
	mreal *pp=0,*cc=0,*kk=0,*nn=0;
	mreal d,xx,yy,zz,nx,ny,nz,dx,dy,dz,tx,ty,tz,alpha;
	if(n<2 || m<2 || l<2)	{	SetWarn(mglWarnLow,"Surf3C");	return;	}
	both = x.nx*x.ny*x.nz==n*m*l && y.nx*y.ny*y.nz==n*m*l && z.nx*z.ny*z.nz==n*m*l;
	if(!(both || (x.nx==n && y.nx==m && z.nx==l)))
	{	SetWarn(mglWarnDim,"Surf3C");	return;	}
	if(b.nx*b.ny*b.nz!=n*m*l)	{	SetWarn(mglWarnDim,"Surf3C");	return;	}
	static int cgid=1;	StartGroup("Surf3C",cgid++);

	bool inv = (sch && strchr(sch,'-'));
	SetScheme(sch);
	kx1 = new long[n*m];	kx2 = new long[n*m];
	ky1 = new long[n*m];	ky2 = new long[n*m];
	kz  = new long[n*m];
	Pal[100]=GetC(val);
//	DefColor(c,AlphaDef);

	for(k=0;k<l;k++)
	{
		memcpy(kx1,kx2,n*m*sizeof(long));	memset(kx2,-1,n*m*sizeof(long));
		memcpy(ky1,ky2,n*m*sizeof(long));	memset(ky2,-1,n*m*sizeof(long));
		memset(kz ,-1,n*m*sizeof(long));
		for(i=0;i<n;i++)	for(j=0;j<m;j++)
		{
			i1 = i+n*j;	i0 = i1+n*m*k;
			if(i<n-1)
			{
				d = _d(val,a.a[i0],a.a[i0+1]);
				if(d>=0 && d<1)
				{
					xx = both ? x.a[i0]*(1.-d)+x.a[i0+1]*d : x.a[i]*(1.-d)+x.a[i+1]*d;
					yy = both ? y.a[i0]*(1.-d)+y.a[i0+1]*d : y.a[j];
					zz = both ? z.a[i0]*(1.-d)+z.a[i0+1]*d : z.a[k];
					alpha = b.a[i0]*(1.-d)+b.a[i0+1]*d;
					normal_3d(a,i+d,j,k,&dx,&dy,&dz,inv);
					if(both)
					{
						normal_3d(x,i+d,j,k,&tx,&ty,&tz,true);
						nx = (dx*tx+dy*ty+dz*tz)/(tx*tx+ty*ty+tz*tz);
						normal_3d(y,i+d,j,k,&tx,&ty,&tz,true);
						ny = (dx*tx+dy*ty+dz*tz)/(tx*tx+ty*ty+tz*tz);
						normal_3d(z,i+d,j,k,&tx,&ty,&tz,true);
						nz = (dx*tx+dy*ty+dz*tz)/(tx*tx+ty*ty+tz*tz);
					}
					else
					{
						normal_1d(x,i+d,&tx,true);		nx = dx/tx;
						normal_1d(y,j,&ty,true);		ny = dy/ty;
						normal_1d(z,k,&tz,true);		nz = dz/tz;
					}
					Pal[100]=GetC(alpha);
					kx2[i1] = add_spoint(pc,&pp,&kk,&cc,&nn,xx,yy,zz,nx,ny,nz,
							i+d,j,k,Transparent ? AlphaDef : 1);
				}
			}
			if(j<m-1)
			{
				d = _d(val,a.a[i0],a.a[i0+n]);
				if(d>=0 && d<1)
				{
					xx = both ? x.a[i0]*(1.-d)+x.a[i0+n]*d : x.a[i];
					yy = both ? y.a[i0]*(1.-d)+y.a[i0+n]*d : y.a[j]*(1.-d)+y.a[j+1]*d;
					zz = both ? z.a[i0]*(1.-d)+z.a[i0+n]*d : z.a[k];
					alpha = b.a[i0]*(1.-d)+b.a[i0+n]*d;
					normal_3d(a,i,j+d,k,&dx,&dy,&dz,inv);
					if(both)
					{
						normal_3d(x,i,j+d,k,&tx,&ty,&tz,true);
						nx = (dx*tx+dy*ty+dz*tz)/(tx*tx+ty*ty+tz*tz);
						normal_3d(y,i,j+d,k,&tx,&ty,&tz,true);
						ny = (dx*tx+dy*ty+dz*tz)/(tx*tx+ty*ty+tz*tz);
						normal_3d(z,i,j+d,k,&tx,&ty,&tz,true);
						nz = (dx*tx+dy*ty+dz*tz)/(tx*tx+ty*ty+tz*tz);
					}
					else
					{
						normal_1d(x,i,&tx,true);		nx = dx/tx;
						normal_1d(y,j+d,&ty,true);		ny = dy/ty;
						normal_1d(z,k,&tz,true);		nz = dz/tz;
					}
					Pal[100]=GetC(alpha);
					ky2[i1] = add_spoint(pc,&pp,&kk,&cc,&nn,xx,yy,zz,nx,ny,nz,
							i,j+d,k,Transparent ? AlphaDef : 1);
				}
			}
			if(k>0)
			{
				d = _d(val,a.a[i0-n*m],a.a[i0]);
				if(d>=0 && d<1)
				{
					xx = both ? x.a[i0-n*m]*(1.-d)+x.a[i0]*d : x.a[i];
					yy = both ? y.a[i0-n*m]*(1.-d)+y.a[i0]*d : y.a[j];
					zz = both ? z.a[i0-n*m]*(1.-d)+z.a[i0]*d : z.a[k-1]*(1.-d)+z.a[k]*d;
					alpha = b.a[i0-n*m]*(1.-d)+b.a[i0]*d;
					normal_3d(a,i,j,k+d-1,&dx,&dy,&dz,inv);
					if(both)
					{
						normal_3d(x,i,j,k+d-1,&tx,&ty,&tz,true);
						nx = (dx*tx+dy*ty+dz*tz)/(tx*tx+ty*ty+tz*tz);
						normal_3d(y,i,j,k+d-1,&tx,&ty,&tz,true);
						ny = (dx*tx+dy*ty+dz*tz)/(tx*tx+ty*ty+tz*tz);
						normal_3d(z,i,j,k+d-1,&tx,&ty,&tz,true);
						nz = (dx*tx+dy*ty+dz*tz)/(tx*tx+ty*ty+tz*tz);
					}
					else
					{
						normal_1d(x,i,&tx,true);		nx = dx/tx;
						normal_1d(y,j,&ty,true);		ny = dy/ty;
						normal_1d(z,k+d-1,&tz,true);	nz = dz/tz;
					}
					Pal[100]=GetC(alpha);
					kz[i1] = add_spoint(pc,&pp,&kk,&cc,&nn,xx,yy,zz,nx,ny,nz,
							i,j,k+d-1,Transparent ? AlphaDef : 1);
				}
			}
		}
		if(k>0)	surf3_plot(n,m,kx1,kx2,ky1,ky2,kz,pp,cc,kk,nn,wire);
	}
	EndGroup();
	delete []kx1;	delete []kx2;	delete []ky1;	delete []ky2;	delete []kz;
	if(pp)	{	free(pp);	free(kk);	free(cc);	free(nn);	}
}
//-----------------------------------------------------------------------------
void mglGraph::Surf3C(mreal val, const mglData &a, const mglData &b, const char *sch)
{
	if(a.nx<2 || a.ny<2 || a.nz<2)	{	SetWarn(mglWarnLow,"Surf3C");	return;	}
	mglData x(a.nx), y(a.ny),z(a.nz);
	x.Fill(Min.x,Max.x);
	y.Fill(Min.y,Max.y);
	z.Fill(Min.z,Max.z);
	Surf3C(val,x,y,z,a,b,sch);
}
//-----------------------------------------------------------------------------
void mglGraph::Surf3C(const mglData &x, const mglData &y, const mglData &z, const mglData &a, const mglData &b,
				const char *stl, int num)
{
	mreal v;
	num = num<=0 ? 1 : num;
	for(long i=0;i<num;i++)
	{
		v = Cmax + (Cmin-Cmax)*(i+1.)/(num+1);
		Surf3C(v,x,y,z,a,b,stl);
	}
}
//-----------------------------------------------------------------------------
void mglGraph::Surf3C(const mglData &a, const mglData &b, const char *stl, int num)
{
	mreal v;
	num = num<=0 ? 1 : num;
	for(long i=0;i<num;i++)
	{
		v = Cmax + (Cmin-Cmax)*(i+1.)/(num+1);
		Surf3C(v,a,b,stl);
	}
}
//-----------------------------------------------------------------------------
//
//	Beam series
//
//-----------------------------------------------------------------------------
// flag & 0x1	--	accompanied coordinates
// flag & 0x2	--	project to r*z
// flag & 0x4	--	normalize field
void mglGraph::Beam(mreal val,const mglData &tr, const mglData &g1, const mglData &g2, const mglData &a,
		mreal r, const char *stl, int flag)
{
	long n = a.nz,m=a.nx,l=a.ny;
	if(n<2 || m<2 || l<2)	{	SetWarn(mglWarnLow);	return;	}
	if(a.Minimal()<0)		{	SetWarn(mglWarnNeg);	return;	}
	if(tr.nx<3 || tr.ny<n || g1.nx<3 || g1.ny<n || g2.nx<3 || g2.ny<n)
	{	SetWarn(mglWarnDim,"Beam");	return;	}
	mglData x(a),y(a),z(a),b(a);
	register long i,j,k,i0;
	mreal asum=1, asum0=1, amax;
	bool old = r<0;
	r = fabs(r);
	if(flag & 4)	for(j=0;j<m*l;j++)	asum0 += a.a[j]*a.a[j];
	if(asum0==0)	{	SetWarn(mglWarnZero,"Beam");	return;	}
	for(i=0;i<n;i++)
	{
		asum=amax=0;
		if(flag & 4)
		{
			for(j=0;j<m*l;j++)
			{
				asum += a.a[j+m*l*i]*a.a[j+m*l*i];
				amax = amax>a.a[j+m*l*i] ? amax : a.a[j+m*l*i];
			}
			if(amax==0)	{	asum=0;	amax=1;	}
			for(j=0;j<m*l;j++)	b.a[j+m*l*i] = b.a[j+m*l*i]*sqrt(asum/asum0)/amax;
		}
		for(j=0;j<m;j++)	for(k=0;k<l;k++)
		{
			i0 = j+m*(k+l*i);
			if(flag & 1)
			{
				x.a[i0] = 2*j/(m-1.)-1;
				y.a[i0] = 2*k/(l-1.)-1;
				z.a[i0] = Max.z*i/(n-1.);
			}
			else
			{
				x.a[i0] = tr.a[tr.nx*i] + g1.a[g1.nx*i]*(2*j/(m-1.)-1)*r +
								g2.a[g2.nx*i]*(2*k/(l-1.)-1)*r;
				y.a[i0] = tr.a[tr.nx*i+(old?2:1)] + g1.a[g1.nx*i+1]*(2*j/(m-1.)-1)*r +
								g2.a[g2.nx*i+1]*(2*k/(l-1.)-1)*r;
				z.a[i0] = tr.a[tr.nx*i+(old?1:2)] + g1.a[g1.nx*i+2]*(2*j/(m-1.)-1)*r +
								g2.a[g2.nx*i+2]*(2*k/(l-1.)-1)*r;
			}
			if(flag & 2)	x.a[i0] = hypot(x.a[i0],y.a[i0]);
		}
	}
	Surf3(val,x,y,z,b,stl);
}
//-----------------------------------------------------------------------------
void mglGraph::Beam(const mglData &tr, const mglData &g1, const mglData &g2, const mglData &a, mreal r,
		const char *stl, int flag, int num)
{
	mreal v;
	num = num<=0 ? 1 : num;
	for(long i=0;i<num;i++)
	{
		v = Cmax + (Cmin-Cmax)*(i+1.)/(num+1);
		Beam(v,tr,g1,g2,a,r,stl,flag);
	}
}
//-----------------------------------------------------------------------------
//
//	CloudP series
//
//-----------------------------------------------------------------------------
void mglGraph::AVertex(mreal x,mreal y,mreal z, mreal a,mreal alpha)
{
	mglColor c;
	if(!ScalePoint(x,y,z) || isnan(a))	return;
	a = GetA(a);
	if(OnCoord)	c = GetC(x,y,z);
	else		c = GetC(a,false);
	a = alpha>0 ? (a+1)*(a+1)/4 : -(a-1)*(a-1)/4;
//	a = alpha>0 ? (a+1)/2 : (a-1)/2;
	if(a)	Ball(x,y,z,c,-alpha*a);
}
//-----------------------------------------------------------------------------
void mglGraph::CloudP(const mglData &x, const mglData &y, const mglData &z, const mglData &a, const char *sch, mreal alpha, bool rnd)
{
	register long i,j,k,n=a.nx,m=a.ny,l=a.nz;
	if(n<2 || m<2 || l<2)	{	SetWarn(mglWarnLow,"CloudP");	return;	}
	bool both = x.nx*x.ny*x.nz==n*m*l && y.nx*y.ny*y.nz==n*m*l && z.nx*z.ny*z.nz==n*m*l;
	if(!(both || (x.nx==n && y.nx==m && z.nx==l)))
	{	SetWarn(mglWarnDim,"CloudP");	return;	}
	static int cgid=1;	StartGroup("CloudP",cgid++);

	if(alpha<0)	alpha = AlphaDef;
	if(sch && strchr(sch,'-'))	alpha = -alpha;
	SetScheme(sch);
	alpha /= pow(n*m*l,1./3)/CloudFactor/15;
	if(alpha>1)	alpha = 1;

	// x, y -- матрицы как и z
	bool al=Alpha(true);
	register mreal xx,yy,zz,aa;
	if(both)
	{
		for(i=0;i<n*m*l;i++)
		{
			if(rnd)
			{	xx=x.a[i];	yy=y.a[i];	zz=z.a[i];	aa=a.a[i];	}
			else
			{
				register mreal tx,ty;
				tx=mgl_rnd();	ty=mgl_rnd();	zz=mgl_rnd();
				aa=a.Spline1(tx,ty,zz);	xx=x.Spline1(tx);
				yy=y.Spline1(ty);		zz=z.Spline1(zz);
			}
			AVertex(xx,yy,zz,aa,alpha);
		}
	}
	else	// x, y -- вектора
	{
		for(i=0;i<n;i++)	for(j=0;j<m;j++)	for(k=0;k<l;k++)
		{
			if(rnd)
			{	xx=x.a[i];	yy=y.a[j];	zz=z.a[k];	aa=a.a[i+n*(j+m*k)];	}
			else
			{
				xx=mgl_rnd();	yy=mgl_rnd();	zz=mgl_rnd();
				aa=a.Spline1(xx,yy,zz);	xx=x.Spline1(xx);
				yy=y.Spline1(yy);		zz=z.Spline1(zz);
			}
			AVertex(xx,yy,zz,aa,alpha);
		}
	}
	Alpha(al);
	EndGroup();
}
//-----------------------------------------------------------------------------
void mglGraph::CloudP(const mglData &a, const char *sch, mreal alpha, bool rnd)
{
	if(a.nx<2 || a.ny<2 || a.nz<2)
	{	SetWarn(mglWarnLow,"CloudP");	return;	}
	mglData x(a.nx), y(a.ny),z(a.nz);
	x.Fill(Min.x,Max.x);
	y.Fill(Min.y,Max.y);
	z.Fill(Min.z,Max.z);
	CloudP(x,y,z,a,sch,alpha,rnd);
}
//-----------------------------------------------------------------------------
//		3D plotting functions
//-----------------------------------------------------------------------------
/// Draw isosurface for 3d data specified parametrically
void mgl_surf3_xyz_val(HMGL gr, mreal Val, const HMDT x, const HMDT y, const HMDT z, const HMDT a, const char *sch)
{	if(gr && a && x && y && z)	gr->Surf3(Val, *x, *y, *z, *a, sch);	}
/// Draw isosurface for 3d data
void mgl_surf3_val(HMGL gr, mreal Val, const HMDT a, const char *sch)
{	if(gr && a)	gr->Surf3(Val, *a, sch);	}
/// Draw several isosurface for 3d data specified parametrically
void mgl_surf3_xyz(HMGL gr, const HMDT x, const HMDT y, const HMDT z, const HMDT a, const char *sch, int num)
{	if(gr && a && x && y && z)	gr->Surf3(*x, *y, *z, *a, sch, num);	}
/// Draw several isosurface for 3d data
void mgl_surf3(HMGL gr, const HMDT a, const char *sch, int num)
{	if(gr && a)	gr->Surf3(*a, sch, num);	}
/// Draw a cloud of points for 3d data specified parametrically
void mgl_cloudp_xyz(HMGL gr, const HMDT x, const HMDT y, const HMDT z, const HMDT a,
			const char *sch, mreal alpha)
{	if(gr && a && x && y && z)	gr->CloudP(*x, *y, *z, *a, sch, alpha);	}
/// Draw a cloud of points for 3d data
void mgl_cloudp(HMGL gr, const HMDT a, const char *sch, mreal alpha)
{	if(gr && a)	gr->CloudP(*a, sch, alpha);	}
/// Draw a semi-transparent cloud for 3d data specified parametrically
void mgl_cloud_xyz(HMGL gr, const HMDT x, const HMDT y, const HMDT z, const HMDT a,
			const char *sch, mreal alpha)
{	if(gr && a && x && y && z)	gr->Cloud(*x, *y, *z, *a, sch, alpha);	}
/// Draw a semi-transparent cloud for 3d data
void mgl_cloud(HMGL gr, const HMDT a, const char *sch, mreal alpha)
{	if(gr && a)	gr->Cloud(*a, sch, alpha);	}
//-----------------------------------------------------------------------------
/// Draw isosurface for 3d data \a a specified parametrically with alpha proportional to \a b
void mgl_surf3a_xyz_val(HMGL gr, mreal Val, const HMDT x, const HMDT y, const HMDT z, const HMDT a, const HMDT b, const char *sch)
{	if(gr && a && b && z && x && y)	gr->Surf3A(Val, *x, *y, *z, *a, *b, sch);	}
/// Draw isosurface for 3d data \a a with alpha proportional to \a b
void mgl_surf3a_val(HMGL gr, mreal Val, const HMDT a, const HMDT b, const char *sch)
{	if(gr && a && b)	gr->Surf3A(Val, *a, *b, sch);	}
/// Draw several isosurface for 3d data \a a specified parametrically with alpha proportional to \a b
void mgl_surf3a_xyz(HMGL gr, const HMDT x, const HMDT y, const HMDT z, const HMDT a, const HMDT b, const char *sch, int num)
{	if(gr && a && b && z && x && y)	gr->Surf3A(*x, *y, *z, *a, *b, sch, num);	}
/// Draw several isosurface for 3d data \a a with alpha proportional to \a b
void mgl_surf3a(HMGL gr, const HMDT a, const HMDT b, const char *sch, int num)
{	if(gr && a && b)	gr->Surf3A(*a, *b, sch, num);	}
/// Draw isosurface for 3d data \a a specified parametrically with color proportional to \a b
void mgl_surf3c_xyz_val(HMGL gr, mreal Val, const HMDT x, const HMDT y, const HMDT z, const HMDT a, const HMDT b, const char *sch)
{	if(gr && a && b && z && x && y)	gr->Surf3C(Val, *x, *y, *z, *a, *b, sch);	}
/// Draw isosurface for 3d data \a a with color proportional to \a b
void mgl_surf3c_val(HMGL gr, mreal Val, const HMDT a, const HMDT b, const char *sch)
{	if(gr && a && b)	gr->Surf3C(Val, *a, *b, sch);	}
/// Draw several isosurface for 3d data \a a specified parametrically with color proportional to \a b
void mgl_surf3c_xyz(HMGL gr, const HMDT x, const HMDT y, const HMDT z, const HMDT a, const HMDT b, const char *sch, int num)
{	if(gr && a && b && z && x && y)	gr->Surf3C(*x, *y, *z, *a, *b, sch, num);	}
/// Draw several isosurface for 3d data \a a with color proportional to \a b
void mgl_surf3c(HMGL gr, const HMDT a, const HMDT b, const char *sch, int num)
{	if(gr && a && b)	gr->Surf3C(*a, *b, sch, num);	}
/// Draw isosurface for 3d beam in curvilinear coordinates
void mgl_beam_val(HMGL gr, mreal val, const HMDT tr, const HMDT g1, const HMDT g2, const HMDT a, mreal r, const char *stl, int norm)
{	if(gr && tr && g1 && g2 && a)	gr->Beam(val,*tr,*g1,*g2,*a,r,stl,norm);	}
/// Draw several isosurfaces for 3d beam in curvilinear coordinates
void mgl_beam(HMGL gr, const HMDT tr, const HMDT g1, const HMDT g2, const HMDT a, mreal r,
		const char *stl, int norm, int num)
{	if(gr && tr && g1 && g2 && a)	gr->Beam(*tr,*g1,*g2,*a,r,stl,norm,num);	}
//-----------------------------------------------------------------------------
//		3D plotting functions (Fortran)
//-----------------------------------------------------------------------------
/// Draw isosurface for 3d data specified parametrically
void mgl_surf3_xyz_val_(uintptr_t *gr, mreal *Val, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *a, const char *sch,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a && x && y && z)	_GR_->Surf3(*Val, _D_(x), _D_(y), _D_(z), _D_(a), s);
	delete []s;
}
/// Draw isosurface for 3d data
void mgl_surf3_val_(uintptr_t *gr, mreal *Val, uintptr_t *a, const char *sch,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a)	_GR_->Surf3(*Val, _D_(a), s);
	delete []s;
}
/// Draw several isosurface for 3d data specified parametrically
void mgl_surf3_xyz_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *a, const char *sch, int *num,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a && x && y && z)	_GR_->Surf3(_D_(x), _D_(y), _D_(z), _D_(a), s, *num);
	delete []s;
}
/// Draw several isosurface for 3d data
void mgl_surf3_(uintptr_t *gr, uintptr_t *a, const char *sch, int *num,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a)	_GR_->Surf3(_D_(a), s, *num);
	delete []s;
}
/// Draw a cloud of points for 3d data specified parametrically
void mgl_cloudp_xyz_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *a,
			const char *sch, mreal *alpha,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a && x && y && z)	_GR_->CloudP(_D_(x), _D_(y), _D_(z), _D_(a), s, *alpha);
	delete []s;
}
/// Draw a cloud of points for 3d data
void mgl_cloudp_(uintptr_t *gr, uintptr_t *a, const char *sch, mreal *alpha,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a)	_GR_->CloudP(_D_(a), s, *alpha);
	delete []s;
}
/// Draw a semi-transparent cloud for 3d data specified parametrically
void mgl_cloud_xyz_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *a,
			const char *sch, mreal *alpha,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a && x && y && z)	_GR_->Cloud(_D_(x), _D_(y), _D_(z), _D_(a), s, *alpha);
	delete []s;
}
/// Draw a semi-transparent cloud for 3d data
void mgl_cloud_(uintptr_t *gr, uintptr_t *a, const char *sch, mreal *alpha,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a)	_GR_->Cloud(_D_(a), s, *alpha);
	delete []s;
}
//-----------------------------------------------------------------------------
/// Draw isosurface for 3d data \a a specified parametrically with alpha proportional to \a b
void mgl_surf3a_xyz_val_(uintptr_t *gr, mreal *Val, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *a, uintptr_t *b,
			const char *sch,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a && b && z && x && y)	_GR_->Surf3A(*Val, _D_(x), _D_(y), _D_(z), _D_(a), _D_(b), s);
	delete []s;
}
/// Draw isosurface for 3d data \a a with alpha proportional to \a b
void mgl_surf3a_val_(uintptr_t *gr, mreal *Val, uintptr_t *a, uintptr_t *b, const char *sch,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a && b)	_GR_->Surf3A(*Val, _D_(a), _D_(b), s);
	delete []s;
}
/// Draw several isosurface for 3d data \a a specified parametrically with alpha proportional to \a b
void mgl_surf3a_xyz_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *a, uintptr_t *b,
			const char *sch, int *num,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a && b && z && x && y)	_GR_->Surf3A(_D_(x), _D_(y), _D_(z), _D_(a), _D_(b), s, *num);
	delete []s;
}
/// Draw several isosurface for 3d data \a a with alpha proportional to \a b
void mgl_surf3a_(uintptr_t *gr, uintptr_t *a, uintptr_t *b, const char *sch, int *num,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a && b)	_GR_->Surf3A(_D_(a), _D_(b), s, *num);
	delete []s;
}
/// Draw isosurface for 3d data \a a specified parametrically with color proportional to \a b
void mgl_surf3c_xyz_val_(uintptr_t *gr, mreal *Val, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *a, uintptr_t *b,
			const char *sch,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a && b && z && x && y)	_GR_->Surf3C(*Val, _D_(x), _D_(y), _D_(z), _D_(a), _D_(b), s);
	delete []s;
}
/// Draw isosurface for 3d data \a a with color proportional to \a b
void mgl_surf3c_val_(uintptr_t *gr, mreal *Val, uintptr_t *a, uintptr_t *b, const char *sch,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a && b)	_GR_->Surf3C(*Val, _D_(a), _D_(b), s);
	delete []s;
}
/// Draw several isosurface for 3d data \a a specified parametrically with color proportional to \a b
void mgl_surf3c_xyz_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *a, uintptr_t *b,
			const char *sch, int *num,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a && b && z && x && y)	_GR_->Surf3C(_D_(x), _D_(y), _D_(z), _D_(a), _D_(b), s, *num);
	delete []s;
}
/// Draw several isosurface for 3d data \a a with color proportional to \a b
void mgl_surf3c_(uintptr_t *gr, uintptr_t *a, uintptr_t *b, const char *sch, int *num,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a && b)	_GR_->Surf3C(_D_(a), _D_(b), s, *num);
	delete []s;
}
/// Draw isosurface for 3d beam in curvilinear coordinates
void mgl_beam_val_(uintptr_t *gr, mreal *val, uintptr_t *tr, uintptr_t *g1, uintptr_t *g2, uintptr_t *a,
					mreal *r, const char *sch, int *norm,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && tr && g1 && g2 && a)	_GR_->Beam(*val,_D_(tr),_D_(g1),_D_(g2),_D_(a),*r,s,*norm);
	delete []s;
}
/// Draw several isosurfaces for 3d beam in curvilinear coordinates
void mgl_beam_(uintptr_t *gr, uintptr_t *tr, uintptr_t *g1, uintptr_t *g2, uintptr_t *a, mreal *r,
				const char *sch, int *norm, int *num,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && tr && g1 && g2 && a)	_GR_->Beam(_D_(tr), _D_(g1), _D_(g2), _D_(a), *r,s,*norm,*num);
	delete []s;
}
//-----------------------------------------------------------------------------
