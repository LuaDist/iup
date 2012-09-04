/***************************************************************************
 * mgl_crust.cpp is part of Math Graphic Library
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
#include <float.h>
#include "mgl/mgl.h"
#include "mgl/mgl_c.h"
#include "mgl/mgl_f.h"
//-----------------------------------------------------------------------------
//
//	TriPlot series
//
//-----------------------------------------------------------------------------
void mglGraph::TriPlot(const mglData &nums, const mglData &x, const mglData &y, const mglData &z, const mglData &a, const char *sch)
{
	long n = x.nx, m = nums.ny;
	if(y.nx!=n || z.nx!=n || nums.nx<3)	{	SetWarn(mglWarnLow,"TriPlot");	return;	}
	if(a.nx!=m && a.nx!=n)	{	SetWarn(mglWarnLow,"TriPlot");	return;	}
	SetScheme(sch);
	static int cgid=1;	StartGroup("TriPlot",cgid++);
	mreal *pp = new mreal[3*n], *cc = new mreal[4*(n>m ? n:m)];
	bool *tt = new bool[n];
	long *nn = new long[3*m];
	mglColor c;
	register long i,j,k1,k2,k3;
	for(i=0;i<n;i++)
	{
		pp[3*i] = x.a[i];	pp[3*i+1] = y.a[i];	pp[3*i+2] = z.a[i];
		tt[i] = ScalePoint(pp[3*i],pp[3*i+1],pp[3*i+2]);
	}
	for(i=0;i<a.nx;i++)
	{
		c = GetC(a.a[i]);	cc[4*i] = c.r;		cc[4*i+1] = c.g;
		cc[4*i+2] = c.b;	cc[4*i+3] = Transparent ? AlphaDef : 1;
	}
	for(i=j=0;i<m;i++)
	{
		k1 = long(nums.a[3*i]+0.1);		if(k1<0 || k1>=n)	continue;
		k2 = long(nums.a[3*i+1]+0.1);	if(k2<0 || k2>=n)	continue;
		k3 = long(nums.a[3*i+2]+0.1);	if(k3<0 || k3>=n)	continue;
		nn[3*j]=k1;	nn[3*j+1]=k2;	nn[3*j+2]=k3;	j++;
	}
	trigs_plot(j,nn,n,pp,cc,tt,sch && strchr(sch,'#'), a.nx==m);
	EndGroup();
	delete []pp;	delete []tt;	delete []cc;	delete []nn;
}
//-----------------------------------------------------------------------------
void mglGraph::TriPlot(const mglData &nums, const mglData &x, const mglData &y, const mglData &z, const char *sch)
{
	TriPlot(nums,x,y,z,z,sch);
}
//-----------------------------------------------------------------------------
void mglGraph::TriPlot(const mglData &nums, const mglData &x, const mglData &y, const char *sch, mreal zVal)
{
	if(isnan(zVal))	zVal = Min.z;
	mglData z(x.nx);
	z.Fill(zVal,zVal);
	TriPlot(nums,x,y,z,z,sch);
}
//-----------------------------------------------------------------------------
//
//	TriCont series
//
//-----------------------------------------------------------------------------
void mglGraph::tricont_line(mreal val, long i, long k1, long k2, long k3, const mglData &x, const mglData &y, const mglData &z, const mglData &a, mreal zVal)
{
	mreal d1,d2,pp[6];
	mglColor q1,q2,q3;
	d1 = _d(val,a.a[k1],a.a[k2]);
	d2 = _d(val,a.a[k1],a.a[k3]);
	if(d1<0 || d1>1 || d2<0 || d2>1)	return;
	if(isnan(zVal))
	{
		pp[2] = z.a[k1]*(1-d1)+z.a[k2]*d1;
		pp[5] = z.a[k1]*(1-d2)+z.a[k3]*d2;
	}
	else	pp[2] = pp[5] = zVal;

	pp[0] = x.a[k1]*(1-d1)+x.a[k2]*d1;
	pp[1] = y.a[k1]*(1-d1)+y.a[k2]*d1;
	if(!ScalePoint(pp[0],pp[1],pp[2]))	return;
	pp[3] = x.a[k1]*(1-d2)+x.a[k3]*d2;
	pp[4] = y.a[k1]*(1-d2)+y.a[k3]*d2;
	if(!ScalePoint(pp[3],pp[4],pp[5]))	return;
	curv_plot(2,pp,0);
}
//-----------------------------------------------------------------------------
void mglGraph::TriContV(const mglData &v, const mglData &nums, const mglData &x, const mglData &y, const mglData &z, const mglData &a, const char *sch,mreal zVal)
{
	long n = x.nx, m = nums.ny;
	if(y.nx!=n || z.nx!=n || nums.nx<3)	{	SetWarn(mglWarnLow,"TriCont");	return;	}
	if(a.nx!=m && a.nx!=n)	{	SetWarn(mglWarnLow,"TriCont");	return;	}
	SetScheme(sch);
	static int cgid=1;	StartGroup("TriCont",cgid++);
	register long i,k;
	long k1,k2,k3;
	for(k=0;k<v.nx;k++)	for(i=0;i<m;i++)
	{
		k1 = long(nums.a[3*i]+0.1);		if(k1<0 || k1>=n)	continue;
		k2 = long(nums.a[3*i+1]+0.1);	if(k2<0 || k2>=n)	continue;
		k3 = long(nums.a[3*i+2]+0.1);	if(k3<0 || k3>=n)	continue;
//		val = isnan(zVal) ? v.a[k] : zVal;
		DefColor(GetC(v.a[k]));
		tricont_line(v.a[k], i,k1,k2,k3,x,y,z,a,zVal);
		tricont_line(v.a[k], i,k2,k1,k3,x,y,z,a,zVal);
		tricont_line(v.a[k], i,k3,k2,k1,x,y,z,a,zVal);
	}
}
//-----------------------------------------------------------------------------
void mglGraph::TriCont(const mglData &nums, const mglData &x, const mglData &y, const mglData &z, const mglData &a, const char *sch, int Num, mreal zVal)
{
	if(Num<1)	{	SetWarn(mglWarnCnt,"Cont");	return;	}
	mglData v(Num);
	for(long i=0;i<Num;i++)	v.a[i] = Cmin + (Cmax-Cmin)*mreal(i+1)/(Num+1);
	TriContV(v,nums,x,y,z,a,sch,zVal);
}
//-----------------------------------------------------------------------------
void mglGraph::TriCont(const mglData &nums, const mglData &x, const mglData &y, const mglData &z, const char *sch, int Num, mreal zVal)
{	TriCont(nums,x,y,z,z,sch,Num,zVal);	}
//-----------------------------------------------------------------------------
void mglGraph::TriContV(const mglData &v, const mglData &nums, const mglData &x,
						const mglData &y, const mglData &z, const char *sch,mreal zVal)
{	TriContV(v,nums,x,y,z,z,sch,zVal);	}
//-----------------------------------------------------------------------------
//
//	QuadPlot series
//
//-----------------------------------------------------------------------------
void mglGraph::QuadPlot(const mglData &nums, const mglData &x, const mglData &y, const mglData &z, const mglData &a, const char *sch)
{
	long n = x.nx, m = nums.ny;
	if(y.nx!=n || z.nx!=n || nums.nx<4)	{	SetWarn(mglWarnLow,"QuadPlot");	return;	}
	if(a.nx!=m && a.nx!=n)	{	SetWarn(mglWarnLow,"QuadPlot");	return;	}
	SetScheme(sch);
	static int cgid=1;	StartGroup("QuadPlot",cgid++);
	mreal *pp = new mreal[3*n], *cc = new mreal[4*n];
	bool *tt = new bool[n];
	long *nn = new long[4*m];
	mglColor c;
	register long i,j,k1,k2,k3,k4;
	for(i=0;i<n;i++)
	{
		pp[3*i] = x.a[i];	pp[3*i+1] = y.a[i];	pp[3*i+2] = z.a[i];
		tt[i] = ScalePoint(pp[3*i],pp[3*i+1],pp[3*i+2]);
	}
	for(i=0;i<a.nx;i++)
	{
		c = GetC(a.a[i]);	cc[4*i] = c.r;		cc[4*i+1] = c.g;
		cc[4*i+2] = c.b;	cc[4*i+3] = Transparent ? AlphaDef : 1;
	}
	for(i=j=0;i<m;i++)
	{
		k1 = long(nums.a[4*i]+0.1);		if(k1<0 || k1>=n)	continue;
		k2 = long(nums.a[4*i+1]+0.1);	if(k2<0 || k2>=n)	continue;
		k3 = long(nums.a[4*i+2]+0.1);	if(k3<0 || k3>=n)	continue;
		k4 = long(nums.a[4*i+3]+0.1);	if(k4<0 || k4>=n)	continue;
		nn[4*j]=k1;	nn[4*j+1]=k2;	nn[4*j+2]=k3;	nn[4*j+4]=k4;	j++;
	}
	quads_plot(j,nn,n,pp,cc,tt,sch && strchr(sch,'#'), a.nx==m);
	EndGroup();
	delete []pp;	delete []tt;	delete []cc;	delete []nn;
}
//-----------------------------------------------------------------------------
void mglGraph::QuadPlot(const mglData &nums, const mglData &x, const mglData &y, const mglData &z, const char *sch)
{
	QuadPlot(nums,x,y,z,z,sch);
}
//-----------------------------------------------------------------------------
void mglGraph::QuadPlot(const mglData &nums, const mglData &x, const mglData &y, const char *sch, mreal zVal)
{
	if(isnan(zVal))	zVal = Min.z;
	mglData z(x.nx);
	z.Fill(zVal,zVal);
	QuadPlot(nums,x,y,z,z,sch);
}
//-----------------------------------------------------------------------------
//
//	Dots series
//
//-----------------------------------------------------------------------------
void mglGraph::Dots(const mglData &x, const mglData &y, const mglData &z, const mglData &a, const char *sch, mreal alpha)
{
	long n = x.nx;
	if(y.nx!=n || z.nx!=n || a.nx!=n)	{	SetWarn(mglWarnDim,"Dots");	return;	}
	static int cgid=1;	StartGroup("Dots",cgid++);
	if(alpha<0)	alpha = AlphaDef;
	if(sch && strchr(sch,'-'))	alpha = -alpha;
	SetScheme(sch);
	alpha /= pow(n,1./3)/CloudFactor/15;
	if(alpha>1)	alpha = 1;
	mglColor c;

//	bool al=Alpha(true);
	for(long i=0;i<n;i++)	AVertex(x.a[i], y.a[i], z.a[i], a.a[i], alpha);
//	Alpha(al);
	EndGroup();
}
//-----------------------------------------------------------------------------
void mglGraph::Dots(const mglData &x, const mglData &y, const mglData &z, const char *sch)
{
	long n = x.nx;
	if(y.nx!=n || z.nx!=n)	{	SetWarn(mglWarnDim,"Dots");	return;	}
	static int cgid=1;	StartGroup("Dots",cgid++);
	SetScheme(sch);
	mglColor c;
	for(long i=0;i<n;i++)
	{
		c = GetC(x.a[i], y.a[i], z.a[i]);
		Ball(x.a[i], y.a[i], z.a[i], c);
	}
	EndGroup();
}
//-----------------------------------------------------------------------------
void mglGraph::Dots(const mglData &tr, const char *sch)
{
	long n = tr.ny, m=tr.nx;
	if(m<3)	{	SetWarn(mglWarnLow,"Dots");	return;	}
	static int cgid=1;	StartGroup("Dots",cgid++);
	SetScheme(sch);
	mglColor c;
	if(m==3)	for(long i=0;i<n;i++)
	{
		c = GetC(tr.a[3*i], tr.a[3*i+1], tr.a[3*i+2]);
		Ball(tr.a[3*i], tr.a[3*i+1], tr.a[3*i+2], c);
	}
	else	for(long i=0;i<n;i++)
		AVertex(tr.a[m*i], tr.a[m*i+1], tr.a[m*i+2], tr.a[m*i+3], 1);
	EndGroup();
}
//-----------------------------------------------------------------------------
//
//	mglTriangulation
//
//-----------------------------------------------------------------------------
long mgl_crust_new(long n,const mreal *pp,long **nn,mreal ff);
long mgl_crust(long n,mreal *pp,long **nn,mreal rs);
mglData mglTriangulation(const mglData &x, const mglData &y, const mglData &z, mreal er)
{
	mglData nums;
	long n = x.nx, m;
	if(y.nx!=n || z.nx!=n)	return nums;
	register long i;
	mreal *pp = new mreal[3*n];
	long *nn=0;
	for(i=0;i<n;i++)
	{	pp[3*i] = x.a[i];	pp[3*i+1] = y.a[i];	pp[3*i+2] = z.a[i];	}
	m = mgl_crust(n,pp,&nn,er);

	if(m>0)
	{
		nums.Create(3,m);
		for(i=0;i<3*m;i++)	nums.a[i]=nn[i];
	}
	delete []pp;	free(nn);	return nums;
}
//-----------------------------------------------------------------------------
mglData mglTriangulation(const mglData &x, const mglData &y, mreal er)
{
	mglData z(x.nx);
	return mglTriangulation(x,y,z,er);
}
//-----------------------------------------------------------------------------
//
//	Crust series
//
//-----------------------------------------------------------------------------
void mglGraph::Crust(const mglData &x, const mglData &y, const mglData &z, const char *sch,mreal er)
{
	if(y.nx!=x.nx || z.nx!=x.nx)	{	SetWarn(mglWarnDim,"Crust");	return;	}
	mglData nums=mglTriangulation(x,y,z,er);
	TriPlot(nums,x,y,z,sch);
}
//-----------------------------------------------------------------------------
void mglGraph::Crust(const mglData &tr, const char *sch,mreal er)
{
	if(tr.nx<3)	{	SetWarn(mglWarnLow,"Crust");	return;	}
	Crust(tr.SubData(0), tr.SubData(1), tr.SubData(2),sch,er);
}
//-----------------------------------------------------------------------------
mreal mgl_dist(const mreal *p1, const mreal *p2)
{
	return (p1[0]-p2[0])*(p1[0]-p2[0])+(p1[1]-p2[1])*(p1[1]-p2[1])+(p1[2]-p2[2])*(p1[2]-p2[2]);
}
mreal mgl_mult(const mreal *p1, const mreal *p2, const mreal *p0)
{
	return (p1[0]-p0[0])*(p2[0]-p0[0])+(p1[1]-p0[1])*(p2[1]-p0[1])+(p1[2]-p0[2])*(p2[2]-p0[2]);
}
//-----------------------------------------------------------------------------
long mgl_insert_trig(long i1,long i2,long i3,long **n)
{
	static long Cur=0,Max=0;
	if(i1<0 || i2<0 || i3<0)	return Cur;
	if(*n==0)
	{
		Max = 1024;		Cur = 0;
		*n = (long *)malloc(Max*3*sizeof(long));
	}
	if(Cur>=Max)
	{
		Max += 1024;
		*n = (long *)realloc(*n,Max*3*sizeof(long));
	}
	long *nn = *n;
	register long i,k;
	for(k=0;k<Cur;k++)
	{
		i = 3*k;
		if((nn[i]==i1 && nn[i+1]==i2 && nn[i+2]==i3))	return Cur;
		if((nn[i]==i1 && nn[i+1]==i3 && nn[i+2]==i2))	return Cur;
		if((nn[i]==i2 && nn[i+1]==i3 && nn[i+2]==i1))	return Cur;
		if((nn[i]==i2 && nn[i+1]==i1 && nn[i+2]==i3))	return Cur;
		if((nn[i]==i3 && nn[i+1]==i2 && nn[i+2]==i1))	return Cur;
		if((nn[i]==i3 && nn[i+1]==i1 && nn[i+2]==i2))	return Cur;
	}
	i = 3*Cur;
	nn[i]=i1;	nn[i+1]=i2;	nn[i+2]=i3;
	Cur++;
	return Cur;
}
//-----------------------------------------------------------------------------
long mgl_get_next(long k1,long n,long *,long *set,mreal *qq)
{
	long i,j=-1;
	mreal r,rm=FLT_MAX;
	for(i=0;i<n;i++)
	{
		if(i==k1 || set[i]>0)	continue;
		r = mgl_dist(qq+3*i, qq+3*k1);
		if(r<rm)	{	rm=r;	j=i;	}
	}
	return j;
}
//-----------------------------------------------------------------------------
long mgl_crust(long n,mreal *pp,long **nn,mreal ff)
{
	register long i,j;
	register mreal r,rm,rs;
	if(ff==0)	ff=2;
	for(rs=0,i=0;i<n;i++)
	{
		for(rm = FLT_MAX,j=0;j<n;j++)
		{
			if(i==j)	continue;
			r = mgl_dist(pp+3*i,pp+3*j);
			if(rm>r)	rm = r;
		}
		rs += sqrt(rm);
	}
	rs *= ff/n;	rs = rs*rs;
	long ind[100], set[100], ii;	// indexes of "close" points, flag that it was added and its number
	mreal qq[300];	// normalized point coordinates
	long k1,k2,k3,m=0;
	for(i=0;i<n;i++)	// now the triangles will be found
	{
		memset(set,0,100*sizeof(long));
		for(ii=0,j=0;j<n;j++)	// find close vertexes
		{
			r = mgl_dist(pp+3*i,pp+3*j);
			if(r<=rs && j!=i)	{	ind[ii] = j;	ii++;	if(ii==99)	break;}
		}
		if(ii<3)	continue;	// nothing to do
		for(j=0;j<ii;j++)
		{
			k1 = 3*j;	k2 = 3*ind[j];	k3 = 3*i;
			qq[k1] = pp[k2] - pp[k3];
			qq[k1+1] = pp[k2+1] - pp[k3+1];
			qq[k1+2] = pp[k2+2] - pp[k3+2];
			r = sqrt(qq[k1]*qq[k1]+qq[k1+1]*qq[k1+1]+qq[k1+2]*qq[k1+2]);
			qq[k1] /= r;	qq[k1+1] /= r;	qq[k1+2] /= r;
		}
		k1 = 0;
		while((k2=mgl_get_next(k1,ii,ind,set,qq))>0)
		{
			set[k1]=1;
			m = mgl_insert_trig(i,ind[k1],ind[k2],nn);
			k1 = k2;
		}
		m = mgl_insert_trig(i,ind[k1],ind[0],nn);
	}
	return m;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define sqr(a) (a)*(a)
void mgl_add_trig(long i1,long i2,long n,const mreal *pp,long **nn,long *c,long *m,mreal ff,char *tt)
{
	register long i,im=-1,ii=-1;
	register mreal cm=2,c2=2,co,fm,f,g=0;
	const mreal *p1=pp+3*i1,*p2=pp+3*i2,*pi;
	fm =(sqr(p2[0]-p1[0])+sqr(p2[1]-p1[1])+sqr(p2[2]-p1[2]));	fm*=4*fm*ff;
	for(i=0;i<n;i++)
	{
		if(tt[i]>1 || i==i1 || i==i2)	continue;	// this is other surface
		pi = pp+3*i;
		co=(pi[0]-p1[0])*(pi[0]-p2[0])+(pi[1]-p1[1])*(pi[1]-p2[1])+(pi[2]-p1[2])*(pi[2]-p2[2]);
		f = (sqr(pi[0]-p1[0])+sqr(pi[1]-p1[1])+sqr(pi[2]-p1[2])) *
			(sqr(pi[0]-p2[0])+sqr(pi[1]-p2[1])+sqr(pi[2]-p2[2]));
		co /= sqrt(f);
		if(co<cm && f<fm && tt[i]==0)	{	g=f;	cm=co;	im=i;	}
		if(co<c2 && f<fm && tt[i]==1)	{	g=f;	c2=co;	ii=i;	}
//		if(co<cm && f<fm)	{	cm=co;	im=i;	}
	}
	bool ok=(im>0);	// try best variant
	long k[3]={i1,i2,im};
	if(i2<i1)		{k[0]=i2;	k[1]=i1;}	// sort vertexes
	if(im<k[0])		{k[2]=k[1];	k[0]=im;}
	if(k[2]<k[1])	{i=k[1];	k[1]=k[2];	k[2]=i;	}
	for(i=0;i<*c;i++)	// check if already exist
		if(!memcmp(nn+3*i,k,3*sizeof(long)))
		{	ok = false;	break;	}
printf("B(%g,%g,%d):%ld,%ld,%ld -- %ld of %ld\n",g,fm,ok,i1,i2,im,*c,n);

	if(!ok)			// try worse variant
	{
		im=ii;		ok=(im>0);
		long k[3]={i1,i2,im};
		if(i2<i1)		{k[0]=i2;	k[1]=i1;}	// sort vertexes
		if(im<k[0])		{k[2]=k[1];	k[0]=im;}
		if(k[2]<k[1])	{i=k[1];	k[1]=k[2];	k[2]=i;	}
		for(i=0;i<*c;i++)	// check if already exist
			if(!memcmp(nn+3*i,k,3*sizeof(long)))
			{	ok = false;	break;	}
	}
printf("B(%g,%g,%d):%ld,%ld,%ld -- %ld of %ld\n",g,fm,ok,i1,i2,im,*c,n);
	if(!ok)	return;

	if(*c>=*m)			// trig is OK -- add it
	{
		*m+=n;
		*nn=(long*)realloc(*nn,(*m)*sizeof(long));
	}
	memcpy(nn+3*(*c),k,3*sizeof(long));	*c+=1;	tt[im]=1;
printf("Add %ld,%ld,%ld -- %ld of %ld\n",i1,i2,im,*c,n);
	mgl_add_trig(i1,im,n,pp,nn,c,m,ff,tt);	// parse bound
	mgl_add_trig(i2,im,n,pp,nn,c,m,ff,tt);	// NOTE: it may require HUGE stack!!!
}
long mgl_crust_new(long n,const mreal *pp,long **nn,mreal ff)
{
	register long i,j,k=0;
	register mreal r,rm;
	if(ff==0)	ff=1;
	char *tt=new char[n],ok=1;	memset(tt,0,n*sizeof(char));
	long c=0,m=n;
	*nn = (long*)malloc(m*sizeof(long));

	while(ok)
	{
		j=-1;
		for(rm=FLT_MAX,i=0;i<n;i++)	// find closest
		{
			r = mgl_dist(pp+3*i,pp);
			if(rm>r && !tt[i] && i!=k)	{	j=i;	rm = r;	}
		}
printf("closest -- %ld-%ld of %g\n",k,j,rm);
		if(j>0)
		{
			tt[0]=tt[j]=1;
			mgl_add_trig(k,j,n,pp,nn,&c,&m,ff,tt);
		}
printf("first iteration done -- %ld of %ld\n",m,n);
		ok = 0;
		for(i=0;i<n;i++)	// check if one more surface is existed
			if(tt[k])	tt[k]=2;
			else	{	ok=1;	k=i;	}
	}
	delete []tt;
	return c;
}
//-----------------------------------------------------------------------------
/// Draw triangle mesh for points in arrays \a x, \a y, \a z.
void mgl_triplot_xyzc(HMGL gr, const HMDT nums, const HMDT x, const HMDT y, const HMDT z, const HMDT c, const char *sch)
{	if(gr&&nums&&x&&y&&z&&c)	gr->TriPlot(*nums, *x, *y, *z, *c, sch);	}
/// Draw triangle mesh for points in arrays \a x, \a y, \a z.
void mgl_triplot_xyz(HMGL gr, const HMDT nums, const HMDT x, const HMDT y, const HMDT z, const char *sch)
{	if(gr&&nums&&x&&y&&z)	gr->TriPlot(*nums, *x, *y, *z, sch);	}
/// Draw triangle mesh for points in arrays \a x, \a y.
void mgl_triplot_xy(HMGL gr, const HMDT nums, const HMDT x, const HMDT y, const char *sch, mreal zVal)
{	if(gr&&nums&&x&&y)	gr->TriPlot(*nums, *x, *y, sch, zVal);	}
/// Draw quad mesh for points in arrays \a x, \a y, \a z.
void mgl_quadplot_xyzc(HMGL gr, const HMDT nums, const HMDT x, const HMDT y, const HMDT z, const HMDT c, const char *sch)
{	if(gr&&nums&&x&&y&&z&&c)	gr->QuadPlot(*nums, *x, *y, *z, *c, sch);	}
/// Draw quad mesh for points in arrays \a x, \a y, \a z.
void mgl_quadplot_xyz(HMGL gr, const HMDT nums, const HMDT x, const HMDT y, const HMDT z, const char *sch)
{	if(gr&&nums&&x&&y&&z)	gr->QuadPlot(*nums, *x, *y, *z, sch);	}
/// Draw quad mesh for points in arrays \a x, \a y.
void mgl_quadplot_xy(HMGL gr, const HMDT nums, const HMDT x, const HMDT y, const char *sch, mreal zVal)
{	if(gr&&nums&&x&&y)	gr->QuadPlot(*nums, *x, *y, sch, zVal);	}

/// Draw contours for triangle mesh for points in arrays \a x, \a y, \a z.
void mgl_tricont_xyzcv(HMGL gr, const HMDT v, const HMDT nums, const HMDT x, const HMDT y, const HMDT z, const HMDT c, const char *sch, mreal zVal)
{	if(gr&&nums&&x&&y&&z&&c)	gr->TriContV(*v, *nums, *x, *y, *z, *c, sch, zVal);	}
void mgl_tricont_xyzc(HMGL gr, const HMDT nums, const HMDT x, const HMDT y, const HMDT z, const HMDT c, const char *sch, int n, mreal zVal)
{	if(gr&&nums&&x&&y&&z&&c)	gr->TriCont(*nums, *x, *y, *z, *c, sch, n, zVal);	}
/// Draw contours for triangle mesh for points in arrays \a x, \a y, \a z.
void mgl_tricont_xyzv(HMGL gr, const HMDT v, const HMDT nums, const HMDT x, const HMDT y, const HMDT z, const char *sch, mreal zVal)
{	if(gr&&nums&&x&&y&&z)	gr->TriContV(*v, *nums, *x, *y, *z, sch, zVal);	}
void mgl_tricont_xyz(HMGL gr, const HMDT nums, const HMDT x, const HMDT y, const HMDT z, const char *sch, int n, mreal zVal)
{	if(gr&&nums&&x&&y&&z)	gr->TriCont(*nums, *x, *y, *z, sch, n, zVal);	}

/// Draw dots in points \a x, \a y, \a z.
void mgl_dots(HMGL gr, const HMDT x, const HMDT y, const HMDT z, const char *sch)
{	if(gr&&x&&y&&z)	gr->Dots(*x,*y,*z,sch);	}
/// Draw half-transparent dots in points \a x, \a y, \a z.
void mgl_dots_a(HMGL gr, const HMDT x, const HMDT y, const HMDT z, const HMDT a, const char *sch)
{	if(gr&&x&&y&&z)	gr->Dots(*x,*y,*z,*a,sch);	}
/// Draw dots in points \a tr.
void mgl_dots_tr(HMGL gr, const HMDT tr, const char *sch)
{	if(gr&&tr)	gr->Dots(*tr,sch);	}
/// Draw power crust for points in arrays \a x, \a y, \a z.
void mgl_crust(HMGL gr, const HMDT x, const HMDT y, const HMDT z, const char *sch, mreal er)
{	if(gr&&x&&y&&z)	gr->Crust(*x,*y,*z,sch,er);	}
/// Draw power crust for points in arrays \a tr.
void mgl_crust_tr(HMGL gr, const HMDT tr, const char *sch, mreal er)
{	if(gr&&tr)	gr->Crust(*tr,sch,er);	}
//-----------------------------------------------------------------------------
//	Fortran interface
//-----------------------------------------------------------------------------
/// Draw triangle mesh for points in arrays \a x, \a y, \a z and color it by \a c.
void mgl_triplot_xyzc_(uintptr_t *gr, uintptr_t *nums, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *c, const char *sch,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && nums && x && y && z && c)
		_GR_->TriPlot(_D_(nums), _D_(x), _D_(y), _D_(z), _D_(c), s);
	delete []s;
}
/// Draw triangle mesh for points in arrays \a x, \a y, \a z.
void mgl_triplot_xyz_(uintptr_t *gr, uintptr_t *nums, uintptr_t *x, uintptr_t *y, uintptr_t *z, const char *sch,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && nums && x && y && z)	_GR_->TriPlot(_D_(nums), _D_(x), _D_(y), _D_(z), s);
	delete []s;
}
/// Draw triangle mesh for points in arrays \a x, \a y.
void mgl_triplot_xy_(uintptr_t *gr, uintptr_t *nums, uintptr_t *x, uintptr_t *y, const char *sch, mreal *zVal,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr&&nums&&x&&y)	_GR_->TriPlot(_D_(nums), _D_(x), _D_(y), s, *zVal);
	delete []s;
}
/// Draw triangle mesh for points in arrays \a x, \a y, \a z and color it by \a c.
void mgl_quadplot_xyzc_(uintptr_t *gr, uintptr_t *nums, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *c, const char *sch,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && nums && x && y && z && c)
		_GR_->QuadPlot(_D_(nums), _D_(x), _D_(y), _D_(z), _D_(c), s);
	delete []s;
}
/// Draw triangle mesh for points in arrays \a x, \a y, \a z.
void mgl_quadplot_xyz_(uintptr_t *gr, uintptr_t *nums, uintptr_t *x, uintptr_t *y, uintptr_t *z, const char *sch,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && nums && x && y && z)	_GR_->QuadPlot(_D_(nums), _D_(x), _D_(y), _D_(z), s);
	delete []s;
}
/// Draw triangle mesh for points in arrays \a x, \a y.
void mgl_quadplot_xy_(uintptr_t *gr, uintptr_t *nums, uintptr_t *x, uintptr_t *y, const char *sch, mreal *zVal,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr&&nums&&x&&y)	_GR_->QuadPlot(_D_(nums), _D_(x), _D_(y), s, *zVal);
	delete []s;
}

/// Draw triangle mesh for points in arrays \a x, \a y, \a z and color it by \a c.
void mgl_tricont_xyzcv_(uintptr_t *gr, uintptr_t *v, uintptr_t *nums, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *c, const char *sch, mreal *zVal,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && nums && x && y && z && c)
		_GR_->TriContV(_D_(v), _D_(nums), _D_(x), _D_(y), _D_(z), _D_(c), s, *zVal);
	delete []s;
}
/// Draw triangle mesh for points in arrays \a x, \a y, \a z.
void mgl_tricont_xyzv_(uintptr_t *gr, uintptr_t *v, uintptr_t *nums, uintptr_t *x, uintptr_t *y, uintptr_t *z, const char *sch, mreal *zVal,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && nums && x && y && z)	_GR_->TriContV(_D_(v), _D_(nums), _D_(x), _D_(y), _D_(z), s, *zVal);
	delete []s;
}
/// Draw triangle mesh for points in arrays \a x, \a y, \a z and color it by \a c.
void mgl_tricont_xyzc_(uintptr_t *gr, uintptr_t *nums, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *c, const char *sch, int *n, mreal *zVal, int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && nums && x && y && z && c)
		_GR_->TriCont( _D_(nums), _D_(x), _D_(y), _D_(z), _D_(c), s, *n, *zVal);
	delete []s;
}
/// Draw triangle mesh for points in arrays \a x, \a y, \a z.
void mgl_tricont_xyz_(uintptr_t *gr, uintptr_t *nums, uintptr_t *x, uintptr_t *y, uintptr_t *z, const char *sch, int *n, mreal *zVal, int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && nums && x && y && z)	_GR_->TriCont(_D_(nums), _D_(x), _D_(y), _D_(z), s, *n, *zVal);
	delete []s;
}

/// Draw dots in points \a x, \a y, \a z.
void mgl_dots_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, const char *sch,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && x && y && z)	_GR_->Dots(_D_(x),_D_(y),_D_(z),s);
	delete []s;
}
/// Draw dots in points \a x, \a y, \a z.
void mgl_dots_a_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *a, const char *sch,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && x && y && z)	_GR_->Dots(_D_(x),_D_(y),_D_(z),_D_(a),s);
	delete []s;
}
/// Draw dots in points \a tr.
void mgl_dots_tr_(uintptr_t *gr, uintptr_t *tr, const char *sch,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && tr)	_GR_->Dots(_D_(tr),s);
	delete []s;
}
/// Draw power crust for points in arrays \a x, \a y, \a z.
void mgl_crust_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, const char *sch, mreal *er,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && x && y && z)	_GR_->Crust(_D_(x),_D_(y),_D_(z),s,*er);
	delete []s;
}
/// Draw power crust for points in arrays \a tr.
void mgl_crust_tr_(uintptr_t *gr, uintptr_t *tr, const char *sch, mreal *er,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && tr)	_GR_->Crust(_D_(tr),s,*er);
	delete []s;
}
//-----------------------------------------------------------------------------
