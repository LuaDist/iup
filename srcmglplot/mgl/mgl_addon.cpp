/***************************************************************************
 * mgl_addon.cpp is part of Math Graphic Library
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
#include <stdarg.h>
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
#include <string.h>
#include "mgl/mgl_addon.h"
//---------------------------------------------------------------------------
void mgl_strcls(char *str)
{
	unsigned len = strlen(str),i,n;
	char *tmp = new char[len];
	for(i=0;i<len;i++)
	{
		if(i<len-1 && str[i]==' ' && str[i+1]==' ')
			continue;
		tmp[i] = str[i];
	}
	for(n=0;n<strlen(tmp);n++)		if(tmp[n]!=' ')	break;
	for(i=strlen(tmp)-1;i>0;i--)	if(tmp[i]!=' ')	break;
	tmp[i+1]=0;	strcpy(str,&(tmp[n]));
	delete []tmp;
}
//---------------------------------------------------------------------------
int mgl_strpos(const char *str,char *fnd)
{
	const char *p=strstr(str,fnd);
	int res;
	if(p)	res = p-str;
	else	res = -1;
	return res;
}
//---------------------------------------------------------------------------
int mgl_strpos(const char *str,char ch)
{
	const char *p=strchr(str,ch);
	int res;
	if(p)	res = p-str;
	else	res = -1;
	return res;
}
//---------------------------------------------------------------------------
char *mgl_fgetstr(FILE *fp)
{
	static char s[256];
	do
	{
		if(!fgets(s,256,fp))	break;
		mgl_strtrim(s);
//		strlwr(s);
	} while(!feof(fp) && (s[0]==0 || s[0]=='%' || s[0]=='#'));
	return s;
}
//---------------------------------------------------------------------------
bool mgl_istrue(char ch)
{	return (ch=='1' || ch=='t' || ch=='+' || ch=='v');	}
//---------------------------------------------------------------------------
void mgl_test(const char *str, ...)
{
	char buf[256];
	va_list lst;
	va_start(lst,str);
	vsprintf(buf,str,lst);
	va_end(lst);
	printf("TEST: %s\n",buf);
	fflush(stdout);
}
//---------------------------------------------------------------------------
void mgl_info(const char *str, ...)
{
	char buf[256];
	va_list lst;
	va_start(lst,str);
	vsprintf(buf,str,lst);
	va_end(lst);
	printf("%s",buf);
	FILE *fp = fopen("info.txt","at");
	fprintf(fp,"%s",buf);
	fclose(fp);
}
//---------------------------------------------------------------------------
FILE *mgl_next_data(const char *fname,int p)
{
	char *s;
	int len;
	static int pos=0;
	static char path[256];

	if(p>0)	pos = p;
	if(fname==NULL)	return NULL;
	if(pos==0)	{	if(!getcwd(path,256))	return 0;	}	// remember ini dyrectory
	else		{	if(chdir(path)==-1)		return 0;	}

	// read the initial (common) date from "mbrs.ini"
	FILE *fp=fopen(fname,"rt");
	if(fp==NULL)	return NULL;
	fseek(fp,0,SEEK_END);
	len = ftell(fp);
	if(pos>=len)		 // no more data
	{	fclose(fp);	return NULL;	}
	fseek(fp,pos,SEEK_SET);
	//printf("pos 1 = %d\t",pos);
	do
	{
		s = mgl_fgetstr(fp);
		fflush(stdout);
		if(s[0]=='$' || s[1]=='$' || s[3]=='$')
		{	fclose(fp);	return NULL;	}
	} while(!feof(fp) && (s[0]!='-' || s[1]!='-' || s[3]!='-'));
	if(feof(fp))	// no more data
	{	fclose(fp);	return NULL;	}
	return fp;
}
//---------------------------------------------------------------------------
bool mglDifrGrid(dual *a,int n,dual q,int Border,dual *b,dual *d,int kk)
{
	register int i,k;
//	if(n<=0 || q>=0.5)	return false;
	dual adt = dual(0.,1.)*q;

	memcpy(b,a,n*sizeof(dual));
	for(k=kk;k>0;k--)	// 3 iterations
	{
		for(i=1;i<n-1;i++)
			d[i] = a[i] + adt*(b[i-1]+b[i+1]-2.*b[i])/double(k);
		memcpy(b,d,n*sizeof(dual));
		switch(Border)
		{
			case 0:		// zero at border
				b[0] = 0;		b[n-1] = 0;		break;
			case 1:		// constant at border
				b[0] = b[1];	b[n-1] = b[n-2];	break;
			case 2:		// linear at border
				b[0] = 2.*b[1]-b[2];
				b[n-1] = 2.*b[n-2]-b[n-3];
				break;
			case 3:		// square at border
				b[0] = b[3]+3.*(b[1]-b[2]);
				b[n-1] = b[n-4]+3.*(b[n-2]-b[n-3]);
				break;
			case -1:		// exponent at border
				b[0] = norm(b[2])<norm(b[1]) ? b[1] : b[1]*b[1]/b[2];
				b[n-1] = norm(b[n-3])<norm(b[n-2]) ? b[n-2] : b[n-2]*b[n-2]/b[n-3];
				break;
			case -2:		// gaussian at border
				b[0] = norm(b[2])<norm(b[1]) ? b[3] : pow(b[1]/b[2],3)*b[3];
				b[n-1] = norm(b[n-3])<norm(b[n-2]) ? b[n-4] : pow(b[n-2]/b[n-3],3)*b[n-4];
				break;
		}
	}
	memcpy(a,b,n*sizeof(dual));
	return true;
}
//----------------------------------------------------------------------------
bool mglDifrAxial(dual *a, int n, dual q, int Border,dual *b, dual *d, int kk, double di)
{
	register int i,k,ii = di<0 ? -int(floor(di)) : 0;
	dual adt = dual(0.,1.)*q;
	register double dd,ff= di==floor(di) ? 4. : 2.,gg;

	memcpy(b,a,n*sizeof(dual));
	for(k=kk;k>0;k--)	// kk iterations
	{
		d[ii] = a[ii] + adt*(b[ii+1]-b[ii])*(ff/k);
		for(i=ii+1;i<n-1;i++)
		{
			dd = i+di;
			dd = 1./(sqrt(dd*dd+1.)+dd);	// corrections for "axiality"
			gg = 1+dd*dd;
			d[i] = a[i] + adt*( b[i-1]*((gg-dd)/k) -
					b[i]*(gg*2./k) + b[i+1]*((gg+dd)/k) );
		}
		memcpy(b,d,n*sizeof(dual));
		switch(Border)
		{
			case 0:		// zero at border
				b[n-1] = 0;		break;
			case 1:		// constant at border
				b[n-1] = b[n-2];	break;
			case 2:		// linear at border
				b[n-1] = -b[n-3] + 2.*b[n-2];
				break;
			case 3:		// square at border
				b[n-1] = b[n-4] + 3.*(b[n-2]-b[n-3]);
				break;
			case -1:		// exponent at border
				b[n-1] = norm(b[n-3])<norm(b[n-2]) ? b[n-2] : b[n-2]*b[n-2]/b[n-3];
				break;
			case -2:		// gaussian at border
				b[n-1] = norm(b[n-3])<norm(b[n-2]) ? b[n-4] : pow(b[n-2]/b[n-3],3)*b[n-4];
				break;
		}
	}
	memcpy(a,b,n*sizeof(dual));
	return true;
}
//----------------------------------------------------------------------------
double mgl_gauss_rnd()
{
	double v1,v2;
	v1 = mgl_rnd();
	v2 = mgl_rnd();
	return v1!=0 ? sqrt(-2.*log(v1))*cos(2*M_PI*v2) : 0;
}
//----------------------------------------------------------------------------
void mgl_fft_freq(double *freq,unsigned nn)
{
	for(unsigned i=0;i<=nn/2;i++)
	{
		freq[i] = i;
		if(i>0) freq[nn-i] = -(double)(i);
	}
}
//----------------------------------------------------------------------------
