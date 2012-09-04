/***************************************************************************
 * mgl_parse.cpp is part of Math Graphic Library
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
#include <ctype.h>
#include <wchar.h>

#include "mgl/mgl_parse.h"
#include "mgl/mgl_c.h"
#include "mgl/mgl_f.h"

#ifdef WIN32
#include <io.h>
#ifdef __CYGWIN__
#include <unistd.h>
#else
wchar_t *wcstokw32(wchar_t *wcs, const wchar_t *delim)	{	return wcstok(wcs,delim);	}
#define wcstok(a,b,c) wcstokw32(a,b)
#endif
#else
#include <unistd.h>
#endif

wchar_t *mgl_wcsdup(const wchar_t *s);
//-----------------------------------------------------------------------------
mglFunc::mglFunc(long p, const wchar_t *f, mglFunc *prev)
{
	pos = p;	next = prev;
	register long i;
	for(i=0;(isalnum(f[i]) || f[i]=='_') && i<31;i++)	func[i]=f[i];
	func[i]=0;
	narg = wcstol(f+i+1,0,0);
	if(narg<0 || narg>9)	narg=0;
}
//-----------------------------------------------------------------------------
long mglParse::IsFunc(const wchar_t *name, int *na)
{
	mglFunc *f=func;
	while(f)
	{
		if(!wcscmp(name,f->func))
		{	if(na)	*na=f->narg;	return f->pos;	}
		f = f->next;
	}
	return 0;
}
//-----------------------------------------------------------------------------
void mglParse::ScanFunc(const wchar_t *line)
{
	static long num=0;
	if(!line)
	{	if(func)	delete func;
		num=0;	func=0;	return;	}
	num++;
	if(wcsncmp(line,L"func",4) || line[4]>' ')	return;
	register long i;
	for(i=4;line[i]<=' ' || line[i]=='\'';i++);
	func = new mglFunc(num-1, line+i, func);
}
//-----------------------------------------------------------------------------
void mgl_wcstrim(wchar_t *str)
{
	wchar_t *c = mgl_wcsdup(str);
	unsigned long n=wcslen(str);
	long k;
	for(k=0;k<long(wcslen(str));k++)		if(str[k]>' ')	break;
	wcscpy(c,&(str[k]));
	n = wcslen(c);
	for(k=n-1;k>=0;k--)	if(c[k]>' ')	break;
	c[k+1] = 0;
	wcscpy(str,c);	free(c);
}
//-----------------------------------------------------------------------------
wchar_t *mgl_str_copy(const char *s)
{
	wchar_t *str = new wchar_t[strlen(s)+1];
	register long i;
	for(i=0;i<int(strlen(s));i++)	str[i] = s[i];
	str[i] = 0;
	return str;
}
//-----------------------------------------------------------------------------
int mgl_cmd_cmp(const void *a, const void *b)
{
	const mglCommand *aa = (const mglCommand *)a;
	const mglCommand *bb = (const mglCommand *)b;
	return wcscmp(aa->name, bb->name);
}
//-----------------------------------------------------------------------------
bool check_for_name(const wchar_t *s)
{
	return !isalpha(s[0])||wcschr(s,'.')||wcschr(s,':')||wcschr(s,'(')||wcschr(s,')');
}
//-----------------------------------------------------------------------------
// It seems that standard wcstombs() have a bug. So, I replace by my own.
void mgl_wcstombs(char *dst, const wchar_t *src, int size)
{
	register int j;
	for(j=0;j<size-1 && src[j]!=0;j++)
		dst[j] = src[j]<0x7f ? src[j] : ' ';
	dst[j] = 0;
}
//-----------------------------------------------------------------------------
mglCommand *mglParse::FindCommand(const wchar_t *com, bool prog)
{
	mglCommand tst, *rts, *cmd = prog?Prg:Cmd;
	long i;
	for(i=0;cmd[i].name[0];i++);	// determine the number of symbols
	tst.name = com;
	rts = (mglCommand *) bsearch(&tst, cmd, i, sizeof(mglCommand), mgl_cmd_cmp);
	return rts;
}
//-----------------------------------------------------------------------------
// return values : 0 -- OK, 1 -- wrong arguments, 2 -- wrong command, 3 -- unclosed string
int mglParse::Exec(mglGraph *gr, const wchar_t *com, long n, mglArg *a, const wchar_t *var)
{
	int k[10], i;
	for(i=0;i<10;i++)	k[i] = i<n ? a[i].type + 1 : 0;
//	for(i=0;i<n;i++)	wcstombs(a[i].s, a[i].w, 1024);
	for(i=0;i<n;i++)		mgl_wcstombs(a[i].s, a[i].w, 1024);
	mglCommand *rts=FindCommand(com);
	if(!rts)	return 2;
	if(rts->create)
	{
		if(n<1 || check_for_name(var))	return 2;
		mglVar *v = AddVar(var);
		v->d.Create(1,1,1);
		a[0].type = 0;	a[0].d = &(v->d);
		wcscpy(a[0].w, var);	k[0] = 1;
		mgl_wcstombs(a[0].s, a[0].w, 1024);
//		wcstombs(a[0].s, a[0].w, 1024);
	}
	if(out)	rts->save(out, n, a, k);
	return rts->exec(gr, n, a, k);
}
//-----------------------------------------------------------------------------
void mglVar::MoveAfter(mglVar *var)
{
	if(prev)	prev->next = next;
	if(next)	next->prev = prev;
	prev = next = 0;
	if(var)
	{
		prev = var;
		next = var->next;
		var->next = this;
		if(func==0)	func = var->func;
	}
	if(next)	next->prev = this;
}
//-----------------------------------------------------------------------------
mglVar::~mglVar()
{
	if(func)	func(o);
	if(prev)	prev->next = next;
	if(next)	next->prev = prev;
}
//-----------------------------------------------------------------------------
void mglNum::MoveAfter(mglNum *var)
{
	if(prev)	prev->next = next;
	if(next)	next->prev = prev;
	prev = next = 0;
	if(var)
	{
		prev = var;
		next = var->next;
		var->next = this;
	}
	if(next)	next->prev = this;
}
//-----------------------------------------------------------------------------
mglNum::~mglNum()
{
	if(prev)	prev->next = next;
	if(next)	next->prev = prev;
}
//-----------------------------------------------------------------------------
mglParse::mglParse(bool setsize)
{
	memset(this,0,sizeof(mglParse));
	Cmd = mgls_base_cmd;
	AllowSetSize=setsize;
	Once = true;	parlen=320;
	op1 = new wchar_t[4096];	op2 = new wchar_t[4096];
	fval = new mglData[40];
}
//-----------------------------------------------------------------------------
mglParse::~mglParse()
{
	if(DataList)
	{
		while(DataList->next)	delete DataList->next;
		delete DataList;
	}
	for(long i=0;i<40;i++)	if(par[i])	delete []par[i];
	delete []op1;	delete []op2;	delete []fval;
	if(Cmd!=mgls_base_cmd)	delete []Cmd;
	if(fn_stack)	free(fn_stack);
}
//-----------------------------------------------------------------------------
bool mglParse::AddParam(int n, const char *str, bool isstr)
{
	unsigned s = strlen(str)+1;
	wchar_t *wcs = new wchar_t[s];
	mbstowcs(wcs,str,s);
	bool r = AddParam(n,wcs,isstr);
	delete []wcs;	return r;
}
//-----------------------------------------------------------------------------
int mglParse::Parse(mglGraph *gr, const char *str, long pos)
{
	unsigned s = strlen(str)+1;
	wchar_t *wcs = new wchar_t[s];
	mbstowcs(wcs,str,s);
	int r = Parse(gr,wcs,pos);
	delete []wcs;	return r;
}
//-----------------------------------------------------------------------------
mglVar *mglParse::AddVar(const char *str)
{
	unsigned s = strlen(str)+1;
	wchar_t *wcs = new wchar_t[s];
	mbstowcs(wcs,str,s);
	mglVar *v = AddVar(wcs);
	delete []wcs;
	return v;
}
//-----------------------------------------------------------------------------
mglVar *mglParse::FindVar(const char *str)
{
	unsigned s = strlen(str)+1;
	wchar_t *wcs = new wchar_t[s];
	mbstowcs(wcs,str,s);
	mglVar *v = FindVar(wcs);
	delete []wcs;
	return v;
}
//-----------------------------------------------------------------------------
mglNum *mglParse::AddNum(const char *str)
{
	unsigned s = strlen(str)+1;
	wchar_t *wcs = new wchar_t[s];
	mbstowcs(wcs,str,s);
	mglNum *v = AddNum(wcs);
	delete []wcs;
	return v;
}
//-----------------------------------------------------------------------------
mglNum *mglParse::FindNum(const char *str)
{
	unsigned s = strlen(str)+1;
	wchar_t *wcs = new wchar_t[s];
	mbstowcs(wcs,str,s);
	mglNum *v = FindNum(wcs);
	delete []wcs;
	return v;
}
//-----------------------------------------------------------------------------
bool mglParse::AddParam(int n, const wchar_t *str, bool isstr)
{
	if(n<0 || n>39 || wcschr(str,'$'))	return false;
	if(!isstr)	parlen += wcslen(str);
	if(par[n])	delete []par[n];
	par[n] = new wchar_t[wcslen(str)+1];
	wcscpy(par[n],str);
	return true;
}
//-----------------------------------------------------------------------------
mglVar *mglParse::FindVar(const wchar_t *name)
{
	mglVar *v=DataList;
	while(v)
	{
		if(!wcscmp(name, v->s))	return v;
		v = v->next;
	}
	return 0;
}
//-----------------------------------------------------------------------------
mglVar *mglParse::AddVar(const wchar_t *name)
{
	mglVar *v = FindVar(name);
	if(v)	return v;
	v = new mglVar;
	wcsncpy(v->s,name,256);
	if(DataList)	v->MoveAfter(DataList);
	else			DataList = v;
	return v;
}
//-----------------------------------------------------------------------------
mglNum *mglParse::FindNum(const wchar_t *name)
{
	mglNum *v=NumList;
	while(v)
	{
		if(!wcscmp(name, v->s))	return v;
		v = v->next;
	}
	return 0;
}
//-----------------------------------------------------------------------------
mglNum *mglParse::AddNum(const wchar_t *name)
{
	mglNum *v = FindNum(name);
	if(v)	return v;
	v = new mglNum;
	wcsncpy(v->s,name,256);
	if(NumList)	v->MoveAfter(NumList);
	else		NumList = v;
	return v;
}
//-----------------------------------------------------------------------------
int mglFindArg(const wchar_t *str)
{
	register long l=0,k=0,i;//,j,len=strlen(lst);
	for(i=0;i<long(wcslen(str));i++)
	{
		if(str[i]=='\'') l++;
		if(str[i]=='{') k++;
		if(str[i]=='}') k--;
		if(l%2==0 && k==0 && (str[i]=='#' || str[i]==';'))	return -i;
		if(l%2==0 && k==0 && (str[i]<=' '))	return i;
	}
	return 0;
}
//-----------------------------------------------------------------------------
void wcstrim_mgl(wchar_t *str);
void wcslwr_mgl(wchar_t *str);
//-----------------------------------------------------------------------------
bool mgls_suffix(const wchar_t *p, mglData *d, mreal *v)
{
	mreal x,y,z,k;
	bool ok=false;
	if(!wcscmp(p,L"a"))			{	ok = true;	*v = d->a[0];	}
	else if(!wcscmp(p,L"fst"))	{	ok = true;	int i=-1,j=-1,k=-1;	*v = d->Find(0,i,j,k);	}
	else if(!wcscmp(p,L"lst"))	{	ok = true;	int i=-1,j=-1,k=-1;	*v = d->Last(0,i,j,k);	}
	else if(!wcscmp(p,L"nx"))	{	ok = true;	*v=d->nx;	}
	else if(!wcscmp(p,L"ny"))	{	ok = true;	*v=d->ny;	}
	else if(!wcscmp(p,L"nz"))	{	ok = true;	*v=d->nz;	}
	else if(!wcscmp(p,L"max"))	{	ok = true;	*v=d->Maximal();	}
	else if(!wcscmp(p,L"min"))	{	ok = true;	*v=d->Minimal();	}
	else if(!wcscmp(p,L"sum"))	{	ok = true;	*v=d->Momentum('x',x,y);	}
	else if(!wcscmp(p,L"mx"))	{	ok = true;	d->Maximal(x,y,z);		*v=x/d->nx;	}
	else if(!wcscmp(p,L"my"))	{	ok = true;	d->Maximal(x,y,z);		*v=y/d->ny;	}
	else if(!wcscmp(p,L"mz"))	{	ok = true;	d->Maximal(x,y,z);		*v=z/d->nz;	}
	else if(!wcscmp(p,L"ax"))	{	ok = true;	d->Momentum('x',x,y);	*v=x/d->nx;	}
	else if(!wcscmp(p,L"ay"))	{	ok = true;	d->Momentum('y',x,y);	*v=x/d->ny;	}
	else if(!wcscmp(p,L"az"))	{	ok = true;	d->Momentum('z',x,y);	*v=x/d->nz;	}
	else if(!wcscmp(p,L"wx"))	{	ok = true;	d->Momentum('x',x,y);	*v=y/d->nx;	}
	else if(!wcscmp(p,L"wy"))	{	ok = true;	d->Momentum('y',x,y);	*v=y/d->ny;	}
	else if(!wcscmp(p,L"wz"))	{	ok = true;	d->Momentum('z',x,y);	*v=y/d->nz;	}
	else if(!wcscmp(p,L"sx"))	{	ok = true;	d->Momentum('x',x,y,z,k);	*v=z/d->nx;	}
	else if(!wcscmp(p,L"sy"))	{	ok = true;	d->Momentum('y',x,y,z,k);	*v=z/d->ny;	}
	else if(!wcscmp(p,L"sz"))	{	ok = true;	d->Momentum('z',x,y,z,k);	*v=z/d->nz;	}
	else if(!wcscmp(p,L"kx"))	{	ok = true;	d->Momentum('x',x,y,z,k);	*v=k/d->nx;	}
	else if(!wcscmp(p,L"ky"))	{	ok = true;	d->Momentum('y',x,y,z,k);	*v=k/d->ny;	}
	else if(!wcscmp(p,L"kz"))	{	ok = true;	d->Momentum('z',x,y,z,k);	*v=k/d->nz;	}
	else if(!wcscmp(p,L"aa"))	{	ok = true;	d->Momentum('a',x,y);	*v=x;	}
	else if(!wcscmp(p,L"wa"))	{	ok = true;	d->Momentum('a',x,y);	*v=y;	}
	else if(!wcscmp(p,L"sa"))	{	ok = true;	d->Momentum('a',x,y,z,k);*v=z;	}
	else if(!wcscmp(p,L"ka"))	{	ok = true;	d->Momentum('a',x,y,z,k);*v=k;	}
	return ok;
}
//-----------------------------------------------------------------------------
// convert substrings to arguments
mglData mglFormulaCalc(const wchar_t *string, mglParse *arg);
void mglParse::FillArg(mglGraph *gr, int k, wchar_t **arg, mglArg *a)
{
	register long n;
	for(n=1;n<k;n++)
	{
		mglVar *v, *u;
		mglNum *f;
		a[n-1].type = -1;
		if(arg[n][0]=='|')	a[n-1].type = -1;
		else if(arg[n][0]=='\'')
		{	// this is string (simplest case)
			a[n-1].type = 1;	arg[n][wcslen(arg[n])-1] = 0;
			if(wcslen(arg[n]+1)>=2048)	arg[n][2048]=0;
			wcscpy(a[n-1].w, arg[n]+1);
		}
		else if(arg[n][0]=='{')
		{	// this is temp data
			arg[n][wcslen(arg[n])-1] = 0;
			u=new mglVar;	u->temp=true;
			mglprintf(a[n-1].w,2048,L"/*%ls*/",arg[n]);
			if(DataList)	u->MoveAfter(DataList);
			else			DataList = u;
			a[n-1].type = 0;	a[n-1].d = &(u->d);
			ParseDat(gr, arg[n]+1, u->d);	// TODO: Check it
		}
		else if((v = FindVar(arg[n]))!=0)
		{	// have to find normal variables (for data creation)
			a[n-1].type = 0;		a[n-1].d = &(v->d);
			wcscpy(a[n-1].w, v->s);
		}
		else if((f = FindNum(arg[n]))!=0)
		{	// have to find normal variables (for data creation)
			a[n-1].type = 2;	a[n-1].d = 0;	a[n-1].v = f->d;
			wcscpy(a[n-1].w, f->s);
		}
		else
		{	// parse all numbers and formulas by unified way
			const mglData &d=mglFormulaCalc(arg[n], this);
//printf("arg: %ls, -- dat: %ld x %ld x %ld, -- val: %g\n",arg[n],d.nx,d.ny,d.nz,d.a[0]);
			if(d.nx*d.ny*d.nz==1)
			{	a[n-1].type = 2;	a[n-1].v = d.a[0];	}
			else
			{
				u=new mglVar;	u->temp=true;	u->d=d;
				mglprintf(a[n-1].w,2048,L"/*%ls*/",arg[n]);
				if(DataList)	u->MoveAfter(DataList);
				else			DataList = u;
				a[n-1].type = 0;	a[n-1].d = &(u->d);
			}
		}
	}
}
//-----------------------------------------------------------------------------
// return values: 0 - not found, 1 - OK, 2 - wrong arguments,
//				3 - wrong command, 4 - string too long
int mglParse::PreExec(mglGraph *, long k, wchar_t **arg, mglArg *a)
{
	long n=0;
	mglVar *v;
	if(!wcscmp(L"delete",arg[0]))	// parse command "delete"
	{
		if(k<2)	return 2;
		DeleteVar(arg[1]);	n=1;
	}
	else if(!wcscmp(L"list",arg[0]))	// parse command "list"
	{
		if(k<3 || check_for_name(arg[1]))	return 2;
		long nx=0, ny=1,j=0,i,t=0;
		char ch;
		for(i=2;i<k;i++)
		{
			ch = arg[i][0];
			if(a[i-1].type==1)	return 2;
			if(a[i-1].type==0)
			{
				if(t==1)	return 2;
				t=2;	nx++;
			}
			if(a[i-1].type==2)
			{
				if(t==2)	return 2;
				j++;	t=1;
			}
			if(ch=='|' && t==1)		{	nx = j>nx ? j:nx;	j=0;	ny++;	}
		}
		v = AddVar(arg[1]);		n=1;
		if(t==1)	nx = j>nx ? j:nx;
		if(t==1)	// list of numeric values
		{
			v->d.Create(nx,ny);
			j=t=0;
			for(i=2;i<k;i++)
			{
				if(arg[i][0]=='|')	{	t++;	j=0;	}
				else
				{	v->d.a[j+nx*t] = a[i-1].v;	j++;	}
			}
		}
		if(t==2)	// list of data
		{
			mglData *b = a[1].d;
			long nn = 0;
			if(b->nz>1)	return 2;
			if(b->ny>1)
			{
				v->d.Create(b->nx, b->ny, nx);
				nn = b->nx*b->ny;
				for(i=2,j=0;i<k;i++)
				{
					b = a[i-1].d;
					if(b==0 || nn!=b->nx*b->ny)	continue;
					memcpy(v->d.a+j*nn,b->a,nn*(b->nz)*sizeof(mreal));
					j+=b->nz;
				}
			}
			else
			{
				v->d.Create(b->nx, nx);
				nn = b->nx;
				for(i=2,j=0;i<k;i++)
				{
					b = a[i-1].d;
					if(b==0 || nn!=b->nx)	continue;
					memcpy(v->d.a+j*nn,b->a,nn*(b->ny)*sizeof(mreal));
					j+=b->ny;
				}
			}
		}
		n=1;
	}
	return n;
}
//-----------------------------------------------------------------------------
void mglParse::PutArg(const wchar_t *string, wchar_t *str, bool def)
{
	if(parlen>0)
	{
		wchar_t *sb = new wchar_t[wcslen(string)+1], *t;
		if(def)	str = str+10;
		register long n;
		while((t=wcschr(str,'$'))!=0)
		{
			wcscpy(sb,t+2);
			t[0]=0;
			n = t[1]-'0';	if(n>=0 && n<=9 && par[n])	wcscat(str,par[n]);
			n = t[1]-'a';	if(n>=0 && n<='z'-'a' && par[n+10])	wcscat(str,par[n+10]);
			if(t[1]=='$')	wcscat(str,L"\xffff");
			wcscat(str,sb);
		}
		delete []sb;
		while((t=wcschr(str,L'\xffff'))!=0)	*t='$';
	}
}
//-----------------------------------------------------------------------------
// return values: 0 - OK, 1 - wrong arguments, 2 - wrong command, 3 - string too long, 4 -- unclosed string
int mglParse::Parse(mglGraph *gr, const wchar_t *string, long pos)
{
	if(!gr || Stop)	return 0;
	wchar_t *str, *s = new wchar_t[wcslen(string)+1+40*parlen],*arg[1024],*t;
	str = s;
	wcscpy(str,string);
	wcstrim_mgl(str);
	long n,k=0,m=0;
	// try parse ':'
	for(n=k=0;n<long(wcslen(str));n++)
	{
		if(str[n]=='\'')	k++;
		if(str[n]=='(' && k%2==0)	m++;
		if(str[n]==')' && k%2==0)	m--;
		if(str[n]=='#' && k%2==0)	break;
		if(str[n]==':' && k%2==0 && m==0)
		{
			str[n]=0;
			int res=Parse(gr,str,pos);
			if(!res)	res=Parse(gr,str+n+1,pos);
			delete []s;	return res;
		}
	}
	// check if string is closed
	for(n=1,k=0;n<long(wcslen(str));n++)	if(str[n]=='\'' && str[n-1]!='\\')	k++;
	if(k%2)	return 4;	// strings is not closed
	// define parameters or start cycle
	if(!skip() && !wcsncmp(str,L"def",3) && (str[6]==' ' || str[6]=='\t'))
	{
		PutArg(string,str,true);
		if(!wcsncmp(str+3,L"ine",3))
		{
			wchar_t *ss=str+7;	wcstrim_mgl(ss);//	int res = 1;
			if(*ss=='$' && ss[1]>='0' && ss[1]<='9')
			{
				int n=ss[1]-'0';//	res = 0;
				ss +=2;	mgl_wcstrim(ss);
				AddParam(n, ss);
				delete []s;	return 0;
			}
			if(*ss=='$' && ss[1]>='a' && ss[1]<='z')
			{
				int n=ss[1]-'a';//	res = 0;
				ss +=2;	mgl_wcstrim(ss);
				AddParam(n+10, ss);
				delete []s;	return 0;
			}
		}
		if(!wcsncmp(str+3,L"num",3))
		{
			str += 7;	wcstrim_mgl(str);	int res = 1;
			if(*str=='$' && str[1]>='0' && str[1]<='9')
			{
				int n=str[1]-'0';	res = 0;
				str +=2;	mgl_wcstrim(str);
				const mglData &d=mglFormulaCalc(str, this);
				char *buf=new char[128];
				sprintf(buf,"%g",d.a[0]);
				AddParam(n, buf);
				delete []buf;
			}
			if(*str=='$' && str[1]>='a' && str[1]<='z')
			{
				int n=str[1]-'a';	res = 0;
				str +=2;	mgl_wcstrim(str);
				const mglData &d=mglFormulaCalc(str, this);
				char *buf=new char[128];
				sprintf(buf,"%g",d.a[0]);
				AddParam(n+10, buf);
				delete []buf;
			}
			delete []s;		return res;
		}
		if(!wcsncmp(str+3,L"chr",3))
		{
			str += 7;	wcstrim_mgl(str);	int res = 1;
			if(*str=='$' && str[1]>='0' && str[1]<='9')
			{
				int n=str[1]-'0';	res = 0;
				str +=2;	mgl_wcstrim(str);
				const mglData &d=mglFormulaCalc(str, this);
				wchar_t buf[2]={0,0};	buf[0] = wchar_t(d.a[0]);
				AddParam(n, buf);
			}
			if(*str=='$' && str[1]>='a' && str[1]<='z')
			{
				int n=str[1]-'a';	res = 0;
				str +=2;	mgl_wcstrim(str);
				const mglData &d=mglFormulaCalc(str, this);
				wchar_t buf[2]={0,0};	buf[0] = wchar_t(d.a[0]);
				AddParam(n+10, buf);
			}
			delete []s;		return res;
		}
		if(!wcsncmp(str+3,L"pal",3))
		{
			str += 7;	wcstrim_mgl(str);	int res = 1;
			if(*str=='$' && str[1]>='0' && str[1]<='9')
			{
				int n=str[1]-'0';	res = 0;
				str +=2;	mgl_wcstrim(str);
				const mglData &d=mglFormulaCalc(str, this);
				wchar_t buf[2]={0,0};
				buf[0] = gr->DefPal[int(d.a[0])%gr->NumPal];
				AddParam(n, buf);
			}
			if(*str=='$' && str[1]>='a' && str[1]<='z')
			{
				int n=str[1]-'a';	res = 0;
				str +=2;	mgl_wcstrim(str);
				const mglData &d=mglFormulaCalc(str, this);
				wchar_t buf[2]={0,0};
				buf[0] = gr->DefPal[int(d.a[0])%gr->NumPal];
				AddParam(n+10, buf);
			}
			delete []s;		return res;
		}
	}
	if(!skip() && !wcsncmp(str,L"for",3) && (str[3]==' ' || str[3]=='\t'))
	{
		t = str+4;
		while(*t && isspace(*t))	t++;
		// if command have format 'for $N ...' then change it to 'for N ...'
		if(*t=='$' && t[1]>='0' && t[1]<='9')	*t = ' ';
		if(*t=='$' && t[1]>='a' && t[1]<='z')	*t = ' ';
	}
	// parse arguments (parameters $1, ..., $9)
	PutArg (string,str,false);	wcstrim_mgl(str);

	for(k=0;k<1024;k++)	// parse string to substrings (by spaces)
	{
		n = mglFindArg(str);
		if(n<1)
		{
			if(str[-n]==';')	ProcOpt(gr,str-n);
			if(n<0)	str[-n]=0;
			break;
		}
		str[n]=0;	arg[k] = str;//	k++;
		str = str+n+1;	wcstrim_mgl(str);
	}
	// try to find last argument
	if(str[0]!=0 && str[0]!='#' && str[0]!=';')	{	arg[k] = str;	k++;	}
	if(k<1) n =0;
	else
	{
		// fill arguments by its values
		mglArg *a = new mglArg[k];
		FillArg(gr, k, arg, a);
		// execute first special (program-flow-control) commands
		if(!skip() && !wcscmp(arg[0],L"stop"))
		{	Stop = true;	delete []s;	delete []a;	return 0;	}
		if(!wcscmp(arg[0],L"func"))	{	delete []s;	delete []a;	return 0;	}
		n = FlowExec(gr, arg[0],k-1,a);
		if(n)		{	delete []s;	delete []a;	return n-1;	}
		if(skip())	{	delete []s;	delete []a;	return 0;	}
		if(!wcscmp(arg[0],L"define"))
		{
			if(k==3)
			{
				mglNum *v=AddNum(arg[1]);
				mglData d=mglFormulaCalc(arg[2],this);
				v->d = d.a[0];
			}
			delete []s;	delete []a;	return k==3?0:1;
		}
		if(!wcscmp(arg[0],L"call"))
		{
			n = 1;
			if(a[0].type==1)
			{
				int na=0;
				mgl_wcstombs(a[0].s, a[0].w, 1024);		n=-IsFunc(a[0].w,&na);
				if(n && k!=na+2)	
				{
					if(gr->Message)
						sprintf(gr->Message,"Bad arguments for %ls: %ld instead of %d\n",
								a[0].w,k-2,na);
					n = 1;
				}
				else if(n)
				{
					if(!fn_stack)
					{	fn_num = 100;
						fn_stack = (mglFnStack*)malloc(fn_num*sizeof(mglFnStack));	}
					if(fn_pos >= fn_num)
					{	fn_num+= 100;
						fn_stack = (mglFnStack*)realloc(fn_stack,fn_num*sizeof(mglFnStack));	}
					memcpy(fn_stack[fn_pos].par,par+1,9*sizeof(wchar_t*));
					memset(par+1,0,9*sizeof(wchar_t*));
					for(int i=1;i<k-1;i++)	AddParam(i,arg[i+1]);
					fn_stack[fn_pos].pos = pos;	fn_pos++;	n--;
				}
				else
				{
					FILE *fp = fopen(a[0].s,"rt");
					if(fp)	{	Execute(gr,fp);	fclose(fp);	}
					else	n=1;
				}
			}
			delete []s;	delete []a;	return n;
		}
		if(!wcscmp(arg[0],L"for"))
		{
			n = 1;
			char ch = arg[1][0];
			int r = ch-'0';
			if(ch>='a' && ch<='z')	r = 10+ch-'a';
//			int r = int(a[0].v);
			if(arg[1][1]==0 && (r>=0 || r<=39))
			{
				if(a[1].type==0)
				{
					n=0;		fval[r] = *(a[1].d);
					fval[r].nx *= fval[r].ny*fval[r].nz;
				}
				else if(a[1].type==2 && a[2].type==2 && a[2].v>a[1].v)
				{
					mreal step = a[3].type==2?a[3].v:1;
					int m = int(step>0 ? (a[2].v-a[1].v)/step : 0);
					if(m>0)
					{
						n=0;	fval[r].Create(m+1);
						for(int ii=0;ii<m+1;ii++)
							fval[r].a[ii] = a[1].v + step*ii;
					}
				}
				if(n==0)
				{
					for(int i=39;i>0;i--)
					{	for_stack[i]=for_stack[i-1];	if_for[i]=if_for[i-1];	}
					for_stack[0] = r+1;		fval[r].nz = pos;	if_for[0]=if_pos;
					wchar_t buf[32];		mglprintf(buf,32,L"%g",fval[r].a[0]);
					AddParam(r, buf,true);	fval[r].ny = 1;
				}
			}
			delete []s;	delete []a;	return n;
		}
		// alocate new arrays and execute the command itself
		n = PreExec(gr, k, arg, a);
		if(n>0)	n--;
		else if(!wcscmp(L"setsize",arg[0]) && !AllowSetSize)	n = 2;
		else	n = Exec(gr, arg[0],k-1,a, arg[1]);
		delete []a;
	}
	mglVar *v = DataList, *u;
	while(v)	// remove temporary data arrays
	{
		u = v->next;
		if(v->temp)	{	if(DataList==v)	DataList = v->next;		delete v;	}
		v = u;
	}
	// restore plot settings if it was changed
	ProcOpt(gr,0);
	delete []s;
	return n;
}
//-----------------------------------------------------------------------------
// return values: 0 - OK, 1 - wrong arguments, 2 - wrong command, 3 - string too long, 4 -- unclosed string
int mglParse::ParseDat(mglGraph *gr, const wchar_t *string, mglData &res)
{
	wchar_t *str, *s = new wchar_t[wcslen(string)+1+parlen],*arg[32];
	str = s;
	wcscpy(str,string);	wcstrim_mgl(str);
	long n,k=0;
	for(k=0;k<32;k++)	// parse string to substrings (by spaces)
	{
		n = mglFindArg(str);
		if(n<1)
		{
			if(str[-n]==';')	ProcOpt(gr,str-n);
			if(n<0)	str[-n]=0;
			break;
		}
		str[n]=0;	arg[k] = str;//	k++;
		str = str+n+1;	wcstrim_mgl(str);
	}
	// try to find last argument
	if(str[0]!=0 && str[0]!='#' && str[0]!=';')	{	arg[k] = str;	k++;	}
	if(k<1) n =0;
	else
	{
		// fill arguments by its values
		mglArg *a = new mglArg[k+1];
		FillArg(gr, k, arg, a+1);	a[0].type=0;	a[0].d=&res;
		// alocate new arrays and execute the command itself
		int kk[10], i;
		for(i=0;i<10;i++)	kk[i] = i<=k ? a[i].type + 1 : 0;
		for(i=0;i<=k;i++)	mgl_wcstombs(a[i].s, a[i].w, 1024);
		mglCommand *rts=FindCommand(arg[0]);
		if(!rts || !rts->create)	return 2;
		if(out)	rts->save(out, k, a, kk);
		n = rts->exec(gr, k, a, kk);
		delete []a;
	}
	// restore plot settings if it was changed
	ProcOpt(gr,0);	delete []s;
	return n;
}
//-----------------------------------------------------------------------------
int mglParse::FlowExec(mglGraph *, const wchar_t *com, long m, mglArg *a)
{
	int n=-1;
	if(!ifskip() && !wcscmp(com,L"once"))
	{
		if(a[0].type==2)
		{
			n = 0;
			if(a[0].v)
			{	Skip = !Once;	if(out)	mglprintf(out,1024,L"if(!once)\t{\tonce = true;");	}
			else
			{	Skip = Once = false;	if(out)	mglprintf(out,1024,L"}");	}
		}
		else n = 1;
	}
	else if(!Skip && !wcscmp(com,L"if"))
	{
		int cond;
		if(a[0].type==2)
		{
			n = 0;	cond = (a[0].v!=0)?3:0;
			if(out)	mglprintf(out,1024,L"if(%g!=0)\t{", a[0].v);
		}
		else if(a[0].type==0)
		{
			n = 0;	mgl_wcstombs(a[1].s, a[1].w, 1024);
			cond = a[0].d->FindAny((m>1 && a[1].type==1) ? a[1].s:"u")?3:0;
			if(out)	mglprintf(out,1024,L"if(%ls.FindAny(\"%s\"))\t{",
				a[0].w, (m>1 && a[1].type==1) ? a[1].s:"u");
		}
		else n = 1;
		if(n==0)
		{	if_stack[if_pos] = cond;	if_pos = if_pos<39 ? if_pos+1 : 39;	}
	}
	else if(!Skip && !wcscmp(com,L"endif"))
	{
		if_pos = if_pos>0 ? if_pos-1 : 0;
		n = 0;	if(out)	mglprintf(out,1024,L"}");
	}
	else if(!Skip && !wcscmp(com,L"else"))
	{
		if(if_pos>0)
		{
			n=0; if_stack[if_pos-1] = (if_stack[if_pos-1]&2)?2:3;
			if(out)	mglprintf(out,1024,L"}\telse\t{");
		}
		else n = 1;
	}
	else if(!Skip && !wcscmp(com,L"elseif"))
	{
		int cond;
		if(if_pos<1 || m<1)	n = 1;
		else if(if_stack[if_pos-1]&2)	{	n = 0;	cond = 2;	}
		else if(a[0].type==2)
		{
			n = 0;	cond = (a[0].v!=0)?3:0;
			if(out)	mglprintf(out,1024,L"else if(%g!=0)\t{", a[0].v);
		}
		else if(a[0].type==0)
		{
			n = 0;	mgl_wcstombs(a[1].s, a[1].w, 1024);
			cond = a[0].d->FindAny((m>1 && a[1].type==1) ? a[1].s:"u")?3:0;
			if(out)	mglprintf(out,1024,L"else if(%ls.FindAny(\"%s\"))\t{",
							a[0].w, (m>1 && a[1].type==1) ? a[1].s:"u");
		}
		else n = 1;
		if(n==0)	if_stack[if_pos-1] = cond;
	}
	else if(!ifskip() && !Skip && !wcscmp(com,L"break"))
	{
		if(if_pos==if_for[0])	if_pos = if_pos>0 ? if_pos-1 : 0;
		if(out)	mglprintf(out,1024,L"break;");	for_br = true;
	}
	else if(!skip() && !wcscmp(com, L"return"))	// parse command "delete"
	{
		if(!fn_pos)	return 2;
		fn_pos--;	n = -fn_stack[fn_pos].pos-1;
		for(int i=1;i<10;i++)	if(par[i])	delete []par[i];
		memcpy(par+1,fn_stack[fn_pos].par,9*sizeof(wchar_t*));
	}
	else if(!ifskip() && !Skip && !wcscmp(com,L"next"))
	{
		if(if_pos==if_for[0])	if_pos = if_pos>0 ? if_pos-1 : 0;
		int r = for_stack[0]-1;
		n = for_stack[0] ? 0:1;
		if(for_stack[0])
		{
			if(fval[r].ny<fval[r].nx && !for_br)
			{
				wchar_t buf[32];		mglprintf(buf,32,L"%g",fval[r].a[fval[r].ny]);
				AddParam(r, buf,true);	fval[r].ny += 1;
				n = -fval[r].nz-1;
			}
			else
			{
				for(int i=0;i<39;i++)
				{	for_stack[i]=for_stack[i+1];	if_for[i]=if_for[i+1];	}
				for_stack[39] = 0;	for_br=false;
			}
		}
		if(out)	mglprintf(out,1024,L"}");
	}
	else if(!ifskip() && !Skip && !wcscmp(com,L"continue"))
	{
		if(if_pos==if_for[0])	if_pos = if_pos>0 ? if_pos-1 : 0;
		int r = for_stack[0]-1;
		n = for_stack[0] ? 0:1;
		if(for_stack[0])
		{
			if(fval[r].ny<fval[r].nx)
			{
				wchar_t buf[32];		mglprintf(buf,32,L"%g",fval[r].a[fval[r].ny]);
				AddParam(r, buf,true);	fval[r].ny += 1;
				n = -fval[r].nz-1;
			}
			else	for_br = true;
		}
		if(out)	mglprintf(out,1024,L"continue;");
	}
	return n+1;
}
//-----------------------------------------------------------------------------
void mgl_error_print(int line, int r, mglGraph *gr)
{
	if(r==0)	printf("%s\n",gr->Message);
	if(r==1)	printf("Wrong argument(s) in line %d\n", line);
	if(r==2)	printf("Wrong command in line %d\n", line);
	if(r==3)	printf("String too long in line %d\n", line);
	if(r==4)	printf("Unbalanced ' in line %d\n", line);
	if(gr->Message)	gr->Message[0]=0;
}
#include <string>
void mglParse::Execute(mglGraph *gr, FILE *fp, bool print)
{
	if(gr==0 || fp==0)	return;
	std::wstring str;
	while(!feof(fp))	str.push_back(fgetwc(fp));
	Execute(gr,str.c_str(),print?mgl_error_print:NULL);
/*	fseek(fp,0,SEEK_END);	// get file size
	long len=ftell(fp),cur=0;
	fseek(fp,0,SEEK_SET);	// restore back
	wchar_t *str=new wchar_t[len+1];
	if(len>0)	for(cur=0;cur<len;cur++)
	{
		str[cur] = fgetwc(fp);
		if(cur==0 && str[cur]==wchar_t(-1))	cur--;	// Shaytan, but it works!!!
	}
	str[len]=0;
	Execute(gr,str,print?mgl_error_print:NULL);
	delete []str;*/
}
//-----------------------------------------------------------------------------
void mglParse::Execute(mglGraph *gr, int n, const wchar_t **text, void (*error)(int line, int kind, mglGraph *gr))
{
	if(gr==0 || n<1 || text==0)	return;
	long i, r;
	for_br=Skip=false;	if_pos=fn_pos=0;	ScanFunc(0);
	for(i=0;i<n;i++)	ScanFunc(text[i]);
	for(i=0;i<n;i++)
	{
//printf("line %ld : %ls\n",i,text[i]);
		r = Parse(gr,text[i],i+1);
		if(r<0)	{	i = -r-2;	continue;	}
		if(error)
		{
			if(r>0)	error(i+1, r, gr);
			if(gr->Message && gr->Message[0])	error(i,0,gr);
		}
	}
}
//-----------------------------------------------------------------------------
void mglParse::Execute(mglGraph *gr, const wchar_t *text, void (*error)(int line, int kind, mglGraph *gr))
{
	unsigned s = wcslen(text)+1;
	wchar_t *wcs = new wchar_t[s];
	const wchar_t **str;
	register unsigned i, n=1;
	for(i=0;i<s;i++)	if(text[i]=='\n')	n++;
	str = (const wchar_t **)malloc(n*sizeof(wchar_t *));
	memcpy(wcs, text, s*sizeof(wchar_t));
	str[0] = wcs;	n=1;
	for(i=0;i<s;i++)	if(text[i]=='\n')
	{	wcs[i]=0;	str[n] = wcs+i+1;	n++;	}
	Execute(gr, n, str, error);
	delete []wcs;	free(str);
}
//-----------------------------------------------------------------------------
void mglParse::Execute(mglGraph *gr, const char *text, void (*error)(int line, int kind, mglGraph *gr))
{
	unsigned s = strlen(text)+1;
	wchar_t *wcs = new wchar_t[s];
	mbstowcs(wcs,text,s);
	Execute(gr, wcs, error);
	delete []wcs;
}
//-----------------------------------------------------------------------------
int mglParse::Export(wchar_t cpp_out[1024], mglGraph *gr, const wchar_t *str)
{
	*op1 = *op2 = 0;
	out = cpp_out;
	int res = Parse(gr, str);
	out = 0;
	return res;
}
//-----------------------------------------------------------------------------
void mglParse::ProcOpt(mglGraph *gr, wchar_t *str)
{
	wchar_t buf[256]=L"";
	if(str==0)
	{
		if(opt[0])
		{
			gr->Min.x = val[0];	gr->Max.x = val[1];	gr->RecalcBorder();
			if(out)	mglprintf(buf,256,L"\tgr->Min.x = tx1;\tgr->Max.x = tx2;}");
			wcscat(op2,buf);
		}
		if(opt[1])
		{
			gr->Min.y = val[2];	gr->Max.y = val[3];	gr->RecalcBorder();
			if(out)	mglprintf(buf,256,L"\tgr->Min.y = ty1;\tgr->Max.y = ty2;}");
			wcscat(op2,buf);
		}
		if(opt[2])
		{
			gr->Min.z = val[4];	gr->Max.z = val[5];	gr->RecalcBorder();
			if(out)	mglprintf(buf,256,L"\tgr->Min.z = tz1;\tgr->Max.z = tz2;}");
			wcscat(op2,buf);
		}
		if(opt[3])
		{
			gr->Cmin = val[6];	gr->Cmax = val[7];
			if(out)	mglprintf(buf,256,L"\tgr->Cmin = tc1;\tgr->Cmax = tc2;}");
			wcscat(op2,buf);
		}
		if(opt[4])
		{
			gr->Ambient(val[8]);
			if(out)	mglprintf(buf,256,L"\tgr->Ambient(tam);}");
			wcscat(op2,buf);
		}
		if(opt[5])
		{
			gr->Cut = val[10]!=0;
			if(out)	mglprintf(buf,256,L"\tgr->Cut = tct;}");
			wcscat(op2,buf);
		}
		if(opt[6])
		{
			gr->AlphaDef = val[12];
			if(out)	mglprintf(buf,256,L"\tgr->AlphaDef = tad;}");
			wcscat(op2,buf);
		}
		if(opt[7])
		{
			gr->MeshNum  = int(val[14]);
			if(out)	mglprintf(buf,256,L"\tgr->MeshNum = tmn;}");
			wcscat(op2,buf);
		}
		if(opt[8])
		{
			gr->FontSize = val[16];
			if(out)	mglprintf(buf,256,L"\tgr->FontSize = tfs;}");
			wcscat(op2,buf);
		}
		if(opt[9])
		{
			gr->MarkSize = val[17];
			if(out)	mglprintf(buf,256,L"\tgr->MarkSize = tms;}");
			wcscat(op2,buf);
		}
		if(opt[10])
		{
			gr->AddLegend(leg, gr->last_style);
			if(out)	mglprintf(buf,256,L"\tgr->AddLegend(\"%s\", \"%s\");", leg, gr->last_style);
			wcscat(op2,buf);
		}
		for(long i=0;i<16;i++)	opt[i]=false;
	}
	else
	{
		wchar_t *q=str,*s,*a,*b,*c;
		long n;
		wcstrim_mgl(q);		q++;
		// NOTE: not consider '#' inside legend entry !!!
		s=wcschr(q,'#');	if(s)	*s=0;
		while(q && *q)
		{
			// NOTE: not consider ';' inside legend entry !!!
			s=q;	q=wcschr(s,';');
			if(q)	{	*q=0;	q++;	}
			wcstrim_mgl(s);		a=s;
			n=mglFindArg(s);	if(n>0)	{	s[n]=0;		s=s+n+1;	}
			wcstrim_mgl(a);		b=s;
			n=mglFindArg(s);	if(n>0)	{	s[n]=0;		s=s+n+1;	}
			wcstrim_mgl(b);

			mglData bb,ss;
			bb = mglFormulaCalc(b, this);
			mreal ff = bb.a[0];
			if(!wcscmp(a+1,L"range"))
			{
				n=mglFindArg(s);	c=s;
				if(n>0)	{	s[n]=0;		s=s+n+1;	}
				wcstrim_mgl(c);
				ss = mglFormulaCalc(c, this);
				if(a[0]=='x')
				{
					val[0] = gr->Min.x;		val[1] = gr->Max.x;		opt[0]=true;
					gr->Min.x = bb.a[0];	gr->Max.x = ss.a[0];
					if(out)	mglprintf(buf,256,L"{mreal tx1=gr->Min.x, tx2=gr->Max.x;\tgr->Min.x=%g;\tgr->Max.x=%g;", wcstod(b,0), wcstod(s,0));
					wcscat(op1,buf);
				}
				else if(a[0]=='y')
				{
					val[2] = gr->Min.y;		val[3] = gr->Max.y;		opt[1]=true;
					gr->Min.y = bb.a[0];	gr->Max.y = ss.a[0];
					if(out)	mglprintf(buf,256,L"{mreal ty1=gr->Min.y, ty2=gr->Max.y;\tgr->Min.y=%g;\tgr->Max.y=%g;", wcstod(b,0), wcstod(s,0));
					wcscat(op1,buf);
				}
				else if(a[0]=='z')
				{
					val[4] = gr->Min.z;		val[5] = gr->Max.z;		opt[2]=true;
					gr->Min.z = bb.a[0];	gr->Max.z = ss.a[0];
					if(out)	mglprintf(buf,256,L"{mreal tz1=gr->Min.z, tz2=gr->Max.z;\tgr->Min.z=%g;\tgr->Max.z=%g;", wcstod(b,0), wcstod(s,0));
					wcscat(op1,buf);
				}
				else if(a[0]=='c')
				{
					val[6] = gr->Cmin;		val[7] = gr->Cmax;		opt[3]=true;
					gr->Cmin = bb.a[0];		gr->Cmax = ss.a[0];
					if(out)	mglprintf(buf,256,L"{mreal tc1=gr->Cmin, tc2=gr->Cmax;\tgr->Cmin=%g;\tgr->Cmax=%g;", wcstod(b,0), wcstod(s,0));
					wcscat(op1,buf);
				}
			}
			else if(!wcscmp(a,L"cut"))
			{
				val[10]= gr->Cut;	opt[5]=true;
				gr->Cut = (ff!=0 || !wcsncmp(s,L"on",2));
				if(out)	mglprintf(buf,256,L"{bool tct=gr->Cut;\tgr->Cut=%s;", gr->Cut?"true":"false");
				wcscat(op1,buf);
			}
			else if(!wcscmp(a,L"meshnum"))
			{
				val[14]= gr->MeshNum;	opt[7]=true;	gr->MeshNum = int(ff);
				if(out)	mglprintf(buf,256,L"{int tmn=gr->MeshNum;\tgr->MeshNum=%d;", gr->MeshNum);
				wcscat(op1,buf);
			}
			else if(!wcscmp(a,L"alphadef") || !wcscmp(a,L"alpha"))
			{
				val[12]= gr->AlphaDef;	opt[6]=true;	gr->AlphaDef = ff;
				if(out)	mglprintf(buf,256,L"{mreal tad=gr->AlphaDef;\tgr->AlphaDef=%g;", ff);
				wcscat(op1,buf);
			}
			else if(!wcscmp(a,L"fontsize"))
			{
				val[16]= gr->FontSize;	opt[8]=true;
				gr->FontSize = ff>0 ? ff : -ff*gr->FontSize;
				if(out)	mglprintf(buf,256,L"{mreal tfs=gr->FontSize;\tgr->FontSize=%g;", gr->FontSize);
				wcscat(op1,buf);
			}
			else if(!wcscmp(a,L"ambient"))
			{
				val[8] = gr->AmbBr;		opt[4]=true;	gr->Ambient(ff);
				if(out)	mglprintf(buf,256,L"{mreal tam=gr->AmbBr;\tgr->Ambient(%g);", ff);
				wcscat(op1,buf);
			}
			else if(!wcscmp(a,L"marksize"))
			{
				val[17]= gr->MarkSize;	opt[9]=true;	gr->MarkSize = ff/50;
				if(out)	mglprintf(buf,256,L"{mreal tad=gr->MarkSize;\tgr->MarkSize=%g/50;", ff);
				wcscat(op1,buf);
			}
			else if(!wcscmp(a,L"legend"))	// TODO: parsing of string tail (other options???)
			{
				b = wcschr(s+1,'\'');	if(b)	*b=0;
				opt[10]=true;	wcscpy(leg,s+1);
			}
		}
	}
}
//-----------------------------------------------------------------------------
void mglParse::DeleteVar(mglVar *v)
{
	if(!v)	return;
	if(DataList==v)	DataList = v->next;
	delete v;
}
//-----------------------------------------------------------------------------
void mglParse::DeleteVar(const char *name)
{
	mglVar *v = FindVar(name);
	DeleteVar(v);
}
//-----------------------------------------------------------------------------
void mglParse::DeleteVar(const wchar_t *name)
{
	mglVar *v = FindVar(name);
	DeleteVar(v);
}
//-----------------------------------------------------------------------------
void mglParse::AddCommand(mglCommand *cmd, int mc)
{
	int i, mp;
	if(mc<1)
	{	for(i=0;cmd[i].name[0];i++){};	mc = i;	}
	// determine the number of symbols
	for(i=0;Cmd[i].name[0];i++){};	mp = i;
	mglCommand *buf = new mglCommand[mp+mc+1];
	memcpy(buf, cmd, mc*sizeof(mglCommand));
	memcpy(buf+mc, Cmd, (mp+1)*sizeof(mglCommand));
	qsort(buf, mp+mc, sizeof(mglCommand), mgl_cmd_cmp);
	if(Cmd!=mgls_base_cmd)   delete []Cmd;
	Cmd = buf;
}
//-----------------------------------------------------------------------------
mglCommand mglParse::Prg[]={
	{L"break",L"Break for-cycle",L"break", 0, 0, 0, 5},
	{L"call",L"Execute script in external file",L"call 'name' [args]", 0, 0, 0, 5},
	{L"continue",L"Skip commands and iterate for-cycle again",L"continue", 0, 0, 0, 5},
	{L"defchr",L"Define parameter as character",L"defchr $N val", 0, 0, 0, 5},
	{L"define",L"Define constant or parameter",L"define $N sth | Var val", 0, 0, 0, 5},
	{L"defnum",L"Define parameter as numerical value",L"defnum $N val", 0, 0, 0, 5},
	{L"defpal",L"Define parameter as palette color",L"defpal $N val", 0, 0, 0, 5},
	{L"delete",L"Deleta variable",L"delete Dat", 0, 0, 0, 5},
	{L"else",L"Execute if condition is false",L"else", 0, 0, 0, 5},
	{L"elseif",L"Conditional operator",L"elseif val|Dat ['cond']", 0, 0, 0, 5},
	{L"endif",L"Finish if/else block",L"endif", 0, 0, 0, 5},
	{L"for",L"For cycle",L"for $N v1 v2 [dv] | $N Dat", 0, 0, 0, 5},
	{L"func",L"Start function definition and stop execution of main script",L"func 'name' [narg]", 0, 0, 0, 5},
	{L"if",L"Conditional operator",L"if val|Dat ['cond']", 0, 0, 0, 5},
	{L"next",L"Start next for-cycle iteration",L"next", 0, 0, 0, 5},
	{L"once",L"Start/close commands which should executed only once",L"once val", 0, 0, 0, 5},
	{L"return",L"Return from function",L"return", 0, 0, 0, 5},
	{L"stop",L"Stop execution",L"stop", 0, 0, 0, 5},
{L"",0,0, 0, 0, 0, 0}};
//-----------------------------------------------------------------------------
HMPR mgl_create_parser()		{	return new mglParse;	}
void mgl_delete_parser(HMPR p)	{	delete p;	}
void mgl_add_param(HMPR p, int id, const char *str)			{	p->AddParam(id,str);	}
void mgl_add_paramw(HMPR p, int id, const wchar_t *str)		{	p->AddParam(id,str);	}
HMDT mgl_add_var(HMPR p, const char *name)	{	mglVar *v=p->AddVar(name);	return &(v->d);	}
HMDT mgl_find_var(HMPR p, const char *name)	{	mglVar *v=p->FindVar(name);	return &(v->d);	}
int mgl_parse(HMGL gr, HMPR p, const char *str, int pos)	{	return p->Parse(gr, str, pos);	}
int mgl_parsew(HMGL gr, HMPR p, const wchar_t *str, int pos){	return p->Parse(gr, str, pos);	}
void mgl_parse_text(HMGL gr, HMPR p, const char *str)	{	p->Execute(gr, str);	}
void mgl_parsew_text(HMGL gr, HMPR p, const wchar_t *str){	p->Parse(gr, str);	}
void mgl_restore_once(HMPR p)	{	p->RestoreOnce();	}
void mgl_parser_allow_setsize(HMPR p, int a)	{	p->AllowSetSize = a;	}
void mgl_scan_func(HMPR p, const wchar_t *line)	{	p->ScanFunc(line);	}
//-----------------------------------------------------------------------------
uintptr_t mgl_create_parser_()	{	return uintptr_t(new mglParse);	}
void mgl_delete_parser_(uintptr_t* p)	{	delete _PR_;	}
void mgl_add_param_(uintptr_t* p, int *id, const char *str, int l)
{
	char *s=new char[l+1];		memcpy(s,str,l);	s[l]=0;
	_PR_->AddParam(*id, s);		delete []s;
}
/*===!!! NOTE !!! You must not delete obtained data arrays !!!===============*/
uintptr_t mgl_add_var_(uintptr_t* p, const char *name, int l)
{
	char *s=new char[l+1];		memcpy(s,name,l);	s[l]=0;
	mglVar *v=_PR_->AddVar(s);	delete []s;
	return uintptr_t(&(v->d));
}
/*===!!! NOTE !!! You must not delete obtained data arrays !!!===============*/
uintptr_t mgl_find_var_(uintptr_t* p, const char *name, int l)
{
	char *s=new char[l+1];		memcpy(s,name,l);	s[l]=0;
	mglVar *v=_PR_->FindVar(s);	delete []s;
	return uintptr_t(&(v->d));
}
int mgl_parse_(uintptr_t* gr, uintptr_t* p, const char *str, int *pos, int l)
{
	char *s=new char[l+1];		memcpy(s,str,l);	s[l]=0;
	int r = _PR_->Parse(_GR_, s, *pos);	delete []s;	return r;
}
void mgl_parse_text_(uintptr_t* gr, uintptr_t* p, const char *str, int l)
{
	char *s=new char[l+1];		memcpy(s,str,l);	s[l]=0;
	_PR_->Execute(_GR_, s);		delete []s;
}
void mgl_restore_once_(uintptr_t* p)	{	_PR_->RestoreOnce();	}
void mgl_parser_allow_setsize_(uintptr_t* p, int *a)
{	_PR_->AllowSetSize = *a;	}
//-----------------------------------------------------------------------------
