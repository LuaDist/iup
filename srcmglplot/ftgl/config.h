// GLUT
//#define HAVE_GL_GLUT_H

// M_PI and friends on VC
#define _USE_MATH_DEFINES

// quell spurious "'this': used in base member initializer list" warnings
#ifdef _MSC_VER
#pragma warning(disable: 4355)
#endif
