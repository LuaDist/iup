/***************************************************************************
 * mgl_gl.cpp is part of Math Graphic Library
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
#include <stdlib.h>
#include <wchar.h>

#include "mgl/mgl_gl.h"
#include "mgl/mgl_c.h"
#include "mgl/mgl_f.h"
//-----------------------------------------------------------------------------
/// Create mglGraph object in OpenGL mode.
HMGL mgl_create_graph_gl()
{	return new mglGraphGL;	}
/// Create mglGraph object in OpenGL mode.
uintptr_t mgl_create_graph_gl_()
{	return uintptr_t(new mglGraphGL);	}
//-----------------------------------------------------------------------------
mglGraphGL::mglGraphGL() : mglGraphAB(1,1)	{}
//-----------------------------------------------------------------------------
mglGraphGL::~mglGraphGL(){}
//-----------------------------------------------------------------------------
bool mglGraphGL::Alpha(bool enable)
{
	bool t = UseAlpha;
	if(enable)
	{
		UseAlpha = true;
    if (glIsEnabled(GL_LINE_SMOOTH))
      glDisable(GL_DEPTH_TEST);
    else
      glEnable(GL_DEPTH_TEST);
		glEnable(GL_ALPHA_TEST);
		glEnable(GL_BLEND);
		if(TranspType==1)	glBlendFunc(GL_DST_COLOR, GL_ZERO);
//		else if(TranspType==2) glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		else glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else
	{
		UseAlpha = false;
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_ALPHA_TEST);
		glDisable(GL_BLEND);
	}
	return t;
}
//-----------------------------------------------------------------------------
void mglGraphGL::Light(int n,mglPoint p,mglColor c, mreal br,bool infty)
{
	mglColor AmbLight = mglColor(AmbBr,AmbBr,AmbBr);
	mglColor DifLight = mglColor(br,br,br);
	GLenum light[8] = {GL_LIGHT0,GL_LIGHT1,GL_LIGHT2,GL_LIGHT3,GL_LIGHT4,
			GL_LIGHT5,GL_LIGHT6,GL_LIGHT7};
	float amb[4], pos[4],dif[4];
	if(n<0 || n>7)	{	SetWarn(mglWarnLId);	return;	}
	if(c.Valid())
	{
		DifLight = c*br;
		AmbLight = c*AmbBr;
	}
	dif[0] = DifLight.r;	dif[1] = DifLight.g;
	dif[2] = DifLight.b;	dif[3] = 1.;
	amb[0] = AmbLight.r;	amb[1] = AmbLight.g;
	amb[2] = AmbLight.b;	amb[3] = 1.;
	pos[0] = p.x;		pos[1] = p.y;
	pos[2] = p.z;		pos[3] = infty ? 0:1;
	glShadeModel(GL_SMOOTH);
	//glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 5.0);
	//glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, pos);

	glLightfv(light[n], GL_AMBIENT, amb);
	glLightfv(light[n], GL_DIFFUSE, dif);
	//glLightfv(light[n], GL_SPECULAR, spc);
	glLightfv(light[n], GL_POSITION, pos);
	glEnable(light[n]);
}
//-----------------------------------------------------------------------------
void mglGraphGL::LightScale()
{
	mglGraphAB::LightScale();
	GLenum light[8] = {GL_LIGHT0,GL_LIGHT1,GL_LIGHT2,GL_LIGHT3,GL_LIGHT4,
			GL_LIGHT5,GL_LIGHT6,GL_LIGHT7};
	float pos[4]={0,0,0,0};
	for(int i=0;i<8;i++)
	{
		pos[0] = pLight[3*i];
		pos[1] = pLight[3*i+1];
		pos[2] = pLight[3*i+2];
		if(nLight[i])	glLightfv(light[i], GL_POSITION, pos);
	}
}
//-----------------------------------------------------------------------------
void mglGraphGL::Light(int n, bool enable)
{
	GLenum light[8] = {GL_LIGHT0,GL_LIGHT1,GL_LIGHT2,GL_LIGHT3,GL_LIGHT4,
			GL_LIGHT5,GL_LIGHT6,GL_LIGHT7};
	if(enable)	glEnable(light[n]);
	else		glDisable(light[n]);
}
//-----------------------------------------------------------------------------
bool mglGraphGL::Light(bool enable)
{
	bool t = UseLight;
	UseLight = enable;
	if(enable)	{	glEnable(GL_LIGHTING);	glEnable(GL_NORMALIZE);}
	else		{	glDisable(GL_LIGHTING);	glDisable(GL_NORMALIZE);	}
	return t;
}
//-----------------------------------------------------------------------------
void mglGraphGL::View(mreal TetX,mreal TetY,mreal TetZ)
{
	glMatrixMode(GL_PROJECTION);//GL_PROJECTION GL_VIEWPORT GL_MODELVIEW
	glRotated(TetX,1.,0.,0.);
	glRotated(TetY,0.,1.,0.);
	glRotated(TetZ,0.,0.,1.);
}
//-----------------------------------------------------------------------------
void mglGraphGL::Fog(mreal , mreal)
{
/*	if(d>0)
	{
		glFogf(GL_FOG_MODE,GL_EXP);
		glFogf(GL_FOG_DENSITY,5*d);
		glFogfv(GL_FOG_COLOR,back);
		glEnable(GL_FOG);
	}
	else	glDisable(GL_FOG);*/
}
//-----------------------------------------------------------------------------
void mglGraphGL::Clf(mglColor Back)
{
	Fog(0);
	CurrPal = -1;
	if(Back==NC)	Back=mglColor(1,1,1);
//	glDepthFunc(GL_LESS);
	glDepthFunc(GL_GREATER);
//	back[0]=Back.r;	back[1]=Back.g;	back[2]=Back.b;
	glClearColor(Back.r,Back.g,Back.b,0.);
	glClearDepth(-10.);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_COLOR_MATERIAL);
}
//-----------------------------------------------------------------------------
void mglGraphGL::Ball(mreal x,mreal y,mreal z,mglColor col,mreal alpha)
{
	if(alpha==0)	return;
	if(alpha<0)	{	alpha = -alpha;	}
	else		{	if(!ScalePoint(x,y,z))	return;	}
	if(!col.Valid())	col = mglColor(1.,0.,0.);
	alpha = Transparent ? alpha : 1;
	mreal p[3] = {x,y,z};	PostScale(p,1);
	glBegin(GL_POINTS);
	glColor4f(col.r,col.g,col.b,alpha);
	glVertex3f(p[0],p[1],p[2]);
	glEnd();
}
//-----------------------------------------------------------------------------
void mglGraphGL::ball(mreal *p,mreal *c)
{
	glBegin(GL_POINTS);
#if(MGL_USE_DOUBLE==1)
	glColor4dv(c);
	glVertex3dv(p);
#else
	glColor4fv(c);
	glVertex3fv(p);
#endif
	glEnd();
}
//-----------------------------------------------------------------------------
void mglGraphGL::Pen(mglColor col, char style,mreal width)
{
	mglGraphAB::Pen(col,style,width);
	if(style==0)	return;
	if(style!='-')	switch(style)
	{
	case '|': glLineStipple(int(width),0xff00);	break;
	case ';': glLineStipple(int(width),0xf0f0);	break;
	case ':': glLineStipple(int(width),0x8888);	break;
	case 'j': glLineStipple(int(width),0xfe10);	break;
	case 'i': glLineStipple(int(width),0xe4e4);	break;
	case ' ': glLineStipple(int(width),0x0000);	break;
	}
	if(width>=1)	glLineWidth(width);
	else			glLineWidth(1);
	if(style=='-')	glDisable(GL_LINE_STIPPLE);
	else			glEnable(GL_LINE_STIPPLE);
}
//-----------------------------------------------------------------------------
void mglGraphGL::EndFrame()
{
//	mglGraph::EndFrame();
	glEndList();
}
//-----------------------------------------------------------------------------
int mglGraphGL::NewFrame()
{
	Clf();
	Identity();
	glNewList(CurFrameId,GL_COMPILE);
	CurFrameId++;
	return CurFrameId-1;
}
//-----------------------------------------------------------------------------
unsigned char **mglGraphGL::GetRGBLines(long &width, long &height, unsigned char *&f, bool alpha)
{
	long x, y, d = alpha ? 4:3;
	GLint w[4];
	glGetIntegerv(GL_VIEWPORT,w);
	x=w[0];	y=w[1];	width=w[2];	height=w[3];
	unsigned char **p;

	p = (unsigned char **)malloc(height * sizeof(unsigned char *));
	f = (unsigned char *) malloc(width*height * sizeof(unsigned char)*d);
	for(long i=0;i<height;i++)	p[i] = f+d*width*(height-1-i);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(x, y, width, height, alpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, f);
	return p;
}
//-----------------------------------------------------------------------------
void mglGraphGL::trig_plot(mreal *p0,mreal *p1,mreal *p2,mreal *c0,mreal *c1,mreal *c2)
{
	glBegin(GL_TRIANGLES);
	glColor4f(c0[0],c0[1],c0[2],c0[3]);	glVertex3f(p0[0],p0[1],p0[2]);
	glColor4f(c1[1],c1[1],c1[2],c1[3]);	glVertex3f(p1[0],p1[1],p1[2]);
	glColor4f(c2[0],c2[1],c2[2],c2[3]);	glVertex3f(p2[0],p2[1],p2[2]);
	glEnd();
}
//-----------------------------------------------------------------------------
void mglGraphGL::trig_plot_n(mreal *p0,mreal *p1,mreal *p2,mreal *c0,mreal *c1,mreal *c2,mreal *n0,mreal *n1,mreal *n2)
{
	glBegin(GL_TRIANGLES);
	glColor4f(c0[0],c0[1],c0[2],c0[3]);	glNormal3f(n0[0],n0[1],n0[2]);	glVertex3f(p0[0],p0[1],p0[2]);
	glColor4f(c1[1],c1[1],c1[2],c1[3]);	glNormal3f(n1[0],n1[1],n1[2]);	glVertex3f(p1[0],p1[1],p1[2]);
	glColor4f(c2[0],c2[1],c2[2],c2[3]);	glNormal3f(n2[0],n2[1],n2[2]);	glVertex3f(p2[0],p2[1],p2[2]);
	glEnd();
}
//-----------------------------------------------------------------------------
void mglGraphGL::quad_plot(mreal *p0,mreal *p1,mreal *p2,mreal *p3, mreal *c0,mreal *c1,mreal *c2,mreal *c3)
{
	glBegin(GL_QUADS);
	glColor4f(c0[0],c0[1],c0[2],c0[3]);	glVertex3f(p0[0],p0[1],p0[2]);
	glColor4f(c1[0],c1[1],c1[2],c1[3]);	glVertex3f(p1[0],p1[1],p1[2]);
	glColor4f(c3[0],c3[1],c3[2],c3[3]);	glVertex3f(p3[0],p3[1],p3[2]);
	glColor4f(c2[0],c2[1],c2[2],c2[3]);	glVertex3f(p2[0],p2[1],p2[2]);
	glEnd();
}
//-----------------------------------------------------------------------------
void mglGraphGL::quad_plot_a(mreal *p0,mreal *p1,mreal *p2,mreal *p3,
				mreal a0,mreal a1,mreal a2,mreal a3,mreal alpha)
{
	register mreal t,s;
	register long k;
	long n = NumCol-1;
	mglColor c;

	glBegin(GL_QUADS);
	s = a0;
	t = alpha*(alpha>0 ? (s+1.f)*(s+1.f) : (1.f-s)*(s-1.f));
	s = n*(s+1.f)/2.f;	k = long(s);	s -= k;
	if(k<n)	c = cmap[k]*(1.f-s) + cmap[k+1]*s;	else	c = cmap[n];
	glColor4f(c.r,c.g,c.b,t);	glVertex3f(p0[0],p0[1],p0[2]);

	s = a1;
	t = alpha*(alpha>0 ? (s+1.f)*(s+1.f) : (1.f-s)*(s-1.f));
	s = n*(s+1.f)/2.f;	k = long(s);	s -= k;
	if(k<n)	c = cmap[k]*(1.f-s) + cmap[k+1]*s;	else	c = cmap[n];
	glColor4f(c.r,c.g,c.b,t);	glVertex3f(p1[0],p1[1],p1[2]);

	s = a3;
	t = alpha*(alpha>0 ? (s+1.f)*(s+1.f) : (1.f-s)*(s-1.f));
	s = n*(s+1.f)/2.f;	k = long(s);	s -= k;
	if(k<n)	c = cmap[k]*(1.f-s) + cmap[k+1]*s;	else	c = cmap[n];
	glColor4f(c.r,c.g,c.b,t);	glVertex3f(p3[0],p3[1],p3[2]);

	s = a2;
	t = alpha*(alpha>0 ? (s+1.f)*(s+1.f) : (1.f-s)*(s-1.f));
	s = n*(s+1.f)/2.f;	k = long(s);	s -= k;
	if(k<n)	c = cmap[k]*(1.f-s) + cmap[k+1]*s;	else	c = cmap[n];
	glColor4f(c.r,c.g,c.b,t);	glVertex3f(p2[0],p2[1],p2[2]);
	glEnd();
}
//-----------------------------------------------------------------------------
void mglGraphGL::quad_plot_n(mreal *p0,mreal *p1,mreal *p2,mreal *p3,mreal *c0,mreal *c1,mreal *c2,mreal *c3,mreal *n0,mreal *n1,mreal *n2,mreal *n3)
{
	glBegin(GL_QUADS);
	glColor4f(c0[0],c0[1],c0[2],c0[3]);	glNormal3f(n0[0],n0[1],n0[2]);	glVertex3f(p0[0],p0[1],p0[2]);
	glColor4f(c1[0],c1[1],c1[2],c1[3]);	glNormal3f(n1[0],n1[1],n1[2]);	glVertex3f(p1[0],p1[1],p1[2]);
	glColor4f(c3[0],c3[1],c3[2],c3[3]);	glNormal3f(n3[0],n3[1],n3[2]);	glVertex3f(p3[0],p3[1],p3[2]);
	glColor4f(c2[0],c2[1],c2[2],c2[3]);	glNormal3f(n2[0],n2[1],n2[2]);	glVertex3f(p2[0],p2[1],p2[2]);
	glEnd();
}
//-----------------------------------------------------------------------------
void mglGraphGL::line_plot(mreal *p1,mreal *p2,mreal *c1,mreal *c2,bool all)
{
	if(all)		Pen(NC,'-',BaseLineWidth);
	glBegin(GL_LINES);
	glColor3f(c1[0],c1[1],c1[2]);	glVertex3f(p1[0],p1[1],p1[2]);
	glColor3f(c2[0],c2[1],c2[2]);	glVertex3f(p2[0],p2[1],p2[2]);
	glEnd();
}
//-----------------------------------------------------------------------------
void mglGraphGL::mark_plot(mreal *pp, char type)
{
	mreal x=pp[0],y=pp[1],z=pp[2], s=MarkSize*0.35*font_factor;	// 175 = 0.35*500
//	if(!ScalePoint(x,y,z))	return;
	Pen(NC,'-',BaseLineWidth);
	glColor3f(CDef[0],CDef[1],CDef[2]);
	if(type=='.')
	{
		glBegin(GL_POINTS);
		glVertex3f(x,y,z);
		glEnd();
	}
	else
	{
		register int i;
		switch(type)
		{
		case '+':
			s = s*1.1;
			glBegin(GL_LINES);
			glVertex3f(x-s,y,z);		glVertex3f(x+s,y,z);
			glVertex3f(x,y-s,z);		glVertex3f(x,y+s,z);
			glEnd();				break;
		case 'x':
			glBegin(GL_LINES);
			glVertex3f(x-s,y-s,z);		glVertex3f(x+s,y+s,z);
			glVertex3f(x+s,y-s,z);		glVertex3f(x-s,y+s,z);
			glEnd();				break;
		case 's':
			glBegin(GL_LINE_LOOP);
			glVertex3f(x-s,y-s,z);		glVertex3f(x+s,y-s,z);
			glVertex3f(x+s,y+s,z);		glVertex3f(x-s,y+s,z);
			glEnd();				break;
		case 'd':
			s = s*1.1;
			glBegin(GL_LINE_LOOP);
			glVertex3f(x,y-s,z);		glVertex3f(x+s,y,z);
			glVertex3f(x,y+s,z);		glVertex3f(x-s,y,z);
			glEnd();				break;
		case '*':
			s = s*1.1;
			glBegin(GL_LINES);
			glVertex3f(x-s,y,z);			glVertex3f(x+s,y,z);
			glVertex3f(x-0.6*s,y-0.8*s,z);	glVertex3f(x+0.6*s,y+0.8*s,z);
			glVertex3f(x+0.6*s,y-0.8*s,z);	glVertex3f(x-0.6*s,y+0.8*s,z);
			glEnd();				break;
		case 'Y':
			glBegin(GL_LINES);
			glVertex3f(x,y-s,z);			glVertex3f(x,y,z);
			glVertex3f(x-0.8*s,y+0.6*s,z);	glVertex3f(x,y,z);
			glVertex3f(x+0.8*s,y+0.6*s,z);	glVertex3f(x,y,z);
			glEnd();				break;

		case '^':
			s = s*1.1;
			glBegin(GL_LINE_LOOP);
			glVertex3f(x-s,y-s/2,z);	glVertex3f(x+s,y-s/2,z);
			glVertex3f(x,y+s,z);		glEnd();		break;
		case 'v':
			s = s*1.1;
			glBegin(GL_LINE_LOOP);
			glVertex3f(x-s,y+s/2,z);	glVertex3f(x+s,y+s/2,z);
			glVertex3f(x,y-s,z);		glEnd();		break;

		case 'R':
			s = s*1.1;
			glBegin(GL_TRIANGLES);
			glVertex3f(x-s/2,y-s,z);	glVertex3f(x-s/2,y+s,z);
			glVertex3f(x+s,y,z);		glEnd();		break;
		case '>':
			s = s*1.1;
			glBegin(GL_LINE_LOOP);
			glVertex3f(x-s/2,y-s,z);	glVertex3f(x-s/2,y+s,z);
			glVertex3f(x+s,y,z);		glEnd();		break;
		case 'L':
			s = s*1.1;
			glBegin(GL_TRIANGLES);
			glVertex3f(x+s/2,y-s,z);	glVertex3f(x+s/2,y+s,z);
			glVertex3f(x-s,y,z);		glEnd();		break;
		case '<':
			s = s*1.1;
			glBegin(GL_LINE_LOOP);
			glVertex3f(x+s/2,y-s,z);	glVertex3f(x+s/2,y+s,z);
			glVertex3f(x-s,y,z);		glEnd();		break;

		case 'S':
			glBegin(GL_QUADS);
			glVertex3f(x-s,y-s,z);		glVertex3f(x+s,y-s,z);
			glVertex3f(x+s,y+s,z);		glVertex3f(x-s,y+s,z);
			glEnd();				break;
		case 'D':
			s = s*1.1;
			glBegin(GL_QUADS);
			glVertex3f(x,y-s,z);		glVertex3f(x+s,y,z);
			glVertex3f(x,y+s,z);		glVertex3f(x-s,y,z);
			glEnd();				break;
		case 'T':
			s = s*1.1;
			glBegin(GL_TRIANGLES);
			glVertex3f(x-s,y-s/2,z);	glVertex3f(x+s,y-s/2,z);
			glVertex3f(x,y+s,z);		glEnd();		break;
		case 'V':
			s = s*1.1;
			glBegin(GL_TRIANGLES);
			glVertex3f(x-s,y+s/2,z);	glVertex3f(x+s,y+s/2,z);
			glVertex3f(x,y-s,z);		glEnd();		break;
		case 'o':
			glBegin(GL_LINE_LOOP);
			for(i=0;i<16;i++)
				glVertex3f(x+s*cos(i*M_PI_4/2),y+s*sin(i*M_PI_4/2),z);
			glEnd();	break;
		case 'O':
			glBegin(GL_POLYGON);
			for(i=0;i<16;i++)
				glVertex3f(x+s*cos(i*M_PI_4/2),y+s*sin(i*M_PI_4/2),z);
			glEnd();	break;
		}
	}
}
//-----------------------------------------------------------------------------
void mglGraphGL::InPlot(mreal x1,mreal x2,mreal y1,mreal y2,bool rel)
{
	mglGraphAB::InPlot(x1,x2,y1,y2,rel);
	glMatrixMode(GL_MODELVIEW);//GL_MODELVIEW GL_VIEWPORT GL_PROJECTION
	glLoadIdentity();
	glScaled(2,2,1.5);
	glTranslated(-0.5,-0.5,-0.5);
}
//-----------------------------------------------------------------------------
void mglGraphGL::Glyph(mreal x, mreal y, mreal f, int s, long j, char col)
{
	int ss=s&3;
	mreal p[12];
	f /= fnt->GetFact(ss);
	mglColor cc = mglColor(col);
	if(!cc.Valid())	cc = mglColor(CDef[0],CDef[1],CDef[2]);
	glColor4f(cc.r,cc.g,cc.b,CDef[3]);
/*	if(s&8)
	{
		mreal dy = 0.004;
		if(s&4)	glBegin(GL_LINE_LOOP);
		else	glBegin(GL_QUADS);
		p[2]=p[5]=p[8]=p[11]=0;
		p[0]=p[6]=x;			p[1]=p[4] =y+dy;
		p[3]=p[9]=fabs(f)+x;	p[7]=p[10]=y-dy;
		PostScale(p,4);
		glVertex3f(p[0],p[1],p[2]);		glVertex3f(p[3],p[4],p[5]);
		glVertex3f(p[9],p[10],p[11]);	glVertex3f(p[6],p[7],p[8]);
		glEnd();
	}
	else if(s&4)
	{
		const short *line = fnt->GetLn(ss,j);
		long ik,ii,il=0, nl=fnt->GetNl(ss,j);
		if(!line || nl<=0)	return;
		glBegin(GL_LINES);
		for(ik=0;ik<nl;ik++)
		{
			ii = 2*ik;
			if(line[ii]==0x3fff && line[ii+1]==0x3fff)	// line breakthrough
			{	il = ik+1;	continue;	}
			else if(ik==nl-1 || (line[ii+2]==0x3fff && line[ii+3]==0x3fff))
			{	// enclose the circle. May be in future this block should be commented
				p[0]=f*line[ii]+x;	p[1]=f*line[ii+1]+y;	p[2]=0;	ii=2*il;
				p[3]=f*line[ii]+x;	p[4]=f*line[ii+1]+y;	p[5]=0;
			}
			else
			{	// normal line
				p[0]=f*line[ii]+x;	p[1]=f*line[ii+1]+y;	p[2]=0;	ii+=2;
				p[3]=f*line[ii]+x;	p[4]=f*line[ii+1]+y;	p[5]=0;
			}
			PostScale(p,2);
			glVertex3f(p[0],p[1],p[2]);	glVertex3f(p[3],p[4],p[5]);
		}
		glEnd();
	}
	else
	{
		const short *trig = fnt->GetTr(ss,j);
		register long ik,ii, nt=fnt->GetNt(ss,j);
		if(!trig || nt<=0)	return;
		glBegin(GL_TRIANGLES);
		for(ik=0;ik<nt;ik++)
		{
			ii = 6*ik;	p[0]=f*trig[ii]+x;	p[1]=f*trig[ii+1]+y;	p[2]=0;
			ii+=2;		p[3]=f*trig[ii]+x;	p[4]=f*trig[ii+1]+y;	p[5]=0;
			ii+=2;		p[6]=f*trig[ii]+x;	p[7]=f*trig[ii+1]+y;	p[8]=0;
			PostScale(p,3);
			glVertex3f(p[0],p[1],p[2]);	glVertex3f(p[3],p[4],p[5]);
			glVertex3f(p[6],p[7],p[8]);
		}
		glEnd();
	}
*/
	if((s&4)==0)
	{
		if(s&8)
		{
			mreal dy = 0.004;
			glBegin(GL_QUADS);
			p[2]=p[5]=p[8]=p[11]=0;
			p[0]=p[6]=x;			p[1]=p[4] =y+dy;
			p[3]=p[9]=fabs(f)+x;	p[7]=p[10]=y-dy;
			PostScale(p,4);
			glVertex3f(p[0],p[1],p[2]);		glVertex3f(p[3],p[4],p[5]);
			glVertex3f(p[9],p[10],p[11]);	glVertex3f(p[6],p[7],p[8]);
			glEnd();
		}
		else
		{
			const short *trig = fnt->GetTr(ss,j);
			register long ik,ii, nt=fnt->GetNt(ss,j);
			if(!trig || nt<=0)	return;
			glBegin(GL_TRIANGLES);
			for(ik=0;ik<nt;ik++)
			{
				ii = 6*ik;	p[0]=f*trig[ii]+x;	p[1]=f*trig[ii+1]+y;	p[2]=0;
				ii+=2;		p[3]=f*trig[ii]+x;	p[4]=f*trig[ii+1]+y;	p[5]=0;
				ii+=2;		p[6]=f*trig[ii]+x;	p[7]=f*trig[ii+1]+y;	p[8]=0;
				PostScale(p,3);
				glVertex3f(p[0],p[1],p[2]);	glVertex3f(p[3],p[4],p[5]);
				glVertex3f(p[6],p[7],p[8]);
			}
			glEnd();
		}

	}
	if(s&8)		// duplicate wire  manually
	{
		mreal dy = 0.004;
		glBegin(GL_QUADS);
		p[2]=p[5]=p[8]=p[11]=0;
		p[0]=p[6]=x;			p[1]=p[4] =y+dy;
		p[3]=p[9]=fabs(f)+x;	p[7]=p[10]=y-dy;
		PostScale(p,4);
		glVertex3f(p[0],p[1],p[2]);		glVertex3f(p[3],p[4],p[5]);
		glVertex3f(p[9],p[10],p[11]);	glVertex3f(p[6],p[7],p[8]);
		glEnd();
	}
	else
	{
		const short *line = fnt->GetLn(ss,j);
		long ik,ii,il=0, nl=fnt->GetNl(ss,j);
		if(!line || nl<=0)	return;
		glBegin(GL_LINES);
		for(ik=0;ik<nl;ik++)
		{
			ii = 2*ik;
			if(line[ii]==0x3fff && line[ii+1]==0x3fff)	// line breakthrough
			{	il = ik+1;	continue;	}
			else if(ik==nl-1 || (line[ii+2]==0x3fff && line[ii+3]==0x3fff))
			{	// enclose the circle. May be in future this block should be commented
				p[0]=f*line[ii]+x;	p[1]=f*line[ii+1]+y;	p[2]=0;	ii=2*il;
				p[3]=f*line[ii]+x;	p[4]=f*line[ii+1]+y;	p[5]=0;
			}
			else
			{	// normal line
				p[0]=f*line[ii]+x;	p[1]=f*line[ii+1]+y;	p[2]=0;	ii+=2;
				p[3]=f*line[ii]+x;	p[4]=f*line[ii+1]+y;	p[5]=0;
			}
			PostScale(p,2);
			glVertex3f(p[0],p[1],p[2]);	glVertex3f(p[3],p[4],p[5]);
		}
		glEnd();
	}
}
//-----------------------------------------------------------------------------
