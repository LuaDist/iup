/***************************************************************************
 * mgl_exec.cpp is part of Math Graphic Library
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
#include <wchar.h>
#ifdef WIN32
#include <io.h>
#ifdef _MSC_VER
#include <direct.h>
#endif
#ifdef __CYGWIN__
#include <unistd.h>
#endif
#else
#include <unistd.h>
#endif

#include "mgl/mgl_parse.h"
#define iint(x)	floor((x)+0.5)
char *mgl_strdup(const char *s);
wchar_t *mgl_str_copy(const char *s);
//-----------------------------------------------------------------------------
int mgls_addlegend(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==2 && k[1]==2)	gr->AddLegend(a[0].w,a[1].s);
	else	return 1;
	return 0;
}
void mglc_addlegend(wchar_t out[1024], long , mglArg *a, int k[10])
{	if(k[0]==2 && k[1]==2)	mglprintf(out,1024,L"gr->AddLegend(\"%s\", \"%s\");",a[0].s,a[1].s);	}
//-----------------------------------------------------------------------------
int mgls_addto(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1)		*(a[0].d) += *(a[1].d);
	else if(k[0]==1 && k[1]==3)	*(a[0].d) += a[1].v;
	else	return 1;
	return 0;
}
void mglc_addto(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1)		mglprintf(out,1024,L"%s += %s;",a[0].s, a[1].s);
	else if(k[0]==1 && k[1]==3)	mglprintf(out,1024,L"%s += %g;",a[0].s, a[1].v);
}
//-----------------------------------------------------------------------------
int mgls_alpha(mglGraph *gr, long , mglArg *a, int k[10])
{
	gr->Alpha(k[0]==3 ? a[0].v!=0 : true);	return 0;
}
void mglc_alpha(wchar_t out[1024], long , mglArg *a, int k[10])
{
	mglprintf(out,1024,L"gr->Alpha(%s);",(k[0]==3&&a[0].v!=0)?"true":"false");
}
//-----------------------------------------------------------------------------
int mgls_alphadef(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==3)	gr->AlphaDef = a[0].v;
	else	return 1;
	return 0;
}
void mglc_alphadef(wchar_t out[1024], long , mglArg *a, int k[10])
{	if(k[0]==3)	mglprintf(out,1024,L"gr->AlphaDef = %g;",a[0].v);	}
//-----------------------------------------------------------------------------
int mgls_ambient(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==3)	gr->Ambient(a[0].v);
	else	return 1;
	return 0;
}
void mglc_ambient(wchar_t out[1024], long , mglArg *a, int k[10])
{	if(k[0]==3)	mglprintf(out,1024,L"gr->Ambient(%g);",a[0].v);	}
//-----------------------------------------------------------------------------
int mgls_area(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return 1;
	else if(k[1]!=1)	gr->Area(*(a[0].d),k[1]==2?a[1].s:0,k[2]==3 ? a[2].v : NAN);
	else if(k[2]!=1)	gr->Area(*(a[0].d),*(a[1].d),k[2]==2?a[2].s:0,k[3]==3 ? a[3].v : NAN);
	else 				gr->Area(*(a[0].d),*(a[1].d),*(a[2].d),k[3]==2?a[3].s:0);
	return 0;
}
void mglc_area(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return;
	else if(k[1]!=1)
		mglprintf(out,1024,L"gr->Area(%s, \"%s\", %g);",a[0].s, k[1]==2?a[1].s:"", k[2]==3?a[2].v:NAN);
	else if(k[2]!=1)
		mglprintf(out,1024,L"gr->Area(%s, %s, \"%s\", %g);", a[0].s, a[1].s, k[2]==2?a[2].s:"", k[3]==3?a[3].v:NAN);
	else 	mglprintf(out,1024,L"gr->Area(%s, %s, %s, \"%s\");", a[0].s, a[1].s, a[2].s, k[3]==2?a[3].s:"");
}
//-----------------------------------------------------------------------------
int mgls_aspect(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==3 && k[1]==3)	gr->Aspect(a[0].v, a[1].v, k[2]==3?a[2].v:1);
	else	return 1;
	return 0;
}
void mglc_aspect(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==3 && k[1]==3)
		mglprintf(out,1024,L"gr->Aspect(%g, %g, %g);", a[0].v, a[1].v, k[2]==3?a[2].v:1);
}
//-----------------------------------------------------------------------------
int mgls_axial(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return 1;
	else if(k[1]!=1)	gr->Axial(*(a[0].d),k[1]==2?a[1].s:0,k[2]==3 ? iint(a[2].v) : 3);
	else if(k[1]==1 && k[2]==1)
		gr->Axial(*(a[0].d),*(a[1].d),*(a[2].d),k[3]==2?a[3].s:0,k[4]==3 ? iint(a[4].v) : 3);
	else	return 1;
	return 0;
}
void mglc_axial(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return;
	else if(k[1]!=1)
		mglprintf(out,1024,L"gr->Axial(%s, \"%s\", %d);", a[0].s,k[1]==2?a[1].s:"",k[2]==3 ? iint(a[2].v) : 3);
	else if(k[1]==1 && k[2]==1)
		mglprintf(out,1024,L"gr->Axial(%s, %s, %s, \"%s\", %d);", a[0].s, a[1].s, a[2].s, k[3]==2?a[3].s:"", k[4]==3?iint(a[4].v):3);
}
//-----------------------------------------------------------------------------
int mgls_axialdir(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==2)	gr->AxialDir = a[0].s[0];	else	return 1;
	return 0;
}
void mglc_axialdir(wchar_t out[1024], long , mglArg *a, int k[10])
{	if(k[0]==2)	mglprintf(out,1024,L"gr->AxialDir = '%c';", a[0].s[0]);	}
//-----------------------------------------------------------------------------
int mgls_axis(mglGraph *gr, long n, mglArg *a, int k[10])
{
	register int i;
	if(k[0]==2 && k[1]==2)	gr->SetFunc(a[0].s,a[1].s,k[2]==2?a[2].s:"",k[3]==2?a[3].s:"");
	else if(n==6)
	{
		bool ok=true;
		for(i=0;i<6;i++)	if(k[i]!=3)	ok = false;
		if(ok)	gr->Axis(mglPoint(a[0].v,a[1].v,a[2].v), mglPoint(a[3].v,a[4].v,a[5].v));
		else return 1;
	}
	else if(n==4)
	{
		bool ok=true;
		for(i=0;i<4;i++)	if(k[i]!=3)	ok = false;
		if(ok)	gr->Axis(mglPoint(a[0].v,a[1].v), mglPoint(a[2].v,a[3].v));
		else return 1;
	}
	else if(k[0]==2)	gr->Axis(a[0].s, k[1]==3 && a[1].v!=0);
	else if(k[0]==3)	gr->SetCoor(iint(a[0].v));
	else if(k[0]==0)	gr->Axis("xyz");
	else return 1;
	return 0;
}
void mglc_axis(wchar_t out[1024], long n, mglArg *a, int k[10])
{
	register int i;
	if(k[0]==2 && k[1]==2 && k[2]==2)
		mglprintf(out,1024,L"gr->SetFunc(\"%s\", \"%s\", \"%s\", \"%s\");", a[0].s,a[1].s,a[2].s,k[2]==2?a[2].s:"",k[3]==2?a[3].s:"");
	else if(n==6)
	{
		bool ok=true;
		for(i=0;i<6;i++)	if(k[i]!=3)	ok = false;
		if(ok)	mglprintf(out,1024,L"gr->Axis(mglPoint(%g, %g, %g), mglPoint(%g, %g, %g));", a[0].v,a[1].v,a[2].v,a[3].v,a[4].v,a[5].v);
	}
	else if(n==4)
	{
		bool ok=true;
		for(i=0;i<4;i++)	if(k[i]!=3)	ok = false;
		if(ok)	mglprintf(out,1024,L"gr->Axis(mglPoint(%g, %g), mglPoint(%g, %g));", a[0].v,a[1].v,a[2].v,a[3].v);
	}
	else if(k[0]==2)	mglprintf(out,1024,L"gr->Axis(\"%s\");", a[0].s);
	else if(k[0]==3)	mglprintf(out,1024,L"gr->SetCoor(\"%d\");", iint(a[0].v));
	else if(k[0]==0)	mglprintf(out,1024,L"gr->Axis(\"xyz\");");
}
//-----------------------------------------------------------------------------
int mgls_ball(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==3 && k[1]==3 && k[2]==3)
		gr->Ball(mglPoint(a[0].v,a[1].v,a[2].v),k[3]==2 ? a[3].s[0]:'r');
	else if(k[0]==3 && k[1]==3)
		gr->Ball(mglPoint(a[0].v,a[1].v),k[2]==2 ? a[2].s[0]:'r');
	else	return 1;
	return 0;
}
void mglc_ball(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==3 && k[1]==3 && k[2]==3)
		mglprintf(out,1024,L"gr->Ball(mglPoint(%g, %g, %g), '%c');", a[0].v,a[1].v,a[2].v,k[3]==2 ? a[3].s[0]:'r');
	else if(k[0]==3 && k[1]==3)
		mglprintf(out,1024,L"gr->Ball(mglPoint(%g, %g), '%c');", a[0].v, a[1].v, k[2]==2 ? a[2].s[0]:'r');
}
//-----------------------------------------------------------------------------
int mgls_box(mglGraph *gr, long , mglArg *a, int k[10])
{
	gr->Box(k[0]==2 ? a[0].s : (gr->TranspType!=2 ?"k-":"w-"), k[1]!=3 || a[1].v);
	return 0;
}
void mglc_box(wchar_t out[1024], long , mglArg *a, int k[10])
{	mglprintf(out,1024,L"gr->Box(\"%s\", %s);", k[0]==2 ? a[0].s : "", k[1]!=3 || a[1].v ? "true":"false");	}
//-----------------------------------------------------------------------------
int mgls_bars(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return 1;
	else if(k[1]!=1)	gr->Bars(*(a[0].d), k[1]==2?a[1].s:0, k[2]==3?a[2].v:NAN);
	else if(k[2]!=1)	gr->Bars(*(a[0].d), *(a[1].d), k[2]==2?a[2].s:0, k[3]==3?a[3].v:NAN);
	else 				gr->Bars(*(a[0].d), *(a[1].d), *(a[2].d), k[3]==2?a[3].s:0);
	return 0;
}
void mglc_bars(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return;
	else if(k[1]!=1)
		mglprintf(out,1024,L"gr->Bars(%s, \"%s\", %g);", a[0].s, k[1]==2?a[1].s:"", k[2]==3?a[2].v:NAN);
	else if(k[2]!=1)
		mglprintf(out,1024,L"gr->Bars(%s, %s, \"%s\", %g);",a[0].s, a[1].s, k[2]==2?a[2].s:"", k[3]==3?a[3].v:NAN);
	else
		mglprintf(out,1024,L"gr->Bars(%s, %s, %s, \"%s\");",a[0].s, a[1].s, a[2].s, k[3]==2?a[3].s:"");
}
//-----------------------------------------------------------------------------
int mgls_barh(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return 1;
	else if(k[1]!=1)
		gr->Barh(*(a[0].d), k[1]==2?a[1].s:0, k[2]==3?a[2].v:NAN);
	else
		gr->Barh(*(a[0].d), *(a[1].d), k[2]==2?a[2].s:0, k[3]==3?a[3].v:NAN);
	return 0;
}
void mglc_barh(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return;
	else if(k[1]!=1)
		mglprintf(out,1024,L"gr->Barh(%s, \"%s\", %g);", a[0].s, k[1]==2?a[1].s:"", k[2]==3?a[2].v:NAN);
	else
		mglprintf(out,1024,L"gr->Barh(%s, %s, \"%s\", %g);",a[0].s, a[1].s, k[2]==2?a[2].s:"", k[3]==3?a[3].v:NAN);
}
//-----------------------------------------------------------------------------
int mgls_belt(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return 1;
	else if(k[1]!=1)			gr->Belt(*(a[0].d),k[1]==2?a[1].s:0);
	else if(k[1]==1 && k[2]==1)	gr->Belt(*(a[0].d),*(a[1].d),*(a[2].d),k[3]==2?a[3].s:0);
	else	return 1;
	return 0;
}
void mglc_belt(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return;
	else if(k[1]!=1)
		mglprintf(out,1024,L"gr->Belt(%s, \"%s\");", a[0].s,k[1]==2?a[1].s:"");
	else if(k[1]==1 && k[2]==1)
		mglprintf(out,1024,L"gr->Belt(%s, %s, %s, \"%s\");", a[0].s, a[1].s, a[2].s, k[3]==2?a[3].s:"");
}
//-----------------------------------------------------------------------------
int mgls_boxs(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return 1;
	else if(k[1]!=1)			gr->Boxs(*(a[0].d),k[1]==2?a[1].s:0);
	else if(k[1]==1 && k[2]==1)	gr->Boxs(*(a[0].d),*(a[1].d),*(a[2].d),k[3]==2?a[3].s:0);
	else	return 1;
	return 0;
}
void mglc_boxs(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return;
	else if(k[1]!=1)
		mglprintf(out,1024,L"gr->Boxs(%s, \"%s\");", a[0].s,k[1]==2?a[1].s:"");
	else if(k[1]==1 && k[2]==1)
		mglprintf(out,1024,L"gr->Boxs(%s, %s, %s, \"%s\");", a[0].s, a[1].s, a[2].s, k[3]==2?a[3].s:"");
}
//-----------------------------------------------------------------------------
int mgls_beam(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1 && k[4]==3)
		gr->Beam(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),a[4].v,
				k[5]==2? a[5].s:0,k[6]==3?iint(a[6].v):0, iint(k[7]==3?a[7].v:3));
	else	return 1;
	return 0;
}
void mglc_beam(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1 && k[4]==3)
		mglprintf(out,1024,L"gr->Beam(%s, %s, %s, %s, %g, \"%s\", %d, %d);", a[0].s, a[1].s, a[2].s, a[3].s, a[4].v, k[5]==2? a[5].s:"",k[6]==3?iint(a[6].v):0, iint(k[7]==3?a[7].v:3));
}
//-----------------------------------------------------------------------------
int mgls_caxis(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==3 && k[1]==3)	gr->CAxis(a[0].v,a[1].v);
	else	return 1;
	return 0;
}
void mglc_caxis(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==3 && k[1]==3)	mglprintf(out,1024,L"gr->CAxis(%g, %g);", a[0].v,a[1].v);
}
//-----------------------------------------------------------------------------
int mgls_clearlegend(mglGraph *gr, long , mglArg *, int [10])
{	gr->ClearLegend();	return 0;	}
void mglc_clearlegend(wchar_t out[1024], long , mglArg *, int [10])
{	mglprintf(out,1024,L"gr->ClearLegend();");	}
//-----------------------------------------------------------------------------
int mgls_clf(mglGraph *gr, long , mglArg *, int [10])
{	gr->Clf();	return 0;	}
void mglc_clf(wchar_t out[1024], long , mglArg *, int [10])
{	mglprintf(out,1024,L"gr->Clf();");	}
//-----------------------------------------------------------------------------
int mgls_chart(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==1)	gr->Chart(*(a[0].d), k[1]==2?a[1].s:0);
	else	return 1;
	return 0;
}
void mglc_chart(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==3 && k[1]==3)	mglprintf(out,1024,L"gr->Chart(%s, \"%s\");", a[0].s, k[1]==2?a[1].s:"");
}
//-----------------------------------------------------------------------------
int mgls_cloud(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return 1;
	else if(k[1]!=1)	gr->Cloud(*(a[0].d),k[1]==2?a[1].s:0);
	else if(k[1]==1 && k[2]==1 && k[3]==1)
		gr->Cloud(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),k[4]==2?a[4].s:0);
	return 0;
}
void mglc_cloud(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return;
	else if(k[1]!=1)
		mglprintf(out,1024,L"gr->Cloud(%s,\"%s\");",a[0].s, k[1]==2?a[1].s:"");
	else if(k[1]==1 && k[2]==1 && k[3]==1)
		mglprintf(out,1024,L"gr->Cloud(%s, %s, %s, %s, \"%s\");", a[0].s,a[1].s,a[2].s,a[3].s,k[4]==2?a[4].s:"");
}
//-----------------------------------------------------------------------------
int mgls_crange(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==1)	gr->CRange(*(a[0].d),k[1]==3?(a[1].v!=0):false,k[2]==3?a[2].v:0);
	else if(k[0]==3 && k[1]==3)	{	gr->Cmin = a[0].v;	gr->Cmax = a[1].v;	}
	else	return 1;
	return 0;
}
void mglc_crange(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1)
		mglprintf(out,1024,L"gr->CRange(%s, %s, %g);",a[0].s, (k[1]==3 && a[1].v!=0)?"true":"false", k[2]==3?a[2].v:0);
	else if(k[0]==3 && k[1]==3)
		mglprintf(out,1024,L"gr->CAxis(%g, %g);", a[0].v, a[1].v);
}
//-----------------------------------------------------------------------------
int mgls_crop(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==3 && k[2]==3 && k[3]==2)
		a[0].d->Crop(iint(a[1].v),iint(a[2].v),a[3].s[0]);
	else	return 1;
	return 0;
}
void mglc_crop(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==3 && k[2]==3 && k[3]==2)
		mglprintf(out,1024,L"%s.Crop(%d, %d, '%c');",a[0].s, iint(a[1].v), iint(a[2].v), a[3].s[0]);
}
//-----------------------------------------------------------------------------
int mgls_cumsum(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2)	a[0].d->CumSum(a[1].s);
	else	return 1;
	return 0;
}
void mglc_cumsum(wchar_t out[1024], long , mglArg *a, int k[10])
{	if(k[0]==1 && k[1]==2)	mglprintf(out,1024,L"%s.CumSum(\"%s\");",a[0].s, a[1].s);	}
//-----------------------------------------------------------------------------
int mgls_curve(mglGraph *gr, long n, mglArg *a, int [10])
{
	register long i;
	for(i=0;i<12;i++)	if(i>=n || a[i].type!=2)	break;
	if(i==12)
		gr->Curve(mglPoint(a[0].v,a[1].v,a[2].v),
			mglPoint(a[3].v,a[4].v,a[5].v),
			mglPoint(a[6].v,a[7].v,a[8].v),
			mglPoint(a[9].v,a[10].v,a[11].v),
			(n>12 && a[12].type==1) ? a[12].s : 0);
	else if(i==8)
		gr->Curve(mglPoint(a[0].v,a[1].v),
			mglPoint(a[2].v,a[3].v),
			mglPoint(a[4].v,a[5].v),
			mglPoint(a[6].v,a[7].v),
			(n>8 && a[8].type==1) ? a[8].s : 0);
	return 0;
}
void mglc_curve(wchar_t out[1024], long n, mglArg *a, int [10])
{
	register long i;
	for(i=0;i<12;i++)	if(i>=n || a[i].type!=2)	break;
	if(i==12)
		mglprintf(out,1024,L"gr->Curve(mglPoint(%g, %g, %g), mglPoint(%g, %g, %g), mglPoint(%g, %g, %g), mglPoint(%g, %g, %g), \"%s\");", a[0].v, a[1].v, a[2].v, a[3].v, a[4].v, a[5].v, a[6].v, a[7].v, a[8].v, a[9].v, a[10].v, a[11].v, (n>12 && a[12].type==1) ? a[12].s : "");
	else if(i==8)
		mglprintf(out,1024,L"gr->Curve(mglPoint(%g, %g), mglPoint(%g, %g), mglPoint(%g, %g), mglPoint(%g, %g), \"%s\");", a[0].v, a[1].v, a[2].v, a[3].v, a[4].v, a[5].v, a[6].v, a[7].v, (n>8 && a[8].type==1) ? a[8].s : "");
}
//-----------------------------------------------------------------------------
int mgls_cut(mglGraph *gr, long n, mglArg *a, int k[10])
{
	if(k[0]==3 && n == 1)	gr->Cut = (a[0].v != 0);
	else if(k[0]==3 && k[1]==3 && k[2]==3 && k[3]==3 && k[4]==3 && k[5]==3)
	{
		gr->CutMin = mglPoint(a[0].v,a[1].v,a[2].v);
		gr->CutMax = mglPoint(a[3].v,a[4].v,a[5].v);
	}
	else if(k[0]==2 && n == 1)	gr->CutOff(a[0].s);
	else	return 1;
	return 0;
}
void mglc_cut(wchar_t out[1024], long n, mglArg *a, int k[10])
{
	if(k[0]==3 && n == 1)	mglprintf(out,1024,L"gr->Cut = %s;",(a[0].v != 0)?"true":"false");
	else if(k[0]==3 && k[1]==3 && k[2]==3 && k[3]==3 && k[4]==3 && k[5]==3)
		mglprintf(out,1024,L"gr->CutMin = mglPoint(%g, %g, %g);\tgr->CutMax = mglPoint(%g, %g, %g);", a[0].v, a[1].v, a[2].v, a[3].v, a[4].v, a[5].v);
	else if(k[0]==2 && n == 1)
		mglprintf(out,1024,L"gr->CutOff(\"%s\");", a[0].s);
}
//-----------------------------------------------------------------------------
int mgls_crust(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==1)
		gr->Crust(*(a[0].d),*(a[1].d),*(a[2].d),k[3]==2?a[3].s:0);
	else if(k[0]==1)
		gr->Crust(*(a[0].d),k[1]==2?a[1].s:0);
	else	return 1;
	return 0;
}
void mglc_crust(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==1)
		mglprintf(out,1024,L"gr->Crust(%s, %s, %s, \"%s\");",a[0].s,a[1].s,a[2].s,k[3]==2?a[3].s:"");
	else if(k[0]==1)
		mglprintf(out,1024,L"gr->Crust(%s, \"%s\");",a[0].s, k[1]==2?a[1].s:"");
}
//-----------------------------------------------------------------------------
int mgls_colorbar(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==2 && k[1]==3 && k[2]==3 && k[3]==3 && k[4]==3 && k[5]==3)
		gr->Colorbar(a[0].s, iint(a[1].v), a[2].v, a[3].v, a[4].v, a[5].v);
	else if(k[0]==1)	gr->Colorbar(*(a[0].d), k[1]==2 ? a[1].s:0, k[2]==3 ? iint(a[2].v):0);
	else	gr->Colorbar(k[0]==2 ? a[0].s:0, k[1]==3 ? iint(a[1].v):0);
	return 0;
}
void mglc_colorbar(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==2 && k[1]==3 && k[2]==3 && k[3]==3 && k[4]==3 && k[5]==3)
		mglprintf(out,1024,L"gr->Colorbar(\"%s\", %d, %g, %g, %g, %g);",a[0].s, iint(a[1].v), a[2].v, a[3].v, a[4].v, a[5].v);
	else if(k[0]==1)
		mglprintf(out,1024,L"gr->Colorbar(%s, \"%s\", %d);",a[0].s, k[1]==2 ? a[1].s:"", k[2]==3 ? iint(a[2].v):0);
	else
		mglprintf(out,1024,L"gr->Colorbar(\"%s\", %d);",k[0]==2 ? a[0].s:"", k[1]==3 ? iint(a[1].v):0);
}
//-----------------------------------------------------------------------------
int mgls_copy(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1)
	{
		a[0].d->Set(*(a[1].d));
		if(k[2]==2 && (k[3]!=3 || a[3].v!=0))
			a[0].d->Fill(a[2].s, gr->Min, gr->Max);
		else if(k[2]==2)	a[0].d->Modify(a[2].s);
	}
	else if(k[0]==1 && k[1]==2)
	{
		a[0].d->Create(1,1,1);
		a[0].d->a[0] = a[1].v;
	}
	else	return 1;
	return 0;
}
void mglc_copy(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==2 && (k[3]!=3 || a[3].v!=0))
		mglprintf(out,1024,L"%s.Set(%s);\t%s.Fill(\"%s\", gr->Min, gr->Max);", a[0].s, a[1].s, a[2].s);
	else if(k[0]==1 && k[1]==1 && k[2]==2)
		mglprintf(out,1024,L"%s.Set(%s);\t%s.Modify(\"%s\");", a[0].s, a[1].s, a[2].s);
	else if(k[0]==1 && k[1]==1)
		mglprintf(out,1024,L"%s.Set(%s);", a[0].s, a[1].s);
	else if(k[0]==1 && k[1]==2)
		mglprintf(out,1024,L"%s.Create();\t%s.a[0]=%g;", a[0].s, a[0].s, a[1].v);
}
//-----------------------------------------------------------------------------
int mgls_cont(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return 1;
	else if(k[1]!=1)	gr->Cont(*(a[0].d),k[1]==2?a[1].s:0,k[2]==3?iint(a[2].v):7,k[3]==3?a[3].v:NAN);
	else if(k[1]==1 && k[2]!=1)
		gr->Cont(*(a[0].d), *(a[1].d), k[2]==2?a[2].s:0,k[3]==3?a[3].v:NAN);
	else if(k[1]==1 && k[2]==1 && k[3]!=1)
		gr->Cont(*(a[0].d), *(a[1].d), *(a[2].d), k[3]==2?a[3].s:0,
			k[4]==3?iint(a[4].v):7, k[5]==3?a[5].v:NAN);
	else if(k[1]==1 && k[2]==1 && k[3]==1 && k[4]!=1)
		gr->Cont(*(a[0].d), *(a[1].d), *(a[2].d), *(a[3].d), k[4]==2?a[4].s:0,
			k[5]==3?a[5].v:NAN);
	else	return 1;
	return 0;
}
void mglc_cont(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return;
	else if(k[1]!=1)
		mglprintf(out,1024,L"gr->Cont(%s, \"%s\", %d, %g);", a[0].s, k[1]==2?a[1].s:"", k[2]==3?iint(a[2].v):7, k[3]==3?a[3].v:NAN);
	else if(k[1]==1 && k[2]!=1)
		mglprintf(out,1024,L"gr->Cont(%s, %s, \"%s\", %g);", a[0].s, a[1].s, k[2]==2?a[2].s:"", k[3]==3?a[3].v:NAN);
	else if(k[1]==1 && k[2]==1 && k[3]!=1)
		mglprintf(out,1024,L"gr->Cont(%s, %s, %s, \"%s\", %d, %g);", a[0].s, a[1].s, a[2].s, k[3]==2?a[3].s:"", k[4]==3?iint(a[4].v):7, k[5]==3?a[5].v:NAN);
	else if(k[1]==1 && k[2]==1 && k[3]==1 && k[4]!=1)
		mglprintf(out,1024,L"gr->Cont(%s, %s, %s, %s, \"%s\", %g);", a[0].s, a[1].s, a[2].s, a[3].s, k[4]==2?a[4].s:"", k[5]==3?a[5].v:NAN);
}
//-----------------------------------------------------------------------------
int mgls_contf(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return 1;
	else if(k[1]!=1)
		gr->ContF(*(a[0].d),k[1]==2?a[1].s:0,k[2]==3?iint(a[2].v):7,k[3]==3?a[3].v:NAN);
	else if(k[1]==1 && k[2]!=1)
		gr->ContF(*(a[0].d), *(a[1].d), k[2]==2?a[2].s:0,k[3]==3?a[3].v:NAN);
	else if(k[1]==1 && k[2]==1 && k[3]!=1)
		gr->ContF(*(a[0].d), *(a[1].d), *(a[2].d), k[3]==2?a[3].s:0,
			k[4]==3?iint(a[4].v):7, k[5]==3?a[5].v:NAN);
	else if(k[1]==1 && k[2]==1 && k[3]==1 && k[4]!=1)
		gr->ContF(*(a[0].d), *(a[1].d), *(a[2].d), *(a[3].d), k[4]==2?a[4].s:0,
			k[5]==3?a[5].v:NAN);
	else	return 1;
	return 0;
}
void mglc_contf(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return;
	else if(k[1]!=1)
		mglprintf(out,1024,L"gr->ContF(%s, \"%s\", %d, %g);", a[0].s, k[1]==2?a[1].s:"", k[2]==3?iint(a[2].v):7, k[3]==3?a[3].v:NAN);
	else if(k[1]==1 && k[2]!=1)
		mglprintf(out,1024,L"gr->ContF(%s, %s, \"%s\", %g);", a[0].s, a[1].s, k[2]==2?a[2].s:"", k[3]==3?a[3].v:NAN);
	else if(k[1]==1 && k[2]==1 && k[3]!=1)
		mglprintf(out,1024,L"gr->ContF(%s, %s, %s, \"%s\", %d, %g);", a[0].s, a[1].s, a[2].s, k[3]==2?a[3].s:"", k[4]==3?iint(a[4].v):7, k[5]==3?a[5].v:NAN);
	else if(k[1]==1 && k[2]==1 && k[3]==1 && k[4]!=1)
		mglprintf(out,1024,L"gr->ContF(%s, %s, %s, %s, \"%s\", %g);", a[0].s, a[1].s, a[2].s, a[3].s, k[4]==2?a[4].s:"", k[5]==3?a[5].v:NAN);
}
//-----------------------------------------------------------------------------
int mgls_contd(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return 1;
	else if(k[1]!=1)
		gr->ContD(*(a[0].d),k[1]==2?a[1].s:0,k[2]==3?iint(a[2].v):7,k[3]==3?a[3].v:NAN);
	else if(k[1]==1 && k[2]!=1)
		gr->ContD(*(a[0].d), *(a[1].d), k[2]==2?a[2].s:0,k[3]==3?a[3].v:NAN);
	else if(k[1]==1 && k[2]==1 && k[3]!=1)
		gr->ContD(*(a[0].d), *(a[1].d), *(a[2].d), k[3]==2?a[3].s:0,
					k[4]==3?iint(a[4].v):7, k[5]==3?a[5].v:NAN);
	else if(k[1]==1 && k[2]==1 && k[3]==1 && k[4]!=1)
		gr->ContD(*(a[0].d), *(a[1].d), *(a[2].d), *(a[3].d), k[4]==2?a[4].s:0,
					k[5]==3?a[5].v:NAN);
	else	return 1;
	return 0;
}
void mglc_contd(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return;
	else if(k[1]!=1)
		mglprintf(out,1024,L"gr->ContD(%s, \"%s\", %d, %g);", a[0].s, k[1]==2?a[1].s:"", k[2]==3?iint(a[2].v):7, k[3]==3?a[3].v:NAN);
	else if(k[1]==1 && k[2]!=1)
		mglprintf(out,1024,L"gr->ContD(%s, %s, \"%s\", %g);", a[0].s, a[1].s, k[2]==2?a[2].s:"", k[3]==3?a[3].v:NAN);
	else if(k[1]==1 && k[2]==1 && k[3]!=1)
		mglprintf(out,1024,L"gr->ContD(%s, %s, %s, \"%s\", %d, %g);", a[0].s, a[1].s, a[2].s, k[3]==2?a[3].s:"", k[4]==3?iint(a[4].v):7, k[5]==3?a[5].v:NAN);
	else if(k[1]==1 && k[2]==1 && k[3]==1 && k[4]!=1)
		mglprintf(out,1024,L"gr->ContD(%s, %s, %s, %s, \"%s\", %g);", a[0].s, a[1].s, a[2].s, a[3].s, k[4]==2?a[4].s:"", k[5]==3?a[5].v:NAN);
}
//-----------------------------------------------------------------------------
int mgls_cont3(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return 1;
	else if(k[1]==2)
		gr->Cont3(*(a[0].d), a[1].s[0], k[2]==3?iint(a[2].v):-1, k[3]==2?a[3].s:0, k[4]==3?iint(a[4].v):7);
	else if(k[1]==1 && k[2]==2)
		gr->Cont3(*(a[0].d), *(a[1].d), a[2].s[0], k[3]==3?iint(a[3].v):-1, k[4]==2?a[4].s:0);
	else if(k[1]==1 && k[2]==1 && k[3]==1 && k[4]==2)
		gr->Cont3(*(a[0].d), *(a[1].d), *(a[2].d), *(a[3].d), a[4].s[0], k[5]==3?iint(a[5].v):-1, k[6]==2?a[6].s:0, k[7]==3?iint(a[7].v):7);
	else if(k[1]==1 && k[2]==1 && k[3]==1 && k[4]==1 && k[5]==2)
		gr->Cont3(*(a[0].d), *(a[1].d), *(a[2].d), *(a[3].d), *(a[4].d), a[5].s[0], k[6]==3?iint(a[6].v):-1, k[7]==2?a[7].s:0);
	else	return 1;
	return 0;
}
void mglc_cont3(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return;
	else if(k[1]==2)
		mglprintf(out,1024,L"gr->Cont3(%s, '%c', %d, \"%s\", %d);", a[0].s, a[1].s[0], k[2]==3?iint(a[2].v):-1, k[3]==2?a[3].s:"", k[4]==3?iint(a[4].v):7);
	else if(k[1]==1 && k[2]==2)
		mglprintf(out,1024,L"gr->Cont3(%s, %s, '%c', %d, \"%s\");", a[0].s, a[1].s, a[2].s[0], k[3]==3?iint(a[3].v):-1, k[4]==2?a[4].s:"");
	else if(k[1]==1 && k[2]==1 && k[3]==1 && k[4]==2)
		mglprintf(out,1024,L"gr->Cont3(%s, %s, %s, %s, '%c', %d, \"%s\", %d);", a[0].s, a[1].s, a[2].s, a[3].s, a[4].s[0], k[5]==3?iint(a[5].v):-1, k[6]==2?a[6].s:0, k[7]==3?iint(a[7].v):7);
	else if(k[1]==1 && k[2]==1 && k[3]==1 && k[4]==1 && k[5]==2)
		mglprintf(out,1024,L"gr->Cont3(%s, %s, %s, %s, %s, '%c', %d, \"%s\");", a[0].s, a[1].s, a[2].s, a[3].s, a[4].s, a[5].s[0], k[6]==3?iint(a[6].v):-1, k[7]==2?a[7].s:0);
}
//-----------------------------------------------------------------------------
int mgls_contf3(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return 1;
	else if(k[1]==2)
		gr->ContF3(*(a[0].d), a[1].s[0], k[2]==3?iint(a[2].v):-1, k[3]==2?a[3].s:0, k[4]==3?iint(a[4].v):7);
	else if(k[1]==1 && k[2]==2)
		gr->ContF3(*(a[0].d), *(a[1].d), a[2].s[0], k[3]==3?iint(a[3].v):-1, k[4]==2?a[4].s:0);
	else if(k[1]==1 && k[2]==1 && k[3]==1 && k[4]==2)
		gr->ContF3(*(a[0].d), *(a[1].d), *(a[2].d), *(a[3].d), a[4].s[0], k[5]==3?iint(a[5].v):-1, k[6]==2?a[6].s:0, k[7]==3?iint(a[7].v):7);
	else if(k[1]==1 && k[2]==1 && k[3]==1 && k[4]==1 && k[5]==2)
		gr->ContF3(*(a[0].d), *(a[1].d), *(a[2].d), *(a[3].d), *(a[4].d), a[5].s[0], k[6]==3?iint(a[6].v):-1, k[7]==2?a[7].s:0);
	else	return 1;
	return 0;
}
void mglc_contf3(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return;
	else if(k[1]==2)
		mglprintf(out,1024,L"gr->ContF3(%s, '%c', %d, \"%s\", %d);", a[0].s, a[1].s[0], k[2]==3?iint(a[2].v):-1, k[3]==2?a[3].s:"", k[4]==3?iint(a[4].v):7);
	else if(k[1]==1 && k[2]==2)
		mglprintf(out,1024,L"gr->ContF3(%s, %s, '%c', %d, \"%s\");", a[0].s, a[1].s, a[2].s[0], k[3]==3?iint(a[3].v):-1, k[4]==2?a[4].s:"");
	else if(k[1]==1 && k[2]==1 && k[3]==1 && k[4]==2)
		mglprintf(out,1024,L"gr->ContF3(%s, %s, %s, %s, '%c', %d, \"%s\", %d);", a[0].s, a[1].s, a[2].s, a[3].s, a[4].s[0], k[5]==3?iint(a[5].v):-1, k[6]==2?a[6].s:0, k[7]==3?iint(a[7].v):7);
	else if(k[1]==1 && k[2]==1 && k[3]==1 && k[4]==1 && k[5]==2)
		mglprintf(out,1024,L"gr->ContF3(%s, %s, %s, %s, %s, '%c', %d, \"%s\");", a[0].s, a[1].s, a[2].s, a[3].s, a[4].s, a[5].s[0], k[6]==3?iint(a[6].v):-1, k[7]==2?a[7].s:0);
}
//-----------------------------------------------------------------------------
int mgls_conta(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return 1;
	else if(k[1]==1 && k[2]==1 && k[3]==1)
		gr->ContA(*(a[0].d), *(a[1].d), *(a[2].d), *(a[3].d), k[4]==2?a[4].s:0, k[5]==3?iint(a[5].v):7);
	else	gr->ContA(*(a[0].d), k[1]==2?a[1].s:0, k[2]==3?iint(a[2].v):7);
	return 0;
}
void mglc_conta(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return;
	else if(k[1]==1 && k[2]==1 && k[3]==1)
		mglprintf(out,1024,L"gr->ContA(%s, %s, %s, %s, \"%s\", %d);", a[0].s, a[1].s, a[2].s, a[3].s, k[4]==2?a[4].s:"", k[5]==3?iint(a[5].v):7);
	else
		mglprintf(out,1024,L"gr->ContA(%s, \"%s\", %d);", a[0].s, k[1]==2?a[1].s:"", k[2]==3?iint(a[2].v):7);
}
//-----------------------------------------------------------------------------
int mgls_contfa(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return 1;
	else if(k[1]==1 && k[2]==1 && k[3]==1)
		gr->ContFA(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),k[4]==2?a[4].s:0,
				k[5]==3?iint(a[5].v):7);
	else	gr->ContFA(*(a[0].d),k[1]==2?a[1].s:0,k[2]==3?iint(a[2].v):7);
	return 0;
}
void mglc_contfa(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return;
	else if(k[1]==1 && k[2]==1 && k[3]==1)
		mglprintf(out,1024,L"gr->ContFA(%s, %s, %s, %s, \"%s\", %d);", a[0].s, a[1].s, a[2].s, a[3].s, k[4]==2?a[4].s:"", k[5]==3?iint(a[5].v):7);
	else
		mglprintf(out,1024,L"gr->ContFA(%s, \"%s\", %d);", a[0].s, k[1]==2?a[1].s:"", k[2]==3?iint(a[2].v):7);
}
//-----------------------------------------------------------------------------
int mgls_contx(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==1)	gr->ContX(*(a[0].d),k[1]==2?a[1].s:0,k[2]==3?a[2].v:NAN,k[3]==3?iint(a[3].v):7);
	else	return 1;
	return 0;
}
void mglc_contx(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1)
		mglprintf(out,1024,L"gr->ContX(%s, \"%s\", %g, %d);", a[0].s, k[1]==2?a[1].s:"", k[2]==3?a[2].v:NAN, k[3]==3?iint(a[3].v):7);
}
//-----------------------------------------------------------------------------
int mgls_contfx(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==1)	gr->ContFX(*(a[0].d),k[1]==2?a[1].s:0,k[2]==3?a[2].v:NAN,k[3]==3?iint(a[3].v):7);
	else	return 1;
	return 0;
}
void mglc_contfx(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1)
		mglprintf(out,1024,L"gr->ContFX(%s, \"%s\", %g, %d);", a[0].s, k[1]==2?a[1].s:"", k[2]==3?a[2].v:NAN, k[3]==3?iint(a[3].v):7);
}
//-----------------------------------------------------------------------------
int mgls_conty(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==1)	gr->ContY(*(a[0].d),k[1]==2?a[1].s:0,k[2]==3?a[2].v:NAN,k[3]==3?iint(a[3].v):7);
	else	return 1;
	return 0;
}
void mglc_conty(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1)
		mglprintf(out,1024,L"gr->ContY(%s, \"%s\", %g, %d);", a[0].s, k[1]==2?a[1].s:"", k[2]==3?a[2].v:NAN, k[3]==3?iint(a[3].v):7);
}
//-----------------------------------------------------------------------------
int mgls_contfy(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==1)	gr->ContFY(*(a[0].d),k[1]==2?a[1].s:0,k[2]==3?a[2].v:NAN,k[3]==3?iint(a[3].v):7);
	else	return 1;
	return 0;
}
void mglc_contfy(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1)
		mglprintf(out,1024,L"gr->ContFY(%s, \"%s\", %g, %d);", a[0].s, k[1]==2?a[1].s:"", k[2]==3?a[2].v:NAN, k[3]==3?iint(a[3].v):7);
}
//-----------------------------------------------------------------------------
int mgls_contz(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==1)	gr->ContZ(*(a[0].d),k[1]==2?a[1].s:0,k[2]==3?a[2].v:NAN,k[3]==3?iint(a[3].v):7);
	else	return 1;
	return 0;
}
void mglc_contz(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1)
		mglprintf(out,1024,L"gr->ContZ(%s, \"%s\", %g, %d);", a[0].s, k[1]==2?a[1].s:"", k[2]==3?a[2].v:NAN, k[3]==3?iint(a[3].v):7);
}
//-----------------------------------------------------------------------------
int mgls_contfz(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==1)	gr->ContFZ(*(a[0].d),k[1]==2?a[1].s:0,k[2]==3?a[2].v:NAN,k[3]==3?iint(a[3].v):7);
	else	return 1;
	return 0;
}
void mglc_contfz(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1)
		mglprintf(out,1024,L"gr->ContFZ(%s, \"%s\", %g, %d);", a[0].s, k[1]==2?a[1].s:"", k[2]==3?a[2].v:NAN, k[3]==3?iint(a[3].v):7);
}
//-----------------------------------------------------------------------------
int mgls_cone(mglGraph *gr, long , mglArg *a, int [10])
{
	bool ok=true;
	for(int i=0;i<7;i++)	if(a[i].type!=2)	ok=false;
	if(ok)
	{
		if(a[7].type==2)
			gr->Cone(mglPoint(a[0].v,a[1].v,a[2].v), mglPoint(a[3].v,a[4].v,a[5].v),
				a[6].v, a[7].v, (a[8].type==1) ? a[8].s : 0, (a[9].type==2) ? a[9].v!=0 : false);
		else
			gr->Cone(mglPoint(a[0].v,a[1].v,a[2].v), mglPoint(a[3].v,a[4].v,a[5].v),
				a[6].v, -1, (a[7].type==1) ? a[7].s : 0, (a[8].type==2) ? a[8].v!=0 : false);
	}
	else	return 1;
	return 0;
}
void mglc_cone(wchar_t out[1024], long , mglArg *a, int [10])
{
	bool ok=true;
	for(int i=0;i<7;i++)	if(a[i].type!=2)	ok=false;
	if(ok)
	{
		if(a[7].type==2)
			mglprintf(out,1024,L"gr->Cone(mglPoint(%g, %g, %g), mglPoint(%g, %g, %g), %g, %g, \"%s\", %s);", a[0].v,a[1].v,a[2].v, a[3].v,a[4].v,a[5].v, a[6].v, a[7].v, (a[8].type==1) ? a[8].s : "", (a[9].type==2 && a[9].v!=0) ? "true" : "false");
		else
			mglprintf(out,1024,L"gr->Cone(mglPoint(%g, %g, %g), mglPoint(%g, %g, %g), %g, -1, \"%s\", %s);", a[0].v,a[1].v,a[2].v, a[3].v,a[4].v,a[5].v, a[6].v, (a[7].type==1) ? a[7].s : "", (a[8].type==2 && a[8].v!=0)? "true" : "false");
	}
}
//-----------------------------------------------------------------------------
int mgls_dens(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return 1;
	else if(k[1]!=1)	gr->Dens(*(a[0].d),k[1]==2?a[1].s:0,k[2]==3?a[2].v:NAN);
	else if(k[1]==1 && k[2]==1 && k[3]!=1)
		gr->Dens(*(a[0].d), *(a[1].d), *(a[2].d), k[3]==2?a[3].s:0, k[4]==3?a[4].v:NAN);
	else	return 1;
	return 0;
}
void mglc_dens(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return;
	else if(k[1]!=1)
		mglprintf(out,1024,L"gr->Dens(%s,\"%s\", %g);", a[0].s, k[1]==2?a[1].s:"", k[2]==3?a[2].v:NAN);
	else if(k[1]==1 && k[2]==1 && k[3]!=1)
		mglprintf(out,1024,L"gr->Dens(%s, %s, %s, \"%s\", %g);", a[0].s, a[1].s, a[2].s, k[3]==2?a[3].s:"", k[4]==3?a[4].v:NAN);
}
//-----------------------------------------------------------------------------
int mgls_dens3(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return 1;
	else if(k[1]==2)
		gr->Dens3(*(a[0].d),a[1].s[0],k[2]==3?iint(a[2].v):-1,
				k[3]==2?a[3].s:0);
	else if(k[1]==1 && k[2]==1 && k[3]==1 && k[4]==2)
		gr->Dens3(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),a[4].s[0],
				k[5]==3?iint(a[5].v):-1, k[6]==2?a[6].s:0);
	else	return 1;
	return 0;
}
void mglc_dens3(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return;
	else if(k[1]==2)
		mglprintf(out,1024,L"gr->Dens3(%s, '%c', %d, \"%s\");", a[0].s, a[1].s[0], k[2]==3?iint(a[2].v):-1, k[3]==2?a[3].s:"");
	else if(k[1]==1 && k[2]==1 && k[3]==1 && k[4]==2)
		mglprintf(out,1024,L"gr->Dens3(%s, %s, %s, %s, '%c', %d, \"%s\");", a[0].s, a[1].s, a[2].s, a[3].s, a[4].s[0], k[5]==3?iint(a[5].v):-1, k[6]==2?a[6].s:0);
}
//-----------------------------------------------------------------------------
int mgls_densa(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return 1;
	else if(k[1]==1 && k[2]==1 && k[3]==1)
		gr->DensA(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),k[4]==2?a[4].s:0);
	else	gr->DensA(*(a[0].d),k[1]==2?a[1].s:0);
	return 0;
}
void mglc_densa(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return;
	else if(k[1]==1 && k[2]==1 && k[3]==1)
		mglprintf(out,1024,L"gr->DensA(%s, %s, %s, %s, \"%s\");", a[0].s, a[1].s, a[2].s, a[3].s, k[4]==2?a[4].s:"");
	else
		mglprintf(out,1024,L"gr->DensA(%s, \"%s\");", a[0].s, k[1]==2?a[1].s:"");
}
//-----------------------------------------------------------------------------
int mgls_densx(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==1)	gr->DensX(*(a[0].d),k[1]==2?a[1].s:0,k[2]==3?a[2].v:NAN);
	else	return 1;
	return 0;
}
void mglc_densx(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1)	mglprintf(out,1024,L"gr->DensX(%s, \"%s\", %g);", a[0].s, k[1]==2?a[1].s:"", k[2]==3?a[2].v:NAN);
}
//-----------------------------------------------------------------------------
int mgls_densy(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==1)	gr->DensY(*(a[0].d),k[1]==2?a[1].s:0,k[2]==3?a[2].v:NAN);
	else	return 1;
	return 0;
}
void mglc_densy(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1)	mglprintf(out,1024,L"gr->DensY(%s, \"%s\", %g);", a[0].s, k[1]==2?a[1].s:"", k[2]==3?a[2].v:NAN);
}
//-----------------------------------------------------------------------------
int mgls_densz(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==1)	gr->DensZ(*(a[0].d),k[1]==2?a[1].s:0,k[2]==3?a[2].v:NAN);
	else	return 1;
	return 0;
}
void mglc_densz(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1)	mglprintf(out,1024,L"gr->DensZ(%s, \"%s\", %g);", a[0].s, k[1]==2?a[1].s:"", k[2]==3?a[2].v:NAN);
}
//-----------------------------------------------------------------------------
int mgls_divto(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1)		*(a[0].d) /= *(a[1].d);
	else if(k[0]==1 && k[1]==3)	*(a[0].d) /= a[1].v;
	else	return 1;
	return 0;
}
void mglc_divto(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1)		mglprintf(out,1024,L"%s /= %s;",a[0].s, a[1].s);
	else if(k[0]==1 && k[1]==3)	mglprintf(out,1024,L"%s /= %g;",a[0].s, a[1].v);
}
//-----------------------------------------------------------------------------
int mgls_multo(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1)		*(a[0].d) *= *(a[1].d);
	else if(k[0]==1 && k[1]==3)	*(a[0].d) *= a[1].v;
	else	return 1;
	return 0;
}
void mglc_multo(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1)		mglprintf(out,1024,L"%s *= %s;",a[0].s, a[1].s);
	else if(k[0]==1 && k[1]==3)	mglprintf(out,1024,L"%s *= %g;",a[0].s, a[1].v);
}
//-----------------------------------------------------------------------------
int mgls_subto(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1)		*(a[0].d) -= *(a[1].d);
	else if(k[0]==1 && k[1]==3)	*(a[0].d) -= a[1].v;
	else	return 1;
	return 0;
}
void mglc_subto(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1)		mglprintf(out,1024,L"%s -= %s;",a[0].s, a[1].s);
	else if(k[0]==1 && k[1]==3)	mglprintf(out,1024,L"%s -= %g;",a[0].s, a[1].v);
}
//-----------------------------------------------------------------------------
int mgls_dots(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1)
		gr->Dots(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),k[4]==2?a[4].s:0);
	else if(k[0]==1 && k[1]==1 && k[2]==1)
		gr->Dots(*(a[0].d),*(a[1].d),*(a[2].d),k[3]==2?a[3].s:0);
	else if(k[0]==1)
		gr->Dots(*(a[0].d),k[1]==2?a[1].s:0);
	else	return 1;
	return 0;
}
void mglc_dots(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==1)
		mglprintf(out,1024,L"gr->Dots(%s, %s, %s, \"%s\");",a[0].s,a[1].s,a[2].s,k[3]==2?a[3].s:"");
	else if(k[0]==1)
		mglprintf(out,1024,L"gr->Dots(%s, \"%s\");",a[0].s, k[1]==2?a[1].s:"");
}
//-----------------------------------------------------------------------------
int mgls_diff(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2)	a[0].d->Diff(a[1].s);
	else if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1)
		a[0].d->Diff(*(a[1].d), *(a[2].d), *(a[3].d));
	else if(k[0]==1 && k[1]==1 && k[2]==1)
		a[0].d->Diff(*(a[1].d), *(a[2].d));
	else	return 1;
	return 0;
}
void mglc_diff(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2)	mglprintf(out,1024,L"%s.Diff(\"%s\");", a[0].s, a[1].s);
	else if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1)
		mglprintf(out,1024,L"%s.Diff(%s, %s, %s);", a[0].s, a[1].s, a[2].s, a[3].s);
	else if(k[0]==1 && k[1]==1 && k[2]==1)
		mglprintf(out,1024,L"%s.Diff(%s, %s);", a[0].s, a[1].s, a[2].s);
}
//-----------------------------------------------------------------------------
int mgls_diff2(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2)	a[0].d->Diff2(a[1].s);
	else	return 1;
	return 0;
}
void mglc_diff2(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2)	mglprintf(out,1024,L"%s.Diff2(\"%s\");", a[0].s, a[1].s);
}
//-----------------------------------------------------------------------------
int mgls_drop(mglGraph *gr, long n, mglArg *a, int k[10])
{
	register int i;
	for(i=0;i<7;i++)	if(k[i]!=3)	break;
	if(i==7)
		gr->Drop(mglPoint(a[0].v,a[1].v,a[2].v), mglPoint(a[3].v,a[4].v,a[5].v), a[6].v,
				k[7]==2 ? a[7].s:"b", k[8]==3 ? a[8].v:1.,  (n>9 && a[9].type==2) ? a[9].v:1.);
	else if(i==5)
		gr->Drop(mglPoint(a[0].v,a[1].v), mglPoint(a[2].v,a[3].v), a[4].v,
				k[5]==2 ? a[5].s:"b", k[6]==3 ? a[6].v:1., k[7]==3 ? a[7].v:1.);
	else	return 1;
	return 0;
}
void mglc_drop(wchar_t out[1024], long n, mglArg *a, int k[10])
{
	register int i;
	for(i=0;i<7;i++)	if(k[i]!=3)	break;
	if(i==7)
		mglprintf(out,1024,L"gr->Drop(mglPoint(%g, %g, %g), mglPoint(%g, %g, %g), %g, \"%s\", %g, %g);", a[0].v,a[1].v,a[2].v, a[3].v,a[4].v,a[5].v, a[6].v, k[7]==2 ? a[7].s:"b", k[8]==3 ? a[8].v:1.,  (n>9 && a[9].type==2) ? a[9].v:1.);
	else if(i==5)
		mglprintf(out,1024,L"gr->Drop(mglPoint(%g, %g), mglPoint(%g, %g), %g, \"%s\", %g, %g);", a[0].v,a[1].v, a[2].v,a[3].v, a[4].v, k[5]==2 ? a[5].s:"b", k[6]==3 ? a[6].v:1., k[7]==3 ? a[7].v:1.);
}
//-----------------------------------------------------------------------------
int mgls_dew(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1)
		gr->Dew(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),k[4]==2?a[4].s:0,k[5]==3?a[5].v:NAN);
	else if(k[0]==1 && k[1]==1)	gr->Dew(*(a[0].d),*(a[1].d),k[2]==2?a[2].s:0,k[3]==3?a[3].v:NAN);
	else	return 1;
	return 0;
}
void mglc_dew(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1)
		mglprintf(out,1024,L"gr->Dew(%s, %s, %s, %s, \"%s\", %g);", a[0].s, a[1].s, a[2].s, a[3].s, k[4]==2?a[4].s:"", k[5]==3?a[5].v:NAN);
	else if(k[0]==1 && k[1]==1)
		mglprintf(out,1024,L"gr->Dew(%s, %s, \"%s\", %g);", a[0].s, a[1].s, k[2]==2?a[2].s:"", k[3]==3?a[3].v:NAN);
}
//-----------------------------------------------------------------------------
int mgls_fall(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return 1;
	else if(k[1]!=1)			gr->Fall(*(a[0].d),k[1]==2?a[1].s:0);
	else if(k[1]==1 && k[2]==1)	gr->Fall(*(a[0].d),*(a[1].d),*(a[2].d),k[3]==2?a[3].s:0);
	else	return 1;
	return 0;
}
void mglc_fall(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return;
	else if(k[1]!=1)
		mglprintf(out,1024,L"gr->Fall(%s, \"%s\");", a[0].s,k[1]==2?a[1].s:"");
	else if(k[1]==1 && k[2]==1)
		mglprintf(out,1024,L"gr->Fall(%s, %s, %s, \"%s\");", a[0].s, a[1].s, a[2].s, k[3]==2?a[3].s:"");
}
//-----------------------------------------------------------------------------
int mgls_mesh(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return 1;
	else if(k[1]!=1)			gr->Mesh(*(a[0].d),k[1]==2?a[1].s:0);
	else if(k[1]==1 && k[2]==1)	gr->Mesh(*(a[0].d),*(a[1].d),*(a[2].d),k[3]==2?a[3].s:0);
	else	return 1;
	return 0;
}
void mglc_mesh(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return;
	else if(k[1]!=1)
		mglprintf(out,1024,L"gr->Mesh(%s, \"%s\");", a[0].s,k[1]==2?a[1].s:"");
	else if(k[1]==1 && k[2]==1)
		mglprintf(out,1024,L"gr->Mesh(%s, %s, %s, \"%s\");", a[0].s, a[1].s, a[2].s, k[3]==2?a[3].s:"");
}
//-----------------------------------------------------------------------------
int mgls_surf(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return 1;
	else if(k[1]!=1)			gr->Surf(*(a[0].d),k[1]==2?a[1].s:0);
	else if(k[1]==1 && k[2]==1)	gr->Surf(*(a[0].d),*(a[1].d),*(a[2].d),k[3]==2?a[3].s:0);
	else	return 1;
	return 0;
}
void mglc_surf(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return;
	else if(k[1]!=1)
		mglprintf(out,1024,L"gr->Surf(%s, \"%s\");", a[0].s,k[1]==2?a[1].s:"");
	else if(k[1]==1 && k[2]==1)
		mglprintf(out,1024,L"gr->Surf(%s, %s, %s, \"%s\");", a[0].s, a[1].s, a[2].s, k[3]==2?a[3].s:"");
}
//-----------------------------------------------------------------------------
int mgls_surfc(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]!=1 || k[1]!=1)	return 1;
	else if(k[2]!=1)			gr->SurfC(*(a[0].d),*(a[1].d),k[2]==2?a[2].s:0);
	else if(k[2]==1 && k[3]==1)	gr->SurfC(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),k[4]==2?a[4].s:0);
	else	return 1;
	return 0;
}
void mglc_surfc(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]!=1 || k[1]!=1)	return;
	else if(k[2]!=1)
		mglprintf(out,1024,L"gr->SurfC(%s, %s, \"%s\");", a[0].s,a[1].s,k[2]==2?a[2].s:"");
	else if(k[2]==1 && k[3]==1)
		mglprintf(out,1024,L"gr->SurfC(%s, %s, %s, %s, \"%s\");", a[0].s, a[1].s, a[2].s, a[3].s, k[4]==2?a[4].s:"");
}
//-----------------------------------------------------------------------------
int mgls_surfa(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]!=1 || k[1]!=1)	return 1;
	else if(k[2]!=1)			gr->SurfA(*(a[0].d),*(a[1].d),k[2]==2?a[2].s:0);
	else if(k[2]==1 && k[3]==1)	gr->SurfA(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),k[4]==2?a[4].s:0);
	else	return 1;
	return 0;
}
void mglc_surfa(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]!=1 || k[1]!=1)	return;
	else if(k[2]!=1)
		mglprintf(out,1024,L"gr->SurfA(%s, %s, \"%s\");", a[0].s,a[1].s,k[2]==2?a[2].s:"");
	else if(k[2]==1 && k[3]==1)
		mglprintf(out,1024,L"gr->SurfA(%s, %s, %s, %s, \"%s\");", a[0].s, a[1].s, a[2].s, a[3].s, k[4]==2?a[4].s:"");
}
//-----------------------------------------------------------------------------
int mgls_flow(mglGraph *gr, long , mglArg *a, int k[10])
{
	int i;
	for(i=0;i<7;i++)	if(k[i]!=1)	break;
	if(i==2)
		gr->Flow(*(a[0].d),*(a[1].d),k[2]==2?a[2].s:0,k[3]==3?iint(a[3].v):5,
			true,k[4]==3?a[4].v:NAN);
	else if(i==4)
		gr->Flow(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),k[4]==2?a[4].s:0,
			k[5]==3?iint(a[5].v):5,true,k[6]==3?a[6].v:NAN);
	else if(i==3)
		gr->Flow(*(a[0].d),*(a[1].d),*(a[2].d),k[3]==2?a[3].s:0,k[4]==3?iint(a[4].v):3);
	else if(i==6)
		gr->Flow(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),*(a[5].d),
			k[6]==2?a[6].s:0,k[7]==3?iint(a[7].v):3);
	else if(k[0]==3 && k[1]==3 && k[2]==1 && k[3]==1 && k[4]==1 && k[5]==1)
		gr->FlowP(mglPoint(a[0].v,a[1].v,k[7]==3?a[7].v:NAN),
			*(a[2].d),*(a[3].d),*(a[4].d),*(a[5].d),k[6]==2?a[6].s:0);
	else if(k[0]==3 && k[1]==3 && k[2]==1 && k[3]==1)
		gr->FlowP(mglPoint(a[0].v,a[1].v,k[5]==3?a[5].v:NAN),
			*(a[2].d),*(a[3].d),k[4]==2?a[4].s:0);
	else if(k[0]==3 && k[1]==3 && k[2]==3 && k[3]==1 && k[4]==1 && k[5]==1 && k[6]==1 && k[7]==1 && k[8]==1)
		gr->FlowP(mglPoint(a[0].v,a[1].v,a[2].v),
			*(a[3].d),*(a[4].d),*(a[5].d),*(a[6].d),*(a[7].d),*(a[8].d),k[9]==2?a[9].s:0);
	else if(k[0]==3 && k[1]==3 && k[2]==3 && k[3]==1 && k[4]==1 && k[5]==1)
		gr->FlowP(mglPoint(a[0].v,a[1].v,a[2].v),
			*(a[3].d),*(a[4].d),*(a[5].d),k[6]==2?a[6].s:0);
	else	return 1;
	return 0;
}
void mglc_flow(wchar_t out[1024], long , mglArg *a, int k[10])
{
	int i;
	for(i=0;i<7;i++)	if(k[i]!=1)	break;
	if(i==2)
		mglprintf(out,1024,L"gr->Flow(%s, %s, \"%s\", %d, true, %g);", a[0].s, a[1].s, k[2]==2?a[2].s:"", k[3]==3?iint(a[3].v):5, k[4]==3?a[4].v:NAN);
	else if(i==4)
		mglprintf(out,1024,L"gr->Flow(%s, %s, %s, %s, \"%s\", %d, true, %g);", a[0].s, a[1].s, a[2].s, a[3].s, k[4]==2?a[4].s:"", k[5]==3?iint(a[5].v):5, k[6]==3?a[6].v:NAN);
	else if(i==3)
		mglprintf(out,1024,L"gr->Flow(%s, %s, %s, \"%s\", %d);", a[0].s, a[1].s, a[2].s, k[3]==2?a[3].s:0, k[4]==3?iint(a[4].v):3);
	else if(i==6)
		mglprintf(out,1024,L"gr->Flow(%s, %s, %s, %s, %s, %s, \"%s\", %d);", a[0].s, a[1].s, a[2].s, a[3].s, a[4].s, a[5].s, k[6]==2?a[6].s:0, k[7]==3?iint(a[7].v):3);
	else if(k[0]==3 && k[1]==3 && k[2]==1 && k[3]==1 && k[4]==1 && k[5]==1)
		mglprintf(out,1024,L"gr->FlowP(mglPoint(%g,%g,%g), %s, %s, %s, %s, \"%s\");",
			a[0].v,a[1].v,k[7]==3?a[7].v:NAN,a[2].s,a[3].s,a[4].s,a[5].s,k[6]==2?a[6].s:"");
	else if(k[0]==3 && k[1]==3 && k[2]==1 && k[3]==1)
		mglprintf(out,1024,L"gr->FlowP(mglPoint(%g,%g,%g), %s, %s, \"%s\");",
			a[0].v,a[1].v,k[5]==3?a[5].v:NAN,a[2].s,a[3].s,k[4]==2?a[4].s:"");
	else if(k[0]==3 && k[1]==3 && k[2]==3 && k[3]==1 && k[4]==1 && k[5]==1 && k[6]==1 && k[7]==1 && k[8]==1)
		mglprintf(out,1024,L"gr->FlowP(mglPoint(%g,%g,%g), %s, %s, %s, %s, %s, %s, \"%s\");",
			a[0].v,a[1].v,a[2].v,a[3].s,a[4].s,a[5].s,a[6].s,a[7].s,a[8].s,k[9]==2?a[9].s:"");
	else if(k[0]==3 && k[1]==3 && k[2]==3 && k[3]==1 && k[4]==1 && k[5]==1)
		mglprintf(out,1024,L"gr->FlowP(mglPoint(%g,%g,%g), %s, %s, %s, \"%s\");",
			a[0].v,a[1].v,a[2].v,a[3].s,a[4].s,a[5].s,k[6]==2?a[6].s:"");
}
//-----------------------------------------------------------------------------
int mgls_grad(mglGraph *gr, long , mglArg *a, int k[10])
{
	int i;
	for(i=0;i<7;i++)	if(k[i]!=1)	break;
	if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1)
		gr->Grad(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d), k[4]==2?a[4].s:0, k[5]==3?iint(a[5].v):5);
	else if(k[0]==1 && k[1]==1 && k[2]==1)
		gr->Grad(*(a[0].d),*(a[1].d),*(a[2].d), k[3]==2?a[3].s:0, k[4]==3?iint(a[4].v):5, k[5]==3?a[5].v:NAN);
	else if(k[0]==1)
		gr->Grad(*(a[0].d), k[1]==2?a[1].s:0, k[2]==3?iint(a[2].v):5, k[3]==3?a[3].v:NAN);
	else	return 1;
	return 0;
}
void mglc_grad(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1)
		mglprintf(out,1024,L"gr->Grad(%s, %s, %s, %s, \"%s\", %d, %g);",
			a[0].s, a[1].s, a[2].s, a[3].s, k[4]==2?a[4].s:"", k[5]==3?iint(a[5].v):5);
	else if(k[0]==1 && k[1]==1 && k[2]==1)
		mglprintf(out,1024,L"gr->Grad(%s, %s, %s, \"%s\", %d, %g);",
			a[0].s, a[1].s, a[2].s, k[3]==2?a[3].s:"", k[4]==3?iint(a[4].v):5, k[5]==3?a[5].v:NAN);
	else if(k[0]==1)
		mglprintf(out,1024,L"gr->Grad(%s, \"%s\", %d, %g);",
			a[0].s, k[1]==2?a[1].s:"", k[2]==3?iint(a[2].v):5, k[3]==3?a[3].v:NAN);
}
//-----------------------------------------------------------------------------
int mgls_fill(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2)
		a[0].d->Fill(a[1].s, gr->Min, gr->Max, k[2]==1?a[2].d:0, k[3]==1?a[3].d:0);
	else if(k[0]==1 && k[1]==3 && k[2]==3)
		a[0].d->Fill(a[1].v,a[2].v,k[3]==2?a[3].s[0]:'x');
	else	return 1;
	return 0;
}
void mglc_fill(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2)
		mglprintf(out,1024,L"%s.Fill(\"%s\", gr->Min, gr->Max, %s, %s);", a[0].s, a[1].s, k[2]==1?a[2].s:"NULL", k[3]==1?a[3].s:"NULL");
	else if(k[0]==1 && k[1]==3 && k[2]==3)
		mglprintf(out,1024,L"%s.Fill(%g, %g, '%c');", a[0].s, a[1].v,a[2].v, k[3]==2?a[3].s[0]:'x');
}
//-----------------------------------------------------------------------------
int mgls_fillsample(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==3 && k[2]==2)	a[0].d->FillSample(iint(a[1].v), a[2].s);
	else	return 1;
	return 0;
}
void mglc_fillsample(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==3 && k[2]==2)
		mglprintf(out,1024,L"%s.FillSample(%d, \"%s\");", a[0].s, iint(a[1].v), a[2].s);
}
//-----------------------------------------------------------------------------
int mgls_fog(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==3)	gr->Fog(a[0].v,k[1]==3?a[1].v:0.25);
	else	return 1;
	return 0;
}
void mglc_fog(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==3)	mglprintf(out,1024,L"gr->Fog(%g, %g);", a[0].v,k[1]==3?a[1].v:0.25);
}
//-----------------------------------------------------------------------------
int mgls_font(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==2)
	{	strcpy(gr->FontDef,a[0].s);	if(k[1]==3)	gr->FontSize = a[1].v;	}
	else	return 1;
	return 0;
}
void mglc_font(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==2 && k[1]==3)
		mglprintf(out,1024,L"strcpy(gr->FontDef,\"%s\");\tgr->FontSize = %g;", a[0].s, a[1].v);
	else if(k[0]==2)
		mglprintf(out,1024,L"strcpy(gr->FontDef,\"%s\");",a[0].s);
}
//-----------------------------------------------------------------------------
int mgls_loadfont(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==2 && a[0].s[0])	gr->GetFont()->Load(a[0].s);
	else	gr->GetFont()->Restore();
	return 0;
}
void mglc_loadfont(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==2 && a[0].s[0])
		mglprintf(out,1024,L"gr->GetFont()->Load(\"%s\");", a[0].s);
	else
		mglprintf(out,1024,L"gr->GetFont()->Restore();");
}
//-----------------------------------------------------------------------------
int mgls_grid(mglGraph *gr, long , mglArg *a, int k[10])
{
	gr->Grid(k[0]==2?a[0].s:"xyzt", k[1]==2?a[1].s:"B-");
	return 0;
}
void mglc_grid(wchar_t out[1024], long , mglArg *a, int k[10])
{
	mglprintf(out,1024,L"gr->Grid(\"%s\", \"%s\");", k[0]==2?a[0].s:"xyz", k[1]==2?a[1].s:"B-");
}
//-----------------------------------------------------------------------------
int mgls_grid2(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]!=1)
		gr->Grid(*(a[0].d), *(a[1].d), *(a[2].d), k[3]==2?a[3].s:0, k[4]==3?a[4].v:NAN);
	else if(k[0]==1)	gr->Grid(*(a[0].d),k[1]==2?a[1].s:0,k[2]==3?a[2].v:NAN);
	else	return 1;
	return 0;
}
void mglc_grid2(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]!=1)
		mglprintf(out,1024,L"gr->Grid(%s, %s, %s, \"%s\", %g);", a[0].s, a[1].s, a[2].s, k[3]==2?a[3].s:"", k[4]==3?a[4].v:NAN);
	else if(k[0]==1)
		mglprintf(out,1024,L"gr->Grid(%s,\"%s\", %g);", a[0].s, k[1]==2?a[1].s:"", k[2]==3?a[2].v:NAN);
}
//-----------------------------------------------------------------------------
int mgls_grid3(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return 1;
	else if(k[1]==2)
		gr->Grid3(*(a[0].d),a[1].s[0],k[2]==3?iint(a[2].v):-1,
				k[3]==2?a[3].s:0);
	else if(k[1]==1 && k[2]==1 && k[3]==1 && k[4]==2)
		gr->Grid3(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),a[4].s[0],
				k[5]==3?iint(a[5].v):-1, k[6]==2?a[6].s:0);
	else	return 1;
	return 0;
}
void mglc_grid3(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return;
	else if(k[1]==2)
		mglprintf(out,1024,L"gr->Grid3(%s, '%c', %d, \"%s\");", a[0].s, a[1].s[0], k[2]==3?iint(a[2].v):-1, k[3]==2?a[3].s:"");
	else if(k[1]==1 && k[2]==1 && k[3]==1 && k[4]==2)
		mglprintf(out,1024,L"gr->Grid3(%s, %s, %s, %s, '%c', %d, \"%s\");", a[0].s, a[1].s, a[2].s, a[3].s, a[4].s[0], k[5]==3?iint(a[5].v):-1, k[6]==2?a[6].s:0);
}
//-----------------------------------------------------------------------------
int mgls_grida(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return 1;
	else if(k[1]==1 && k[2]==1 && k[3]==1)
		gr->GridA(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),k[4]==2?a[4].s:0);
	else	gr->GridA(*(a[0].d),k[1]==2?a[1].s:0);
	return 0;
}
void mglc_grida(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return;
	else if(k[1]==1 && k[2]==1 && k[3]==1)
		mglprintf(out,1024,L"gr->GridA(%s, %s, %s, %s, \"%s\");", a[0].s, a[1].s, a[2].s, a[3].s, k[4]==2?a[4].s:"");
	else
		mglprintf(out,1024,L"gr->GridA(%s, \"%s\");", a[0].s, k[1]==2?a[1].s:"");
}
//-----------------------------------------------------------------------------
int mgls_light(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==0)			gr->Light(true);
	else if(k[0]!=3)	return 1;
	else if(k[1]!=3)	gr->Light(a[0].v!=0);
	else if(k[2]==3 && k[3]==3)
		gr->Light(iint(a[0].v),mglPoint(a[1].v,a[2].v,a[3].v),
			k[4]==2?a[4].s[0]:'w',k[5]==3?a[5].v:0.5,true,k[6]==3?a[6].v:0);
	else	gr->Light(iint(a[0].v),a[1].v!=0);
	return 0;
}
void mglc_light(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==0)			mglprintf(out,1024,L"gr->Light(true);");
	else if(k[0]!=3)	return;
	else if(k[1]!=3)	mglprintf(out,1024,L"gr->Light(%s);", a[0].v!=0 ? "true":"false");
	else if(k[2]==3 && k[3]==3)
		mglprintf(out,1024,L"gr->Light(%d, mglPoint(%g, %g, %g), '%c', %g, true, %g);",
		iint(a[0].v), a[1].v, a[2].v, a[3].v, k[4]==2?a[4].s[0]:'w', k[5]==3?a[5].v:0.5, k[6]==3?a[6].v:0);
	else	mglprintf(out,1024,L"gr->Light(%d, %s);", iint(a[0].v), a[1].v!=0 ? "true":"false");
}
//-----------------------------------------------------------------------------
int mgls_line(mglGraph *gr, long n, mglArg *a, int [10])
{
	int i;
	if(n>5)
	{
		bool ok=true;
		for(i=0;i<6;i++)	if(a[i].type!=2)	ok=false;
		if(ok)
			gr->Line(mglPoint(a[0].v,a[1].v,a[2].v),
				mglPoint(a[3].v,a[4].v,a[5].v),
				(n==7 && a[6].type==1) ? a[6].s : 0, 100);
		else	return 1;
	}
	else if(n>3)
	{
		bool ok=true;
		for(i=0;i<4;i++)	if(a[i].type!=2)	ok=false;
		if(ok)
			gr->Line(mglPoint(a[0].v,a[1].v,gr->Min.z), mglPoint(a[2].v,a[3].v,gr->Min.z),
				(n==5 && a[4].type==1) ? a[4].s : 0, 100);
		else	return 1;
	}
	else	return 1;
	return 0;
}
void mglc_line(wchar_t out[1024], long n, mglArg *a, int [10])
{
	int i;
	if(n>5)
	{
		bool ok=true;
		for(i=0;i<6;i++)	if(a[i].type!=2)	ok=false;
		if(ok)
			mglprintf(out,1024,L"gr->Line(mglPoint(%g, %g, %g), mglPoint(%g, %g, %g), \"%s\", 100);", a[0].v,a[1].v,a[2].v, a[3].v,a[4].v,a[5].v, (n==7 && a[6].type==1) ? a[6].s : "");
	}
	else if(n>3)
	{
		bool ok=true;
		for(i=0;i<4;i++)	if(a[i].type!=2)	ok=false;
		if(ok)
			mglprintf(out,1024,L"gr->Line(mglPoint(%g, %g, gr->Min.z), mglPoint(%g, %g, gr->Min.z), \"%s\", 100);", a[0].v,a[1].v,a[2].v, a[3].v, (n==5 && a[4].type==1) ? a[4].s : "");
	}
}
//-----------------------------------------------------------------------------
int mgls_legend(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==3 && k[1]==3)
		gr->Legend(a[0].v, a[1].v, k[2]==2?a[2].s:"rL", k[3]==3?a[3].v:-1, k[4]==3?a[4].v:0.1);
	else
		gr->Legend(k[0]==3?iint(a[0].v):3, k[1]==2?a[1].s:"rL", k[2]==3?a[2].v:-1, k[3]==3?a[3].v:0.1);
	return 0;
}
void mglc_legend(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==3 && k[1]==3)
		mglprintf(out,1024,L"gr->Legend(%g, %g, \"%s\", %g, %g);", a[0].v, a[1].v, k[2]==2?a[2].s:"rL", k[3]==3?a[3].v:-1, k[4]==3?a[4].v:0.1);
	else
		mglprintf(out,1024,L"gr->Legend(%d, \"%s\", %g, %g);", k[0]==3?iint(a[0].v):3, k[1]==2?a[1].s:"rL", k[2]==3?a[2].v:-1, k[4]==3?a[4].v:0.1);
}
//-----------------------------------------------------------------------------
int mgls_linewidth(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==3 && a[0].v>=1)	gr->BaseLineWidth = a[0].v;
	else	return 1;
	return 0;
}
void mglc_linewidth(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==3 && a[0].v>=1)	mglprintf(out,1024,L"gr->BaseLineWidth = %g;", a[0].v);
}
//-----------------------------------------------------------------------------
int mgls_barwidth(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==3)	gr->SetBarWidth(a[0].v);
	else	return 1;
	return 0;
}
void mglc_barwidth(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==3)	mglprintf(out,1024,L"gr->SetBarWidth(%g);", a[0].v);
}
//-----------------------------------------------------------------------------
int mgls_legendbox(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==3)	gr->SetLegendBox(a[0].v!=0);
	else	return 1;
	return 0;
}
void mglc_legendbox(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==3)	mglprintf(out,1024,L"gr->SetLegendBox(%s);", (a[0].v!=0)?"true":"false");
}
//-----------------------------------------------------------------------------
int mgls_legendmarks(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==3)	gr->SetLegendMarks(iint(a[0].v));
	else	return 1;
	return 0;
}
void mglc_legendmarks(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==3)	mglprintf(out,1024,L"gr->SetLegendMarks(%d);", iint(a[0].v));
}
//-----------------------------------------------------------------------------
int mgls_modify(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2 && k[2]==1 && k[3]==1)
		a[0].d->Modify(a[1].s,*(a[2].d),*(a[3].d));
	else if(k[0]==1 && k[1]==2 && k[2]==1)
		a[0].d->Modify(a[1].s,*(a[2].d));
	else if(k[0]==1 && k[1]==2)
		a[0].d->Modify(a[1].s, k[2]==3 ? iint(a[2].v):0);
	else	return 1;
	return 0;
}
void mglc_modify(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2 && k[2]==1 && k[3]==1)
		mglprintf(out,1024,L"%s.Modify(\"%s\", %s, %s);", a[0].s, a[1].s, a[2].s, a[3].s);
	else if(k[0]==1 && k[1]==2 && k[2]==1)
		mglprintf(out,1024,L"%s.Modify(\"%s\", %s);", a[0].s, a[1].s, a[2].s);
	else if(k[0]==1 && k[1]==2)
		mglprintf(out,1024,L"%s.Modify(\"%s\", %d);", a[0].s, a[1].s, k[2]==3 ? iint(a[2].v):0);
}
//-----------------------------------------------------------------------------
int mgls_max(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==2)	*(a[0].d) = a[1].d->Max(a[2].s);
	else	return 1;
	return 0;
}
void mglc_max(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==2)
		mglprintf(out,1024,L"%s = %s.Max(\"%s\");", a[0].s, a[1].s, a[2].s);
}
//-----------------------------------------------------------------------------
int mgls_min(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==2)	*(a[0].d) = a[1].d->Min(a[2].s);
	else	return 1;
	return 0;
}
void mglc_min(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==2)
		mglprintf(out,1024,L"%s = %s.Min(\"%s\");", a[0].s, a[1].s, a[2].s);
}
//-----------------------------------------------------------------------------
int mgls_sum(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==2)	*(a[0].d) = a[1].d->Sum(a[2].s);
	else	return 1;
	return 0;
}
void mglc_sum(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==2)
		mglprintf(out,1024,L"%s = %s.Sum(\"%s\");", a[0].s, a[1].s, a[2].s);
}
//-----------------------------------------------------------------------------
int mgls_meshnum(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==3)	gr->MeshNum = iint(a[0].v);
	else	return 1;
	return 0;
}
void mglc_meshnum(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==3)	mglprintf(out,1024,L"gr->MeshNum = %d;", iint(a[0].v));
}
//-----------------------------------------------------------------------------
int mgls_marksize(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==3)	gr->MarkSize = a[0].v/50;
	else	return 1;
	return 0;
}
void mglc_marksize(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==3)	mglprintf(out,1024,L"gr->MarkSize = %g;", a[0].v/50);
}
//-----------------------------------------------------------------------------
int mgls_mark(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]!=1 || k[1]!=1)	return 1;
	else if(k[2]!=1)	gr->Mark(*(a[0].d),*(a[1].d),k[2]==2?a[2].s:0,k[3]==3?a[3].v:NAN);
	else if(k[3]!=1)	gr->Mark(*(a[0].d),*(a[1].d),*(a[2].d),k[3]==2?a[3].s:0,k[4]==3?a[4].v:NAN);
	else 				gr->Mark(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),k[4]==2?a[4].s:0);
	return 0;
}
void mglc_mark(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]!=1 || k[1]!=1)	return;
	else if(k[2]!=1)
		mglprintf(out,1024,L"gr->Mark(%s, %s, \"%s\", %g);", a[0].s, a[1].s, k[2]==2?a[2].s:"", k[3]==3?a[3].v:NAN);
	else if(k[3]!=1)
		mglprintf(out,1024,L"gr->Mark(%s, %s, %s, \"%s\", %g);", a[0].s, a[1].s, a[2].s, k[3]==2?a[3].s:"", k[4]==3?a[4].v:NAN);
	else
		mglprintf(out,1024,L"gr->Mark(%s, %s, %s, %s, \"%s\");", a[0].s, a[1].s, a[2].s, a[3].s, k[4]==2?a[4].s:"");
}
//-----------------------------------------------------------------------------
int mgls_map(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1)
		gr->Map(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),k[4]==2?a[4].s:0, 0,k[5]==3?(a[5].v!=0):true);
	else if(k[0]==1 && k[1]==1)
		gr->Map(*(a[0].d),*(a[1].d),k[2]==2?a[2].s:0,0,k[3]==3?(a[3].v!=0):true);
	else	return 1;
	return 0;
}
void mglc_map(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1)
		mglprintf(out,1024,L"gr->Map(%s, %s, %s, %s, \"%s\", 0, %s);", a[0].s, a[1].s, a[2].s, a[3].s, k[4]==2?a[4].s:"", (k[5]!=3 || a[5].v!=0)?"true":"false");
	else if(k[0]==1 && k[1]==1)
		mglprintf(out,1024,L"gr->Map(%s, %s, \"%s\", 0, %s);", a[0].s, a[1].s, k[2]==2?a[2].s:"", (k[3]!=3 || a[3].v!=0)?"true":"false");
}
//-----------------------------------------------------------------------------
int mgls_read(mglGraph *gr, long n, mglArg *a, int k[10])
{
	bool rr=true;
	if(k[0]!=1 || k[1]!=2)	return 1;
	else if(n==2)	rr=a[0].d->Read(a[1].s);
	else rr=a[0].d->Read(a[1].s, k[2]==3?iint(a[2].v):1,
			k[3]==3?iint(a[3].v):1, k[4]==3?iint(a[4].v):1);
	if(!rr)	gr->SetWarn(mglWarnFile);
	return 0;
}
void mglc_read(wchar_t out[1024], long n, mglArg *a, int k[10])
{
	if(k[0]!=1 || k[1]!=2)	return;
	else if(n==2)	mglprintf(out,1024,L"%s.Read(\"%s\");", a[0].s, a[1].s);
	else
		mglprintf(out,1024,L"%s.Read(\"%s\", %d, %d, %d);", a[0].s, a[1].s, k[2]==3?iint(a[2].v):1, k[3]==3?iint(a[3].v):1, k[4]==3?iint(a[4].v):1);
}
//-----------------------------------------------------------------------------
int mgls_readmat(mglGraph *gr, long , mglArg *a, int k[10])
{
	bool rr=true;
	if(k[0]!=1 || k[1]!=2)	return 1;
	else	rr=a[0].d->ReadMat(a[1].s, k[2]==3?iint(a[2].v):2);
	if(!rr)	gr->SetWarn(mglWarnFile);
	return 0;
}
void mglc_readmat(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2)
		mglprintf(out,1024,L"%s.ReadMat(\"%s\", %d);", a[0].s, a[1].s, k[2]==3?iint(a[2].v):2);
}
//-----------------------------------------------------------------------------
int mgls_readall(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]!=1 || k[1]!=2)	return 1;
	else if(k[2]==3 && k[3]==3)
		a[0].d->ReadRange(a[1].s, a[2].v, a[3].v, k[4]==3?a[4].v:1.f, k[5]==3?(a[5].v!=0) : false);
	else a[0].d->ReadAll(a[1].s, k[2]==3?(a[2].v!=0) : false);
	return 0;
}
void mglc_readall(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]!=1 || k[1]!=2)	return;
	else if(k[2]==3 && k[3]==3)
		mglprintf(out,1024,L"%s.ReadRange(\"%s\", %g, %g, %g, %s);", a[0].s, a[1].s, a[2].v, a[3].v, k[4]==3?a[4].v:1.f, (k[5]==3&&a[5].v!=0)?"true":"false");
	else
		mglprintf(out,1024,L"%s.ReadAll(\"%s\", %s);", a[0].s, a[1].s, (k[2]==3&&a[2].v!=0)?"true":"false");
}
//-----------------------------------------------------------------------------
int mgls_readhdf(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2 && k[2]==2)	a[0].d->ReadHDF(a[1].s, a[2].s);
	else	return 1;
	return 0;
}
void mglc_readhdf(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2 && k[2]==2)	mglprintf(out,1024,L"%s.ReadHDF(\"%s\", \"%s\");", a[0].s, a[1].s, a[2].s);
}
//-----------------------------------------------------------------------------
int mgls_savehdf(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2 && k[2]==2)	a[0].d->SaveHDF(a[1].s, a[2].s);
	else	return 1;
	return 0;
}
void mglc_savehdf(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2 && k[2]==2)
		mglprintf(out,1024,L"%s.SaveHDF(\"%s\", \"%s\");", a[0].s, a[1].s, a[2].s);
}
//-----------------------------------------------------------------------------
int mgls_rect(mglGraph *gr, long n, mglArg *a, int [10])
{
	long i;
	if(n>5)
	{
		bool ok=true;
		for(i=0;i<6;i++)	if(a[i].type!=2)	ok=false;
		if(ok)
		{
			if(a[0].v==a[3].v)	gr->Face(mglPoint(a[0].v,a[1].v,a[2].v),
				mglPoint(a[0].v,a[4].v,a[2].v),
				mglPoint(a[3].v,a[1].v,a[5].v),
				mglPoint(a[3].v,a[4].v,a[5].v),
				(n==7 && a[6].type==1) ? a[6].s : 0, 2);
			else	gr->Face(mglPoint(a[0].v,a[1].v,a[2].v),
				mglPoint(a[0].v,a[4].v,a[5].v),
				mglPoint(a[3].v,a[1].v,a[2].v),
				mglPoint(a[3].v,a[4].v,a[5].v),
				(n==7 && a[6].type==1) ? a[6].s : 0, 2);
		}
		else	return 1;
	}
	else if(n>3)
	{
		bool ok=true;
		mreal p = (gr->Min.z+gr->Max.z)/2;
		for(i=0;i<4;i++)	if(a[i].type!=2)	ok=false;
		if(ok)
			gr->Face(mglPoint(a[0].v,a[1].v,p), mglPoint(a[0].v,a[3].v,p),
				mglPoint(a[2].v,a[1].v,p), mglPoint(a[2].v,a[3].v,p),
				(n==5 && a[4].type==1) ? a[4].s : 0, 2);
		else	return 1;
	}
	else	return 1;
	return 0;
}
void mglc_rect(wchar_t out[1024], long n, mglArg *a, int [10])
{
	long i;
	if(n>5)
	{
		bool ok=true;
		for(i=0;i<6;i++)	if(a[i].type!=2)	ok=false;
		if(ok)
		{
			if(a[0].v==a[3].v)
				mglprintf(out,1024,L"gr->Face(mglPoint(%g, %g, %g), mglPoint(%g, %g, %g), mglPoint(%g, %g, %g), mglPoint(%g, %g, %g), \"%s\", 2);", a[0].v,a[1].v,a[2].v, a[0].v,a[4].v,a[2].v, a[3].v,a[1].v,a[5].v, a[3].v,a[4].v,a[5].v, (n==7 && a[6].type==1) ? a[6].s : "");
			else
				mglprintf(out,1024,L"gr->Face(mglPoint(%g, %g, %g), mglPoint(%g, %g, %g), mglPoint(%g, %g, %g), mglPoint(%g, %g, %g), \"%s\", 2);", a[0].v,a[1].v,a[2].v, a[0].v,a[4].v,a[5].v, a[3].v,a[1].v,a[2].v, a[3].v,a[4].v,a[5].v, (n==7 && a[6].type==1) ? a[6].s : "");
		}
	}
	else if(n>3)
	{
		bool ok=true;
		for(i=0;i<4;i++)	if(a[i].type!=2)	ok=false;
		if(ok)
			mglprintf(out,1024,L"gr->Face(mglPoint(%g, %g, (gr->Min.z+gr->Max.z)/2), mglPoint(%g, %g, (gr->Min.z+gr->Max.z)/2), mglPoint(%g, %g, (gr->Min.z+gr->Max.z)/2), mglPoint(%g, %g, (gr->Min.z+gr->Max.z)/2), \"%s\", 2);", a[0].v,a[1].v, a[0].v,a[3].v, a[2].v,a[1].v, a[2].v,a[3].v, (n==5 && a[4].type==1) ? a[4].s : "");
	}
}
//-----------------------------------------------------------------------------
int mgls_resize(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1)
		*(a[0].d) = a[1].d->Resize(k[2]==3?iint(a[2].v):1,
				k[3]==3?iint(a[3].v):1, k[4]==3?iint(a[4].v):1);
	else	return 1;
	return 0;
}
void mglc_resize(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1)
		mglprintf(out,1024,L"%s = %s.Resize(%d, %d, %d);", a[0].s, a[1].s, k[2]==3?iint(a[2].v):1, k[3]==3?iint(a[3].v):1, k[4]==3?iint(a[4].v):1);
}
//-----------------------------------------------------------------------------
int mgls_rotate(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==3 && k[1]==3 && k[2]==3 && k[3]==3)
		gr->RotateN(a[0].v, a[1].v, a[2].v, a[3].v);
	else if(k[0]==3 && k[1]==3)
		gr->Rotate(a[0].v, a[1].v, k[2]==3?a[2].v:0);
	else	return 1;
	return 0;
}
void mglc_rotate(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==3 && k[1]==3 && k[2]==3 && k[3]==3)
		mglprintf(out,1024,L"gr->RotateN(%g, %g, %g, %g);", a[0].v, a[1].v, a[2].v, a[3].v);
	else if(k[0]==3 && k[1]==3)
		mglprintf(out,1024,L"gr->Rotate(%g, %g, %g);", a[0].v, a[1].v, k[2]==3?a[2].v:0);
}
//-----------------------------------------------------------------------------
int mgls_rotatetext(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==3)	gr->RotatedText = (a[0].v!=0);
	else	return 1;
	return 0;
}
void mglc_rotatetext(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==3)	mglprintf(out,1024,L"gr->RotatedText = %s;", (a[0].v!=0)?"true":"false");
}
//-----------------------------------------------------------------------------
int mgls_save(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2)	a[0].d->Save(a[1].s);
	else	return 1;
	return 0;
}
void mglc_save(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2)	mglprintf(out,1024,L"%s.Save(\"%s\");", a[0].s, a[1].s);
}
//-----------------------------------------------------------------------------
int mgls_smooth(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1)		a[0].d->Smooth(k[1]==3?iint(a[1].v):2,k[2]==2?a[2].s:"xyz");
	else	return 1;
	return 0;
}
void mglc_smooth(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1)
		mglprintf(out,1024,L"%s.Smooth(%d, \"%s\");", a[0].s, k[1]==3?iint(a[1].v):2, k[2]==2?a[2].s:"xyz");
}
//-----------------------------------------------------------------------------
int mgls_swap(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2)	a[0].d->Swap(a[1].s);
	else	return 1;
	return 0;
}
void mglc_swap(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2)	mglprintf(out,1024,L"%s.Swap(\"%s\");", a[0].s, a[1].s);
}
//-----------------------------------------------------------------------------
int mgls_idset(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2)	a[0].d->SetColumnId(a[1].s);
	else	return 1;
	return 0;
}
void mglc_idset(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2)	mglprintf(out,1024,L"%s.SetColumnId(\"%s\");", a[0].s, a[1].s);
}
//-----------------------------------------------------------------------------
int mgls_stem(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return 1;
	else if(k[1]!=1)	gr->Stem(*(a[0].d),k[1]==2?a[1].s:0,k[2]==3 ? a[2].v : NAN);
	else if(k[2]!=1)	gr->Stem(*(a[0].d),*(a[1].d),k[2]==2?a[2].s:0,k[3]==3 ? a[3].v : NAN);
	else 				gr->Stem(*(a[0].d),*(a[1].d),*(a[2].d),k[3]==2?a[3].s:0);
	return 0;
}
void mglc_stem(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return;
	else if(k[1]!=1)
		mglprintf(out,1024,L"gr->Stem(%s, \"%s\", %g);",a[0].s, k[1]==2?a[1].s:"", k[2]==3?a[2].v:NAN);
	else if(k[2]!=1)
		mglprintf(out,1024,L"gr->Stem(%s, %s, \"%s\", %g);", a[0].s, a[1].s, k[2]==2?a[2].s:"", k[3]==3?a[3].v:NAN);
	else 	mglprintf(out,1024,L"gr->Stem(%s, %s, %s, \"%s\");", a[0].s, a[1].s, a[2].s, k[3]==2?a[3].s:"");
}
//-----------------------------------------------------------------------------
int mgls_step(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return 1;
	else if(k[1]!=1)	gr->Step(*(a[0].d),k[1]==2?a[1].s:0,k[2]==3 ? a[2].v : NAN);
	else if(k[2]!=1)	gr->Step(*(a[0].d),*(a[1].d),k[2]==2?a[2].s:0,k[3]==3 ? a[3].v : NAN);
	else 				gr->Step(*(a[0].d),*(a[1].d),*(a[2].d),k[3]==2?a[3].s:0);
	return 0;
}
void mglc_step(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return;
	else if(k[1]!=1)
		mglprintf(out,1024,L"gr->Step(%s, \"%s\", %g);",a[0].s, k[1]==2?a[1].s:"", k[2]==3?a[2].v:NAN);
	else if(k[2]!=1)
		mglprintf(out,1024,L"gr->Step(%s, %s, \"%s\", %g);", a[0].s, a[1].s, k[2]==2?a[2].s:"", k[3]==3?a[3].v:NAN);
	else 	mglprintf(out,1024,L"gr->Step(%s, %s, %s, \"%s\");", a[0].s, a[1].s, a[2].s, k[3]==2?a[3].s:"");
}
//-----------------------------------------------------------------------------
int mgls_plot(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return 1;
	else if(k[1]!=1)	gr->Plot(*(a[0].d),k[1]==2?a[1].s:0,k[2]==3 ? a[2].v : NAN);
	else if(k[2]!=1)	gr->Plot(*(a[0].d),*(a[1].d),k[2]==2?a[2].s:0,k[3]==3 ? a[3].v : NAN);
	else 				gr->Plot(*(a[0].d),*(a[1].d),*(a[2].d),k[3]==2?a[3].s:0);
	return 0;
}
void mglc_plot(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return;
	else if(k[1]!=1)
		mglprintf(out,1024,L"gr->Plot(%s, \"%s\", %g);",a[0].s, k[1]==2?a[1].s:"", k[2]==3?a[2].v:NAN);
	else if(k[2]!=1)
		mglprintf(out,1024,L"gr->Plot(%s, %s, \"%s\", %g);", a[0].s, a[1].s, k[2]==2?a[2].s:"", k[3]==3?a[3].v:NAN);
	else 	mglprintf(out,1024,L"gr->Plot(%s, %s, %s, \"%s\");", a[0].s, a[1].s, a[2].s, k[3]==2?a[3].s:"");
}
//-----------------------------------------------------------------------------
int mgls_boxplot(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return 1;
	else if(k[1]!=1)	gr->BoxPlot(*(a[0].d),k[1]==2?a[1].s:0,k[2]==3 ? a[2].v : NAN);
	else if(k[2]!=1)	gr->BoxPlot(*(a[0].d),*(a[1].d),k[2]==2?a[2].s:0,k[3]==3 ? a[3].v : NAN);
	return 0;
}
void mglc_boxplot(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return;
	else if(k[1]!=1)
		mglprintf(out,1024,L"gr->BoxPlot(%s, \"%s\", %g);",a[0].s, k[1]==2?a[1].s:"", k[2]==3?a[2].v:NAN);
	else if(k[2]!=1)
		mglprintf(out,1024,L"gr->BoxPlot(%s, %s, \"%s\", %g);", a[0].s, a[1].s, k[2]==2?a[2].s:"", k[3]==3?a[3].v:NAN);
}
//-----------------------------------------------------------------------------
int mgls_radar(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==1)	gr->Radar(*(a[0].d), k[1]==2?a[1].s:0, k[2]==3?a[2].v:-1);
	else return 1;
	return 0;
}
void mglc_radar(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1)
		mglprintf(out,1024,L"gr->Radar(%s, \"%s\", %g);",a[0].s, k[1]==2?a[1].s:"", k[2]==3?a[2].v:-1);
}
//-----------------------------------------------------------------------------
int mgls_squeeze(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==3)
		a[0].d->Squeeze(iint(a[1].v), k[2]==3?iint(a[2].v):1, k[3]==3?iint(a[3].v):1, k[4]==3 && a[4].v>0);
	else	return 1;
	return 0;
}
void mglc_squeeze(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==3)
		mglprintf(out,1024,L"%s.Squeeze(%d, %d, %d, %s);", a[0].s, iint(a[1].v),
			k[2]==3?iint(a[2].v):1, k[3]==3?iint(a[3].v):1, k[4]==3 && a[4].v>0 ? "true":"false");
}
//-----------------------------------------------------------------------------
int mgls_stfad(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==3)
		*(a[0].d) = mglSTFA(*(a[1].d),*(a[2].d), iint(a[3].v), k[4]==2?a[4].s[0]:'x');
	else	return 1;
	return 0;
}
void mglc_stfad(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==3)
		mglprintf(out,1024,L"%s = mglSTFA(%s, %s, %d, '%c');", a[0].s, a[1].s, a[2].s, iint(a[3].v), k[4]==2?a[4].s[0]:'x');
}
//-----------------------------------------------------------------------------
int mgls_setsize(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==3 && k[1]==3 && a[1].v>1 && a[0].v>1)
		gr->SetSize(iint(a[0].v), iint(a[1].v));
	else	return 1;
	return 0;
}
void mglc_setsize(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==3 && k[1]==3 && a[1].v>1 && a[0].v>1)
		mglprintf(out,1024,L"gr->SetSize(%d, %d);", iint(a[0].v), iint(a[1].v));
}
//-----------------------------------------------------------------------------
int mgls_sphere(mglGraph *gr, long , mglArg *a, int k[10])
{
	int i;
	for(i=0;i<7;i++)	if(k[i]!=3)	break;
	if(i==4)
		gr->Sphere(mglPoint(a[0].v,a[1].v,a[2].v), a[3].v, k[4]==2 ? a[4].s:"r");
	else if(i==3)
		gr->Sphere(mglPoint(a[0].v,a[1].v), a[2].v, k[3]==2 ? a[3].s:"r");
	else	return 1;
	return 0;
}
void mglc_sphere(wchar_t out[1024], long , mglArg *a, int k[10])
{
	int i;
	for(i=0;i<7;i++)	if(k[i]!=3)	break;
	if(i==4)
		mglprintf(out,1024,L"gr->Sphere(mglPoint(%g, %g, %g), %g, \"%s\");", a[0].v,a[1].v,a[2].v, a[3].v, k[4]==2 ? a[4].s:"r");
	else if(i==3)
		mglprintf(out,1024,L"gr->Sphere(mglPoint(%g, %g), %g, \"%s\");", a[0].v,a[1].v, a[2].v, k[3]==2 ? a[3].s:"r");
}
//-----------------------------------------------------------------------------
int mgls_stfa(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==3)
		gr->STFA(*(a[0].d),*(a[1].d), iint(a[2].v), k[3]==2?a[3].s:"");
	else if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1 && k[4]==3)
		gr->STFA(*(a[0].d),*(a[1].d), *(a[2].d),*(a[3].d), iint(a[4].v), k[5]==2?a[5].s:"");
	else	return 1;
	return 0;
}
void mglc_stfa(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==3)
		mglprintf(out,1024,L"gr->STFA(%s, %s, %d, \"%s\");", a[0].s, a[1].s, iint(a[2].v), k[3]==2?a[3].s:"");
	else if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1 && k[4]==3)
		mglprintf(out,1024,L"gr->STFA(%s, %s, %s, %s, %d, \"%s\");", a[0].s, a[1].s,  a[2].s, a[3].s, iint(a[4].v), k[5]==2?a[5].s:"");
}
//-----------------------------------------------------------------------------
int mgls_surf3(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return 1;
	else if(k[1]==0)	gr->Surf3(*(a[0].d));
	else if(k[1]==1 && k[2]==1 && k[3]==1 && k[4]==3)
		gr->Surf3(a[4].v,*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),
				k[5]==2? a[5].s:0);
	else if(k[1]==3)	gr->Surf3(a[1].v,*(a[0].d),k[2]==2? a[2].s:0);
	else if(k[1]==2)	gr->Surf3(*(a[0].d),a[1].s,iint(k[2]==3?a[2].v:3));
	else if(k[1]==1 && k[2]==1 && k[3]==1)
		gr->Surf3(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),
				k[4]==2? a[4].s:0,iint(k[5]==3?a[5].v:3));
	else	return 1;
	return 0;
}
void mglc_surf3(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return;
	else if(k[1]==0)	mglprintf(out,1024,L"gr->Surf3(%s);", a[0].s);
	else if(k[1]==1 && k[2]==1 && k[3]==1 && k[4]==3)
		mglprintf(out,1024,L"gr->Surf3(%g, %s, %s, %s, %s, \"%s\");", a[4].v, a[0].s, a[1].s, a[2].s, a[3].s, k[5]==2?a[5].s:"");
	else if(k[1]==3)
		mglprintf(out,1024,L"gr->Surf3(%g, %s, \"%s\");", a[1].v, a[0].s, k[2]==2? a[2].s:"");
	else if(k[1]==2)
		mglprintf(out,1024,L"gr->Surf3(%s, \"%s\", %d);",  a[0].s, a[1].s, iint(k[2]==3?a[2].v:3));
	else if(k[1]==1 && k[2]==1 && k[3]==1)
		mglprintf(out,1024,L"gr->Surf3(%s, %s, %s, %s, \"%s\", %d);",  a[0].s, a[1].s, a[2].s, a[3].s, k[4]==2?a[4].s:"", iint(k[5]==3?a[5].v:3));
}
//-----------------------------------------------------------------------------
int mgls_surf3c(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]!=1 || k[1]!=1)	return 1;
	else if(k[2]==0)	gr->Surf3C(*(a[0].d),*(a[1].d));
	else if(k[2]==1 && k[3]==1 && k[4]==1 && k[5]==3)
		gr->Surf3C(a[5].v,*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),
				k[6]==2? a[6].s:0);
	else if(k[2]==3)	gr->Surf3C(a[2].v,*(a[0].d),*(a[1].d),k[3]==2? a[3].s:0);
	else if(k[2]==2)	gr->Surf3C(*(a[0].d),*(a[1].d),a[2].s,iint(k[3]==3?a[3].v:3));
	else if(k[2]==1 && k[3]==1 && k[4]==1)
		gr->Surf3C(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),
				k[5]==2? a[5].s:0,iint(k[6]==3?a[6].v:3));
	else	return 1;
	return 0;
}
void mglc_surf3c(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]!=1 || k[1]!=1)	return;
	else if(k[2]==0)	mglprintf(out,1024,L"gr->Surf3C(%s, %s);",  a[0].s, a[1].s);
	else if(k[2]==1 && k[3]==1 && k[4]==1 && k[5]==3)
		mglprintf(out,1024,L"gr->Surf3C(%g, %s, %s, %s, %s, %s, \"%s\");", a[5].v, a[0].s, a[1].s, a[2].s, a[3].s, a[4].s, k[6]==2? a[6].s:"");
	else if(k[2]==3)
		mglprintf(out,1024,L"gr->Surf3C(%g, %s, %s, \"%s\");", a[2].v, a[0].s, a[1].s, k[3]==2?a[3].s:0);
	else if(k[2]==2)
		mglprintf(out,1024,L"gr->Surf3C(%s, %s, \"%s\", %d);",  a[0].s, a[1].s, a[2].s, iint(k[3]==3?a[3].v:3));
	else if(k[2]==1 && k[3]==1 && k[4]==1)
		mglprintf(out,1024,L"gr->Surf3C(%s, %s, %s, %s, %s, \"%s\", %d);",  a[0].s, a[1].s, a[2].s, a[3].s, a[4].s, k[5]==2?a[5].s:"", iint(k[6]==3?a[6].v:3));
}
//-----------------------------------------------------------------------------
int mgls_surf3a(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]!=1 || k[1]!=1)	return 1;
	else if(k[2]==0)	gr->Surf3A(*(a[0].d),*(a[1].d));
	else if(k[2]==1 && k[3]==1 && k[4]==1 && k[5]==3)
		gr->Surf3A(a[5].v,*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),
				k[6]==2? a[6].s:0);
	else if(k[2]==3)	gr->Surf3A(a[2].v,*(a[0].d),*(a[1].d),k[3]==2? a[3].s:0);
	else if(k[2]==2)	gr->Surf3A(*(a[0].d),*(a[1].d),a[2].s,iint(k[3]==3?a[3].v:3));
	else if(k[2]==1 && k[3]==1 && k[4]==1)
		gr->Surf3A(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),
				k[5]==2? a[5].s:0,iint(k[6]==3?a[6].v:3));
	else	return 1;
	return 0;
}
void mglc_surf3a(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]!=1 || k[1]!=1)	return;
	else if(k[2]==0)	mglprintf(out,1024,L"gr->Surf3A(%s, %s);",  a[0].s, a[1].s);
	else if(k[2]==1 && k[3]==1 && k[4]==1 && k[5]==3)
		mglprintf(out,1024,L"gr->Surf3A(%g, %s, %s, %s, %s, %s, \"%s\");", a[5].v, a[0].s, a[1].s, a[2].s, a[3].s, a[4].s, k[6]==2? a[6].s:"");
	else if(k[2]==3)
		mglprintf(out,1024,L"gr->Surf3A(%g, %s, %s, \"%s\");", a[2].v, a[0].s, a[1].s, k[3]==2?a[3].s:0);
	else if(k[2]==2)
		mglprintf(out,1024,L"gr->Surf3A(%s, %s, \"%s\", %d);",  a[0].s, a[1].s, a[2].s, iint(k[3]==3?a[3].v:3));
	else if(k[2]==1 && k[3]==1 && k[4]==1)
		mglprintf(out,1024,L"gr->Surf3A(%s, %s, %s, %s, %s, \"%s\", %d);",  a[0].s, a[1].s, a[2].s, a[3].s, a[4].s, k[5]==2?a[5].s:"", iint(k[6]==3?a[6].v:3));
}
//-----------------------------------------------------------------------------
int mgls_subplot(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==3 && k[1]==3 && k[2]==3 && k[3]==2)
		gr->SubPlot(iint(a[0].v), iint(a[1].v), iint(a[2].v), a[3].s);
	else if(k[0]==3 && k[1]==3 && k[2]==3)
		gr->SubPlot(iint(a[0].v), iint(a[1].v), iint(a[2].v), k[3]==3?a[3].v:0, k[4]==3?a[4].v:0);
	else	return 1;
	return 0;
}
void mglc_subplot(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==3 && k[1]==3 && k[2]==3 && k[3]==2)
		mglprintf(out,1024,L"gr->SubPlot(%d, %d, %d, \"%s\");", iint(a[0].v), iint(a[1].v), iint(a[2].v), a[3].s);
	else if(k[0]==3 && k[1]==3 && k[2]==3)
		mglprintf(out,1024,L"gr->SubPlot(%d, %d, %d, %g, %g);", iint(a[0].v), iint(a[1].v), iint(a[2].v), k[3]==3?a[3].v:0, k[4]==3?a[4].v:0);
}
//-----------------------------------------------------------------------------
int mgls_subdata(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==3)
		*(a[0].d) = a[1].d->SubData(iint(a[2].v), k[3]==3?iint(a[3].v):-1, k[4]==3?iint(a[4].v):-1);
	else	return 1;
	return 0;
}
void mglc_subdata(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==3)
		mglprintf(out,1024,L"%s = %s.SubData(%d, %d, %d);",  a[0].s, a[1].s, iint(a[2].v), k[3]==3?iint(a[3].v):-1, k[4]==3?iint(a[4].v):-1);
}
//-----------------------------------------------------------------------------
int mgls_trace(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1)	*(a[0].d) = a[1].d->Trace();
	else	return 1;
	return 0;
}
void mglc_trace(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1)	mglprintf(out,1024,L"%s = %s.Trace();",  a[0].s, a[1].s);
}
//-----------------------------------------------------------------------------
int mgls_tile(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return 1;
	else if(k[1]!=1)	gr->Tile(*(a[0].d), k[1]==2? a[1].s:0);
	else if(k[1]==1 && k[2]==1)
		gr->Tile(*(a[0].d),*(a[1].d),*(a[2].d),k[3]==2? a[3].s:0);
	else	return 1;
	return 0;
}
void mglc_tile(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return;
	else if(k[1]!=1)
		mglprintf(out,1024,L"gr->Tile(%s, \"%s\");", a[0].s, k[1]==2?a[1].s:"");
	else if(k[1]==1 && k[2]==1)
		mglprintf(out,1024,L"gr->Tile(%s, %s, %s, \"%s\");", a[0].s, a[1].s, a[2].s, k[3]==2?a[3].s:"");
}
//-----------------------------------------------------------------------------
int mgls_tiles(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return 1;
	else if(k[1]==1 && k[2]!=1)
		gr->TileS(*(a[0].d),*(a[1].d),k[2]==2?a[2].s:0);
	else if(k[1]==1 && k[2]==1 && k[3]==1)
		gr->TileS(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),k[4]==2? a[4].s:0);
	else	return 1;
	return 0;
}
void mglc_tiles(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]!=1)	return;
	else if(k[1]==1 && k[2]!=1)
		mglprintf(out,1024,L"gr->TileS(%s, %s, \"%s\");", a[0].s, a[1].s, k[2]==2?a[2].s:"");
	else if(k[1]==1 && k[2]==1 && k[3]==1)
		mglprintf(out,1024,L"gr->TileS(%s, %s, %s, %s, \"%s\");", a[0].s, a[1].s, a[2].s, a[3].s, k[4]==2?a[4].s:"");
}
//-----------------------------------------------------------------------------
int mgls_text(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==3 && k[1]==3 && k[2]==3 && k[3]==2)
		gr->Putsw(mglPoint(a[0].v,a[1].v,a[2].v),a[3].w,
			(k[4]==2 && a[4].s[0]!=0)?a[4].s:gr->FontDef,k[5]==3?a[5].v:-1.4);
	else if(k[0]==3 && k[1]==3 && k[2]==2)
		gr->Putsw(mglPoint(a[0].v,a[1].v),a[2].w,
			(k[3]==2 && a[3].s[0]!=0)?a[3].s:gr->FontDef,k[4]==3?a[4].v:-1.4);
	else if(k[0]==3 && k[1]==3 && k[2]==3 && k[3]==3 && k[4]==3 && k[5]==3 && k[6]==2)
		gr->Putsw(mglPoint(a[0].v,a[1].v,a[2].v), mglPoint(a[3].v,a[4].v,a[5].v), a[6].w, 't', k[8]==3?a[8].v:-1.4);
	else if(k[0]==3 && k[1]==3 && k[2]==3 && k[3]==3 && k[4]==2)
		gr->Putsw(mglPoint(a[0].v,a[1].v), mglPoint(a[2].v,a[3].v), a[4].w, 't', k[6]==3?a[6].v:-1.4);
	else if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==2)
		gr->Text(*(a[0].d),*(a[1].d),*(a[2].d),a[3].w,k[4]==2?a[4].s:0,k[5]==3?a[5].v:-1);
	else if(k[0]==1 && k[1]==1 && k[2]==2)
		gr->Text(*(a[0].d),*(a[1].d),a[2].w,k[3]==2?a[3].s:0,
			k[4]==3?a[4].v:-1,k[5]==3?a[5].v:NAN);
	else if(k[0]==1 && k[1]==2)
		gr->Text(*(a[0].d),a[1].w,k[2]==2?a[2].s:0,k[3]==3?a[3].v:-1,k[4]==3?a[4].v:NAN);
	else	return 1;
	return 0;
}
void mglc_text(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==3 && k[1]==3 && k[2]==3 && k[3]==2)
		mglprintf(out,1024,L"gr->Puts(mglPoint(%g, %g, %g), L\"%ls\", \"%s\", %g);", a[0].v, a[1].v, a[2].v, a[3].w, (k[4]==2 && a[4].s[0]!=0)?a[4].s:"C", k[5]==3?a[5].v:-1.4);
	else if(k[0]==3 && k[1]==3 && k[2]==2)
		mglprintf(out,1024,L"gr->Puts(mglPoint(%g, %g), L\"%ls\", \"%s\", %g);", a[0].v, a[1].v, a[2].w, (k[3]==2 && a[3].s[0]!=0)?a[3].s:"C", k[4]==3?a[4].v:-1.4);
	else if(k[0]==3 && k[1]==3 && k[2]==3 && k[3]==3 && k[4]==3 && k[5]==3 && k[6]==2)
		mglprintf(out,1024,L"gr->Puts(mglPoint(%g, %g, %g), mglPoint(%g, %g, %g), L\"%ls\", 't', %g);", a[0].v, a[1].v, a[2].v, a[3].v, a[4].v, a[5].v, a[6].w, k[8]==3?a[8].v:-1.4);
	else if(k[0]==3 && k[1]==3 && k[2]==3 && k[3]==3 && k[4]==2)
		mglprintf(out,1024,L"gr->Puts(mglPoint(%g, %g), mglPoint(%g, %g), L\"%ls\", 't', %g);", a[0].v, a[1].v, a[2].v, a[3].v, a[4].w, k[6]==3?a[6].v:-1.4);
	else if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==2)
		mglprintf(out,1024,L"gr->Text(%s, %s, %s, L\"%ls\", \"%s\", %g);",  a[0].s, a[1].s, a[2].s, a[3].w, k[4]==2?a[4].s:"", k[5]==3?a[5].v:-1);
	else if(k[0]==1 && k[1]==1 && k[2]==2)
		mglprintf(out,1024,L"gr->Text(%s, %s, L\"%ls\", \"%s\", %g, %g);",  a[0].s, a[1].s, a[2].w, k[3]==2?a[3].s:"", k[4]==3?a[4].v:-1, k[5]==3?a[5].v:NAN);
	else if(k[0]==1 && k[1]==2)
		mglprintf(out,1024,L"gr->Text(%s, L\"%ls\", \"%s\", %g, %g);",  a[0].s, a[1].w, k[2]==2?a[2].s:"", k[3]==3?a[3].v:-1, k[4]==3?a[4].v:NAN);
}
//-----------------------------------------------------------------------------
int mgls_torus(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1)	gr->Torus(*(a[0].d),*(a[1].d),k[2]==2?a[2].s:0);
	else if(k[0]==1)		gr->Torus(*(a[0].d),k[1]==2?a[1].s:0);
	else	return 1;
	return 0;
}
void mglc_torus(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1)	mglprintf(out,1024,L"gr->Torus(%s, %s, \"%s\");", a[0].s, a[1].s, k[2]==2?a[2].s:"");
	else if(k[0]==1)		mglprintf(out,1024,L"gr->Torus(%s, \"%s\");", a[0].s, k[1]==2?a[1].s:"");
}
//-----------------------------------------------------------------------------
int mgls_transparent(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==3)	gr->Transparent = (a[0].v!=0);
	else	return 1;
	return 0;
}
void mglc_transparent(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==3)	mglprintf(out,1024,L"gr->Transparent = %s;", (a[0].v!=0)?"true":"false");
}
//-----------------------------------------------------------------------------
int mgls_transptype(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==3)	gr->TranspType = iint(a[0].v);
	else	return 1;
	return 0;
}
void mglc_transptype(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==3)	mglprintf(out,1024,L"gr->TranspType = %d;", iint(a[0].v));
}
//-----------------------------------------------------------------------------
int mgls_transform(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2 && k[2]==1 && k[3]==1)
		*(a[0].d) = mglTransform(*(a[2].d),*(a[3].d),a[1].s);
	else	return 1;
	return 0;
}
void mglc_transform(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2 && k[2]==1 && k[3]==1)
		mglprintf(out,1024,L"%s = mglTransform(%s, %s, \"%s\");",a[0].s,  a[2].s, a[3].s, a[1].s);
}
//-----------------------------------------------------------------------------
int mgls_transforma(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2 && k[2]==1 && k[3]==1)
		*(a[0].d) = mglTransformA(*(a[2].d),*(a[3].d),a[1].s);
	else	return 1;
	return 0;
}
void mglc_transforma(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2 && k[2]==1 && k[3]==1)
		mglprintf(out,1024,L"%s = mglTransformA(%s, %s, \"%s\");",a[0].s,  a[2].s, a[3].s, a[1].s);
}
//-----------------------------------------------------------------------------
int mgls_tube(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1)
		gr->Tube(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),k[4]==2?a[4].s:0);
	else if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==3)
		gr->Tube(*(a[0].d),*(a[1].d),*(a[2].d),a[3].v,k[4]==2?a[4].s:0);
	else if(k[0]==1 && k[1]==1 && k[2]==1)
		gr->Tube(*(a[0].d),*(a[1].d),*(a[2].d),k[3]==2?a[3].s:0,k[4]==3?a[4].v:NAN);
	else if(k[0]==1 && k[1]==1 && k[2]==3)
		gr->Tube(*(a[0].d),*(a[1].d),a[2].v,k[3]==2?a[3].s:0,k[4]==3?a[4].v:NAN);
	else if(k[0]==1 && k[1]==1)
		gr->Tube(*(a[0].d),*(a[1].d),k[2]==2?a[2].s:0,k[3]==3?a[3].v:NAN);
	else if(k[0]==1 && k[1]==3)
		gr->Tube(*(a[0].d),a[1].v,k[2]==2?a[2].s:0,k[3]==3?a[3].v:NAN);
	else	return 1;
	return 0;
}
void mglc_tube(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1)
		mglprintf(out,1024,L"gr->Tube(%s, %s, %s, %s, \"%s\");", a[0].s, a[1].s, a[2].s, a[3].s, k[4]==2?a[4].s:"");
	else if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==3)
		mglprintf(out,1024,L"gr->Tube(%s, %s, %s, %g, \"%s\");", a[0].s, a[1].s, a[2].s, a[3].v, k[4]==2?a[4].s:"");
	else if(k[0]==1 && k[1]==1 && k[2]==1)
		mglprintf(out,1024,L"gr->Tube(%s, %s, %s, \"%s\", %g);", a[0].s, a[1].s, a[2].s, k[3]==2?a[3].s:"", k[4]==3?a[4].v:NAN);
	else if(k[0]==1 && k[1]==1 && k[2]==3)
		mglprintf(out,1024,L"gr->Tube(%s, %s, %g, \"%s\", %g);", a[0].s, a[1].s, a[2].v, k[3]==2?a[3].s:"", k[4]==3?a[4].v:NAN);
	else if(k[0]==1 && k[1]==1)
		mglprintf(out,1024,L"gr->Tube(%s, %s, \"%s\", %g);", a[0].s, a[1].s, k[2]==2?a[2].s:"", k[3]==3?a[3].v:NAN);
	else if(k[0]==1 && k[1]==3)
		mglprintf(out,1024,L"gr->Tube(%s, %g, \"%s\", %g);", a[0].s, a[1].v, k[2]==2?a[2].s:"", k[3]==3?a[3].v:NAN);
}
//-----------------------------------------------------------------------------
int mgls_textmark(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1 && k[4]==2)
		gr->TextMark(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),a[4].w,k[5]==2?a[5].s:0);
	else if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==2)
		gr->TextMark(*(a[0].d),*(a[1].d),*(a[2].d),a[3].w,k[4]==2?a[4].s:0,k[5]==3?a[5].v:NAN);
	else if(k[0]==1 && k[1]==1 && k[2]==2)
		gr->TextMark(*(a[0].d),*(a[1].d),a[2].w,k[3]==2?a[3].s:0,k[4]==3?a[4].v:NAN);
	else if(k[0]==1 && k[1]==2)
		gr->TextMark(*(a[0].d),a[1].w,k[2]==2?a[2].s:0,k[3]==3?a[3].v:NAN);
	else	return 1;
	return 0;
}
void mglc_textmark(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1 && k[4]==2)
		mglprintf(out,1024,L"gr->TextMark(%s, %s, %s, %s, L\"%ls\", \"%s\");", a[0].s, a[1].s, a[2].s, a[3].s, a[4].w, k[5]==2?a[5].s:"");
	else if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==2)
		mglprintf(out,1024,L"gr->TextMark(%s, %s, %s, L\"%ls\", \"%s\", %g);", a[0].s, a[1].s, a[2].s, a[3].w, k[4]==2?a[4].s:"", k[5]==3?a[5].v:NAN);
	else if(k[0]==1 && k[1]==1 && k[2]==2)
		mglprintf(out,1024,L"gr->TextMark(%s, %s, L\"%ls\", \"%s\", %g);", a[0].s, a[1].s, a[2].w, k[3]==2?a[3].s:"", k[4]==3?a[4].v:NAN);
	else if(k[0]==1 && k[1]==2)
		mglprintf(out,1024,L"gr->TextMark(%s, L\"%ls\", \"%s\", %g);", a[0].s, a[1].w, k[2]==2?a[2].s:"", k[3]==3?a[3].v:NAN);
}
//-----------------------------------------------------------------------------
int mgls_triplot(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1 && k[4]==1)
		gr->TriPlot(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),k[5]==2?a[5].s:0);
	else if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1)
		gr->TriPlot(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),k[4]==2?a[4].s:0);
	else if(k[0]==1 && k[1]==1 && k[2]==1)
		gr->TriPlot(*(a[0].d),*(a[1].d),*(a[2].d),k[3]==2?a[3].s:0,k[4]==3?a[4].v:NAN);
	else	return 1;
	return 0;
}
void mglc_triplot(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1 && k[4]==1)
		mglprintf(out,1024,L"gr->TriPlot(%s, %s, %s, %s, %s, \"%s\");", a[0].s, a[1].s, a[2].s, a[3].s, a[4].s, k[5]==2?a[5].s:"");
	else if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1)
		mglprintf(out,1024,L"gr->TriPlot(%s, %s, %s, %s, \"%s\");", a[0].s, a[1].s, a[2].s, a[3].s, k[4]==2?a[4].s:"");
	else if(k[0]==1 && k[1]==1 && k[2]==1)
		mglprintf(out,1024,L"gr->TriPlot(%s, %s, %s, \"%s\", %g);", a[0].s, a[1].s, a[2].s, k[3]==2?a[3].s:"", k[4]==3?a[4].v:NAN);
}
//-----------------------------------------------------------------------------
int mgls_quadplot(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1 && k[4]==1)
		gr->QuadPlot(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),k[5]==2?a[5].s:0);
	else if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1)
		gr->QuadPlot(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),k[4]==2?a[4].s:0);
	else if(k[0]==1 && k[1]==1 && k[2]==1)
		gr->QuadPlot(*(a[0].d),*(a[1].d),*(a[2].d),k[3]==2?a[3].s:0,k[4]==3?a[4].v:NAN);
	else	return 1;
	return 0;
}
void mglc_quadplot(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1 && k[4]==1)
		mglprintf(out,1024,L"gr->QuadPlot(%s, %s, %s, %s, %s, \"%s\");", a[0].s, a[1].s, a[2].s, a[3].s, a[4].s, k[5]==2?a[5].s:"");
	else if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1)
		mglprintf(out,1024,L"gr->QuadPlot(%s, %s, %s, %s, \"%s\");", a[0].s, a[1].s, a[2].s, a[3].s, k[4]==2?a[4].s:"");
	else if(k[0]==1 && k[1]==1 && k[2]==1)
		mglprintf(out,1024,L"gr->QuadPlot(%s, %s, %s, \"%s\", %g);", a[0].s, a[1].s, a[2].s, k[3]==2?a[3].s:"", k[4]==3?a[4].v:NAN);
}
//-----------------------------------------------------------------------------
int mgls_tricont(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1 && k[4]==1 && k[5]==1)
		gr->TriContV(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),*(a[5].d),k[6]==2?a[6].s:0,k[7]==3?a[7].v:NAN);
	else if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1 && k[4]==1)
		gr->TriContV(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),k[5]==2?a[5].s:0,k[6]==3?a[6].v:NAN);
	else if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1)
		gr->TriCont(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),k[4]==2?a[4].s:0,7,k[5]==3?a[5].v:NAN);
	else	return 1;
	return 0;
}
void mglc_tricont(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1 && k[4]==1 && k[5]==1)
		mglprintf(out,1024,L"gr->TriContV(%s, %s, %s, %s, %s, %s, \"%s\", %g);",
			a[0].s, a[1].s, a[2].s, a[3].s, a[4].s, a[5].s, k[6]==2?a[6].s:"",k[7]==3?a[7].v:NAN);
	else if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1 && k[4]==1)
		mglprintf(out,1024,L"gr->TriContV(%s, %s, %s, %s, %s, \"%s\", %g);",
			a[0].s, a[1].s, a[2].s, a[3].s, a[4].s, k[5]==2?a[5].s:"",k[6]==3?a[6].v:NAN);
	else if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1)
		mglprintf(out,1024,L"gr->TriCont(%s, %s, %s, %s, \"%s\", 7, %g);",
			a[0].s, a[1].s, a[2].s, a[3].s, k[4]==2?a[4].s:"", k[5]==3?a[5].v:NAN);
}
//-----------------------------------------------------------------------------
int mgls_ternary(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==3)	gr->Ternary(int(a[0].v));
	else	return 1;
	return 0;
}
void mglc_ternary(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==3)	mglprintf(out,1024,L"gr->Ternary(%d);", int(a[0].v));
}
//-----------------------------------------------------------------------------
int mgls_transpose(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1)	a[0].d->Transpose(k[1]==2?a[1].s:"yxz");
	else	return 1;
	return 0;
}
void mglc_transpose(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1)	mglprintf(out,1024,L"%s.Transpose(\"%s\");", a[0].s, k[1]==2?a[1].s:"yxz");
}
//-----------------------------------------------------------------------------
int mgls_vectc(mglGraph *gr, long , mglArg *a, int k[10])
{
	int i;
	for(i=0;i<7;i++)	if(k[i]!=1)	break;
	if(i==2)
		gr->VectC(*(a[0].d),*(a[1].d),k[2]==2?a[2].s:0,k[3]==3?a[3].v:NAN);
	else if(i==4)
		gr->VectC(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),k[4]==2?a[4].s:0,k[5]==3?a[5].v:NAN);
	else if(i==3)
		gr->VectC(*(a[0].d),*(a[1].d),*(a[2].d),k[3]==2?a[3].s:0);
	else if(i==6)
		gr->VectC(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),*(a[5].d),k[6]==2?a[6].s:0);
	else	return 1;
	return 0;
}
void mglc_vectc(wchar_t out[1024], long , mglArg *a, int k[10])
{
	int i;
	for(i=0;i<7;i++)	if(k[i]!=1)	break;
	if(i==2)
		mglprintf(out,1024,L"gr->VectC(%s, %s, \"%s\", %g);", a[0].s, a[1].s, k[2]==2?a[2].s:"", k[3]==3?a[3].v:NAN);
	else if(i==4)
		mglprintf(out,1024,L"gr->VectC(%s, %s, %s, %s, \"%s\", %g);", a[0].s, a[1].s, a[2].s, a[3].s, k[4]==2?a[4].s:"", k[5]==3?a[5].v:NAN);
	else if(i==3)
		mglprintf(out,1024,L"gr->VectC(%s, %s, %s, \"%s\");", a[0].s, a[1].s, a[2].s, k[3]==2?a[3].s:0);
	else if(i==6)
		mglprintf(out,1024,L"gr->VectC(%s, %s, %s, %s, %s, %s, \"%s\");", a[0].s, a[1].s, a[2].s, a[3].s, a[4].s, a[5].s, k[6]==2?a[6].s:"");
}
//-----------------------------------------------------------------------------
int mgls_vect(mglGraph *gr, long , mglArg *a, int k[10])
{
	int i;
	for(i=0;i<7;i++)	if(k[i]!=1)	break;
	if(i==2)
		gr->Vect(*(a[0].d),*(a[1].d),k[2]==2?a[2].s:0, k[3]==3?a[3].v:NAN, k[4]==3?iint(a[4].v):0);
	else if(i==4)
		gr->Vect(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),k[4]==2?a[4].s:0, k[5]==3?a[5].v:NAN, k[6]==3?iint(a[6].v):0);
	else if(i==3)
		gr->Vect(*(a[0].d),*(a[1].d),*(a[2].d),k[3]==2?a[3].s:0, k[4]==3?iint(a[4].v):0);
	else if(i==6)
		gr->Vect(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),*(a[5].d),k[6]==2?a[6].s:0, k[7]==3?iint(a[7].v):0);
	else	return 1;
	return 0;
}
void mglc_vect(wchar_t out[1024], long , mglArg *a, int k[10])
{
	int i;
	for(i=0;i<7;i++)	if(k[i]!=1)	break;
	if(i==2)
		mglprintf(out,1024,L"gr->Vect(%s, %s, \"%s\", %g, %d);", a[0].s, a[1].s, k[2]==2?a[2].s:"", k[3]==3?a[3].v:NAN,k[4]==3?iint(a[4].v):0);
	else if(i==4)
		mglprintf(out,1024,L"gr->Vect(%s, %s, %s, %s, \"%s\", %g, %d);", a[0].s, a[1].s, a[2].s, a[3].s, k[4]==2?a[4].s:"", k[5]==3?a[5].v:NAN,k[6]==3?iint(a[6].v):0);
	else if(i==3)
		mglprintf(out,1024,L"gr->Vect(%s, %s, %s, \"%s\", %d);", a[0].s, a[1].s, a[2].s, k[3]==2?a[3].s:0,k[4]==3?iint(a[4].v):0);
	else if(i==6)
		mglprintf(out,1024,L"gr->Vect(%s, %s, %s, %s, %s, %s, \"%s\", %d);", a[0].s, a[1].s, a[2].s, a[3].s, a[4].s, a[5].s, k[6]==2?a[6].s:"",k[7]==3?iint(a[7].v):0);
}
//-----------------------------------------------------------------------------
int mgls_vectl(mglGraph *gr, long , mglArg *a, int k[10])
{
	int i;
	for(i=0;i<7;i++)	if(k[i]!=1)	break;
	if(i==2)
		gr->VectL(*(a[0].d),*(a[1].d),k[2]==2?a[2].s:0,k[3]==3?a[3].v:NAN);
	else if(i==4)
		gr->VectL(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),k[4]==2?a[4].s:0,k[5]==3?a[5].v:NAN);
	else if(i==3)
		gr->VectL(*(a[0].d),*(a[1].d),*(a[2].d),k[3]==2?a[3].s:0);
	else if(i==6)
		gr->VectL(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),*(a[5].d),k[6]==2?a[6].s:0);
	else	return 1;
	return 0;
}
void mglc_vectl(wchar_t out[1024], long , mglArg *a, int k[10])
{
	int i;
	for(i=0;i<7;i++)	if(k[i]!=1)	break;
	if(i==2)
		mglprintf(out,1024,L"gr->VectL(%s, %s, \"%s\", %g);", a[0].s, a[1].s, k[2]==2?a[2].s:"", k[3]==3?a[3].v:NAN);
	else if(i==4)
		mglprintf(out,1024,L"gr->VectL(%s, %s, %s, %s, \"%s\", %g);", a[0].s, a[1].s, a[2].s, a[3].s, k[4]==2?a[4].s:"", k[5]==3?a[5].v:NAN);
	else if(i==3)
		mglprintf(out,1024,L"gr->VectL(%s, %s, %s, \"%s\");", a[0].s, a[1].s, a[2].s, k[3]==2?a[3].s:0);
	else if(i==6)
		mglprintf(out,1024,L"gr->VectL(%s, %s, %s, %s, %s, %s, \"%s\");", a[0].s, a[1].s, a[2].s, a[3].s, a[4].s, a[5].s, k[6]==2?a[6].s:"");
}
//-----------------------------------------------------------------------------
int mgls_traj(mglGraph *gr, long , mglArg *a, int k[10])
{
	int i;
	for(i=0;i<7;i++)	if(k[i]!=1)	break;
	if(i==4)
		gr->Traj(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),k[4]==2?a[4].s:0, k[5]==3?a[5].v:NAN, k[6]==3?a[6].v:0);
	else if(i==6)
		gr->Traj(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),*(a[5].d),k[6]==2?a[6].s:0, k[7]==3?a[7].v:0);
	else	return 1;
	return 0;
}
void mglc_traj(wchar_t out[1024], long , mglArg *a, int k[10])
{
	int i;
	for(i=0;i<7;i++)	if(k[i]!=1)	break;
	if(i==4)
		mglprintf(out,1024,L"gr->Traj(%s, %s, %s, %s, \"%s\", %g, %g);", a[0].s, a[1].s, a[2].s, a[3].s, k[4]==2?a[4].s:"", k[5]==3?a[5].v:NAN,k[6]==3?a[6].v:0);
	else if(i==6)
		mglprintf(out,1024,L"gr->Traj(%s, %s, %s, %s, %s, %s, \"%s\", %g);", a[0].s, a[1].s, a[2].s, a[3].s, a[4].s, a[5].s, k[6]==2?a[6].s:"",k[7]==3?a[7].v:0);
}
//-----------------------------------------------------------------------------
int mgls_xlabel(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==2)	gr->Labelw('x', a[0].w, k[1]==3?a[1].v:0, k[2]==3?a[2].v:-1.4, k[3]==3?a[3].v:0);
	else	return 1;
	return 0;
}
void mglc_xlabel(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==2)	mglprintf(out,1024,L"gr->Label('x', L\"%ls\", %g, %g, %g);", a[0].w, k[1]==3?a[1].v:1, k[2]==3?a[2].v:-1.4, k[3]==3?a[3].v:0);
}
//-----------------------------------------------------------------------------
int mgls_ylabel(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==2)	gr->Labelw('y', a[0].w, k[1]==3?a[1].v:0, k[2]==3?a[2].v:-1.4, k[3]==3?a[3].v:0);
	else	return 1;
	return 0;
}
void mglc_ylabel(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==2)	mglprintf(out,1024,L"gr->Label('y', L\"%ls\", %g, %g, %g);", a[0].w, k[1]==3?a[1].v:1, k[2]==3?a[2].v:-1.4, k[3]==3?a[3].v:0);
}
//-----------------------------------------------------------------------------
int mgls_zlabel(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==2)	gr->Labelw('z', a[0].w, k[1]==3?a[1].v:0, k[2]==3?a[2].v:-1.4, k[3]==3?a[3].v:0);
	else	return 1;
	return 0;
}
void mglc_zlabel(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==2)	mglprintf(out,1024,L"gr->Label('z', L\"%ls\", %g, %g, %g);", a[0].w, k[1]==3?a[1].v:1, k[2]==3?a[2].v:-1.4, k[3]==3?a[3].v:0);
}
//-----------------------------------------------------------------------------
int mgls_tlabel(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==2)	gr->Labelw('t', a[0].w, k[1]==3?a[1].v:0, k[2]==3?a[2].v:-1.4, k[3]==3?a[3].v:0);
	else	return 1;
	return 0;
}
void mglc_tlabel(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==2)	mglprintf(out,1024,L"gr->Label('t', L\"%ls\", %g, %g, %g);", a[0].w, k[1]==3?a[1].v:1, k[2]==3?a[2].v:-1.4, k[3]==3?a[3].v:0);
}
//-----------------------------------------------------------------------------
int mgls_label(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==3 && k[1]==3 && k[2]==2)
		gr->Labelw(a[0].v, a[1].v, a[2].w, k[3]==2?a[3].s:0, k[4]==3?a[2].v:-1.4);
	else	return 1;
	return 0;
}
void mglc_label(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==3 && k[1]==3 && k[2]==2)
		mglprintf(out,1024,L"gr->Label(%g, %g, \"%s\", \"%s\", %g);", a[0].v, a[1].v, a[2].w, k[3]==2?a[3].s:0, k[4]==3?a[2].v:-1.4);
}
//-----------------------------------------------------------------------------
int mgls_xrange(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==1)	gr->XRange(*(a[0].d),k[1]==3?(a[1].v!=0):false,k[2]==3?a[2].v:0);
	else if(k[0]==3 && k[1]==3)
	{	gr->Min.x = a[0].v;	gr->Max.x = a[1].v;	gr->RecalcBorder();	}
	else	return 1;
	return 0;
}
void mglc_xrange(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1)
		mglprintf(out,1024,L"gr->XRange(%s, %s, %g);", a[0].s, (k[1]==3&&a[1].v!=0)?"true":"false", k[2]==3?a[2].v:0);
	else if(k[0]==3 && k[1]==3)
		mglprintf(out,1024,L"gr->Axis(mglPoint(%g), mglPoint(%g));", a[0].v, a[1].v);
}
//-----------------------------------------------------------------------------
int mgls_yrange(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==1)	gr->YRange(*(a[0].d),k[1]==3?(a[1].v!=0):false,k[2]==3?a[2].v:0);
	else if(k[0]==3 && k[1]==3)
	{	gr->Min.y = a[0].v;	gr->Max.y = a[1].v;	gr->RecalcBorder();	}
	else	return 1;
	return 0;
}
void mglc_yrange(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1)
		mglprintf(out,1024,L"gr->YRange(%s, %s, %g);", a[0].s, (k[1]==3&&a[1].v!=0)?"true":"false", k[2]==3?a[2].v:0);
	else if(k[0]==3 && k[1]==3)
		mglprintf(out,1024,L"gr->Axis(mglPoint(0,%g), mglPoint(0,%g));", a[0].v, a[1].v);
}
//-----------------------------------------------------------------------------
int mgls_zrange(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==1)	gr->ZRange(*(a[0].d),k[1]==3?(a[1].v!=0):false,k[2]==3?a[2].v:0);
	else if(k[0]==3 && k[1]==3)
	{	gr->Min.z = a[0].v;	gr->Max.z = a[1].v;	gr->RecalcBorder();	}
	else	return 1;
	return 0;
}
void mglc_zrange(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1)
		mglprintf(out,1024,L"gr->ZRange(%s, %s, %g);", a[0].s, (k[1]==3&&a[1].v!=0)?"true":"false", k[2]==3?a[2].v:0);
	else if(k[0]==3 && k[1]==3)
		mglprintf(out,1024,L"gr->Axis(mglPoint(0,0,%g), mglPoint(0,0,%g));", a[0].v, a[1].v);
}
//-----------------------------------------------------------------------------
int mgls_xtick(mglGraph *gr, long n, mglArg *a, int k[10])
{
	if(k[0]==3 && k[1]==2)
	{
		mreal v[50];	const wchar_t *s[50];	int i;
		for(i=0;i<50 && i<n/2;i++)
		{
			if(a[2*i].type==2 && a[2*i+1].type==1)
			{	v[i] = a[2*i].v;	s[i] = a[2*i+1].w;	}
			else	break;
		}
		gr->SetTicksVal('x',i,v,s);
	}
	else if(k[0]==3)
		gr->SetTicks('x', a[0].v, k[1]==3?iint(a[1].v):0, k[2]==3?a[2].v:NAN);
	else if(k[0]==2)	gr->SetXTT(a[0].w);
	else	return 1;
	return 0;
}
void mglc_xtick(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==3 && k[1]==3 && k[2]==3)
		mglprintf(out,1024,L"gr->SetTicks('x', %g, %d, %g);", a[0].v, iint(a[1].v),a[2].v);
	else if(k[0]==3 && k[1]==3)
		mglprintf(out,1024,L"gr->SetTicks('x', %g, %d);", a[0].v, iint(a[1].v));
	else if(k[0]==3 && k[1]==2)
		mglprintf(out,1024,L"gr->SetTicksVal('x', %g, \"%s\");", a[0].v, a[1].s);
	else if(k[0]==3)	mglprintf(out,1024,L"gr->SetTicks('x', %g);", a[0].v);
	else if(k[0]==2)
		mglprintf(out,1024,L"gr->SetXTT(L\"%ls\");", a[0].w);
}
//-----------------------------------------------------------------------------
int mgls_ytick(mglGraph *gr, long n, mglArg *a, int k[10])
{
	if(k[0]==3 && k[1]==2)
	{
		mreal v[50];	const wchar_t *s[50];	int i;
		for(i=0;i<50 && i<n/2;i++)
		{
			if(a[2*i].type==2 && a[2*i+1].type==1)
			{	v[i] = a[2*i].v;	s[i] = a[2*i+1].w;	}
			else	break;
		}
		gr->SetTicksVal('y',i,v,s);
	}
	else if(k[0]==3)
		gr->SetTicks('y', a[0].v, k[1]==3?iint(a[1].v):0, k[2]==3?a[2].v:NAN);
	else if(k[0]==2)	gr->SetYTT(a[0].w);
	else	return 1;
	return 0;
}
void mglc_ytick(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==3 && k[1]==3 && k[2]==3)
		mglprintf(out,1024,L"gr->SetTicks('y', %g, %d, %g);", a[0].v, iint(a[1].v),a[2].v);
	else if(k[0]==3 && k[1]==3)
		mglprintf(out,1024,L"gr->SetTicks('y', %g, %d);", a[0].v, iint(a[1].v));
	else if(k[0]==3 && k[1]==2)
		mglprintf(out,1024,L"gr->SetTicksVal('y', %g, \"%s\");", a[0].v, a[1].s);
	else if(k[0]==3)	mglprintf(out,1024,L"gr->SetTicks('y', %g);", a[0].v);
	else if(k[0]==2)	mglprintf(out,1024,L"gr->SetYTT(L\"%ls\");", a[0].w);
}
//-----------------------------------------------------------------------------
int mgls_ztick(mglGraph *gr, long n, mglArg *a, int k[10])
{
	if(k[0]==3 && k[1]==2)
	{
		mreal v[50];	const wchar_t *s[50]; int i;
		for(i=0;i<50 && i<n/2;i++)
		{
			if(a[2*i].type==2 && a[2*i+1].type==1)
			{	v[i] = a[2*i].v;	s[i] = a[2*i+1].w;	}
			else	break;
		}
		gr->SetTicksVal('z',i,v,s);
	}
	else if(k[0]==3)
		gr->SetTicks('z', a[0].v, k[1]==3?iint(a[1].v):0, k[2]==3?a[2].v:NAN);
	else if(k[0]==2)	gr->SetZTT(a[0].w);
	else	return 1;
	return 0;
}
void mglc_ztick(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==3 && k[1]==3 && k[2]==3)
		mglprintf(out,1024,L"gr->SetTicks('z', %g, %d, %g);", a[0].v, iint(a[1].v),a[2].v);
	else if(k[0]==3 && k[1]==3)
		mglprintf(out,1024,L"gr->SetTicks('z', %g, %d);", a[0].v, iint(a[1].v));
	else if(k[0]==3 && k[1]==2)
		mglprintf(out,1024,L"gr->SetTicksVal('z', %g, \"%s\");", a[0].v, a[1].s);
	else if(k[0]==3)	mglprintf(out,1024,L"gr->SetTicks('z', %g);", a[0].v);
	else if(k[0]==2)	mglprintf(out,1024,L"gr->SetZTT(L\"%ls\");", a[0].s);
}
//-----------------------------------------------------------------------------
int mgls_zoom(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==3 && k[1]==3 && k[2]==3 && k[3]==3)
		gr->Zoom(a[0].v, a[1].v, a[2].v, a[3].v);
	else	return 1;
	return 0;
}
void mglc_zoom(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==3 && k[1]==3 && k[2]==3 && k[3]==3)
		mglprintf(out,1024,L"gr->Zoom(%g, %g, %g, %g);", a[0].v, a[1].v, a[2].v, a[3].v);
}
//-----------------------------------------------------------------------------
int mgls_error(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1)
		gr->Error(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),
			k[4]==2?a[4].s:0, k[5]==3?a[5].v:NAN);
	else if(k[0]==1 && k[1]==1 && k[2]==1)
		gr->Error(*(a[0].d),*(a[1].d),*(a[2].d), k[3]==2?a[3].s:0, k[4]==3?a[4].v:NAN);
	else if(k[0]==1 && k[1]==1)
		gr->Error(*(a[0].d),*(a[1].d), k[2]==2?a[2].s:0, k[3]==3?a[3].v:NAN);
	else	return 1;
	return 0;
}
void mglc_error(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1)
		mglprintf(out,1024,L"gr->Error(%s, %s, %s, %s, \"%s\", %g);",  a[0].s, a[1].s, a[2].s, a[3].s, k[4]==2?a[4].s:"", k[5]==3?a[5].v:NAN);
	else if(k[0]==1 && k[1]==1 && k[2]==1)
		mglprintf(out,1024,L"gr->Error(%s, %s, %s, \"%s\", %g);",  a[0].s, a[1].s, a[2].s, k[3]==2?a[3].s:"", k[4]==3?a[4].v:NAN);
	else if(k[0]==1 && k[1]==1)
		mglprintf(out,1024,L"gr->Error(%s, %s, \"%s\", %g);",  a[0].s, a[1].s, k[2]==2?a[2].s:"", k[3]==3?a[3].v:NAN);
}
//-----------------------------------------------------------------------------
int mgls_extend(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==3)	a[0].d->Extend(iint(a[1].v),k[2]==3?iint(a[2].v):0);
	else	return 1;
	return 0;
}
void mglc_extend(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==3)
		mglprintf(out,1024,L"%s.Extend(%d, %d);", a[0].s, iint(a[1].v), k[2]==3?iint(a[2].v):0);
}
//-----------------------------------------------------------------------------
int mgls_info(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==1)	a[0].d->PrintInfo(gr->Message, k[1]==3 && a[1].v!=0);
	else if(k[0]==2)	sprintf(gr->Message, "%ls",a[0].w);
	else	return 1;
	return 0;
}
void mglc_info(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1)	mglprintf(out,1024,L"%s.PrintInfo(gr->Message, %s);", a[0].s, k[1]==3 && a[1].v!=0 ? "true" : "false");
	if(k[0]==2)	mglprintf(out,1024,L"sprintf(gr->Message, L\"%ls\");",a[0].w);
}
//-----------------------------------------------------------------------------
int mgls_integrate(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2)	a[0].d->Integral(a[1].s);
	else	return 1;
	return 0;
}
void mglc_integrate(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2)	mglprintf(out,1024,L"%s.Integral(\"%s\");", a[0].s, a[1].s);
}
//-----------------------------------------------------------------------------
int mgls_inplot(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==3 && k[1]==3 && k[2]==3 && k[3]==3)
		gr->InPlot(a[0].v, a[1].v, a[2].v, a[3].v, k[4]==3&&a[4].v!=0);
	else	return 1;
	return 0;
}
void mglc_inplot(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==3 && k[1]==3 && k[2]==3 && k[3]==3)
		mglprintf(out,1024,L"gr->InPlot(%g, %g, %g, %g, %s);", a[0].v, a[1].v, a[2].v, a[3].v, k[4]==3&&a[4].v!=0 ? "true":"false");
}
//-----------------------------------------------------------------------------
int mgls_columnplot(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==3 && k[1]==3)	gr->ColumnPlot(iint(a[0].v), iint(a[1].v), k[2]==3?a[2].v:0);
	else	return 1;
	return 0;
}
void mglc_columnplot(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==3 && k[1]==3)
		mglprintf(out,1024,L"gr->ColumnPlot(%d, %d, %g);", iint(a[0].v), iint(a[1].v), k[2]==3?a[2].v:0);
}
//-----------------------------------------------------------------------------
int mgls_stickplot(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==3 && k[1]==3 && k[2]==3 && k[3]==3)
		gr->StickPlot(iint(a[0].v), iint(a[1].v), a[2].v, a[3].v);
	else	return 1;
	return 0;
}
void mglc_stickplot(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==3 && k[1]==3 && k[2]==3 && k[3]==3)
		mglprintf(out,1024,L"gr->StickPlot(%d, %d, %g, %g);",
			iint(a[0].v), iint(a[1].v), a[2].v, a[3].v);
}
//-----------------------------------------------------------------------------
int mgls_pipe(mglGraph *gr, long , mglArg *a, int k[10])
{
	int i;
	for(i=0;i<7;i++)	if(k[i]!=1)	break;
	if(i==2)
		gr->Pipe(*(a[0].d),*(a[1].d),k[2]==2?a[2].s:0,k[3]==3?a[3].v:0.05,k[4]==3?iint(a[4].v):5,
			true,k[5]==3?a[5].v:NAN);
	else if(i==4)
		gr->Pipe(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),k[4]==2?a[4].s:0,k[5]==3?a[5].v:0.05,
			k[6]==3?iint(a[6].v):5,true,k[7]==3?a[7].v:NAN);
	else if(i==3)
		gr->Pipe(*(a[0].d),*(a[1].d),*(a[2].d),k[3]==2?a[3].s:0,k[4]==3?a[4].v:0.05,k[5]==3?iint(a[5].v):3);
	else if(i==6)
		gr->Pipe(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),*(a[5].d),
			k[6]==2?a[6].s:0,k[7]==3?a[7].v:0.05,k[8]==3?iint(a[8].v):3);
	else	return 1;
	return 0;
}
void mglc_pipe(wchar_t out[1024], long , mglArg *a, int k[10])
{
	int i;
	for(i=0;i<7;i++)	if(k[i]!=1)	break;
	if(i==2)
		mglprintf(out,1024,L"gr->Pipe(%s, %s, \"%s\", %g, %d, true, %g);",  a[0].s, a[1].s, k[2]==2?a[2].s:"", k[3]==3?a[3].v:0.05, k[4]==3?iint(a[4].v):5, k[5]==3?a[5].v:NAN);
	else if(i==4)
		mglprintf(out,1024,L"gr->Pipe(%s, %s, %s, %s, \"%s\", %g, %d, true, %g);",  a[0].s, a[1].s, a[2].s, a[3].s, k[4]==2?a[4].s:"", k[5]==3?a[5].v:0.05, k[6]==3?iint(a[6].v):5, k[7]==3?a[7].v:NAN);
	else if(i==3)
		mglprintf(out,1024,L"gr->Pipe(%s, %s, %s, \"%s\", %g, %d);",  a[0].s, a[1].s, a[2].s, k[3]==2?a[3].s:"", k[4]==3?a[4].v:0.05, k[5]==3?iint(a[5].v):3);
	else if(i==6)
		mglprintf(out,1024,L"gr->Pipe(%s, %s, %s, %s, %s, %s, \"%s\", %g, %d);",  a[0].s, a[1].s, a[2].s, a[3].s, a[4].s, a[5].s, k[6]==2?a[6].s:"", k[7]==3?a[7].v:0.05, k[8]==3?iint(a[8].v):3);
}
//-----------------------------------------------------------------------------
int mgls_origin(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==3 && k[1]==3)	gr->Org = mglPoint(a[0].v,a[1].v,k[2]==3?a[2].v:NAN);
	else	return 1;
	return 0;
}
void mglc_origin(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==3 && k[1]==3)
		mglprintf(out,1024,L"gr->Org = mglPoint(%g, %g, %g);", a[0].v, a[1].v, k[2]==3?a[2].v:0);
}
//-----------------------------------------------------------------------------
int mgls_plotfactor(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==3)
	{
		if(a[0].v>0)	{	gr->PlotFactor=a[0].v;	gr->AutoPlotFactor=false;	}
		else			{	gr->PlotFactor=1.55f;	gr->AutoPlotFactor=false;	}
	}
	else	return 1;
	return 0;
}
void mglc_plotfactor(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==3)	mglprintf(out,1024,L"if(%g>0) gr->PlotFactor = %g;\tgr->AutoPlotFactor = !(%g>0);", a[0].v, a[0].v, a[0].v);
}
//-----------------------------------------------------------------------------
int mgls_norm(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==3 && k[2]==3)
		a[0].d->Norm(a[1].v,a[2].v,k[3]==3?a[3].v!=0:false,k[4]==3?iint(a[4].v):0);
	else	return 1;
	return 0;
}
void mglc_norm(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==3 && k[2]==3)
		mglprintf(out,1024,L"%s.Norm(%g, %g, %s, %d);", a[0].s, a[1].v, a[2].v, (k[3]==3&&a[3].v!=0)?"true":"false", k[4]==3?iint(a[4].v):0);
}
//-----------------------------------------------------------------------------
int mgls_hist(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==3 && k[4]==3 && k[5]==3)
		*(a[0].d) = a[1].d->Hist(*(a[2].d), int(a[3].v+0.5), a[4].v, a[5].v, k[6]==3?int(a[6].v+0.5):0);
	else if(k[0]==1 && k[1]==1 && k[2]==3 && k[3]==3 && k[4]==3)
		*(a[0].d) = a[1].d->Hist(int(a[2].v+0.5), a[3].v, a[4].v, k[5]==3?int(a[5].v+0.5):0);
	else if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1 && k[4]==1)
		gr->Hist(*(a[0].d), *(a[1].d), *(a[2].d), *(a[3].d), *(a[4].d));
	else if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1)
		gr->Hist(*(a[0].d), *(a[1].d), *(a[2].d), *(a[3].d));
	else if(k[0]==1 && k[1]==1 && k[2]==1)
		gr->Hist(*(a[0].d), *(a[1].d), *(a[2].d));
	else	return 1;
	return 0;
}
void mglc_hist(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==3 && k[4]==3 && k[5]==3)
		mglprintf(out,1024,L"%s = %s.Hist(%s, %d, %g, %g, %d);", a[0].s, a[1].s, a[2].s, iint(a[3].v), a[4].v, a[5].v, k[6]==3?iint(a[6].v):0);
	else if(k[0]==1 && k[1]==1 && k[2]==3 && k[3]==3 && k[4]==3)
		mglprintf(out,1024,L"%s = %s.Hist(%d, %g, %g, %d);", a[0].s, a[1].s, iint(a[2].v), a[3].v, a[4].v, k[5]==3?iint(a[5].v):0);
	else if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1 && k[4]==1)
		mglprintf(out,1024,L"gr->Hist(%s, %s, %s, %s, %s);", a[0].s, a[1].s, a[2].s, a[3].s, a[4].s);
	else if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1)
		mglprintf(out,1024,L"gr->Hist(%s, %s, %s, %s);", a[0].s, a[1].s, a[2].s, a[3].s);
	else if(k[0]==1 && k[1]==1 && k[2]==1)
		mglprintf(out,1024,L"gr->Hist(%s, %s, %s);", a[0].s, a[1].s, a[2].s);
}
//-----------------------------------------------------------------------------
int mgls_mirror(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2)	a[0].d->Mirror(a[1].s);
	else	return 1;
	return 0;
}
void mglc_mirror(wchar_t out[1024], long , mglArg *a, int k[10])
{	if(k[0]==1 && k[1]==2)	mglprintf(out,1024,L"%s.Mirror(\"%s\");", a[0].s, a[1].s);	}
//-----------------------------------------------------------------------------
int mgls_hankel(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2)	a[0].d->Hankel(a[1].s);	else	return 1;
	return 0;
}
void mglc_hankel(wchar_t out[1024], long , mglArg *a, int k[10])
{	if(k[0]==1 && k[1]==2)	mglprintf(out,1024,L"%s.Hankel('%s');", a[0].s, a[1].s);	}
//-----------------------------------------------------------------------------
int mgls_sinfft(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2)	a[0].d->SinFFT(a[1].s);	else	return 1;
	return 0;
}
void mglc_sinfft(wchar_t out[1024], long , mglArg *a, int k[10])
{	if(k[0]==1 && k[1]==2)	mglprintf(out,1024,L"%s.SinFFT('%s');", a[0].s, a[1].s);	}
//-----------------------------------------------------------------------------
int mgls_cosfft(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2)	a[0].d->CosFFT(a[1].s);	else	return 1;
	return 0;
}
void mglc_cosfft(wchar_t out[1024], long , mglArg *a, int k[10])
{	if(k[0]==1 && k[1]==2)	mglprintf(out,1024,L"%s.CosFFT('%s');", a[0].s, a[1].s);	}
//-----------------------------------------------------------------------------
int mgls_new(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1)	a[0].d->Create(k[1]==3?iint(a[1].v):1, k[2]==3?iint(a[2].v):1, k[3]==3?iint(a[3].v):1);
	else	return 1;
	return 0;
}
void mglc_new(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1)
		mglprintf(out,1024,L"%s.Create(%d, %d, %d);", a[0].s, k[1]==3?iint(a[1].v):1, k[2]==3?iint(a[2].v):1, k[3]==3?iint(a[3].v):1);
}
//-----------------------------------------------------------------------------
int mgls_var(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==3 && k[2]==3)
	{	a[0].d->Create(iint(a[1].v));	a[0].d->Fill(a[2].v, k[3]==3?a[3].v:NAN);	}
	else	return 1;
	return 0;
}
void mglc_var(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==3 && k[2]==3)
	mglprintf(out,1024,L"%s.Create(%d);\t%s.Fill(%g,%g);",a[0].s, iint(a[1].v), a[0].s, a[2].v, k[3]==3?a[3].v:NAN);
}
//-----------------------------------------------------------------------------
int mgls_chdir(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==2)
	{	int r=chdir(a[0].s);	if(r)	gr->SetWarn(mglWarnFile, "chdir");	}
	else	return 1;
	return 0;
}
void mglc_chdir(wchar_t out[1024], long , mglArg *a, int k[10])
{	if(k[0]==2)	mglprintf(out,1024,L"chdir(\"%s\");", a[0].s);	}
//-----------------------------------------------------------------------------
int mgls_perspective(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==3)	gr->Perspective(a[0].v);
	else	return 1;
	return 0;
}
void mglc_perspective(wchar_t out[1024], long , mglArg *a, int k[10])
{	if(k[0]==3)	mglprintf(out,1024,L"gr->Perspective(%g);", a[0].v);	}
//-----------------------------------------------------------------------------
int mgls_facex(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==3 && k[1]==3 && k[2]==3 && k[3]==3 && k[4]==3)
		gr->FaceX(a[0].v, a[1].v, a[2].v, a[3].v, a[4].v, k[5]==2?a[5].s:0, k[6]==3?a[6].v:0, k[7]==3?a[7].v:0);
	else	return 1;
	return 0;
}
void mglc_facex(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==3 && k[1]==3 && k[2]==3 && k[3]==3 && k[4]==3)
		mglprintf(out,1024,L"gr->FaceX(%g, %g, %g, %g, %g, \"%s\", %g, %g);", a[0].v, a[1].v, a[2].v, a[3].v, a[4].v, k[5]==2?a[5].s:"", k[6]==3?a[6].v:0, k[7]==3?a[7].v:0);
}
//-----------------------------------------------------------------------------
int mgls_facey(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==3 && k[1]==3 && k[2]==3 && k[3]==3 && k[4]==3)
		gr->FaceY(a[0].v, a[1].v, a[2].v, a[3].v, a[4].v, k[5]==2?a[5].s:0, k[6]==3?a[6].v:0, k[7]==3?a[7].v:0);
	else	return 1;
	return 0;
}
void mglc_facey(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==3 && k[1]==3 && k[2]==3 && k[3]==3 && k[4]==3)
		mglprintf(out,1024,L"gr->FaceY(%g, %g, %g, %g, %g, \"%s\", %g, %g);", a[0].v, a[1].v, a[2].v, a[3].v, a[4].v, k[5]==2?a[5].s:"", k[6]==3?a[6].v:0, k[7]==3?a[7].v:0);
}
//-----------------------------------------------------------------------------
int mgls_facez(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==3 && k[1]==3 && k[2]==3 && k[3]==3 && k[4]==3)
		gr->FaceZ(a[0].v, a[1].v, a[2].v, a[3].v, a[4].v, k[5]==2?a[5].s:0, k[6]==3?a[6].v:0, k[7]==3?a[7].v:0);
	else	return 1;
	return 0;
}
void mglc_facez(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==3 && k[1]==3 && k[2]==3 && k[3]==3 && k[4]==3)
		mglprintf(out,1024,L"gr->FaceZ(%g, %g, %g, %g, %g, \"%s\", %g, %g);", a[0].v, a[1].v, a[2].v, a[3].v, a[4].v, k[5]==2?a[5].s:"", k[6]==3?a[6].v:0, k[7]==3?a[7].v:0);
}
//-----------------------------------------------------------------------------
int mgls_normsl(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==3 && k[2]==3)
		a[0].d->NormSl(a[1].v, a[2].v, k[3]==2?a[3].s[0]:'z', k[4]==3?a[4].v!=0:true, k[5]==3?a[5].v!=0:false);
	else	return 1;
	return 0;
}
void mglc_normsl(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==3 && k[2]==3)
		mglprintf(out,1024,L"%s.NormSl(%g, %g, '%c', %s, %s);", a[0].s, a[1].v, a[2].v, k[3]==2?a[3].s[0]:'z', (k[4]!=3||a[4].v!=0)?"true":"false", (k[5]==3&&a[5].v!=0)?"true":"false");
}
//-----------------------------------------------------------------------------
int mgls_momentum(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==2)
		*(a[0].d) = a[1].d->Momentum(k[3]==2?a[3].s[0]:'z', a[2].s);
	else	return 1;
	return 0;
}
void mglc_momentum(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==2)
		mglprintf(out,1024,L"%s = %s.Momentum('%c', \"%s\");", a[0].s, a[1].s, k[3]==2?a[3].s[0]:'z', a[2].s);
}
//-----------------------------------------------------------------------------
int mgls_fit(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1 && k[4]==1&& k[5]==2 && k[6]==2)
		gr->Fit(*(a[0].d), *(a[1].d), *(a[2].d), *(a[3].d), *(a[4].d), a[5].s, a[6].s, (k[7]==1 && a[7].d->nx>=long(strlen(a[6].s)))?a[7].d->a:0, k[8]==3?a[8].v!=0:false);
	else if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1 && k[4]==2 && k[5]==2)
		gr->Fit(*(a[0].d), *(a[1].d), *(a[2].d), *(a[3].d), a[4].s, a[5].s, (k[6]==1 && a[6].d->nx>=long(strlen(a[5].s)))?a[6].d->a:0, k[7]==3?a[7].v!=0:false);
	else if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==2 && k[4]==2)
		gr->Fit(*(a[0].d), *(a[1].d), *(a[2].d), a[3].s, a[4].s, (k[5]==1 && a[5].d->nx>=long(strlen(a[4].s)))?a[5].d->a:0, k[6]==3?a[6].v!=0:false);
	else if(k[0]==1 && k[1]==1 && k[2]==2 && k[3]==2)
		gr->Fit(*(a[0].d), *(a[1].d), a[2].s, a[3].s, (k[4]==1 && a[4].d->nx>=long(strlen(a[3].s)))?a[4].d->a:0, k[5]==3?a[5].v!=0:false);
	else	return 1;
	return 0;
}
void mglc_fit(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1 && k[4]==1&& k[5]==2 && k[6]==2)
		mglprintf(out,1024,L"gr->Fit(%s, %s, %s, %s, %s, \"%s\", \"%s\", %s, %s);", a[0].s, a[1].s, a[2].s, a[3].s, a[4].s, a[5].s, a[6].s, (k[7]==1 && a[7].d->nx>=long(strlen(a[6].s)))?a[7].s:"NULL", (k[8]==3&&a[8].v!=0)?"true":"false");
	else if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1 && k[4]==2 && k[5]==2)
		mglprintf(out,1024,L"gr->Fit(%s, %s, %s, %s, \"%s\", \"%s\", %s, %s);", a[0].s, a[1].s, a[2].s, a[3].s, a[4].s, a[5].s, (k[6]==1 && a[6].d->nx>=long(strlen(a[5].s)))?a[6].s:"NULL", (k[7]==3&&a[7].v!=0)?"true":"false");
	else if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==2 && k[4]==2)
		mglprintf(out,1024,L"gr->Fit(%s, %s, %s, \"%s\", \"%s\", %s, %s);", a[0].s, a[1].s, a[2].s, a[3].s, a[4].s, (k[5]==1 && a[5].d->nx>=long(strlen(a[4].s)))?a[5].s:"NULL", (k[6]==3&&a[6].v!=0)?"true":"false");
	else if(k[0]==1 && k[1]==1 && k[2]==2 && k[3]==2)
		mglprintf(out,1024,L"gr->Fit(%s, %s, \"%s\", \"%s\", %s, %s);", a[0].s, a[1].s, a[2].s, a[3].s, (k[5]==1 && a[5].d->nx>=long(strlen(a[4].s)))?a[5].s:"NULL", (k[6]==3&&a[6].v!=0)?"true":"false");
}
//-----------------------------------------------------------------------------
int mgls_fits(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1 && k[4]==1 && k[5]==1 && k[6]==2 && k[7]==2)
		gr->FitS(*(a[0].d), *(a[1].d), *(a[2].d), *(a[3].d), *(a[4].d), *(a[5].d), a[6].s, a[7].s, (k[8]==1 && a[8].d->nx>=long(strlen(a[7].s)))?a[8].d->a:0, k[9]==3?a[9].v!=0:false);
	else if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1 && k[4]==1 && k[5]==2 && k[6]==2)
		gr->FitS(*(a[0].d), *(a[1].d), *(a[2].d), *(a[3].d), *(a[4].d), a[5].s, a[6].s, (k[7]==1 && a[7].d->nx>=long(strlen(a[6].s)))?a[7].d->a:0, k[8]==3?a[8].v!=0:false);
	else if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1 && k[4]==2 && k[5]==2)
		gr->FitS(*(a[0].d), *(a[1].d), *(a[2].d), *(a[3].d), a[4].s, a[5].s, (k[6]==1 && a[6].d->nx>=long(strlen(a[5].s)))?a[6].d->a:0, k[7]==3?a[7].v!=0:false);
	else if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==2 && k[4]==2)
		gr->FitS(*(a[0].d), *(a[1].d), *(a[2].d), a[3].s, a[4].s, (k[5]==1 && a[5].d->nx>=long(strlen(a[4].s)))?a[5].d->a:0, k[6]==3?a[6].v!=0:false);
	else	return 1;
	return 0;
}
void mglc_fits(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1 && k[4]==1 && k[5]==1 && k[6]==2 && k[7]==2)
		mglprintf(out,1024,L"gr->FitS(%s, %s, %s, %s, %s, %s, \"%s\", \"%s\", %s, %s);", a[0].s, a[1].s, a[2].s, a[3].s, a[4].s, a[5].s, a[6].s, a[7].s, (k[8]==1 && a[8].d->nx>=long(strlen(a[7].s)))?a[8].s:"NULL", (k[9]==3&&a[9].v!=0)?"true":"false");
	else if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1 && k[4]==1 && k[5]==2 && k[6]==2)
		mglprintf(out,1024,L"gr->FitS(%s, %s, %s, %s, %s, \"%s\", \"%s\", %s, %s);", a[0].s, a[1].s, a[2].s, a[3].s, a[4].s, a[5].s, a[6].s, (k[7]==1 && a[7].d->nx>=long(strlen(a[6].s)))?a[7].s:"NULL", (k[8]==3&&a[8].v!=0)?"true":"false");
	else if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1 && k[4]==2 && k[5]==2)
		mglprintf(out,1024,L"gr->FitS(%s, %s, %s, %s, \"%s\", \"%s\", %s, %s);", a[0].s, a[1].s, a[2].s, a[3].s, a[4].s, a[5].s, (k[6]==1 && a[6].d->nx>=long(strlen(a[5].s)))?a[6].s:"NULL", (k[7]==3&&a[7].v!=0)?"true":"false");
	else if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==2 && k[4]==2)
		mglprintf(out,1024,L"gr->FitS(%s, %s, %s, \"%s\", \"%s\", %s, %s);", a[0].s, a[1].s, a[2].s, a[3].s, a[4].s, (k[5]==1 && a[5].d->nx>=long(strlen(a[4].s)))?a[5].s:"NULL", (k[6]==3&&a[6].v!=0)?"true":"false");
}
//-----------------------------------------------------------------------------
int mgls_putsfit(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==3 && k[1]==3 && k[2]==3)
		gr->PutsFit(mglPoint(a[0].v,a[1].v,a[2].v), k[3]==2?a[3].s:0, k[4]==2?a[4].s:0, k[5]==3?a[5].v:-1);
	else if(k[0]==3 && k[1]==3)
		gr->PutsFit(mglPoint(a[0].v,a[1].v), k[2]==2?a[2].s:0, k[3]==2?a[3].s:0, k[4]==3?a[4].v:-1);
	else	return 1;
	return 0;
}
void mglc_putsfit(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==3 && k[1]==3 && k[2]==3)
		mglprintf(out,1024,L"gr->PutsFit(mglPoint(%g, %g, %g), \"%s\", \"%s\", %g);", a[0].v,a[1].v,a[2].v, k[3]==2?a[3].s:"", k[4]==2?a[4].s:"", k[5]==3?a[5].v:-1);
	else if(k[0]==3 && k[1]==3)
		mglprintf(out,1024,L"gr->PutsFit(mglPoint(%g, %g), \"%s\", \"%s\", %g);", a[0].v,a[1].v, k[2]==2?a[2].s:"", k[3]==2?a[3].s:"", k[4]==3?a[4].v:-1);

	if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1 && k[4]==1 && k[5]==1 && k[6]==2 && k[7]==2)
		mglprintf(out,1024,L"gr->FitS(%s, %s, %s, %s, %s, %s, \"%s\", \"%s\", %s, %s);", a[0].s, a[1].s, a[2].s, a[3].s, a[4].s, a[5].s, a[6].s, a[7].s, (k[8]==1 && a[8].d->nx>=long(strlen(a[7].s)))?a[8].s:"NULL", (k[9]==3&&a[9].v!=0)?"true":"false");
	else if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1 && k[4]==1 && k[5]==2 && k[6]==2)
		mglprintf(out,1024,L"gr->FitS(%s, %s, %s, %s, %s, \"%s\", \"%s\", %s, %s);", a[0].s, a[1].s, a[2].s, a[3].s, a[4].s, a[5].s, a[6].s, (k[7]==1 && a[7].d->nx>=long(strlen(a[6].s)))?a[7].s:"NULL", (k[8]==3&&a[8].v!=0)?"true":"false");
	else if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1 && k[4]==2 && k[5]==2)
		mglprintf(out,1024,L"gr->FitS(%s, %s, %s, %s, \"%s\", \"%s\", %s, %s);", a[0].s, a[1].s, a[2].s, a[3].s, a[4].s, a[5].s, (k[6]==1 && a[6].d->nx>=long(strlen(a[5].s)))?a[6].s:"NULL", (k[7]==3&&a[7].v!=0)?"true":"false");
	else if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==2 && k[4]==2)
		mglprintf(out,1024,L"gr->FitS(%s, %s, %s, \"%s\", \"%s\", %s, %s);", a[0].s, a[1].s, a[2].s, a[3].s, a[4].s, (k[5]==1 && a[5].d->nx>=long(strlen(a[4].s)))?a[5].s:"NULL", (k[6]==3&&a[6].v!=0)?"true":"false");
}
//-----------------------------------------------------------------------------
int mgls_arrowsize(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==3)	gr->ArrowSize = a[0].v/50;
	else	return 1;
	return 0;
}
void mglc_arrowsize(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==3)	mglprintf(out,1024,L"gr->ArrowSize = %g;", a[0].v/50);
}
//-----------------------------------------------------------------------------
int mgls_rearrange(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==3)
		a[0].d->Rearrange(iint(a[1].v), k[2]==3?iint(a[2].v):0, k[3]==3?iint(a[3].v):0);
	else	return 1;
	return 0;
}
void mglc_rearrange(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==3)
		mglprintf(out,1024,L"%s.Rearrange(%d, %d, %d);",a[0].s, iint(a[1].v), k[2]==3?iint(a[2].v):0, k[3]==3?iint(a[3].v):0);
}
//-----------------------------------------------------------------------------
int mgls_ctick(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==2)	gr->SetCTT(a[0].w);
	else if(k[0]==3)	gr->SetTicks('c',a[0].v);
	else	return 1;
	return 0;
}
void mglc_ctick(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==2)	mglprintf(out,1024,L"gr->SetCTT(L\"%ls\");", a[0].w);
	else if(k[0]==3)	mglprintf(out,1024,L"gr->SetTicks('c',%g);", a[0].v);
}
//-----------------------------------------------------------------------------
int mgls_fplot(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==2 && k[1]==2 && k[2]==2)	gr->Plot(a[0].s, a[1].s, a[2].s, k[3]==2?a[3].s:0, k[4]==3?iint(a[4].v):100);
	else if(k[0]==2)	gr->Plot(a[0].s, k[1]==2?a[1].s:0, k[2]==3?a[2].v:NAN, k[3]==3?iint(a[3].v):100);
	else	return 1;
	return 0;
}
void mglc_fplot(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==2 && k[1]==2 && k[2]==2)
		mglprintf(out,1024,L"gr->Plot(\"%s\", \"%s\", \"%s\", \"%s\", %d);", a[0].s, a[1].s, a[2].s, k[3]==2?a[3].s:"", k[4]==3?iint(a[4].v):100);
	else if(k[0]==2)
		mglprintf(out,1024,L"gr->Plot(\"%s\", \"%s\", %g, %d);", a[0].s, k[1]==2?a[1].s:"", k[2]==3?a[2].v:NAN, k[3]==3?iint(a[3].v):100);
}
//-----------------------------------------------------------------------------
int mgls_fsurf(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==2 && k[1]==2 && k[2]==2)	gr->Surf(a[0].s, a[1].s, a[2].s, k[3]==2?a[3].s:0, k[4]==3?iint(a[4].v):100);
	else if(k[0]==2)	gr->Surf(a[0].s, k[1]==2?a[1].s:0, k[2]==3?iint(a[2].v):100);
	else	return 1;
	return 0;
}
void mglc_fsurf(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==2 && k[1]==2 && k[2]==2)
		mglprintf(out,1024,L"gr->Surf(\"%s\", \"%s\", \"%s\", \"%s\", %d);", a[0].s, a[1].s, a[2].s, k[3]==2?a[3].s:"", k[4]==3?iint(a[4].v):100);
	else if(k[0]==2)
		mglprintf(out,1024,L"gr->Surf(\"%s\", \"%s\", %d);", a[0].s, k[1]==2?a[1].s:"", k[2]==3?iint(a[2].v):100);
}
//-----------------------------------------------------------------------------
int mgls_fgets(mglGraph *gr, long , mglArg *a, int k[10])
{
	const long size=1024;
	char buf[size];
	FILE *fp;
	if(k[0]==3 && k[1]==3 && k[2]==3 && k[3]==2)
	{
		int i, n = (k[4]==3?iint(a[4].v):0);
		fp = fopen(a[3].s,"rt");
		if(!fp)
		{
			if(gr->Message)	sprintf(gr->Message,"Wrong file %s",a[3].s);
			return 0;
		}
		for(i=0;i<n;i++)	if(!fgets(buf,size,fp))	continue;
		memset(buf,0,size);
		if(!fgets(buf,size,fp))
		{
			if(gr->Message)	sprintf(gr->Message,"Couldn't read %d-th string of file %s",n,a[3].s);
			fclose(fp);	return 0;
		}
		fclose(fp);
		gr->Text(mglPoint(a[0].v,a[1].v,a[2].v),buf, (k[5]==2 && a[5].s[0]!=0)?a[5].s:gr->FontDef, k[6]==3?a[6].v:-1.4);
	}
	else if(k[0]==3 && k[1]==3 && k[2]==2)
	{
		int i, n = k[3]==3?iint(a[3].v):0;
		fp = fopen(a[2].s,"rt");
		if(!fp)
		{
			if(gr->Message)	sprintf(gr->Message,"Wrong file %s",a[2].s);
			return 0;
		}
		for(i=0;i<n;i++)	if(!fgets(buf,1024,fp))	continue;
		if(!fgets(buf,1024,fp))
		{
			if(gr->Message)	sprintf(gr->Message,"Couldn't read %d-th string of file %s",n,a[3].s);
			fclose(fp);	return 0;
		}
		fclose(fp);
		gr->Text(mglPoint(a[0].v,a[1].v,gr->Min.z),buf, (k[4]==2 && a[4].s[0]!=0)?a[4].s:gr->FontDef, k[5]==3?a[5].v:-1.4);
	}
	else	return 1;
	return 0;
}
void mglc_fgets(wchar_t [1024], long , mglArg *, int [10])	{}
//-----------------------------------------------------------------------------
int mgls_import(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2 && k[2]==2)
		a[0].d->Import(a[1].s, a[2].s, k[3]==3?a[3].v:0, k[4]==3?a[4].v:1);
	else	return 1;
	return 0;
}
void mglc_import(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2 && k[2]==2)
		mglprintf(out,1024,L"%s->Import(%s, %s, %g, %g);", a[0].s, a[1].s, a[2].s, k[3]==3?a[3].v:0, k[4]==3?a[4].v:1);
}
//-----------------------------------------------------------------------------
int mgls_export(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2 && k[2]==2)
		a[0].d->Export(a[1].s, a[2].s, k[3]==3?a[3].v:0, k[4]==3?a[4].v:1);
	else	return 1;
	return 0;
}
void mglc_export(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2 && k[2]==2)
		mglprintf(out,1024,L"%s->Export(%s, %s, %g, %g);", a[0].s, a[1].s, a[2].s, k[3]==3?a[3].v:0, k[4]==3?a[4].v:1);
}
//-----------------------------------------------------------------------------
int mgls_write(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==2)	gr->WriteFrame(a[0].s, "MathGL");
	else	return 1;
	return 0;
}
void mglc_write(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==2)
		mglprintf(out,1024,L"gr->WriteFrame(\"%s\", \"MathGL\");", a[0].s);
}
//-----------------------------------------------------------------------------
int mgls_region(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==1)
		gr->Region(*(a[0].d),*(a[1].d),*(a[2].d),k[3]==2?a[3].s:0, NAN, k[4]!=3 || a[4].v!=0);
	else if(k[0]==1 && k[1]==1)
		gr->Region(*(a[0].d),*(a[1].d),k[2]==2?a[2].s:0, NAN, k[3]!=3 || a[3].v!=0);
	else	return 1;
	return 0;
}
void mglc_region(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==1)
		mglprintf(out,1024,L"gr->Region(%s, %s, %s, \"%s\", NAN, %s);", a[0].s, a[1].s, a[2].s, k[3]==2?a[3].s:"", k[4]!=3 || a[4].v!=0 ? "true" : "false");
	else if(k[0]==1 && k[1]==1)
		mglprintf(out,1024,L"gr->Region(%s, %s, \"%s\", NAN, %s);", a[0].s, a[1].s, k[2]==2?a[2].s:"", k[3]!=3 || a[3].v!=0 ? "true" : "false");
}
//-----------------------------------------------------------------------------
int mgls_title(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==2)	gr->Title(a[0].s, k[1]==2?a[1].s:0, k[2]==3?a[2].v:-2);
	else	return 1;
	return 0;
}
void mglc_title(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==2)	mglprintf(out,1024,L"gr->Title(\"%s\", \"%s\", %g);", a[0].s, k[1]==2?a[1].s:"", k[2]==3?a[2].v:-2);
}
//-----------------------------------------------------------------------------
int mgls_envelop(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1)		a[0].d->Envelop(k[1]==2 ? a[1].s[0] : 'x');
	else	return 1;
	return 0;
}
void mglc_envelop(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1)	mglprintf(out,1024,L"%s.Envelop('%c');",a[0].s, k[1]==2?a[1].s[0]:'x');
}
//-----------------------------------------------------------------------------
int mgls_sew(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1)		a[0].d->Sew(k[1]==2?a[1].s:"xyz", k[2]==3 ? a[2].v : 2*M_PI);
	else	return 1;
	return 0;
}
void mglc_sew(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1)	mglprintf(out,1024,L"%s.Sew(\"%s\", %g);", a[0].s, k[1]==2?a[1].s:"xyz", k[2]==3 ? a[2].v : 2*M_PI);
}
//-----------------------------------------------------------------------------
int mgls_evaluate(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1 && k[4]==1)
		*(a[0].d) = a[1].d->Evaluate(*(a[2].d), *(a[3].d), *(a[4].d), k[5]!=3 || a[5].v!=0);
	else if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1)
		*(a[0].d) = a[1].d->Evaluate(*(a[2].d), *(a[3].d), k[4]!=3 || a[4].v!=0);
	else if(k[0]==1 && k[1]==1 && k[2]==1)
		*(a[0].d) = a[1].d->Evaluate(*(a[2].d), k[3]!=3 || a[3].v!=0);
	else	return 1;
	return 0;
}
void mglc_evaluate(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1)	mglprintf(out,1024,L"%s.Sew(\"%s\", %g);", a[0].s, k[1]==2?a[1].s:"xyz", k[2]==3 ? a[2].v : 2*M_PI);
}
//-----------------------------------------------------------------------------
int mgls_put(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[1]==3 && k[0]==1)
		a[0].d->Put(a[1].v, k[2]==3?iint(a[2].v):-1, k[3]==3?iint(a[3].v):-1, k[4]==3?iint(a[4].v):-1);
	else if(k[0]==1 && k[1]==1)
		a[0].d->Put(*(a[1].d), k[2]==3?iint(a[2].v):-1, k[3]==3?iint(a[3].v):-1, k[4]==3?iint(a[4].v):-1);
	else	return 1;
	return 0;
}
void mglc_put(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==3)
		mglprintf(out,1024,L"%s.Put(%g, %d, %d, %d);", a[0].s, a[1].v, k[2]==3?iint(a[2].v):-1, k[3]==3?iint(a[3].v):-1, k[4]==3?iint(a[4].v):-1);
	else if(k[0]==1 && k[1]==1)
		mglprintf(out,1024,L"%s.Put(%s, %d, %d, %d);", a[0].s, a[1].s, k[2]==3?iint(a[2].v):-1, k[3]==3?iint(a[3].v):-1, k[4]==3?iint(a[4].v):-1);
}
//-----------------------------------------------------------------------------
int mgls_palette(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==2)	gr->SetPalette(a[0].s);
	else	return 1;
	return 0;
}
void mglc_palette(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==2)	mglprintf(out,1024,L"gr->SetPalette(\"%s\");", a[0].s);
}
//-----------------------------------------------------------------------------
int mgls_combine(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==1)
		*(a[0].d) = a[1].d->Combine(*(a[2].d));
	else	return 1;
	return 0;
}
void mglc_combine(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==1)
		mglprintf(out,1024,L"%s = %s.Combine(%s);",a[0].s, a[1].s, a[2].s);
}
//-----------------------------------------------------------------------------
int mgls_pde(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2 && k[2]==1 && k[3]==1)
		*(a[0].d) = mglPDE(a[1].s, *(a[2].d), *(a[3].d), gr->Min, gr->Max, k[4]==3?a[4].v:0.1, k[5]==3?a[5].v:100);
	else	return 1;
	return 0;
}
void mglc_pde(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2 && k[2]==1 && k[3]==1)
		mglprintf(out,1024,L"%s = mglPDE(\"%s\", %s, %s, gr->Min, gr->Max, %g, %g);",a[0].s, a[1].s, a[2].s, a[3].s, k[4]==3?a[4].v:0.1, k[5]==3?a[5].v:100);
}
//-----------------------------------------------------------------------------
int mgls_qo2d(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2 && k[2]==1 && k[3]==1 && k[4]==1)
		*(a[0].d) = mglQO2d(a[1].s, *(a[2].d), *(a[3].d), *(a[4].d), k[5]==3?a[5].v:1, k[6]==3?a[6].v:100, k[7]==1?a[7].d:0, k[8]==1?a[8].d:0);
	else	return 1;
	return 0;
}
void mglc_qo2d(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2 && k[2]==1 && k[3]==1)
		mglprintf(out,1024,L"%s = mglQO2d(\"%s\", %s, %s, %s, %g, %g, %s, %s);",a[0].s, a[1].s, a[2].s, a[3].s, a[4].s, k[5]==3?a[5].v:1, k[6]==3?a[6].v:100, k[7]==1?a[7].s:"NULL", k[8]==1?a[8].s:"NULL");
}
//-----------------------------------------------------------------------------
int mgls_af2d(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2 && k[2]==1 && k[3]==1 && k[4]==1)
		*(a[0].d) = mglAF2d(a[1].s, *(a[2].d), *(a[3].d), *(a[4].d), k[5]==3?a[5].v:1, k[6]==3?a[6].v:100, k[7]==1?a[7].d:0, k[8]==1?a[8].d:0);
	else	return 1;
	return 0;
}
void mglc_af2d(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2 && k[2]==1 && k[3]==1)
		mglprintf(out,1024,L"%s = mglAF2d(\"%s\", %s, %s, %s, %g, %g, %s, %s);",a[0].s, a[1].s, a[2].s, a[3].s, a[4].s, k[5]==3?a[5].v:1, k[6]==3?a[6].v:100, k[7]==1?a[7].s:"NULL", k[8]==1?a[8].s:"NULL");
}
//-----------------------------------------------------------------------------
int mgls_ray(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2 && k[2]==3 && k[3]==3 && k[4]==3 && k[5]==3 && k[6]==3 && k[7]==3)
		*(a[0].d) = mglRay(a[1].s, mglPoint(a[2].v, a[3].v, a[4].v), mglPoint(a[5].v, a[6].v, a[7].v), k[8]==3?a[8].v:0.1, k[9]==3?a[9].v:10);
	else	return 1;
	return 0;
}
void mglc_ray(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2 && k[2]==3 && k[3]==3 && k[4]==3 && k[5]==3 && k[6]==3 && k[7]==3)
		mglprintf(out,1024,L"%s = mglRay(\"%s\", mglPoint(%g, %g, %g), mglPoint(%g, %g, %g), %g, %g);",a[0].s, a[1].s, a[2].v, a[3].v, a[4].v, a[5].v, a[6].v, a[7].v, k[8]==3?a[8].v:0.1, k[9]==3?a[9].v:10);
}
//-----------------------------------------------------------------------------
int mgls_jacobian(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1)
		*(a[0].d) = mglJacobian(*(a[1].d), *(a[2].d), *(a[3].d));
	else if(k[0]==1 && k[1]==1 && k[2]==1)
		*(a[0].d) = mglJacobian(*(a[1].d), *(a[2].d));
	else	return 1;
	return 0;
}
void mglc_jacobian(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1)
		mglprintf(out,1024,L"%s = mglJacobian(%s, %s, %s);", a[0].s, a[1].s, a[2].s, a[3].s);
	else if(k[0]==1 && k[1]==1 && k[2]==1)
		mglprintf(out,1024,L"%s = mglJacobian(%s, %s);", a[0].s, a[1].s, a[2].s);
}
//-----------------------------------------------------------------------------
int mgls_tens(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]!=1 || k[1]!=1)	return 1;
	else if(k[2]!=1)	gr->Tens(*(a[0].d),*(a[1].d), k[2]==2?a[2].s:0, k[3]==3 ? a[3].v : NAN);
	else if(k[3]!=1)	gr->Tens(*(a[0].d),*(a[1].d),*(a[2].d), k[3]==2?a[3].s:0, k[4]==3 ? a[4].v : NAN);
	else 				gr->Tens(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d), k[4]==2?a[4].s:0);
	return 0;
}
void mglc_tens(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]!=1 || k[1]!=1)	return;
	else if(k[2]!=1)
		mglprintf(out,1024,L"gr->Tens(%s, %s, \"%s\", %g);",a[0].s, a[1].s, k[2]==2?a[2].s:"", k[3]==3?a[3].v:NAN);
	else if(k[3]!=1)
		mglprintf(out,1024,L"gr->Tens(%s, %s, %s, \"%s\", %g);", a[0].s, a[1].s, a[2].s, k[3]==2?a[3].s:"", k[4]==3?a[4].v:NAN);
	else 	mglprintf(out,1024,L"gr->Plot(%s, %s, %s, %s, \"%s\");", a[0].s, a[1].s, a[2].s, a[3].s, k[4]==2?a[4].s:"");
}
//-----------------------------------------------------------------------------
int mgls_ticklen(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==3)	gr->SetTickLen(a[0].v, k[1]==3?a[1].v:1);
	else	return 1;
	return 0;
}
void mglc_ticklen(wchar_t out[1024], long , mglArg *a, int k[10])
{	if(k[0]==3)	mglprintf(out,1024,L"gr->SetTickLen(%g,%g);", a[0].v, k[1]==3?a[1].v:1);	}
//-----------------------------------------------------------------------------
int mgls_tickstl(mglGraph *gr, long , mglArg *a, int k[10])
{
	if(k[0]==2)	gr->SetTickStl(a[0].s, k[1]==2?a[1].s:"");
	else	return 1;
	return 0;
}
void mglc_tickstl(wchar_t out[1024], long , mglArg *a, int k[10])
{	if(k[0]==2)	mglprintf(out,1024,L"gr->SetTickStl(\"%s\",\"%s\");", a[0].s, k[1]==2?a[1].s:"");	}
//-----------------------------------------------------------------------------
int mgls_ranges(mglGraph *gr, long n, mglArg *a, int k[10])
{
	register int i;
	if(n==6)
	{
		bool ok=true;
		for(i=0;i<6;i++)	if(k[i]!=3)	ok = false;
		if(ok)	gr->SetRanges(a[0].v,a[1].v,a[2].v, a[3].v,a[4].v,a[5].v);
		else return 1;
	}
	else if(n==4)
	{
		bool ok=true;
		for(i=0;i<4;i++)	if(k[i]!=3)	ok = false;
		if(ok)	gr->SetRanges(a[0].v,a[1].v, a[2].v,a[3].v);
		else return 1;
	}
	else return 1;
	return 0;
}
void mglc_ranges(wchar_t out[1024], long n, mglArg *a, int k[10])
{
	register int i;
	if(n==6)
	{
		bool ok=true;
		for(i=0;i<6;i++)	if(k[i]!=3)	ok = false;
		if(ok)	mglprintf(out,1024,L"gr->SetRanges(%g, %g, %g, %g, %g, %g);", a[0].v,a[1].v,a[2].v,a[3].v,a[4].v,a[5].v);
	}
	else if(n==4)
	{
		bool ok=true;
		for(i=0;i<4;i++)	if(k[i]!=3)	ok = false;
		if(ok)	mglprintf(out,1024,L"gr->SetRanges(%g, %g, %g, %g);", a[0].v,a[1].v,a[2].v,a[3].v);
	}
}
//-----------------------------------------------------------------------------
int mgls_adjust(mglGraph *gr, long , mglArg *a, int k[10])
{	gr->AdjustTicks(k[0]==2?a[0].s:"xyzc");	return 0;	}
void mglc_adjust(wchar_t out[1024], long , mglArg *a, int k[10])
{	mglprintf(out,1024,L"gr->AdjustTicks(\"%s\");",k[0]==2?a[0].s:"xyzc");	}
//-----------------------------------------------------------------------------
int mgls_insert(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2)	a[0].d->Insert(a[1].s[0], k[2]==3 ? iint(a[2].v):0, k[3]==3 ? iint(a[3].v):1);
	else	return 1;
	return 0;
}
void mglc_insert(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2)	mglprintf(out,1024,L"%s.Insert('%c', %d, %d);",
	    a[0].s, a[1].s[0], k[2]==3 ? iint(a[2].v):0, k[3]==3 ? iint(a[3].v):1);
}
//-----------------------------------------------------------------------------
int mgls_delete(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2)	a[0].d->Delete(a[1].s[0], k[2]==3 ? iint(a[2].v):0, k[3]==3 ? iint(a[3].v):1);
	else	return 1;
	return 0;
}
void mglc_delete(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2)	mglprintf(out,1024,L"%s.Delete('%c', %d, %d);",
	    a[0].s, a[1].s[0], k[2]==3 ? iint(a[2].v):0, k[3]==3 ? iint(a[3].v):1);
}
//-----------------------------------------------------------------------------
int mgls_roll(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2 && k[2]==3)	a[0].d->Roll(a[1].s[0], iint(a[2].v));
	else	return 1;
	return 0;
}
void mglc_roll(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==2 && k[2]==3)	mglprintf(out,1024,L"%s.Roll('%c', %d);",
	    a[0].s, a[1].s[0], iint(a[2].v));
}
//-----------------------------------------------------------------------------
int mgls_triangulate(mglGraph *, long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1)
		*(a[0].d) = mglTriangulation(*(a[1].d), *(a[2].d), *(a[3].d), k[4]==3?a[4].v:0);
	else if(k[0]==1 && k[1]==1 && k[2]==1)
		*(a[0].d) = mglTriangulation(*(a[1].d), *(a[2].d), k[3]==3?a[3].v:0);
	else	return 1;
	return 0;
}
void mglc_triangulate(wchar_t out[1024], long , mglArg *a, int k[10])
{
	if(k[0]==1 && k[1]==1 && k[2]==1 && k[3]==1)
		mglprintf(out,1024,L"%s = mglTriangulation(%s, %s, %s, %g);",a[0].s, a[1].s, a[2].s, a[3].s, k[4]==3?a[4].v:0);
	else if(k[0]==1 && k[1]==1 && k[2]==1)
		mglprintf(out,1024,L"%s = mglTriangulation(%s, %s, %g);",a[0].s, a[1].s, a[2].s, k[3]==3?a[3].v:0);
}
//-----------------------------------------------------------------------------
mglCommand mgls_base_cmd[] = {
	{L"addlegend",L"Add legend entry",L"addlegend 'txt' 'fmt'", mgls_addlegend, mglc_addlegend, false, 2},
	{L"addto",L"Add data or number",L"addto Var Dat|Var num", mgls_addto, mglc_addto, false, 3},
	{L"adjust",L"Adjust ticks for best view",L"adjust ['dir']", mgls_adjust, mglc_adjust, false, 2},
	{L"af2d",L"Solve PDE in accompanied coordinates",L"af2d Res 'ham' IniRe IniIm Ray [r k0 Xout Yout]", mgls_af2d, mglc_af2d, true, 3},
	{L"alpha",L"Switch on/off transparency",L"alpha [val]", mgls_alpha, mglc_alpha, false, 2},
	{L"alphadef",L"Set default transparency",L"alphadef val", mgls_alphadef, mglc_alphadef, false, 2},
	{L"ambient",L"Set ambient light brightness",L"ambient val", mgls_ambient, mglc_ambient, false, 2},
	{L"area",L"Draw area plot for 1D data",L"area Ydat ['fmt' zval]|Xdat Ydat ['fmt' zval]|Xdat Ydat Zdat ['fmt']", mgls_area, mglc_area, false, 0},
	{L"arrowsize",L"Set size of arrows",L"arrowsize val", mgls_arrowsize, mglc_arrowsize, false, 2},
	{L"aspect",L"Set aspect ration",L"aspect valx valy [valz]", mgls_aspect, mglc_aspect, false, 4},
	{L"axial",L"Draw surfaces of contour lines rotation",L"axial Zdat ['fmt' num]|Xdat Ydat Zdat ['fmt' num]", mgls_axial, mglc_axial, false, 0},
	{L"axialdir",L"Set axis of rotation",L"axialdir 'dir'", mgls_axialdir, mglc_axialdir, false, 2},
	{L"axis",L"Setup or draw axis",L"axis ['dir' adjust]|'fx' 'fy' ['fz' 'fc']|how", mgls_axis, mglc_axis, false, 1},
	{L"ball",L"Draw point (ball)",L"ball posx posy ['fmt']|posx posy posz ['fmt']", mgls_ball, mglc_ball, false, 1},
	{L"barh",L"Draw horizontal bars for 1D data", L"barh Ydat ['fmt' zval above]|Xdat Ydat ['fmt' zval above]", mgls_barh, mglc_barh, false, 0},
	{L"bars",L"Draw bars for 1D data",L"bars Ydat ['fmt' zval above]|Xdat Ydat ['fmt' zval above]|Xdat Ydat Zdat ['fmt' above]", mgls_bars, mglc_bars, false, 0},
	{L"barwidth",L"Set default bars width",L"barwidth val", mgls_barwidth, mglc_barwidth},
	{L"beam",L"Draw quasioptical beam",L"beam Tr G1 G2 Adat r ['sch' flag num] ", mgls_beam, mglc_beam, false, 0},
	{L"belt",L"Draw belts",L"belt Zdat ['fmt']|Xdat Ydat Zdat ['fmt']", mgls_belt, mglc_belt, false, 0},
	{L"box",L"Draw bounding box",L"box ['fmt' ticks]", mgls_box, mglc_box, false, 1},
	{L"boxplot",L"Draw boxplot for 2D data",L"boxplot Ydat ['fmt']|Xdat Ydat ['fmt']", mgls_boxplot, mglc_boxplot},
	{L"boxs",L"Draw boxes",L"boxs Zdat ['fmt']|Xdat Ydat Zdat ['fmt']", mgls_boxs, mglc_boxs, false, 0},
	{L"caxis",L"Set color range",L"caxis c1 c2", mgls_caxis, mglc_caxis, false, 2},
	{L"chart",L"Draw chart",L"chart Dat ['fmt']", mgls_chart, mglc_chart, false, 0},
	{L"chdir",L"Change current directory",L"chdir 'dir'", mgls_chdir, mglc_chdir, false, 5},
	{L"clearlegend",L"Clear legend antries",L"clearlegend", mgls_clearlegend, mglc_clearlegend, false, 2},
	{L"clf",L"Clear picture",L"clf", mgls_clf, mglc_clf, false, 5},
	{L"cloud",L"Draw cloud",L"cloud Adat ['fmt']|Xdat Ydat Zdat Adat ['fmt']", mgls_cloud, mglc_cloud, false, 0},
	{L"colorbar",L"Draw colorbar",L"colorbar ['fmt' pos]|Vdat ['fmt' pos]|'sch' pos x y w h ", mgls_colorbar, mglc_colorbar, false, 1},
	{L"columnplot",L"Set position of plot inside cell of column", L"columnplot num ind [d]", mgls_columnplot, mglc_columnplot, false, 4},
	{L"combine", L"Direct multiplication of arrays", L"combine Res Adat Bdat", mgls_combine, mglc_combine, false, 3},
	{L"cone",L"Draw cone",L"cone x1 y1 z1 x2 y2 z2 r1 [r2 'fmt' edge]", mgls_cone, mglc_cone, false, 1},
	{L"cont",L"Draw contour lines",L"cont Zdat ['fmt' num zpos]|Vdat Zdat ['fmt' zpos]|Xdat Ydat Zdat ['fmt' num zpos]|Vdat Xdat Ydat Zdat ['fmt' zpos]", mgls_cont, mglc_cont, false, 0},
	{L"cont3",L"Draw contour lines for 3D data",L"cont3 Adat 'dir' [val 'fmt' num]|Vdat Adat 'dir' [val 'fmt']|Xdat Ydat Zdat Adat 'dir' [val 'fmt' num]|Vdat Xdat Ydat Zdar Adat 'dir' [val 'fmt']", mgls_cont3, mglc_cont3, false, 0},
	{L"conta",L"Draw contour lines at central slices of 3D data",L"conta Adat ['fmt' num]|Xdat Ydat Zdat Adat ['fmt' num]", mgls_conta, mglc_conta, false, 0},
	{L"contd",L"Draw solid contours with manual colors",L"contd Zdat ['fmt' num zpos]|Vdat Zdat ['fmt' zpos]|Xdat Ydat Zdat ['fmt' num zpos]|Vdat Xdat Ydat Zdat ['fmt' zpos]", mgls_contd, mglc_contd, false, 0},
	{L"contf",L"Draw solid contours",L"contf Zdat ['fmt' num zpos]|Vdat Zdat ['fmt' zpos]|Xdat Ydat Zdat ['fmt' num zpos]|Vdat Xdat Ydat Zdat ['fmt' zpos]", mgls_contf, mglc_contf, false, 0},
	{L"contf3",L"Draw solid contour lines for 3D data",L"contf3 Adat 'dir' [val 'fmt' num]|Vdat Adat 'dir' [val 'fmt']|Xdat Ydat Zdat Adat 'dir' [val 'fmt' num]|Vdat Xdat Ydat Zdar Adat 'dir' [val 'fmt']", mgls_contf3, mglc_contf3, false, 0},
	{L"contfa",L"Draw solid contour lines at central slices of 3D data",L"contfa Adat [fmt num]|Xdat Ydat Zdat Adat [fmt num]", mgls_contfa, mglc_contfa, false, 0},
	{L"contfx",L"Draw solid contour lines at x-slice (or x-plane)",L"contfx Dat ['fmt' pos num]", mgls_contfx, mglc_contfx, false, 0},
	{L"contfy",L"Draw solid contour lines at y-slice (or y-plane)",L"contfy Dat ['fmt' pos num]", mgls_contfy, mglc_contfy, false, 0},
	{L"contfz",L"Draw solid contour lines at z-slice (or z-plane)",L"contfz Dat ['fmt' pos num]", mgls_contfz, mglc_contfz, false, 0},
	{L"contx",L"Draw contour lines at x-slice (or x-plane)",L"contx Dat ['fmt' pos num]", mgls_contx, mglc_contx, false, 0},
	{L"conty",L"Draw contour lines at y-slice (or y-plane)",L"conty Dat ['fmt' pos num]", mgls_conty, mglc_conty, false, 0},
	{L"contz",L"Draw contour lines at z-slice (or z-plane)",L"contz Dat ['fmt' pos num]", mgls_contz, mglc_contz, false, 0},
	{L"copy",L"Copy data from another variable",L"copy Dat1 Dat2 ['eq' onaxis]", mgls_copy, mglc_copy, true, 3},
	{L"cosfft",L"Cos-Fourier transform at some direction",L"cosfft Dat 'dir'", mgls_cosfft, mglc_cosfft, false, 3},
	{L"crange",L"Set color range",L"crange Dat [sym] | c1 c2", mgls_crange, mglc_crange, false, 2},
	{L"crop",L"Crop edge of data",L"crop Dat n1 n2 'dir'", mgls_crop, mglc_crop, false, 3},
	{L"crust",L"Draw reconstructed surface for arbitrary data points",L"crust Xdat Ydat Zdat ['fmt']", mgls_crust, mglc_crust, false, 0},
	{L"ctick",L"Set ticks for colorbar",L"ctick 'tmpl' | dx", mgls_ctick, mglc_ctick, false, 2},
	{L"cumsum",L"Cumulative summation",L"cumsum Dat 'dir'", mgls_cumsum, mglc_cumsum, false, 3},
	{L"curve",L"Draw curve",L"curve x1 y1 dx1 dy1 x2 y2 dx2 dy2 ['fmt']|x1 y1 z1 dx1 dy1 dz1 x2 y2 z2 dx2 dy2 dz2 ['fmt']", mgls_curve, mglc_curve, false, 1},
	{L"cut",L"Setup plot points cutting",L"cut val|x1 y1 z1 x2 y2 z2|'cond'", mgls_cut, mglc_cut, false, 2},
	{L"delete",L"Delete slice of data",L"delete Dat 'dir' [pos=0 num=1]", mgls_delete, mglc_delete, false, 3},
	{L"dens",L"Draw density plot",L"dens Zdat ['fmt' zpos]|Xdat Ydat Zdat ['fmt' zpos]", mgls_dens, mglc_dens, false, 0},
	{L"dens3",L"Draw density plot at slices of 3D data",L"dens3 Adat 'dir' [pos 'fmt']|Xdat Ydat Zdat Adat 'dir' [pos 'fmt']", mgls_dens3, mglc_dens3, false, 0},
	{L"densa",L"Draw density plot at central slices of 3D data",L"densa Adat ['fmt']|Xdat Ydat Zdat Adat ['fmt']", mgls_densa, mglc_densa, false, 0},
	{L"densx",L"Draw density plot at x-slice (or x-plane)",L"densx Dat ['fmt' pos]", mgls_densx, mglc_densx, false, 0},
	{L"densy",L"Draw density plot at y-slice (or y-plane)",L"densy Dat ['fmt' pos]", mgls_densy, mglc_densy, false, 0},
	{L"densz",L"Draw density plot at z-slice (or z-plane)",L"densz Dat ['fmt' pos]", mgls_densz, mglc_densz, false, 0},
	{L"dew",L"Draw dew plot",L"dew Udat Vdat ['fmt']|Xdat Ydat Udat Vdat ['fmt']", mgls_dew, mglc_dew, false, 0},
	{L"diff",L"Numerically differentiate data",L"diff Var 'dir'", mgls_diff, mglc_diff, false, 3},
	{L"diff2",L"Numerically double differentiate data",L"diff2 Var 'dir'", mgls_diff2, mglc_diff2, false, 3},
	{L"divto",L"Divide by data or number",L"divto Var Dat|Var num", mgls_divto, mglc_divto, false, 3},
	{L"dots",L"Draw dots for arbitrary data points",L"dots Xdat Ydat Zdat ['fmt']", mgls_dots, mglc_dots, false, 0},
	{L"drop",L"Draw drop",L"drop x0 y0 dx dy r ['col' sh asp]|x0 y0 z0 dx dy dz r ['col' sh asp]", mgls_drop, mglc_drop, false, 0},
	{L"envelop",L"Find envelop for the data",L"envelop Dat ['dir']", mgls_envelop, mglc_envelop, false, 3},
	{L"error",L"Draw error boxes",L"error Ydat Yerr ['fmt' zval]|Xdat Ydat Yerr ['fmt' zval]|Xdat Ydat Xerr Yerr ['fmt' zval]", mgls_error, mglc_error, false, 0},
	{L"evaluate",L"Evaluate (interpolate) values of array Dat at points i=idat,j=jdat,k=kdat",L"evaluate Res Dat Idat [norm]|Res Dat Idat Jdat [norm]|Res Dat Idat Jdat Kdat [norm]", mgls_evaluate, mglc_evaluate, true, 3},
	{L"export",L"Export data to PNG picture",L"export Dat 'fname' 'sch' [v1 v2]", mgls_import, mglc_import, false, 3},
	{L"extend",L"Extend data array",L"extend Dat dim1 [dim2]", mgls_extend, mglc_extend, false, 3},
	{L"facex",L"Draw face perpendicular to x-axis",L"facex x0 y0 z0 wy wz ['fmt' d1 d2]", mgls_facex, mglc_facex, false, 1},
	{L"facey",L"Draw face perpendicular to y-axis",L"facex x0 y0 z0 wx wz ['fmt' d1 d2]", mgls_facey, mglc_facey, false, 1},
	{L"facez",L"Draw face perpendicular to z-axis",L"facex x0 y0 z0 wy wz ['fmt' d1 d2]", mgls_facez, mglc_facez, false, 1},
	{L"fall",L"Draw waterfalls",L"fall Zdat ['fmt']|Xdat Ydat Zdat ['fmt']", mgls_fall, mglc_fall, false, 0},
	{L"fgets",L"Print string from file",L"fgets x y z 'fname' [pos=0 'fmt' size]|x y z 'fname' [pos=0 'fmt' size]", mgls_fgets, mglc_fgets, false, 1},
	{L"fill",L"Fill data linearly in range [v1, v2]",L"fill Var v1 v2 ['dir'] | Var 'eq' [Vdat Wdat]", mgls_fill, mglc_fill, false, 3},
	{L"fillsample",L"Fill x-,k-samples for transforms",L"fillsample Var num 'how'", mgls_fillsample, mglc_fillsample, false, 3},
	{L"fit",L"Fit data to formula",L"fit Res A 'eq' 'var' [Ini]|Res X A 'eq' 'var' [Ini]|Res X Y A 'eq' 'var' [Ini]|Res X Y Z A 'eq' 'var' [Ini]", mgls_fit, mglc_fit, true, 3},
	{L"fits",L"Fit data to formula",L"fits Res A S 'eq' 'var' [Ini]|Res X A S 'eq' 'var' [Ini]|Res X Y A S 'eq' 'var' [Ini]|Res X Y Z A S 'eq' 'var' [Ini]", mgls_fits, mglc_fits, true, 3},
	{L"flow",L"Draw flow threads for vector field",L"flow Udat Vdat ['fmt' num zval]|Xdat Ydat Udat Vdat ['fmt' num zval]|Udat Vdat Wdat ['fmt' num]|Xdat Ydat Zdat Udat Vdat ['fmt' num]|\
			x0 y0 Udat Vdat ['fmt' zval]|x0 y0 Xdat Ydat Udat Vdat ['fmt' zval]|x0 y0 z0 Udat Vdat Wdat ['fmt']|x0 y0 z0 Xdat Ydat Zdat Udat Vdat Wdat ['fmt']", mgls_flow, mglc_flow, false, 0},
	{L"fog",L"Switch on/off fog",L"fog val [pos]", mgls_fog, mglc_fog, false, 2},
	{L"font",L"Setup font",L"font 'fmt' [size]", mgls_font, mglc_font, false, 2},
	{L"fplot",L"Plot curve by formula",L"fplot 'y(x)' ['fmt' zval num]|'x(t)' 'y(t)' 'z(t)' ['fmt' num]", mgls_fplot, mglc_fplot, false, 1},
	{L"fsurf",L"Plot surface by formula",L"fsurf 'z(x,y)' ['fmt' num]|'x(u,v)' 'y(u,v)' 'z(u,v)' ['fmt' num]", mgls_fsurf, mglc_fsurf, false, 1},
	{L"grad",L"Draw gradient lines for scalar field",L"grad Phi ['fmt' num zval]|Xdat Ydat Phi ['fmt' num zval]|Xdat Ydat Zdat Phi ['fmt' num]", mgls_grad, mglc_grad, false, 0},
	{L"grid",L"Draw grid",L"grid ['dir' 'fmt']", mgls_grid, mglc_grid, false, 0},
	{L"grid2",L"Draw grid for data array(s)",L"grid Zdat ['fmt']|Xdat Ydat Zdat ['fmt']", mgls_grid2, mglc_grid2, false, 0},
	{L"grid3",L"Draw grid at slices of 3D data",L"grid3 Adat 'dir' [pos 'fmt']|Xdat Ydat Zdat Adat 'dir' [pos 'fmt']", mgls_grid3, mglc_grid3, false, 0},
	{L"grida",L"Draw grid at central slices of 3D data",L"grida Adat ['fmt']|Xdat Ydat Zdat Adat ['fmt']", mgls_grida, mglc_grida, false, 0},
	{L"hankel",L"Hankel transform at some direction",L"hankel Dat 'dir'", mgls_hankel, mglc_hankel, false, 3},
	{L"hist",L"Create histogram (distribution) of data values",L"hist Res Dat num v1 v2 [nsub]|Res Dat Wdat num v1 v2 [nsub]", mgls_hist, mglc_hist, true, 3},
	{L"idset",L"Set column id for data",L"idset Dat 'ids'", mgls_idset, mglc_idset, false, 3},
	{L"import",L"Import data from PNG picture",L"import Dat 'fname' 'scheme' [v1 v2]", mgls_import, mglc_import, true, 3},
	{L"info",L"Print information about data",L"info Dat [detail]|'message'", mgls_info, mglc_info, false, 3},
	{L"inplot",L"Set position of plot in picture",L"x1 x2 y1 y2 [rel]", mgls_inplot, mglc_inplot, false, 4},
	{L"insert",L"Insert slice of data",L"insert Dat 'dir' [pos=0 num=1]", mgls_insert, mglc_insert, false, 3},
	{L"integrate",L"Integrate data",L"integrate Dat 'dir'", mgls_integrate, mglc_integrate, false, 3},
	{L"jacobian",L"Get Jacobian",L"jacobian Res Xdat Ydat [Zdat]", mgls_jacobian, mglc_jacobian, true, 3},
	{L"label",L"Draw label at arbitrary position",L"label x y 'txt' ['fmt' size]", mgls_label, mglc_label, false, 1},
	{L"legend",L"Draw legend",L"legend [pos 'fmt' size llen]|x y ['fmt' size llen]", mgls_legend, mglc_legend, false, 1},
	{L"legendbox",L"Switch on/off drawing box in legend",L"legendbox val", mgls_legendbox, mglc_legendbox, false, 2},
	{L"legendmarks",L"Set number of marks in the legend",L"legendmarks val", mgls_legendbox, mglc_legendbox, false, 2},
	{L"light",L"Setup light",L"light [val] | val num | num xpos ypos zpos ['fmt' br]", mgls_light, mglc_light, false, 2},
	{L"line",L"Draw line",L"line x1 y1 x2 y2 ['fmt']|x1 y1 z1 x2 y2 z2 ['fmt']", mgls_line, mglc_line, false, 1},
	{L"linewidth",L"Set default line width",L"linewidth val", mgls_linewidth, mglc_linewidth, false, 2},
	{L"loadfont",L"Load fontfaces",L"loadfont ['fmt']", mgls_loadfont, mglc_loadfont, false, 2},
	{L"map",L"Draw mapping plot",L"map Udat Vdat ['fmt']|Xdat Ydat Udat Vdat ['fmt']", mgls_map, mglc_map, false, 0},
	{L"mark",L"Draw mark plot for 1D data",L"mark Ydat Rdat ['fmt' zval]|Xdat Ydat Rdat ['fmt' zval]|Xdat Ydat Zdat Rdat ['fmt']", mgls_mark, mglc_mark, false, 0},
	{L"marksize",L"Set size of markers",L"marksize val", mgls_marksize, mglc_marksize, false, 2},
	{L"max",L"Find maximal value over direction",L"max Res Dat 'dir'", mgls_max, mglc_max, true, 3},
	{L"mesh",L"Draw mesh surface",L"mesh Zdat ['fmt']|Xdat Ydat Zdat ['fmt']", mgls_mesh, mglc_mesh, false, 0},
	{L"meshnum",L"Set number of lines in mesh/fall/vect and so on",L"meshnum val", mgls_meshnum, mglc_meshnum, false, 2},
	{L"min",L"Find minimal value over direction",L"min Res Dat 'dir'", mgls_min, mglc_min, true, 3},
	{L"mirror",L"Mirror data at some direction",L"mirror Dat 'dir'", mgls_mirror, mglc_mirror, false, 3},
	{L"modify",L"Modify data values by formula",L"modify Dat 'eq' [num] | Dat 'eq' Vdat [Wdat]", mgls_modify, mglc_modify, false, 3},
	{L"momentum",L"Get momentum along direction",L"momentum Res Dat 'how' ['dir']", mgls_momentum, mglc_momentum, true, 3},
	{L"multo",L"Multiply by data or number",L"multo Var Dat|Var num", mgls_multo, mglc_multo, false, 3},
	{L"new",L"Create new data",L"new Dat nx [ny nz]", mgls_new, mglc_new, true, 3},
	{L"norm",L"Normalize data",L"norm Dat v1 v2 [sym dim]", mgls_norm, mglc_norm, false, 3},
	{L"normsl",L"Normalize data slice by slice",L"normsl Dat v1 v2 ['dir' keep sym] ", mgls_normsl, mglc_normsl, false, 3},
	{L"origin",L"Set axis origin",L"origin x0 y0 [z0]", mgls_origin, mglc_origin, false, 2},
	{L"palette",L"Set palette for 1D plots",L"palette 'colors'", mgls_palette, mglc_palette, false, 2},
	{L"pde",L"Solve PDE",L"pde Res 'ham' IniRe IniIm [dz k0]", mgls_pde, mglc_pde, true, 3},
	{L"perspective",L"Set perspective",L"perspective val", mgls_perspective, mglc_perspective, false, 2},
	{L"pipe",L"Draw flow pipes for vector field",L"pipe Udat Vdat ['fmt' rad num zval]|Xdat Ydat Udat Vdat ['fmt' rad num zval]|Udat Vdat Wdat ['fmt' rad num]|Xdat Ydat Zdat Udat Vdat Wdat ['fmt' rad num]", mgls_pipe, mglc_pipe, false, 0},
	{L"plot",L"Draw usual plot for 1D data",L"plot Ydat ['fmt' zval]|Xdat Ydat ['fmt' zval]|Xdat Ydat Zdat ['fmt']", mgls_plot, mglc_plot, false, 0},
	{L"plotfactor",L"Set plotfactor",L"plotfactor val", mgls_plotfactor, mglc_plotfactor, false, 2},
	{L"put",L"Put value (numeric or array) to given data element",L"put Dat val [i j k] | Dat Val [i j k]", mgls_put, mglc_put, false, 3},
	{L"putsfit",L"Print fitted formula",L"putsfit x y ['pre' 'font' size]|x y z ['pre' 'font' size]", mgls_putsfit, mglc_putsfit, false, 0},
	{L"qo2d",L"Solve PDE in accompanied coordinates",L"qo2d Res 'ham' IniRe IniIm Ray [r k0 Xout Yout]", mgls_qo2d, mglc_qo2d, true, 3},
	{L"quadplot",L"Draw surface of quadrangles",L"quadplot Idat Xdat Ydat ['fmt']|Idat Xdat Ydat Zdat ['fmt']|Idat Xdat Ydat Zdat Cdat ['fmt'] ", mgls_quadplot, mglc_quadplot, false, 0},
	{L"radar",L"Draw radar chart",L"radar Rdat ['fmt' r]", mgls_radar, mglc_radar, false, 0},
	{L"ranges",L"Set axis ranges",L"ranges x1 x2 y1 y2 [z1 z2]", mgls_ranges, mglc_ranges, false, 2},
	{L"ray",L"Solve Hamiltonian ODE (find GO ray or trajectory)",L"ray Res 'ham' x0 y0 z0 px0 py0 pz0 [dz=0.1 tmax=10]", mgls_ray, mglc_ray, true, 3},
	{L"read",L"Read data from file",L"read Dat 'file' [nx ny nz]", mgls_read, mglc_read, true, 3},
	{L"readall",L"Read and join data from several files",L"readall Dat 'templ' [slice]", mgls_readall, mglc_readall, true, 3},
	{L"readhdf",L"Read data from HDF5 file",L"readhdf Dat 'file' 'id'", mgls_readhdf, mglc_readhdf, true, 3},
	{L"readmat",L"Read data from file with sizes specified in first row",L"readmat Dat 'file' [dim]", mgls_readmat, mglc_readmat, true, 3},
	{L"rearrange",L"Rearrange data dimensions",L"rearrange Dat mx [my mz]", mgls_rearrange, mglc_rearrange, false, 3},
	{L"rect",L"Draw rectangle",L"rect x1 y1 x2 y2 ['fmt']|x1 y1 z1 x2 y2 z2 ['fmt']", mgls_rect, mglc_rect, false, 1},
	{L"region",L"Draw filled region between 2 curves",L"region Ydat1 Ydat2 ['fmt' inside]|Xdat Ydat1 Ydat2 ['fmt' inside]", mgls_region, mglc_region, false, 0},
	{L"resize",L"Resize data",L"resize Res Dat mx [my mz]", mgls_resize, mglc_resize, true, 3},
	{L"roll",L"Roll data along direction",L"roll Dat 'dir' num", mgls_roll, mglc_roll, false, 0},
	{L"rotate",L"Rotate plot",L"rotate tetz tetx [tety] | tet x y z", mgls_rotate, mglc_rotate, false, 4},
	{L"rotatetext",L"Set to auto rotate text or not",L"rotatetext val", mgls_rotatetext, mglc_rotatetext, false, 2},
	{L"save",L"Save data to file",L"save Dat 'file'", mgls_save, mglc_save, false, 3},
	{L"savehdf",L"Save data to HDF5 file",L"savehdf Dat 'file' 'id'", mgls_savehdf, mglc_savehdf, false, 3},
	{L"setsize",L"Set picture size",L"setsize width height", mgls_setsize, mglc_setsize, false, 5},
	{L"sew",L"Remove jump into the data, like phase jumps",L"sew Dat ['dir' da]", mgls_sew, mglc_sew, false, 3},
	{L"sinfft",L"Sin-Fourier transform at some direction",L"sinfft Dat 'dir'", mgls_sinfft, mglc_sinfft, false, 3},
	{L"smooth",L"Smooth data",L"smooth Dat [kind 'dir']", mgls_smooth, mglc_smooth, false, 3},
	{L"sphere",L"Draw sphere",L"sphere x0 y0 r ['fmt']|x0 y0 z0 r ['fmt']", mgls_sphere, mglc_sphere, false, 1},
	{L"squeeze",L"Squeeze data",L"squeeze Dat kx [ky kz]", mgls_squeeze, mglc_squeeze, false, 3},
	{L"stem",L"Draw stem plot for 1D data",L"stem Ydat ['fmt' zval]|Xdat Ydat ['fmt' zval]|Xdat Ydat Zdat ['fmt']", mgls_stem, mglc_stem, false, 0},
	{L"step",L"Draw step plot for 1D data",L"step Ydat ['fmt' zval]|Xdat Ydat ['fmt' zval]|Xdat Ydat Zdat ['fmt']", mgls_step, mglc_step, false, 0},
	{L"stfa",L"Draw STFA diagram",L"stfa Udat Vdat dn ['fmt']|Xdat Ydat Udat Vdat dn ['fmt']", mgls_stfa, mglc_stfa, false, 0},
	{L"stfad",L"Do STFA transform",L"stfad Res Real Imag dn ['dir']", mgls_stfad, mglc_stfad, true, 3},
	{L"stickplot",L"Set position of plot inside cell of stick", L"stickplot num ind tet phi", mgls_stickplot, mglc_stickplot, false, 4},
	{L"subdata",L"Extract sub-array",L"subdata Res Dat nx [ny nz]", mgls_subdata, mglc_subdata, true, 3},
	{L"subplot",L"Set position of plot",L"subplot m n pos [dx dy]|m n pos 'style'", mgls_subplot, mglc_subplot, false, 4},
	{L"subto",L"Subtract data or number",L"subto Var Dat|Var num", mgls_subto, mglc_subto, false, 3},
	{L"sum",L"Find summation over direction",L"sum Res Dat 'dir'", mgls_sum, mglc_sum, true, 3},
	{L"surf",L"Draw solid surface",L"surf Zdat ['fmt']|Xdat Ydat Zdat ['fmt']", mgls_surf, mglc_surf, false, 0},
	{L"surf3",L"Draw isosurface for 3D data",L"surf3 Adat ['fmt' num]|Xdat Ydat Zdat Adat ['fmt' num]|Adat val ['fmt']|Xdat Ydat Zdat Adat val ['fmt']", mgls_surf3, mglc_surf3, false, 0},
	{L"surf3a",L"Draw isosurface for 3D data transpared by other data",L"surf3a Adat Cdat ['fmt' num]|Xdat Ydat Zdat Adat Cdat ['fmt' num]|Adat Cdat val ['fmt']|Xdat Ydat Zdat Adat Cdat val ['fmt']", mgls_surf3a, mglc_surf3a, false, 0},
	{L"surf3c",L"Draw isosurface for 3D data colored by other data",L"surf3c Adat Cdat ['fmt' num]|Xdat Ydat Zdat Adat Cdat ['fmt' num]|Adat Cdat val ['fmt']|Xdat Ydat Zdat Adat Cdat val ['fmt']", mgls_surf3c, mglc_surf3c, false, 0},
	{L"surfa",L"Draw solid surface transpared by other data",L"surfa Zdat Cdat ['fmt']|Xdat Ydat Zdat Cdat ['fmt']", mgls_surfa, mglc_surfa, false, 0},
	{L"surfc",L"Draw solid surface colored by other data",L"surfc Zdat Cdat ['fmt']|Xdat Ydat Zdat Cdat ['fmt']", mgls_surfc, mglc_surfc, false, 0},
	{L"swap",L"Swap data (usefull after Fourier transform)",L"swap Dat 'dir'", mgls_swap, mglc_swap, false, 0},
	{L"tens",L"Draw tension plot for 1D data",L"tens Ydat Cdat ['fmt' zval]|Xdat Ydat Cdat ['fmt' zval]|Xdat Ydat Zdat Cdat ['fmt']", mgls_tens, mglc_tens, 0},
	{L"ternary",L"Switch on/off to use ternary axis",L"ternary val", mgls_ternary, mglc_ternary, false, 2},
	{L"text",L"Draw text at some position or along curve",L"text x y 'txt' ['fmt' size]|x y z 'txt' ['fmt' size]|x y dx dy 'txt' ['fmt' size]|x y z dx dy dz 'txt' ['fmt' size]|Ydat 'txt' ['font' sise zval]|Xdat Ydat 'txt' ['font' sise zval]", mgls_text, mglc_text, false, 0},
	{L"textmark",L"Draw TeX mark at point position",L"textmark Ydat Rdat 'text' ['fmt' zval]|Xdat Ydat Rdat 'text' ['fmt' zval]|Xdat Ydat Zdat Rdat 'text' ['fmt']", mgls_textmark, mglc_textmark, false, 0},
	{L"ticklen",L"Set tick length",L"ticklen val [stt]", mgls_ticklen, mglc_ticklen, false, 2},
	{L"tickstl",L"Set tick style",L"tickstl 'stl' ['sub']", mgls_tickstl, mglc_tickstl, false, 2},
	{L"tile",L"Draw horizontal tiles",L"tile Zdat ['fmt']|Xdat Ydat Zdat ['fmt']", mgls_tile, mglc_tile, false, 0},
	{L"tiles",L"Draw horizontal tiles with variable size",L"tiles Zdat Rdat ['fmt']|Xdat Ydat Zdat Rdat ['fmt']", mgls_tiles, mglc_tiles, false, 0},
	{L"title",L"Print title for the picture",L"title 'text' ['fmt' size]", mgls_title, mglc_title, false, 1},
	{L"tlabel",L"Draw label for t-axis",L"tlabel 'txt' [pos size shift]", mgls_tlabel, mglc_tlabel, false, 1},
	{L"torus",L"Draw surface of curve rotation",L"torus Rdat ['fmt']|Zdat Rdat ['fmt']", mgls_torus, mglc_torus, false, 0},
	{L"trace",L"Get trace of array",L"trace Res Dat", mgls_trace, mglc_trace, true, 3},
	{L"traj",L"Draw vectors along a curve",L"traj Xdat Ydat Udat Vdat ['fmt' zval len]|Xdat Ydat Zdat Udat Vdat Wdat ['fmt' len]", mgls_traj, mglc_traj, false, 0},
	{L"transform",L"Do integral transform of data",L"transform Res 'how' Rdat Idat", mgls_transform, mglc_transform, true, 3},
	{L"transforma",L"Do integral transform of data",L"transforma Res 'how' Adat Pdat", mgls_transforma, mglc_transforma, true, 3},
	{L"transparent",L"Switch off transparency",L"transparent val", mgls_transparent, mglc_transparent, false, 2}, //!!! OLD !!!
	{L"transpose",L"Transpose data array",L"transpose Dat ['dir']", mgls_transpose, mglc_transpose, false, 3},
	{L"transptype",L"Set type transparency",L"transptype val", mgls_transptype, mglc_transptype, false, 2},
	{L"triangulate",L"Find triangles of randomly placed points",L"triangulate Res Xdat Ydat [er]|Res Xdat Ydat Zdat [er]", mgls_triangulate, mglc_triangulate, true, 3},
	{L"tricont",L"Draw contour lines for surface of triangles",L"tricont Vdat Idat Xdat Ydat ['fmt']|Vdat Idat Xdat Ydat Zdat ['fmt']|Vdat Idat Xdat Ydat Zdat Cdat ['fmt'] ", mgls_tricont, mglc_tricont, false, 0},
	{L"triplot",L"Draw surface of triangles",L"triplot Idat Xdat Ydat ['fmt']|Idat Xdat Ydat Zdat ['fmt']|Idat Xdat Ydat Zdat Cdat ['fmt'] ", mgls_triplot, mglc_triplot, false, 0},
	{L"tube",L"Draw curve by tube",L"tube Ydat Rdat ['fmt' zval]|Ydat rval ['fmt' zval]|Xdat Ydat Rdat ['fmt' zval]|Xdat Ydat rval ['fmt' zval]|Xdat Ydat Zdat Rdat ['fmt']|Xdat Ydat Zdat rval ['fmt']", mgls_tube, mglc_tube, false, 0},
	{L"var",L"Create new 1D data and fill it in range",L"var Dat nx x1 [x2]", mgls_var, mglc_var, true, 3},
	{L"vect",L"Draw vector field",L"vect Udat Vdat ['fmt' zval kind]|Xdat Ydat Udat Vdat ['fmt' zval kind]|Udat Vdat Wdat ['fmt' kind]|Xdat Ydat Zdat Udat Vdat Wdat ['fmt' kind]", mgls_vect, mglc_vect, false, 0},
	{L"vectc",L"Draw vector field with coloring",L"vectc Udat Vdat ['fmt' zval]|Xdat Ydat Udat Vdat ['fmt' zval]|Udat Vdat Wdat ['fmt']|Xdat Ydat Zdat Udat Vdat Wdat ['fmt']", mgls_vectc, mglc_vectc, false, 0},
	{L"vectl",L"Draw vector field with different lengths",L"vectl Udat Vdat ['fmt' zval]|Xdat Ydat Udat Vdat ['fmt' zval]|Udat Vdat Wdat ['fmt']|Xdat Ydat Zdat Udat Vdat Wdat ['fmt']", mgls_vect, mglc_vect, false, 0},
	{L"write",L"Write current image to graphical file",L"write 'fname' [solid]", mgls_write, mglc_write, false, 5},
	{L"xlabel",L"Draw label for x-axis",L"xlabel 'txt' [pos size shift]", mgls_xlabel, mglc_xlabel, false, 1},
	{L"xrange",L"Set range for x-axis",L"xrange Dat [add] | x1 x2", mgls_xrange, mglc_xrange, false, 2},
	{L"xtick",L"Set ticks for x-axis",L"xtick dx [sx tx] | 'tmpl'", mgls_xtick, mglc_xtick, false, 2},
	{L"ylabel",L"Draw label for y-axis",L"ylabel 'txt' [pos size shift]", mgls_ylabel, mglc_ylabel, false, 1},
	{L"yrange",L"Set range for y-axis",L"yrange Dat [add] | y1 y2", mgls_yrange, mglc_yrange, false, 2},
	{L"ytick",L"Set ticks for y-axis",L"ytick dy [sy ty] | 'tmpl'", mgls_ytick, mglc_ytick, false, 2},
	{L"zlabel",L"Draw label for z-axis",L"zlabel 'txt' [pos size shift]", mgls_zlabel, mglc_zlabel, false, 1},
	{L"zoom",L"Zoom in/out plot",L"zoom x1 y1 x2 y2", mgls_zoom, mglc_zoom, false, 5},
	{L"zrange",L"Set range for z-axis",L"yrange Dat [add] | z1 z2", mgls_zrange, mglc_zrange, false, 2},
	{L"ztick",L"Set ticks for z-axis",L"ztick dz [sz tz] | 'tmpl'", mgls_ztick, mglc_ztick, false, 2},
{L"",0,0,0,0,0,0}};
//-----------------------------------------------------------------------------
