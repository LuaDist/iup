/***************************************************************************
 * mgl_parse.h is part of Math Graphic Library
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
#ifndef _MGL_PARSE_H_
#define _MGL_PARSE_H_
#include "mgl/mgl.h"
//-----------------------------------------------------------------------------
/// Structure for the command argument.
struct mglArg
{
	int type;		///< Type of argument {0-data,1-string,2-number}
	mglData *d;		///< Pointer to data (used if type==0)
	wchar_t w[2048];///< String with parameters
	char s[2048];	///< String with parameters
	mreal v;		///< Numerical value (used if type==2)
	mglArg()	{	type=-1;	d=0;	v=0;	s[0]=0;	w[0]=0;	};
};
//-----------------------------------------------------------------------------
/// Structure for MGL command
struct mglCommand
{
	const wchar_t *name;	///< Name of command
	const wchar_t *desc;	///< Short command description (can be NULL)
	const wchar_t *form;	///< Format of command arguments (can be NULL)
	/// Function for executing (plotting)
	int (*exec)(mglGraph *gr, long n, mglArg *a, int k[10]);
	/// Function for exporting in C++ (can be NULL)
	void (*save)(wchar_t out[1024], long n, mglArg *a, int k[10]);
	bool create;	///< Should parser create 1st the array automatically
	int type;		///< Type of command: 0 - data plot, 1 - other plot,
					///	2 - setup, 3 - data handle, 4 - subplot, 5 - program
};
extern mglCommand mgls_base_cmd[];
//-----------------------------------------------------------------------------
/// Structure for the mglData handling (see mglParse class).
struct mglVar
{
	mglData d;		///< Data itself
	wchar_t s[256];	///< Data name
	void *o;		///< Pointer to external object
	mglVar *next;	///< Pointer to next instance in list
	mglVar *prev;	///< Pointer to prev instance in list
	bool temp;		///< This temporar variable
	void (*func)(void *);	///< Callback function for destroying

	mglVar()	{	o=0;	s[0]=0;		next=prev=0;	func=0;	temp=false;	};
	virtual ~mglVar();
	/// Move variable after \a var and copy \a func from \a var (if \a func is 0)
	void MoveAfter(mglVar *var);
};
//-----------------------------------------------------------------------------
/// Structure for the number handling (see mglParse class).
struct mglNum
{
	mreal d;		///< Number itself
	wchar_t s[256];	///< Number name
	mglNum *next;	///< Pointer to next instance in list
	mglNum *prev;	///< Pointer to prev instance in list
	mglNum()	{	d=0;	s[0]=0;		next=prev=0;	};
	virtual ~mglNum();
	/// Move variable after \a var and copy \a func from \a var (if \a func is 0)
	void MoveAfter(mglNum *var);
};
//-----------------------------------------------------------------------------
/// Structure for function name and position.
struct mglFunc
{
	long pos;
	int narg;
	wchar_t func[32];
	mglFunc *next;
	mglFunc(long p, const wchar_t *f, mglFunc *prev=0);
	~mglFunc()	{	if(next)	delete next;	};
};
//-----------------------------------------------------------------------------
/// Structure for stack of functions and its arguments.
struct mglFnStack
{
	long pos;
	wchar_t *par[9];
};
//-----------------------------------------------------------------------------
/// Structure for the command argument (see mglGraph::Exec()).
class mglParse
{
friend void mgl_export(wchar_t *out, const wchar_t *in, int type);
public:
	static mglCommand Prg[];	///< List of program flow commands (parsed by itself)
	mglVar *DataList;	///< List with data and its names
	mglNum *NumList;	///< List with numbers and its names
	bool AllowSetSize;	///< Allow using setsize command
	bool Stop;			///< Stop command was. Flag prevent further execution
	mglCommand *Cmd;	///< Table of recognizable MGL commands (can be changed by user). It MUST be sorted by 'name' field !!!
	wchar_t *op1, *op2;	///< Buffer for options (are used if out!=NULL)

	mglParse(bool setsize=false);
	~mglParse();	///< NOTE: virtual destructor here give SegFault under GCC 4.5.2
	/// Find the command by the keyword name
	mglCommand *FindCommand(const wchar_t *name, bool prog=false);
	/// Parse and execute the string of MGL script
	int Parse(mglGraph *gr, const char *str, long pos=0);
	/// Parse and execute the unicode string of MGL script
	int Parse(mglGraph *gr, const wchar_t *str, long pos=0);
	/// Parse, execute and export it in C++ code the string of MGL script
	int Export(wchar_t cpp_out[1024], mglGraph *gr, const wchar_t *str);
	/// Execute MGL script file \a fname
	void Execute(mglGraph *gr, FILE *fp, bool print=false);
	/// Execute MGL script from array of lines
	void Execute(mglGraph *gr, int num, const wchar_t **text, void (*error)(int line, int kind, mglGraph *gr)=NULL);
	/// Execute MGL script text with '\n' separated lines
	void Execute(mglGraph *gr, const wchar_t *text, void (*error)(int line, int kind, mglGraph *gr)=NULL);
	/// Execute MGL script text with '\n' separated lines
	void Execute(mglGraph *gr, const char *text, void (*error)(int line, int kind, mglGraph *gr)=NULL);
	/// Scan for functions (use NULL for reset)
	void ScanFunc(const wchar_t *line);
	/// Check if name is function and return its address (or 0 if no)
	long IsFunc(const wchar_t *name, int *narg=0);
	/// Find variable or return 0 if absent
	mglVar *FindVar(const char *name);
	/// Find variable or return 0 if absent
	mglVar *FindVar(const wchar_t *name);
	/// Find variable or create it if absent
	mglVar *AddVar(const char *name);
	/// Find variable or create it if absent
	mglVar *AddVar(const wchar_t *name);
	/// Find number or return 0 if absent
	mglNum *FindNum(const char *name);
	/// Find number or return 0 if absent
	mglNum *FindNum(const wchar_t *name);
	/// Find number or create it if absent
	mglNum *AddNum(const char *name);
	/// Find number or create it if absent
	mglNum *AddNum(const wchar_t *name);
	/// Add string for parameter $1, ..., $9
	bool AddParam(int n, const char *str, bool isstr=true);
	/// Add unicode string for parameter $1, ..., $9
	bool AddParam(int n, const wchar_t *str, bool isstr=true);
	/// Add new MGL command(s) (last command MUST HAVE name[0]=0 !!!)
	void AddCommand(mglCommand *cmd, int num=0);
	/// Restore Once flag
	inline void RestoreOnce()	{	Once = true;	};
	/// Delete variable
	void DeleteVar(mglVar *v);
	/// Delete variable by its name
	void DeleteVar(const char *name);
	/// Delete variable by its name
	void DeleteVar(const wchar_t *name);
private:
	long parlen;		///< Length of parameter strings
	wchar_t *par[40];	///< Parameter for substituting instead of $1, ..., $9
	wchar_t *out;		///< Buffer for writing C++ code (if not NULL)
	wchar_t leg[128];	///< Buffer for legend
	bool opt[16];		///< Set on/off optional parameters for command argument
	mreal val[20];		///< Values for optional parameters
	bool Once;			///< Flag for command which should be executed only once
	bool Skip;			///< Flag that commands should be skiped (inside 'once' block)
	int if_stack[40];	///< Stack for if-else-endif commands
	int if_pos;			///< position in if_stack
	mglFunc *func;		///< function names and position
	mglFnStack *fn_stack;	///< function calls stack
	int fn_pos;			///< position in function stack
	int fn_num;			///< size of function stack
	int if_for[40];		///< position in if_stack for for-cycle start
	mglData *fval;		///< Values for for-cycle. Note that nx - number of elements, ny - next element, nz - address (or string number) of first cycle command
	int for_stack[40];	///< The order of for-variables
	int for_addr;		///< Flag for saving address in variable (for_addr-1)
	bool for_br;		///< Break is switched on (skip all comands until 'next')

	/// Parse command
	int Exec(mglGraph *gr, const wchar_t *com, long n, mglArg *a, const wchar_t *var);
	/// Fill arguments \a a from strings
	void FillArg(mglGraph *gr, int n, wchar_t **arg, mglArg *a);
	/// PreExecute stage -- parse some commands and create variables
	int PreExec(mglGraph *gr, long n, wchar_t **arg, mglArg *a);
	/// Process optional arguments
	void ProcOpt(mglGraph *gr, wchar_t *str);
	/// Execute program-flow control commands
	int FlowExec(mglGraph *gr, const wchar_t *com, long n, mglArg *a);
	/// Parse and execute the unicode string of MGL script
	int ParseDat(mglGraph *gr, const wchar_t *str, mglData &res);
	/// Parse $N arguments
	void PutArg(const wchar_t *string, wchar_t *str, bool def);
	/// In skip mode
	bool inline ifskip()	{	return (if_pos>0 && !(if_stack[if_pos-1]&1));	};
	bool inline skip()		{	return (Skip || ifskip() || for_br);	};
};
//-----------------------------------------------------------------------------
#endif
#endif
//-----------------------------------------------------------------------------

