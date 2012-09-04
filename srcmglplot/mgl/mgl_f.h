/***************************************************************************
 * mgl_f.h is part of Math Graphic Library
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
#ifndef _MGL_F_H_
#define _MGL_F_H_

#ifndef _MSC_VER
#include <stdint.h>
#endif
#include <mgl/config.h>

#if(MGL_USE_DOUBLE==1)
typedef double mreal;
#else
typedef float mreal;
#endif

#ifdef __cplusplus
extern "C" {
#endif
#define _GR_	((mglGraph *)(*gr))
#define _PR_	((mglParse *)(*p))
#define _DT_	((mglData *)*d)
#define _D_(d)	*((const mglData *)*(d))
#define _DM_(d)	*((mglData *)*(d))
/*****************************************************************************/
uintptr_t mgl_create_graph_gl_();
uintptr_t mgl_create_graph_idtf_();
/*uintptr_t mgl_create_graph_glut_(int argc, char **argv, int _(*draw)_(mglGraph *gr, const void *p), const char *title, void _(*reload)_(int *next), const void *par);
uintptr_t mgl_create_graph_fltk_(int argc, char **argv, int _(*draw)_(mglGraph *gr, const void *p), const char *title, void _(*reload)_(int *next), const void *par);*/
uintptr_t mgl_create_graph_zb_(int *width, int *height);
uintptr_t mgl_create_graph_ps_(int *width, int *height);
void mgl_delete_graph_(uintptr_t *graph);

uintptr_t mgl_create_graph_fltk_(const char *title, int);
uintptr_t mgl_create_graph_qt_(const char *title, int);
void mgl_fltk_run_();
void mgl_qt_run_();
void mgl_set_show_mouse_pos_(uintptr_t *gr, int *enable);
void mgl_get_last_mouse_pos_(uintptr_t *gr, mreal *x, mreal *y, mreal *z);
void mgl_calc_xyz_(uintptr_t *gr, int *xs, int *ys, mreal *x, mreal *y, mreal *z);
void mgl_calc_scr_(uintptr_t *gr, mreal *x, mreal *y, mreal *z, int *xs, int *ys);
//void mgl_fltk_thread_();
//void mgl_qt_thread_();

void mgl_wnd_set_delay_(uintptr_t *gr, mreal *dt);
void mgl_wnd_set_auto_clf_(uintptr_t *gr, int *val);
void mgl_wnd_set_show_mouse_pos_(uintptr_t *gr, int *val);
void mgl_wnd_set_clf_update_(uintptr_t *gr, int *val);
void mgl_wnd_toggle_alpha_(uintptr_t *gr);
void mgl_wnd_toggle_light_(uintptr_t *gr);
void mgl_wnd_toggle_zoom_(uintptr_t *gr);
void mgl_wnd_toggle_rotate_(uintptr_t *gr);
void mgl_wnd_toggle_no_(uintptr_t *gr);
void mgl_wnd_update_(uintptr_t *gr);
void mgl_wnd_reload_(uintptr_t *gr, int *o);
void mgl_wnd_adjust_(uintptr_t *gr);
void mgl_wnd_next_frame_(uintptr_t *gr);
void mgl_wnd_prev_frame_(uintptr_t *gr);
void mgl_wnd_animation_(uintptr_t *gr);
/*****************************************************************************/
uintptr_t mgl_create_data_();
uintptr_t mgl_create_data_size_(int *nx, int *ny, int *nz);
uintptr_t mgl_create_data_file_(const char *fname, int len);
void mgl_delete_data_(uintptr_t *dat);
/*****************************************************************************/
/*		Setup mglGraph														 */
/*****************************************************************************/
void mgl_set_def_param_(uintptr_t *gr);
void mgl_set_palette_(uintptr_t *gr, const char *colors, int);
void mgl_set_pal_color_(uintptr_t *graph, int *n, mreal *r, mreal *g, mreal *b);
void mgl_set_pal_num_(uintptr_t *graph, int *num);
void mgl_set_rotated_text_(uintptr_t *graph, int *rotated);
void mgl_set_cut_(uintptr_t *graph, int *cut);
void mgl_set_cut_box_(uintptr_t *gr, mreal *x1,mreal *y1,mreal *z1,mreal *x2,mreal *y2,mreal *z2);
void mgl_set_tick_len_(uintptr_t *graph, mreal *len, mreal *stt);
void mgl_set_tick_stl_(uintptr_t *graph, const char *stl, const char *sub, int,int);
void mgl_set_bar_width_(uintptr_t *graph, mreal *width);
void mgl_set_base_line_width_(uintptr_t *gr, mreal *size);
void mgl_set_mark_size_(uintptr_t *graph, mreal *size);
void mgl_set_arrow_size_(uintptr_t *graph, mreal *size);
void mgl_set_font_size_(uintptr_t *graph, mreal *size);
void mgl_set_font_def_(uintptr_t *graph, const char *fnt, int);
void mgl_set_alpha_default_(uintptr_t *graph, mreal *alpha);
void mgl_set_size_(uintptr_t *graph, int *width, int *height);
void mgl_set_axial_dir_(uintptr_t *graph, const char *dir,int);
void mgl_set_meshnum_(uintptr_t *graph, int *num);
void mgl_set_zoom_(uintptr_t *gr, mreal *x1, mreal *y1, mreal *x2, mreal *y2);
void mgl_set_plotfactor_(uintptr_t *gr, mreal *val);
void mgl_set_draw_face_(uintptr_t *gr, int *enable);
void mgl_set_scheme_(uintptr_t *gr, char *sch, int);
void mgl_load_font_(uintptr_t *gr, char *name, char *path, int l, int n);
void mgl_copy_font_(uintptr_t *gr, uintptr_t *gr_from);
void mgl_restore_font_(uintptr_t *gr);
int mgl_get_warn_(uintptr_t *gr);
/*****************************************************************************/
/*		Export to file or to memory														 */
/*****************************************************************************/
void mgl_show_image_(uintptr_t *graph, const char *viewer, int *keep, int);
void mgl_write_frame_(uintptr_t *graph, const char *fname,const char *descr,int lf,int ld);
void mgl_write_bmp_(uintptr_t *graph, const char *fname,const char *descr,int lf,int ld);
void mgl_write_jpg_(uintptr_t *graph, const char *fname,const char *descr,int lf,int ld);
void mgl_write_png_(uintptr_t *graph, const char *fname,const char *descr,int lf,int ld);
void mgl_write_png_solid_(uintptr_t *graph, const char *fname,const char *descr,int lf,int ld);
void mgl_write_eps_(uintptr_t *graph, const char *fname,const char *descr,int lf,int ld);
void mgl_write_svg_(uintptr_t *graph, const char *fname,const char *descr,int lf,int ld);
void mgl_write_idtf_(uintptr_t *graph, const char *fname,const char *descr,int lf,int ld);
void mgl_write_gif_(uintptr_t *graph, const char *fname,const char *descr,int lf,int ld);
void mgl_start_gif_(uintptr_t *graph, const char *fname,int *ms,int l);
void mgl_close_gif_(uintptr_t *graph);
const unsigned char *mgl_get_rgb_(uintptr_t *graph);
const unsigned char *mgl_get_rgba_(uintptr_t *graph);
int mgl_get_width_(uintptr_t *graph);
int mgl_get_height_(uintptr_t *graph);
/*****************************************************************************/
/*		Setup frames transparency _(alpha) and lightning						 */
/*****************************************************************************/
int mgl_new_frame_(uintptr_t *graph);
void mgl_end_frame_(uintptr_t *graph);
int mgl_get_num_frame_(uintptr_t *graph);
void mgl_reset_frames_(uintptr_t *graph);
void mgl_set_transp_type_(uintptr_t *graph, int *type);
void mgl_set_transp_(uintptr_t *graph, int *enable);
void mgl_set_alpha_(uintptr_t *graph, int *enable);
void mgl_set_fog_(uintptr_t *graph, mreal *dist, mreal *dz);
void mgl_set_light_(uintptr_t *graph, int *enable);
void mgl_set_light_n_(uintptr_t *gr, int *n, int *enable);
void mgl_add_light_(uintptr_t *graph, int *n, mreal *x, mreal *y, mreal *z, char *c, int);
void mgl_add_light_rgb_(uintptr_t *graph, int *n, mreal *x, mreal *y, mreal *z, int *infty,
						mreal *r, mreal *g, mreal *b, mreal *i);
void mgl_set_ambbr_(uintptr_t *gr, mreal *i);
/*****************************************************************************/
/*		Scale and rotate													 */
/*****************************************************************************/
void mgl_mat_push_(uintptr_t *gr);
void mgl_mat_pop_(uintptr_t *gr);
void mgl_identity_(uintptr_t *graph, int *rel);
void mgl_clf_(uintptr_t *graph);
void mgl_flush_(uintptr_t *gr);
void mgl_clf_rgb_(uintptr_t *graph, mreal *r, mreal *g, mreal *b);
void mgl_subplot_(uintptr_t *graph, int *nx,int *ny,int *m);
void mgl_subplot_d_(uintptr_t *graph, int *nx,int *ny,int *m,mreal *dx,mreal *dy);
void mgl_subplot_s_(uintptr_t *graph, int *nx,int *ny,int *m, const char *s,int);
void mgl_inplot_(uintptr_t *graph, mreal *x1,mreal *x2,mreal *y1,mreal *y2);
void mgl_relplot_(uintptr_t *graph, mreal *x1,mreal *x2,mreal *y1,mreal *y2);
void mgl_columnplot_(uintptr_t *graph, int *num, int *i);
void mgl_columnplot_d_(uintptr_t *graph, int *num, int *i, mreal *d);
void mgl_stickplot_(uintptr_t *graph, int *num, int *i, mreal *tet, mreal *phi);
void mgl_aspect_(uintptr_t *graph, mreal *Ax,mreal *Ay,mreal *Az);
void mgl_rotate_(uintptr_t *graph, mreal *TetX,mreal *TetZ,mreal *TetY);
void mgl_rotate_vector_(uintptr_t *graph, mreal *Tet,mreal *x,mreal *y,mreal *z);
void mgl_perspective_(uintptr_t *graph, mreal val);
/*****************************************************************************/
/*		Axis functions														 */
/*****************************************************************************/
void mgl_tune_ticks_(uintptr_t *gr, int *tune, mreal *fact_pos);
void mgl_adjust_ticks_(uintptr_t *gr, const char *dir, int l);
void mgl_set_ticks_dir_(uintptr_t *gr, char *dir, mreal *d, int *ns, mreal *org, int);
void mgl_set_ticks_(uintptr_t *graph, mreal *DX, mreal *DY, mreal *DZ);
void mgl_set_subticks_(uintptr_t *graph, int *NX, int *NY, int *NZ);
void mgl_set_caxis_(uintptr_t *graph, mreal *C1,mreal *C2);
void mgl_set_axis_(uintptr_t *graph, mreal *x1, mreal *y1, mreal *z1, mreal *x2, mreal *y2, mreal *z2, mreal *x0, mreal *y0, mreal *z0);
void mgl_set_axis_3d_(uintptr_t *graph, mreal *x1, mreal *y1, mreal *z1, mreal *x2, mreal *y2, mreal *z2);
void mgl_set_axis_2d_(uintptr_t *graph, mreal *x1, mreal *y1, mreal *x2, mreal *y2);
inline void mgl_set_ranges_(uintptr_t *graph, mreal *x1, mreal *x2, mreal *y1, mreal *y2, mreal *z1, mreal *z2)
{	mgl_set_axis_3d_(graph, x1,y1,z1,x2,y2,z2);	};
void mgl_set_origin_(uintptr_t *graph, mreal *x0, mreal *y0, mreal *z0);
void mgl_set_tick_origin_(uintptr_t *graph, mreal *x0, mreal *y0, mreal *z0);
void mgl_set_crange_(uintptr_t *graph, uintptr_t *a, int *add);
void mgl_set_xrange_(uintptr_t *graph, uintptr_t *a, int *add);
void mgl_set_yrange_(uintptr_t *graph, uintptr_t *a, int *add);
void mgl_set_zrange_(uintptr_t *graph, uintptr_t *a, int *add);
void mgl_set_auto_(uintptr_t *graph, mreal *x1, mreal *x2, mreal *y1, mreal *y2, mreal *z1, mreal *z2);
void mgl_set_func_(uintptr_t *graph, const char *EqX, const char *EqY, const char *EqZ, int, int, int);
void mgl_set_func_ext_(uintptr_t *graph, const char *EqX, const char *EqY, const char *EqZ, const char *EqA, int, int, int, int);
void mgl_set_coor_(uintptr_t *gr, int *how);
void mgl_set_ternary_(uintptr_t *graph, int *enable);
void mgl_set_cutoff_(uintptr_t *graph, const char *EqC, int);
void mgl_box_(uintptr_t *graph, int *ticks);
void mgl_box_str_(uintptr_t *graph, const char *col, int *ticks, int);
void mgl_box_rgb_(uintptr_t *graph, mreal *r, mreal *g, mreal *b, int *ticks);
void mgl_axis_(uintptr_t *graph, const char *dir,int);
void mgl_axis_grid_(uintptr_t *graph, const char *dir,const char *pen,int,int);
void mgl_label_(uintptr_t *graph, const char *dir, const char *text,int,int);
void mgl_label_ext_(uintptr_t *graph, const char *dir, const char *text, mreal *pos, mreal *size, mreal *shift,int,int);
void mgl_label_xy_(uintptr_t *graph, mreal *x, mreal *y, const char *text, const char *fnt, mreal *size,int,int);
void mgl_set_xtt_(uintptr_t * graph, const char *templ,int);
void mgl_set_ytt_(uintptr_t * graph, const char *templ,int);
void mgl_set_ztt_(uintptr_t * graph, const char *templ,int);
void mgl_set_ctt_(uintptr_t * graph, const char *templ,int);
/*****************************************************************************/
/*		Simple drawing														 */
/*****************************************************************************/
void mgl_ball_(uintptr_t *graph, mreal *x,mreal *y,mreal *z);
void mgl_ball_rgb_(uintptr_t *graph, mreal *x, mreal *y, mreal *z, mreal *r, mreal *g, mreal *b, mreal *alpha);
void mgl_ball_str_(uintptr_t *graph, mreal *x, mreal *y, mreal *z, const char *col,int);
void mgl_line_(uintptr_t *graph, mreal *x1, mreal *y1, mreal *z1, mreal *x2, mreal *y2, mreal *z2, const char *pen,int *n,int);

void mgl_facex_(uintptr_t* graph, mreal *x0, mreal *y0, mreal *z0, mreal *wy, mreal *wz, const char *stl, mreal *dx, mreal *dy, int l);
void mgl_facey_(uintptr_t* graph, mreal *x0, mreal *y0, mreal *z0, mreal *wx, mreal *wz, const char *stl, mreal *dx, mreal *dy, int l);
void mgl_facez_(uintptr_t* graph, mreal *x0, mreal *y0, mreal *z0, mreal *wx, mreal *wy, const char *stl, mreal *dx, mreal *dy, int l);
void mgl_curve_(uintptr_t* graph, mreal *x1, mreal *y1, mreal *z1, mreal *dx1, mreal *dy1, mreal *dz1, mreal *x2, mreal *y2, mreal *z2, mreal *dx2, mreal *dy2, mreal *dz2, const char *pen,int *n, int l);

void mgl_puts_(uintptr_t *graph, mreal *x, mreal *y, mreal *z,const char *text,int);
void mgl_puts_dir_(uintptr_t *graph, mreal *x, mreal *y, mreal *z, mreal *dx, mreal *dy, mreal *dz, const char *text, mreal size, int);
void mgl_text_(uintptr_t *graph, mreal *x, mreal *y, mreal *z,const char *text,int);
void mgl_title_(uintptr_t *graph, const char *text, const char *fnt, mreal *size, int,int);
void mgl_puts_ext_(uintptr_t *graph, mreal *x, mreal *y, mreal *z,const char *text,const char *font,mreal *size,const char *dir,int,int,int);
void mgl_text_ext_(uintptr_t *graph, mreal *x, mreal *y, mreal *z,const char *text,const char *font,mreal *size,const char *dir,int,int,int);
void mgl_colorbar_(uintptr_t *graph, const char *sch,int *where,int);
void mgl_colorbar_ext_(uintptr_t *graph, const char *sch,int *where, mreal *x, mreal *y, mreal *w, mreal *h, int);
void mgl_colorbar_val_(uintptr_t *graph, uintptr_t *dat, const char *sch,int *where,int);
void mgl_simple_plot_(uintptr_t *graph, uintptr_t *a, int *type, const char *stl,int);
void mgl_add_legend_(uintptr_t *graph, const char *text,const char *style,int,int);
void mgl_clear_legend_(uintptr_t *graph);
void mgl_legend_xy_(uintptr_t *graph, mreal *x, mreal *y, const char *font, mreal *size, mreal *llen,int);
void mgl_legend_(uintptr_t *graph, int *where, const char *font, mreal *size, mreal *llen,int);
void mgl_set_legend_box_(uintptr_t *gr, int *enable);
void mgl_set_legend_marks_(uintptr_t *gr, int *num);
/*****************************************************************************/
/*		1D plotting functions												 */
/*****************************************************************************/
void mgl_fplot_(uintptr_t *graph, const char *fy, const char *stl, int *n, int ly, int ls);
void mgl_fplot_xyz_(uintptr_t *graph, const char *fx, const char *fy, const char *fz, const char *stl, int *n, int lx, int ly, int lz, int ls);
void mgl_plot_xyz_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *z, const char *pen,int);
void mgl_plot_xy_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, const char *pen,int);
void mgl_plot_(uintptr_t *graph, uintptr_t *y,	const char *pen,int);
void mgl_radar_(uintptr_t *graph, uintptr_t *a,	const char *pen, mreal *r, int);
void mgl_boxplot_xy_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, const char *pen,int);
void mgl_boxplot_(uintptr_t *graph, uintptr_t *y, const char *pen,int);
void mgl_tens_xyz_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *c, const char *pen,int);
void mgl_tens_xy_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *c, const char *pen,int);
void mgl_tens_(uintptr_t *graph, uintptr_t *y, uintptr_t *c, const char *pen,int);
void mgl_area_xyz_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *z, const char *pen,int);
void mgl_area_xy_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, const char *pen,int);
void mgl_area_s_(uintptr_t *graph, uintptr_t *y, const char *pen,int);
void mgl_area_(uintptr_t *graph, uintptr_t *y, const char *pen,int);
void mgl_region_xy_(uintptr_t *graph, uintptr_t *x, uintptr_t *y1, uintptr_t *y2, const char *pen, int *inside, int);
void mgl_region_(uintptr_t *graph, uintptr_t *y1, uintptr_t *y2, const char *pen, int *inside, int);
void mgl_mark_(uintptr_t *graph, mreal *x,mreal *y,mreal *z,const char *mark,int);
void mgl_stem_xyz_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *z, const char *pen,int);
void mgl_stem_xy_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, const char *pen,int);
void mgl_stem_(uintptr_t *graph, uintptr_t *y,	const char *pen,int);
void mgl_step_xyz_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *z, const char *pen,int);
void mgl_step_xy_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, const char *pen,int);
void mgl_step_(uintptr_t *graph, uintptr_t *y,	const char *pen,int);
void mgl_bars_xyz_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *z, const char *pen,int);
void mgl_bars_xy_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, const char *pen,int);
void mgl_bars_(uintptr_t *graph, uintptr_t *y,	const char *pen,int);
void mgl_barh_yx_(uintptr_t *graph, uintptr_t *y, uintptr_t *v, const char *pen,int);
void mgl_barh_(uintptr_t *graph, uintptr_t *v,	const char *pen,int);
/*****************************************************************************/
/*		Advanced 1D plotting functions												 */
/*****************************************************************************/
void mgl_torus_(uintptr_t *graph, uintptr_t *r, uintptr_t *z, const char *pen,int);
void mgl_text_xyz_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *z,const char *text, const char *font, mreal *size,int,int);
void mgl_text_xy_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, const char *text, const char *font, mreal *size,int,int);
void mgl_text_y_(uintptr_t *graph, uintptr_t *y, const char *text, const char *font, mreal *size,int,int);
void mgl_chart_(uintptr_t *graph, uintptr_t *a, const char *col,int);
void mgl_error_(uintptr_t *graph, uintptr_t *y, uintptr_t *ey, const char *pen,int);
void mgl_error_xy_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *ey, const char *pen,int);
void mgl_error_exy_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *ex, uintptr_t *ey, const char *pen,int);
void mgl_mark_xyz_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *r, const char *pen,int);
void mgl_mark_xy_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *r, const char *pen,int);
void mgl_mark_y_(uintptr_t *graph, uintptr_t *y, uintptr_t *r, const char *pen,int);
void mgl_tube_xyzr_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *r, const char *pen,int);
void mgl_tube_xyr_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *r, const char *pen,int);
void mgl_tube_r_(uintptr_t *graph, uintptr_t *y, uintptr_t *r, const char *pen,int);
void mgl_tube_xyz_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *z, mreal *r, const char *pen,int);
void mgl_tube_xy_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, mreal *r, const char *pen,int);
void mgl_tube_(uintptr_t *graph, uintptr_t *y, mreal *r, const char *pen,int);

void mgl_textmark_xyzr_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *r, const char *text, const char *fnt,int,int);
void mgl_textmark_xyr_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *r, const char *text, const char *fnt,int,int);
void mgl_textmark_yr_(uintptr_t *graph, uintptr_t *y, uintptr_t *r, const char *text, const char *fnt,int,int);
void mgl_textmark_(uintptr_t *graph, uintptr_t *y, const char *text, const char *fnt,int,int);
/*****************************************************************************/
/*		2D plotting functions												 */
/*****************************************************************************/
void mgl_fsurf_(uintptr_t *graph, const char *fz, const char *stl, int *n, int lz, int ls);
void mgl_fsurf_xyz_(uintptr_t *graph, const char *fx, const char *fy, const char *fz, const char *stl, int *n, int lx, int ly, int lz, int ls);
void mgl_grid_xy_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *z, const char *stl,mreal *zVal,int);
void mgl_grid_(uintptr_t *graph, uintptr_t *a,const char *stl,mreal *zVal,int);
void mgl_mesh_xy_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *z, const char *sch,int);
void mgl_mesh_(uintptr_t *graph, uintptr_t *z, const char *sch,int);
void mgl_fall_xy_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *z, const char *sch,int);
void mgl_fall_(uintptr_t *graph, uintptr_t *z, const char *sch,int);
void mgl_belt_xy_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *z, const char *sch,int);
void mgl_belt_(uintptr_t *graph, uintptr_t *z, const char *sch,int);
void mgl_surf_xy_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *z, const char *sch,int);
void mgl_surf_(uintptr_t *graph, uintptr_t *z, const char *sch,int);
void mgl_dens_xy_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *z, const char *sch,mreal *zVal,int);
void mgl_dens_(uintptr_t *graph, uintptr_t *z, const char *sch,mreal *zVal,int);
void mgl_boxs_xy_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *z, const char *sch,mreal *zVal,int);
void mgl_boxs_(uintptr_t *graph, uintptr_t *z, const char *sch,mreal *zVal,int);
void mgl_tile_xy_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *z, const char *sch,int);
void mgl_tile_(uintptr_t *graph, uintptr_t *z, const char *sch,int);
void mgl_tiles_xy_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *r, const char *sch,int);
void mgl_tiles_(uintptr_t *graph, uintptr_t *z, uintptr_t *r, const char *sch,int);
void mgl_cont_xy_val_(uintptr_t *graph, uintptr_t *v, uintptr_t *x, uintptr_t *y, uintptr_t *z, const char *sch, mreal *zVal,int);
void mgl_cont_val_(uintptr_t *graph, uintptr_t *v, uintptr_t *z, const char *sch,mreal *zVal,int);
void mgl_cont_xy_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *z, const char *sch, int *Num, mreal *zVal,int);
void mgl_cont_(uintptr_t *graph, uintptr_t *z, const char *sch, int *Num, mreal *zVal,int);

void mgl_contf_xy_val_(uintptr_t *graph, uintptr_t *v, uintptr_t *x, uintptr_t *y, uintptr_t *z, const char *sch, mreal *zVal,int);
void mgl_contf_val_(uintptr_t *graph, uintptr_t *v, uintptr_t *z, const char *sch,mreal *zVal,int);
void mgl_contf_xy_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *z, const char *sch, int *Num, mreal *zVal,int);
void mgl_contf_(uintptr_t *graph, uintptr_t *z, const char *sch, int *Num, mreal *zVal,int);

void mgl_contd_xy_val_(uintptr_t *graph, uintptr_t *v, uintptr_t *x, uintptr_t *y, uintptr_t *z, const char *sch, mreal *zVal,int);
void mgl_contd_val_(uintptr_t *graph, uintptr_t *v, uintptr_t *z, const char *sch,mreal *zVal,int);
void mgl_contd_xy_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *z, const char *sch, int *Num, mreal *zVal,int);
void mgl_contd_(uintptr_t *graph, uintptr_t *z, const char *sch, int *Num, mreal *zVal,int);

void mgl_axial_xy_val_(uintptr_t *graph, uintptr_t *v, uintptr_t *x, uintptr_t *y, uintptr_t *a, const char *sch,int);
void mgl_axial_val_(uintptr_t *graph, uintptr_t *v, uintptr_t *a, const char *sch,int);
void mgl_axial_xy_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *a, const char *sch, int *Num,int);
void mgl_axial_(uintptr_t *graph, uintptr_t *a, const char *sch, int *Num,int);
/*****************************************************************************/
/*		Dual plotting functions												 */
/*****************************************************************************/
void mgl_surfc_xy_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *c, const char *sch,int);
void mgl_surfc_(uintptr_t *graph, uintptr_t *z, uintptr_t *c, const char *sch,int);
void mgl_surfa_xy_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *c, const char *sch,int);
void mgl_surfa_(uintptr_t *graph, uintptr_t *z, uintptr_t *c, const char *sch,int);
void mgl_stfa_xy_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *re, uintptr_t *im, int *dn, const char *sch, mreal *zVal, int);
void mgl_stfa_(uintptr_t *graph, uintptr_t *re, uintptr_t *im, int *dn, const char *sch, mreal *zVal, int);
void mgl_traj_xy_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *ax, uintptr_t *ay, const char *sch,mreal *zVal,mreal *len,int);
void mgl_traj_xyz_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *ax, uintptr_t *ay, uintptr_t *az, const char *sch,mreal *len,int);
void mgl_vect_xy_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *ax, uintptr_t *ay, const char *sch,mreal *zVal,int *flag,int);
void mgl_vect_2d_(uintptr_t *graph, uintptr_t *ax, uintptr_t *ay, const char *sch,mreal *zVal,int *flag,int);
void mgl_vectl_xy_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *ax, uintptr_t *ay, const char *sch,mreal *zVal,int);
void mgl_vectl_2d_(uintptr_t *graph, uintptr_t *ax, uintptr_t *ay, const char *sch,mreal *zVal,int);
void mgl_vectc_xy_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *ax, uintptr_t *ay, const char *sch,mreal *zVal,int);
void mgl_vectc_2d_(uintptr_t *graph, uintptr_t *ax, uintptr_t *ay, const char *sch,mreal *zVal,int);
void mgl_vect_xyz_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *ax, uintptr_t *ay, uintptr_t *az, const char *sch,int *flag,int);
void mgl_vect_3d_(uintptr_t *graph, uintptr_t *ax, uintptr_t *ay, uintptr_t *az, const char *sch,int *flag,int);
void mgl_vectl_xyz_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *ax, uintptr_t *ay, uintptr_t *az, const char *sch,int);
void mgl_vectl_3d_(uintptr_t *graph, uintptr_t *ax, uintptr_t *ay, uintptr_t *az, const char *sch,int);
void mgl_vectc_xyz_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *ax, uintptr_t *ay, uintptr_t *az, const char *sch,int);
void mgl_vectc_3d_(uintptr_t *graph, uintptr_t *ax, uintptr_t *ay, uintptr_t *az, const char *sch,int);
void mgl_map_xy_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *a, uintptr_t *b, const char *sch, int *ks, int *pnts,int);
void mgl_map_(uintptr_t *graph, uintptr_t *a, uintptr_t *b, const char *sch, int *ks, int *pnts,int);
void mgl_surf3a_xyz_val_(uintptr_t *graph, mreal *Val, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *a, uintptr_t *b,
			const char *stl,int);
void mgl_surf3a_val_(uintptr_t *graph, mreal *Val, uintptr_t *a, uintptr_t *b, const char *stl,int);
void mgl_surf3a_xyz_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *a, uintptr_t *b,
			const char *stl, int *num,int);
void mgl_surf3a_(uintptr_t *graph, uintptr_t *a, uintptr_t *b, const char *stl, int *num,int);
void mgl_surf3c_xyz_val_(uintptr_t *graph, mreal *Val, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *a, uintptr_t *b,
			const char *stl,int);
void mgl_surf3c_val_(uintptr_t *graph, mreal *Val, uintptr_t *a, uintptr_t *b, const char *stl,int);
void mgl_surf3c_xyz_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *a, uintptr_t *b,
			const char *stl, int *num,int);
void mgl_surf3c_(uintptr_t *graph, uintptr_t *a, uintptr_t *b, const char *stl, int *num,int);
void mgl_flow_xy_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *ax, uintptr_t *ay, const char *sch, int *num, int *central, mreal *zVal,int);
void mgl_flow_2d_(uintptr_t *graph, uintptr_t *ax, uintptr_t *ay, const char *sch, int *num, int *central, mreal *zVal,int);
void mgl_flow_xyz_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *ax, uintptr_t *ay, uintptr_t *az, const char *sch, int *num, int *central,int);
void mgl_flow_3d_(uintptr_t *graph, uintptr_t *ax, uintptr_t *ay, uintptr_t *az, const char *sch, int *num, int *central,int);

void mgl_flowp_xy_(uintptr_t *graph, mreal *x0, mreal *y0, mreal *z0, uintptr_t *x, uintptr_t *y, uintptr_t *ax, uintptr_t *ay, const char *sch, int);
void mgl_flowp_2d_(uintptr_t *graph, mreal *x0, mreal *y0, mreal *z0, uintptr_t *ax, uintptr_t *ay, const char *sch, int);
void mgl_flowp_xyz_(uintptr_t *graph, mreal *x0, mreal *y0, mreal *z0, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *ax, uintptr_t *ay, uintptr_t *az, const char *sch, int);
void mgl_flowp_3d_(uintptr_t *graph, mreal *x0, mreal *y0, mreal *z0, uintptr_t *ax, uintptr_t *ay, uintptr_t *az, const char *sch,int);

void mgl_pipe_xy_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *ax, uintptr_t *ay, const char *sch, mreal *r0, int *num, int *central, mreal *zVal,int);
void mgl_pipe_2d_(uintptr_t *graph, uintptr_t *ax, uintptr_t *ay, const char *sch, mreal *r0, int *num, int *central, mreal *zVal,int);
void mgl_pipe_xyz_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *ax, uintptr_t *ay, uintptr_t *az, const char *sch, mreal *r0, int *num, int *central,int);
void mgl_pipe_3d_(uintptr_t *graph, uintptr_t *ax, uintptr_t *ay, uintptr_t *az, const char *sch, mreal *r0, int *num, int *central,int);
void mgl_dew_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *ax, uintptr_t *ay, const char *sch, mreal *zVal,int l);
void mgl_dew_2d_(uintptr_t *gr, uintptr_t *ax, uintptr_t *ay, const char *sch, mreal *zVal,int l);

void mgl_grad_xyz_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *ph, const char *sch, int *num, int);
void mgl_grad_xy_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *ph, const char *sch, int *num, mreal *zVal,int);
void mgl_grad_(uintptr_t *graph, uintptr_t *ph, const char *sch, int *num, mreal *zVal,int);
/*****************************************************************************/
/*		3D plotting functions												 */
/*****************************************************************************/
void mgl_grid3_xyz_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *a, const char *dir, int *sVal, const char *stl,int,int);
void mgl_grid3_(uintptr_t *graph, uintptr_t *a, const char *dir, int *sVal, const char *stl,int,int);
void mgl_grid3_all_xyz_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *a, const char *stl,int);
void mgl_grid3_all_(uintptr_t *graph, uintptr_t *a, const char *stl,int);
void mgl_dens3_xyz_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *a, const char *dir, int *sVal, const char *stl,int,int);
void mgl_dens3_(uintptr_t *graph, uintptr_t *a, const char *dir, int *sVal, const char *stl,int,int);
void mgl_dens3_all_xyz_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *a, const char *stl,int);
void mgl_dens3_all_(uintptr_t *graph, uintptr_t *a, const char *stl,int);
void mgl_surf3_xyz_val_(uintptr_t *graph, mreal *Val, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *a, const char *stl,int);
void mgl_surf3_val_(uintptr_t *graph, mreal *Val, uintptr_t *a, const char *stl,int);
void mgl_surf3_xyz_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *a, const char *stl, int *num,int);
void mgl_surf3_(uintptr_t *graph, uintptr_t *a, const char *stl, int *num,int);
void mgl_cont3_xyz_val_(uintptr_t *graph, uintptr_t *v, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *a,
			const char *dir, int *sVal, const char *sch,int,int);
void mgl_cont3_val_(uintptr_t *graph, uintptr_t *v, uintptr_t *a, const char *dir, int *sVal, const char *sch,int,int);
void mgl_cont3_xyz_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *a,
			const char *dir, int *sVal, const char *sch, int *Num,int,int);
void mgl_cont3_(uintptr_t *graph, uintptr_t *a, const char *dir, int *sVal, const char *sch, int *Num,int,int);
void mgl_cont_all_xyz_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *a,
			const char *sch, int *Num,int);
void mgl_cont_all_(uintptr_t *graph, uintptr_t *a, const char *sch, int *Num,int);
void mgl_cloudp_xyz_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *a,
			const char *stl, mreal *alpha,int);
void mgl_cloudp_(uintptr_t *graph, uintptr_t *a, const char *stl, mreal *alpha,int);
void mgl_cloud_xyz_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *a,
			const char *stl, mreal *alpha,int);
void mgl_cloud_(uintptr_t *graph, uintptr_t *a, const char *stl, mreal *alpha,int);
void mgl_contf3_xyz_val_(uintptr_t *graph, uintptr_t *v, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *a,
			const char *dir, int *sVal, const char *sch,int,int);
void mgl_contf3_val_(uintptr_t *graph, uintptr_t *v, uintptr_t *a, const char *dir, int *sVal, const char *sch,int,int);
void mgl_contf3_xyz_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *a,
			const char *dir, int *sVal, const char *sch, int *Num,int,int);
void mgl_contf3_(uintptr_t *graph, uintptr_t *a, const char *dir, int *sVal, const char *sch, int *Num,int,int);
void mgl_contf_all_xyz_(uintptr_t *graph, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *a,
			const char *sch, int *Num,int);
void mgl_contf_all_(uintptr_t *graph, uintptr_t *a, const char *sch, int *Num,int);
void mgl_beam_val_(uintptr_t *gr, mreal *val, uintptr_t *tr, uintptr_t *g1, uintptr_t *g2, uintptr_t *a,
				mreal *r, const char *sch, int *norm,int l);
void mgl_beam_(uintptr_t *gr, uintptr_t *tr, uintptr_t *g1, uintptr_t *g2, uintptr_t *a, mreal *r,
			const char *sch, int *norm, int *num,int l);
/*****************************************************************************/
/*		Triangular plotting functions											 */
/*****************************************************************************/
void mgl_triplot_xyzc_(uintptr_t *gr, uintptr_t *nums, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *c, const char *sch,int);
void mgl_triplot_xyz_(uintptr_t *gr, uintptr_t *nums, uintptr_t *x, uintptr_t *y, uintptr_t *z, const char *sch,int);
void mgl_triplot_xy_(uintptr_t *gr, uintptr_t *nums, uintptr_t *x, uintptr_t *y, const char *sch, mreal *zVal,int);
void mgl_quadplot_xyzc_(uintptr_t *gr, uintptr_t *nums, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *c, const char *sch,int);
void mgl_quadplot_xyz_(uintptr_t *gr, uintptr_t *nums, uintptr_t *x, uintptr_t *y, uintptr_t *z, const char *sch,int);
void mgl_quadplot_xy_(uintptr_t *gr, uintptr_t *nums, uintptr_t *x, uintptr_t *y, const char *sch, mreal *zVal,int);

void mgl_tricont_xyzcv_(uintptr_t *gr, uintptr_t *v, uintptr_t *nums, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *c, const char *sch, mreal *zVal,int);
void mgl_tricont_xyzv_(uintptr_t *gr, uintptr_t *v, uintptr_t *nums, uintptr_t *x, uintptr_t *y, uintptr_t *z, const char *sch, mreal *zVal,int);
void mgl_tricont_xyzc_(uintptr_t *gr, uintptr_t *nums, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *c, const char *sch, int *n, mreal *zVal, int);
void mgl_tricont_xyz_(uintptr_t *gr, uintptr_t *nums, uintptr_t *x, uintptr_t *y, uintptr_t *z, const char *sch, int *n, mreal *zVal, int);

void mgl_dots_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, const char *sch,int);
void mgl_dots_a_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *a, const char *sch,int);
void mgl_dots_tr_(uintptr_t *gr, uintptr_t *tr, const char *sch,int);
void mgl_crust_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, const char *sch, mreal *er,int);
void mgl_crust_tr_(uintptr_t *gr, uintptr_t *tr, const char *sch, mreal *er,int);
/*****************************************************************************/
/*		Combined plotting functions											 */
/*****************************************************************************/
void mgl_dens_x_(uintptr_t *graph, uintptr_t *a, const char *stl, mreal *sVal,int);
void mgl_dens_y_(uintptr_t *graph, uintptr_t *a, const char *stl, mreal *sVal,int);
void mgl_dens_z_(uintptr_t *graph, uintptr_t *a, const char *stl, mreal *sVal,int);
void mgl_cont_x_(uintptr_t *graph, uintptr_t *a, const char *stl, mreal *sVal, int *Num,int);
void mgl_cont_y_(uintptr_t *graph, uintptr_t *a, const char *stl, mreal *sVal, int *Num,int);
void mgl_cont_z_(uintptr_t *graph, uintptr_t *a, const char *stl, mreal *sVal, int *Num,int);
void mgl_cont_x_val_(uintptr_t *graph, uintptr_t *v, uintptr_t *a, const char *stl, mreal *sVal,int);
void mgl_cont_y_val_(uintptr_t *graph, uintptr_t *v, uintptr_t *a, const char *stl, mreal *sVal,int);
void mgl_cont_z_val_(uintptr_t *graph, uintptr_t *v, uintptr_t *a, const char *stl, mreal *sVal,int);
void mgl_contf_x_(uintptr_t *graph, uintptr_t *a, const char *stl, mreal *sVal, int *Num,int);
void mgl_contf_y_(uintptr_t *graph, uintptr_t *a, const char *stl, mreal *sVal, int *Num,int);
void mgl_contf_z_(uintptr_t *graph, uintptr_t *a, const char *stl, mreal *sVal, int *Num,int);
void mgl_contf_x_val_(uintptr_t *graph, uintptr_t *v, uintptr_t *a, const char *stl, mreal *sVal,int);
void mgl_contf_y_val_(uintptr_t *graph, uintptr_t *v, uintptr_t *a, const char *stl, mreal *sVal,int);
void mgl_contf_z_val_(uintptr_t *graph, uintptr_t *v, uintptr_t *a, const char *stl, mreal *sVal,int);
/*****************************************************************************/
/*		Data creation functions												 */
/*****************************************************************************/
void mgl_data_rearrange_(uintptr_t *dat, int *mx, int *my, int *mz);
void mgl_data_set_float_(uintptr_t *dat, const float *A,int *NX,int *NY,int *NZ);
void mgl_data_set_double_(uintptr_t *dat, const double *A,int *NX,int *NY,int *NZ);
void mgl_data_set_float1_(uintptr_t *d, const float *A,int *N1);
void mgl_data_set_double1_(uintptr_t *d, const double *A,int *N1);
void mgl_data_set_float2_(uintptr_t *d, const float *A,int *N1,int *N2);
void mgl_data_set_double2_(uintptr_t *d, const double *A,int *N1,int *N2);
void mgl_data_set_float3_(uintptr_t *d, const float *A,int *N1,int *N2,int *N3);
void mgl_data_set_double3_(uintptr_t *d, const double *A,int *N1,int *N2,int *N3);
void mgl_data_set_(uintptr_t *dat, uintptr_t *a);
mreal mgl_data_get_value_(uintptr_t *d, int *i, int *j, int *k);
int mgl_data_get_nx_(uintptr_t *d);
int mgl_data_get_ny_(uintptr_t *d);
int mgl_data_get_nz_(uintptr_t *d);
void mgl_data_set_value_(uintptr_t *d, mreal *v, int *i, int *j, int *k);
void mgl_data_set_values_(uintptr_t *d, const char *val, int *nx, int *ny, int *nz, int l);
int mgl_data_read_(uintptr_t *d, const char *fname,int l);
int mgl_data_read_mat_(uintptr_t *dat, const char *fname, int *dim, int);
int mgl_data_read_dim_(uintptr_t *dat, const char *fname,int *mx,int *my,int *mz,int);
void mgl_data_save_(uintptr_t *dat, const char *fname,int *ns,int);
void mgl_data_export_(uintptr_t *dat, const char *fname, const char *scheme,mreal *v1,mreal *v2,int *ns,int,int);
void mgl_data_import_(uintptr_t *dat, const char *fname, const char *scheme,mreal *v1,mreal *v2,int,int);
void mgl_data_create_(uintptr_t *dat, int *nx,int *ny,int *nz);
void mgl_data_transpose_(uintptr_t *dat, const char *dim,int);
void mgl_data_norm_(uintptr_t *dat, mreal *v1,mreal *v2,int *sym,int *dim);
void mgl_data_norm_slice_(uintptr_t *dat, mreal *v1,mreal *v2,char *dir,int *keep_en,int *sym,int l);
uintptr_t mgl_data_subdata_(uintptr_t *dat, int *xx,int *yy,int *zz);
uintptr_t mgl_data_subdata_ext_(uintptr_t *dat, uintptr_t *xx,uintptr_t *yy,uintptr_t *zz);
uintptr_t mgl_data_column_(uintptr_t *dat, const char *eq,int l);
void mgl_data_set_id_(uintptr_t *dat, const char *id,int l);
void mgl_data_fill_(uintptr_t *dat, mreal *x1,mreal *x2,const char *dir,int);
void mgl_data_fill_eq_(uintptr_t *gr, uintptr_t *dat, const char *eq, uintptr_t *vdat, uintptr_t *wdat, int);
void mgl_data_put_val_(uintptr_t *dat, mreal *val, int *i, int *j, int *k);
void mgl_data_put_dat_(uintptr_t *dat, uintptr_t *val, int *i, int *j, int *k);
void mgl_data_modify_(uintptr_t *dat, const char *eq,int *dim,int);
void mgl_data_modify_vw_(uintptr_t *dat, const char *eq, uintptr_t *vdat, uintptr_t *wdat,int);
void mgl_data_squeeze_(uintptr_t *dat, int *rx,int *ry,int *rz,int *smooth);
mreal mgl_data_max_(uintptr_t *dat);
mreal mgl_data_min_(uintptr_t *dat);
const mreal *mgl_data_data_(uintptr_t *dat);

mreal mgl_data_first_(uintptr_t *dat, const char *cond, int *i, int *j, int *k, int);
mreal mgl_data_last_(uintptr_t *dat, const char *cond, int *i, int *j, int *k, int);
int mgl_data_find_(uintptr_t *dat, const char *cond, char *dir, int *i, int *j, int *k, int,int);
int mgl_data_find_any_(uintptr_t *dat, const char *cond, int);
mreal mgl_data_max_int_(uintptr_t *dat, int *i, int *j, int *k);
mreal mgl_data_max_real_(uintptr_t *dat, mreal *x, mreal *y, mreal *z);
mreal mgl_data_min_int_(uintptr_t *dat, int *i, int *j, int *k);
mreal mgl_data_min_real_(uintptr_t *dat, mreal *x, mreal *y, mreal *z);
mreal mgl_data_momentum_mw_(uintptr_t *dat, char *dir, mreal *m, mreal *w,int);

uintptr_t mgl_data_combine_(uintptr_t *dat1, uintptr_t *dat2);
void mgl_data_extend_(uintptr_t *dat, int *n1, int *n2);
void mgl_data_insert_(uintptr_t *dat, const char *dir, int *at, int *num, int);
void mgl_data_delete_(uintptr_t *dat, const char *dir, int *at, int *num, int);
/*****************************************************************************/
/*		Data manipulation functions											 */
/*****************************************************************************/
void mgl_data_smooth_(uintptr_t *dat, int *Type,mreal *delta,const char *dirs,int);
uintptr_t mgl_data_sum_(uintptr_t *dat, const char *dir,int);
uintptr_t mgl_data_max_dir_(uintptr_t *dat, const char *dir,int);
uintptr_t mgl_data_min_dir_(uintptr_t *dat, const char *dir,int);
void mgl_data_cumsum_(uintptr_t *dat, const char *dir,int);
void mgl_data_integral_(uintptr_t *dat, const char *dir,int);
void mgl_data_diff_(uintptr_t *dat, const char *dir,int);
void mgl_data_diff_par_(uintptr_t *dat, uintptr_t *v1, uintptr_t *v2, uintptr_t *v3);
void mgl_data_diff2_(uintptr_t *dat, const char *dir,int);
void mgl_data_swap_(uintptr_t *dat, const char *dir,int);
void mgl_data_roll_(uintptr_t *dat, const char *dir, int *num, int);
void mgl_data_mirror_(uintptr_t *dat, const char *dir,int);

void mgl_data_hankel_(uintptr_t *dat, const char *dir,int);
void mgl_data_sinfft_(uintptr_t *dat, const char *dir,int);
void mgl_data_cosfft_(uintptr_t *dat, const char *dir,int);
void mgl_data_fill_sample_(uintptr_t *dat, int num, const char *how,int);

mreal mgl_data_spline_(uintptr_t *dat, mreal *x,mreal *y,mreal *z);
mreal mgl_data_spline1_(uintptr_t *dat, mreal *x,mreal *y,mreal *z);
mreal mgl_data_linear_(uintptr_t *dat, mreal *x,mreal *y,mreal *z);
mreal mgl_data_linear1_(uintptr_t *dat, mreal *x,mreal *y,mreal *z);
uintptr_t mgl_data_resize_(uintptr_t *dat, int *mx,int *my,int *mz);
uintptr_t mgl_data_resize_box_(uintptr_t *dat, int *mx,int *my,int *mz,mreal *x1,mreal *x2,mreal *y1,mreal *y2,mreal *z1,mreal *z2);
uintptr_t mgl_data_momentum_(uintptr_t *dat, char *dir, const char *how, int,int);
uintptr_t mgl_data_hist_(uintptr_t *dat, int *n, mreal *v1, mreal *v2, int *nsub);
uintptr_t mgl_data_hist_w_(uintptr_t *dat, uintptr_t *weight, int *n, mreal *v1, mreal *v2, int *nsub);
uintptr_t mgl_data_evaluate_i_(uintptr_t *dat, uintptr_t *idat, int *norm);
uintptr_t mgl_data_evaluate_ij_(uintptr_t *dat, uintptr_t *idat, uintptr_t *jdat, int *norm);
uintptr_t mgl_data_evaluate_ijk_(uintptr_t *dat, uintptr_t *idat, uintptr_t *jdat, uintptr_t *kdat, int *norm);
void mgl_data_envelop_(uintptr_t *dat, const char *dir, int);
void mgl_data_sew_(uintptr_t *dat, const char *dirs, mreal *da, int);
void mgl_data_crop_(uintptr_t *dat, int *n1, int *n2, const char *dir,int);
/*****************************************************************************/
/*		Data operations														 */
/*****************************************************************************/
void mgl_data_mul_dat_(uintptr_t *dat, uintptr_t *d);
void mgl_data_div_dat_(uintptr_t *dat, uintptr_t *d);
void mgl_data_add_dat_(uintptr_t *dat, uintptr_t *d);
void mgl_data_sub_dat_(uintptr_t *dat, uintptr_t *d);
void mgl_data_mul_num_(uintptr_t *dat, mreal *d);
void mgl_data_div_num_(uintptr_t *dat, mreal *d);
void mgl_data_add_num_(uintptr_t *dat, mreal *d);
void mgl_data_sub_num_(uintptr_t *dat, mreal *d);
/*****************************************************************************/
/*		Nonlinear fitting													 */
/*****************************************************************************/
void mgl_hist_x_(uintptr_t* gr, uintptr_t* res, uintptr_t* x, uintptr_t* a);
void mgl_hist_xy_(uintptr_t* gr, uintptr_t* res, uintptr_t* x, uintptr_t* y, uintptr_t* a);
void mgl_hist_xyz_(uintptr_t* gr, uintptr_t* res, uintptr_t* x, uintptr_t* y, uintptr_t* z, uintptr_t* a);
/*****************************************************************************/
/*		Nonlinear fitting													 */
/*****************************************************************************/
mreal mgl_fit_1_(uintptr_t* gr, uintptr_t* fit, uintptr_t* y, const char *eq, const char *var, mreal *ini, int l, int n);
mreal mgl_fit_2_(uintptr_t* gr, uintptr_t* fit, uintptr_t* z, const char *eq, const char *var, mreal *ini, int l, int n);
mreal mgl_fit_3_(uintptr_t* gr, uintptr_t* fit, uintptr_t* a, const char *eq, const char *var, mreal *ini, int l, int n);
mreal mgl_fit_xy_(uintptr_t* gr, uintptr_t* fit, uintptr_t* x, uintptr_t* y, const char *eq, const char *var, mreal *ini, int l, int n);
mreal mgl_fit_xyz_(uintptr_t* gr, uintptr_t* fit, uintptr_t* x, uintptr_t* y, uintptr_t* z, const char *eq, const char *var, mreal *ini, int l, int n);
mreal mgl_fit_xyza_(uintptr_t* gr, uintptr_t* fit, uintptr_t* x, uintptr_t* y, uintptr_t* z, uintptr_t* a, const char *eq, const char *var, mreal *ini, int l, int n);
mreal mgl_fit_ys_(uintptr_t* gr, uintptr_t* fit, uintptr_t* y, uintptr_t* ss, const char *eq, const char *var, mreal *ini, int l, int n);
mreal mgl_fit_xys_(uintptr_t* gr, uintptr_t* fit, uintptr_t* x, uintptr_t* y, uintptr_t* ss, const char *eq, const char *var, mreal *ini, int l, int n);
mreal mgl_fit_xyzs_(uintptr_t* gr, uintptr_t* fit, uintptr_t* x, uintptr_t* y, uintptr_t* z, uintptr_t* ss, const char *eq, const char *var, mreal *ini, int l, int n);
mreal mgl_fit_xyzas_(uintptr_t* gr, uintptr_t* fit, uintptr_t* x, uintptr_t* y, uintptr_t* z, uintptr_t* a, uintptr_t* ss, const char *eq, const char *var, mreal *ini, int l, int n);
void mgl_puts_fit_(uintptr_t* gr, mreal *x, mreal *y, mreal *z, const char *prefix, const char *font, mreal *size, int l, int n);
/*****************************************************************************/
uintptr_t mgl_create_parser_();
void mgl_delete_parser_(uintptr_t* p);
void mgl_add_param_(uintptr_t* p, int *id, const char *str, int l);
/*===!!! NOTE !!! You must not delete obtained data arrays !!!===============*/
uintptr_t mgl_add_var_(uintptr_t* p, const char *name, int l);
/*===!!! NOTE !!! You must not delete obtained data arrays !!!===============*/
uintptr_t mgl_find_var_(uintptr_t* p, const char *name, int l);
int mgl_parse_(uintptr_t* gr, uintptr_t* p, const char *str, int *pos, int l);
void mgl_parse_text_(uintptr_t* gr, uintptr_t* p, const char *str, int l);
void mgl_restore_once_(uintptr_t* p);
void mgl_parser_allow_setsize_(uintptr_t* p, int *a);
/*****************************************************************************/
void mgl_sphere_(uintptr_t* graph, mreal *x, mreal *y, mreal *z, mreal *r, const char *stl, int);
void mgl_drop_(uintptr_t* graph, mreal *x1, mreal *y1, mreal *z1, mreal *x2, mreal *y2, mreal *z2, mreal *r, const char *stl, mreal *shift, mreal *ap, int);
void mgl_cone_(uintptr_t* graph, mreal *x1, mreal *y1, mreal *z1, mreal *x2, mreal *y2, mreal *z2, mreal *r1, mreal *r2, const char *stl, int *edge, int);

uintptr_t mgl_pde_solve_(uintptr_t* gr, const char *ham, uintptr_t* ini_re, uintptr_t* ini_im, mreal *dz, mreal *k0,int);
uintptr_t mgl_qo2d_solve_(const char *ham, uintptr_t* ini_re, uintptr_t* ini_im, uintptr_t* ray, mreal *r, mreal *k0, uintptr_t* xx, uintptr_t* yy, int);
uintptr_t mgl_af2d_solve_(const char *ham, uintptr_t* ini_re, uintptr_t* ini_im, uintptr_t* ray, mreal *r, mreal *k0, uintptr_t* xx, uintptr_t* yy, int);
uintptr_t mgl_ray_trace_(const char *ham, mreal *x0, mreal *y0, mreal *z0, mreal *px, mreal *py, mreal *pz, mreal *dt, mreal *tmax,int);
void mgl_data_fill_eq_(uintptr_t* gr, uintptr_t* res, const char *eq, uintptr_t* vdat, uintptr_t* wdat,int);
uintptr_t mgl_jacobian_2d_(uintptr_t* x, uintptr_t* y);
uintptr_t mgl_jacobian_3d_(uintptr_t* x, uintptr_t* y, uintptr_t* z);

uintptr_t mgl_transform_a_(uintptr_t *am, uintptr_t *ph, const char *tr, int);
uintptr_t mgl_transform_(uintptr_t *re, uintptr_t *im, const char *tr, int);
uintptr_t mgl_data_stfa_(uintptr_t *re, uintptr_t *im, int *dn, char *dir, int);

#ifdef __cplusplus
}
#endif

#endif /* _mgl_f_h_ */
