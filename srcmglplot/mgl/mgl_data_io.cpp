/***************************************************************************
 * mgl_data_io.cpp is part of Math Graphic Library
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
#include <ctype.h>
#include <math.h>
#include <string.h>
#include <zlib.h>
#ifdef HAVE_HDF5
#include <hdf5.h>
#endif
#ifdef HAVE_HDF4
#define intf hdf4_intf
#include <hdf/mfhdf.h>
#undef intf
#endif

#ifndef WIN32
#include <glob.h>
#endif

#include "mgl/mgl_eval.h"
#include "mgl/mgl_data.h"

//#define isn(ch)		((ch)<' ' && (ch)!='\t')
#define isn(ch)		((ch)=='\n')
//-----------------------------------------------------------------------------
void mglData::Set(const char *v,int NX,int NY,int NZ)
{
	if(NX<1 || NY <1 || NZ<1)	return;
	register int i,j=0,m=NX*NY*NZ;
	mreal *b = new mreal[m];	memset(b,0,m*sizeof(mreal));
	for(i=0;i<m;i++)
	{
		while(isspace(v[j]) && v[j])	j++;
		if(v[j]==0)	break;
		b[i] = atof(v+j);
		while(!isspace(v[j])&& v[j])	j++;
	}
	delete []a;
	a=b;	nx=NX;	ny=NY;	nz=NZ;	NewId();
}
//-----------------------------------------------------------------------------
void mglData::Set(gsl_vector *v)
{
#ifndef NO_GSL
	if(!v || v->size<1)	return;
	Create(v->size);
	for(long i=0;i<nx;i++)	a[i] = v->data[i*v->stride];
#endif
}
//-----------------------------------------------------------------------------
void mglData::Set(gsl_matrix *m)
{
#ifndef NO_GSL
	if(!m || m->size1<1 || m->size2<1)	return;
	Create(m->size1,m->size2);
	register long i,j;
	for(i=0;i<nx;i++)	for(j=0;j<ny;j++)
		a[i+j*nx] = m->data[i * m->tda + j];
#endif
}
//-----------------------------------------------------------------------------
void mglData::Set(const float *A,int NX,int NY,int NZ)
{
	if(NX<=0 || NY<=0 || NZ<=0)	return;
	Create(NX,NY,NZ);
#if(MGL_USE_DOUBLE==1)
	for(long i=0;i<NX*NY*NZ;i++)	a[i] = A[i];
#else
	memcpy(a,A,NX*NY*NZ*sizeof(float));
#endif
}
//-----------------------------------------------------------------------------
void mglData::Set(const double *A,int NX,int NY,int NZ)
{
	if(NX<=0 || NY<=0 || NZ<=0)	return;
	Create(NX,NY,NZ);
#if(MGL_USE_DOUBLE==1)
	memcpy(a,A,NX*NY*NZ*sizeof(double));
#else
	for(long i=0;i<NX*NY*NZ;i++)	a[i] = A[i];
#endif
}
//-----------------------------------------------------------------------------
void mglData::Set(const float **A,int N1,int N2)
{
	if(N1<=0 || N2<=0)	return;
	Create(N2,N1);
#if(MGL_USE_DOUBLE==1)
	for(long i=0;i<N1;i++)	for(long j=0;j<N2;j++)	a[j+i*N2] = A[i][j];
#else
	for(long i=0;i<N1;i++)
		memcpy(a+i*N2,A[i],N2*sizeof(float));
#endif
}
//-----------------------------------------------------------------------------
void mglData::Set(const double **A,int N1,int N2)
{
	if(N1<=0 || N2<=0)	return;
	Create(N2,N1);
#if(MGL_USE_DOUBLE==1)
	for(long i=0;i<N1;i++)
		memcpy(a+i*N2,A[i],N2*sizeof(double));
#else
	for(long i=0;i<N1;i++)	for(long j=0;j<N2;j++)	a[j+i*N2] = A[i][j];
#endif
}
//-----------------------------------------------------------------------------
void mglData::Set(const float ***A,int N1,int N2,int N3)
{
	if(N1<=0 || N2<=0 || N3<=0)	return;
	Create(N3,N2,N1);
#if(MGL_USE_DOUBLE==1)
	for(long i=0;i<N1;i++)	for(long j=0;j<N2;j++)	for(long k=0;k<N3;k++)
		a[k+N3*(j+i*N2)] = A[i][j][k];
#else
	for(long i=0;i<N1;i++)	for(long j=0;j<N2;j++)
		memcpy(a+N3*(j+i*N2),A[i][j],N3*sizeof(float));
#endif
}
//-----------------------------------------------------------------------------
void mglData::Set(const double ***A,int N1,int N2,int N3)
{
	if(N1<=0 || N2<=0 || N3<=0)	return;
	Create(N3,N2,N1);
#if(MGL_USE_DOUBLE==1)
	for(long i=0;i<N1;i++)	for(long j=0;j<N2;j++)
		memcpy(a+N3*(j+i*N2),A[i][j],N3*sizeof(double));
#else
	for(long i=0;i<N1;i++)	for(long j=0;j<N2;j++)	for(long k=0;k<N3;k++)
		a[k+N3*(j+i*N2)] = A[i][j][k];
#endif
}
//-----------------------------------------------------------------------------
mglData mglData::Trace() const
{
	mglData r(nx);
	register long i;
	if(ny>=nx && nz>=nx)	for(i=0;i<nx;i++)	r.a[i] = a[i+nx*(i+ny*i)];
	else if(ny>=nx)		for(i=0;i<nx;i++)	r.a[i] = a[i+nx*i];
	else	memcpy(r.a,a,nx*sizeof(mreal));
	return r;
}
//-----------------------------------------------------------------------------
mglData mglData::SubData(const mglData &xx, const mglData &yy, const mglData &zz) const
{
	long n=0,m=0,l=0,i,j,k,i0,x,y,z;
	mglData d;
	bool ix=false, iy=false, iz=false;
	if(xx.nz>1)	// 3d data
	{
		n = xx.nx;	m = xx.ny;	l = xx.nz;
		j = yy.nx*yy.ny*yy.nz;	if(j>1 && j!=n*m*l)	return d;	// wrong sizes
		k = zz.nx*zz.ny*zz.nz;	if(k>1 && k!=n*m*l)	return d;	// wrong sizes
		ix = true;	iy = j>1;	iz = k>1;
	}
	else if(yy.nz>1)
	{
		n = yy.nx;	m = yy.ny;	l = yy.nz;
		j = xx.nx*xx.ny*xx.nz;	if(j>1 && j!=n*m*l)	return d;	// wrong sizes
		k = zz.nx*zz.ny*zz.nz;	if(k>1 && k!=n*m*l)	return d;	// wrong sizes
		iy = true;	ix = j>1;	iz = k>1;
	}
	else if(zz.nz>1)
	{
		n = zz.nx;	m = zz.ny;	l = zz.nz;
		j = yy.nx*yy.ny*yy.nz;	if(j>1 && j!=n*m*l)	return d;	// wrong sizes
		k = xx.nx*xx.ny*xx.nz;	if(k>1 && k!=n*m*l)	return d;	// wrong sizes
		iz = true;	iy = j>1;	ix = k>1;
	}
	else if(xx.ny>1)	// 2d data
	{
		n = xx.nx;	m = xx.ny;	l = 1;
		j = yy.nx*yy.ny;	if(j>1 && j!=n*m)	return d;	// wrong sizes
		k = zz.nx*zz.ny;	if(k>1 && k!=n*m)	return d;	// wrong sizes
		ix = true;	iy = j>1;	iz = k>1;
	}
	else if(yy.ny>1)
	{
		n = yy.nx;	m = yy.ny;	l = 1;
		j = xx.nx*xx.ny;	if(j>1 && j!=n*m)	return d;	// wrong sizes
		k = zz.nx*zz.ny;	if(k>1 && k!=n*m)	return d;	// wrong sizes
		iy = true;	ix = j>1;	iz = k>1;
	}
	else if(zz.ny>1)
	{
		n = zz.nx;	m = zz.ny;	l = 1;
		j = yy.nx*yy.ny;	if(j>1 && j!=n*m)	return d;	// wrong sizes
		k = xx.nx*xx.ny;	if(k>1 && k!=n*m)	return d;	// wrong sizes
		iz = true;	iy = j>1;	ix = k>1;
	}
	if(n*m*l>1)	// this is 2d or 3d data
	{
		d.Create(n,m,l);
		for(i0=0;i0<n*m*l;i0++)
		{
			i = int((ix?xx.a[i0]:xx.a[0])+0.5);	if(i<0)i=0;	if(i>=nx)i=nx-1;
			j = int((iy?yy.a[i0]:yy.a[0])+0.5);	if(j<0)j=0;	if(j>=ny)j=ny-1;
			k = int((iz?zz.a[i0]:zz.a[0])+0.5);	if(k<0)k=0;	if(k>=nz)k=nz-1;
			d.a[i0] = a[i+nx*(j+ny*k)];
		}
		return d;
	}
	// this is 1d data -> try as normal SubData()
	if(xx.nx>1 || xx.a[0]>=0)	{	n=xx.nx;	ix=true;	}
	else	{	n=nx;	ix=false;	}
	if(yy.nx>1 || yy.a[0]>=0)	{	m=yy.nx;	iy=true;	}
	else	{	m=ny;	iy=false;	}
	if(zz.nx>1 || zz.a[0]>=0)	{	l=zz.nx;	iz=true;	}
	else	{	l=nz;	iz=false;	}
	d.Create(n,m,l);
	for(i=0;i<n;i++)	for(j=0;j<m;j++)	for(k=0;k<l;k++)
	{
		x = ix?int(xx.a[i]+0.5):i;	if(x<0)x=0;	if(x>=nx)x=nx-1;
		y = iy?int(yy.a[j]+0.5):j;	if(y<0)y=0;	if(y>=ny)y=ny-1;
		z = iz?int(zz.a[k]+0.5):k;	if(z<0)z=0;	if(z>=nz)z=nz-1;
		d.a[i+n*(j+m*k)] = a[x+nx*(y+ny*z)];
	}
	if(m==1)	{	d.ny=d.nz;	d.nz=1;	}// "squeeze" dimensions
	if(n==1)	{	d.nx=d.ny;	d.ny=d.nz;	d.nz=1;	d.NewId();}
	return d;
}
//-----------------------------------------------------------------------------
mglData mglData::SubData(int xx,int yy,int zz) const
{
	mglData x,y,z;
	x.a[0]=xx;	y.a[0]=yy;	z.a[0]=zz;
	return SubData(x,y,z);
}
//-----------------------------------------------------------------------------
mglData mglData::Column(const char *eq)
{
	mglFormula f(eq);
	mglData d;
	d.Create(ny,nz);
	mreal var[MGL_VS];
	memset(var,0,('z'-'a')*sizeof(mreal));
	register long i,j;
	for(i=0;i<ny*nz;i++)
	{
		for(j=0;j<nx;j++)
			if(id[j]>='a' && id[j]<='z')
				var[id[j]-'a'] = a[j+nx*i];
		d.a[i] = f.Calc(var);
	}
	return d;
}
//-----------------------------------------------------------------------------
void mglData::SetColumnId(const char *ids)
{
	NewId();	// clearing + be sure about correct length
	if(ids)	for(long i=0;i<nx && ids[i]!=0;i++)	id[i]=ids[i];
}
//-----------------------------------------------------------------------------
void mglData::Save(const char *fname,int ns) const
{
	FILE *fp;
	fp = fopen(fname,"w");
	if(ns<0 || (ns>=nz && nz>1))	for(long k=0;k<nz;k++)
	{	// ñîõðàíÿåì âñå äàííûå
		for(long i=0;i<ny;i++)
		{
			for(long j=0;j<nx;j++)
				fprintf(fp,"%g\t",a[j+nx*(i+ny*k)]);
			fprintf(fp,"\n");
		}
		fprintf(fp,"\n");
	}
	else
	{	// ñîõðàíÿåì òîëüêî ñðåç
		if(nz>1)		// äëÿ 3D -- ïëîñêîñòü
		{
			for(long i=0;i<ny;i++)
			{
				for(long j=0;j<nx;j++)
					fprintf(fp,"%g\t",a[j+nx*(i+ny*ns)]);
				fprintf(fp,"\n");
			}
		}
		else if(ns<ny)	// äëÿ 2D -- ëèíèÿ
		{
			for(long j=0;j<nx;j++)
				fprintf(fp,"%g\t",a[j+nx*ns]);
		}
	}
	fclose(fp);
}
//-----------------------------------------------------------------------------
char *mgl_read_gz(gzFile fp)
{
	long size=1024,n=0,m;
	char *buf=(char*)malloc(size);
	while((m=gzread(fp,buf+size*n,size))>0)
	{
		if(m<size)	{	buf[size*n+m]=0;	break;	}
		n++;		buf=(char*)realloc(buf,size*(n+1));
	}
	return buf;
}
//-----------------------------------------------------------------------------
bool mglData::Read(const char *fname)
{
	long l=1,m=1,k=1;
	long nb,i;
	gzFile fp = gzopen(fname,"r");
	if(!fp)
	{
		if(!a)	Create(1,1,1);
		return	false;
	}
	char *buf = mgl_read_gz(fp);
	nb = strlen(buf);	gzclose(fp);

	bool first=false,com=false;
	register char ch;
	for(i=nb-1;i>=0;i--)	if(buf[i]>' ')	break;
	buf[i+1]=0;	nb = i;		// remove tailing spaces
	for(i=0;i<nb-1 && !isn(buf[i]);i++)	// determine nx
	{
		if(buf[i]=='#')		while(!isn(buf[i]) && i<nb)	i++;
		ch = buf[i];
		if(ch>' ' && !first)	first=true;
		if(first && (ch==' ' || ch=='\t') && buf[i+1]>' ') k++;
	}
	first = false;
	for(i=0;i<nb-1;i++)					// determine ny
	{
		ch = buf[i];
		if(ch=='#')	while(!isn(buf[i]) && i<nb)	i++;
		if(isn(ch))
		{
			if(isn(buf[i+1]))	{first=true;	break;	}
			m++;
		}
		if(ch=='\f')	break;
	}
	if(first)	for(i=0;i<nb-1;i++)		// determine nz
	{
		ch = buf[i];
		if(ch=='#')	com = true;	// comment
		if(isn(ch))
		{
			if(com)	{	com=false;	continue;	}
			if(isn(buf[i+1]))	l++;
		}
	}
	else	for(i=0;i<nb-1;i++)	if(buf[i]=='\f')	l++;
	free(buf);
	return Read(fname,k,m,l);
}
//-----------------------------------------------------------------------------
void mglData::Create(int mx,int my,int mz)
{
	nx = mx>0 ? mx:1;	ny = my>0 ? my:1;	nz = mz>0 ? mz:1;
	if(a)	{	delete []a;	delete []id;	}
	a = new mreal[nx*ny*nz];
	id = new char[nx];
	memset(a,0,nx*ny*nz*sizeof(mreal));
	memset(id,0,nx*sizeof(char));
}
//-----------------------------------------------------------------------------
bool mglData::Read(const char *fname,int mx,int my,int mz)
{
	if(mx<=0 || my<=0 || mz<=0)	return false;
	gzFile fp = gzopen(fname,"r");
	if(!fp)	return false;
	Create(mx,my,mz);
	char *buf = mgl_read_gz(fp);
	long nb = strlen(buf);	gzclose(fp);

	register long i=0, j=0, k=0;
	while(j<nb)
	{
		while(buf[j]<=' ' && j<nb)	j++;
		while(buf[j]=='#')		// skip comment
		{
			if(i>0 || buf[j+1]!='#')	// this is columns id
				while(!isn(buf[j]) && j<nb)	j++;
			else
			{
				while(!isn(buf[j]) && j<nb)
				{
					if(buf[j]>='a' && buf[j]<='z')
						id[k++] = buf[j];
					j++;
				}
			}
//			while(buf[j]!='\n' && j<nb)	j++;
			while(buf[j]<=' ' && j<nb)	j++;
		}
		char *s=buf+j;
		while(buf[j]>' ' && j<nb)	j++;
		buf[j]=0;
		a[i] = atof(s);
		i++;	if(i>=nx*ny*nz)	break;
	}
	free(buf);
	return true;
}
//-----------------------------------------------------------------------------
bool mglData::ReadMat(const char *fname,int dim)
{
	if(dim<=0 || dim>3)	return false;
	gzFile fp = gzopen(fname,"r");
	if(!fp)	return false;
	nx = ny = nz = 1;	NewId();
	char *buf = mgl_read_gz(fp);
	long nb = strlen(buf);	gzclose(fp);

	register long i=0,j=0;
	while(j<nb)
	{
		if(buf[j]=='#')	while(!isn(buf[j]))	j++;	// skip comment
		while(buf[j]<=' ' && j<nb)	j++;
		break;
	}
	if(dim==1)
	{
		sscanf(buf+j,"%ld",&nx);
		while(buf[j]>' ')	j++;
	}
	else if(dim==2)
	{
		sscanf(buf+j,"%ld%ld",&nx,&ny);
		while(buf[j]>' ' && j<nb)	j++;
		while(buf[j]<=' ' && j<nb)	j++;
		while(buf[j]>' ' && j<nb)	j++;
	}
	else if(dim==3)
	{
		sscanf(buf+j,"%ld%ld%ld",&nx,&ny,&nz);
		while(buf[j]>' ' && j<nb)	j++;
		while(buf[j]<=' ' && j<nb)	j++;
		while(buf[j]>' ' && j<nb)	j++;
		while(buf[j]<=' ' && j<nb)	j++;
		while(buf[j]>' ' && j<nb)	j++;
	}
	Create(nx,ny,nz);
	while(j<nb)
	{
		while(buf[j]<=' ' && j<nb)	j++;
		while(buf[j]=='#')		// skip comment
		{
			while(!isn(buf[j]) && j<nb)	j++;
			while(buf[j]<=' ' && j<nb)	j++;
		}
		a[i] = atof(buf+j);	i++;
		if(i>=nx*ny*nz)	break;
		while(buf[j]>' ' && j<nb)	j++;
	}
	free(buf);
	return true;
}
//-----------------------------------------------------------------------------
mreal mglData::v(int i,int j,int k) const
{
	bool not_ok = i<0 || i>=nx || j<0 || j>=ny || k<0 || k>=nz;
	if(not_ok)	return 0;
	return a[i+nx*(j+ny*k)];
}
//-----------------------------------------------------------------------------
mglData mglData::Resize(int mx, int my, int mz, mreal x1, mreal x2,
	mreal y1, mreal y2, mreal z1, mreal z2) const
{
	register long i,j,k;
	mglData d;
	mx = mx<1 ? 1:mx;	my = my<1 ? 1:my;	mz = mz<1 ? 1:mz;
	d.Create(mx,my,mz);
	mreal dx, dy, dz;
	dx = mx>1 ? (x2-x1)/(mx-1):0;
	dy = my>1 ? (y2-y1)/(my-1):0;
	dz = mz>1 ? (z2-z1)/(mz-1):0;
	for(i=0;i<mx;i++)	for(j=0;j<my;j++)	for(k=0;k<mz;k++)
		d.a[i+mx*(j+my*k)] = Spline1(x1+i*dx, y1+j*dy, z1+k*dz);
	return d;
}
//-----------------------------------------------------------------------------
void mglData::operator*=(const mglData &d)
{
	register long i,j;
	if(d.nz==1 && d.ny==1 && nx==d.nx)
	{
		for(j=0;j<ny*nz;j++)	for(i=0;i<nx;i++)	a[i+nx*j] *= d.a[i];
	}
	else if(d.nz==1 && ny==d.ny && nx==d.nx)
	{
		for(j=0;j<nz;j++)	for(i=0;i<ny*nx;i++)	a[i+nx*ny*j] *= d.a[i];
	}
	else if(nz==d.nz && d.ny==ny && nx==d.nx)
	{
		for(i=0;i<ny*nz*nx;i++)		a[i] *= d.a[i];
	}
}
//-----------------------------------------------------------------------------
void mglData::operator/=(const mglData &d)
{
	register long i,j;
	if(d.nz==1 && d.ny==1 && nx==d.nx)
	{
		for(j=0;j<ny*nz;j++)	for(i=0;i<nx;i++)
			a[i+nx*j] = d.a[i] ? a[i+nx*j]/d.a[i] : 0;
	}
	else if(d.nz==1 && ny==d.ny && nx==d.nx)
	{
		for(j=0;j<nz;j++)	for(i=0;i<ny*nx;i++)
			a[i+nx*ny*j] = d.a[i] ? a[i+nx*ny*j]/d.a[i] : 0;
	}
	else if(nz==d.nz && d.ny==ny && nx==d.nx)
	{
		for(i=0;i<ny*nz*nx;i++)
			a[i] = d.a[i] ? a[i]/d.a[i] : 0;
	}
}
//-----------------------------------------------------------------------------
void mglData::operator+=(const mglData &d)
{
	register long i,j;
	if(d.nz==1 && d.ny==1 && nx==d.nx)
	{
		for(j=0;j<ny*nz;j++)	for(i=0;i<nx;i++)
			a[i+nx*j] += d.a[i];
	}
	else if(d.nz==1 && ny==d.ny && nx==d.nx)
	{
		for(j=0;j<nz;j++)	for(i=0;i<ny*nx;i++)
			a[i+nx*ny*j] += d.a[i];
	}
	else if(nz==d.nz && d.ny==ny && nx==d.nx)
	{
		for(i=0;i<ny*nz*nx;i++)
			a[i] += d.a[i];
	}
}
//-----------------------------------------------------------------------------
void mglData::operator-=(const mglData &d)
{
	register long i,j;
	if(d.nz==1 && d.ny==1 && nx==d.nx)
	{
		for(j=0;j<ny*nz;j++)	for(i=0;i<nx;i++)
			a[i+nx*j] -= d.a[i];
	}
	else if(d.nz==1 && ny==d.ny && nx==d.nx)
	{
		for(j=0;j<nz;j++)	for(i=0;i<ny*nx;i++)
			a[i+nx*ny*j] -= d.a[i];
	}
	else if(nz==d.nz && d.ny==ny && nx==d.nx)
	{
		for(i=0;i<ny*nz*nx;i++)
			a[i] -= d.a[i];
	}
}
//-----------------------------------------------------------------------------
void mglData::operator-=(mreal d)
{
	for(long i=0;i<ny*nz*nx;i++)	a[i] -= d;
}
//-----------------------------------------------------------------------------
void mglData::operator+=(mreal d)
{
	for(long i=0;i<ny*nz*nx;i++)	a[i] += d;
}
//-----------------------------------------------------------------------------
void mglData::operator*=(mreal d)
{
	for(long i=0;i<ny*nz*nx;i++)	a[i] *= d;
}
//-----------------------------------------------------------------------------
void mglData::operator/=(mreal d)
{
	for(long i=0;i<ny*nz*nx;i++)	a[i] = d ? a[i]/d : 0;
}
//-----------------------------------------------------------------------------
mreal mglData::Maximal() const
{
	register mreal m=-1e10;
	for(long i=0;i<nx*ny*nz;i++)
		m = m>a[i] ? m : a[i];
	return m;
}
//-----------------------------------------------------------------------------
mreal mglData::Minimal() const
{
	register mreal m=1e10;
	for(long i=0;i<nx*ny*nz;i++)
		m = m<a[i] ? m : a[i];
	return m;
}
//-----------------------------------------------------------------------------
mreal mglData::Maximal(int &im,int &jm,int &km) const
{
	register mreal m=-1e10;
	for(long i=0;i<nx*ny*nz;i++)
		if(!isnan(a[i]) && m<a[i])
		{	m=a[i];	im=i%nx;	jm=(i/nx)%ny;	km=i/(nx*ny);   }
	return m;
}
//-----------------------------------------------------------------------------
mreal mglData::Minimal(int &im,int &jm,int &km) const
{
	register mreal m=1e10;
	for(long i=0;i<nx*ny*nz;i++)
		if(!isnan(a[i]) && m>a[i])
		{	m=a[i];	im=i%nx;	jm=(i/nx)%ny;	km=i/(nx*ny);   }
	return m;
}
//-----------------------------------------------------------------------------
mreal mglData::Maximal(mreal &x,mreal &y,mreal &z) const
{
	int im=-1,jm=-1,km=-1;
	register long tm,i;
	mreal m=Maximal(im,jm,km);
	x=im;	y=jm;	z=km;

	if(nx>2)
	{
		if(im==0)	im=1;
		if(im==nx-1)im=nx-2;
		x = (a[im+1]+a[im-1]-2*a[im])==0 ? im : im+(a[im+1]-a[im-1])/(a[im+1]+a[im-1]-2*a[im])/2;
	}
	if(ny>2)
	{
		if(jm==0)	jm=1;
		if(jm==ny-1)jm=ny-2;
		i=nx;		tm = jm*nx;
		y = (a[tm+i]+a[tm-i]-2*a[tm])==0? jm : jm+(a[tm+i]-a[tm-i])/(a[tm+i]+a[tm-i]-2*a[tm])/2;
	}
	if(nz>2)
	{
		if(km==0)	km=1;
		if(km==nz-1)km=nz-2;
		i=nx*ny;	tm = km*i;
		z = (a[tm+i]+a[tm-i]-2*a[tm])==0? km : km+(a[tm+i]-a[tm-i])/(a[tm+i]+a[tm-i]-2*a[tm])/2;
	}
	return m;
}
//-----------------------------------------------------------------------------
mreal mglData::Minimal(mreal &x,mreal &y,mreal &z) const
{
	int im=-1,jm=-1,km=-1;
	register long tm,i;
	mreal m=Minimal(im,jm,km);
	x=im;	y=jm;	z=km;
	if(nx>2)
	{
		if(im==0)	im=1;
		if(im==nx-1)im=nx-2;
		x = im+(a[im+1]-a[im-1])/(a[im+1]+a[im-1]-2*a[im])/2;
	}
	if(ny>2)
	{
		if(jm==0)	jm=1;
		if(jm==ny-1)jm=ny-2;
		i=nx;		tm = jm*nx;
		y = jm+(a[tm+i]-a[tm-i])/(a[tm+i]+a[tm-i]-2*a[tm])/2;
	}
	if(nz>2)
	{
		if(km==0)	km=1;
		if(km==nz-1)km=nz-2;
		i=nx*ny;	tm = km*i;
		z = km+(a[tm+i]-a[tm-i])/(a[tm+i]+a[tm-i]-2*a[tm])/2;
	}
	return m;
}
//-----------------------------------------------------------------------------
void mglData::Modify(const char *eq,int dim)
{
	long i,j,k;
	mreal y,z,dx=nx>1?1/(nx-1.):0,dy=ny>1?1/(ny-1.):0, *aa;
	mglFormula eqs(eq);
	if(dim<0)	dim=0;
	if(nz>1)	// 3D array
	{
		for(k=dim;k<nz;k++)
		{
			z = (nz>dim+1) ? (k-dim)/(nz-dim-1.) : 0;
			aa = a+nx*ny*k;
//#pragma omp parallel for
			for(i=0;i<nx*ny;i++)
				aa[i] = eqs.Calc(dx*(i%nx),dy*(i/nx),z,aa[i]);
		}
	}
	else		// 2D or 1D array
	{
		if(ny==1)	dim = 0;
		dy = ny>dim+1 ? 1/(ny-dim-1.) : 0;
		for(j=dim;j<ny;j++)
		{
			y = dy*(j-dim);		aa = a+nx*j;
//#pragma omp parallel for
			for(i=0;i<nx;i++)	aa[i] = eqs.Calc(dx*i,y,0,aa[i]);
		}
	}
}
//-----------------------------------------------------------------------------
void mglData::Fill(mreal x1,mreal x2,char dir)
{
	long i,j,k;
	register mreal x;
	if(isnan(x2))	x2=x1;
	if(dir<'x' || dir>'z')	dir='x';
	for(k=0;k<nz;k++)	for(j=0;j<ny;j++)	for(i=0;i<nx;i++)
	{
		x = nx>1 ? i/(nx-1.):0;
		if(dir=='y')	x = ny>1 ? j/(ny-1.):0;
		if(dir=='z')	x = nz>1 ? k/(nz-1.):0;
		a[i+nx*(j+ny*k)] = x1+(x2-x1)*x;
	}
}
//-----------------------------------------------------------------------------
void mglData::Norm(mreal v1,mreal v2,bool sym,int dim)
{
	long i,s,nn=nx*ny*nz;
	mreal a1=1e20,a2=-1e20,v;
	if(nz>1)	s = dim*nx*ny;
	else		s = dim*ny;
	for(i=s;i<nn;i++)	// determines borders of existing data
	{
		if(isnan(a[i]))	continue;
		a1 = (a1<a[i] ? a1 : a[i]);	a2 = (a2>a[i] ? a2 : a[i]);
	}
	if(a1==a2)  {  if(a1!=0)	a1=0.;  else a2=1;  }
	if(v1>v2)	{	v=v1;	v1=v2;	v2=v;	}	// swap if uncorrect
	if(sym)				// use symmetric
	{
		v2 = -v1>v2 ? -v1:v2;	v1 = -v2;
		a2 = -a1>a2 ? -a1:a2;	a1 = -a2;
	}
	for(i=s;i<nn;i++)	// normalize
	{
		a[i] = v1 + (v2-v1)*(a[i]-a1)/(a2-a1);
	}

}
//-----------------------------------------------------------------------------
void mglData::Squeeze(int rx,int ry,int rz,bool smooth)
{
	long kx,ky,kz,i,j,k;
	mreal *b;

	// simple checking
	if(rx>=nx)	rx=nx-1;	if(rx<1)	rx=1;
	if(ry>=ny)	ry=ny-1;	if(ry<1)	ry=1;
	if(rz>=nz)	rz=nz-1;	if(rz<1)	rz=1;
	// new sizes
	kx = 1+(nx-1)/rx;	ky = 1+(ny-1)/ry;	kz = 1+(nz-1)/rz;
	b = new mreal[kx*ky*kz];
	if(!smooth)	for(i=0;i<kx;i++)  for(j=0;j<ky;j++)  for(k=0;k<kz;k++)
		b[i+kx*(j+ky*k)] = a[i*rx+nx*(j*ry+ny*rz*k)];
	else		for(i=0;i<kx;i++)  for(j=0;j<ky;j++)  for(k=0;k<kz;k++)
	{
		long dx,dy,dz,i1,j1,k1;
		dx = (i+1)*rx<=nx ? rx : nx-i*rx;
		dy = (j+1)*ry<=ny ? ry : ny-j*ry;
		dz = (k+1)*rz<=nz ? rz : nz-k*rz;
		mreal s = 0;
		for(i1=i*rx;i1<i*rx+dx;i1++)	for(j1=j*ry;j1<j*ry+dz;j1++)	for(k1=k*rz;k1<k*rz+dz;k1++)
			s += a[i1+nx*(j1+ny*k1)];
		b[i+kx*(j+ky*k)] = s/dx*dy*dz;
	}
	delete []a;	a=b;
	nx = kx;  ny = ky;  nz = kz;	NewId();
}
//-----------------------------------------------------------------------------
mglData mglData::Combine(const mglData &b) const
{
	mglData d;
	d.Create(1,1,1);
	if(nz>1 || (ny>1 && b.ny>1) || b.nz>1)	return d;
	long n1=ny,n2=b.nx;
	bool dim2=true;
	if(ny==1)	{	n1 = b.nx;	n2 = b.ny;	dim2 = false;	}
	d.Create(nx,n1,n2);
	register long i,j;
	if(dim2)	n1=nx*ny;	else	{	n1=nx;	n2=b.nx*b.ny;	}
	for(i=0;i<n1;i++)	for(j=0;j<n2;j++)	d.a[i+n1*j] = a[i]*b.a[j];
	return d;
}
//-----------------------------------------------------------------------------
void mglData::Extend(int n1, int n2)
{
	if(nz>2 || n1==0)	return;
	long mx,my,mz;
	mreal *b=0;
	register long i,j;
	if(n1>0) // extend to higher dimension(s)
	{
		n2 = n2>0 ? n2:1;
		mx = nx;	my = ny>1?ny:n1;	mz = ny>1 ? n1 : n2;
		b = new mreal[mx*my*mz];
		if(ny>1)	for(i=0;i<n1;i++)
			memcpy(b+i*nx*ny, a, nx*ny*sizeof(mreal));
		else		for(i=0;i<n1*n2;i++)
			memcpy(b+i*nx, a, nx*sizeof(mreal));
	}
	else
	{
		mx = -n1;	my = n2<0 ? -n2 : nx;	mz = n2<0 ? nx : ny;
		if(n2>0 && ny==1)	mz = n2;
		b = new mreal[mx*my*mz];
		if(n2<0)	for(i=0;i<mx*my;i++)	for(j=0;j<nx;j++)
			b[i+mx*my*j] = a[j];
		else		for(i=0;i<mx;i++)		for(j=0;j<nx*ny;j++)
			b[i+mx*j] = a[j];
		if(n2>0 && ny==1)	for(i=0;i<n2;i++)
			memcpy(b+i*mx*my, a, mx*my*sizeof(mreal));
	}
	if(b)	{	delete []a;	a=b;	nx=mx;	ny=my;	nz=mz;	NewId();	}
}
//-----------------------------------------------------------------------------
void mglData::Transpose(const char *dim)
{
	mreal *b=new mreal[nx*ny*nz];
	register long i,j,k,n;
	if(!strcmp(dim,"xyz"))	memcpy(b,a,nx*ny*nz*sizeof(mreal));
	else if(!strcmp(dim,"xzy") || !strcmp(dim,"zy"))
	{
		for(i=0;i<nx;i++)	for(j=0;j<ny;j++)	for(k=0;k<nz;k++)
			b[i+nx*(k+nz*j)] = a[i+nx*(j+ny*k)];
		n=nz;	nz=ny;	ny=n;
	}
	else if(!strcmp(dim,"yxz") || !strcmp(dim,"yx"))
	{
		for(i=0;i<nx;i++)	for(j=0;j<ny;j++)	for(k=0;k<nz;k++)
			b[j+ny*(i+nx*k)] = a[i+nx*(j+ny*k)];
		n=nx;	nx=ny;	ny=n;	NewId();
	}
	else if(!strcmp(dim,"yzx"))
	{
		for(i=0;i<nx;i++)	for(j=0;j<ny;j++)	for(k=0;k<nz;k++)
			b[j+ny*(k+nz*i)] = a[i+nx*(j+ny*k)];
		n=nx;	nx=ny;	ny=nz;	nz=n;	NewId();
	}
	else if(!strcmp(dim,"zxy"))
	{
		for(i=0;i<nx;i++)	for(j=0;j<ny;j++)	for(k=0;k<nz;k++)
			b[k+nz*(i+nx*j)] = a[i+nx*(j+ny*k)];
		n=nx;	nx=nz;	nz=ny;	ny=n;	NewId();
	}
	else if(!strcmp(dim,"zyx") || !strcmp(dim,"zx"))
	{
		for(i=0;i<nx;i++)	for(j=0;j<ny;j++)	for(k=0;k<nz;k++)
			b[k+nz*(j+ny*i)] = a[i+nx*(j+ny*k)];
		n=nz;	nz=nx;	nx=n;	NewId();
	}
	delete []a;		a = b;
}
//-----------------------------------------------------------------------------
void mglData::Modify(const char *eq, const mglData &v, const mglData &w)
{
	if(v.nx*v.ny*v.nz!=nx*ny*nz || w.nx*w.ny*w.nz!=nx*ny*nz)
		return;
	long i,j,k,i0;
	mreal x,y,z,dx=nx>1?1/(nx-1.):0,dy=ny>1?1/(ny-1.):0,dz=nz>1?1/(nz-1.):0;
	mglFormula eqs(eq);
	for(k=0;k<nz;k++)	for(j=0;j<ny;j++)	for(i=0;i<nx;i++)
	{
		x = dx*i;	y = dy*j;	z = dz*k;
		i0 = i+nx*(j+ny*k);
		a[i0] = eqs.Calc(x,y,z,a[i0],v.a[i0],w.a[i0]);
	}
}
//-----------------------------------------------------------------------------
void mglData::Modify(const char *eq, const mglData &v)
{
	if(v.nx*v.ny*v.nz!=nx*ny*nz)	return;
	long i,j,k,i0;
	mreal x,y,z,dx=nx>1?1/(nx-1.):0,dy=ny>1?1/(ny-1.):0,dz=nz>1?1/(nz-1.):0;
	mglFormula eqs(eq);
	for(k=0;k<nz;k++)	for(j=0;j<ny;j++)	for(i=0;i<nx;i++)
	{
		x = dx*i;	y = dy*j;	z = dz*k;
		i0 = i+nx*(j+ny*k);
		a[i0] = eqs.Calc(x,y,z,a[i0],v.a[i0],0);
	}
}
//-----------------------------------------------------------------------------
void mglData::Fill(const char *eq, mglPoint r1, mglPoint r2, const mglData *v, const mglData *w)
{
	if(v && v->nx*v->ny*v->nz!=nx*ny*nz)	return;
	if(w && w->nx*w->ny*w->nz!=nx*ny*nz)	return;
	long i,j,k,i0;
	mreal x,y,z,dx=nx>1?(r2.x-r1.x)/(nx-1.):0;
	mreal dy=ny>1?(r2.y-r1.y)/(ny-1.):0;
	mreal dz=nz>1?(r2.z-r1.z)/(nz-1.):0;
	mglFormula eqs(eq);
	for(k=0;k<nz;k++)	for(j=0;j<ny;j++)	for(i=0;i<nx;i++)
	{
		x = r1.x+dx*i;	y = r1.y+dy*j;	z = r1.z+dz*k;
		i0 = i+nx*(j+ny*k);
		a[i0] = eqs.Calc(x,y,z,a[i0], v?v->a[i0]:0, w?w->a[i0]:0);
	}
}
//-----------------------------------------------------------------------------
void mglData::ReadHDF4(const char *fname,const char *data)
{
#ifdef HAVE_HDF4
	int32 sd = SDstart(fname,DFACC_READ), nn, i;
	if(sd==-1)	return;	// is not a HDF4 file
	char name[64];
	SDfileinfo(sd,&nn,&i);
	for(i=0;i<nn;i++)
	{
		int32 sds, rank, dims[32], type, attr, in[2]={0,0};
		sds = SDselect(sd,i);
		SDgetinfo(sds,name,&rank,dims,&type,&attr);
		if(!strcmp(name,data))	// as I understand there are possible many datas with the same name
		{
			if(rank==1)			Create(dims[0]);
			else if(rank==2)	Create(dims[1],dims[0]);
			else if(rank==3)	Create(dims[3],dims[1],dims[0]);
			else	continue;
			if(type==DFNT_FLOAT32)
			{
				float *b = new float[nx*ny*nz];
				SDreaddata(sds,in,0,dims,b);
				for(long j=0;j<nx*ny*nz;j++)	a[j]=b[j];
				delete []b;
			}
			if(type==DFNT_FLOAT64)
			{
				double *b = new double[nx*ny*nz];
				SDreaddata(sds,in,0,dims,b);
				for(long j=0;j<nx*ny*nz;j++)	a[j]=b[j];
				delete []b;
			}
		}
		SDendaccess(sds);
	}
	SDend(sd);
#endif
}
//-----------------------------------------------------------------------------
#ifdef HAVE_HDF5
void mglData::SaveHDF(const char *fname,const char *data,bool rewrite) const
{
	hid_t hf,hd,hs;
	hsize_t dims[3];
	long rank = 3, res;
#ifndef H5_USE_16_API
	H5Eset_auto(H5E_DEFAULT,0,0);
#else
	H5Eset_auto(0,0);
#endif
	res=H5Fis_hdf5(fname);
	if(res>0 && !rewrite)	hf = H5Fopen(fname, H5F_ACC_RDWR, H5P_DEFAULT);
	else	hf = H5Fcreate(fname, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
	if(hf<0)	return;
	if(nz==1 && ny == 1)	{	rank = 1;	dims[0] = nx;	}
	else if(nz==1)	{	rank = 2;	dims[0] = ny;	dims[1] = nx;	}
	else	{	rank = 3;	dims[0] = nz;	dims[1] = ny;	dims[2] = nx;	}
	hs = H5Screate_simple(rank, dims, 0);
#if(MGL_USE_DOUBLE==1)
#ifndef H5_USE_16_API
	hd = H5Dcreate(hf, data, H5T_NATIVE_DOUBLE, hs, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
#else  /* ! HAVE_HDF5_18 */
	hd = H5Dcreate(hf, data, H5T_NATIVE_DOUBLE, hs, H5P_DEFAULT);
#endif /* HAVE_HDF5_18 */
	H5Dwrite(hd, H5T_NATIVE_DOUBLE, hs, hs, H5P_DEFAULT, a);
#else
#ifndef H5_USE_16_API
	hd = H5Dcreate(hf, data, H5T_NATIVE_FLOAT, hs, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
#else  /* ! HAVE_HDF5_18 */
	hd = H5Dcreate(hf, data, H5T_NATIVE_FLOAT, hs, H5P_DEFAULT);
#endif /* HAVE_HDF5_18 */
	H5Dwrite(hd, H5T_NATIVE_FLOAT, hs, hs, H5P_DEFAULT, a);
#endif
	H5Dclose(hd);	H5Sclose(hs);	H5Fclose(hf);
}
//-----------------------------------------------------------------------------
void mglData::ReadHDF(const char *fname,const char *data)
{
	hid_t hf,hd,hs;
	hsize_t dims[3];
	long rank, res = H5Fis_hdf5(fname);
	if(res<=0)	{	ReadHDF4(fname,data);	return;	}
	hf = H5Fopen(fname, H5F_ACC_RDONLY, H5P_DEFAULT);	if(hf<0)	return;
#ifndef H5_USE_16_API
	hd = H5Dopen(hf,data,H5P_DEFAULT);
#else
	hd = H5Dopen(hf,data);
#endif
	if(hd<0)	return;
	hs = H5Dget_space(hd);
	rank = H5Sget_simple_extent_ndims(hs);
	if(rank>0 && rank<=3)
	{
		H5Sget_simple_extent_dims(hs,dims,0);
		nx = ny = nz = 1;
		switch(rank)
		{
		case 1:	nx = dims[0];	break;
		case 2:	nx = dims[1];	ny = dims[0];	break;
		case 3:	nx = dims[2];	ny = dims[1];	nz = dims[0];	break;
		}
		delete []a;		a = new mreal[nx*ny*nz];	NewId();
#if(MGL_USE_DOUBLE==1)
		H5Dread(hd, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, a);
#else
		H5Dread(hd, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, a);
#endif
	}
	H5Dclose(hd);	H5Sclose(hs);	H5Fclose(hf);
}
#else
void mglData::SaveHDF(const char *,const char *,bool ) const {}
void mglData::ReadHDF(const char *,const char *)	{}
#endif
//-----------------------------------------------------------------------------
bool mgl_add_file(long &kx,long &ky, long &kz, mreal *&b, mglData &d,bool as_slice)
{
	if(as_slice && d.nz==1)
	{
		if(kx==d.nx && d.ny==1)
		{
			b = (mreal *)realloc(b,kx*(ky+1)*sizeof(mreal));
			memcpy(b+kx*ky,d.a,kx*sizeof(mreal));		ky++;
		}
		else if(kx==d.nx && ky==d.ny)
		{
			b = (mreal *)realloc(b,kx*ky*(kz+1)*sizeof(mreal));
			memcpy(b+kx*ky*kz,d.a,kx*ky*sizeof(mreal));	kz++;
		}
		else	return false;
	}
	else
	{
		if(d.ny*d.nz==1 && ky*kz==1)
		{
			b = (mreal *)realloc(b,(kx+d.nx)*sizeof(mreal));
			memcpy(b+kx,d.a,d.nx*sizeof(mreal));	kx+=d.nx;
		}
		else if(kx==d.nx && kz==1 && d.nz==1)
		{
			b = (mreal *)realloc(b,kx*(ky+d.ny)*sizeof(mreal));
			memcpy(b+kx*ky,d.a,kx*d.ny*sizeof(mreal));	ky+=d.ny;
		}
		else if(kx==d.nx && ky==d.ny)
		{
			b = (mreal *)realloc(b,kx*kx*(kz+d.nz)*sizeof(mreal));
			memcpy(b+kx*ky*kz,d.a,kx*ky*d.nz*sizeof(mreal));	kz+=d.nz;
		}
		else	return false;
	}
	return true;
}
//-----------------------------------------------------------------------------
bool mglData::ReadRange(const char *templ, mreal from, mreal to, mreal step, bool as_slice)
{
	mglData d;
	mreal t = from, *b;
	long kx,ky,kz;
	char *fname = new char[strlen(templ)+20];

	//read first file
	do{	sprintf(fname,templ,t);	t+= step;	} while(!d.Read(fname) && t<=to);

	if(t>to)	return false;
	kx = d.nx;	ky = d.ny;	kz = d.nz;
	b = (mreal *)malloc(kx*ky*kz*sizeof(mreal));
	memcpy(b,d.a,kx*ky*kz*sizeof(mreal));

	// read other files
	for(;t<=to;t+=step)
	{
		sprintf(fname,templ,t);
		if(d.Read(fname))
			if(!mgl_add_file(kx,ky,kz,b,d,as_slice))
				return false;
	}
	Set(b,kx,ky,kz);
	delete []fname;		free(b);
	return true;
}
//-----------------------------------------------------------------------------
bool mglData::ReadAll(const char *templ, bool as_slice)
{
#ifndef WIN32
	mglData d;
	glob_t res;
	unsigned long i;
	mreal *b;
	long kx,ky,kz;
	char *fname = new char[256];
	glob (templ, GLOB_TILDE, NULL, &res);

	//read first file
	for(i=0;i<res.gl_pathc;i++)
		if(d.Read(res.gl_pathv[i]))	break;

	if(i>=res.gl_pathc)	{	delete []fname;	return false;	}
	kx = d.nx;	ky = d.ny;	kz = d.nz;
	b = (mreal *)malloc(kx*ky*kz*sizeof(mreal));
	memcpy(b,d.a,kx*ky*kz*sizeof(mreal));

	for(;i<res.gl_pathc;i++)
	{
		if(d.Read(res.gl_pathv[i]))
			if(!mgl_add_file(kx,ky,kz,b,d,as_slice))
			{	delete []fname;		return false;	}
	}
	Set(b,kx,ky,kz);

	globfree (&res);
	delete []fname;		free(b);
	return true;
#else
	return false;
#endif
}
//-----------------------------------------------------------------------------
mglData operator*(const mglData &b, const mglData &d)
{	mglData a(b);	a*=d;	return a;	}
mglData operator*(mreal b, const mglData &d)
{	mglData a(d);	a*=b;	return a;	}
mglData operator*(const mglData &d, mreal b)
{	mglData a(d);	a*=b;	return a;	}
mglData operator-(const mglData &b, const mglData &d)
{	mglData a(b);	a-=d;	return a;	}
mglData operator-(mreal b, const mglData &d)
{	mglData a(d);	a-=b;	return a;	}
mglData operator-(const mglData &d, mreal b)
{	mglData a(d);	a-=b;	return a;	}
mglData operator+(const mglData &b, const mglData &d)
{	mglData a(b);	a+=d;	return a;	}
mglData operator+(mreal b, const mglData &d)
{	mglData a(d);	a+=b;	return a;	}
mglData operator+(const mglData &d, mreal b)
{	mglData a(d);	a+=b;	return a;	}
mglData operator/(const mglData &b, const mglData &d)
{	mglData a(b);	a/=d;	return a;	}
mglData operator/(const mglData &d, mreal b)
{	mglData a(d);	a/=b;	return a;	}
void mglData::operator=(mreal v)
{	for(long i=0;i<nx*ny*nz;i++)	a[i]=v;	}
//-----------------------------------------------------------------------------
void mglData::Set(const std::vector<int> &d)
{	if(d.size()<1)	return;
	Create(d.size());	for(long i=0;i<nx;i++)	a[i] = d[i];	}
void mglData::Set(const std::vector<float> &d)
{	if(d.size()<1)	return;
	Create(d.size());	for(long i=0;i<nx;i++)	a[i] = d[i];	}
void mglData::Set(const std::vector<double> &d)
{	if(d.size()<1)	return;
	Create(d.size());	for(long i=0;i<nx;i++)	a[i] = d[i];	}
//-----------------------------------------------------------------------------
void mglData::NewId()
{	delete []id;	id=new char[nx];	memset(id,0,nx*sizeof(char));	}
//-----------------------------------------------------------------------------
