/***************************************************************************
 * mgl_f.cpp is part of Math Graphic Library
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
#include "mgl/mgl_f.h"
#include "mgl/mgl.h"
#include "mgl/mgl_ab.h"
//-----------------------------------------------------------------------------
/// Delete mglGraph object. MUST be called for each mgl_create_* call.
void mgl_delete_graph_(uintptr_t *gr)
{	if(*gr)	delete (mglGraph *)(*gr);	}
//-----------------------------------------------------------------------------
/// Create mglData object.
uintptr_t mgl_create_data_()
{	return uintptr_t(new mglData());	};
/// Create mglData object with specified sizes.
uintptr_t mgl_create_data_size_(int *nx, int *ny, int *nz)
{	return uintptr_t(new mglData(*nx,*ny,*nz));	};
/// Create mglData object from data file.
uintptr_t mgl_create_data_file_(const char *fname,int l)
{
	char *s=new char[l+1];	memcpy(s,fname,l);	s[l]=0;
	uintptr_t r = uintptr_t(new mglData(s));	delete []s;
	return r;
}
/// Delete mglData object. MUST be called for each mgl_create_data_* call.
void mgl_delete_data_(uintptr_t *dat)
{	if(*dat)	delete (mglData *)(*dat);	};
//-----------------------------------------------------------------------------
//		Setup mglGraph
//-----------------------------------------------------------------------------
/// Set color in mglGraph::Pal array at index \a n.
void mgl_set_palette_(uintptr_t *gr, const char *colors, int l)
{
	char *s=new char[l+1];	memcpy(s,colors,l);	s[l]=0;
	if(gr)	_GR_->SetPalette(s);
	delete []s;
}
/// Set color in mglGraph::Pal array at index \a n.
void mgl_set_pal_color_(uintptr_t *gr, int *n, mreal *r, mreal *g, mreal *b)
{	if(gr && *n<100)	_GR_->Pal[*n] = mglColor(*r,*g,*b);	}
/// Set number of colors in mglGraph::Pal array.
void mgl_set_pal_num_(uintptr_t *gr, int *num)
{	if(gr && *num<100)	_GR_->NumPal = *num;	}
/// Set mglGraph::RotatedText.
void mgl_set_rotated_text_(uintptr_t *gr, int *rotated)
{	_GR_->RotatedText = *rotated;	}
/// Set mglGraph::Cut.
void mgl_set_cut_(uintptr_t *gr, int *cut)
{	_GR_->Cut = *cut;	}
/// Set mglGraph::CutMin, mglGraph::CutMax.
void mgl_set_cut_box_(uintptr_t *gr, mreal *x1,mreal *y1,mreal *z1,mreal *x2,mreal *y2,mreal *z2)
{	_GR_->CutMin = mglPoint(*x1,*y1,*z1);	_GR_->CutMax = mglPoint(*x2,*y2,*z2);	}
/// Set type of transparency mglGraph::TranspType.
void mgl_set_transp_type_(uintptr_t *gr, int *type)
{	_GR_->TranspType = *type;	}
/// Switch on/off transparency mglGraph::Transparent.
void mgl_set_transp_(uintptr_t *gr, int *enable)
{	_GR_->Transparent = *enable;	}
/// Set width of rectangles in mglGraph::Bars().
void mgl_set_bar_width_(uintptr_t *gr, mreal *width)
{	_GR_->BarWidth = *width;	}
/// Set size of marks mglGraph::BaseLineWidth.
void mgl_set_base_line_width_(uintptr_t *gr, mreal *size)
{	_GR_->BaseLineWidth = *size;	}
/// Set size of marks mglGraph::MarkSize.
void mgl_set_mark_size_(uintptr_t *gr, mreal *size)
{	_GR_->MarkSize = *size;	}
/// Set size of arrows mglGraph::ArrowSize.
void mgl_set_arrow_size_(uintptr_t *gr, mreal *size)
{	_GR_->ArrowSize = *size;	}
/// Set default font size mglGraph::FontSize.
void mgl_set_font_size_(uintptr_t *gr, mreal *size)
{	_GR_->FontSize = *size;	}
/// Set default alpha value mglGraph::AlphaDef.
void mgl_set_alpha_default_(uintptr_t *gr, mreal *alpha)
{	_GR_->AlphaDef = *alpha;	}
/// Set size of frame in pixels. Normally this function is called internaly.
void mgl_set_size_(uintptr_t *gr, int *width, int *height)
{	_GR_->SetSize(*width,*height);	}
void mgl_set_axial_dir_(uintptr_t *gr, const char *dir, int)
{	_GR_->AxialDir = *dir;	}
void mgl_set_meshnum_(uintptr_t *gr, int *num)
{	_GR_->MeshNum = *num;	}
void mgl_set_draw_face_(uintptr_t *gr, int *enable)
{	_GR_->DrawFace = *enable;	}
void mgl_set_scheme_(uintptr_t *gr, char *sch, int l)
{
	char *s;
	if(sch && sch[0])
	{	s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;	}
	else
	{	s=new char[7];		strcpy(s,"BbcyrR");			}
	_GR_->SetScheme(s);		delete []s;
}
/// Set font facename
void mgl_load_font_(uintptr_t *gr, char *name, char *path, int l,int n)
{
	char *s=new char[l+1];		memcpy(s,name,l);	s[l]=0;
	char *d=new char[n+1];		memcpy(d,path,n);	d[n]=0;
	_GR_->GetFont()->Load(s,d);	delete []s;		delete []d;
}
/// Copy font data from another HMGL object
void mgl_copy_font_(uintptr_t *gr, uintptr_t *gr_from)
{	_GR_->GetFont()->Copy(((mglGraph *)(*gr_from))->GetFont());	}
/// Restore font data
void mgl_restore_font_(uintptr_t *gr)
{	_GR_->GetFont()->Restore();	}
//-----------------------------------------------------------------------------
//		Export to file
//-----------------------------------------------------------------------------
/// Write the frame in file using PostScript format
void mgl_write_eps_(uintptr_t *gr, const char *fname,const char *descr,int l,int n)
{
	char *s=new char[l+1];	memcpy(s,fname,l);	s[l]=0;
	char *d=new char[n+1];	memcpy(d,descr,n);	d[n]=0;
	_GR_->WriteEPS(s,d);	delete []s;		delete []d;
}
/// Write the frame in file using ScalableVectorGraphics format
void mgl_write_svg_(uintptr_t *gr, const char *fname,const char *descr,int l,int n)
{
	char *s=new char[l+1];	memcpy(s,fname,l);	s[l]=0;
	char *d=new char[n+1];	memcpy(d,descr,n);	d[n]=0;
	_GR_->WriteSVG(s,d);	delete []s;		delete []d;
}
void mgl_show_image_(uintptr_t *gr, const char *viewer, int *keep, int l)
{
	char *s=new char[l+1];	memcpy(s,viewer,l);	s[l]=0;
	_GR_->ShowImage(s,*keep);	delete []s;
}
//-----------------------------------------------------------------------------
//		Setup frames transparency (alpha) and lightning
//-----------------------------------------------------------------------------
/// Create new frame.
int mgl_new_frame_(uintptr_t *gr)
{	return _GR_->NewFrame();	}
/// Finish frame drawing
void mgl_end_frame_(uintptr_t *gr)
{	_GR_->EndFrame();	}
/// Get the number of created frames
int mgl_get_num_frame_(uintptr_t *gr)
{	return _GR_->GetNumFrame();	}
/// Reset frames counter
void mgl_reset_frames_(uintptr_t *gr)
{	_GR_->ResetFrames();	}
/// Set the transparency on/off.
void mgl_set_alpha_(uintptr_t *gr, int *enable)
{	_GR_->Alpha(*enable);	}
/// Set the fog on/off.
void mgl_set_fog_(uintptr_t *gr, mreal *d, mreal *dz)
{	_GR_->Fog(*d, *dz);	}
/// Set the using of light on/off.
void mgl_set_light_(uintptr_t *gr, int *enable)
{	_GR_->Light(*enable);	}
void mgl_set_light_n_(uintptr_t *gr, int *n, int *enable)
{	_GR_->Light(*n, *enable);	}
/// Add white light source.
void mgl_add_light_(uintptr_t *gr, int *n, mreal *x, mreal *y, mreal *z, char *c, int)
{	_GR_->Light(*n,mglPoint(*x,*y,*z),*c);	}
/// Add a light source with color {r,g,b}.
void mgl_add_light_rgb_(uintptr_t *gr, int *n, mreal *x, mreal *y, mreal *z, int *infty,
						mreal *r, mreal *g, mreal *b,mreal *i)
{	_GR_->Light(*n,mglPoint(*x,*y,*z),mglColor(*r,*g,*b),*i,*infty);	}
// Set ambient light brightness
void mgl_set_ambbr_(uintptr_t *gr, mreal *i)
{	_GR_->Ambient(*i);	}
//-----------------------------------------------------------------------------
//		Scale and rotate
//-----------------------------------------------------------------------------
/// Push transformation matrix into stack.
void mgl_mat_push_(uintptr_t *gr)	{	_GR_->Push();	}
/// Pop transformation matrix into stack.
void mgl_mat_pop_(uintptr_t *gr)	{	_GR_->Pop();	}
/// Clear transformation matrix.
void mgl_identity_(uintptr_t *gr, int *rel)	{	_GR_->Identity(*rel);	}
/// Clear up the frame
void mgl_clf_(uintptr_t *gr)
{	_GR_->Clf();	}
/// Clear up the frame
void mgl_clf_rgb_(uintptr_t *gr, mreal *r, mreal *g, mreal *b)
{	_GR_->Clf(mglColor(*r,*g,*b));	}
/// Put further plotting in some region of whole frame surface.
void mgl_subplot_(uintptr_t *gr, int *nx,int *ny,int *m)
{	_GR_->SubPlot(*nx,*ny,*m);	}
void mgl_subplot_d_(uintptr_t *gr, int *nx,int *ny,int *m,mreal *dx,mreal *dy)
{	_GR_->SubPlot(*nx,*ny,*m,*dx,*dy);	}
void mgl_subplot_s_(uintptr_t *gr, int *nx,int *ny,int *m,const char *st,int l)
{	char *s=new char[l+1];	memcpy(s,st,l);	s[l]=0;
	_GR_->SubPlot(*nx,*ny,*m,s);	delete []s;	}
/// Put further plotting in some region of whole frame surface.
void mgl_inplot_(uintptr_t *gr, mreal *x1,mreal *x2,mreal *y1,mreal *y2)
{	_GR_->InPlot(*x1,*x2,*y1,*y2,false);	}
void mgl_relplot_(uintptr_t *gr, mreal *x1,mreal *x2,mreal *y1,mreal *y2)
{	_GR_->InPlot(*x1,*x2,*y1,*y2,true);	}
void mgl_columnplot_(uintptr_t *gr, int *num, int *i)
{	_GR_->ColumnPlot(*num,*i);	}
void mgl_columnplot_d_(uintptr_t *gr, int *num, int *i, mreal *d)
{	_GR_->ColumnPlot(*num,*i,*d);	}
void mgl_stickplot_(uintptr_t *gr, int *num, int *i, mreal *tet, mreal *phi)
{	_GR_->StickPlot(*num, *i, *tet, *phi);	}
/// Set aspect ratio for further plotting.
void mgl_aspect_(uintptr_t *gr, mreal *Ax,mreal *Ay,mreal *Az)
{	_GR_->Aspect(*Ax,*Ay,*Az);	}
/// Rotate a further plotting.
void mgl_rotate_(uintptr_t *gr, mreal *TetX,mreal *TetZ,mreal *TetY)
{	_GR_->Rotate(*TetX,*TetZ,*TetY);	}
/// Rotate a further plotting around vector {x,y,z}.
void mgl_rotate_vector_(uintptr_t *gr, mreal *Tet,mreal *x,mreal *y,mreal *z)
{	_GR_->RotateN(*Tet,*x,*y,*z);	}
void mgl_perspective_(uintptr_t *gr, mreal val)
{	_GR_->Perspective(val);	}
//-----------------------------------------------------------------------------
//		Axis functions
//-----------------------------------------------------------------------------
/// Auto adjust ticks
void mgl_adjust_ticks_(uintptr_t *gr, const char *dir, int l)
{
	char *s=new char[l+1];	memcpy(s,dir,l);	s[l]=0;
	_GR_->AdjustTicks(s);	delete []s;
}
/// Switch on/off ticks tunning and set factor position for tunned ticks.
void mgl_tune_ticks_(uintptr_t *gr, int *tune, mreal *fact_pos)
{	_GR_->TuneTicks = *tune;	_GR_->FactorPos = *fact_pos;	}
void mgl_set_ticks_dir_(uintptr_t *gr, char *dir, mreal *d, int *ns, mreal *org)
{	_GR_->SetTicks(*dir, *d, *ns, *org);	}
/// Set ticks interval mglGraph::dx, mglGraph::dy, mglGraph::dz.
void mgl_set_ticks_(uintptr_t *gr, mreal *DX, mreal *DY, mreal *DZ)
{	_GR_->dx=*DX;	_GR_->dy=*DY;	_GR_->dz=*DZ;	}
/// Set number of subticks mglGraph::NSx, mglGraph::NSy, mglGraph::NSz.
void mgl_set_subticks_(uintptr_t *gr, int *NX, int *NY, int *NZ)
{	_GR_->NSx=*NX;	_GR_->NSy=*NY;	_GR_->NSz=*NZ;	}
/// Set the values of mglGraph::Cmin and mglGraph::Cmax
void mgl_set_caxis_(uintptr_t *gr, mreal *C1,mreal *C2)
{	_GR_->CAxis(*C1,*C2);	}
void mgl_set_crange_(uintptr_t *gr, uintptr_t *a, int *add)	{	_GR_->CRange(_D_(a),*add);	}
void mgl_set_xrange_(uintptr_t *gr, uintptr_t *a, int *add)	{	_GR_->XRange(_D_(a),*add);	}
void mgl_set_yrange_(uintptr_t *gr, uintptr_t *a, int *add)	{	_GR_->YRange(_D_(a),*add);	}
void mgl_set_zrange_(uintptr_t *gr, uintptr_t *a, int *add)	{	_GR_->ZRange(_D_(a),*add);	}
/// Safetly set the value for mglGraph::Min, mglGraph::Max and mglGraph::Org members of the class.
void mgl_set_axis_(uintptr_t *gr, mreal *x1, mreal *y1, mreal *z1, mreal *x2, mreal *y2, mreal *z2, mreal *x0, mreal *y0, mreal *z0)
{	_GR_->Axis(mglPoint(*x1,*y1,*z1),mglPoint(*x2,*y2,*z2),mglPoint(*x0,*y0,*z0));	}
/// Safetly set the transformation formulas for coordinate.
void mgl_set_func_(uintptr_t *gr, const char *EqX,const char *EqY,const char *EqZ,int lx,int ly,int lz)
{
	char *sx=new char[lx+1];	memcpy(sx,EqX,lx);	sx[lx]=0;
	char *sy=new char[ly+1];	memcpy(sy,EqY,ly);	sy[ly]=0;
	char *sz=new char[lz+1];	memcpy(sz,EqZ,lz);	sz[lz]=0;
	_GR_->SetFunc(sx,sy,sz);
	delete []sx;	delete []sy;	delete []sz;
}
void mgl_set_func_ext_(uintptr_t *gr, const char *EqX,const char *EqY,const char *EqZ,const char *EqA,int lx,int ly,int lz,int la)
{
	char *sx=new char[lx+1];	memcpy(sx,EqX,lx);	sx[lx]=0;
	char *sy=new char[ly+1];	memcpy(sy,EqY,ly);	sy[ly]=0;
	char *sz=new char[lz+1];	memcpy(sz,EqZ,lz);	sz[lz]=0;
	char *sa=new char[la+1];	memcpy(sa,EqA,la);	sa[la]=0;
	_GR_->SetFunc(sx,sy,sz,sa);
	delete []sx;	delete []sy;	delete []sz;	delete []sa;
}
/// Set predefined coordinate system
void mgl_set_coor_(uintptr_t *gr, int *how)
{	_GR_->SetCoor(*how);	}
void mgl_set_cutoff_(uintptr_t *gr, const char *EqC, int l)
{
	char *s=new char[l+1];	memcpy(s,EqC,l);	s[l]=0;
	_GR_->CutOff(s);
	delete []s;
}
/// Draws bounding box outside the plotting volume by default color.
void mgl_box_(uintptr_t *gr, int *ticks)
{	_GR_->Box(NC,*ticks);	}
/// Draws bounding box outside the plotting volume with color \a c.
void mgl_box_str_(uintptr_t *gr, const char *col, int *ticks,int l)
{
	char *s=new char[l+1];	memcpy(s,col,l);	s[l]=0;
	_GR_->Box(s,*ticks);	delete []s;
}
/// Draws bounding box outside the plotting volume.
void mgl_box_rgb_(uintptr_t *gr, mreal *r, mreal *g, mreal *b, int *ticks)
{	_GR_->Box(mglColor(*r,*g,*b),*ticks);	}
/// Draw axises with ticks in directions determined by string parameter \a dir.
void mgl_axis_(uintptr_t *gr, const char *dir,int l)
{	char *s=new char[l+1];	memcpy(s,dir,l);	s[l]=0;	_GR_->Axis(s);	delete []s;	}
/// Draw grid lines perpendicular to direction determined by string parameter \a dir.
void mgl_axis_grid_(uintptr_t *gr, const char *dir,const char *pen,int l,int n)
{
	char *s=new char[l+1];	memcpy(s,dir,l);	s[l]=0;
	char *p=new char[n+1];	memcpy(p,pen,n);	p[n]=0;
	_GR_->Grid(s,p);	delete []s;	delete []p;
}
/// Print the label \a text for axis \a dir.
void mgl_label_(uintptr_t *gr, const char *dir, const char *text,int,int l)
{
	char *s=new char[l+1];	memcpy(s,text,l);	s[l]=0;
	_GR_->Label(*dir, s);	delete []s;
}
/// Print the label \a text for axis \a dir.
void mgl_label_ext_(uintptr_t *gr, const char *dir, const char *text, mreal *pos, mreal *size, mreal *shift,int,int l)
{
	char *s=new char[l+1];	memcpy(s,text,l);	s[l]=0;
	_GR_->Label(*dir, s, *pos, *size, *shift);	delete []s;
}
void mgl_label_xy_(uintptr_t *gr, mreal *x, mreal *y, const char *txt, const char *fnt, mreal *size,int l,int n)
{
	char *s=new char[l+1];	memcpy(s,txt,l);	s[l]=0;
	char *p=new char[n+1];	memcpy(p,fnt,n);	p[n]=0;
	_GR_->Label(*x,*y,s,p,*size);	delete []s;	delete []p;
}
void mgl_set_xtt_(uintptr_t * gr, const char *templ, int l)
{
	char *s=new char[l+1];	memcpy(s,templ,l);	s[l]=0;
	_GR_->SetXTT(s);	delete []s;
}
void mgl_set_ytt_(uintptr_t * gr, const char *templ, int l)
{
	char *s=new char[l+1];	memcpy(s,templ,l);	s[l]=0;
	_GR_->SetYTT(s);	delete []s;
}
void mgl_set_ztt_(uintptr_t * gr, const char *templ, int l)
{
	char *s=new char[l+1];	memcpy(s,templ,l);	s[l]=0;
	_GR_->SetZTT(s);	delete []s;
}
void mgl_set_ctt_(uintptr_t * gr, const char *templ, int l)
{
	char *s=new char[l+1];	memcpy(s,templ,l);	s[l]=0;
	_GR_->SetCTT(s);	delete []s;
}
//-----------------------------------------------------------------------------
//		Simple drawing
//-----------------------------------------------------------------------------
/// Draws the red point (ball) at position \a {x,y,z}.
void mgl_ball_(uintptr_t *gr, mreal *x,mreal *y,mreal *z)
{	_GR_->Ball(mglPoint(*x,*y,*z));	}
/// Draws the point (ball) at position \a {x,y,z} with color {r,g,b}.
void mgl_ball_rgb_(uintptr_t *gr, mreal *x, mreal *y, mreal *z, mreal *r, mreal *g, mreal *b, mreal *alpha)
{	_GR_->Ball(*x,*y,*z,mglColor(*r,*g,*b),*alpha);	}
/// Draws the point (ball) at position \a p with color \a col.
void mgl_ball_str_(uintptr_t *gr, mreal *x, mreal *y, mreal *z, const char *col,int)
{	_GR_->Ball(mglPoint(*x,*y,*z),*col);	}
/// Draws the line between points with style \a sch.
void mgl_line_(uintptr_t *gr, mreal *x1, mreal *y1, mreal *z1, mreal *x2, mreal *y2, mreal *z2,
				const char *pen,int *n,int l)
{
	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	_GR_->Line(mglPoint(*x1,*y1,*z1),mglPoint(*x2,*y2,*z2),s,*n);	delete []s;
}
void mgl_facex_(uintptr_t* gr, mreal *x0, mreal *y0, mreal *z0, mreal *wy, mreal *wz, const char *stl, mreal *dx, mreal *dy, int l)
{
	char *s=new char[l+1];	memcpy(s,stl,l);	s[l]=0;
	_GR_->FaceX(*x0,*y0,*z0,*wy,*wz,s,*dx,*dy);	delete []s;
}
void mgl_facey_(uintptr_t* gr, mreal *x0, mreal *y0, mreal *z0, mreal *wx, mreal *wz, const char *stl, mreal *dx, mreal *dy, int l)
{
	char *s=new char[l+1];	memcpy(s,stl,l);	s[l]=0;
	_GR_->FaceX(*x0,*y0,*z0,*wx,*wz,s,*dx,*dy);	delete []s;
}
void mgl_facez_(uintptr_t* gr, mreal *x0, mreal *y0, mreal *z0, mreal *wx, mreal *wy, const char *stl, mreal *dx, mreal *dy, int l)
{
	char *s=new char[l+1];	memcpy(s,stl,l);	s[l]=0;
	_GR_->FaceX(*x0,*y0,*z0,*wx,*wy,s,*dx,*dy);	delete []s;
}
void mgl_curve_(uintptr_t* gr, mreal *x1, mreal *y1, mreal *z1, mreal *dx1, mreal *dy1, mreal *dz1, mreal *x2, mreal *y2, mreal *z2, mreal *dx2, mreal *dy2, mreal *dz2, const char *pen,int *n, int l)
{
	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	_GR_->Curve(mglPoint(*x1,*y1,*z1), mglPoint(*dx1,*dy1,*dz1), mglPoint(*x2,*y2,*z2), mglPoint(*dx2,*dy2,*dz2), s, *n);	delete []s;
}

/// Print string \a str in position \a p with font size \a size.
void mgl_puts_(uintptr_t *gr, mreal *x, mreal *y, mreal *z,const char *text,int l)
{
	char *s=new char[l+1];	memcpy(s,text,l);	s[l]=0;
	_GR_->Puts(mglPoint(*x,*y,*z),s);	delete []s;
}
/// Print string \a str in position \a p auintptr_t direction \a d with font size \a size.
void mgl_puts_dir_(uintptr_t *gr, mreal *x, mreal *y, mreal *z, mreal *dx, mreal *dy, mreal *dz, const char *text, mreal *size, int l)
{
	char *s=new char[l+1];	memcpy(s,text,l);	s[l]=0;
	_GR_->Puts(mglPoint(*x,*y,*z), mglPoint(*dx,*dy,*dz), s, 't', *size);
	delete []s;
}
/// Print unrotated string \a str in position \a p with font size \a size.
void mgl_text_(uintptr_t *gr, mreal *x, mreal *y, mreal *z,const char *text,int l)
{
	char *s=new char[l+1];	memcpy(s,text,l);	s[l]=0;
	_GR_->Text(mglPoint(*x,*y,*z),s);	delete []s;
}
/// Print string \a str in position \a p with font size \a size.
void mgl_puts_ext_(uintptr_t *gr, mreal *x, mreal *y, mreal *z,const char *text,
					const char *font, mreal *size, const char *dir,int l,int n,int)
{
	char *s=new char[l+1];	memcpy(s,text,l);	s[l]=0;
	char *f=new char[n+1];	memcpy(f,font,n);	f[n]=0;
	_GR_->Puts(mglPoint(*x,*y,*z),s,f,*size,*dir);	delete []s;	delete []f;
}
/// Print unrotated string \a str in position \a p with font size \a size.
void mgl_text_ext_(uintptr_t *gr, mreal *x, mreal *y, mreal *z,const char *text,
					const char *font, mreal *size,const char *dir,int l,int n,int)
{
	char *s=new char[l+1];	memcpy(s,text,l);	s[l]=0;
	char *f=new char[n+1];	memcpy(f,font,n);	f[n]=0;
	_GR_->Text(mglPoint(*x,*y,*z),s,f,*size,*dir);	delete []s;	delete []f;
}
/// Draw colorbar at edge of axis
void mgl_colorbar_(uintptr_t *gr, const char *sch,int *where,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	_GR_->Colorbar(s,*where);	delete []s;
}
void mgl_colorbar_val_(uintptr_t *gr, uintptr_t *v, const char *sch,int *where,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	_GR_->Colorbar(_D_(v), s,*where);	delete []s;
}
void mgl_colorbar_ext_(uintptr_t *gr, const char *sch,int *where, mreal *x, mreal *y, mreal *w, mreal *h, int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	_GR_->Colorbar(s,*where,*x,*y,*w,*h);	delete []s;
}
/// Plot data depending on its dimensions and \a type parameter
void mgl_simple_plot_(uintptr_t *gr, uintptr_t *a, int *type, const char *sch,int l)
{
	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	if(gr && a)	_GR_->SimplePlot(_D_(a),*type,s);
	delete []s;
}
//-----------------------------------------------------------------------------
//		Plot legend drawing
//-----------------------------------------------------------------------------
/// Add string to legend
void mgl_add_legend_(uintptr_t *gr, const char *text,const char *style,int l,int n)
{
	char *s=new char[l+1];	memcpy(s,text,l);	s[l]=0;
	char *f=new char[n+1];	memcpy(f,style,n);	f[n]=0;
	if(gr)	_GR_->AddLegend(s,f);	delete []s;	delete []f;
}
/// Clear saved legend string
void mgl_clear_legend_(uintptr_t *gr)
{	if(gr)	_GR_->ClearLegend();	}
/// Draw legend of accumulated strings at position (x, y) by \a font with \a size
void mgl_legend_xy_(uintptr_t *gr, mreal *x, mreal *y, const char *font, mreal *size, mreal *llen,int l)
{
	char *s=new char[l+1];	memcpy(s,font,l);	s[l]=0;
	if(gr)	_GR_->Legend(*x, *y, s, *size,*llen);		delete []s;
}
/// Draw legend of accumulated strings by \a font with \a size
void mgl_legend_(uintptr_t *gr, int *where, const char *font, mreal *size, mreal *llen,int l)
{
	char *s=new char[l+1];	memcpy(s,font,l);	s[l]=0;
	if(gr)	_GR_->Legend(*where, s, *size,*llen);	delete []s;
}
void mgl_set_legend_box_(uintptr_t *gr, int *enable)
{	_GR_->LegendBox = *enable;	}
void mgl_set_legend_marks_(uintptr_t *gr, int *num)
{	_GR_->SetLegendMarks(*num);	}
//-----------------------------------------------------------------------------
const unsigned char *mgl_get_rgb_(uintptr_t *graph)
{
	mglGraphAB *g = dynamic_cast<mglGraphAB *>((mglGraph *)(*graph));
	return g ? g->GetBits():0;
}
const unsigned char *mgl_get_rgba_(uintptr_t *graph)
{
	mglGraphAB *g = dynamic_cast<mglGraphAB *>((mglGraph *)(*graph));
	return g ? g->GetRGBA():0;
}
int mgl_get_width_(uintptr_t *graph)
{
	mglGraphAB *g = dynamic_cast<mglGraphAB *>((mglGraph *)(*graph));
	return g ? g->GetWidth():0;
}
int mgl_get_height_(uintptr_t *graph)
{
	mglGraphAB *g = dynamic_cast<mglGraphAB *>((mglGraph *)(*graph));
	return g ? g->GetHeight():0;
}
void mgl_set_show_mouse_pos_(uintptr_t *gr, int *enable)
{
	mglGraphAB *g = dynamic_cast<mglGraphAB *>((mglGraph *)(*gr));
	if(g) g->ShowMousePos=*enable;
}
void mgl_get_last_mouse_pos_(uintptr_t *gr, mreal *x, mreal *y, mreal *z)
{
	mglGraphAB *g = dynamic_cast<mglGraphAB *>((mglGraph *)(*gr));
	if(g)
	{	*x=g->LastMousePos.x;	*y=g->LastMousePos.y;	*z=g->LastMousePos.z;}
}
void mgl_calc_xyz_(uintptr_t *gr, int *xs, int *ys, mreal *x, mreal *y, mreal *z)
{
	mglGraphAB *g = dynamic_cast<mglGraphAB *>((mglGraph *)(*gr));
	if(g)
	{
		mglPoint p = g->CalcXYZ(*xs,*ys);
		*x = p.x;	*y = p.y;	*z = p.z;
	}
}
void mgl_calc_scr_(uintptr_t *gr, mreal *x, mreal *y, mreal *z, int *xs, int *ys)
{
	mglGraphAB *g = dynamic_cast<mglGraphAB *>((mglGraph *)(*gr));
	if(g)	g->CalcScr(mglPoint(*x,*y,*z),xs,ys);
}
//-----------------------------------------------------------------------------
void mgl_wnd_set_delay_(uintptr_t *gr, mreal *dt)
{	mglGraphAB *g = dynamic_cast<mglGraphAB *>((mglGraph *)(*gr));
	if(g)	g->Delay = *dt;	}
void mgl_wnd_set_auto_clf_(uintptr_t *gr, int *val)
{	mglGraphAB *g = dynamic_cast<mglGraphAB *>((mglGraph *)(*gr));
	if(g)	g->AutoClf = *val;	}
void mgl_wnd_set_show_mouse_pos_(uintptr_t *gr, int *val)
{	mglGraphAB *g = dynamic_cast<mglGraphAB *>((mglGraph *)(*gr));
	if(g)	g->ShowMousePos = *val;	}
void mgl_wnd_set_clf_update_(uintptr_t *gr, int *val)
{	mglGraphAB *g = dynamic_cast<mglGraphAB *>((mglGraph *)(*gr));
	if(g)	g->ClfOnUpdate = *val;	}
void mgl_wnd_toggle_alpha_(uintptr_t *gr)
{	mglGraphAB *g = dynamic_cast<mglGraphAB *>((mglGraph *)(*gr));
	if(g)	g->ToggleAlpha();	}
void mgl_wnd_toggle_light_(uintptr_t *gr)
{	mglGraphAB *g = dynamic_cast<mglGraphAB *>((mglGraph *)(*gr));
	if(g)	g->ToggleLight();	}
void mgl_wnd_toggle_zoom_(uintptr_t *gr)
{	mglGraphAB *g = dynamic_cast<mglGraphAB *>((mglGraph *)(*gr));
	if(g)	g->ToggleZoom();	}
void mgl_wnd_toggle_rotate_(uintptr_t *gr)
{	mglGraphAB *g = dynamic_cast<mglGraphAB *>((mglGraph *)(*gr));
	if(g)	g->ToggleRotate();	}
void mgl_wnd_toggle_no_(uintptr_t *gr)
{	mglGraphAB *g = dynamic_cast<mglGraphAB *>((mglGraph *)(*gr));
	if(g)	g->ToggleNo();	}
void mgl_wnd_update_(uintptr_t *gr)
{	mglGraphAB *g = dynamic_cast<mglGraphAB *>((mglGraph *)(*gr));
	if(g)	g->Update();	}
void mgl_wnd_reload_(uintptr_t *gr, int *o)
{	mglGraphAB *g = dynamic_cast<mglGraphAB *>((mglGraph *)(*gr));
	if(g)	g->ReLoad(*o);	}
void mgl_wnd_adjust_(uintptr_t *gr)
{	mglGraphAB *g = dynamic_cast<mglGraphAB *>((mglGraph *)(*gr));
	if(g)	g->Adjust();	}
void mgl_wnd_next_frame_(uintptr_t *gr)
{	mglGraphAB *g = dynamic_cast<mglGraphAB *>((mglGraph *)(*gr));
	if(g)	g->NextFrame();	}
void mgl_wnd_prev_frame_(uintptr_t *gr)
{	mglGraphAB *g = dynamic_cast<mglGraphAB *>((mglGraph *)(*gr));
	if(g)	g->PrevFrame();	}
void mgl_wnd_animation_(uintptr_t *gr)
{	mglGraphAB *g = dynamic_cast<mglGraphAB *>((mglGraph *)(*gr));
	if(g)	g->Animation();	}
//-----------------------------------------------------------------------------
mreal mgl_data_get_value_(uintptr_t *d, int *i, int *j, int *k)
{	return	_DT_->a[*i+_DT_->nx*(*j+*k*_DT_->ny)];	}
void mgl_data_set_value_(uintptr_t *d, mreal *v, int *i, int *j, int *k)
{	_DT_->a[*i+_DT_->nx*(*j+*k*_DT_->ny)] = *v;	}
//-----------------------------------------------------------------------------
/// Zoom in/out a part of picture
void mgl_set_zoom_(uintptr_t *gr, mreal *x1, mreal *y1, mreal *x2, mreal *y2)
{	_GR_->Zoom(*x1,*y1,*x2,*y2);	}
void mgl_set_plotfactor_(uintptr_t *gr, mreal *val)
{
	if(*val>0)	{	_GR_->PlotFactor = *val;	_GR_->AutoPlotFactor=false;	}
	else		{	_GR_->PlotFactor = 1.55f;	_GR_->AutoPlotFactor=true;	}
}
void mgl_set_axis_3d_(uintptr_t *gr, mreal *x1, mreal *y1, mreal *z1, mreal *x2, mreal *y2, mreal *z2)
{	_GR_->Axis(mglPoint(*x1,*y1,*z1),mglPoint(*x2,*y2,*z2));	}
void mgl_set_axis_2d_(uintptr_t *gr, mreal *x1, mreal *y1, mreal *x2, mreal *y2)
{	_GR_->Axis(mglPoint(*x1,*y1),mglPoint(*x2,*y2));	}
void mgl_set_origin_(uintptr_t *gr, mreal *x0, mreal *y0, mreal *z0)
{	_GR_->Org = mglPoint(*x0,*y0,*z0);	}
void mgl_set_tick_origin_(uintptr_t *gr, mreal *x0, mreal *y0, mreal *z0)
{	_GR_->OrgT = mglPoint(*x0,*y0,*z0);	}
//-----------------------------------------------------------------------------
void mgl_title_(uintptr_t *gr, const char *text, const char *fnt, mreal *size,int l,int n)
{
	char *s=new char[l+1];	memcpy(s,text,l);	s[l]=0;
	char *f=new char[n+1];	memcpy(f,fnt,n);	f[n]=0;
	if(gr)	_GR_->Title(s,f, *size);
	delete []s;	delete []f;
}
//-----------------------------------------------------------------------------
void mgl_set_ternary_(uintptr_t *gr, int *enable)
{	_GR_->Ternary(*enable);	}
void mgl_sphere_(uintptr_t* gr, mreal *x, mreal *y, mreal *z, mreal *r, const char *stl,int l)
{
	char *s=new char[l+1];	memcpy(s,stl,l);	s[l]=0;
	_GR_->Sphere(mglPoint(*x,*y,*z),*r,s);	delete []s;
}
void mgl_drop_(uintptr_t* gr, mreal *x1, mreal *y1, mreal *z1, mreal *x2, mreal *y2, mreal *z2, mreal *r, const char *stl, mreal *shift, mreal *ap, int l)
{
	char *s=new char[l+1];	memcpy(s,stl,l);	s[l]=0;
	_GR_->Drop(mglPoint(*x1,*y1,*z1),mglPoint(*x2,*y2,*z2),*r,s,*shift,*ap);
	delete []s;
}
void mgl_cone_(uintptr_t* gr, mreal *x1, mreal *y1, mreal *z1, mreal *x2, mreal *y2, mreal *z2, mreal *r1, mreal *r2, const char *stl, int *edge, int l)
{
	char *s=new char[l+1];	memcpy(s,stl,l);	s[l]=0;
	_GR_->Cone(mglPoint(*x1,*y1,*z1),mglPoint(*x2,*y2,*z2),*r1,*r2,s,*edge);
	delete []s;
}
//-----------------------------------------------------------------------------
void mgl_set_def_param_(uintptr_t* gr)	{	_GR_->DefaultPlotParam();	}
void mgl_set_font_def_(uintptr_t *gr, const char *fnt, int l)
{	l=l<31?l:31;	memcpy(_GR_->FontDef, fnt,l);	_GR_->FontDef[l]=0;	}
void mgl_flush_(uintptr_t *gr)	{	_GR_->Flush();	}
void mgl_data_fill_eq_(uintptr_t *gr, uintptr_t *d, const char *eq, uintptr_t *vdat, uintptr_t *wdat, int l)
{
	char *s=new char[l+1];	memcpy(s,eq,l);	s[l]=0;
	_DT_->Fill(s,_GR_->Min,_GR_->Max,((mglData *)*vdat),((mglData *)*wdat));
	delete []s;
}
//-----------------------------------------------------------------------------
void mgl_set_auto_(uintptr_t *gr, mreal *x1, mreal *x2, mreal *y1, mreal *y2, mreal *z1, mreal *z2)
{
	_GR_->SetAutoRanges(*x1,*x2,*y1,*y2,*z1,*z2);
}
void mgl_set_tick_len_(uintptr_t *gr, mreal *len, mreal *stt)
{	_GR_->SetTickLen(*len, *stt);	}
void mgl_set_tick_stl_(uintptr_t *gr, const char *stl, const char *sub, int l, int m)
{
	char *t=new char[l+1];	memcpy(t,stl,l);	t[l]=0;
	char *s=new char[m+1];	memcpy(s,sub,m);	s[m]=0;
	_GR_->SetTickStl(t,s);
	delete []s;	delete []t;
}
int mgl_get_warn_(uintptr_t *gr)	{	return _GR_->WarnCode;	}
//-----------------------------------------------------------------------------
