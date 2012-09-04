/***************************************************************************
 * mgl_pde.cpp is part of Math Graphic Library
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
#include "mgl/mgl_eval.h"
#include "mgl/mgl_data.h"
#include "mgl/mgl.h"
#include "mgl/mgl_c.h"
#include "mgl/mgl_f.h"
#include <complex>
#define dual	std::complex<double>
#define GAMMA	0.1
#ifndef NO_GSL
#include <gsl/gsl_fft_complex.h>
#endif
//-----------------------------------------------------------------------------
// Solve equation du/dz = ham(p,q,x,y,z,|u|)[u] where p=d/dx, q=d/dy. At this moment simplified form of ham is supported: ham = f(p,q,z) + g(x,y,z,'u'), where variable 'u'=|u| (for allowing solve nonlinear problems). You may specify imaginary part like ham = p^2 + i*x*(x>0) but only if dependence on variable 'i' is linear (i.e. ham = hre+i*him).
mglData mglPDE(const char *ham, const mglData &ini_re, const mglData &ini_im, mglPoint Min, mglPoint Max, mreal dz, mreal k0)
{
	mglData res;
	int nx=ini_re.nx, ny=ini_re.ny;
	int nz = int((Max.z-Min.z)/dz)+1;
	if(nx<2 || nz<2 || Max.x==Min.x)	return res;	// Too small data
	if(ini_im.nx*ini_im.ny != nx*ny)	return res;	// Wrong dimensions
	res.Create(nz, nx, ny);
#ifndef NO_GSL
	mglFormula eqs(ham);
	dual *a = new dual[4*nx*ny], h, h0, h1, h2;	// Add "damping" area
	mreal *dmp = new mreal[4*nx*ny];
	memset(a,0,4*nx*ny*sizeof(dual));
	memset(dmp,0,4*nx*ny*sizeof(mreal));
	register int i,j,k,i0;
	for(j=0;j<ny;j++)	for(i=0;i<nx;i++)	// Initial conditions
	{
		i0 = i+nx/2+2*ny*(j+ny/2);
		a[i0] = dual(ini_re.a[i+nx*j], ini_im.a[i+nx*j]);
		res.a[nz*(i+nx*j)] = abs(a[i0]);
	}
	for(j=0;j<2*ny;j++)	for(i=0;i<2*nx;i++)	// mask
	{
		i0 = i+2*nx*j;
		if(i<nx/2)		dmp[i0] = GAMMA*mgl_ipow((nx/2-i)/(nx/2.),2);
		if(i>3*nx/2)	dmp[i0] = GAMMA*mgl_ipow((i-3*nx/2-1)/(nx/2.),2);
		if(j<ny/2)		dmp[i0]+= GAMMA*mgl_ipow((ny/2-j)/(ny/2.),2);
		if(j>3*ny/2)	dmp[i0]+= GAMMA*mgl_ipow((j-3*ny/2-1)/(ny/2.),2);
	}
	mreal dx = (Max.x-Min.x)/(nx-1), dy = ny>1?(Max.y-Min.y)/(ny-1):0;
	mreal dp = M_PI/(Max.x-Min.x)/k0, dq = M_PI/(Max.y-Min.y)/k0;
	mreal var[MGL_VS], xs=(Min.x+Max.x)/2, ys=(Min.y+Max.y)/2;
	double xx = Min.x - dx*nx/2, yy = Min.x - dy*ny/2;
	double ff = ny>1?4*nx*ny:2*nx, dd = k0*dz;
	memset(var,0,MGL_VS*sizeof(mreal));
	// prepare fft. NOTE: slow procedures due to unknown nx, ny.
	gsl_fft_complex_wavetable *wtx = gsl_fft_complex_wavetable_alloc(2*nx);
	gsl_fft_complex_workspace *wsx = gsl_fft_complex_workspace_alloc(2*nx);
	gsl_fft_complex_wavetable *wty = gsl_fft_complex_wavetable_alloc(2*ny);
	gsl_fft_complex_workspace *wsy = gsl_fft_complex_workspace_alloc(2*ny);
	for(k=1;k<nz;k++)
	{
		var['z'-'a'] = Min.z+dz*k;
		for(j=0;j<2*ny;j++)	for(i=0;i<2*nx;i++)	// step 1
		{
			i0 = i+2*nx*j;
			var['x'-'a'] = xx+dx*i;		var['p'-'a'] = 0;
			var['y'-'a'] = yy+dy*j;		var['q'-'a'] = 0;
			var['u'-'a'] = abs(a[i0]);
			h = dual(-eqs.CalcD(var,'i'), eqs.Calc(var))*dd;
			a[i0] *= exp(h)*exp(-double(dmp[i0]*dz));
		}
		// "central" point
		var['x'-'a'] = xs;	var['y'-'a'] = ys;
		var['u'-'a'] = var['p'-'a'] = var['q'-'a'] = 0;
		h0 = dual(-eqs.CalcD(var,'i'), eqs.Calc(var))*dd;
		// do fft
		for(i=0;i<2*ny;i++)
			gsl_fft_complex_transform((double *)(a+i*2*nx), 1, 2*nx, wtx, wsx, forward);
		if(ny>1) for(j=0;j<2*ny;j++)	for(i=0;i<2*nx;i++)	// step 3/2
		{
			i0 = i+2*nx*j;
			var['x'-'a'] = xs;			var['p'-'a'] = 0;
			var['y'-'a'] = yy+dy*j;		var['q'-'a'] = 0;
			h1 = dual(-eqs.CalcD(var,'i'), eqs.Calc(var))*dd;
			var['x'-'a'] = xs;			var['p'-'a'] = dp*(i<nx ? i:2*nx-i);
			var['y'-'a'] = ys;			var['q'-'a'] = 0;
			h2 = dual(-eqs.CalcD(var,'i'), eqs.Calc(var))*dd;
			var['x'-'a'] = xs;			var['p'-'a'] = dp*(i<nx ? i:2*nx-i);
			var['y'-'a'] = yy+dy*j;		var['q'-'a'] = 0;
			h = dual(-eqs.CalcD(var,'i'), eqs.Calc(var))*dd;
			a[i0] *= exp(h-h1-h2+h0);
		}
		if(ny>1) for(i=0;i<2*nx;i++)
			gsl_fft_complex_transform((double *)(a+i), 2*nx, 2*ny, wty, wsy, forward);
		for(j=0;j<2*ny;j++)	for(i=0;i<2*nx;i++)	// step 2
		{
			i0 = i+2*nx*j;
			var['x'-'a'] = xs;			var['p'-'a'] = dp*(i<nx ? i:2*nx-i);
			var['y'-'a'] = ys;			var['q'-'a'] = dq*(j<ny ? j:2*ny-j);
			h = dual(-eqs.CalcD(var,'i'), eqs.Calc(var))*dd;
			a[i0] *= exp(h-h0)/ff;
		}
		// do ifft
		if(ny>1) for(i=0;i<2*nx;i++)
			gsl_fft_complex_transform((double *)(a+i), 2*nx, 2*ny, wty, wsy, backward);
		if(ny>1) for(j=0;j<2*ny;j++)	for(i=0;i<2*nx;i++)	// step 3/2
		{
			i0 = i+2*nx*j;
			var['x'-'a'] = xx+dx*i;		var['p'-'a'] = 0;
			var['y'-'a'] = ys;			var['q'-'a'] = 0;
			h1 = dual(-eqs.CalcD(var,'i'), eqs.Calc(var))*dd;
			var['x'-'a'] = xs;			var['p'-'a'] = 0;
			var['y'-'a'] = ys;			var['q'-'a'] = dq*(j<ny ? j:2*ny-j);
			h2 = dual(-eqs.CalcD(var,'i'), eqs.Calc(var))*dd;
			var['x'-'a'] = xx+dx*i;		var['p'-'a'] = 0;
			var['y'-'a'] = ys;			var['q'-'a'] = dq*(j<ny ? j:2*ny-j);
			h = dual(-eqs.CalcD(var,'i'), eqs.Calc(var))*dd;
			a[i0] *= exp(h-h1-h2+h0);
		}
		for(i=0;i<2*ny;i++)
			gsl_fft_complex_transform((double *)(a+2*i*nx), 1, 2*nx, wtx, wsx, backward);
		// save result
		for(i=0;i<nx;i++)	for(j=0;j<ny;j++)
			res.a[k+nz*(i+nx*j)] = abs(a[i+nx/2+2*ny*(j+ny/2)]);
	}
	gsl_fft_complex_workspace_free(wsx);
	gsl_fft_complex_wavetable_free(wtx);
	gsl_fft_complex_workspace_free(wsy);
	gsl_fft_complex_wavetable_free(wty);
	delete []a;	delete []dmp;
#endif
	return res;
}
//-----------------------------------------------------------------------------
// Solve GO ray equation like dr/dt = d ham/dp, dp/dt = -d ham/dr where ham = ham(x,y,z,p,q,v,t) and px=p, py=q, pz=v. The starting point (at t=0) is r0, p0. Result is array of {x,y,z,p,q,v,t}
mglData mglRay(const char *ham, mglPoint r0, mglPoint p0, mreal dt, mreal tmax)
{
	mglData res;
	if(tmax<dt)	return res;	// nothing to do
	int nt = int(tmax/dt)+1;
	mreal x[6], k1[6], k2[6], k3[6], hh=dt/2;
	res.Create(7,nt);	res.SetColumnId("xyzpqvt");
#ifndef NO_GSL
	mglFormula eqs(ham);
	// initial conditions
	x[0] = res.a[0] = r0.x;	x[1] = res.a[1] = r0.y;	x[2] = res.a[2] = r0.z;
	x[3] = res.a[3] = p0.x;	x[4] = res.a[4] = p0.y;	x[5] = res.a[5] = p0.z;
	res.a[6] = 0;
	// Runge Kutta scheme of 4th order
	char v[7]="xyzpqv";
	mreal var[MGL_VS];	memset(var,0,MGL_VS*sizeof(mreal));
	register int i,k;
	for(k=1;k<nt;k++)
	{
		// 		md->H(cy,k1);
		var['t'-'a']=k*dt;		for(i=0;i<6;i++)	var[v[i]-'a'] = x[i];
		k1[0] = eqs.CalcD(var,'p');	k1[3] = -eqs.CalcD(var,'x');
		k1[1] = eqs.CalcD(var,'q');	k1[4] = -eqs.CalcD(var,'y');
		k1[2] = eqs.CalcD(var,'v');	k1[5] = -eqs.CalcD(var,'z');
		// 		ty = cy/(k1*hh);	md->H(ty,k2);
		var['t'-'a']=k*dt+hh;	for(i=0;i<6;i++)	var[v[i]-'a'] = x[i]+k1[i]*hh;
		k2[0] = eqs.CalcD(var,'p');	k2[3] = -eqs.CalcD(var,'x');
		k2[1] = eqs.CalcD(var,'q');	k2[4] = -eqs.CalcD(var,'y');
		k2[2] = eqs.CalcD(var,'v');	k2[5] = -eqs.CalcD(var,'z');
		//		ty = cy/(k2*hh);	md->H(ty,k3);
		var['t'-'a']=k*dt+hh;	for(i=0;i<6;i++)	var[v[i]-'a'] = x[i]+k2[i]*hh;
		k3[0] = eqs.CalcD(var,'p');	k3[3] = -eqs.CalcD(var,'x');
		k3[1] = eqs.CalcD(var,'q');	k3[4] = -eqs.CalcD(var,'y');
		k3[2] = eqs.CalcD(var,'v');	k3[5] = -eqs.CalcD(var,'z');
		//		ty = cy/(k2*h);	k3+=k2;	md->H(ty,k2);
		var['t'-'a']=k*dt+dt;	for(i=0;i<6;i++)
		{	var[v[i]-'a'] = x[i]+k2[i]*dt;	k3[i] += k2[i];	}
		k2[0] = eqs.CalcD(var,'p');	k2[3] = -eqs.CalcD(var,'x');
		k2[1] = eqs.CalcD(var,'q');	k2[4] = -eqs.CalcD(var,'y');
		k2[2] = eqs.CalcD(var,'v');	k2[5] = -eqs.CalcD(var,'z');
		//		cy /= (k1+k2+k3*2.)*(h/6);
		for(i=0;i<6;i++)
			res.a[i+7*k] = x[i] += (k1[i]+k2[i]+2*k3[i])*dt/6;
		res.a[6+7*k] = dt*k;
	}
#endif
	return res;
}
//-----------------------------------------------------------------------------
struct mgl_ap
{
	double x0,y0,x1,y1;	// vectors {l, g1, g2}
	double t1,ch,q1,pt,dt,d1;	// theta_{1,2}, chi, q_{1,2}, p_t, dtau, dq_{1,2}
	mgl_ap()	{	memset(this,0,sizeof(mgl_ap));	};
};
//-----------------------------------------------------------------------------
void mgl_init_ra(int n, const mreal *r, mgl_ap *ra)	// prepare some intermediate data for mglPDE2d
{
	register double tt;
	tt = hypot(r[7]-r[0], r[8]-r[1]);
	ra[0].x1 = (r[8]-r[1])/tt;
	ra[0].y1 = (r[0]-r[7])/tt;
	register long i;
	for(i=1;i<n;i++)
	{
		ra[i].dt = r[6+7*i] - r[7*i-1];
		ra[i].x0 = r[7*i] - r[7*i-7];	// NOTE: very rough formulas
		ra[i].y0 = r[7*i+1] - r[7*i-6];	// for corresponding with dt one
		tt = sqrt(ra[i].x0*ra[i].x0 + ra[i].y0*ra[i].y0);
		ra[i].x0 /= tt;	ra[i].y0 /= tt;
		ra[i].ch = tt/ra[i].dt;
		tt = ra[i].x0*ra[i-1].x1 + ra[i].y0*ra[i-1].y1;
		ra[i].x1 = ra[i-1].x1 - tt*ra[i].x0;	// vector g_1
		ra[i].y1 = ra[i-1].y1 - tt*ra[i].y0;
		ra[i].t1 = tt/(ra[i].dt*ra[i].ch);
		tt = sqrt(ra[i].x1*ra[i].x1 + ra[i].y1*ra[i].y1);
		ra[i].x1 /= tt;	ra[i].y1 /= tt;
		// other parameters
		ra[i].pt = r[7*i+3]*ra[i].x0 + r[7*i+4]*ra[i].y0;
		ra[i].q1 = r[7*i+3]*ra[i].x1 + r[7*i+4]*ra[i].y1;
		ra[i].d1 = (ra[i].q1-ra[i-1].q1)/(ra[i].dt*ra[i].ch);
	}
	memcpy(ra,ra+1,sizeof(mgl_ap));	// setup zero point
	ra[0].pt = r[3]*ra[0].x0 + r[4]*ra[0].y0;
	ra[0].q1 = r[3]*ra[0].x1 + r[4]*ra[0].y1;
}
//-----------------------------------------------------------------------------
mglData mglQO2d(const char *ham, const mglData &ini_re, const mglData &ini_im, const mglData &ray, mreal r, mreal k0, mglData *xx, mglData *yy, bool UseR)
{
	mglData res;
	int nx=ini_re.nx, nt=ray.ny;
	if(nx<2 || ini_im.nx!=nx || nt<2)	return res;
	res.Create(nx,nt);
#ifndef NO_GSL
	dual *a=new dual[2*nx], *hu=new dual[2*nx],  *hx=new dual[2*nx], h0;
	double *ru=new double[2*nx],  *rx=new double[2*nx],
			*pu=new double[2*nx],  *px=new double[2*nx];
	mgl_ap *ra = new mgl_ap[nt];	mgl_init_ra(ray.ny, ray.a, ra);	// ray
	register int i;
	for(i=0;i<nx;i++)	a[i+nx/2] = dual(ini_re.a[i],ini_im.a[i]);	// ini
	for(i=0;i<2*nx;i++)	{	rx[i] = ru[i] = 1;	}
	mglFormula h(ham);
	mreal var[MGL_VS], dr = r/(nx-1), dk = M_PI*(nx-1)/(k0*r*nx), tt, x1, hh, B1, pt0;
	memset(var,0,MGL_VS*sizeof(mreal));
	gsl_fft_complex_wavetable *wtx = gsl_fft_complex_wavetable_alloc(2*nx);
	gsl_fft_complex_workspace *wsx = gsl_fft_complex_workspace_alloc(2*nx);
	if(xx && yy)	{	xx->Create(nx,nt);	yy->Create(nx,nt);	}
	// start calculation
	for(int k=0;k<nt;k++)
	{
		for(i=0;i<nx;i++)	// "save"
			res.a[i+k*nx]=abs(a[i+nx/2])*sqrt(ra[0].ch/ra[k].ch);
		if(xx && yy)	for(i=0;i<nx;i++)	// prepare xx, yy
		{
			x1 = (2*i-nx+1)*dr;
			xx->a[i+k*nx] = ray.a[7*k] + ra[k].x1*x1;	// new coordiantes
			yy->a[i+k*nx] = ray.a[7*k+1] + ra[k].y1*x1;
		}
		memcpy(px,rx,2*nx*sizeof(double));
		memcpy(pu,ru,2*nx*sizeof(double));
		hh = ra[k].pt*(1/sqrt(sqrt(1.041))-1);	// 0.041=0.45^4 -- minimal value of h
		var['x'-'a'] = ray.a[7*k];	var['y'-'a'] = ray.a[7*k+1];
		var['p'-'a'] = ray.a[7*k+3] + ra[k].x0*hh;
		var['q'-'a'] = ray.a[7*k+4] + ra[k].y0*hh;	var['u'-'a'] = 0;
		pt0 = (h.CalcD(var,'p')*ra[k].x0 + h.CalcD(var,'q')*ra[k].y0)/ra[k].ch;
		for(i=0;i<2*nx;i++)	// prepare hamiltonian
		{
			hh = ra[k].pt*(1/sqrt(sqrt(1.041))-1);
			var['x'-'a'] = ray.a[7*k];	var['y'-'a'] = ray.a[7*k+1];
			var['p'-'a'] = ray.a[7*k+3] + ra[k].x0*hh;
			var['q'-'a'] = ray.a[7*k+4] + ra[k].y0*hh;	var['u'-'a'] = 0;
			h0 = dual(h.Calc(var), -h.CalcD(var,'i'));
			// x terms
			x1 = (2*i-2*nx+1)*dr;
			var['x'-'a'] = ray.a[7*k] + ra[k].x1*x1;	// new coordiantes
			var['y'-'a'] = ray.a[7*k+1] + ra[k].y1*x1;
			hh = 1 - ra[k].t1*x1;	hh = sqrt(sqrt(0.041+hh*hh*hh*hh));
			tt = (ra[k].pt + ra[k].d1*x1)/hh - ra[k].pt;
			var['p'-'a'] = ray.a[7*k+3] + ra[k].x0*tt;	// new momentums
			var['q'-'a'] = ray.a[7*k+4] + ra[k].y0*tt;	var['u'-'a'] = abs(a[i]);
			rx[i] = (h.CalcD(var,'p')*ra[k].x0 + h.CalcD(var,'q')*ra[k].y0)/ra[k].ch;
			rx[i] = rx[i]>0.3*pt0 ? rx[i] : 0.3*pt0;
			hx[i] = (dual(h.Calc(var), -h.CalcD(var,'i'))-h0/2.)/rx[i];
			if(imag(hx[i])>0)	hx[i] = hx[i].real();
			// u-y terms
			x1 = dk/2*(i<nx ? i:i-2*nx);
			var['x'-'a'] = ray.a[7*k];	var['y'-'a'] = ray.a[7*k+1];
			var['p'-'a'] = ray.a[7*k+3] + ra[k].x1*x1;	// new momentums
			var['q'-'a'] = ray.a[7*k+4] + ra[k].y1*x1;	var['u'-'a'] = 0;
			ru[i] = (h.CalcD(var,'p')*ra[k].x0 + h.CalcD(var,'q')*ra[k].y0)/ra[k].ch;
			ru[i] = ru[i]>0.3*pt0 ? ru[i] : 0.3*pt0;
			hu[i] = (dual(h.Calc(var), -h.CalcD(var,'i'))-h0/2.)/ru[i];
			if(imag(hu[i])>0)	hu[i] = hu[i].real();
			// add boundary conditions for x-direction
			if(i<nx/2)
			{
				x1 = (nx/2-i)/(nx/2.);
				hx[i] -= dual(0,30*GAMMA*x1*x1/k0);
			}
			if(i>=3*nx/2)
			{
				x1 = (i-3*nx/2-1)/(nx/2.);
				hx[i] -= dual(0,30*GAMMA*x1*x1/k0);
			}
		}
		// Calculate B1
		hh = ra[k].pt*(1/sqrt(sqrt(1.041))-1);
		var['x'-'a'] = ray.a[7*k];	// new coordiantes
		var['y'-'a'] = ray.a[7*k+1];
		var['p'-'a'] = ray.a[7*k+3] + ra[k].x0*hh;	// new momentums
		var['q'-'a'] = ray.a[7*k+4] + ra[k].y0*hh;
		tt = h.CalcD(var,'p')*ra[k].x1 + h.CalcD(var,'q')*ra[k].y1;
		var['x'-'a'] = ray.a[7*k] + ra[k].x1*dr;	// new coordiantes
		var['y'-'a'] = ray.a[7*k+1] + ra[k].y1*dr;
		hh = 1 - ra[k].t1*dr;	hh = sqrt(sqrt(0.041+hh*hh*hh*hh));
		hh = (ra[k].ch*ra[k].pt + ra[k].d1*dr)/(hh*ra[k].ch) - ra[k].pt;
		var['p'-'a'] = ray.a[7*k+3] + ra[k].x0*hh;	// new momentums
		var['q'-'a'] = ray.a[7*k+4] + ra[k].y0*hh;
		B1 = h.CalcD(var,'p')*ra[k].x1 + h.CalcD(var,'q')*ra[k].y1;
		B1 = (B1-tt)/dr;
		// Step for field
		dual dt = dual(0, -ra[k].dt*k0);
		for(i=0;i<2*nx;i++)
			a[i] *= exp(hx[i]*dt)*((UseR && k>0)?sqrt(px[i]/rx[i]):1.);
		gsl_fft_complex_transform((double *)a, 1, 2*nx, wtx, wsx, forward);
		for(i=0;i<2*nx;i++)
			a[i] *= exp(hu[i]*dt)*((UseR && k>0)?sqrt(pu[i]/ru[i]):1.)/(2.*nx);
		gsl_fft_complex_transform((double *)a, 1, 2*nx, wtx, wsx, backward);
		double a1=0, a2=0;
		for(i=0;i<2*nx;i++)	a1 += norm(a[i]);
		hx[0] = hx[2*nx-1] = 0.;
		for(i=1;i<2*nx-1;i++)	hx[i] = (B1*ra[k].dt*(i-nx))*(a[i+1]-a[i-1]);
		for(i=0;i<2*nx;i++)	{	a[i] += hx[i];	a2 += norm(a[i]);	}
		a1 = sqrt(a1/a2);
		for(i=0;i<2*nx;i++)	a[i] *= a1;
	}
	gsl_fft_complex_workspace_free(wsx);
	gsl_fft_complex_wavetable_free(wtx);
	delete []a;		delete []hu;	delete []hx;	delete []ra;
	delete []rx;	delete []ru;	delete []px;	delete []pu;
#endif
	return res;
}
//-----------------------------------------------------------------------------
mglData mglAF2d(const char *ham, const mglData &ini_re, const mglData &ini_im, const mglData &ray, mreal r, mreal k0, mglData *xx, mglData *yy, bool UseR)
{
	mglData res;
	int nx=ini_re.nx, nt=ray.ny;
	if(nx<2 || ini_im.nx!=nx || nt<2)	return res;
	res.Create(nx,nt);
#ifndef NO_GSL
	dual *a=new dual[2*nx], *hu=new dual[2*nx],  *hx=new dual[2*nx];
	mgl_ap *ra = new mgl_ap[nt];	mgl_init_ra(ray.ny, ray.a, ra);	// ray
	register int i;
	for(i=0;i<nx;i++)	a[i+nx/2] = dual(ini_re.a[i],ini_im.a[i]);	// ini
	mglFormula h(ham);
	mreal var[MGL_VS], dr = r/(nx-1), dk = M_PI/(k0*r), tt, x1, hh, B1;
	memset(var,0,MGL_VS*sizeof(mreal));
	gsl_fft_complex_wavetable *wtx = gsl_fft_complex_wavetable_alloc(2*nx);
	gsl_fft_complex_workspace *wsx = gsl_fft_complex_workspace_alloc(2*nx);
	if(xx && yy)	{	xx->Create(nx,nt);	yy->Create(nx,nt);	}
	// start calculation
	for(int k=0;k<nt;k++)
	{
		for(i=0;i<nx;i++)	// "save"
			res.a[i+k*nx]=abs(a[i+nx/2])*sqrt(ra[0].ch/ra[k].ch);
		if(xx && yy)	for(i=0;i<nx;i++)	// prepare xx, yy
		{
			x1 = (2*i-nx+1)*dr;
			xx->a[i+k*nx] = ray.a[7*k] + ra[k].x1*x1;	// new coordiantes
			yy->a[i+k*nx] = ray.a[7*k+1] + ra[k].y1*x1;
		}
		var['x'-'a'] = ray.a[7*k];	var['y'-'a'] = ray.a[7*k+1];
		var['p'-'a'] = ray.a[7*k+3];var['q'-'a'] = ray.a[7*k+4];
		var['u'-'a'] = 0;
		double p01 = h.CalcD(var,'p'), p02 = h.CalcD(var,'q'),
				h0 = h.Calc(var), g0 = h.CalcD(var,'i'), hpp, hxx;
		// derivative along momentum
		var['p'-'a'] = ray.a[7*k+3] + ra[k].x1*dk;	// new momentums
		var['q'-'a'] = ray.a[7*k+4] + ra[k].y1*dk;
		hpp = ((h.CalcD(var,'p')-p01)*ra[k].x1 + (h.CalcD(var,'q')-p02)*ra[k].y1)/dk;
		// derivative along coordinates
		var['x'-'a'] = ray.a[7*k] + ra[k].x1*dr;	// new coordiantes
		var['y'-'a'] = ray.a[7*k+1] + ra[k].y1*dr;
		hh = 1 - ra[k].t1*dr;	tt = (ra[k].pt + ra[k].d1*dr)/hh - ra[k].pt;
		var['p'-'a'] = ray.a[7*k+3] + ra[k].x0*tt;	// new momentums
		var['q'-'a'] = ray.a[7*k+4] + ra[k].y0*tt;
		B1 = ((h.CalcD(var,'p')-p01)*ra[k].x1 + (h.CalcD(var,'q')-p02)*ra[k].y1)/dr;
		hxx = h.Calc(var)*hh;
		var['x'-'a'] = ray.a[7*k] - ra[k].x1*dr;	// new coordiantes
		var['y'-'a'] = ray.a[7*k+1] - ra[k].y1*dr;
		hh = 1 + ra[k].t1*dr;	tt = (ra[k].pt - ra[k].d1*dr)/hh - ra[k].pt;
		var['p'-'a'] = ray.a[7*k+3] + ra[k].x0*tt;	// new momentums
		var['q'-'a'] = ray.a[7*k+4] + ra[k].y0*tt;
		hxx = (hxx+h.Calc(var)*hh-2*h0)/(dr*dr);

		for(i=0;i<2*nx;i++)	// prepare hamiltonian
		{
			// x terms
			x1 = 2*(i-nx)*dr;	hx[i] = dual(hxx*x1*x1/2, -g0);
			// u-y terms
			x1 = dk/2*(i<nx ? i:i-2*nx);	hu[i] = hpp*x1*x1/2;
			// add boundary conditions for x-direction
			if(i<nx/2)
			{
				x1 = (nx/2-i)/(nx/2.);
				hx[i] -= dual(0,30*GAMMA*x1*x1/k0);
			}
			if(i>=3*nx/2)
			{
				x1 = (i-3*nx/2-1)/(nx/2.);
				hx[i] -= dual(0,30*GAMMA*x1*x1/k0);
			}
		}
		// Step for field
		dual dt = dual(0, -ra[k].dt*k0);
		for(i=0;i<2*nx;i++)
			a[i] *= exp(hx[i]*dt)/(2.*nx);
		gsl_fft_complex_transform((double *)a, 1, 2*nx, wtx, wsx, forward);
		for(i=0;i<2*nx;i++)
			a[i] *= exp((hu[i]-hu[0])*dt);
		gsl_fft_complex_transform((double *)a, 1, 2*nx, wtx, wsx, backward);
		double a1=0, a2=0;
		for(i=0;i<2*nx;i++)	a1 += norm(a[i]);
		hx[0] = hx[2*nx-1] = 0.;
		for(i=1;i<2*nx-1;i++)	hx[i] = (B1*ra[k].dt*(i-nx))*(a[i+1]-a[i-1]);
		for(i=0;i<2*nx;i++)	{	a[i] += hx[i];	a2 += norm(a[i]);	}
		a1 = sqrt(a1/a2);
		for(i=0;i<2*nx;i++)	a[i] *= a1;
	}
	gsl_fft_complex_workspace_free(wsx);
	gsl_fft_complex_wavetable_free(wtx);
	delete []a;		delete []hu;	delete []hx;	delete []ra;
#endif
	return res;
}
//-----------------------------------------------------------------------------
mglData mglJacobian(const mglData &x, const mglData &y)
{
	int nx = x.nx, ny=x.ny;
	mglData r;
	if(nx!=y.nx || ny!=y.ny || nx<2 || ny<2)	return	r;
	r.Create(nx,ny);
	register int i,j,i0,ip,im,jp,jm;
	for(i=0;i<nx;i++)	for(j=0;j<ny;j++)
	{
		i0 = i+nx*j;
		ip = i<nx-1 ? 1:0;	jp = j<ny-1 ? nx:0;
		im = i>0 ? -1:0;	jm = j>0 ? -nx:0;
		r.a[i0] = (x.a[i0+ip]-x.a[i0+im])*(y.a[i0+jp]-y.a[i0+jm]) -
				(y.a[i0+ip]-y.a[i0+im])*(x.a[i0+jp]-x.a[i0+jm]);
		r.a[i0] *= mreal((nx-1)*(ny-1)*nx) / ((ip-im)*(jp-jm));
	}
	return r;
}
//-----------------------------------------------------------------------------
mglData mglJacobian(const mglData &x, const mglData &y, const mglData &z)
{
	int nx = x.nx, ny=x.ny, nz=x.nz;
	mglData r;
	if(nx*ny*nz!=y.nx*y.ny*y.nz || nx*ny*nz!=z.nx*z.ny*z.nz)	return r;
	if(nx<2 || ny<2 || nz<2)	return	r;
	r.Create(nx,ny,nz);
	register int i,j,k,i0,ip,im,jp,jm,kp,km;
	for(i=0;i<nx;i++)	for(j=0;j<ny;j++)	for(k=0;k<nz;k++)
	{
		i0 = i+nx*(j+ny*k);
		ip = i<nx-1 ? 1:0;	jp = j<ny-1 ? nx:0;	kp = k<nz-1 ? nx*ny:0;
		im = i>0 ? -1:0;	jm = j>0 ? -nx:0;	km = k>0 ? -nx*ny:0;
		r.a[i0] = (x.a[i0+ip]-x.a[i0+im])*(y.a[i0+jp]-y.a[i0+jm])*(z.a[i0+kp]-z.a[i0+km]) -
				(x.a[i0+ip]-x.a[i0+im])*(y.a[i0+kp]-y.a[i0+km])*(z.a[i0+jp]-z.a[i0+jm]) -
				(x.a[i0+jp]-x.a[i0+jm])*(y.a[i0+ip]-y.a[i0+im])*(z.a[i0+kp]-z.a[i0+km]) +
				(x.a[i0+jp]-x.a[i0+jm])*(y.a[i0+kp]-y.a[i0+km])*(z.a[i0+ip]-z.a[i0+im]) +
				(x.a[i0+kp]-x.a[i0+km])*(y.a[i0+ip]-y.a[i0+im])*(z.a[i0+jp]-z.a[i0+jm]) -
				(x.a[i0+kp]-x.a[i0+km])*(y.a[i0+jp]-y.a[i0+jm])*(z.a[i0+ip]-z.a[i0+im]);
		r.a[i0] *= mreal((nx-1)*(ny-1)*(nz-1))*mreal(nx*nx*ny) / ((ip-im)*(jp-jm)*(kp-km));
	}
	return r;
}
//-----------------------------------------------------------------------------
HMDT mgl_pde_solve(HMGL gr, const char *ham, const HMDT ini_re, const HMDT ini_im, mreal dz, mreal k0)
{	return new mglData(mglPDE(ham, *ini_re, *ini_im, gr->Min, gr->Max, dz,k0));	}
HMDT mgl_ray_trace(const char *ham, mreal x0, mreal y0, mreal z0, mreal px, mreal py, mreal pz, mreal dt, mreal tmax)
{	return new mglData(mglRay(ham, mglPoint(x0,y0,z0), mglPoint(px,py,pz), dt,tmax));	}
HMDT mgl_qo2d_solve(const char *ham, const HMDT ini_re, const HMDT ini_im, const HMDT ray, mreal r, mreal k0, HMDT xx, HMDT yy)
{	return new mglData(mglQO2d(ham, *ini_re, *ini_im, *ray, r, k0, xx, yy));	}
HMDT mgl_af2d_solve(const char *ham, const HMDT ini_re, const HMDT ini_im, const HMDT ray, mreal r, mreal k0, HMDT xx, HMDT yy)
{	return new mglData(mglAF2d(ham, *ini_re, *ini_im, *ray, r, k0, xx, yy));	}
HMDT mgl_jacobian_2d(const HMDT x, const HMDT y)
{	return new mglData(mglJacobian(*x, *y));	}
HMDT mgl_jacobian_3d(const HMDT x, const HMDT y, const HMDT z)
{	return new mglData(mglJacobian(*x, *y, *z));	}
//-----------------------------------------------------------------------------
uintptr_t mgl_pde_solve_(uintptr_t* gr, const char *ham, uintptr_t* ini_re, uintptr_t* ini_im, mreal *dz, mreal *k0, int l)
{
	char *s=new char[l+1];	memcpy(s,ham,l);	s[l]=0;
	uintptr_t res = uintptr_t(new mglData(mglPDE(s, _D_(ini_re), _D_(ini_im), _GR_->Min, _GR_->Max, *dz, *k0)));
	delete []s;	return res;
}
uintptr_t mgl_ray_trace_(const char *ham, mreal *x0, mreal *y0, mreal *z0, mreal *px, mreal *py, mreal *pz, mreal *dt, mreal *tmax,int l)
{
	char *s=new char[l+1];	memcpy(s,ham,l);	s[l]=0;
	uintptr_t res = uintptr_t(new mglData(mglRay(s, mglPoint(*x0,*y0,*z0), mglPoint(*px,*py,*pz), *dt,*tmax)));
	delete []s;	return res;
}
uintptr_t mgl_qo2d_solve_(const char *ham, uintptr_t* ini_re, uintptr_t* ini_im, uintptr_t* ray, mreal *r, mreal *k0, uintptr_t* xx, uintptr_t* yy, int l)
{
	char *s=new char[l+1];	memcpy(s,ham,l);	s[l]=0;
	uintptr_t res = uintptr_t(new mglData(mglQO2d(s, _D_(ini_re), _D_(ini_im), _D_(ray), *r, *k0, ((mglData *)*xx), ((mglData *)*yy))));
	delete []s;	return res;
}
uintptr_t mgl_af2d_solve_(const char *ham, uintptr_t* ini_re, uintptr_t* ini_im, uintptr_t* ray, mreal *r, mreal *k0, uintptr_t* xx, uintptr_t* yy, int l)
{
	char *s=new char[l+1];	memcpy(s,ham,l);	s[l]=0;
	uintptr_t res = uintptr_t(new mglData(mglAF2d(s, _D_(ini_re), _D_(ini_im), _D_(ray), *r, *k0, ((mglData *)*xx), ((mglData *)*yy))));
	delete []s;	return res;
}
uintptr_t mgl_jacobian_2d_(uintptr_t* x, uintptr_t* y)
{	return uintptr_t(new mglData(mglJacobian(_D_(x), _D_(y))));	}
uintptr_t mgl_jacobian_3d_(uintptr_t* x, uintptr_t* y, uintptr_t* z)
{	return uintptr_t(new mglData(mglJacobian(_D_(x), _D_(y), _D_(z))));	}
//-----------------------------------------------------------------------------
