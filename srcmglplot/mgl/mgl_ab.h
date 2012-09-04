/***************************************************************************
 * mgl_ab.h is part of Math Graphic Library
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
//-----------------------------------------------------------------------------
#ifdef _MGL_W_H_
#warning "MathGL wrapper was enabled. So disable original MathGL classes"
#else
#ifndef _MGL_AB_H_
#define _MGL_AB_H_
#include "mgl/mgl.h"
#ifndef MGL_STACK_ENTRY
#define MGL_STACK_ENTRY	10
#endif
//-----------------------------------------------------------------------------
/// Class implement the creation of different mathematical plots using Z-ordering
class mglGraphAB : public mglGraph
{
public:
using mglGraph::Mark;
using mglGraph::Ball;
//using mglGraph::Colorbar;
using mglGraph::Legend;
	int *OI;	///< ObjId arrays
	int ObjId;	///< object id for mglPrim

	/// Initialize ZBuffer drawing and allocate the memory for image with size [Width x Height].
	mglGraphAB(int w=600, int h=400);
	virtual ~mglGraphAB();
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ��������� ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	virtual void WriteEPS(const char *fname,const char *descr=0);
	virtual void WriteSVG(const char *fname,const char *descr=0);
//	virtual int NewFrame();
//	virtual void EndFrame();

	void SetFontSizePT(mreal pt, int dpi=72);
	void DefColor(mglColor c, mreal alpha=-1);
	bool Alpha(bool enable);
	bool Light(bool enable);
	void Light(int n, bool enable);
	void Light(int n,mglPoint p, mglColor c=WC, mreal br=0.5, bool infty=true, mreal ap=0);

	virtual void InPlot(mreal x1,mreal x2,mreal y1,mreal y2, bool rel=false);
	virtual void DefaultPlotParam();	///< Set default parameter for plotting
	virtual void Rotate(mreal TetX,mreal TetZ,mreal TetY=0);
	void StickPlot(int num, int i, mreal tet, mreal phi);
	void Aspect(mreal Ax,mreal Ay,mreal Az);
	void RotateN(mreal Tet,mreal x,mreal y,mreal z);
	void Perspective(mreal a);

	virtual void SetSize(int w,int h);

	void Mark(mreal x,mreal y,mreal z,char mark='.');
	void Ball(mreal x,mreal y,mreal z,mglColor col=RC,mreal alpha=1);
	void Glyph(mreal x, mreal y, mreal f, int style, long icode, char col);
	mreal GetRatio()	{	return B1[0]/B1[4];	};
	void Putsw(mglPoint p,const wchar_t *text,const char *font=0,mreal size=-1,char dir=0,mreal shift=0);
	mreal Putsw(mglPoint p,mglPoint l,const wchar_t *text,char font='t',mreal size=-1);
	void Pen(mglColor col, char style,mreal width);
	void Legend(int n, wchar_t **text, char **style, mreal x, mreal y, const char *font="rL", mreal size=-0.8, mreal llen=0.1);

//	void Colorbar(int where, mreal x, mreal y, mreal w, mreal h);
//	void Colorbar(const mglData &v, const char *sch, int where, mreal x, mreal y, mreal w, mreal h);
	/// Get RGB bitmap of current state image.
	virtual const unsigned char *GetBits();
	/// Get RGBA bitmap of current state image.
	const unsigned char *GetRGBA();
	/// Get width of the image
	int GetWidth()	{	return Width;	};
	/// Get height of the image
	int GetHeight()	{	return Height;	};
	/// Set allowed drawing region (for multithreading, like subplots)
	void SetDrawReg(int m, int n, int k);
	/// Put drawing from other mglGraphZB (for multithreading, like subplots)
	virtual void PutDrawReg(int m, int n, int k, mglGraphAB *gr);
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	/** @name Widget functions
	  * These functions control window behaviar in classes mglGraphFLTK, mglGraphQT
	  * and so on. They do nothing in "non-visual" classes like mglGraphZB,
	  * mglGraphPS, mglGraphGL, mglGraphIDTF. */
	//@{
	bool AutoClf;		///< Clear canvas between drawing
	mreal Delay;		///< Delay for animation in seconds
	bool ShowMousePos;	///< Switch to show or not mouse click position
	mglPoint LastMousePos;	///< Last mouse position
	bool ClfOnUpdate;	///< Clear plot before Update()
	/// Calculate 3D coordinate {x,y,z} for screen point {xs,ys}
	mglPoint CalcXYZ(int xs, int ys);
	/// Calculate screen point {xs,ys} for 3D coordinate {x,y,z}
	void CalcScr(mglPoint p, int *xs, int *ys);
	mglPoint CalcScr(mglPoint p);
	/// Switch on/off transparency (do not overwrite switches in user drawing function)
	virtual void ToggleAlpha();
	/// Switch on/off lighting (do not overwrite switches in user drawing function)
	virtual void ToggleLight();
	virtual void ToggleZoom();	///< Switch on/off zooming by mouse
	virtual void ToggleRotate();///< Switch on/off rotation by mouse
	virtual void ToggleNo();	///< Switch off all zooming and rotation
	virtual void Update();		///< Update picture by calling user drawing function
	virtual void ReLoad(bool o);///< Reload user data and update picture
	virtual void Adjust();		///< Adjust size of bitmap to window size
	virtual void NextFrame();	///< Show next frame (if one)
	virtual void PrevFrame();	///< Show previous frame (if one)
	virtual void Animation();	///< Run slideshow (animation) of frames
	/// Create a window for plotting based on callback function (can be NULL).
	virtual void Window(int argc, char **argv, int (*draw)(mglGraph *gr, void *p),
						const char *title, void *par=NULL,
						void (*reload)(int next, void *p)=NULL, bool maximize=false);
	/// Create a window for plotting based on class mglDraw.
	void Window(int argc, char **argv, const char *title, mglDraw *draw, bool maximize=false);
	/// Push transformation matrix into stack
	virtual void Push();
	/// Pop transformation matrix from stack
	virtual void Pop();
	/// Set diagonal matrix and its shift
	void SetPosScale(mreal xp, mreal yp, mreal zp, mreal scl=1);
	//@}
protected:
	unsigned char *G4;		///< Final picture in RGBA format. Prepared after calling mglGraphZB::Finish().
	unsigned char *G;		///< Final picture in RGB format. Prepared after calling mglGraphZB::Finish().
	mreal CDef[4];			///< Default color
	unsigned char BDef[4];	///< Background color
	mreal Persp;		///< Perspective factor (=0 is perspective off)
	mreal xPos;			///< Shifting plot in X-direction (used by PostScale() function)
	mreal yPos;			///< Shifting plot in Y-direction (used by PostScale() function)
	mreal zPos;			///< Shifting plot in depth (used by PostScale() function)
	int Width;			///< Width of the image
	int Height;			///< Height of the image
	int Depth;			///< Depth of the image
	mreal B[9];			///< Transformation matrix (used by PostScale() function)
	mreal B1[12];		///< Transformation matrix for colorbar
	mreal inW, inH;		///< Relative width and height of last InPlot
	unsigned PDef;		///< Pen bit mask
	mreal pPos;			///< Current position in pen mask
	bool UseLight;		///< Flag of using lightning
	bool nLight[10];	///< Availability of light sources
	bool iLight[10];	///< Infinity/local position of light sources
	mreal rLight[30];	///< Position of light sources
	mreal pLight[30];	///< Actual position of light sources (filled by LightScale() function)
	mreal aLight[10];	///< Aperture of light sources
	mreal bLight[10];	///< Brightness of light sources
	mreal cLight[30];	///< Color of light sources
	bool Finished;		///< Flag that final picture \a mglGraphZB::G is ready
	mreal PenWidth;		///< Pen width for further line plotting (must be >0 !!!)
	bool NoAutoFactor;	///< Temporary variable
	mreal f_size;		///< font size for glyph lines
	mreal fscl,ftet;	///< last scale and rotation for glyphs
	int nx1, nx2, ny1, ny2;		// Allowed drawing region

	int NumFig;			///< Number of figures in the list. If 0 then no list and mglGraph::DrawFunc will called for each drawing.
	void (*LoadFunc)(int next, void *par);
	void *FuncPar;		///< Parameters for drawing function mglGraph::DrawFunc.
	/// Drawing function for window procedure. It should return the number of frames.
	int (*DrawFunc)(mglGraph *gr, void *par);

	unsigned char **GetRGBLines(long &w, long &h, unsigned char *&f, bool solid=true);
	/// Additionally scale points \a p (array with length 3*n) for positioning in image
	void PostScale(mreal *p,long n);
	/// Additionally scale positions of light sources
	virtual void LightScale();
	/// Additionally scale normals \a s (array with length 3*n)
	void NormScale(mreal *s,long n);
	void colorbar(const mglData &v, const mglColor *s, int where, mreal x, mreal y, mreal w, mreal h);

	/// Plot point \a p with color \a c
	virtual void pnt_plot(long x,long y,mreal z,unsigned char c[4]);
	/// Transform mglColor and alpha value to bits format
	unsigned char* col2int(mglColor c, mreal alpha,unsigned char *r);	// mglColor -> int
	/// Transform mreal color and alpha to bits format
	unsigned char* col2int(mreal *c,mreal *n,unsigned char *r);
	/// Draw line between points \a p1,\a p2 with color \a c1, \a c2 at edges
	virtual void line_plot(mreal *p1,mreal *p2,mreal *c1,mreal *c2,bool all=false);
	virtual void line_draw(mreal *p1,mreal *p2,mreal *c1,mreal *c2,bool all=false);
	/// Draws the point (ball) at position \a p with color \a c.
	virtual void ball(mreal *p,mreal *c);

	/// Draw triangle between points \a p0,\a p1,\a p2 with color \a c0, \a c1, \a c2 at edges
	virtual void trig_plot(mreal *p0,mreal *p1,mreal *p2,
		mreal *c0,mreal *c1,mreal *c2);
	virtual void trig_draw(mreal *p0,mreal *p1,mreal *p2,
		mreal *c0,mreal *c1,mreal *c2);
	/// Draw triangle between points \a p0,\a p1,\a p2 with color \a c0, \a c1, \a c2 at edges
	virtual void trig_plot_n(mreal *p0,mreal *p1,mreal *p2,
	    mreal *c0,mreal *c1,mreal *c2,
	    mreal *n0,mreal *n1,mreal *n2);
	/// Draw face of points \a p0,\a p1,\a p2,\a p3 with color \a c0, \a c1, \a c2, \a c3 at edges
	virtual void quad_plot(mreal *p0,mreal *p1,mreal *p2,mreal *p3,
					mreal *c0,mreal *c1,mreal *c2,mreal *c3);
	virtual void quad_draw(mreal *p0,mreal *p1,mreal *p2,mreal *p3,
					mreal *c0,mreal *c1,mreal *c2,mreal *c3);
	/// Draw face of points \a p0,\a p1,\a p2,\a p3 with values \a a0, \a a1, \a a2, \a a3 at edges
	virtual void quad_plot_a(mreal *p0,mreal *p1,mreal *p2,mreal *p3,
					mreal a0,mreal a1,mreal a2,mreal a3,mreal alpha);
	/// Draw face of points \a p0,\a p1,\a p2,\a p3 with color \a c0, \a c1, \a c2, \a c3 at edges
	virtual void quad_plot_n(mreal *p0,mreal *p1,mreal *p2,mreal *p3,
					mreal *c0,mreal *c1,mreal *c2,mreal *c3,
					mreal *n0,mreal *n1,mreal *n2,mreal *n3);
	/// Draw mark at position \a pp with style \a type
	virtual void mark_plot(mreal *pp, char type);
	/// Combine colors in 2 plane.
	void combine(unsigned char *c1,unsigned char *c2);	// �������� ������
	virtual void cloud_plot(long nx,long ny,long nz,mreal *pp,mreal *a,mreal alpha);
	// ��������� ���������� ��� mglGraph
	virtual void arrow_plot(mreal *p1,mreal *p2,char st);
	void curv_plot(long n,mreal *pp,bool *tt);
	void curv_plot(long n,mreal *pp,bool *tt,long *nn);
	void curv_plot(long n,mreal *pp,mreal *cc,bool *tt);
	void mesh_plot(long n,long m,mreal *pp,mreal *cc,bool *tt,int how);
	void wire_plot(long n,long m,mreal *pp,mreal *cc,bool *tt);
	void surf_plot(long n,long m,mreal *pp,mreal *cc,bool *tt);
	void axial_plot(long n,mreal *pp,long *nn,long np, bool wire);
	void boxs_plot(long n, long m, mreal *pp, mglColor *cc,
				bool *tt,mreal Alpha, bool line);
	void surf3_plot(long n,long m,long *kx1,long *kx2,long *ky1,long *ky2,long *kz,
							mreal *pp,mreal *cc,mreal *kk,mreal *nn,bool wire);
	void trigs_plot(long n, long *nn, long m, mreal *pp, mreal *cc, bool *tt,bool wire, bool bytrig=false);
	void quads_plot(long n, long *nn, long m, mreal *pp, mreal *cc, bool *tt,bool wire, bool byquad);
	void quads_plot(long n, mreal *pp, mreal *cc, bool *tt);
	void lines_plot(long n, mreal *pp, mreal *cc, bool *tt, bool ball, bool grad);
	void vects_plot(long n, mreal *pp, mreal *cc, bool *tt, bool grad);
	void glyph_fill(mreal x,mreal y, mreal f, int nt, const short *trig, mreal *c);
	void glyph_wire(mreal x,mreal y, mreal f, int nl, const short *line, mreal *c);
	void glyph_line(mreal x,mreal y, mreal f, mreal *c, bool solid);
	void FindOptOrg(mreal ax[3], mreal ay[3], mreal az[3]);
	mreal GetOrgX(char dir);
	mreal GetOrgY(char dir);
	mreal GetOrgZ(char dir);
	void DrawTick(mreal *pp,bool sub);
private:
	mreal stack[MGL_STACK_ENTRY*13];	// stack for transformation matrixes
	int st_pos;
};
//-----------------------------------------------------------------------------
#endif
#endif
//-----------------------------------------------------------------------------
