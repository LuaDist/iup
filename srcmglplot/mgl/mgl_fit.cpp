/***************************************************************************
 * mgl_fit.cpp is part of Math Graphic Library
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
#ifndef NO_GSL
#include <gsl/gsl_multifit_nlin.h>
#include <gsl/gsl_blas.h>
#endif
#include <ctype.h>
#include "mgl/mgl_eval.h"
#include "mgl/mgl.h"
#include "mgl/mgl_c.h"
#include "mgl/mgl_f.h"
//-----------------------------------------------------------------------------
/// Structure for keeping data and precompiled fitted formula
struct mglFitData
{
	long n;				///< number of points
	mreal *x;			///< x values
	mreal *y;			///< y values
	mreal *z;			///< z values
	mreal *a;			///< function values
	mreal *s;			///< value dispersions (sigma)
	mglFormula *eq;		///< approximation formula
	int m;				///< number of variables
	const char *var;	///< variables for fitting
};
//-----------------------------------------------------------------------------
#ifndef NO_GSL
int	mgl_fit__f (const gsl_vector *x, void *data, gsl_vector *f)
{
	mglFitData *fd = (mglFitData *)data;
	register long i;
	mreal val[MGL_VS];
	for(i=0;i<fd->m;i++)	val[fd->var[i]-'a'] = gsl_vector_get(x,i);
	for(i=0;i<fd->n;i++)
	{
		val['x'-'a'] = fd->x[i];
		val['y'-'a'] = fd->y ? fd->y[i] : 0;
		val['z'-'a'] = fd->z ? fd->z[i] : 0;
		gsl_vector_set (f, i, (fd->eq->Calc(val) - fd->a[i])/fd->s[i]);
	}
	return GSL_SUCCESS;
}
//-----------------------------------------------------------------------------
int mgl_fit__df (const gsl_vector * x, void *data, gsl_matrix * J)
{
	mglFitData *fd = (mglFitData *)data;
	register long i,j;
	mreal val[MGL_VS],s;
	for(i=0;i<fd->m;i++)	val[fd->var[i]-'a'] = gsl_vector_get(x,i);
	for(i=0;i<fd->n;i++)
	{
		val['x'-'a'] = fd->x[i];	s = fd->s[i];
		val['y'-'a'] = fd->y ? fd->y[i] : 0;
		val['z'-'a'] = fd->z ? fd->z[i] : 0;
		for(j=0;j<fd->m;j++)
			gsl_matrix_set (J, i, j, fd->eq->CalcD(val, fd->var[j])/s);
	}
	return GSL_SUCCESS;
}
//-----------------------------------------------------------------------------
int mgl_fit__fdf (const gsl_vector * x, void *data, gsl_vector * f, gsl_matrix * J)
{
	mgl_fit__f(x, data, f);
	mgl_fit__df(x, data, J);
	return GSL_SUCCESS;
}
#endif
//-----------------------------------------------------------------------------
/// GSL based fitting procedure for formula/arguments specified by string
mreal mgl_fit_base(mglFitData *fd, mreal *ini)
{
#ifndef NO_GSL
	register long i,m=fd->m,n=fd->n,iter=0;
	if(n<1 || fd==0 || ini==0)	return -1;
	// setup data
	double *x_init = new double[fd->m];
	for(i=0;i<m;i++)	x_init[i] = ini[i];
	// setup fitting
	gsl_matrix *covar = gsl_matrix_alloc(m, m);
	gsl_vector_view vx = gsl_vector_view_array(x_init, m);
	const gsl_multifit_fdfsolver_type *T = gsl_multifit_fdfsolver_lmsder;
	gsl_multifit_fdfsolver *s = gsl_multifit_fdfsolver_alloc(T, n, m);
	gsl_multifit_function_fdf f;
	f.f = mgl_fit__f;		f.df = mgl_fit__df;
	f.fdf = mgl_fit__fdf;	f.n = n;	f.p = m;
	f.params = fd;
	gsl_multifit_fdfsolver_set(s, &f, &vx.vector);
	int status;	// start fitting
	do
	{
		iter++;
		status = gsl_multifit_fdfsolver_iterate(s);
		if ( status )	break;
		status = gsl_multifit_test_delta (s->dx, s->x, 1e-4, 1e-4 );
	}
	while ( status == GSL_CONTINUE && iter < 500 );
	gsl_multifit_covar (s->J, 0.0, covar );
	mreal res = gsl_blas_dnrm2(s->f);
	for(i=0;i<m;i++)	ini[i] = gsl_vector_get(s->x, i);
	// free memory
	gsl_multifit_fdfsolver_free (s);
	gsl_matrix_free (covar);
	delete []x_init;
	return res;
#else
	return 0.0;
#endif
}
//-----------------------------------------------------------------------------
mreal mglGraph::Fit(mglData &fit, const mglData &y, const char *eq, const char *var, mreal *ini, bool print)
{
	mglData x(y.nx);	x.Fill(Min.x, Max.x);
	mglData s(y);		s.Fill(1,1);
	return FitS(fit,x,y,s,eq,var,ini,print);
}
//-----------------------------------------------------------------------------
mreal mglGraph::Fit2(mglData &fit, const mglData &z, const char *eq, const char *var, mreal *ini, bool print)
{
	mglData x(z.nx);	x.Fill(Min.x, Max.x);
	mglData y(z.ny);	y.Fill(Min.y, Max.y);
	mglData s(z);		s.Fill(1,1);
	return FitS(fit,x,y,z,s,eq,var,ini,print);
}
//-----------------------------------------------------------------------------
mreal mglGraph::Fit3(mglData &fit, const mglData &a, const char *eq, const char *var, mreal *ini, bool print)
{
	mglData x(a.nx);	x.Fill(Min.x, Max.x);
	mglData y(a.ny);	y.Fill(Min.y, Max.y);
	mglData z(a.nz);	z.Fill(Min.z, Max.z);
	mglData s(a);		s.Fill(1,1);
	return FitS(fit,x,y,z,a,s,eq,var,ini,print);
}
//-----------------------------------------------------------------------------
mreal mglGraph::Fit(mglData &fit, const mglData &x, const mglData &y, const char *eq, const char *var, mreal *ini, bool print)
{
	mglData s(y);	s.Fill(1,1);
	return FitS(fit,x,y,s,eq,var,ini,print);
}
//-----------------------------------------------------------------------------
mreal mglGraph::Fit(mglData &fit, const mglData &x, const mglData &y, const mglData &z, const char *eq, const char *var, mreal *ini, bool print)
{
	mglData s(z);	s.Fill(1,1);
	return FitS(fit,x,y,z,s,eq,var,ini,print);
}
//-----------------------------------------------------------------------------
mreal mglGraph::Fit(mglData &fit, const mglData &x, const mglData &y, const mglData &z, const mglData &a, const char *eq, const char *var, mreal *ini, bool print)
{
	mglData s(a);	s.Fill(1,1);
	return FitS(fit,x,y,z,a,s,eq,var,ini,print);
}
//-----------------------------------------------------------------------------
mreal mglGraph::FitS(mglData &fit, const mglData &y, const mglData &s, const char *eq, const char *var, mreal *ini, bool print)
{
	mglData x(y.nx);	x.Fill(Min.x, Max.x);
	return FitS(fit,x,y,s,eq,var,ini,print);
}

//-----------------------------------------------------------------------------
mreal mglGraph::FitS(mglData &fit, const mglData &x, const mglData &y, const mglData &s, const char *eq, const char *var, mreal *ini, bool print)
{
	if(x.nx!=y.nx)	{	SetWarn(mglWarnDim,"Fit[S]");	return -1;	}
	if(y.nx<2)		{	SetWarn(mglWarnLow,"Fit[S]");	return -1;	}
	if(s.nx*s.ny*s.nz != y.nx*y.ny*y.nz)
	{	SetWarn(mglWarnDim,"FitS");	return -1;	}
	mglFitData fd;
	fd.n = y.nx;	fd.x = x.a;	fd.y = 0;
	fd.z = 0;		fd.a = y.a;	fd.s = s.a;
	fd.var = var;	fd.m = strlen(var);
	fd.eq = new mglFormula(eq);
	mglData in(fd.m);
	if(fit.nx<FitPnts)	fit.Create(FitPnts, y.ny, y.nz);
	mreal val[MGL_VS],res=-1;
	register long j;
	for(long i=0;i<y.ny*y.nz && i<fit.nx*fit.ny;i++)
	{
		if(ini)	in.Set(ini,fd.m);	else in.Fill(0,0);
		fd.a = y.a+i*y.nx;		fd.x = x.a+(i%x.ny)*y.nx;
		fd.s = s.a+i*y.nx;
		res = mgl_fit_base(&fd,in.a);
		for(j=0;j<fd.m;j++)	val[var[j]-'a'] = in.a[j];
		for(j=0;j<fit.nx;j++)
		{
			val['x'-'a'] = Min.x+j*(Max.x-Min.x)/(fit.nx-1);
			fit.a[j+i*fit.nx] = fd.eq->Calc(val);
		}
		if(ini)	memcpy(ini,in.a,fd.m*sizeof(mreal));
	}
	PrepareFitEq(res,eq,var,ini,print);
	delete fd.eq;
	return res;
}
//-----------------------------------------------------------------------------
mreal mglGraph::FitS(mglData &fit, const mglData &xx, const mglData &yy, const mglData &z, const mglData &s, const char *eq, const char *var, mreal *ini, bool print)
{
	if(xx.nx!=z.nx)		{	SetWarn(mglWarnDim,"Fit[S]");	return -1;	}
	if(s.nx*s.ny*s.nz != z.nx*z.ny*z.nz)
	{	SetWarn(mglWarnDim,"FitS");	return -1;	}
	if(yy.nx!=z.ny && (xx.ny!=z.ny || yy.nx!=z.nx || yy.ny!=z.ny))
	{	SetWarn(mglWarnDim);	return -1;	}
	if(z.nx<2|| z.ny<2)	{	SetWarn(mglWarnLow,"Fit[S]");	return -1;	}

	mglData x(z.nx, z.ny), y(z.nx, z.ny);
	register long i,j;
	for(i=0;i<z.nx;i++)	for(j=0;j<z.ny;j++)	// создаем массив точек
	{
		x.a[i+z.nx*j] = GetX(xx,i,j,0);
		y.a[i+z.nx*j] = GetY(yy,i,j,0);
	}
	mglFitData fd;
	fd.n = y.nx;	fd.x = x.a;	fd.y = y.a;
	fd.z = 0;		fd.a = z.a;	fd.s = s.a;
	fd.var = var;	fd.m = strlen(var);
	fd.eq = new mglFormula(eq);
	mglData in(fd.m);
	if(fit.nx<FitPnts || fit.ny<FitPnts)	fit.Create(FitPnts, FitPnts, z.nz);
	mreal val[MGL_VS], res = -1;
	for(long i=0;i<z.nz && i<fit.nz;i++)
	{
		if(ini)	in.Set(ini,fd.m);	else in.Fill(0,0);
		fd.a = z.a+i*z.nx*z.ny;		fd.s = s.a+i*z.nx*z.ny;
		res = mgl_fit_base(&fd,in.a);
		for(j=0;j<fd.m;j++)	val[var[j]-'a'] = in.a[j];
		for(j=0;j<fit.nx*fit.ny;j++)
		{
			val['x'-'a'] = Min.x+(j%fit.nx)*(Max.x-Min.x)/(fit.nx-1);
			val['y'-'a'] = Min.y+(j/fit.nx)*(Max.y-Min.y)/(fit.ny-1);
			fit.a[j+i*fit.nx*fit.ny] = fd.eq->Calc(val);
		}
		if(ini)	memcpy(ini,in.a,fd.m*sizeof(mreal));
	}
	PrepareFitEq(res, eq,var,ini,print);
	delete fd.eq;
	return res;
}
//-----------------------------------------------------------------------------
mreal mglGraph::FitS(mglData &fit, const mglData &xx, const mglData &yy, const mglData &zz, const mglData &a, const mglData &s, const char *eq, const char *var, mreal *ini, bool print)
{
	register long i,j,k,i0;
	i = a.nx*a.ny*a.nz;
	if(a.nx<2 || a.ny<2 || a.nz<2)	{	SetWarn(mglWarnLow,"Fit[S]");	return -1;	}
	if(s.nx*s.ny*s.nz != i)			{	SetWarn(mglWarnDim,"FitS");	return -1;	}
	bool both = xx.nx*xx.ny*xx.nz==i && yy.nx*yy.ny*yy.nz==i && zz.nx*zz.ny*zz.nz==i;
	if(!(both || (xx.nx==a.nx && yy.nx==a.ny && zz.nx==a.nz)))
	{	SetWarn(mglWarnDim,"Fit[S]");	return -1;	}
	mglData x(a), y(a), z(a);
	for(i=0;i<a.nx;i++)	for(j=0;j<a.ny;j++)	for(k=0;k<a.nz;k++)	// создаем массив точек
	{
		i0 = i+z.nx*j+z.nx*z.ny*k;
		x.a[i0] = GetX(xx,i,j,k);
		y.a[i0] = GetY(yy,i,j,k);
		z.a[i0] = GetZ(zz,i,j,k);
	}
	mglFitData fd;
	fd.n = y.nx;	fd.x = x.a;	fd.y = y.a;
	fd.z = z.a;		fd.a = a.a;	fd.s = s.a;
	fd.var = var;	fd.m = strlen(var);
	fd.eq = new mglFormula(eq);
	mglData in(fd.m);
	if(fit.nx<FitPnts || fit.ny<FitPnts || fit.nz<FitPnts)	fit.Create(FitPnts, FitPnts, FitPnts);
	mreal val[MGL_VS], res = -1;

	if(ini)	in.Set(ini,fd.m);	else in.Fill(0,0);
	fd.a = a.a+i*a.nx*a.ny*a.nz;
	res = mgl_fit_base(&fd,in.a);
	for(j=0;j<fd.m;j++)	val[var[j]-'a'] = in.a[j];
	for(i=0;i<fit.nz;i++)	for(j=0;j<fit.nx*fit.ny;j++)
	{
		val['x'-'a'] = Min.x+(j%fit.nx)*(Max.x-Min.x)/(fit.nx-1);
		val['y'-'a'] = Min.y+(j/fit.nx)*(Max.y-Min.y)/(fit.ny-1);
		val['z'-'a'] = Min.z+i*(Max.y-Min.y)/(fit.nz-1);
		fit.a[j+fit.nx*fit.ny*i] = fd.eq->Calc(val);
	}
	if(ini)	memcpy(ini,in.a,fd.m*sizeof(mreal));

	PrepareFitEq(res, eq,var,ini,print);
	delete fd.eq;
	return res;
}
//-----------------------------------------------------------------------------
mreal mglGraph::Fit(mglData &fit, const mglData &y, const char *eq, const char *var, mglData &ini, bool print)
{
	if(ini.nx<int(strlen(var)))	{	SetWarn(mglWarnLow,"Fit");	return -1;	}
	return Fit(fit,y,eq,var,ini.a,print);
}
mreal mglGraph::Fit2(mglData &fit, const mglData &z, const char *eq, const char *var, mglData &ini, bool print)
{
	if(ini.nx<int(strlen(var)))	{	SetWarn(mglWarnLow,"Fit");	return -1;	}
	return Fit2(fit,z,eq,var,ini.a,print);
}
mreal mglGraph::Fit3(mglData &fit, const mglData &a, const char *eq, const char *var, mglData &ini, bool print)
{
	if(ini.nx<int(strlen(var)))	{	SetWarn(mglWarnLow,"Fit");	return -1;	}
	return Fit3(fit,a,eq,var,ini.a,print);
}
mreal mglGraph::Fit(mglData &fit, const mglData &x, const mglData &y, const char *eq, const char *var, mglData &ini, bool print)
{
	if(ini.nx<int(strlen(var)))	{	SetWarn(mglWarnLow,"Fit");	return -1;	}
	return Fit(fit,x,y,eq,var,ini.a,print);
}
mreal mglGraph::Fit(mglData &fit, const mglData &x, const mglData &y, const mglData &z, const char *eq, const char *var, mglData &ini, bool print)
{
	if(ini.nx<int(strlen(var)))	{	SetWarn(mglWarnLow,"Fit");	return -1;	}
	return Fit(fit,x,y,z,eq,var,ini.a,print);
}
mreal mglGraph::Fit(mglData &fit, const mglData &x, const mglData &y, const mglData &z, const mglData &a, const char *eq, const char *var, mglData &ini, bool print)
{
	if(ini.nx<int(strlen(var)))	{	SetWarn(mglWarnLow,"Fit");	return -1;	}
	return Fit(fit,x,y,z,a,eq,var,ini.a,print);
}
mreal mglGraph::FitS(mglData &fit, const mglData &y, const mglData &s, const char *eq, const char *var, mglData &ini, bool print)
{
	if(ini.nx<int(strlen(var)))	{	SetWarn(mglWarnLow,"Fit");	return -1;	}
	return FitS(fit,y,s,eq,var,ini.a,print);
}
mreal mglGraph::FitS(mglData &fit, const mglData &x, const mglData &y, const mglData &s, const char *eq, const char *var, mglData &ini, bool print)
{
	if(ini.nx<int(strlen(var)))	{	SetWarn(mglWarnLow,"Fit");	return -1;	}
	return FitS(fit,x,y,s,eq,var,ini.a,print);
}
mreal mglGraph::FitS(mglData &fit, const mglData &x, const mglData &y, const mglData &z, const mglData &s, const char *eq, const char *var, mglData &ini, bool print)
{
	if(ini.nx<int(strlen(var)))	{	SetWarn(mglWarnLow,"Fit");	return -1;	}
	return FitS(fit,x,y,z,s,eq,var,ini.a,print);
}
mreal mglGraph::FitS(mglData &fit, const mglData &x, const mglData &y, const mglData &z, const mglData &a, const mglData &s, const char *eq, const char *var, mglData &ini, bool print)
{
	if(ini.nx<int(strlen(var)))	{	SetWarn(mglWarnLow,"Fit");	return -1;	}
	return FitS(fit,x,y,z,a,s,eq,var,ini.a,print);
}
//-----------------------------------------------------------------------------
void mglGraph::PrepareFitEq(mreal chi, const char *eq, const char *var, mreal *par, bool print)
{
	if(print && Message)
	{
		sprintf(Message,"chi=%g,\t",chi);
		for(int i=0;i<int(strlen(var));i++)
		{
			sprintf(fit_res,"%c=%g,\t",var[i],par[i]);
			strcat(Message, fit_res);
		}
	}
	memset(fit_res, 0, 1024);	//fit_res[0] = 0;
	char buf[32]="";
	register long i,k,len=strlen(eq);
	for(i=k=0;i<len;i++)
	{
		const char *c = strchr(var,eq[i]);
		if(c && (i==0 || !isalnum(eq[i-1])) && (i==len-1 || !isalnum(eq[i+1])))
		{
			sprintf(buf,"%g",par[c-var]);
			strcat(fit_res+k, buf);	k+=strlen(buf);
		}
		else	{	fit_res[k] = eq[i];	k++;	}
	}
	fit_res[k]=0;
}
//-----------------------------------------------------------------------------
void mglGraph::PutsFit(mglPoint p, const char *pre, const char *font, mreal size)
{
	long n = strlen(fit_res)+(pre?strlen(pre):0)+1;
	char *buf = new char[n];
	if(pre)	sprintf(buf,"%s%s",pre,fit_res);
	else	strcpy(buf,fit_res);
	Puts(p,buf,font,size);
	delete []buf;
}
//-----------------------------------------------------------------------------
void mglGraph::Hist(mglData &res, const mglData &x, const mglData &a)
{
	int n1=res.nx, nn=a.nx*a.ny*a.nz;
	if(n1<FitPnts)	n1=FitPnts;
	res.Create(n1);
	if(nn!=x.nx*x.ny*x.nz)	{	SetWarn(mglWarnDim);	return;	}
	register int i,j1;
	for(i=0;i<nn;i++)
	{
		j1 = int(n1*(x.a[i]-Min.x)/(Max.x-Min.x));	if(j1<0 || j1>=n1)	continue;
		res.a[j1] += a.a[i];
	}
}
//-----------------------------------------------------------------------------
void mglGraph::Hist(mglData &res, const mglData &x, const mglData &y, const mglData &a)
{
	int n1=res.nx, n2=res.ny, nn=a.nx*a.ny*a.nz;
	if(n1<FitPnts) n1=FitPnts;	if(n2<FitPnts) n2=FitPnts;
	res.Create(n1,n2);
	if(nn!=x.nx*x.ny*x.nz || nn!=y.nx*y.ny*y.nz)
	{	SetWarn(mglWarnDim);	return;	}
	register int i,j1,j2;
	for(i=0;i<nn;i++)
	{
		j1 = int(n1*(x.a[i]-Min.x)/(Max.x-Min.x));	if(j1<0 || j1>=n1)	continue;
		j2 = int(n2*(y.a[i]-Min.y)/(Max.y-Min.y));	if(j2<0 || j2>=n2)	continue;
		res.a[j1+n1*j2] += a.a[i];
	}
}
//-----------------------------------------------------------------------------
void mglGraph::Hist(mglData &res, const mglData &x, const mglData &y, const mglData &z, const mglData &a)
{
	int n1=res.nx, n2=res.ny, n3=res.nz, nn=a.nx*a.ny*a.nz;
	if(n1<FitPnts) n1=FitPnts;	if(n2<FitPnts) n2=FitPnts;	if(n3<FitPnts) n3=FitPnts;
	res.Create(n1,n2,n3);
	if(nn!=x.nx*x.ny*x.nz || nn!=y.nx*y.ny*y.nz || nn!=z.nx*z.ny*z.nz)
	{	SetWarn(mglWarnDim);	return;	}
	register int i,j1,j2,j3;
	for(i=0;i<nn;i++)
	{
		j1 = int(n1*(x.a[i]-Min.x)/(Max.x-Min.x));	if(j1<0 || j1>=n1)	continue;
		j2 = int(n2*(y.a[i]-Min.y)/(Max.y-Min.y));	if(j2<0 || j2>=n2)	continue;
		j3 = int(n3*(z.a[i]-Min.z)/(Max.z-Min.z));	if(j3<0 || j3>=n3)	continue;
		res.a[j1+n1*(j2+n2*j3)] += a.a[i];
	}
}
//-----------------------------------------------------------------------------
void mgl_hist_x(HMGL gr, HMDT res, const HMDT x, const HMDT a)
{	gr->Hist(*res, *x, *a);	}
void mgl_hist_xy(HMGL gr, HMDT res, const HMDT x, const HMDT y, const HMDT a)
{	gr->Hist(*res, *x, *y, *a);	}
void mgl_hist_xyz(HMGL gr, HMDT res, const HMDT x, const HMDT y, const HMDT z, const HMDT a)
{	gr->Hist(*res, *x, *y, *z, *a);	}
void mgl_hist_x_(uintptr_t* gr, uintptr_t* res, uintptr_t* x, uintptr_t* a)
{	_GR_->Hist(_DM_(res), _D_(x), _D_(a));	}
void mgl_hist_xy_(uintptr_t* gr, uintptr_t* res, uintptr_t* x, uintptr_t* y, uintptr_t* a)
{	_GR_->Hist(_DM_(res), _D_(x), _D_(y), _D_(a));	}
void mgl_hist_xyz_(uintptr_t* gr, uintptr_t* res, uintptr_t* x, uintptr_t* y, uintptr_t* z, uintptr_t* a)
{	_GR_->Hist(_DM_(res), _D_(x), _D_(y), _D_(z), _D_(a));	}
//-----------------------------------------------------------------------------
mreal mgl_fit_1(HMGL gr, HMDT fit, const HMDT y, const char *eq, const char *var, mreal *ini)
{	return gr->Fit(*fit, *y, eq, var, ini);	}
mreal mgl_fit_2(HMGL gr, HMDT fit, const HMDT z, const char *eq, const char *var, mreal *ini)
{	return gr->Fit2(*fit, *z, eq, var, ini);	}
mreal mgl_fit_3(HMGL gr, HMDT fit, const HMDT a, const char *eq, const char *var, mreal *ini)
{	return gr->Fit3(*fit, *a, eq, var, ini);	}
mreal mgl_fit_xy(HMGL gr, HMDT fit, const HMDT x, const HMDT y, const char *eq, const char *var, mreal *ini)
{	return gr->Fit(*fit, *x, *y, eq, var, ini);	}
mreal mgl_fit_xyz(HMGL gr, HMDT fit, const HMDT x, const HMDT y, const HMDT z, const char *eq, const char *var, mreal *ini)
{	return gr->Fit(*fit, *x, *y, *z, eq, var, ini);	}
mreal mgl_fit_xyza(HMGL gr, HMDT fit, const HMDT x, const HMDT y, const HMDT z, const HMDT a, const char *eq, const char *var, mreal *ini)
{	return gr->Fit(*fit, *x, *y, *z, *a, eq, var, ini);	}
mreal mgl_fit_ys(HMGL gr, HMDT fit, const HMDT y, const HMDT s, const char *eq, const char *var, mreal *ini)
{	return gr->FitS(*fit, *y, *s, eq, var, ini);	}
mreal mgl_fit_xys(HMGL gr, HMDT fit, const HMDT x, const HMDT y, const HMDT s, const char *eq, const char *var, mreal *ini)
{	return gr->FitS(*fit, *x, *y, *s, eq, var, ini);	}
mreal mgl_fit_xyzs(HMGL gr, HMDT fit, const HMDT x, const HMDT y, const HMDT z, const HMDT s, const char *eq, const char *var, mreal *ini)
{	return gr->FitS(*fit, *x, *y, *z, *s, eq, var, ini);	}
mreal mgl_fit_xyzas(HMGL gr, HMDT fit, const HMDT x, const HMDT y, const HMDT z, const HMDT a, const HMDT s, const char *eq, const char *var, mreal *ini)
{	return gr->FitS(*fit, *x, *y, *z, *a, *s, eq, var, ini);	}

mreal mgl_fit_1_d(HMGL gr, HMDT fit, const HMDT y, const char *eq, const char *var, HMDT ini)
{	return gr->Fit(*fit, *y, eq, var, *ini);	}
mreal mgl_fit_2_d(HMGL gr, HMDT fit, const HMDT z, const char *eq, const char *var, HMDT ini)
{	return gr->Fit2(*fit, *z, eq, var, *ini);	}
mreal mgl_fit_3_d(HMGL gr, HMDT fit, const HMDT a, const char *eq, const char *var, HMDT ini)
{	return gr->Fit3(*fit, *a, eq, var, *ini);	}
mreal mgl_fit_xy_d(HMGL gr, HMDT fit, const HMDT x, const HMDT y, const char *eq, const char *var, HMDT ini)
{	return gr->Fit(*fit, *x, *y, eq, var, *ini);	}
mreal mgl_fit_xyz_d(HMGL gr, HMDT fit, const HMDT x, const HMDT y, const HMDT z, const char *eq, const char *var, HMDT ini)
{	return gr->Fit(*fit, *x, *y, *z, eq, var, *ini);	}
mreal mgl_fit_xyza_d(HMGL gr, HMDT fit, const HMDT x, const HMDT y, const HMDT z, const HMDT a, const char *eq, const char *var, HMDT ini)
{	return gr->Fit(*fit, *x, *y, *z, *a, eq, var, *ini);	}
mreal mgl_fit_ys_d(HMGL gr, HMDT fit, const HMDT y, const HMDT s, const char *eq, const char *var, HMDT ini)
{	return gr->FitS(*fit, *y, *s, eq, var, *ini);	}
mreal mgl_fit_xys_d(HMGL gr, HMDT fit, const HMDT x, const HMDT y, const HMDT s, const char *eq, const char *var, HMDT ini)
{	return gr->FitS(*fit, *x, *y, *s, eq, var, *ini);	}
mreal mgl_fit_xyzs_d(HMGL gr, HMDT fit, const HMDT x, const HMDT y, const HMDT z, const HMDT s, const char *eq, const char *var, HMDT ini)
{	return gr->FitS(*fit, *x, *y, *z, *s, eq, var, *ini);	}
mreal mgl_fit_xyzas_d(HMGL gr, HMDT fit, const HMDT x, const HMDT y, const HMDT z, const HMDT a, const HMDT s, const char *eq, const char *var, HMDT ini)
{	return gr->FitS(*fit, *x, *y, *z, *a, *s, eq, var, *ini);	}

void mgl_puts_fit(HMGL gr, mreal x, mreal y, mreal z, const char *prefix, const char *font, mreal size)
{	gr->PutsFit(mglPoint(x,y,z), prefix, font, size);	}
const char *mgl_get_fit(HMGL gr)	{	return gr->GetFit();	}
//-----------------------------------------------------------------------------
mreal mgl_fit_1_(uintptr_t* gr, uintptr_t* fit, uintptr_t* y, const char *eq, const char *var, mreal *ini, int l, int n)
{
	char *s=new char[l+1];	memcpy(s,eq,l);		s[l]=0;
	char *d=new char[n+1];	memcpy(d,var,n);	d[n]=0;
	mreal r = _GR_->Fit(_DM_(fit), _D_(y), s, d, ini);
	delete []s;		delete []d;	return r;
}
mreal mgl_fit_2_(uintptr_t* gr, uintptr_t* fit, uintptr_t* z, const char *eq, const char *var, mreal *ini, int l, int n)
{
	char *s=new char[l+1];	memcpy(s,eq,l);		s[l]=0;
	char *d=new char[n+1];	memcpy(d,var,n);	d[n]=0;
	mreal r = _GR_->Fit2(_DM_(fit), _D_(z), s, d, ini);
	delete []s;		delete []d;	return r;
}
mreal mgl_fit_3_(uintptr_t* gr, uintptr_t* fit, uintptr_t* a, const char *eq, const char *var, mreal *ini, int l, int n)
{
	char *s=new char[l+1];	memcpy(s,eq,l);		s[l]=0;
	char *d=new char[n+1];	memcpy(d,var,n);	d[n]=0;
	mreal r = _GR_->Fit3(_DM_(fit), _D_(a), s, d, ini);
	delete []s;		delete []d;	return r;
}
mreal mgl_fit_xy_(uintptr_t* gr, uintptr_t* fit, uintptr_t* x, uintptr_t* y, const char *eq, const char *var, mreal *ini, int l, int n)
{
	char *s=new char[l+1];	memcpy(s,eq,l);		s[l]=0;
	char *d=new char[n+1];	memcpy(d,var,n);	d[n]=0;
	mreal r = _GR_->Fit(_DM_(fit), _D_(x), _D_(y), s, d, ini);
	delete []s;		delete []d;	return r;
}
mreal mgl_fit_xyz_(uintptr_t* gr, uintptr_t* fit, uintptr_t* x, uintptr_t* y, uintptr_t* z, const char *eq, const char *var, mreal *ini, int l, int n)
{
	char *s=new char[l+1];	memcpy(s,eq,l);		s[l]=0;
	char *d=new char[n+1];	memcpy(d,var,n);	d[n]=0;
	mreal r = _GR_->Fit(_DM_(fit), _D_(x), _D_(y), _D_(z), s, d, ini);
	delete []s;		delete []d;	return r;
}
mreal mgl_fit_xyza_(uintptr_t* gr, uintptr_t* fit, uintptr_t* x, uintptr_t* y, uintptr_t* z, uintptr_t* a, const char *eq, const char *var, mreal *ini, int l, int n)
{
	char *s=new char[l+1];	memcpy(s,eq,l);		s[l]=0;
	char *d=new char[n+1];	memcpy(d,var,n);	d[n]=0;
	mreal r = _GR_->Fit(_DM_(fit), _D_(x), _D_(y), _D_(z), _D_(a), s, d, ini);
	delete []s;		delete []d;	return r;
}
mreal mgl_fit_ys_(uintptr_t* gr, uintptr_t* fit, uintptr_t* y, uintptr_t* ss, const char *eq, const char *var, mreal *ini, int l, int n)
{
	char *s=new char[l+1];	memcpy(s,eq,l);		s[l]=0;
	char *d=new char[n+1];	memcpy(d,var,n);	d[n]=0;
	mreal r = _GR_->FitS(_DM_(fit), _D_(y), _D_(ss), s, d, ini);
	delete []s;		delete []d;	return r;
}
mreal mgl_fit_xys_(uintptr_t* gr, uintptr_t* fit, uintptr_t* x, uintptr_t* y, uintptr_t* ss, const char *eq, const char *var, mreal *ini, int l, int n)
{
	char *s=new char[l+1];	memcpy(s,eq,l);		s[l]=0;
	char *d=new char[n+1];	memcpy(d,var,n);	d[n]=0;
	mreal r = _GR_->FitS(_DM_(fit), _D_(x), _D_(y), _D_(ss), s, d, ini);
	delete []s;		delete []d;	return r;
}
mreal mgl_fit_xyzs_(uintptr_t* gr, uintptr_t* fit, uintptr_t* x, uintptr_t* y, uintptr_t* z, uintptr_t* ss, const char *eq, const char *var, mreal *ini, int l, int n)
{
	char *s=new char[l+1];	memcpy(s,eq,l);		s[l]=0;
	char *d=new char[n+1];	memcpy(d,var,n);	d[n]=0;
	mreal r = _GR_->FitS(_DM_(fit), _D_(x), _D_(y), _D_(z), _D_(ss), s, d, ini);
	delete []s;		delete []d;	return r;
}
mreal mgl_fit_xyzas_(uintptr_t* gr, uintptr_t* fit, uintptr_t* x, uintptr_t* y, uintptr_t* z, uintptr_t* a, uintptr_t* ss, const char *eq, const char *var, mreal *ini, int l, int n)
{
	char *s=new char[l+1];	memcpy(s,eq,l);		s[l]=0;
	char *d=new char[n+1];	memcpy(d,var,n);	d[n]=0;
	mreal r = _GR_->FitS(_DM_(fit), _D_(x), _D_(y), _D_(z), _D_(a), _D_(ss), s, d, ini);
	delete []s;		delete []d;	return r;
}
void mgl_puts_fit_(uintptr_t* gr, mreal *x, mreal *y, mreal *z, const char *prefix, const char *font, mreal *size, int l, int n)
{
	char *s=new char[l+1];	memcpy(s,prefix,l);	s[l]=0;
	char *d=new char[n+1];	memcpy(d,font,n);	d[n]=0;
	_GR_->PutsFit(mglPoint(*x,*y,*z), s, d, *size);
	delete []s;		delete []d;
}
//-----------------------------------------------------------------------------
