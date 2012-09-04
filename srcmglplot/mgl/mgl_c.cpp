/***************************************************************************
 * mgl_c.cpp is part of Math Graphic Library
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
#include "mgl/mgl_ab.h"
#include "mgl/mgl_c.h"
//-----------------------------------------------------------------------------
/// Delete mglGraph object. MUST be called for each mgl_create_* call.
void mgl_delete_graph(HMGL gr)
{	if(gr)	delete gr;	}
//-----------------------------------------------------------------------------
/// Create mglData object.
HMDT mgl_create_data()
{	return new mglData();	};
/// Create mglData object with specified sizes.
HMDT mgl_create_data_size(int nx, int ny, int nz)
{	return new mglData(nx,ny,nz);	};
/// Create mglData object from data file.
HMDT mgl_create_data_file(const char *fname)
{	return new mglData(fname);	};
/// Delete mglData object. MUST be called for each mgl_create_data_* call.
void mgl_delete_data(HMDT dat)
{	if(dat)	delete dat;	};
//-----------------------------------------------------------------------------
//		Setup mglGraph
//-----------------------------------------------------------------------------
/// Set color in mglGraph::Pal array at index \a n.
void mgl_set_palette(HMGL gr, const char *colors)
{	if(gr)	gr->SetPalette(colors);	}
/// Set color in mglGraph::Pal array at index \a n.
void mgl_set_pal_color(HMGL gr, int n, mreal r, mreal g, mreal b)
{	if(gr && n<100)	gr->Pal[n] = mglColor(r,g,b);	}
/// Set number of colors in mglGraph::Pal array.
void mgl_set_pal_num(HMGL gr, int num)
{	if(gr && num<100)	gr->NumPal = num;	}
/// Set mglGraph::RotatedText.
void mgl_set_rotated_text(HMGL gr, int rotated)
{	gr->RotatedText = rotated;	}
/// Set mglGraph::Cut.
void mgl_set_cut(HMGL gr, int cut)
{	gr->Cut = cut;	}
/// Set mglGraph::CutMin, mglGraph::CutMax.
void mgl_set_cut_box(HMGL gr, mreal x1,mreal y1,mreal z1,mreal x2,mreal y2,mreal z2)
{	gr->CutMin = mglPoint(x1,y1,z1);	gr->CutMax = mglPoint(x2,y2,z2);	}
/// Set type of transparency mglGraph::TranspType.
void mgl_set_transp_type(HMGL gr, int type)
{	gr->TranspType = type;	}
/// Switch on/off transparency mglGraph::Transparent.
void mgl_set_transp(HMGL gr, int enable)
{	gr->Transparent = enable;	}
/// Set width of rectangles in mglGraph::Bars().
void mgl_set_bar_width(HMGL gr, mreal width)
{	gr->BarWidth = width;	}
/// Set size of marks mglGraph::BaseLineWidth.
void mgl_set_base_line_width(HMGL gr, mreal size)
{	gr->BaseLineWidth = size;	}
/// Set size of marks mglGraph::MarkSize.
void mgl_set_mark_size(HMGL gr, mreal size)
{	gr->MarkSize = size;	}
/// Set size of arrows mglGraph::ArrowSize.
void mgl_set_arrow_size(HMGL gr, mreal size)
{	gr->ArrowSize = size;	}
/// Set default font size mglGraph::FontSize.
void mgl_set_font_size(HMGL gr, mreal size)
{	gr->FontSize = size;	}
/// Set default alpha value mglGraph::AlphaDef.
void mgl_set_alpha_default(HMGL gr, mreal alpha)
{	gr->AlphaDef = alpha;	}
/// Set size of frame in pixels. Normally this function is called internaly.
void mgl_set_size(HMGL gr, int width, int height)
{	gr->SetSize(width,height);	}
/// Set rotation direction
void mgl_set_axial_dir(HMGL gr, char dir)
{	gr->AxialDir = dir;	}
/// Set number of lines in mesh
void mgl_set_meshnum(HMGL gr, int num)
{	gr->MeshNum = num;	}
/// Switch on/off face drawing
void mgl_set_draw_face(HMGL gr, int enable)
{	gr->DrawFace = enable;	}
/// Set color scheme
void mgl_set_scheme(HMGL gr, const char *sch)
{	gr->SetScheme((sch && sch[0]) ? sch : "BbcyrR");	}
/// Set font facename
void mgl_load_font(HMGL gr, const char *name, const char *path)
{	gr->GetFont()->Load(name,path);	}
/// Copy font data from another HMGL object
void mgl_copy_font(HMGL gr, HMGL gr_from)
{	gr->GetFont()->Copy(gr_from->GetFont());	}
/// Restore font data
void mgl_restore_font(HMGL gr)
{	gr->GetFont()->Restore();	}
//-----------------------------------------------------------------------------
//		Export to file
//-----------------------------------------------------------------------------
/// Write the frame in file using PostScript format
void mgl_write_eps(HMGL gr, const char *fname,const char *descr)
{	gr->WriteEPS(fname,descr);	}
/// Write the frame in file using ScalableVectorGraphics format
void mgl_write_svg(HMGL gr, const char *fname,const char *descr)
{	gr->WriteSVG(fname,descr);	}
void mgl_show_image(HMGL gr, const char *viewer, int keep)
{	gr->ShowImage(viewer,keep);	}
//-----------------------------------------------------------------------------
//		Setup frames transparency (alpha) and lightning
//-----------------------------------------------------------------------------
/// Create new frame.
int mgl_new_frame(HMGL gr)
{	return gr->NewFrame();	}
/// Finish frame drawing
void mgl_end_frame(HMGL gr)
{	gr->EndFrame();	}
/// Get the number of created frames
int mgl_get_num_frame(HMGL gr)
{	return gr->GetNumFrame();	}
/// Reset frames counter
void mgl_reset_frames(HMGL gr)
{	gr->ResetFrames();	}
/// Set the transparency on/off.
void mgl_set_alpha(HMGL gr, int enable)
{	gr->Alpha(enable);	}
/// Set the fog distance.
void mgl_set_fog(HMGL gr, mreal d, mreal dz)
{	gr->Fog(d,dz);	}
/// Set the using of light on/off.
void mgl_set_light(HMGL gr, int enable)
{	gr->Light(enable);	}
/// Set the using of n-th light on/off.
void mgl_set_light_n(HMGL gr, int n, int enable)
{	gr->Light(n, enable);	}
/// Add white light source.
void mgl_add_light(HMGL gr, int n, mreal x, mreal y, mreal z, char c)
{	gr->Light(n,mglPoint(x,y,z),c,0.5);	}
/// Add a light source with color {r,g,b}.
void mgl_add_light_rgb(HMGL gr, int n, mreal x, mreal y, mreal z, int infty,
						mreal r, mreal g, mreal b,mreal i)
{	gr->Light(n,mglPoint(x,y,z),mglColor(r,g,b),i,infty);	}
// Set ambient light brightness
void mgl_set_ambbr(HMGL gr, mreal i)
{	gr->Ambient(i);	}
//-----------------------------------------------------------------------------
//		Scale and rotate
//-----------------------------------------------------------------------------
/// Push transformation matrix into stack.
void mgl_mat_push(HMGL gr)	{	gr->Push();	}
/// Pop transformation matrix into stack.
void mgl_mat_pop(HMGL gr)	{	gr->Pop();	}
/// Clear transformation matrix.
void mgl_identity(HMGL gr, int rel)	{	gr->Identity(rel);	}
/// Clear up the frame
void mgl_clf(HMGL gr)	{	gr->Clf();	}
/// Clear up the frame
void mgl_clf_rgb(HMGL gr, mreal r, mreal g, mreal b){	gr->Clf(mglColor(r,g,b));	}
/// Put further plotting in some region of whole frame surface.
void mgl_subplot(HMGL gr, int nx,int ny,int m)		{	gr->SubPlot(nx,ny,m);	}
void mgl_subplot_d(HMGL gr, int nx,int ny,int m,mreal dx,mreal dy)
{	gr->SubPlot(nx,ny,m,dx,dy);	}
void mgl_subplot_s(HMGL gr, int nx,int ny,int m,const char *style)
{	gr->SubPlot(nx,ny,m,style);	}
/// Put further plotting in some region of whole frame surface.
void mgl_inplot(HMGL gr, mreal x1,mreal x2,mreal y1,mreal y2)
{	gr->InPlot(x1,x2,y1,y2,false);	}
void mgl_relplot(HMGL gr, mreal x1,mreal x2,mreal y1,mreal y2)
{	gr->InPlot(x1,x2,y1,y2,true);	}
void mgl_columnplot(HMGL gr, int num, int i)
{	gr->ColumnPlot(num,i);	}
void mgl_columnplot_d(HMGL gr, int num, int i, mreal d)
{	gr->ColumnPlot(num,i,d);	}
void mgl_stickplot(HMGL gr, int num, int i, mreal tet, mreal phi)
{	gr->StickPlot(num, i, tet, phi);	}
/// Set aspect ratio for further plotting.
void mgl_aspect(HMGL gr, mreal Ax,mreal Ay,mreal Az)
{	gr->Aspect(Ax,Ay,Az);	}
/// Rotate a further plotting.
void mgl_rotate(HMGL gr, mreal TetX,mreal TetZ,mreal TetY)
{	gr->Rotate(TetX,TetZ,TetY);	}
/// Rotate a further plotting around vector {x,y,z}.
void mgl_rotate_vector(HMGL gr, mreal Tet,mreal x,mreal y,mreal z)
{	gr->RotateN(Tet,x,y,z);	}
void mgl_perspective(HMGL gr, mreal val)
{	gr->Perspective(val);	}
//-----------------------------------------------------------------------------
//		Axis functions
//-----------------------------------------------------------------------------
/// Auto adjust ticks
void mgl_adjust_ticks(HMGL gr, const char *dir)
{	gr->AdjustTicks(dir);	}
/// Switch on/off ticks tunning and set factor position for tunned ticks.
void mgl_tune_ticks(HMGL gr, int tune, mreal fact_pos)
{	gr->TuneTicks = tune;	gr->FactorPos = fact_pos;	}
void mgl_set_ticks_dir(HMGL gr, char dir, mreal d, int ns, mreal org)
{	gr->SetTicks(dir, d, ns, org);	}
/// Set ticks interval mglGraph::dx, mglGraph::dy, mglGraph::dz.
void mgl_set_ticks(HMGL gr, mreal DX, mreal DY, mreal DZ)
{	gr->dx=DX;	gr->dy=DY;	gr->dz=DZ;	}
/// Set number of subticks mglGraph::NSx, mglGraph::NSy, mglGraph::NSz.
void mgl_set_subticks(HMGL gr, int NX, int NY, int NZ)
{	gr->NSx=NX;	gr->NSy=NY;	gr->NSz=NZ;	}
/// Set the values of mglGraph::Cmin and mglGraph::Cmax
void mgl_set_caxis(HMGL gr, mreal C1,mreal C2)
{	gr->CAxis(C1,C2);	}
void mgl_set_crange(HMGL gr, const HMDT a, int add)	{	gr->CRange(*a,add);	}
void mgl_set_xrange(HMGL gr, const HMDT a, int add)	{	gr->XRange(*a,add);	}
void mgl_set_yrange(HMGL gr, const HMDT a, int add)	{	gr->YRange(*a,add);	}
void mgl_set_zrange(HMGL gr, const HMDT a, int add)	{	gr->ZRange(*a,add);	}
/// Safetly set the value for mglGraph::Min, mglGraph::Max and mglGraph::Org members of the class.
void mgl_set_axis(HMGL gr, mreal x1, mreal y1, mreal z1, mreal x2, mreal y2, mreal z2, mreal x0, mreal y0, mreal z0)
{	gr->Axis(mglPoint(x1,y1,z1),mglPoint(x2,y2,z2),mglPoint(x0,y0,z0));	}
/// Safetly set the transformation formulas for coordinate.
void mgl_set_func(HMGL gr, const char *EqX,const char *EqY,const char *EqZ)
{	gr->SetFunc(EqX,EqY,EqZ);	}
void mgl_set_func_ext(HMGL gr, const char *EqX,const char *EqY,const char *EqZ,const char *EqA)
{	gr->SetFunc(EqX,EqY,EqZ,EqA);	}
/// Set predefined coordinate system
void mgl_set_coor(HMGL gr, int how)	{	gr->SetCoor(how);	}
void mgl_set_cutoff(HMGL gr, const char *EqC)	{	gr->CutOff(EqC);	}
/// Draws bounding box outside the plotting volume by default color.
void mgl_box(HMGL gr, int ticks)
{	gr->Box(NC,ticks);	}
/// Draws bounding box outside the plotting volume with color \a c.
void mgl_box_str(HMGL gr, const char *col, int ticks)
{	gr->Box(col,ticks);	}
/// Draws bounding box outside the plotting volume.
void mgl_box_rgb(HMGL gr, mreal r, mreal g, mreal b, int ticks)
{	gr->Box(mglColor(r,g,b),ticks);	}
/// Draw axises with ticks in directions determined by string parameter \a dir.
void mgl_axis(HMGL gr, const char *dir)
{	gr->Axis(dir);	}
/// Draw grid lines perpendicular to direction determined by string parameter \a dir.
void mgl_axis_grid(HMGL gr, const char *dir,const char *pen)
{	gr->Grid(dir,pen);	}
/// Print the label \a text for axis \a dir.
void mgl_label(HMGL gr, char dir, const char *text)
{	gr->Label(dir, text);	}
/// Print the label \a text for axis \a dir.
void mgl_label_ext(HMGL gr, char dir, const char *text, mreal pos, mreal size, mreal shift)
{	gr->Label(dir, text, pos, size, shift);	}
void mgl_labelw_ext(HMGL gr, char dir, const wchar_t *text, mreal pos, mreal size, mreal shift)
{	gr->Labelw(dir, text, pos, size, shift);	}
void mgl_label_xy(HMGL gr, mreal x, mreal y, const char *text, const char *fnt, mreal size)
{	gr->Label(x,y,text,fnt,size);	}
void mgl_labelw_xy(HMGL gr, mreal x, mreal y, const wchar_t *text, const char *fnt, mreal size)
{	gr->Labelw(x,y,text,fnt,size);	}
void mgl_set_xttw(HMGL gr, const wchar_t *templ)	{gr->SetXTT(templ);}
void mgl_set_yttw(HMGL gr, const wchar_t *templ)	{gr->SetYTT(templ);}
void mgl_set_zttw(HMGL gr, const wchar_t *templ)	{gr->SetZTT(templ);}
void mgl_set_cttw(HMGL gr, const wchar_t *templ)	{gr->SetCTT(templ);}
void mgl_set_xtt(HMGL gr, const char *templ)	{gr->SetXTT(templ);}
void mgl_set_ytt(HMGL gr, const char *templ)	{gr->SetYTT(templ);}
void mgl_set_ztt(HMGL gr, const char *templ)	{gr->SetZTT(templ);}
void mgl_set_ctt(HMGL gr, const char *templ)	{gr->SetCTT(templ);}
//-----------------------------------------------------------------------------
//		Simple drawing
//-----------------------------------------------------------------------------
/// Draws the red point (ball) at position \a {x,y,z}.
void mgl_ball(HMGL gr, mreal x,mreal y,mreal z)
{	gr->Ball(mglPoint(x,y,z));	}
/// Draws the point (ball) at position \a {x,y,z} with color {r,g,b}.
void mgl_ball_rgb(HMGL gr, mreal x, mreal y, mreal z, mreal r, mreal g, mreal b, mreal alpha)
{	gr->Ball(x,y,z,mglColor(r,g,b),alpha);	}
/// Draws the point (ball) at position \a p with color \a col.
void mgl_ball_str(HMGL gr, mreal x, mreal y, mreal z, char col)
{	gr->Ball(mglPoint(x,y,z),col);	}
/// Draws the line between points with style \a sch.
void mgl_line(HMGL gr, mreal x1, mreal y1, mreal z1, mreal x2, mreal y2, mreal z2, const char *pen,int n)
{	gr->Line(mglPoint(x1,y1,z1),mglPoint(x2,y2,z2),pen,n);	}

void mgl_facex(HMGL gr, mreal x0, mreal y0, mreal z0, mreal wy, mreal wz, const char *stl, mreal dx, mreal dy)
{	gr->FaceX(x0,y0,z0,wy,wz,stl,dx,dy);	}
void mgl_facey(HMGL gr, mreal x0, mreal y0, mreal z0, mreal wx, mreal wz, const char *stl, mreal dx, mreal dy)
{	gr->FaceY(x0,y0,z0,wx,wz,stl,dx,dy);	}
void mgl_facez(HMGL gr, mreal x0, mreal y0, mreal z0, mreal wx, mreal wy, const char *stl, mreal dx, mreal dy)
{	gr->FaceZ(x0,y0,z0,wx,wy,stl,dx,dy);	}
void mgl_curve(HMGL gr, mreal x1, mreal y1, mreal z1, mreal dx1, mreal dy1, mreal dz1, mreal x2, mreal y2, mreal z2, mreal dx2, mreal dy2, mreal dz2, const char *pen,int n)
{	gr->Curve(mglPoint(x1,y1,z1), mglPoint(dx1,dy1,dz1), mglPoint(x2,y2,z2), mglPoint(dx2,dy2,dz2), pen, n);	}

/// Print string \a str in position \a p with font size \a size.
void mgl_puts(HMGL gr, mreal x, mreal y, mreal z,const char *text)
{	gr->Puts(mglPoint(x,y,z),text);	}
/// Print string \a str in position \a p with font size \a size.
void mgl_putsw(HMGL gr, mreal x, mreal y, mreal z,const wchar_t *text)
{	gr->Putsw(mglPoint(x,y,z),text);	}
/// Print string \a str in position \a p along direction \a d with font size \a size.
void mgl_puts_dir(HMGL gr, mreal x, mreal y, mreal z, mreal dx, mreal dy, mreal dz, const char *text, mreal size)
{	gr->Puts(mglPoint(x,y,z), mglPoint(dx,dy,dz), text, 't', size);	}
/// Print string \a str in position \a p along direction \a d with font size \a size.
void mgl_putsw_dir(HMGL gr, mreal x, mreal y, mreal z, mreal dx, mreal dy, mreal dz, const wchar_t *text, mreal size)
{	gr->Putsw(mglPoint(x,y,z), mglPoint(dx,dy,dz), text, 't', size);	}
/// Print unrotated string \a str in position \a p with font size \a size.
void mgl_text(HMGL gr, mreal x, mreal y, mreal z,const char *text)
{	gr->Text(mglPoint(x,y,z),text);	}
/// Print string \a str in position \a p with font size \a size.
void mgl_puts_ext(HMGL gr, mreal x, mreal y, mreal z,const char *text,const char *font,mreal size,char dir)
{	gr->Puts(mglPoint(x,y,z),text,font,size,dir);	}
/// Print string \a str in position \a p with font size \a size.
void mgl_putsw_ext(HMGL gr, mreal x, mreal y, mreal z,const wchar_t *text,const char *font,mreal size,char dir)
{	gr->Putsw(mglPoint(x,y,z),text,font,size,dir);	}
/// Print unrotated string \a str in position \a p with font size \a size.
void mgl_text_ext(HMGL gr, mreal x, mreal y, mreal z,const char *text,const char *font,mreal size,char dir)
{	gr->Text(mglPoint(x,y,z),text,font,size,dir);	}
/// Draw colorbar at edge of axis
void mgl_colorbar(HMGL gr, const char *sch,int where)
{	gr->Colorbar(sch,where);	}
void mgl_colorbar_ext(HMGL gr, const char *sch,int where, mreal x, mreal y, mreal w, mreal h)
{	gr->Colorbar(sch,where,x,y,w,h);	}
void mgl_colorbar_val(HMGL gr, const HMDT val, const char *sch,int where)
{	gr->Colorbar(*val,sch,where);	}
/// Plot data depending on its dimensions and \a type parameter
void mgl_simple_plot(HMGL gr, const HMDT a, int type, const char *sch)
{	if(gr && a)	gr->SimplePlot(*a,type,sch);	}
//-----------------------------------------------------------------------------
//		Plot legend drawing
//-----------------------------------------------------------------------------
/// Add string to legend
void mgl_add_legend(HMGL gr, const char *text,const char *style)
{	if(gr)	gr->AddLegend(text,style);	}
/// Add string to legend
void mgl_add_legendw(HMGL gr, const wchar_t *text,const char *style)
{	if(gr)	gr->AddLegend(text,style);	}
/// Clear saved legend string
void mgl_clear_legend(HMGL gr)
{	if(gr)	gr->ClearLegend();	}
/// Draw legend of accumulated strings at position (x, y) by \a font with \a size
void mgl_legend_xy(HMGL gr, mreal x, mreal y, const char *font, mreal size, mreal llen)
{	if(gr)	gr->Legend(x, y, font, size, llen);	}
/// Draw legend of accumulated strings by \a font with \a size
void mgl_legend(HMGL gr, int where, const char *font, mreal size, mreal llen)
{	if(gr)	gr->Legend(where, font, size, llen);	}
void mgl_set_legend_box(HMGL gr, int enable)
{	gr->LegendBox = enable;	}
void mgl_set_legend_marks(HMGL gr, int num)
{	gr->SetLegendMarks(num);	}
//-----------------------------------------------------------------------------
const unsigned char *mgl_get_rgb(HMGL graph)
{
	mglGraphAB *g = dynamic_cast<mglGraphAB *>(graph);
	return g ? g->GetBits():0;
}
const unsigned char *mgl_get_rgba(HMGL graph)
{
	mglGraphAB *g = dynamic_cast<mglGraphAB *>(graph);
	return g ? g->GetRGBA():0;
}
int mgl_get_width(HMGL graph)
{
	mglGraphAB *g = dynamic_cast<mglGraphAB *>(graph);
	return g ? g->GetWidth():0;
}
int mgl_get_height(HMGL graph)
{
	mglGraphAB *g = dynamic_cast<mglGraphAB *>(graph);
	return g ? g->GetHeight():0;
}
void mgl_update(HMGL graph)
{
	mglGraphAB *g = dynamic_cast<mglGraphAB *>(graph);
	if(g)	g->Update();
}
void mgl_set_show_mouse_pos(HMGL gr, int enable)
{
	mglGraphAB *g = dynamic_cast<mglGraphAB *>(gr);
	if(g) g->ShowMousePos=enable;
}
void mgl_get_last_mouse_pos(HMGL gr, mreal *x, mreal *y, mreal *z)
{
	mglGraphAB *g = dynamic_cast<mglGraphAB *>(gr);
	if(g)
	{	*x=g->LastMousePos.x;	*y=g->LastMousePos.y;	*z=g->LastMousePos.z;}
}
void mgl_calc_xyz(HMGL gr, int xs, int ys, mreal *x, mreal *y, mreal *z)
{
	mglGraphAB *g = dynamic_cast<mglGraphAB *>(gr);
	if(g && x && y && z)
	{
		mglPoint p = g->CalcXYZ(xs,ys);
		*x = p.x;	*y = p.y;	*z = p.z;
	}
}
void mgl_calc_scr(HMGL gr, mreal x, mreal y, mreal z, int *xs, int *ys)
{
	mglGraphAB *g = dynamic_cast<mglGraphAB *>(gr);
	if(g && xs && ys)	g->CalcScr(mglPoint(x,y,z),xs,ys);
}
//-----------------------------------------------------------------------------
void mgl_wnd_set_delay(HMGL gr, mreal dt)
{	mglGraphAB *g = dynamic_cast<mglGraphAB *>(gr);	if(g)	g->Delay = dt;	}
void mgl_wnd_set_auto_clf(HMGL gr, int val)
{	mglGraphAB *g = dynamic_cast<mglGraphAB *>(gr);	if(g)	g->AutoClf = val;	}
void mgl_wnd_set_show_mouse_pos(HMGL gr, int val)
{	mglGraphAB *g = dynamic_cast<mglGraphAB *>(gr);	if(g)	g->ShowMousePos = val;	}
void mgl_wnd_set_clf_update(HMGL gr, int val)
{	mglGraphAB *g = dynamic_cast<mglGraphAB *>(gr);	if(g)	g->ClfOnUpdate = val;	}
void mgl_wnd_toggle_alpha(HMGL gr)
{	mglGraphAB *g = dynamic_cast<mglGraphAB *>(gr);	if(g)	g->ToggleAlpha();	}
void mgl_wnd_toggle_light(HMGL gr)
{	mglGraphAB *g = dynamic_cast<mglGraphAB *>(gr);	if(g)	g->ToggleLight();	}
void mgl_wnd_toggle_zoom(HMGL gr)
{	mglGraphAB *g = dynamic_cast<mglGraphAB *>(gr);	if(g)	g->ToggleZoom();	}
void mgl_wnd_toggle_rotate(HMGL gr)
{	mglGraphAB *g = dynamic_cast<mglGraphAB *>(gr);	if(g)	g->ToggleRotate();	}
void mgl_wnd_toggle_no(HMGL gr)
{	mglGraphAB *g = dynamic_cast<mglGraphAB *>(gr);	if(g)	g->ToggleNo();	}
void mgl_wnd_update(HMGL gr)
{	mglGraphAB *g = dynamic_cast<mglGraphAB *>(gr);	if(g)	g->Update();	}
void mgl_wnd_reload(HMGL gr, int o)
{	mglGraphAB *g = dynamic_cast<mglGraphAB *>(gr);	if(g)	g->ReLoad(o);	}
void mgl_wnd_adjust(HMGL gr)
{	mglGraphAB *g = dynamic_cast<mglGraphAB *>(gr);	if(g)	g->Adjust();	}
void mgl_wnd_next_frame(HMGL gr)
{	mglGraphAB *g = dynamic_cast<mglGraphAB *>(gr);	if(g)	g->NextFrame();	}
void mgl_wnd_prev_frame(HMGL gr)
{	mglGraphAB *g = dynamic_cast<mglGraphAB *>(gr);	if(g)	g->PrevFrame();	}
void mgl_wnd_animation(HMGL gr)
{	mglGraphAB *g = dynamic_cast<mglGraphAB *>(gr);	if(g)	g->Animation();	}
//-----------------------------------------------------------------------------
mreal mgl_data_get_value(const HMDT d, int i, int j, int k)
{	return	d->a[i+d->nx*(j+d->ny*k)];	}
void mgl_data_set_value(HMDT d, mreal v, int i, int j, int k)
{	d->a[i+d->nx*(j+d->ny*k)] = v;	}
//-----------------------------------------------------------------------------
/// Zoom in/out a part of picture
void mgl_set_zoom(HMGL gr, mreal x1, mreal y1, mreal x2, mreal y2)
{	gr->Zoom(x1,y1,x2,y2);	}
void mgl_set_plotfactor(HMGL gr, mreal val)
{	gr->SetPlotFactor(val);	}
void mgl_set_axis_3d(HMGL gr, mreal x1, mreal y1, mreal z1, mreal x2, mreal y2, mreal z2)
{	gr->Axis(mglPoint(x1,y1,z1),mglPoint(x2,y2,z2));	}
void mgl_set_axis_2d(HMGL gr, mreal x1, mreal y1, mreal x2, mreal y2)
{	gr->Axis(mglPoint(x1,y1),mglPoint(x2,y2));	}
void mgl_set_origin(HMGL gr, mreal x0, mreal y0, mreal z0)
{	gr->Org = mglPoint(x0,y0,z0);	}
void mgl_set_tick_origin(HMGL gr, mreal x0, mreal y0, mreal z0)
{	gr->OrgT = mglPoint(x0,y0,z0);	}
//-----------------------------------------------------------------------------
void mgl_title(HMGL gr, const char *text, const char *fnt, mreal size)
{	gr->Title(text, fnt,size);	}
void mgl_titlew(HMGL gr, const wchar_t *text, const char *fnt, mreal size)
{	gr->Title(text, fnt,size);	}
//-----------------------------------------------------------------------------
void mgl_set_ternary(HMGL gr, int enable)
{	gr->Ternary(enable);	}
void mgl_sphere(HMGL gr, mreal x, mreal y, mreal z, mreal r, const char *stl)
{	gr->Sphere(mglPoint(x,y,z),r,stl);	}
void mgl_drop(HMGL gr, mreal x1, mreal y1, mreal z1, mreal x2, mreal y2, mreal z2, mreal r, const char *stl, mreal shift, mreal ap)
{	gr->Drop(mglPoint(x1,y1,z1),mglPoint(x2,y2,z2),r,stl,shift,ap);	}
void mgl_cone(HMGL gr, mreal x1, mreal y1, mreal z1, mreal x2, mreal y2, mreal z2, mreal r1, mreal r2, const char *stl, int edge)
{	gr->Cone(mglPoint(x1,y1,z1),mglPoint(x2,y2,z2),r1,r2,stl,edge);	}
void mgl_set_def_param(HMGL gr)	{	gr->DefaultPlotParam();	}
void mgl_set_font_def(HMGL gr, const char *fnt)
{	strncpy(gr->FontDef, fnt, 31);	}
void mgl_flush(HMGL gr)	{	gr->Flush();	}
void mgl_data_fill_eq(HMGL gr, HMDT dat, const char *eq, const HMDT vdat, const HMDT wdat)
{	dat->Fill(eq, gr->Min, gr->Max, vdat, wdat);	}
void mgl_set_ticks_vals(HMGL gr, char dir, int n, mreal *val, const char **lbl)
{	gr->SetTicksVal(dir,n,val,lbl);	}
//-----------------------------------------------------------------------------
#ifndef _MSC_VER
#include <stdint.h>
#endif
int mgl_fortran_func(HMGL gr, void *f)
{
	typedef int (*func_draw)(uintptr_t *gr);
	func_draw draw = func_draw(f);
	static uintptr_t p = uintptr_t(gr);
	int res = draw(&p);
	return res;
}
//-----------------------------------------------------------------------------
void mgl_set_auto(HMGL gr, mreal x1, mreal x2, mreal y1, mreal y2, mreal z1, mreal z2)
{	gr->SetAutoRanges(x1,x2,y1,y2,z1,z2);	}
void mgl_set_tick_len(HMGL gr, mreal len, mreal stt)
{	gr->SetTickLen(len,stt);	}
void mgl_set_tick_stl(HMGL gr, const char *stl, const char *sub)
{	gr->SetTickStl(stl,sub);	}
int mgl_get_warn(HMGL gr)	{	return gr->WarnCode;	}
//-----------------------------------------------------------------------------
