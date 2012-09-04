/***************************************************************************
 * mgl_data_cf.cpp is part of Math Graphic Library
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
#include <string.h>

#include "mgl/mgl_data.h"
#include "mgl/mgl_c.h"
#include "mgl/mgl_f.h"

//-----------------------------------------------------------------------------
//		Data operations
//-----------------------------------------------------------------------------
/// Multiplicate the data by other one for each element
void mgl_data_mul_dat(HMDT d, HMDT b){	*d *= *b;	}
/// Divide the data by other one for each element
void mgl_data_div_dat(HMDT d, HMDT b) {	*d /= *b;	}
/// Add the other data
void mgl_data_add_dat(HMDT d, HMDT b) {	*d += *b;	}
/// Substract the other data
void mgl_data_sub_dat(HMDT d, HMDT b) {	*d -= *b;	}
/// Multiplicate each element by the number
void mgl_data_mul_num(HMDT d, mreal b) {	*d *= b;	}
/// Divide each element by the number
void mgl_data_div_num(HMDT d, mreal b) {	*d /= b;	}
/// Add the number
void mgl_data_add_num(HMDT d, mreal b) {	*d += b;	}
/// Substract the number
void mgl_data_sub_num(HMDT d, mreal b) {	*d -= b;	}
//-----------------------------------------------------------------------------
/// Rearrange data dimensions
void mgl_data_rearrange(HMDT d, int mx, int my, int mz)
{	d->Rearrange(mx,my,mz);	}
/// Resize the data to new size of box [x1,x2]*[y1,y2]*[z1,z2]
HMDT mgl_data_resize(const HMDT d, int mx,int my,int mz)
{	return new mglData(d->Resize(mx,my,mz));	}
HMDT mgl_data_resize_box(const HMDT d, int mx,int my,int mz,mreal x1,mreal x2,
	mreal y1,mreal y2,mreal z1,mreal z2)
{	return new mglData(d->Resize(mx,my,mz,x1,x2,y1,y2,z1,z2));	}
/// Get sub-array of the data with given fixed indexes
HMDT mgl_data_subdata(const HMDT d, int xx,int yy,int zz)
{	return new mglData(d->SubData(xx,yy,zz));	}
HMDT mgl_data_subdata_ext(const HMDT d, const HMDT xx, const HMDT yy, const HMDT zz)
{	return new mglData(d->SubData(*xx,*yy,*zz));	}
/// Get column (or slice) of the data filled by formulas of other named columns
HMDT mgl_data_column(const HMDT d, const char *eq)
{	return new mglData(d->Column(eq));	}
/// Set names for columns (slices)
void mgl_data_set_id(HMDT d, const char *id)
{	d->SetColumnId(id);	}
/// Eqidistantly fill the data to range [x1,x2] in direction \a dir
void mgl_data_fill(HMDT d, mreal x1,mreal x2,char dir)
{	d->Fill(x1,x2,dir);	}
/// Modify the data by specified formula
void mgl_data_modify(HMDT d, const char *eq,int dim)
{	d->Modify(eq,dim);	}
/// Modify the data by specified formula
void mgl_data_modify_vw(HMDT d, const char *eq, HMDT v, HMDT w)
{	if(d)
	{	if(v&&w)	d->Modify(eq,*v,*w);	else if(v)	d->Modify(eq,*v);	else	d->Modify(eq);	}
}
/// Create or recreate the array with specified size and fill it by zero
void mgl_data_create(HMDT d, int nx,int ny,int nz)
{	d->Create(nx,ny,nz);	}
/// Transpose the x<->y dimensions of the data
void mgl_data_transpose(HMDT d, const char *dim)	{	d->Transpose(dim);	}
/// Normalize the data to range [v1,v2]
void mgl_data_norm(HMDT d, mreal v1,mreal v2,int sym,int dim)
{	d->Norm(v1,v2,sym,dim);	}
/// Normalize the data to range [v1,v2] slice by slice
void mgl_data_norm_slice(HMDT d, mreal v1,mreal v2,char dir,int keep_en,int sym)
{	d->NormSl(v1,v2,dir,keep_en,sym);	}
/// Reduce size of the data
void mgl_data_squeeze(HMDT d, int rx,int ry,int rz,int smooth)
{	d->Squeeze(rx,ry,rz,smooth);	}
/// Get maximal value of the data
mreal mgl_data_max(const HMDT d)	{	return d->Maximal();	}
/// Get minimal value of the data
mreal mgl_data_min(const HMDT d)	{	return d->Minimal();	}
/// Get the value in given cell of the data
mreal *mgl_data_value(HMDT d, int i,int j,int k)
{	return d->a+i+d->nx*(j+d->ny*k);	}
/// Swap left and right part of the data in given direction (useful for fourier spectrums)
void mgl_data_swap(HMDT d, const char *dir)		{	d->Swap(dir);	}
/// Swap left and right part of the data in given direction (useful for fourier spectrums)
void mgl_data_roll(HMDT d, char dir, int num)	{	d->Roll(dir,num);	}
/// Mirror the data in given direction
void mgl_data_mirror(HMDT d, const char *dir)	{	d->Mirror(dir);	}
void mgl_data_fill_sample(HMDT d, int num, const char *how)	{	d->FillSample(num,how);	}
void mgl_data_insert(HMDT d, char dir, int at, int num){	d->Insert(dir,at,num);	}
void mgl_data_delete(HMDT d, char dir, int at, int num){	d->Delete(dir,at,num);	}
void mgl_data_hankel(HMDT d, const char *dir)	{	d->Hankel(dir);	}
void mgl_data_sinfft(HMDT d, const char *dir)	{	d->SinFFT(dir);	}
void mgl_data_cosfft(HMDT d, const char *dir)	{	d->CosFFT(dir);	}
//-----------------------------------------------------------------------------
/// Allocate memory and copy the data from the (mreal *) array
void mgl_data_set_float(HMDT d, const float *A,int NX,int NY,int NZ)
{	d->Set(A,NX,NY,NZ);	}
/// Allocate memory and copy the data from the (mreal *) array
void mgl_data_set_double(HMDT d, const double *A,int NX,int NY,int NZ)
{	d->Set(A,NX,NY,NZ);	}
/// Allocate memory and copy the data from the (mreal **) array
void mgl_data_set_float2(HMDT d, const float **A,int N1,int N2)
{	d->Set(A,N1,N2);	}
/// Allocate memory and copy the data from the (mreal **) array
void mgl_data_set_double2(HMDT d, const double **A,int N1,int N2)
{	d->Set(A,N1,N2);	}
/// Allocate memory and copy the data from the (mreal ***) array
void mgl_data_set_float3(HMDT d, const float ***A,int N1,int N2,int N3)
{	d->Set(A,N1,N2,N3);	}
/// Allocate memory and copy the data from the (mreal ***) array
void mgl_data_set_double3(HMDT d, const double ***A,int N1,int N2,int N3)
{	d->Set(A,N1,N2,N3);	}
/// Copy the data from other mglData variable
void mgl_data_set(HMDT d, HMDT a)	{	d->Set(*a);	}
/// Read data from tab-separated text file with auto determining size
int mgl_data_read(HMDT d, const char *fname)	{	return d->Read(fname);	}
/// Read data from tab-separated text file with built-in sizes
int mgl_data_read_mat(HMDT d, const char *fname, int dim)
{	return d->ReadMat(fname,dim);	}
/// Read data from text file with specifeid size
int mgl_data_read_dim(HMDT d, const char *fname,int mx,int my,int mz)
{	return d->Read(fname,mx,my,mz);	}
/// Save whole data array (for ns=-1) or only ns-th slice to text file
void mgl_data_save(HMDT d, const char *fname,int ns)
{	d->Save(fname,ns);	}
const mreal *mgl_data_data(const HMDT d)
{	return d->a;	}
/// Get the data which is tensor multiplication (d[i,j] = a[i]*b[j] and so on)
HMDT mgl_data_combine(HMDT a, HMDT b)
{	return new mglData(a->Combine(*b));	}
/// Extend data dimensions
void mgl_data_extend(HMDT dat, int n1, int n2)
{	dat->Extend(n1,n2);	}
//-----------------------------------------------------------------------------
void mgl_data_set_vector(HMDT dat, gsl_vector *v)
{	dat->Set(v);	}
void mgl_data_set_matrix(HMDT dat, gsl_matrix *m)
{	dat->Set(m);	}
//-----------------------------------------------------------------------------
//		Data operations (Fortran)
//-----------------------------------------------------------------------------
/// Multiplicate the data by other one for each element
void mgl_data_mul_dat_(uintptr_t *d, uintptr_t *b){	_DM_(d) *= _D_(b);	}
/// Divide the data by other one for each element
void mgl_data_div_dat_(uintptr_t *d, uintptr_t *b) {	_DM_(d) /= _D_(b);	}
/// Add the other data
void mgl_data_add_dat_(uintptr_t *d, uintptr_t *b) {	_DM_(d) += _D_(b);	}
/// Substract the other data
void mgl_data_sub_dat_(uintptr_t *d, uintptr_t *b) {	_DM_(d) -= _D_(b);	}
/// Multiplicate each element by the number
void mgl_data_mul_num_(uintptr_t *d, mreal *b) {	_DM_(d) *= *b;	}
/// Divide each element by the number
void mgl_data_div_num_(uintptr_t *d, mreal *b) {	_DM_(d) /= *b;	}
/// Add the number
void mgl_data_add_num_(uintptr_t *d, mreal *b) {	_DM_(d) += *b;	}
/// Substract the number
void mgl_data_sub_num_(uintptr_t *d, mreal *b) {	_DM_(d) -= *b;	}
//-----------------------------------------------------------------------------
/// Rearrange data dimensions
void mgl_data_rearrange_(uintptr_t *d, int *mx, int *my, int *mz)
{	_DT_->Rearrange(*mx,*my,*mz);	}
/// Resize the data to new size of box [x1,x2]*[y1,y2]*[z1,z2]
uintptr_t mgl_data_resize_(uintptr_t *d, int *mx,int *my,int *mz)
{	return uintptr_t(new mglData(_DT_->Resize(*mx,*my,*mz)));	}
uintptr_t mgl_data_resize_box_(uintptr_t *d, int *mx,int *my,int *mz,mreal *x1,mreal *x2,
						mreal *y1,mreal *y2,mreal *z1,mreal *z2)
{	return uintptr_t(new mglData(_DT_->Resize(*mx,*my,*mz,*x1,*x2,*y1,*y2,*z1,*z2)));	}
/// Get sub-array of the data with given fixed indexes
uintptr_t mgl_data_subdata_(uintptr_t *d, int *xx,int *yy,int *zz)
{	return uintptr_t(new mglData(_DT_->SubData(*xx,*yy,*zz)));	}
uintptr_t mgl_data_subdata_ext_(uintptr_t *d, uintptr_t *xx, uintptr_t *yy, uintptr_t *zz)
{	return uintptr_t(new mglData(_DT_->SubData(_D_(xx),_D_(yy),_D_(zz))));	}
/// Get column (or slice) of the data filled by formulas of other named columns
uintptr_t mgl_data_column_(uintptr_t *d, const char *eq,int l)
{
	char *s=new char[l+1];	memcpy(s,eq,l);	s[l]=0;
	uintptr_t r = uintptr_t(new mglData(_DT_->Column(s)));
	delete []s;	return r;
}
/// Set names for columns (slices)
void mgl_data_set_id_(uintptr_t *d, const char *eq,int l)
{
	char *s=new char[l+1];	memcpy(s,eq,l);	s[l]=0;
	_DT_->SetColumnId(s);	delete []s;
}
/// Eqidistantly fill the data to range [x1,x2] in direction \a dir
void mgl_data_fill_(uintptr_t *d, mreal *x1,mreal *x2,const char *dir,int)
{	_DT_->Fill(*x1,*x2,*dir);	}
/// Modify the data by specified formula
void mgl_data_modify_(uintptr_t *d, const char *eq,int *dim,int l)
{
	char *s=new char[l+1];	memcpy(s,eq,l);	s[l]=0;
	_DT_->Modify(s,*dim);	delete []s;
}
/// Modify the data by specified formula
void mgl_data_modify_vw_(uintptr_t *d, const char *eq, uintptr_t *v, uintptr_t *w,int l)
{
	char *s=new char[l+1];	memcpy(s,eq,l);	s[l]=0;
	if(d&&v&&w)		_DT_->Modify(s,_D_(v),_D_(w));
	else if(d&&v)	_DT_->Modify(s,_D_(v));
	else if(d)		_DT_->Modify(s);
	delete []s;
}
/// Create or recreate the array with specified size and fill it by zero
void mgl_data_create_(uintptr_t *d, int *nx,int *ny,int *nz)
{	_DT_->Create(*nx,*ny,*nz);	}
/// Transpose the x<->y dimensions of the data
void mgl_data_transpose_(uintptr_t *d, const char *dim,int l)
{
	char *s=new char[l+1];	memcpy(s,dim,l);	s[l]=0;
	_DT_->Transpose(s);
	delete []s;
}
/// Normalize the data to range [v1,v2]
void mgl_data_norm_(uintptr_t *d, mreal *v1,mreal *v2,int *sym,int *dim)
{	_DT_->Norm(*v1,*v2,*sym,*dim);	}
/// Normalize the data to range [v1,v2]
void mgl_data_norm_slice_(uintptr_t *d, mreal *v1,mreal *v2,char *dir,int *keep_en,int *sym,int )
{	_DT_->NormSl(*v1,*v2,*dir,*keep_en,*sym);	}
/// Reduce size of the data
void mgl_data_squeeze_(uintptr_t *d, int *rx,int *ry,int *rz,int *smooth)
{	_DT_->Squeeze(*rx,*ry,*rz,*smooth);	}
/// Get maximal value of the data
mreal mgl_data_max_(uintptr_t *d)	{	return _DT_->Maximal();	}
/// Get minimal value of the data
mreal mgl_data_min_(uintptr_t *d)	{	return _DT_->Minimal();	}
/// Get the value in given cell of the data with border checking
//mreal mgl_data_value_(uintptr_t *d, int *i,int *j,int *k)	{	return d->a[*i+d->nx*(*j+d->ny**k)];	}
/// Swap left and right part of the data in given direction (useful for fourier spectrums)
void mgl_data_swap_(uintptr_t *d, const char *dir,int l)
{
	char *s=new char[l+1];	memcpy(s,dir,l);	s[l]=0;
	_DT_->Swap(s);
	delete []s;
}
void mgl_data_roll_(uintptr_t *d, const char *dir, int *num, int)
{	_DT_->Roll(*dir, *num);	}
/// Mirror the data in given direction
void mgl_data_mirror_(uintptr_t *d, const char *dir,int l)
{
	char *s=new char[l+1];	memcpy(s,dir,l);	s[l]=0;
	_DT_->Mirror(s);
	delete []s;
}
void mgl_data_fill_sample_(uintptr_t *d, int num, const char *how,int l)
{
	char *s=new char[l+1];	memcpy(s,how,l);	s[l]=0;
	_DT_->FillSample(num,s);
	delete []s;
}
void mgl_data_insert_(uintptr_t *d, const char *dir, int *at, int *num, int)
{	_DT_->Insert(*dir,*at,*num);	}
void mgl_data_delete_(uintptr_t *d, const char *dir, int *at, int *num, int)
{	_DT_->Delete(*dir,*at,*num);	}
void mgl_data_hankel_(uintptr_t *d, const char *dir,int l)
{
	char *s=new char[l+1];	memcpy(s,dir,l);	s[l]=0;
	_DT_->Hankel(s);	delete []s;
}
void mgl_data_sinfft_(uintptr_t *d, const char *dir,int l)
{
	char *s=new char[l+1];	memcpy(s,dir,l);	s[l]=0;
	_DT_->SinFFT(s);	delete []s;
}
void mgl_data_cosfft_(uintptr_t *d, const char *dir,int l)
{
	char *s=new char[l+1];	memcpy(s,dir,l);	s[l]=0;
	_DT_->CosFFT(s);	delete []s;
}
//-----------------------------------------------------------------------------
/// Allocate memory and copy the data from the (mreal *) array
void mgl_data_set_float1_(uintptr_t *d, const float *A,int *NX)
{	_DT_->Set(A,*NX);	}
/// Allocate memory and copy the data from the (mreal *) array
void mgl_data_set_double1_(uintptr_t *d, const double *A,int *NX)
{	_DT_->Set(A,*NX);	}
/// Allocate memory and copy the data from the (mreal *) array
void mgl_data_set_float_(uintptr_t *d, const float *A,int *NX,int *NY,int *NZ)
{	_DT_->Set(A,*NX,*NY,*NZ);	}
/// Allocate memory and copy the data from the (mreal *) array
void mgl_data_set_double_(uintptr_t *d, const double *A,int *NX,int *NY,int *NZ)
{	_DT_->Set(A,*NX,*NY,*NZ);	}
/// Allocate memory and copy the data from the (mreal **) array
void mgl_data_set_float2_(uintptr_t *d, const float *A,int *N1,int *N2)
{	_DT_->Set(A,*N1,*N2);	}
/// Allocate memory and copy the data from the (mreal **) array
void mgl_data_set_double2_(uintptr_t *d, const double *A,int *N1,int *N2)
{	_DT_->Set(A,*N1,*N2);	}
/// Allocate memory and copy the data from the (mreal ***) array
void mgl_data_set_float3_(uintptr_t *d, const float *A,int *N1,int *N2,int *N3)
{	_DT_->Set(A,*N1,*N2,*N3);	}
/// Allocate memory and copy the data from the (mreal ***) array
void mgl_data_set_double3_(uintptr_t *d, const double *A,int *N1,int *N2,int *N3)
{	_DT_->Set(A,*N1,*N2,*N3);	}
/// Copy the data from other mglData variable
void mgl_data_set_(uintptr_t *d, uintptr_t *a)	{	_DT_->Set(_D_(a));	}
/// Read data from tab-separated text file with auto determining size
int mgl_data_read_(uintptr_t *d, const char *fname,int l)
{
	char *s=new char[l+1];		memcpy(s,fname,l);	s[l]=0;
	int r = _DT_->Read(s);	delete []s;			return r;
}
/// Read data from tab-separated text file with buit-in sizes
int mgl_data_read_mat_(uintptr_t *d, const char *fname,int *dim,int l)
{
	char *s=new char[l+1];		memcpy(s,fname,l);	s[l]=0;
	int r = _DT_->Read(s,*dim);	delete []s;			return r;
}
/// Read data from text file with specifeid size
int mgl_data_read_dim_(uintptr_t *d, const char *fname,int *mx,int *my,int *mz,int l)
{
	char *s=new char[l+1];	memcpy(s,fname,l);	s[l]=0;
	int r = _DT_->Read(s,*mx,*my,*mz);
	delete []s;			return r;
}
/// Save whole data array (for ns=-1) or only ns-th slice to text file
void mgl_data_save_(uintptr_t *d, const char *fname,int *ns,int l)
{
	char *s=new char[l+1];	memcpy(s,fname,l);	s[l]=0;
	_DT_->Save(s,*ns);		delete []s;
}
const mreal *mgl_data_data_(uintptr_t *d)
{	return _DT_->a;	}
/// Get the data which is tensor multiplication (d[i,j] = a[i]*b[j] and so on)
uintptr_t mgl_data_combine_(uintptr_t *a, uintptr_t *b)
{	return uintptr_t(new mglData(((mglData *)*a)->Combine(_D_(b))));	}
/// Extend data dimensions
void mgl_data_extend_(uintptr_t *d, int *n1, int *n2)
{	_DT_->Extend(*n1,*n2);	}
//-----------------------------------------------------------------------------
//		Data manipulation functions
//-----------------------------------------------------------------------------
/// Smooth the data on specified direction or directions
void mgl_data_smooth(HMDT d, int Type,mreal delta,const char *dirs)
{	d->Smooth(Type,dirs,delta);	}
/// Get array which is result of summation in given direction or directions
HMDT mgl_data_sum(const HMDT d, const char *dir)
{	return new mglData(d->Sum(dir));	}
/// Integrate (cumulative summation) the data in given direction or directions
void mgl_data_integral(HMDT d, const char *dir)
{	d->Integral(dir);	}
/// Differentiate the data in given direction or directions
void mgl_data_diff(HMDT d, const char *dir)
{	d->Diff(dir);	}
/// Double-differentiate (like laplace operator) the data in given direction
void mgl_data_diff2(HMDT d, const char *dir)
{	d->Diff2(dir);	}
/// Interpolate by qubic splain the data to given point x=[0...nx-1], y=[0...ny-1], z=[0...nz-1]
mreal mgl_data_spline(const HMDT d, mreal x,mreal y,mreal z)
{	return d->Spline(x,y,z);	}
/// Interpolate by qubic splain the data to given point \a x,\a y,\a z which normalized in range [0, 1]
mreal mgl_data_spline1(const HMDT d, mreal x,mreal y,mreal z)
{	return d->Spline1(x,y,z);	}
/// Interpolate by linear function the data to given point x=[0...nx-1], y=[0...ny-1], z=[0...nz-1]
mreal mgl_data_linear(const HMDT d, mreal x,mreal y,mreal z)
{	return d->Linear(x,y,z);	}
/// Interpolate by line the data to given point \a x,\a y,\a z which normalized in range [0, 1]
mreal mgl_data_linear1(const HMDT d, mreal x,mreal y,mreal z)
{	return d->Linear1(x,y,z);	}
HMDT mgl_data_momentum(const HMDT d, char dir, const char *how)
{	return new mglData(d->Momentum(dir,how));	}
/// Create n-th points distribution of this data values in range [v1, v2]
HMDT mgl_data_hist(const HMDT d, int n, mreal v1, mreal v2, int nsub)
{	return new mglData(d->Hist(n,v1,v2,nsub));	}
/// Create n-th points distribution of this data values in range [v1, v2] with weight \a w
HMDT mgl_data_hist_w(const HMDT d, HMDT w, int n, mreal v1, mreal v2, int nsub)
{	return new mglData(d->Hist(*w,n,v1,v2,nsub));	}
/// Cumulative summation the data in given direction or directions
void mgl_data_cumsum(HMDT d, const char *dir)
{	d->CumSum(dir);	}
/// Crop the data (cut off data edges)
void mgl_data_crop(HMDT d, int n1, int n2, char dir)
{	d->Crop(n1,n2,dir);	}
/// Get array which is result of maximal values in given direction or directions
HMDT mgl_data_max_dir(const HMDT d, const char *dir)
{	return new mglData(d->Max(dir));	}
/// Get array which is result of minimal values in given direction or directions
HMDT mgl_data_min_dir(const HMDT d, const char *dir)
{	return new mglData(d->Min(dir));	}
//-----------------------------------------------------------------------------
//		Data manipulation functions (Fortran)
//-----------------------------------------------------------------------------
/// Smooth the data on specified direction or directions
void mgl_data_smooth_(uintptr_t *d, int *Type,mreal *delta,const char *dir,int l)
{
	char *s=new char[l+1];	memcpy(s,dir,l);	s[l]=0;
	_DT_->Smooth(*Type,s,*delta);				delete []s;
}
/// Get array which is result of summation in given direction or directions
uintptr_t mgl_data_sum_(uintptr_t *d, const char *dir,int l)
{
	char *s=new char[l+1];	memcpy(s,dir,l);	s[l]=0;
	uintptr_t r=uintptr_t(new mglData(_DT_->Sum(s)));		delete []s;
	return r;
}
/// Integrate (cumulative summation) the data in given direction or directions
void mgl_data_integral_(uintptr_t *d, const char *dir,int l)
{
	char *s=new char[l+1];	memcpy(s,dir,l);	s[l]=0;
	_DT_->Integral(s);		delete []s;
}
/// Differentiate the data in given direction or directions
void mgl_data_diff_(uintptr_t *d, const char *dir,int l)
{
	char *s=new char[l+1];	memcpy(s,dir,l);	s[l]=0;
	_DT_->Diff(s);			delete []s;
}
/// Double-differentiate (like laplace operator) the data in given direction
void mgl_data_diff2_(uintptr_t *d, const char *dir,int l)
{
	char *s=new char[l+1];	memcpy(s,dir,l);	s[l]=0;
	_DT_->Diff2(s);			delete []s;
}
/// Interpolate by qubic splain the data to given point x=[0...nx-1], y=[0...ny-1], z=[0...nz-1]
mreal mgl_data_spline_(uintptr_t *d, mreal *x,mreal *y,mreal *z)
{	return _DT_->Spline(*x,*y,*z);	}
/// Interpolate by qubic splain the data to given point \a x,\a y,\a z which normalized in range [0, 1]
mreal mgl_data_spline1_(uintptr_t *d, mreal *x,mreal *y,mreal *z)
{	return _DT_->Spline1(*x,*y,*z);	}
/// Interpolate by linear function the data to given point x=[0...nx-1], y=[0...ny-1], z=[0...nz-1]
mreal mgl_data_linear_(uintptr_t *d, mreal *x,mreal *y,mreal *z)
{	return _DT_->Linear(*x,*y,*z);	}
/// Interpolate by line the data to given point \a x,\a y,\a z which normalized in range [0, 1]
mreal mgl_data_linear1_(uintptr_t *d, mreal *x,mreal *y,mreal *z)
{	return _DT_->Linear1(*x,*y,*z);	}
uintptr_t mgl_data_momentum_(uintptr_t *d, char *dir, const char *how, int,int l)
{
	char *s=new char[l+1];	memcpy(s,how,l);	s[l]=0;
	uintptr_t r=uintptr_t(new mglData(_DT_->Momentum(*dir, s)));
	delete []s;	return r;
}
/// Create n-th points distribution of this data values in range [v1, v2]
uintptr_t mgl_data_hist_(uintptr_t *d, int *n, mreal *v1, mreal *v2, int *nsub)
{	return uintptr_t(new mglData(_DT_->Hist(*n,*v1,*v2,*nsub)));	}
/// Create n-th points distribution of this data values in range [v1, v2] with weight \a w
uintptr_t mgl_data_hist_w_(uintptr_t *d, uintptr_t *w, int *n, mreal *v1, mreal *v2, int *nsub)
{	return uintptr_t(new mglData(_DT_->Hist(*((mglData *)*w),*n,*v1,*v2,*nsub)));	}
/// Cumulative summation the data in given direction or directions
void mgl_data_cumsum_(uintptr_t *d, const char *dir,int l)
{
	char *s=new char[l+1];	memcpy(s,dir,l);	s[l]=0;
	_DT_->CumSum(s);		delete []s;
}
/// Crop the data (cut off data edges)
void mgl_data_crop_(uintptr_t *d, int *n1, int *n2, const char *dir,int)
{	_DT_->Crop(*n1,*n2,*dir);	}
/// Get array which is result of maximal values in given direction or directions
uintptr_t mgl_data_max_dir_(uintptr_t *d, const char *dir,int l)
{
	char *s=new char[l+1];	memcpy(s,dir,l);	s[l]=0;
	uintptr_t r = uintptr_t(new mglData(_DT_->Max(s)));	delete []s;
	return r;
}
/// Get array which is result of minimal values in given direction or directions
uintptr_t mgl_data_min_dir_(uintptr_t *d, const char *dir,int l)
{
	char *s=new char[l+1];	memcpy(s,dir,l);	s[l]=0;
	uintptr_t r = uintptr_t(new mglData(_DT_->Min(s)));	delete []s;
	return r;
}
//-----------------------------------------------------------------------------
/// Set array values from the string
void mgl_data_set_values(HMDT d, const char *val, int nx, int ny, int nz)
{
	d->Set(val,nx,ny,nz);
}
void mgl_data_set_values_(uintptr_t *d, const char *val, int *nx, int *ny, int *nz, int l)
{
	char *s=new char[l+1];	memcpy(s,val,l);	s[l]=0;
	_DT_->Set(s,*nx,*ny,*nz);	delete []s;
}
//-----------------------------------------------------------------------------
HMDT mgl_data_evaluate_i(const HMDT dat, const HMDT idat, int norm)
{	return new mglData(dat->Evaluate(*idat,norm));	}
HMDT mgl_data_evaluate_ij(const HMDT dat, const HMDT idat, const HMDT jdat, int norm)
{	return new mglData(dat->Evaluate(*idat,*jdat,norm));	}
HMDT mgl_data_evaluate_ijk(const HMDT dat, const HMDT idat, const HMDT jdat, const HMDT kdat, int norm)
{	return new mglData(dat->Evaluate(*idat,*jdat,*kdat,norm));	}
void mgl_data_envelop(HMDT dat, char dir)	{	dat->Envelop(dir);	}
void mgl_data_sew(HMDT dat, const char *dirs, mreal da)
{	dat->Sew(dirs,da);	}
//-----------------------------------------------------------------------------
uintptr_t mgl_data_evaluate_i_(uintptr_t *d, uintptr_t *idat, int *norm)
{	return uintptr_t(new mglData(_DT_->Evaluate(_D_(idat),*norm)));	}
uintptr_t mgl_data_evaluate_ij_(uintptr_t *d, uintptr_t *idat, uintptr_t *jdat, int *norm)
{	return uintptr_t(new mglData(_DT_->Evaluate(_D_(idat),_D_(jdat),*norm)));	}
uintptr_t mgl_data_evaluate_ijk_(uintptr_t *d, uintptr_t *idat, uintptr_t *jdat, uintptr_t *kdat, int *norm)
{	return uintptr_t(new mglData(_DT_->Evaluate(_D_(idat),_D_(jdat),_D_(kdat),*norm)));	}
void mgl_data_envelop_(uintptr_t *d, const char *dir, int)
{	_DT_->Envelop(*dir);	}
void mgl_data_sew_(uintptr_t *d, const char *dirs, mreal *da, int l)
{
	char *s=new char[l+1];	memcpy(s,dirs,l);	s[l]=0;
	_DT_->Sew(s,*da);	delete []s;
}
//-----------------------------------------------------------------------------
void mgl_data_put_val(HMDT dat, mreal val, int i, int j, int k)
{	dat->Put(val, i,j,k);	}
void mgl_data_put_dat(HMDT dat, const HMDT val, int i, int j, int k)
{	dat->Put(*val, i,j,k);	}
void mgl_data_put_val_(uintptr_t *d, mreal *val, int *i, int *j, int *k)
{	_DT_->Put(*val, *i,*j,*k);	}
void mgl_data_put_dat_(uintptr_t *d, uintptr_t *val, int *i, int *j, int *k)
{	_DT_->Put(_D_(val), *i,*j,*k);	}
//-----------------------------------------------------------------------------
int mgl_data_get_nx(const HMDT dat)	{	return	dat->nx;	}
int mgl_data_get_ny(const HMDT dat)	{	return	dat->ny;	}
int mgl_data_get_nz(const HMDT dat)	{	return	dat->nz;	}
int mgl_data_get_nx_(uintptr_t *d)	{	return	_DT_->nx;	}
int mgl_data_get_ny_(uintptr_t *d)	{	return	_DT_->ny;	}
int mgl_data_get_nz_(uintptr_t *d)	{	return	_DT_->nz;	}
//-----------------------------------------------------------------------------
HMDT mgl_transform_a(const HMDT am, const HMDT ph, const char *tr)
{	return new mglData(mglTransformA(*am,*ph,tr));	}
HMDT mgl_transform(const HMDT re, const HMDT im, const char *tr)
{	return new mglData(mglTransform(*re,*im,tr));	}
HMDT mgl_data_stfa(const HMDT re, const HMDT im, int dn, char dir)
{	return new mglData(mglSTFA(*re,*im,dn,dir));	}
uintptr_t mgl_transform_a_(uintptr_t *am, uintptr_t *ph, const char *tr, int l)
{
	char *s=new char[l+1];	memcpy(s,tr,l);	s[l]=0;
	uintptr_t res = uintptr_t(new mglData(mglTransformA(_D_(am),_D_(ph),s)));
	delete []s;		return res;
}
uintptr_t mgl_transform_(uintptr_t *re, uintptr_t *im, const char *tr, int l)
{
	char *s=new char[l+1];	memcpy(s,tr,l);	s[l]=0;
	uintptr_t res = uintptr_t(new mglData(mglTransform(_D_(re),_D_(im),s)));
	delete []s;		return res;
}
uintptr_t mgl_data_stfa_(uintptr_t *re, uintptr_t *im, int *dn, char *dir, int)
{	return uintptr_t(new mglData(mglSTFA(*re,*im,*dn,*dir)));	}
//-----------------------------------------------------------------------------
void mgl_data_diff_par(HMDT d, const HMDT v1, const HMDT v2, const HMDT v3)
{	if(v3)	d->Diff(*v1,*v2,*v3);	else	d->Diff(*v1,*v2);	}
void mgl_data_diff_par_(uintptr_t *d, uintptr_t *v1, uintptr_t *v2, uintptr_t *v3)
{
	if(v3)	_DT_->Diff(_D_(v1),_D_(v2),_D_(v3));
	else	_DT_->Diff(_D_(v1),_D_(v2));
}
//-----------------------------------------------------------------------------
mreal mgl_data_first(const HMDT d, const char *cond, int *i, int *j, int *k)
{	return d->Find(cond,*i,*j,*k);	}
mreal mgl_data_last(const HMDT d, const char *cond, int *i, int *j, int *k)
{	return d->Last(cond,*i,*j,*k);	}
int mgl_data_find(const HMDT d, const char *cond, char dir, int i, int j, int k)
{	return d->Find(cond,dir,i,j,k);	}
int mgl_data_find_any(const HMDT d, const char *cond)
{	return d->FindAny(cond);	}
mreal mgl_data_first_(uintptr_t *d, const char *cond, int *i, int *j, int *k, int l)
{
	char *s=new char[l+1];	memcpy(s,cond,l);	s[l]=0;
	mreal res = _DT_->Find(s,*i,*j,*k);
	delete []s;		return res;
}
mreal mgl_data_last_(uintptr_t *d, const char *cond, int *i, int *j, int *k, int l)
{
	char *s=new char[l+1];	memcpy(s,cond,l);	s[l]=0;
	mreal res = _DT_->Last(s,*i,*j,*k);
	delete []s;		return res;
}
int mgl_data_find_(uintptr_t *d, const char *cond, char *dir, int *i, int *j, int *k, int l, int)
{
	char *s=new char[l+1];	memcpy(s,cond,l);	s[l]=0;
	int res = _DT_->Find(s,*dir,*i,*j,*k);
	delete []s;		return res;
}
int mgl_data_find_any_(uintptr_t *d, const char *cond, int l)
{
	char *s=new char[l+1];	memcpy(s,cond,l);	s[l]=0;
	int res = _DT_->FindAny(s);
	delete []s;		return res;
}
//-----------------------------------------------------------------------------
mreal mgl_data_max_int(const HMDT d, int *i, int *j, int *k)
{	return d->Maximal(*i,*j,*k);	}
mreal mgl_data_max_real(const HMDT d, mreal *i, mreal *j, mreal *k)
{	return d->Maximal(*i,*j,*k);	}
mreal mgl_data_min_int(const HMDT d, int *i, int *j, int *k)
{	return d->Minimal(*i,*j,*k);	}
mreal mgl_data_min_real(const HMDT d, mreal *i, mreal *j, mreal *k)
{	return d->Minimal(*i,*j,*k);	}
mreal mgl_data_momentum_mw(const HMDT d, char dir, mreal *m, mreal *w)
{	return d->Momentum(dir,*m,*w);	}
mreal mgl_data_max_int_(uintptr_t *d, int *i, int *j, int *k)
{	return _DT_->Maximal(*i,*j,*k);	}
mreal mgl_data_max_real_(uintptr_t *d, mreal *i, mreal *j, mreal *k)
{	return _DT_->Maximal(*i,*j,*k);	}
mreal mgl_data_min_int_(uintptr_t *d, int *i, int *j, int *k)
{	return _DT_->Minimal(*i,*j,*k);	}
mreal mgl_data_min_real_(uintptr_t *d, mreal *i, mreal *j, mreal *k)
{	return _DT_->Minimal(*i,*j,*k);	}
mreal mgl_data_momentum_mw_(uintptr_t *d, char *dir, mreal *m, mreal *w,int)
{	return _DT_->Momentum(*dir,*m,*w);	}
//-----------------------------------------------------------------------------
