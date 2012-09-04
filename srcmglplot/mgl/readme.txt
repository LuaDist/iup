MathGL is a free library of fast C++ routines for the plotting of the data 
varied in one or more dimensions. MathGL has more than 40 general types 
of graphics for 1d, 2d and 3d data arrays. It can export graphics to bitmap 
and vector (EPS or SVG) files. It has OpenGL interface and can be used from 
console programs. It has functions for data handling and script MGL language 
for simplification of data plotting. Also it has several types of transparency 
and smoothed lightning, vector fonts and TeX-like symbol parsing, arbitrary 
curvilinear coordinate system and many over useful things. Finally it is 
platform independent and free (under GPL v.2.0 license).

MathGL can plot a wide range of graphics. It includes:
  * one-dimensional plots (Plot(), Area(), Bars(), Step(), Stem(), Torus(), 
    Chart(), Error(), Tube(), Mark(), Text()), 
  * two-dimensional plots (Mesh(), Surf(), Dens(), Cont(), ContF(), 
    Boxs(), Axial(), Belt()),
  * three-dimensional plots (Dens3(), Cont3(), ContF3(), Surf3(), 
    CloudQ(), CloudP()).
  * dual data plots: vector plot Vect() and VectC(), flow chart Flow(), 
    mapping chart Map(), surface or isosurface transpared (SurfA(), Surf3A()) 
	or colored (SurfC(), Surf3C()) by other data 
  * and so on. See class mglGraph for detail.

In fact, I created functions for drawing of all scientific plots that I know.
The list of plots is enlarging, so if you need some special type of plot then 
write me e-mail (balakin@appl.sci-nnov.ru) and it will appear in new version.

I tried to make plots as well-looking as possible: surface can be transparent 
and highlighted by several (up to 10) light sources. Most of drawing functions 
have 2 variants: simple one for the fast plotting of data, complex one for 
specifying of the exact position of plot (including parametric representation). 
Resulting image can be saved in bitmap (with the help of mglGraphZB, mglGraphGL) 
PNG, JPEG, TIFF format or in vector EPS or SVG format (with the help of 
mglGraphPS).

All text are drawn by vector font, that allows high scalability and portability. 
Text may contain commands for: some of TeX-like symbols, changing index (upper 
or lower indexes) and style of font inside text string (see mglFont). Texts of 
ticks are rotated with axis rotation. It is possible to create a legend of plot 
and put text in arbitrary position of plot.

Special class mglData is used for data incapsulation. Except safe creation and 
deletion of data arrays it includes functions for data processing (smoothing, 
differentiating, integrating, interpolating and so on) and reading of data 
files with automatic size determination. Class mglData can handle arrays with 
up to three dimensions (arrays which depend up to 3 independent indexes 
a[i,j,k]). Using an array with higher number of dimensions is not reasonable 
because I do not know how it can be plotted. Data filling and modifying may be 
fulfilled manually or by textual formulas.

Class mglFormula allows the fast evaluation of a textual mathematical 
expression. It is based on string precompilation to tree-like code at creation 
of class instance. At evaluation stage code performs only fast tree-walk and 
returns the value of the expression. Except changing data values, textual 
formulas are used for drawing in \emph{arbitrary} curvilinear coordinates. 
A set of such curvilinear coordinates is limited only by user imagination 
but not a fixed list like polar, parabolic, spherical and so on.

Installation instructions are provided in the manual (don't worry, it
is straightforward).

CONTACTS
--------
MathGL was written by Alexey Balakin. You can contact me at 
balakin@appl.sci-nnov.ru. The latest version of MathGL can be found at 
the sourceforge.net page (http://mathgl.sourceforge.net).
