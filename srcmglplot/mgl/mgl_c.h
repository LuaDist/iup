/***************************************************************************
 * mgl_c.h is part of Math Graphic Library
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
#ifndef _MGL_C_H_
#define _MGL_C_H_

#include <mgl/config.h>

#if(MGL_USE_DOUBLE==1)
typedef double mreal;
#else
typedef float mreal;
#endif
//#include <mgl/mgl_define.h>

#ifdef __cplusplus
extern "C" {
#endif
/*****************************************************************************/
//#ifdef _MGL_DATA_H_
#ifdef __cplusplus
struct mglDraw;
typedef mglDraw* HMDR;
class mglGraph;
typedef mglGraph* HMGL;
class mglData;
typedef mglData* HMDT;
class mglParse;
typedef mglParse* HMPR;
#else
typedef void* HMDR;
typedef void* HMGL;
typedef void* HMDT;
typedef void* HMPR;
#endif
#ifndef NO_GSL
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#else
struct gsl_vector;
struct gsl_matrix;
#endif
/*****************************************************************************/
HMGL mgl_create_graph_gl();
HMGL mgl_create_graph_zb(int width, int height);
HMGL mgl_create_graph_ps(int width, int height);
HMGL mgl_create_graph_idtf();
#ifndef MGL_NO_WIDGET
int mgl_fortran_func(HMGL gr, void *);
HMGL mgl_create_graph_glut(int (*draw)(HMGL gr, void *p), const char *title, void *par);
HMGL mgl_create_graph_fltk(int (*draw)(HMGL gr, void *p), const char *title, void *par);
HMGL mgl_create_graph_qt(int (*draw)(HMGL gr, void *p), const char *title, void *par);
HMGL mgl_create_graph_glut_dr(HMDR dr, const char *title);
HMGL mgl_create_graph_fltk_dr(HMDR dr, const char *title);
HMGL mgl_create_graph_qt_dr(HMDR dr, const char *title);
void mgl_fltk_run();
void mgl_qt_run();

void mgl_wnd_set_delay(HMGL gr, mreal dt);
void mgl_wnd_set_auto_clf(HMGL gr, int val);
void mgl_wnd_set_show_mouse_pos(HMGL gr, int val);
void mgl_wnd_set_clf_update(HMGL gr, int val);
void mgl_wnd_toggle_alpha(HMGL gr);
void mgl_wnd_toggle_light(HMGL gr);
void mgl_wnd_toggle_zoom(HMGL gr);
void mgl_wnd_toggle_rotate(HMGL gr);
void mgl_wnd_toggle_no(HMGL gr);
void mgl_wnd_update(HMGL gr);
void mgl_wnd_reload(HMGL gr, int o);
void mgl_wnd_adjust(HMGL gr);
void mgl_wnd_next_frame(HMGL gr);
void mgl_wnd_prev_frame(HMGL gr);
void mgl_wnd_animation(HMGL gr);
#endif
void mgl_set_show_mouse_pos(HMGL gr, int enable);
void mgl_get_last_mouse_pos(HMGL gr, mreal *x, mreal *y, mreal *z);
void mgl_calc_xyz(HMGL gr, int xs, int ys, mreal *x, mreal *y, mreal *z);
void mgl_calc_scr(HMGL gr, mreal x, mreal y, mreal z, int *xs, int *ys);
//void mgl_fltk_thread();
//void mgl_qt_thread();
void mgl_update(HMGL graph);
void mgl_delete_graph(HMGL graph);
/*****************************************************************************/
HMDT mgl_create_data();
HMDT mgl_create_data_size(int nx, int ny, int nz);
HMDT mgl_create_data_file(const char *fname);
void mgl_delete_data(HMDT dat);
/*****************************************************************************/
HMPR mgl_create_parser();
void mgl_delete_parser(HMPR p);
void mgl_scan_func(HMPR p, const wchar_t *line);
void mgl_add_param(HMPR p, int id, const char *str);
void mgl_add_paramw(HMPR p, int id, const wchar_t *str);
/*===!!! NOTE !!! You must not delete obtained data arrays !!!===============*/
HMDT mgl_add_var(HMPR, const char *name);
/*===!!! NOTE !!! You must not delete obtained data arrays !!!===============*/
HMDT mgl_find_var(HMPR, const char *name);
int mgl_parse(HMGL gr, HMPR p, const char *str, int pos);
int mgl_parsew(HMGL gr, HMPR p, const wchar_t *str, int pos);
void mgl_parse_text(HMGL gr, HMPR p, const char *str);
void mgl_parsew_text(HMGL gr, HMPR p, const wchar_t *str);
void mgl_restore_once(HMPR p);
void mgl_parser_allow_setsize(HMPR p, int a);
/*****************************************************************************/
/*		Setup mglGraph														 */
/*****************************************************************************/
void mgl_set_def_param(HMGL gr);
void mgl_set_palette(HMGL gr, const char *colors);
void mgl_set_pal_color(HMGL graph, int n, mreal r, mreal g, mreal b);
void mgl_set_pal_num(HMGL graph, int num);
void mgl_set_rotated_text(HMGL graph, int rotated);
void mgl_set_cut(HMGL graph, int cut);
void mgl_set_cut_box(HMGL gr, mreal x1,mreal y1,mreal z1,mreal x2,mreal y2,mreal z2);
void mgl_set_tick_len(HMGL graph, mreal len, mreal stt);
void mgl_set_tick_stl(HMGL gr, const char *stl, const char *sub);
void mgl_set_bar_width(HMGL graph, mreal width);
void mgl_set_base_line_width(HMGL gr, mreal size);
void mgl_set_mark_size(HMGL graph, mreal size);
void mgl_set_arrow_size(HMGL graph, mreal size);
void mgl_set_font_size(HMGL graph, mreal size);
void mgl_set_font_def(HMGL graph, const char *fnt);
void mgl_set_alpha_default(HMGL graph, mreal alpha);
void mgl_set_size(HMGL graph, int width, int height);
void mgl_set_axial_dir(HMGL graph, char dir);
void mgl_set_meshnum(HMGL graph, int num);
void mgl_set_zoom(HMGL gr, mreal x1, mreal y1, mreal x2, mreal y2);
void mgl_set_plotfactor(HMGL gr, mreal val);
void mgl_set_draw_face(HMGL gr, int enable);
void mgl_set_scheme(HMGL gr, const char *sch);
void mgl_load_font(HMGL gr, const char *name, const char *path);
void mgl_copy_font(HMGL gr, HMGL gr_from);
void mgl_restore_font(HMGL gr);
int mgl_get_warn(HMGL gr);
/*****************************************************************************/
/*		Export to file or to memory											 */
/*****************************************************************************/
void mgl_show_image(HMGL graph, const char *viewer, int keep);
void mgl_write_frame(HMGL graph, const char *fname,const char *descr);
void mgl_write_bmp(HMGL graph, const char *fname,const char *descr);
void mgl_write_jpg(HMGL graph, const char *fname,const char *descr);
void mgl_write_png(HMGL graph, const char *fname,const char *descr);
void mgl_write_png_solid(HMGL graph, const char *fname,const char *descr);
void mgl_write_eps(HMGL graph, const char *fname,const char *descr);
void mgl_write_svg(HMGL graph, const char *fname,const char *descr);
void mgl_write_idtf(HMGL graph, const char *fname,const char *descr);
void mgl_write_gif(HMGL graph, const char *fname,const char *descr);
void mgl_start_gif(HMGL graph, const char *fname,int ms);
void mgl_close_gif(HMGL graph);
const unsigned char *mgl_get_rgb(HMGL graph);
const unsigned char *mgl_get_rgba(HMGL graph);
int mgl_get_width(HMGL graph);
int mgl_get_height(HMGL graph);
/*****************************************************************************/
/*		Setup frames transparency (alpha) and lightning						 */
/*****************************************************************************/
int mgl_new_frame(HMGL graph);
void mgl_end_frame(HMGL graph);
int mgl_get_num_frame(HMGL graph);
void mgl_reset_frames(HMGL graph);
void mgl_set_transp_type(HMGL graph, int type);
void mgl_set_transp(HMGL graph, int enable);
void mgl_set_alpha(HMGL graph, int enable);
void mgl_set_fog(HMGL graph, mreal d, mreal dz);
void mgl_set_light(HMGL graph, int enable);
void mgl_set_light_n(HMGL gr, int n, int enable);
void mgl_add_light(HMGL graph, int n, mreal x, mreal y, mreal z, char c);
void mgl_add_light_rgb(HMGL graph, int n, mreal x, mreal y, mreal z, int infty, mreal r, mreal g, mreal b, mreal i);
void mgl_set_ambbr(HMGL gr, mreal i);
/*****************************************************************************/
/*		Scale and rotate													 */
/*****************************************************************************/
void mgl_mat_pop(HMGL gr);
void mgl_mat_push(HMGL gr);
void mgl_identity(HMGL graph, int rel);
void mgl_clf(HMGL graph);
void mgl_flush(HMGL gr);
void mgl_clf_rgb(HMGL graph, mreal r, mreal g, mreal b);
void mgl_subplot(HMGL graph, int nx,int ny,int m);
void mgl_subplot_d(HMGL graph, int nx,int ny,int m, mreal dx, mreal dy);
void mgl_subplot_s(HMGL graph, int nx,int ny,int m,const char *style);
void mgl_inplot(HMGL graph, mreal x1,mreal x2,mreal y1,mreal y2);
void mgl_relplot(HMGL graph, mreal x1,mreal x2,mreal y1,mreal y2);
void mgl_columnplot(HMGL graph, int num, int ind);
void mgl_columnplot_d(HMGL graph, int num, int ind, mreal d);
void mgl_stickplot(HMGL graph, int num, int ind, mreal tet, mreal phi);
void mgl_aspect(HMGL graph, mreal Ax,mreal Ay,mreal Az);
void mgl_rotate(HMGL graph, mreal TetX,mreal TetZ,mreal TetY);
void mgl_rotate_vector(HMGL graph, mreal Tet,mreal x,mreal y,mreal z);
void mgl_perspective(HMGL graph, mreal val);
/*****************************************************************************/
/*		Axis functions														 */
/*****************************************************************************/
void mgl_adjust_ticks(HMGL graph, const char *dir);
void mgl_set_ticks(HMGL graph, mreal DX, mreal DY, mreal DZ);
void mgl_set_subticks(HMGL graph, int NX, int NY, int NZ);
void mgl_set_ticks_dir(HMGL graph, char dir, mreal d, int ns, mreal org);
void mgl_set_ticks_val(HMGL graph, char dir, int n, double val, const char *lbl, ...);
void mgl_set_ticks_vals(HMGL graph, char dir, int n, mreal *val, const char **lbl);
void mgl_set_caxis(HMGL graph, mreal C1,mreal C2);
void mgl_set_axis(HMGL graph, mreal x1, mreal y1, mreal z1, mreal x2, mreal y2, mreal z2, mreal x0, mreal y0, mreal z0);
void mgl_set_axis_3d(HMGL graph, mreal x1, mreal y1, mreal z1, mreal x2, mreal y2, mreal z2);
void mgl_set_axis_2d(HMGL graph, mreal x1, mreal y1, mreal x2, mreal y2);
inline void mgl_set_ranges(HMGL graph, mreal x1, mreal x2, mreal y1, mreal y2, mreal z1, mreal z2)
{	mgl_set_axis_3d(graph, x1,y1,z1,x2,y2,z2);	};
void mgl_set_origin(HMGL graph, mreal x0, mreal y0, mreal z0);
void mgl_set_tick_origin(HMGL graph, mreal x0, mreal y0, mreal z0);
void mgl_set_crange(HMGL graph, const HMDT a, int add);
void mgl_set_xrange(HMGL graph, const HMDT a, int add);
void mgl_set_yrange(HMGL graph, const HMDT a, int add);
void mgl_set_zrange(HMGL graph, const HMDT a, int add);
void mgl_set_auto(HMGL graph, mreal x1, mreal x2, mreal y1, mreal y2, mreal z1, mreal z2);
void mgl_set_func(HMGL graph, const char *EqX,const char *EqY,const char *EqZ);
void mgl_set_func_ext(HMGL graph, const char *EqX,const char *EqY,const char *EqZ,const char *EqA);
void mgl_set_coor(HMGL gr, int how);
void mgl_set_ternary(HMGL gr, int enable);
void mgl_set_cutoff(HMGL graph, const char *EqC);
void mgl_box(HMGL graph, int ticks);
void mgl_box_str(HMGL graph, const char *col, int ticks);
void mgl_box_rgb(HMGL graph, mreal r, mreal g, mreal b, int ticks);
void mgl_axis(HMGL graph, const char *dir);
void mgl_axis_grid(HMGL graph, const char *dir,const char *pen);
void mgl_label(HMGL graph, char dir, const char *text);
void mgl_label_ext(HMGL graph, char dir, const char *text, mreal pos, mreal size, mreal shift);
void mgl_labelw_ext(HMGL graph, char dir, const wchar_t *text, mreal pos, mreal size, mreal shift);
void mgl_label_xy(HMGL graph, mreal x, mreal y, const char *text, const char *fnt, mreal size);
void mgl_labelw_xy(HMGL graph, mreal x, mreal y, const wchar_t *text, const char *fnt, mreal size);
void mgl_tune_ticks(HMGL graph, int tune, mreal fact_pos);
void mgl_set_xttw(HMGL graph, const wchar_t *templ);
void mgl_set_yttw(HMGL graph, const wchar_t *templ);
void mgl_set_zttw(HMGL graph, const wchar_t *templ);
void mgl_set_cttw(HMGL graph, const wchar_t *templ);
void mgl_set_xtt(HMGL graph, const char *templ);
void mgl_set_ytt(HMGL graph, const char *templ);
void mgl_set_ztt(HMGL graph, const char *templ);
void mgl_set_ctt(HMGL graph, const char *templ);
/*****************************************************************************/
/*		Simple drawing														 */
/*****************************************************************************/
void mgl_ball(HMGL graph, mreal x,mreal y,mreal z);
void mgl_ball_rgb(HMGL graph, mreal x, mreal y, mreal z, mreal r, mreal g, mreal b, mreal alpha);
void mgl_ball_str(HMGL graph, mreal x, mreal y, mreal z, char col);
void mgl_line(HMGL graph, mreal x1, mreal y1, mreal z1, mreal x2, mreal y2, mreal z2, const char *pen,int n);
void mgl_facex(HMGL graph, mreal x0, mreal y0, mreal z0, mreal wy, mreal wz, const char *stl, mreal dx, mreal dy);
void mgl_facey(HMGL graph, mreal x0, mreal y0, mreal z0, mreal wx, mreal wz, const char *stl, mreal dx, mreal dy);
void mgl_facez(HMGL graph, mreal x0, mreal y0, mreal z0, mreal wx, mreal wy, const char *stl, mreal dx, mreal dy);
void mgl_curve(HMGL graph, mreal x1, mreal y1, mreal z1, mreal dx1, mreal dy1, mreal dz1, mreal x2, mreal y2, mreal z2, mreal dx2, mreal dy2, mreal dz2, const char *pen,int n);

void mgl_puts(HMGL graph, mreal x, mreal y, mreal z,const char *text);
void mgl_putsw(HMGL graph, mreal x, mreal y, mreal z,const wchar_t *text);
void mgl_puts_dir(HMGL graph, mreal x, mreal y, mreal z, mreal dx, mreal dy, mreal dz, const char *text, mreal size);
void mgl_putsw_dir(HMGL graph, mreal x, mreal y, mreal z, mreal dx, mreal dy, mreal dz, const wchar_t *text, mreal size);
void mgl_text(HMGL graph, mreal x, mreal y, mreal z,const char *text);
void mgl_title(HMGL graph, const char *text, const char *fnt, mreal size);
void mgl_titlew(HMGL graph, const wchar_t *text, const char *fnt, mreal size);
void mgl_putsw_ext(HMGL graph, mreal x, mreal y, mreal z,const wchar_t *text,const char *font,mreal size,char dir);
void mgl_puts_ext(HMGL graph, mreal x, mreal y, mreal z,const char *text,const char *font,mreal size,char dir);
void mgl_text_ext(HMGL graph, mreal x, mreal y, mreal z,const char *text,const char *font,mreal size,char dir);
void mgl_colorbar(HMGL graph, const char *sch,int where);
void mgl_colorbar_ext(HMGL graph, const char *sch, int where, mreal x, mreal y, mreal w, mreal h);
void mgl_colorbar_val(HMGL graph, const HMDT dat, const char *sch,int where);
void mgl_simple_plot(HMGL graph, const HMDT a, int type, const char *stl);
void mgl_add_legend(HMGL graph, const char *text,const char *style);
void mgl_add_legendw(HMGL graph, const wchar_t *text,const char *style);
void mgl_clear_legend(HMGL graph);
void mgl_legend_xy(HMGL graph, mreal x, mreal y, const char *font, mreal size, mreal llen);
void mgl_legend(HMGL graph, int where, const char *font, mreal size, mreal llen);
void mgl_set_legend_box(HMGL gr, int enable);
void mgl_set_legend_marks(HMGL gr, int num);
/*****************************************************************************/
/*		1D plotting functions												 */
/*****************************************************************************/
void mgl_fplot(HMGL graph, const char *fy, const char *stl, int n);
void mgl_fplot_xyz(HMGL graph, const char *fx, const char *fy, const char *fz, const char *stl, int n);
void mgl_plot_xyz(HMGL graph, const HMDT x, const HMDT y, const HMDT z, const char *pen);
void mgl_plot_xy(HMGL graph, const HMDT x, const HMDT y, const char *pen);
void mgl_plot(HMGL graph, const HMDT y, const char *pen);
void mgl_radar(HMGL graph, const HMDT a, const char *pen, mreal r);
void mgl_boxplot_xy(HMGL graph, const HMDT x, const HMDT a, const char *pen);
void mgl_boxplot(HMGL graph, const HMDT a, const char *pen);
void mgl_tens_xyz(HMGL graph, const HMDT x, const HMDT y, const HMDT z, const HMDT c, const char *pen);
void mgl_tens_xy(HMGL graph, const HMDT x, const HMDT y, const HMDT c, const char *pen);
void mgl_tens(HMGL graph, const HMDT y, const HMDT c,	const char *pen);
void mgl_area_xyz(HMGL graph, const HMDT x, const HMDT y, const HMDT z, const char *pen);
void mgl_area_xy(HMGL graph, const HMDT x, const HMDT y, const char *pen);
void mgl_area_xys(HMGL graph, const HMDT x, const HMDT y, const char *pen);
void mgl_area_s(HMGL graph, const HMDT y, const char *pen);
void mgl_area(HMGL graph, const HMDT y, const char *pen);
void mgl_region_xy(HMGL graph, const HMDT x, const HMDT y1, const HMDT y2, const char *pen, int inside);
void mgl_region(HMGL graph, const HMDT y1, const HMDT y2, const char *pen, int inside);
void mgl_mark(HMGL graph, mreal x,mreal y,mreal z,char mark);
void mgl_stem_xyz(HMGL graph, const HMDT x, const HMDT y, const HMDT z, const char *pen);
void mgl_stem_xy(HMGL graph, const HMDT x, const HMDT y, const char *pen);
void mgl_stem(HMGL graph, const HMDT y,	const char *pen);
void mgl_step_xyz(HMGL graph, const HMDT x, const HMDT y, const HMDT z, const char *pen);
void mgl_step_xy(HMGL graph, const HMDT x, const HMDT y, const char *pen);
void mgl_step(HMGL graph, const HMDT y,	const char *pen);
void mgl_bars_xyz(HMGL graph, const HMDT x, const HMDT y, const HMDT z, const char *pen);
void mgl_bars_xy(HMGL graph, const HMDT x, const HMDT y, const char *pen);
void mgl_bars(HMGL graph, const HMDT y,	const char *pen);
void mgl_barh_yx(HMGL graph, const HMDT y, const HMDT v, const char *pen);
void mgl_barh(HMGL graph, const HMDT v,	const char *pen);
/*****************************************************************************/
/*		Advanced 1D plotting functions												 */
/*****************************************************************************/
void mgl_torus(HMGL graph, const HMDT r, const HMDT z, const char *pen);
void mgl_text_xyz(HMGL graph, const HMDT x, const HMDT y, const HMDT z,const char *text, const char *font, mreal size);
void mgl_text_xy(HMGL graph, const HMDT x, const HMDT y, const char *text, const char *font, mreal size);
void mgl_text_y(HMGL graph, const HMDT y, const char *text, const char *font, mreal size);
void mgl_chart(HMGL graph, const HMDT a, const char *col);
void mgl_error(HMGL graph, const HMDT y, const HMDT ey, const char *pen);
void mgl_error_xy(HMGL graph, const HMDT x, const HMDT y, const HMDT ey, const char *pen);
void mgl_error_exy(HMGL graph, const HMDT x, const HMDT y, const HMDT ex, const HMDT ey, const char *pen);
void mgl_mark_xyz(HMGL graph, const HMDT x, const HMDT y, const HMDT z, const HMDT r, const char *pen);
void mgl_mark_xy(HMGL graph, const HMDT x, const HMDT y, const HMDT r, const char *pen);
void mgl_mark_y(HMGL graph, const HMDT y, const HMDT r, const char *pen);
void mgl_tube_xyzr(HMGL graph, const HMDT x, const HMDT y, const HMDT z, const HMDT r, const char *pen);
void mgl_tube_xyr(HMGL graph, const HMDT x, const HMDT y, const HMDT r, const char *pen);
void mgl_tube_r(HMGL graph, const HMDT y, const HMDT r, const char *pen);
void mgl_tube_xyz(HMGL graph, const HMDT x, const HMDT y, const HMDT z, mreal r, const char *pen);
void mgl_tube_xy(HMGL graph, const HMDT x, const HMDT y, mreal r, const char *penl);
void mgl_tube(HMGL graph, const HMDT y, mreal r, const char *pen);

void mgl_textmark_xyzr(HMGL graph, const HMDT x, const HMDT y, const HMDT z, const HMDT r, const char *text, const char *fnt);
void mgl_textmark_xyr(HMGL graph, const HMDT x, const HMDT y, const HMDT r, const char *text, const char *fnt);
void mgl_textmark_yr(HMGL graph, const HMDT y, const HMDT r, const char *text, const char *fnt);
void mgl_textmark(HMGL graph, const HMDT y, const char *text, const char *fnt);
void mgl_textmarkw_xyzr(HMGL graph, const HMDT x, const HMDT y, const HMDT z, const HMDT r, const wchar_t *text, const char *fnt);
void mgl_textmarkw_xyr(HMGL graph, const HMDT x, const HMDT y, const HMDT r, const wchar_t *text, const char *fnt);
void mgl_textmarkw_yr(HMGL graph, const HMDT y, const HMDT r, const wchar_t *text, const char *fnt);
void mgl_textmarkw(HMGL graph, const HMDT y, const wchar_t *text, const char *fnt);
/*****************************************************************************/
/*		2D plotting functions												 */
/*****************************************************************************/
void mgl_fsurf(HMGL graph, const char *fz, const char *stl, int n);
void mgl_fsurf_xyz(HMGL graph, const char *fx, const char *fy, const char *fz, const char *stl, int n);
void mgl_grid_xy(HMGL graph, const HMDT x, const HMDT y, const HMDT z, const char *stl,mreal zVal);
void mgl_grid(HMGL graph, const HMDT a,const char *stl,mreal zVal);
void mgl_mesh_xy(HMGL graph, const HMDT x, const HMDT y, const HMDT z, const char *sch);
void mgl_mesh(HMGL graph, const HMDT z, const char *sch);
void mgl_fall_xy(HMGL graph, const HMDT x, const HMDT y, const HMDT z, const char *sch);
void mgl_fall(HMGL graph, const HMDT z, const char *sch);
void mgl_belt_xy(HMGL graph, const HMDT x, const HMDT y, const HMDT z, const char *sch);
void mgl_belt(HMGL graph, const HMDT z, const char *sch);
void mgl_surf_xy(HMGL graph, const HMDT x, const HMDT y, const HMDT z, const char *sch);
void mgl_surf(HMGL graph, const HMDT z, const char *sch);
void mgl_dens_xy(HMGL graph, const HMDT x, const HMDT y, const HMDT z, const char *sch,mreal zVal);
void mgl_dens(HMGL graph, const HMDT z, const char *sch,mreal zVal);
void mgl_boxs_xy(HMGL graph, const HMDT x, const HMDT y, const HMDT z, const char *sch,mreal zVal);
void mgl_boxs(HMGL graph, const HMDT z, const char *sch,mreal zVal);
void mgl_tile_xy(HMGL graph, const HMDT x, const HMDT y, const HMDT z, const char *sch);
void mgl_tile(HMGL graph, const HMDT z, const char *sch);
void mgl_tiles_xy(HMGL graph, const HMDT x, const HMDT y, const HMDT z, const HMDT r, const char *sch);
void mgl_tiles(HMGL graph, const HMDT z, const HMDT r, const char *sch);
void mgl_cont_xy_val(HMGL graph, const HMDT v, const HMDT x, const HMDT y, const HMDT z, const char *sch, mreal zVal);
void mgl_cont_val(HMGL graph, const HMDT v, const HMDT z, const char *sch,mreal zVal);
void mgl_cont_xy(HMGL graph, const HMDT x, const HMDT y, const HMDT z, const char *sch, int Num, mreal zVal);
void mgl_cont(HMGL graph, const HMDT z, const char *sch, int Num, mreal zVal);

void mgl_contf_xy_val(HMGL graph, const HMDT v, const HMDT x, const HMDT y, const HMDT z, const char *sch, mreal zVal);
void mgl_contf_val(HMGL graph, const HMDT v, const HMDT z, const char *sch,mreal zVal);
void mgl_contf_xy(HMGL graph, const HMDT x, const HMDT y, const HMDT z, const char *sch, int Num, mreal zVal);
void mgl_contf(HMGL graph, const HMDT z, const char *sch, int Num, mreal zVal);

void mgl_contd_xy_val(HMGL graph, const HMDT v, const HMDT x, const HMDT y, const HMDT z, const char *sch, mreal zVal);
void mgl_contd_val(HMGL graph, const HMDT v, const HMDT z, const char *sch,mreal zVal);
void mgl_contd_xy(HMGL graph, const HMDT x, const HMDT y, const HMDT z, const char *sch, int Num, mreal zVal);
void mgl_contd(HMGL graph, const HMDT z, const char *sch, int Num, mreal zVal);

void mgl_axial_xy_val(HMGL graph, const HMDT v, const HMDT x, const HMDT y, const HMDT a, const char *sch);
void mgl_axial_val(HMGL graph, const HMDT v, const HMDT a, const char *sch);
void mgl_axial_xy(HMGL graph, const HMDT x, const HMDT y, const HMDT a, const char *sch, int Num);
void mgl_axial(HMGL graph, const HMDT a, const char *sch, int Num);
/*****************************************************************************/
/*		Dual plotting functions												 */
/*****************************************************************************/
void mgl_surfc_xy(HMGL graph, const HMDT x, const HMDT y, const HMDT z, const HMDT c, const char *sch);
void mgl_surfc(HMGL graph, const HMDT z, const HMDT c, const char *sch);
void mgl_surfa_xy(HMGL graph, const HMDT x, const HMDT y, const HMDT z, const HMDT c, const char *sch);
void mgl_surfa(HMGL graph, const HMDT z, const HMDT c, const char *sch);
void mgl_stfa_xy(HMGL graph, const HMDT x, const HMDT y, const HMDT re, const HMDT im, int dn, const char *sch, mreal zVal);
void mgl_stfa(HMGL graph, const HMDT re, const HMDT im, int dn, const char *sch, mreal zVal);
void mgl_traj_xy(HMGL graph, const HMDT x, const HMDT y, const HMDT ax, const HMDT ay, const char *sch,mreal zVal,mreal len);
void mgl_traj_xyz(HMGL graph, const HMDT x, const HMDT y, const HMDT z, const HMDT ax, const HMDT ay, const HMDT az, const char *sch,mreal len);
void mgl_vect_xy(HMGL graph, const HMDT x, const HMDT y, const HMDT ax, const HMDT ay, const char *sch,mreal zVal,int flag);
void mgl_vect_2d(HMGL graph, const HMDT ax, const HMDT ay, const char *sch,mreal zVal,int flag);
void mgl_vectl_xy(HMGL graph, const HMDT x, const HMDT y, const HMDT ax, const HMDT ay, const char *sch,mreal zVal);
void mgl_vectl_2d(HMGL graph, const HMDT ax, const HMDT ay, const char *sch,mreal zVal);
void mgl_vectc_xy(HMGL graph, const HMDT x, const HMDT y, const HMDT ax, const HMDT ay, const char *sch,mreal zVal);
void mgl_vectc_2d(HMGL graph, const HMDT ax, const HMDT ay, const char *sch,mreal zVal);
void mgl_vect_xyz(HMGL graph, const HMDT x, const HMDT y, const HMDT z, const HMDT ax, const HMDT ay, const HMDT az, const char *sch,int flag);
void mgl_vect_3d(HMGL graph, const HMDT ax, const HMDT ay, const HMDT az, const char *sch,int flag);
void mgl_vectl_xyz(HMGL graph, const HMDT x, const HMDT y, const HMDT z, const HMDT ax, const HMDT ay, const HMDT az, const char *sch);
void mgl_vectl_3d(HMGL graph, const HMDT ax, const HMDT ay, const HMDT az, const char *sch);
void mgl_vectc_xyz(HMGL graph, const HMDT x, const HMDT y, const HMDT z, const HMDT ax, const HMDT ay, const HMDT az, const char *sch);
void mgl_vectc_3d(HMGL graph, const HMDT ax, const HMDT ay, const HMDT az, const char *sch);
void mgl_map_xy(HMGL graph, const HMDT x, const HMDT y, const HMDT a, const HMDT b, const char *sch, int ks, int pnts);
void mgl_map(HMGL graph, const HMDT a, const HMDT b, const char *sch, int ks, int pnts);
void mgl_surf3a_xyz_val(HMGL graph, mreal Val, const HMDT x, const HMDT y, const HMDT z, const HMDT a, const HMDT b, const char *stl);
void mgl_surf3a_val(HMGL graph, mreal Val, const HMDT a, const HMDT b, const char *stl);
void mgl_surf3a_xyz(HMGL graph, const HMDT x, const HMDT y, const HMDT z, const HMDT a, const HMDT b, const char *stl, int num);
void mgl_surf3a(HMGL graph, const HMDT a, const HMDT b, const char *stl, int num);
void mgl_surf3c_xyz_val(HMGL graph, mreal Val, const HMDT x, const HMDT y, const HMDT z, const HMDT a, const HMDT b, const char *stl);
void mgl_surf3c_val(HMGL graph, mreal Val, const HMDT a, const HMDT b, const char *stl);
void mgl_surf3c_xyz(HMGL graph, const HMDT x, const HMDT y, const HMDT z, const HMDT a, const HMDT b,
			const char *stl, int num);
void mgl_surf3c(HMGL graph, const HMDT a, const HMDT b, const char *stl, int num);
void mgl_flow_xy(HMGL graph, const HMDT x, const HMDT y, const HMDT ax, const HMDT ay, const char *sch, int num, int central, mreal zVal);
void mgl_flow_2d(HMGL graph, const HMDT ax, const HMDT ay, const char *sch, int num, int central, mreal zVal);
void mgl_flow_xyz(HMGL graph, const HMDT x, const HMDT y, const HMDT z, const HMDT ax, const HMDT ay, const HMDT az, const char *sch, int num, int central);
void mgl_flow_3d(HMGL graph, const HMDT ax, const HMDT ay, const HMDT az, const char *sch, int num, int central);

void mgl_flowp_xy(HMGL graph, mreal x0, mreal y0, mreal z0, const HMDT x, const HMDT y, const HMDT ax, const HMDT ay, const char *sch);
void mgl_flowp_2d(HMGL graph, mreal x0, mreal y0, mreal z0, const HMDT ax, const HMDT ay, const char *sch);
void mgl_flowp_xyz(HMGL graph, mreal x0, mreal y0, mreal z0, const HMDT x, const HMDT y, const HMDT z, const HMDT ax, const HMDT ay, const HMDT az, const char *sch);
void mgl_flowp_3d(HMGL graph, mreal x0, mreal y0, mreal z0, const HMDT ax, const HMDT ay, const HMDT az, const char *sch);

void mgl_pipe_xy(HMGL graph, const HMDT x, const HMDT y, const HMDT ax, const HMDT ay, const char *sch, mreal r0, int num, int central, mreal zVal);
void mgl_pipe_2d(HMGL graph, const HMDT ax, const HMDT ay, const char *sch, mreal r0, int num, int central, mreal zVal);
void mgl_pipe_xyz(HMGL graph, const HMDT x, const HMDT y, const HMDT z, const HMDT ax, const HMDT ay, const HMDT az, const char *sch, mreal r0, int num, int central);
void mgl_pipe_3d(HMGL graph, const HMDT ax, const HMDT ay, const HMDT az, const char *sch, mreal r0, int num, int central);
void mgl_dew_xy(HMGL gr, const HMDT x, const HMDT y, const HMDT ax, const HMDT ay, const char *sch,mreal zVal);
void mgl_dew_2d(HMGL gr, const HMDT ax, const HMDT ay, const char *sch,mreal zVal);

void mgl_grad_xyz(HMGL graph, const HMDT x, const HMDT y, const HMDT z, const HMDT ph, const char *sch, int num);
void mgl_grad_xy(HMGL graph, const HMDT x, const HMDT y, const HMDT ph, const char *sch, int num, mreal zVal);
void mgl_grad(HMGL graph, const HMDT ph, const char *sch, int num, mreal zVal);
/*****************************************************************************/
/*		3D plotting functions												 */
/*****************************************************************************/
void mgl_grid3_xyz(HMGL graph, const HMDT x, const HMDT y, const HMDT z, const HMDT a, char dir, int sVal, const char *stl);
void mgl_grid3(HMGL graph, const HMDT a, char dir, int sVal, const char *stl);
void mgl_grid3_all_xyz(HMGL graph, const HMDT x, const HMDT y, const HMDT z, const HMDT a, const char *stl);
void mgl_grid3_all(HMGL graph, const HMDT a, const char *stl);
void mgl_dens3_xyz(HMGL graph, const HMDT x, const HMDT y, const HMDT z, const HMDT a, char dir, int sVal, const char *stl);
void mgl_dens3(HMGL graph, const HMDT a, char dir, int sVal, const char *stl);
void mgl_dens3_all_xyz(HMGL graph, const HMDT x, const HMDT y, const HMDT z, const HMDT a, const char *stl);
void mgl_dens3_all(HMGL graph, const HMDT a, const char *stl);
void mgl_surf3_xyz_val(HMGL graph, mreal Val, const HMDT x, const HMDT y, const HMDT z, const HMDT a, const char *stl);
void mgl_surf3_val(HMGL graph, mreal Val, const HMDT a, const char *stl);
void mgl_surf3_xyz(HMGL graph, const HMDT x, const HMDT y, const HMDT z, const HMDT a, const char *stl, int num);
void mgl_surf3(HMGL graph, const HMDT a, const char *stl, int num);
void mgl_cont3_xyz_val(HMGL graph, const HMDT v, const HMDT x, const HMDT y, const HMDT z, const HMDT a, char dir, int sVal, const char *sch);
void mgl_cont3_val(HMGL graph, const HMDT v, const HMDT a, char dir, int sVal, const char *sch);
void mgl_cont3_xyz(HMGL graph, const HMDT x, const HMDT y, const HMDT z, const HMDT a, char dir, int sVal, const char *sch, int Num);
void mgl_cont3(HMGL graph, const HMDT a, char dir, int sVal, const char *sch, int Num);
void mgl_cont_all_xyz(HMGL graph, const HMDT x, const HMDT y, const HMDT z, const HMDT a, const char *sch, int Num);
void mgl_cont_all(HMGL graph, const HMDT a, const char *sch, int Num);
void mgl_cloudp_xyz(HMGL graph, const HMDT x, const HMDT y, const HMDT z, const HMDT a, const char *stl, mreal alpha);
void mgl_cloudp(HMGL graph, const HMDT a, const char *stl, mreal alpha);
void mgl_cloud_xyz(HMGL graph, const HMDT x, const HMDT y, const HMDT z, const HMDT a, const char *stl, mreal alpha);
void mgl_cloud(HMGL graph, const HMDT a, const char *stl, mreal alpha);
void mgl_contf3_xyz_val(HMGL graph, const HMDT v, const HMDT x, const HMDT y, const HMDT z, const HMDT a, char dir, int sVal, const char *sch);
void mgl_contf3_val(HMGL graph, const HMDT v, const HMDT a, char dir, int sVal, const char *sch);
void mgl_contf3_xyz(HMGL graph, const HMDT x, const HMDT y, const HMDT z, const HMDT a, char dir, int sVal, const char *sch, int Num);
void mgl_contf3(HMGL graph, const HMDT a, char dir, int sVal, const char *sch, int Num);
void mgl_contf_all_xyz(HMGL graph, const HMDT x, const HMDT y, const HMDT z, const HMDT a, const char *sch, int Num);
void mgl_contf_all(HMGL graph, const HMDT a, const char *sch, int Num);
void mgl_beam_val(HMGL graph, mreal Val, const HMDT tr, const HMDT g1, const HMDT g2, const HMDT a, mreal r, const char *stl, int norm);
void mgl_beam(HMGL graph, const HMDT tr, const HMDT g1, const HMDT g2, const HMDT a, mreal r, const char *stl, int norm, int num);
/*****************************************************************************/
/*		Triangular plotting functions											 */
/*****************************************************************************/
void mgl_triplot_xyzc(HMGL gr, const HMDT nums, const HMDT x, const HMDT y, const HMDT z, const HMDT c, const char *sch);
void mgl_triplot_xyz(HMGL gr, const HMDT nums, const HMDT x, const HMDT y, const HMDT z, const char *sch);
void mgl_triplot_xy(HMGL gr, const HMDT nums, const HMDT x, const HMDT y, const char *sch, mreal zVal);
void mgl_quadplot_xyzc(HMGL gr, const HMDT nums, const HMDT x, const HMDT y, const HMDT z, const HMDT c, const char *sch);
void mgl_quadplot_xyz(HMGL gr, const HMDT nums, const HMDT x, const HMDT y, const HMDT z, const char *sch);
void mgl_quadplot_xy(HMGL gr, const HMDT nums, const HMDT x, const HMDT y, const char *sch, mreal zVal);
void mgl_tricont_xyzcv(HMGL gr, const HMDT v, const HMDT nums, const HMDT x, const HMDT y, const HMDT z, const HMDT c, const char *sch, mreal zVal);
void mgl_tricont_xyzv(HMGL gr, const HMDT v, const HMDT nums, const HMDT x, const HMDT y, const HMDT z, const char *sch, mreal zVal);
void mgl_tricont_xyzc(HMGL gr, const HMDT nums, const HMDT x, const HMDT y, const HMDT z, const HMDT c, const char *sch, int n, mreal zVal);
void mgl_tricont_xyz(HMGL gr, const HMDT nums, const HMDT x, const HMDT y, const HMDT z, const char *sch, int n, mreal zVal);
void mgl_dots(HMGL gr, const HMDT x, const HMDT y, const HMDT z, const char *sch);
void mgl_dots_a(HMGL gr, const HMDT x, const HMDT y, const HMDT z, const HMDT a, const char *sch);
void mgl_dots_tr(HMGL gr, const HMDT tr, const char *sch);
void mgl_crust(HMGL gr, const HMDT x, const HMDT y, const HMDT z, const char *sch, mreal er);
void mgl_crust_tr(HMGL gr, const HMDT tr, const char *sch, mreal er);
/*****************************************************************************/
/*		Combined plotting functions											 */
/*****************************************************************************/
void mgl_dens_x(HMGL graph, const HMDT a, const char *stl, mreal sVal);
void mgl_dens_y(HMGL graph, const HMDT a, const char *stl, mreal sVal);
void mgl_dens_z(HMGL graph, const HMDT a, const char *stl, mreal sVal);
void mgl_cont_x(HMGL graph, const HMDT a, const char *stl, mreal sVal, int Num);
void mgl_cont_y(HMGL graph, const HMDT a, const char *stl, mreal sVal, int Num);
void mgl_cont_z(HMGL graph, const HMDT a, const char *stl, mreal sVal, int Num);
void mgl_cont_x_val(HMGL graph, const HMDT v, const HMDT a, const char *stl, mreal sVal);
void mgl_cont_y_val(HMGL graph, const HMDT v, const HMDT a, const char *stl, mreal sVal);
void mgl_cont_z_val(HMGL graph, const HMDT v, const HMDT a, const char *stl, mreal sVal);
void mgl_contf_x(HMGL graph, const HMDT a, const char *stl, mreal sVal, int Num);
void mgl_contf_y(HMGL graph, const HMDT a, const char *stl, mreal sVal, int Num);
void mgl_contf_z(HMGL graph, const HMDT a, const char *stl, mreal sVal, int Num);
void mgl_contf_x_val(HMGL graph, const HMDT v, const HMDT a, const char *stl, mreal sVal);
void mgl_contf_y_val(HMGL graph, const HMDT v, const HMDT a, const char *stl, mreal sVal);
void mgl_contf_z_val(HMGL graph, const HMDT v, const HMDT a, const char *stl, mreal sVal);
/*****************************************************************************/
/*		Data creation functions												 */
/*****************************************************************************/
void mgl_data_rearrange(HMDT dat, int mx, int my, int mz);
void mgl_data_set_float(HMDT dat, const float *A,int NX,int NY,int NZ);
void mgl_data_set_double(HMDT dat, const double *A,int NX,int NY,int NZ);
void mgl_data_set_float2(HMDT d, const float **A,int N1,int N2);
void mgl_data_set_double2(HMDT d, const double **A,int N1,int N2);
void mgl_data_set_float3(HMDT d, const float ***A,int N1,int N2,int N3);
void mgl_data_set_double3(HMDT d, const double ***A,int N1,int N2,int N3);
void mgl_data_set(HMDT dat, const HMDT a);
void mgl_data_set_vector(HMDT dat, gsl_vector *v);
void mgl_data_set_matrix(HMDT dat, gsl_matrix *m);

mreal mgl_data_get_value(const HMDT dat, int i, int j, int k);
int mgl_data_get_nx(const HMDT dat);
int mgl_data_get_ny(const HMDT dat);
int mgl_data_get_nz(const HMDT dat);
void mgl_data_set_value(HMDT dat, mreal v, int i, int j, int k);
void mgl_data_set_values(HMDT dat, const char *val, int nx, int ny, int nz);
int mgl_data_read(HMDT dat, const char *fname);
int mgl_data_read_mat(HMDT dat, const char *fname, int dim);
int mgl_data_read_dim(HMDT dat, const char *fname,int mx,int my,int mz);
void mgl_data_save(HMDT dat, const char *fname,int ns);
void mgl_data_export(HMDT dat, const char *fname, const char *scheme,mreal v1,mreal v2,int ns);
void mgl_data_import(HMDT dat, const char *fname, const char *scheme,mreal v1,mreal v2);
void mgl_data_create(HMDT dat, int nx,int ny,int nz);
void mgl_data_transpose(HMDT dat, const char *dim);
void mgl_data_norm(HMDT dat, mreal v1,mreal v2,int sym,int dim);
void mgl_data_norm_slice(HMDT dat, mreal v1,mreal v2,char dir,int keep_en,int sym);
HMDT mgl_data_subdata(const HMDT dat, int xx,int yy,int zz);
HMDT mgl_data_subdata_ext(const HMDT dat, const HMDT xx, const HMDT yy, const HMDT zz);
HMDT mgl_data_column(const HMDT dat, const char *eq);
void mgl_data_set_id(HMDT d, const char *id);
void mgl_data_fill(HMDT dat, mreal x1,mreal x2,char dir);
void mgl_data_fill_eq(HMGL gr, HMDT dat, const char *eq, const HMDT vdat, const HMDT wdat);
void mgl_data_put_val(HMDT dat, mreal val, int i, int j, int k);
void mgl_data_put_dat(HMDT dat, const HMDT val, int i, int j, int k);
void mgl_data_modify(HMDT dat, const char *eq,int dim);
void mgl_data_modify_vw(HMDT dat, const char *eq,const HMDT vdat,const HMDT wdat);
void mgl_data_squeeze(HMDT dat, int rx,int ry,int rz,int smooth);
mreal mgl_data_max(const HMDT dat);
mreal mgl_data_min(const HMDT dat);
mreal *mgl_data_value(HMDT dat, int i,int j,int k);
const mreal *mgl_data_data(const HMDT dat);

mreal mgl_data_first(const HMDT dat, const char *cond, int *i, int *j, int *k);
mreal mgl_data_last(const HMDT dat, const char *cond, int *i, int *j, int *k);
int mgl_data_find(const HMDT dat, const char *cond, char dir, int i, int j, int k);
int mgl_data_find_any(const HMDT dat, const char *cond);
mreal mgl_data_max_int(const HMDT dat, int *i, int *j, int *k);
mreal mgl_data_max_real(const HMDT dat, mreal *x, mreal *y, mreal *z);
mreal mgl_data_min_int(const HMDT dat, int *i, int *j, int *k);
mreal mgl_data_min_real(const HMDT dat, mreal *x, mreal *y, mreal *z);
mreal mgl_data_momentum_mw(const HMDT dat, char dir, mreal *m, mreal *w);

HMDT mgl_data_combine(const HMDT dat1, const HMDT dat2);
void mgl_data_extend(HMDT dat, int n1, int n2);
void mgl_data_insert(HMDT dat, char dir, int at, int num);
void mgl_data_delete(HMDT dat, char dir, int at, int num);
/*****************************************************************************/
/*		Data manipulation functions											 */
/*****************************************************************************/
void mgl_data_smooth(HMDT dat, int Type,mreal delta,const char *dirs);
HMDT mgl_data_sum(const HMDT dat, const char *dir);
HMDT mgl_data_max_dir(const HMDT dat, const char *dir);
HMDT mgl_data_min_dir(const HMDT dat, const char *dir);
void mgl_data_cumsum(HMDT dat, const char *dir);
void mgl_data_integral(HMDT dat, const char *dir);
void mgl_data_diff(HMDT dat, const char *dir);
void mgl_data_diff_par(HMDT dat, const HMDT v1, const HMDT v2, const HMDT v3);
void mgl_data_diff2(HMDT dat, const char *dir);
void mgl_data_swap(HMDT dat, const char *dir);
void mgl_data_roll(HMDT dat, char dir, int num);
void mgl_data_mirror(HMDT dat, const char *dir);

void mgl_data_hankel(HMDT dat, const char *dir);
void mgl_data_sinfft(HMDT dat, const char *dir);
void mgl_data_cosfft(HMDT dat, const char *dir);
void mgl_data_fill_sample(HMDT dat, int num, const char *how);

mreal mgl_data_spline(const HMDT dat, mreal x,mreal y,mreal z);
mreal mgl_data_spline1(const HMDT dat, mreal x,mreal y,mreal z);
mreal mgl_data_linear(const HMDT dat, mreal x,mreal y,mreal z);
mreal mgl_data_linear1(const HMDT dat, mreal x,mreal y,mreal z);
HMDT mgl_data_resize(const HMDT dat, int mx,int my,int mz);
HMDT mgl_data_resize_box(const HMDT dat, int mx,int my,int mz,mreal x1,mreal x2,
	mreal y1,mreal y2,mreal z1,mreal z2);
HMDT mgl_data_hist(const HMDT dat, int n, mreal v1, mreal v2, int nsub);
HMDT mgl_data_hist_w(const HMDT dat, const HMDT weight, int n, mreal v1, mreal v2, int nsub);
HMDT mgl_data_momentum(const HMDT dat, char dir, const char *how);
HMDT mgl_data_evaluate_i(const HMDT dat, const HMDT idat, int norm);
HMDT mgl_data_evaluate_ij(const HMDT dat, const HMDT idat, const HMDT jdat, int norm);
HMDT mgl_data_evaluate_ijk(const HMDT dat, const HMDT idat, const HMDT jdat, const HMDT kdat, int norm);
void mgl_data_envelop(HMDT dat, char dir);
void mgl_data_sew(HMDT dat, const char *dirs, mreal da);
void mgl_data_crop(HMDT dat, int n1, int n2, char dir);
/*****************************************************************************/
/*		Data operations														 */
/*****************************************************************************/
void mgl_data_mul_dat(HMDT dat, const HMDT d);
void mgl_data_div_dat(HMDT dat, const HMDT d);
void mgl_data_add_dat(HMDT dat, const HMDT d);
void mgl_data_sub_dat(HMDT dat, const HMDT d);
void mgl_data_mul_num(HMDT dat, mreal d);
void mgl_data_div_num(HMDT dat, mreal d);
void mgl_data_add_num(HMDT dat, mreal d);
void mgl_data_sub_num(HMDT dat, mreal d);
/*****************************************************************************/
/*		Nonlinear fitting													 */
/*****************************************************************************/
void mgl_hist_x(HMGL gr, HMDT res, const HMDT x, const HMDT a);
void mgl_hist_xy(HMGL gr, HMDT res, const HMDT x, const HMDT y, const HMDT a);
void mgl_hist_xyz(HMGL gr, HMDT res, const HMDT x, const HMDT y, const HMDT z, const HMDT a);
/*****************************************************************************/
/*		Nonlinear fitting													 */
/*****************************************************************************/
mreal mgl_fit_1(HMGL gr, HMDT fit, const HMDT y, const char *eq, const char *var, mreal *ini);
mreal mgl_fit_2(HMGL gr, HMDT fit, const HMDT z, const char *eq, const char *var, mreal *ini);
mreal mgl_fit_3(HMGL gr, HMDT fit, const HMDT a, const char *eq, const char *var, mreal *ini);
mreal mgl_fit_xy(HMGL gr, HMDT fit, const HMDT x, const HMDT y, const char *eq, const char *var, mreal *ini);
mreal mgl_fit_xyz(HMGL gr, HMDT fit, const HMDT x, const HMDT y, const HMDT z, const char *eq, const char *var, mreal *ini);
mreal mgl_fit_xyza(HMGL gr, HMDT fit, const HMDT x, const HMDT y, const HMDT z, const HMDT a, const char *eq, const char *var, mreal *ini);
mreal mgl_fit_ys(HMGL gr, HMDT fit, const HMDT y, const HMDT s, const char *eq, const char *var, mreal *ini);
mreal mgl_fit_xys(HMGL gr, HMDT fit, const HMDT x, const HMDT y, const HMDT s, const char *eq, const char *var, mreal *ini);
mreal mgl_fit_xyzs(HMGL gr, HMDT fit, const HMDT x, const HMDT y, const HMDT z, const HMDT s, const char *eq, const char *var, mreal *ini);
mreal mgl_fit_xyzas(HMGL gr, HMDT fit, const HMDT x, const HMDT y, const HMDT z, const HMDT a, const HMDT s, const char *eq, const char *var, mreal *ini);

mreal mgl_fit_1_d(HMGL gr, HMDT fit, const HMDT y, const char *eq, const char *var, HMDT ini);
mreal mgl_fit_2_d(HMGL gr, HMDT fit, const HMDT z, const char *eq, const char *var, HMDT ini);
mreal mgl_fit_3_d(HMGL gr, HMDT fit, const HMDT a, const char *eq, const char *var, HMDT ini);
mreal mgl_fit_xy_d(HMGL gr, HMDT fit, const HMDT x, const HMDT y, const char *eq, const char *var, HMDT ini);
mreal mgl_fit_xyz_d(HMGL gr, HMDT fit, const HMDT x, const HMDT y, const HMDT z, const char *eq, const char *var, HMDT ini);
mreal mgl_fit_xyza_d(HMGL gr, HMDT fit, const HMDT x, const HMDT y, const HMDT z, const HMDT a, const char *eq, const char *var, HMDT ini);
mreal mgl_fit_ys_d(HMGL gr, HMDT fit, const HMDT y, const HMDT s, const char *eq, const char *var, HMDT ini);
mreal mgl_fit_xys_d(HMGL gr, HMDT fit, const HMDT x, const HMDT y, const HMDT s, const char *eq, const char *var, HMDT ini);
mreal mgl_fit_xyzs_d(HMGL gr, HMDT fit, const HMDT x, const HMDT y, const HMDT z, const HMDT s, const char *eq, const char *var, HMDT ini);
mreal mgl_fit_xyzas_d(HMGL gr, HMDT fit, const HMDT x, const HMDT y, const HMDT z, const HMDT a, const HMDT s, const char *eq, const char *var, HMDT ini);

void mgl_puts_fit(HMGL gr, mreal x, mreal y, mreal z, const char *prefix, const char *font, mreal size);
const char *mgl_get_fit(HMGL gr);
/*****************************************************************************/
void mgl_sphere(HMGL graph, mreal x, mreal y, mreal z, mreal r, const char *stl);
void mgl_drop(HMGL graph, mreal x1, mreal y1, mreal z1, mreal x2, mreal y2, mreal z2, mreal r, const char *stl, mreal shift, mreal ap);
void mgl_cone(HMGL graph, mreal x1, mreal y1, mreal z1, mreal x2, mreal y2, mreal z2, mreal r1, mreal r2, const char *stl, int edge);

HMDT mgl_pde_solve(HMGL gr, const char *ham, const HMDT ini_re, const HMDT ini_im, mreal dz, mreal k0);
HMDT mgl_qo2d_solve(const char *ham, const HMDT ini_re, const HMDT ini_im, const HMDT ray, mreal r, mreal k0, HMDT xx, HMDT yy);
HMDT mgl_af2d_solve(const char *ham, const HMDT ini_re, const HMDT ini_im, const HMDT ray, mreal r, mreal k0, HMDT xx, HMDT yy);
HMDT mgl_ray_trace(const char *ham, mreal x0, mreal y0, mreal z0, mreal px, mreal py, mreal pz, mreal dt, mreal tmax);
HMDT mgl_jacobian_2d(const HMDT x, const HMDT y);
HMDT mgl_jacobian_3d(const HMDT x, const HMDT y, const HMDT z);
HMDT mgl_transform_a(const HMDT am, const HMDT ph, const char *tr);
HMDT mgl_transform(const HMDT re, const HMDT im, const char *tr);
HMDT mgl_data_stfa(const HMDT re, const HMDT im, int dn, char dir);

#ifdef __cplusplus
}
#endif

#endif /* _mgl_c_h_ */
