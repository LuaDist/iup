/***************************************************************************
 * mgl_data.h is part of Math Graphic Library
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
#ifdef _MGL_W_H_
#warning "MathGL wrapper was enabled. So disable original MathGL classes"
#else
#ifndef _MGL_DATA_H_
#define _MGL_DATA_H_
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <vector>
//-----------------------------------------------------------------------------
#ifndef NO_GSL
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#else
struct gsl_vector;
struct gsl_matrix;
#endif
#include "mgl/mgl_define.h"
//-----------------------------------------------------------------------------
/// Class for incapsulating point in space
struct mglPoint
{
	mreal x,y,z;
 	mglPoint(mreal X=0,mreal Y=0,mreal Z=0){x=X;y=Y;z=Z;};
};
inline mglPoint operator+(const mglPoint &a, const mglPoint &b)
{	return mglPoint(a.x+b.x, a.y+b.y, a.z+b.z);	};
inline mglPoint operator-(const mglPoint &a, const mglPoint &b)
{	return mglPoint(a.x-b.x, a.y-b.y, a.z-b.z);	};
inline mglPoint operator*(mreal b, const mglPoint &a)
{	return mglPoint(a.x*b, a.y*b, a.z*b);	};
inline mglPoint operator*(const mglPoint &a, mreal b)
{	return mglPoint(a.x*b, a.y*b, a.z*b);	};
inline mglPoint operator/(const mglPoint &a, mreal b)
{	return mglPoint(a.x/b, a.y/b, a.z/b);	};
inline mreal operator*(const mglPoint &a, const mglPoint &b)
{	return a.x*b.x+a.y*b.y+a.z*b.z;	};
inline mglPoint operator&(const mglPoint &a, const mglPoint &b)
{	return a - b*((a*b)/(b*b));	};
inline mglPoint operator|(const mglPoint &a, const mglPoint &b)
{	return b*((a*b)/(b*b));	};
inline mglPoint operator^(const mglPoint &a, const mglPoint &b)
{	return mglPoint(a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x);	};
inline mglPoint operator!(const mglPoint &a)
{	return (a.x==0 && a.y==0)?mglPoint(1,0,0):mglPoint(-a.y/(mreal)hypot(a.x,a.y), a.x/(mreal)hypot(a.x,a.y), 0);	};
inline bool operator==(const mglPoint &a, const mglPoint &b)
{	return !memcmp(&a, &b, sizeof(mglPoint));	}
inline bool operator!=(const mglPoint &a, const mglPoint &b)
{	return memcmp(&a, &b, sizeof(mglPoint))!=0;	}
inline mreal Norm(const mglPoint &p)
{	return sqrt(p.x*p.x+p.y*p.y+p.z*p.z);	};
//-----------------------------------------------------------------------------
/// Class for working with data array
class mglData
{
protected:
	/// Read data array from HDF4 file
	void ReadHDF4(const char *fname,const char *data);
public:

	long nx;		///< number of points in 1st dimensions ('x' dimension)
	long ny;		///< number of points in 2nd dimensions ('y' dimension)
	long nz;		///< number of points in 3d dimensions ('z' dimension)
	mreal *a;		///< data array
	char *id;		///< column (or slice) names

	/// Initiate by other mglData variable
	mglData(const mglData &d)			{	a=0;	Set(d);		};
	/// Initiate by flat array
	mglData(int size, const float *d)	{	a=0;	Set(d,size);	};
	mglData(int rows, int cols, const float *d)	{	a=0;	Set(d,cols,rows);	};
	mglData(int size, const double *d)	{	a=0;	Set(d,size);	};
	mglData(int rows, int cols, const double *d)	{	a=0;	Set(d,cols,rows);	};
	mglData(const double *d, int size)	{	a=0;	Set(d,size);	};
	mglData(const double *d, int rows, int cols)	{	a=0;	Set(d,cols,rows);	};
	/// Read data from file
	mglData(const char *fname)			{	a=0;	Read(fname);	}
	/// Allocate the memory for data array and initialize it zero
	mglData(int xx=1,int yy=1,int zz=1)	{	a=0;	Create(xx,yy,zz);	};
	/// Delete the array
	virtual ~mglData()	{	if(a)	{	delete []a;	delete []id;	}	};
	// ~~~~~~~~~~~~~~~~~~~~ операции ~~~~~~~~~~~~~~~~~~~~
	// Get sizes
	inline long GetNx()	{	return nx;	};
	inline long GetNy()	{	return ny;	};
	inline long GetNz()	{	return nz;	};

	/// Allocate memory and copy the data from the gsl_vector
	void Set(gsl_vector *v);
	/// Allocate memory and copy the data from the gsl_matrix
	void Set(gsl_matrix *m);
	/// Allocate memory and copy the data from the (float *) array
	void Set(const float *A,int NX,int NY=1,int NZ=1);
	/// Allocate memory and copy the data from the (double *) array
	void Set(const double *A,int NX,int NY=1,int NZ=1);
	/// Allocate memory and copy the data from the (float **) array
	void Set(const float **A,int N1,int N2);
	/// Allocate memory and copy the data from the (double **) array
	void Set(const double **A,int N1,int N2);
	/// Allocate memory and copy the data from the (float ***) array
	void Set(const float ***A,int N1,int N2,int N3);
	/// Allocate memory and copy the data from the (double ***) array
	void Set(const double ***A,int N1,int N2,int N3);
	/// Allocate memory and scanf the data from the string
	void Set(const char *str,int NX,int NY=1,int NZ=1);
	/// Allocate memory and copy data from std::vector<T>
	void Set(const std::vector<int> &d);
	void Set(const std::vector<float> &d);
	void Set(const std::vector<double> &d);
	/// Copy the data from other mglData variable
	inline void Set(const mglData &dat)	{	Set(dat.a,dat.nx,dat.ny,dat.nz);	};
	/// Rearange data dimensions
	void Rearrange(int mx, int my=0, int mz=0);

	/// Read data array from HDF file (parse HDF4 and HDF5 files)
	void ReadHDF(const char *fname,const char *data);
	/// Save data to HDF file
	void SaveHDF(const char *fname,const char *data,bool rewrite=false) const;
	/// Put HDF data names into buf as '\t' separated.
	int DatasHDF(const char *fname, char *buf, long size);
	/// Read data from tab-separated text file with auto determining size
	bool Read(const char *fname);
	/// Read data from text file with specifeid size
	bool Read(const char *fname,int mx,int my=1,int mz=1);
	/// Save whole data array (for ns=-1) or only ns-th slice to text file
	void Save(const char *fname,int ns=-1) const;
	/// Export data array (for ns=-1) or only ns-th slice to PNG file according color scheme
	void Export(const char *fname,const char *scheme,mreal v1=0,mreal v2=0,int ns=-1) const;
	/// Import data array from PNG file according color scheme
	void Import(const char *fname,const char *scheme,mreal v1=0,mreal v2=1);
	/// Read data from tab-separated text files with auto determining size which filenames are result of sprintf(fname,templ,t) where t=from:step:to
	bool ReadRange(const char *templ, mreal from, mreal to, mreal step=1.f, bool as_slice=false);
	/// Read data from tab-separated text files with auto determining size which filenames are satisfied to template (like "t_*.dat")
	bool ReadAll(const char *templ, bool as_slice=false);
	/// Read data from text file with size specified at beginning of the file
	bool ReadMat(const char *fname, int dim=2);

	/// Create or recreate the array with specified size and fill it by zero
	void Create(int nx,int ny=1,int nz=1);
	/// Extend data dimensions
	void Extend(int n1, int n2=0);
	/// Transpose dimensions of the data (generalization of Transpose)
	void Transpose(const char *dim="yx");
	/// Normalize the data to range [v1,v2]
	void Norm(mreal v1=0,mreal v2=1,bool sym=false,int dim=0);
	/// Normalize the data to range [v1,v2] slice by slice
	void NormSl(mreal v1=0,mreal v2=1,char dir='z',bool keep_en=true,bool sym=false);
	/// Put value to data element(s)
	void Put(mreal val, int i=-1, int j=-1, int k=-1);
	/// Put array to data element(s)
	void Put(const mglData &val, int i=-1, int j=-1, int k=-1);
	/// Modify the data by specified formula
	void Modify(const char *eq,int dim=0);
	/// Modify the data by specified formula
	void Modify(const char *eq,const mglData &v, const mglData &w);
	/// Modify the data by specified formula
	void Modify(const char *eq,const mglData &v);
	/// Modify the data by specified formula assuming x,y,z in range [r1,r2]
	void Fill(const char *eq, mglPoint r1, mglPoint r2, const mglData *v=0, const mglData *w=0);
	/// Eqidistantly fill the data to range [x1,x2] in direction \a dir
	void Fill(mreal x1,mreal x2,char dir='x');
	/// Fill data by 'x'/'k' samples for Hankel ('h') or Fourier ('f') transform
	void FillSample(int n, const char *how);
	/// Get column (or slice) of the data filled by formulas of other named columns
	mglData Column(const char *eq);
	/// Set names for columns (slices)
	void SetColumnId(const char *ids);
	/// Make new id
	void NewId();
	/// Reduce size of the data
	void Squeeze(int rx,int ry=1,int rz=1,bool smooth=false);
	/// Crop the data
	void Crop(int n1, int n2,char dir='x');

	/// Get maximal value of the data
	mreal Maximal() const;
	/// Get minimal value of the data
	mreal Minimal() const;
	/// Get maximal value of the data and its position
	mreal Maximal(int &i,int &j,int &k) const;
	/// Get minimal value of the data and its position
	mreal Minimal(int &i,int &j,int &k) const;
	/// Get maximal value of the data and its approximated position
	mreal Maximal(mreal &x,mreal &y,mreal &z) const;
	/// Get minimal value of the data and its approximated position
	mreal Minimal(mreal &x,mreal &y,mreal &z) const;
	/// Get "energy" and find first (median) and second (width) momentums of data
	mreal Momentum(char dir,mreal &m,mreal &w) const;
	/// Get "energy and find 4 momentums of data: median, width, skewness, kurtosis
	mreal Momentum(char dir,mreal &m,mreal &w,mreal &s,mreal &k) const;
	/// Get momentum (1D-array) of data along direction 'dir'. String looks like "x1" for median in x-direction, "x2" for width in x-dir and so on.
	mglData Momentum(char dir, const char *how) const;
	/// Print information about the data (sizes and momentum) to string
	void PrintInfo(char *buf, bool all=false) const;
	/// Print information about the data (sizes and momentum) to FILE (for example, stdout)
	void PrintInfo(FILE *fp) const;
	/// Find position (after specified in i,j,k) of first nonzero value of formula
	mreal Find(const char *cond, int &i, int &j, int &k) const;
	/// Find position (before specified in i,j,k) of last nonzero value of formula
	mreal Last(const char *cond, int &i, int &j, int &k) const;
	/// Find position of first in direction 'dir' nonzero value of formula
	int Find(const char *cond, char dir, int i=0, int j=0, int k=0) const;
	/// Find if any nonzero value of formula
	bool FindAny(const char *cond) const;

	/// Smooth the data on specified direction or directions
	void Smooth(int Type,const char *dirs="xyz",mreal delta=0);
	/// Set as the data envelop
	void Envelop(char dir='x');
	/// Remove phase jump
	void Sew(const char *dirs="xyz", mreal da=2*M_PI);
	/// Smooth the data on specified direction or directions
	void Smooth(const char *dirs="xyz");
	/// Get sub-array of the data with given fixed indexes
	mglData SubData(int xx,int yy=-1,int zz=-1) const;
	mglData SubData(const mglData &xx, const mglData &yy, const mglData &zz) const;
	/// Get trace of the data array
	mglData Trace() const;
	/// Create n-th points distribution of this data values in range [v1, v2]
	mglData Hist(int n,mreal v1=0,mreal v2=1, int nsub=0) const;
	/// Create n-th points distribution of this data values in range [v1, v2] with weight \a w
	mglData Hist(const mglData &w, int n,mreal v1=0,mreal v2=1, int nsub=0) const;
	/// Get array which is result of summation in given direction or directions
	mglData Sum(const char *dir) const;
	/// Get array which is result of maximal values in given direction or directions
	mglData Max(const char *dir) const;
	/// Get array which is result of minimal values in given direction or directions
	mglData Min(const char *dir) const;
	/// Resize the data to new size of box [x1,x2]*[y1,y2]*[z1,z2]
	mglData Resize(int mx,int my=1,int mz=1,mreal x1=0,mreal x2=1,
		mreal y1=0,mreal y2=1,mreal z1=0,mreal z2=1) const;
	/// Get the data which is direct multiplication (like, d[i,j] = this[i]*a[j] and so on)
	mglData Combine(const mglData &a) const;
	/// Get array which values is result of interpolation this for coordinates from other arrays
	mglData Evaluate(const mglData &idat, bool norm=true) const;
	mglData Evaluate(const mglData &idat, const mglData &jdat, bool norm=true) const;
	mglData Evaluate(const mglData &idat, const mglData &jdat, const mglData &kdat, bool norm=true) const;

	/// Cumulative summation the data in given direction or directions
	void CumSum(const char *dir);
	/// Integrate (cumulative summation) the data in given direction or directions
	void Integral(const char *dir);
	/// Differentiate the data in given direction or directions
	void Diff(const char *dir);
	/// Differentiate the parametrically specified data along direction v1 with v2=const
	void Diff(const mglData &v1, const mglData &v2);
	/// Differentiate the parametrically specified data along direction v1 with v2,v3=const
	void Diff(const mglData &v1, const mglData &v2, const mglData &v3);
	/// Double-differentiate (like laplace operator) the data in given direction
	void Diff2(const char *dir);
	/// Swap left and right part of the data in given direction (useful for fourier spectrums)
	void Swap(const char *dir);
	/// Roll data along direction \a dir by \a num slices
	void Roll(char dir, int num);
	/// Mirror the data in given direction (useful for fourier spectrums)
	void Mirror(const char *dir);

	/// Hankel transform
	void Hankel(const char *dir);
	/// Sin-Fourier transform
	void SinFFT(const char *dir);
	/// Cos-Fourier transform
	void CosFFT(const char *dir);

	/// Interpolate by 5-th order splain the data to given point \a x,\a y,\a z which normalized in range [0, 1] and evaluate its derivatives
	mreal Spline5(mreal x,mreal y,mreal z,mreal &dx,mreal &dy,mreal &dz) const;
	/// Interpolate by qubic splain the data to given point x=[0...nx-1], y=[0...ny-1], z=[0...nz-1]
	mreal Spline(mreal x,mreal y=0,mreal z=0) const;
	mreal Spline3(mreal x,mreal y,mreal z,mreal &dx,mreal &dy,mreal &dz) const;
	/// Interpolate by qubic splain the data to given point \a x,\a y,\a z which normalized in range [0, 1]
	inline mreal Spline1(mreal x,mreal y=0,mreal z=0) const
	{	return Spline(x*(nx-1),y*(ny-1),z*(nz-1));	};
	/// Interpolate by linear function the data to given point x=[0...nx-1], y=[0...ny-1], z=[0...nz-1]
	mreal Linear(mreal x,mreal y=0,mreal z=0) const;
	/// Interpolate by line the data to given point \a x,\a y,\a z which normalized in range [0, 1]
	inline mreal Linear1(mreal x,mreal y=0,mreal z=0) const
	{	return Linear(x*(nx-1),y*(ny-1),z*(nz-1));	};

	/// Insert \a num rows after \a at and fill it by formula \a eq
	void InsertRows(int at, int num=1, const char *eq=0);
	/// Insert \a num columns after \a at and fill it by formula \a eq
	void InsertColumns(int at, int num=1, const char *eq=0);
	/// Insert \a num slices after \a at and fill it by formula \a eq
	void InsertSlices(int at, int num=1, const char *eq=0);
	/// Delete \a num rows starting from \a at
	void DeleteRows(int at, int num=1);
	/// Delete \a num rows starting from \a at
	void DeleteColumns(int at, int num=1);
	/// Delete \a num rows starting from \a at
	void DeleteSlices(int at, int num=1);
	/// Insert data
	void Insert(char dir, int at=0, int num=1);
	/// Delete data
	void Delete(char dir, int at=0, int num=1);

	/// Get the value in given cell of the data with border checking
	mreal v(int i,int j=0,int k=0) const;
	/// Copy data from other mglData variable
	void operator=(const mglData &d)	{	Set(d.a,d.nx,d.ny,d.nz);	};
	void operator=(mreal v);
	/// Multiplicate the data by other one for each element
	void operator*=(const mglData &d);
	/// Divide the data by other one for each element
	void operator/=(const mglData &d);
	/// Add the other data
	void operator+=(const mglData &d);
	/// Substract the other data
	void operator-=(const mglData &d);
	/// Multiplicate each element by the number
	void operator*=(mreal d);
	/// Divide each element by the number
	void operator/=(mreal d);
	/// Add the number
	void operator+=(mreal d);
	/// Substract the number
	void operator-=(mreal d);
};
//-----------------------------------------------------------------------------
mglData operator*(const mglData &b, const mglData &d);
mglData operator*(mreal b, const mglData &d);
mglData operator*(const mglData &d, mreal b);
mglData operator-(const mglData &b, const mglData &d);
mglData operator-(mreal b, const mglData &d);
mglData operator-(const mglData &d, mreal b);
mglData operator+(const mglData &b, const mglData &d);
mglData operator+(mreal b, const mglData &d);
mglData operator+(const mglData &d, mreal b);
mglData operator/(const mglData &b, const mglData &d);
mglData operator/(const mglData &d, mreal b);
//-----------------------------------------------------------------------------
void mgl_srnd(long seed);
double mgl_rnd();
double mgl_ipow(double x,int n);
/// Integral data transformation (like Fourier 'f' or 'i', Hankel 'h' or None 'n') for amplitude and phase
mglData mglTransformA(const mglData &am, const mglData &ph, const char *tr);
/// Integral data transformation (like Fourier 'f' or 'i', Hankel 'h' or None 'n') for mreal and imaginary parts
mglData mglTransform(const mglData &re, const mglData &im, const char *tr);
/// Apply Fourier transform for the data and save result into it
void mglFourier(mglData &re, mglData &im, const char *dir);
/// Short time fourier analysis for mreal and imaginary parts. Output is amplitude of partial fourier (result will have size {dn, floor(nx/dn), ny} for dir='x'
mglData mglSTFA(const mglData &re, const mglData &im, int dn, char dir='x');
/// Saves result of PDE solving (|u|^2) for "Hamiltonian" \a ham with initial conditions \a ini
mglData mglPDE(const char *ham, const mglData &ini_re, const mglData &ini_im, mglPoint Min, mglPoint Max, mreal dz=0.1, mreal k0=100);
/// Saves result of PDE solving for "Hamiltonian" \a ham with initial conditions \a ini along a curve \a ray (must have nx>=7 - x,y,z,px,py,pz,tau or nx=5 - x,y,px,py,tau)
mglData mglQO2d(const char *ham, const mglData &ini_re, const mglData &ini_im, const mglData &ray, mreal r=1, mreal k0=100, mglData *xx=0, mglData *yy=0, bool UseR=true);
/// Saves result of PDE solving in aberration-free approxiamtion for "Hamiltonian" \a ham with initial conditions \a ini along a curve \a ray (must have nx>=7 - x,y,z,px,py,pz,tau or nx=5 - x,y,px,py,tau)
mglData mglAF2d(const char *ham, const mglData &ini_re, const mglData &ini_im, const mglData &ray, mreal r=1, mreal k0=100, mglData *xx=0, mglData *yy=0, bool UseR=true);
/// Prepares ray data for mglQO_PDE with starting point \a r0, \a p0
mglData mglRay(const char *ham, mglPoint r0, mglPoint p0, mreal dt=0.1, mreal tmax=10);
/// Calculate Jacobian determinant for D{x(u,v), y(u,v)} = dx/du*dy/dv-dx/dv*dy/du
mglData mglJacobian(const mglData &x, const mglData &y);
/// Calculate Jacobian determinant for D{x(u,v,w), y(u,v,w), z(u,v,w)}
mglData mglJacobian(const mglData &x, const mglData &y, const mglData &z);
// Do something like Delone triangulation
mglData mglTriangulation(const mglData &x, const mglData &y, const mglData &z, mreal er);
mglData mglTriangulation(const mglData &x, const mglData &y, mreal er);
//-----------------------------------------------------------------------------
#endif
#endif
