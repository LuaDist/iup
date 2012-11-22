# Copyright (C) 2007-2012 LuaDist.
# Created by Peter Kapec <kapecp@gmail.com>
# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with LuaDist.
# Please note that the package source code is licensed under its own license.

PROJECT(iup C CXX)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
INCLUDE(cmake/dist.cmake)
include ( lua )

# Additional link libraries
	IF (WIN32)
		SET ( EXE_TYPE WIN32 )
	ENDIF (WIN32)

# Macros
	MACRO(ADD_PREFIX prefix rootlist)
	  SET(outlist )
	  FOREACH(root ${${rootlist}})
		LIST(APPEND outlist ${prefix}${root})
	  ENDFOREACH(root)
	  SET(${rootlist} ${outlist})
	ENDMACRO(ADD_PREFIX)

###########################################################################

# find headers & libs

	# Find OpenGL
	FIND_PACKAGE(OpenGL)
	IF(OPENGL_FOUND)
		INCLUDE_DIRECTORIES (${OPENGL_INCLUDE_DIR})

		# On Apple we need X11 OpenGL so FindOpenGL wont help us
		IF(APPLE)
			SET(OPENGL_INCLUDE_DIR /usr/X11/include )
			FIND_LIBRARY(OPENGL_GL_LIB NAMES gl PATHS /usr/X11/lib)
			FIND_LIBRARY(OPENGL_GLU_LIB NAMES glu PATHS /usr/X11/lib)
			SET(OPENGL_LIBRARIES ${OPENGL_GL_LIB} ${OPENGL_GLU_LIB})
		ENDIF()
	ENDIF()

#~ 	Ugly hack, because FindGTK2.cmake is buggy - it will fing GTK even when NOT installed....
#~ 	Still valid for CMake 2.8.8 ???
#~ 	IF(NOT WIN32)
		SET(GTK2_FIND_COMPONENTS gtk)
		FIND_PACKAGE(GTK2 COMPONENTS gtk)
#~ 	ENDIF()

	IF(GTK2_FOUND)
		MESSAGE("Found GTK:${GTK2_FOUND}")
		MESSAGE("GTK2_INCLUDE_DIRS:${GTK2_INCLUDE_DIRS}")
		MESSAGE("GTK2_LIBRARIES:${GTK2_LIBRARIES}")

		INCLUDE_DIRECTORIES(${GTK2_INCLUDE_DIRS})
		ADD_DEFINITIONS(-DGTK_DISABLE_DEPRECATED)
	ENDIF()

	# Find X11
	FIND_PACKAGE(X11)
	IF(X11_FOUND)
		INCLUDE_DIRECTORIES(${X11_INCLUDE_DIR})
		FIND_LIBRARY(X11_Xp_LIB NAMES Xp)

		MESSAGE(">>>> iup ${X11_LIBRARIES} | ${X11_Xpm_LIB} | ${X11_Xt_LIB} | ${X11_Xext_LIB} | ${X11_Xp_LIB}")

		# Find Motif
		FIND_PACKAGE(Motif)	# only on UNIX
		IF(MOTIF_FOUND)
			INCLUDE_DIRECTORIES(${MOTIF_INCLUDE_DIR})
		ENDIF()
	ENDIF()

	# Find CD and IM
	find_package(CD REQUIRED)
	find_package(IM REQUIRED)
	INCLUDE_DIRECTORIES (${CD_INCLUDE_DIRS} ${IM_INCLUDE_DIRS})

	# Find Lua
	find_package(Lua)

	# Find FreeType
	FIND_PACKAGE(Freetype REQUIRED)
	INCLUDE_DIRECTORIES (${FREETYPE_INCLUDE_DIRS})

###########################################################################

# include dirs
	INCLUDE_DIRECTORIES(include src)

# iup lib
	FILE(GLOB SRC_IUP_COMMON "src/*.c")

	FILE(GLOB SRC_IUP_WIN "src/win/*.c")
	FILE(GLOB SRC_IUP_WIN_WINDOWS "src/win/windows/*.c")

	FILE(GLOB SRC_IUP_GTK "src/gtk/*.c")
	FILE(GLOB SRC_IUP_GTK_MAC "src/gtk/mac/*.c")

	FILE(GLOB SRC_IUP_MOT "src/mot/*.c")
	FILE(GLOB SRC_IUP_MOT_UNIX "src/mot/unix/*.c")

	IF(WIN32)

		INCLUDE_DIRECTORIES(src/win)
		ADD_DEFINITIONS(-D_WIN32_WINNT=0x0501 -D_WIN32_IE=0x0501 -DWINVER=0x0501 -DNOTREEVIEW)
		ADD_DEFINITIONS(-DIUP_DLL)

#~ 		SET(SRC_IUP ${SRC_IUP_COMMON} ${SRC_IUP_WIN})
		SET(SRC_IUP ${SRC_IUP_COMMON} ${SRC_IUP_WIN} ${SRC_IUP_WIN_WINDOWS})

		IF(GTK2_FOUND)
			INCLUDE_DIRECTORIES(src/gtk)

			SET(SRC_IUP ${SRC_IUP_COMMON} ${SRC_IUP_GTK} ${SRC_IUP_WIN_WINDOWS})

			ADD_LIBRARY(iupgtk SHARED ${SRC_IUP} resources/iup.def resources/iup_dll.rc)
			TARGET_LINK_LIBRARIES(iupgtk ${GTK2_LIBRARIES} )

			install_library(iupgtk)
		ENDIF()

	ELSEIF(UNIX OR APPLE)
		IF(GTK2_FOUND)
			INCLUDE_DIRECTORIES(src/gtk)

			SET(SRC_IUP ${SRC_IUP_COMMON} ${SRC_IUP_GTK} )

			IF(APPLE)
				ADD_DEFINITIONS(-DGTK_MAC)
				SET(SRC_IUP ${SRC_IUP} ${SRC_IUP_GTK_MAC})
        
        FIND_LIBRARY(CG_LIB CoreGraphics)
        FIND_LIBRARY(CF_LIB CoreFoundation)
        FIND_LIBRARY(CS_LIB CoreServices)
        FIND_LIBRARY(CA_LIB Carbon)
        
        SET(APPLE_LIBRARIES ${CG_LIB} ${CF_LIB} ${CS_LIB} ${CA_LIB})
			ELSE()
				SET(SRC_IUP ${SRC_IUP} src/gtk/unix/iupgtk_help.c src/mot/unix/iupunix_info.c)
			ENDIF()

			IF(X11_FOUND AND MOTIF_FOUND)
				INCLUDE_DIRECTORIES(src/mot)

				SET(SRC_IUPMOTIF ${SRC_IUP_MOT} ${SRC_IUP_MOT_UNIX})
				ADD_LIBRARY(iupmotif SHARED ${SRC_IUPMOTIF} resources/iup.def etc/iup.rc)
				TARGET_LINK_LIBRARIES(iupmotif ${X11_LIBRARIES} ${X11_Xpm_LIB} ${X11_Xt_LIB} ${X11_Xext_LIB} ${X11_Xp_LIB} ${MOTIF_LIBRARIES})

				install_library(iupmotif)
			ENDIF()

		ELSEIF(X11_FOUND AND MOTIF_FOUND)
			INCLUDE_DIRECTORIES(src/mot)

			SET(SRC_IUP ${SRC_IUP_MOT} ${SRC_IUP_MOT_UNIX})
		ENDIF()
	ENDIF()

	ADD_LIBRARY(iup SHARED ${SRC_IUP} resources/iup.def etc/iup.rc)

	IF(WIN32)
		TARGET_LINK_LIBRARIES(iup ole32 comdlg32 comctl32 gdi32 )
	ELSEIF(UNIX OR APPLE)
		IF(GTK2_FOUND)
			TARGET_LINK_LIBRARIES(iup ${GTK2_LIBRARIES} ${APPLE_LIBRARIES})
		ELSEIF(X11_FOUND AND MOTIF_FOUND)
			#	TODO: Motif + X11
			#	link with "-liup -lXm -lXpm -lXmu -lXt -lXext -lX11 -lm"
			TARGET_LINK_LIBRARIES(iup ${X11_LIBRARIES} ${X11_Xpm_LIB} ${X11_Xt_LIB} ${X11_Xext_LIB} ${X11_Xp_LIB})
			TARGET_LINK_LIBRARIES(iup ${MOTIF_LIBRARIES})
		ENDIF()
	ENDIF()

	install_library(iup)

# iupgl
	IF(OPENGL_FOUND)
		INCLUDE_DIRECTORIES(srcgl)

		IF(WIN32)
			SET(SRC_IUP_GL srcgl/iup_glcanvas_win.c)
		ELSE()
			SET(SRC_IUP_GL srcgl/iup_glcanvas_x.c)
		ENDIF()

		ADD_LIBRARY(iupgl SHARED ${SRC_IUP_GL} resources/iupgl.def)

		TARGET_LINK_LIBRARIES( iupgl iup ${OPENGL_LIBRARIES} ${X11_LIBRARIES} )

		install_library(iupgl)
    
    # iup_mglplot lib
    find_package(Zlib REQUIRED)
    find_package(Fontconfig REQUIRED)
    
    include_directories (
    	srcmglplot/freetype
    	srcmglplot/ftgl
    	srcmglplot/mgl
    	srcmglplot
      ${FONTCONFIG_INCLUDE_DIR}
      ${ZLIB_INCLUDE_DIR}
    	)

    file ( GLOB FTFont "srcmglplot/ftgl/FTFont/*.cpp" )
    file ( GLOB FTGlyph "srcmglplot/ftgl/FTGlyph/*.cpp" )
    file ( GLOB FTGL "srcmglplot/ftgl/*.cpp" )
    file ( GLOB MGL "srcmglplot/mgl/*.cpp" )
    file ( GLOB MGLPLOT "srcmglplot/*.cpp")

    	if(MINGW)
    #~ 			SET_SOURCE_FILES_PROPERTIES(${MGL} PROPERTIES COMPILE_DEFINITIONS "FTGL_LIBRARY_STATIC;NO_PNG;NO_GSL;swprintf=_snwprintf")
    #~ 			ADD_DEFINITIONS(-Dswprintf=_snwprintf)
    	endif()

    set ( MGLPLOT_SRC ${FTFont} ${FTGlyph} ${FTGL} ${MGLPLOT} ${MGL})
    #~ 	set ( MGLPLOT_SRC ${FTFont} ${FTGlyph} ${FTGL} ${MGLPLOT})

    if ( APPLE )
    	set_source_files_properties ( ${MGLPLOT_SRC} PROPERTIES COMPILE_DEFINITIONS "USE_MACOS_OPENGL;FTGL_LIBRARY_STATIC;NO_PNG;NO_GSL" )
    #		set_target_properties ( ftgl PROPERTIES COMPILE_FLAGS "-framework OpenGL" )
    else ( )
    	set_source_files_properties ( ${MGLPLOT_SRC} PROPERTIES COMPILE_DEFINITIONS "FTGL_LIBRARY_STATIC;NO_PNG;NO_GSL" )
    endif ( )

    #~ 	set ( MGLPLOT_SRC ${MGLPLOT_SRC} ${MGL}  )

    ADD_LIBRARY(iup_mglplot SHARED ${MGLPLOT_SRC} resources/iup_mglplot.def)
    TARGET_LINK_LIBRARIES (iup_mglplot iup iupgl ${FREETYPE_LIBRARIES} ${FONTCONFIG_LIBRARIES} ${ZLIB_LIBRARIES})

    install_library(iup_mglplot)
    
	ENDIF()

# iupcontrols  lib
	INCLUDE_DIRECTORIES( srccontrols )

	FILE(GLOB_RECURSE SRC_IUPCONTROLS "srccontrols/*.c")

	SET_SOURCE_FILES_PROPERTIES(${SRC_IUPCONTROLS} PROPERTIES COMPILE_DEFINITIONS "IUP_DEFMATHFLOAT")

	ADD_LIBRARY(iupcontrols SHARED ${SRC_IUPCONTROLS} resources/iupcontrols.def)
	TARGET_LINK_LIBRARIES (iupcontrols  iupcd iup ${CD_LIBRARY} )

	IF(CYGWIN)
		SET_TARGET_PROPERTIES(iupcontrols PROPERTIES COMPILE_FLAGS "-DWIN32")
	ENDIF()

	install_library(iupcontrols)

# iup_pplot lib
	INCLUDE_DIRECTORIES( srcpplot )

	FILE(GLOB SRC_IUPPPLOT "srcpplot/*.cpp")

	SET_SOURCE_FILES_PROPERTIES(${SRC_IUPPPLOT} PROPERTIES COMPILE_DEFINITIONS "_IUP_PPLOT_;CD_NO_OLD_INTERFACE")

	ADD_LIBRARY(iup_pplot SHARED ${SRC_IUPPPLOT} resources/iup_pplot.def)
	TARGET_LINK_LIBRARIES (iup_pplot iup  iupcd ${CD_LIBRARY})

	install_library(iup_pplot)

# iupimglib lib
	FILE(GLOB IUPIMGLIB_SRC "srcimglib/*.c")
	FILE(GLOB IUPIMGLIB_SRC_WIN "srcimglib/windows/*.c")
	FILE(GLOB IUPIMGLIB_SRC_NONWIN "srcimglib/non-windows/*.c")
	IF(WIN32)
		LIST(APPEND IUPIMGLIB_SRC ${IUPIMGLIB_SRC_WIN})
	ELSE()
		LIST(APPEND IUPIMGLIB_SRC ${IUPIMGLIB_SRC_NONWIN})
	ENDIF()


	#~ 	[2DO] - won't link under VC9 (waited ~15 min with 100% CPU load)
	# IF(NOT MSVC)
		ADD_LIBRARY(iupimglib SHARED  ${IUPIMGLIB_SRC} resources/iupimglib.def)
		TARGET_LINK_LIBRARIES (iupimglib iup)

		install_library(iupimglib)
	# ENDIF()

# iupole
	# ??? Works only on Visual C++ or Borland C++ (acording to documentation)
	# ??? How do they then distribute precompiled  "libiupluaole51.a" for MinGW???
	#		- they create the MinGW library from a MSVC library with dlltool ;-(
	# MinGW & Cygwin -> they miss "LPOLEINPLACEOBJECT" that is used in tOleHandler.h
	IF(MSVC OR MSVC80 OR MSVC_IDE)
		INCLUDE_DIRECTORIES(srcole)

		FILE(GLOB SRC_IUP_OLE "srcole/*.cpp")

		ADD_LIBRARY(iupole SHARED ${SRC_IUP_OLE} resources/iupole.def)
		TARGET_LINK_LIBRARIES( iupole iup )
		#IF(CYGWIN)
		#	SET_TARGET_PROPERTIES(iupole PROPERTIES COMPILE_FLAGS "-UWIN32")
		#ENDIF()

		ADD_DEPENDENCIES(iupole iup)

		install_library(iupole)
	ENDIF()

# iupweb lib
	IF(WIN32 OR GTK2_FOUND)

		INCLUDE_DIRECTORIES(srcweb)

		SET(IUPWEB_SRC  srcweb/iup_webbrowser.c)

		IF(WIN32)
			SET(IUPWEB_SRC  ${IUPWEB_SRC} srcweb/iupwin_webbrowser.cpp)
			ADD_DEFINITIONS(-D_MBCS)

			FIND_LIBRARY( COMSUPPW_LIBRARY NAMES comsuppw)
			IF(COMSUPPW_LIBRARY)
				ADD_LIBRARY(iupweb SHARED ${IUPWEB_SRC})
				TARGET_LINK_LIBRARIES( iupweb iup iupole  ${COMSUPPW_LIBRARY})

				install_library(iupweb)
			ENDIF()

		ELSEIF(GTK2_FOUND)
			SET(IUPWEB_SRC ${IUPWEB_SRC} srcweb/iupgtk_webbrowser.c)

			# 2DO: FindWebKit.cmake
			FIND_PATH(WEBKIT_INCLUDE_DIR webkit/webkit.h PATHS /usr/include/webkit-1.0)
			FIND_PATH(SOUP_INCLUDE_DIR libsoup/soup.h PATHS /usr/include/libsoup-2.4)
			FIND_LIBRARY( WEBKIT_LIBRARY NAMES webkit-1.0)
			FIND_LIBRARY( SOUP_LIBRARY NAMES soup-2.4)

			IF(WEBKIT_INCLUDE_DIR AND WEBKIT_LIBRARY AND SOUP_INCLUDE_DIR )
				INCLUDE_DIRECTORIES(${WEBKIT_INCLUDE_DIR} ${SOUP_INCLUDE_DIR})

				ADD_LIBRARY(iupweb SHARED ${IUPWEB_SRC})
				TARGET_LINK_LIBRARIES( iupweb iup ${WEBKIT_LIBRARY})

				install_library(iupweb)
			ENDIF()
		ENDIF()
	ENDIF()

# iuptuio lib
	INCLUDE_DIRECTORIES(srctuio/tuio srctuio/oscpack)

	FILE(GLOB TUIO "srctuio/tuio/*.cpp")
	FILE(GLOB OSC "srctuio/oscpack/osc/*.cpp")
	FILE(GLOB OSC_WIN32 "srctuio/oscpack/ip/win32/*.cpp")
	FILE(GLOB OSC_POSIX "srctuio/oscpack/ip/posix/*.cpp")

	SET(TUIPLIB_SRC ${TUIO} ${OSC} srctuio/iup_tuio.cpp )

	IF(WIN32)
		SET(TUIPLIB_SRC ${TUIPLIB_SRC} ${OSC_WIN32})
	ELSE()
		SET(TUIPLIB_SRC ${TUIPLIB_SRC} ${OSC_POSIX})
	ENDIF()

	INCLUDE(TestBigEndian)
	TEST_BIG_ENDIAN(IS_BIG_ENDIAN)
	IF(IS_BIG_ENDIAN)
		SET_SOURCE_FILES_PROPERTIES(${TUIPLIB_SRC} PROPERTIES COMPILE_DEFINITIONS "OSC_HOST_BIG_ENDIAN")
	ELSE()
		SET_SOURCE_FILES_PROPERTIES(${TUIPLIB_SRC} PROPERTIES COMPILE_DEFINITIONS "OSC_HOST_LITTLE_ENDIAN")
	ENDIF()

	ADD_LIBRARY(iuptuio SHARED ${TUIPLIB_SRC})
	TARGET_LINK_LIBRARIES( iuptuio iup)
	IF(WIN32)
		TARGET_LINK_LIBRARIES( iuptuio ws2_32 winmm)
	ENDIF()

	install_library(iuptuio)

# iupim lib
	INCLUDE_DIRECTORIES( srcimglib )

	ADD_LIBRARY(iupim SHARED srcim/iup_im.c resources/iupim.def)
	TARGET_LINK_LIBRARIES (iupim iup  ${IM_LIBRARY})

	install_library(iupim)

# iupcd lib
	SET_SOURCE_FILES_PROPERTIES(srccd/iup_cd.c PROPERTIES COMPILE_DEFINITIONS "CD_NO_OLD_INTERFACE")

	ADD_LIBRARY(iupcd SHARED srccd/iup_cd.c resources/iupcd.def)
	TARGET_LINK_LIBRARIES(iupcd iup ${CD_LIBRARIES} )

	install_library(iupcd)

###########################################################################

# iupview
	IF(OPENGL_FOUND)
		SET_SOURCE_FILES_PROPERTIES(srcview/iup_view.c PROPERTIES COMPILE_DEFINITIONS "USE_IM")
		ADD_EXECUTABLE (iupview ${EXE_TYPE} srcview/iup_view.c etc/iup.rc)
		TARGET_LINK_LIBRARIES (iupview
			iupim
			iupgl iupimglib iupcontrols iup
			${IM_LIBRARY}
			${OPENGL_LIBRARIES}
			)
		install_executable(iupview)
	ENDIF()

# ledc
	INCLUDE_DIRECTORIES (srcledc)
	FILE(GLOB SRC_LEDC "srcledc/*.c")
	ADD_EXECUTABLE (ledc ${EXE_TYPE} ${SRC_LEDC} resources/iuplua5.rc)

	install_executable(ledc)

###########################################################################


if(LUA_FOUND)

	INCLUDE_DIRECTORIES (${LUA_INCLUDE_DIR}
		srclua5
		srclua5/libiuplua/il
		srclua5/libiuplua/lh
		srclua5/modules/il
		srclua5/modules/lh
		srcconsole/lh)

	# use *.lh files
	add_definitions(-DIUPLUA_USELH)

####

# iuplua
	FILE(GLOB SRC_IUPLUA "srclua5/libiuplua/*.c")
	FILE(GLOB SRC_IUPLUA_IL "srclua5/libiuplua/il/*.c")

	SET_SOURCE_FILES_PROPERTIES(${SRC_IUPLUA} ${SRC_IUPLUA_IL} PROPERTIES COMPILE_DEFINITIONS "IUPLUA_USELH")

	ADD_LIBRARY(iuplua SHARED ${SRC_IUPLUA} ${SRC_IUPLUA_IL})
	TARGET_LINK_LIBRARIES(iuplua ${LUA_LIBRARY} iup)

	install_library(iuplua)

# iuplua module
	install_lua_module ( iuplua_module srclua5/iuplua/iuplua_module.c resources/iuplua.def LINK  iuplua )
	set_target_properties ( iuplua_module PROPERTIES OUTPUT_NAME iuplua )

####

# iupluacontrols
	FILE(GLOB SRC_IUPLUACONTROLS "srclua5/modules/controls/*.c")
	install_lua_module ( iupluacontrols
		${SRC_IUPLUACONTROLS}
		srclua5/modules/il/il_dial.c
		srclua5/modules/il/il_gauge.c
		srclua5/modules/il/il_colorbrowser.c
		srclua5/modules/il/il_colorbar.c
		srclua5/modules/il/il_matrix.c
		srclua5/modules/il/il_cells.c
		resources/iupluacontrols.def
		LINK  iuplua  iupcontrols ${CD_LIBRARY_LUA})

# iupluagl
	IF(TARGET iupgl)
		install_lua_module ( iupluagl
			srclua5/modules/gl/iuplua_glcanvas.c
			srclua5/modules/il/il_glcanvas.c
			resources/iupluagl.def
			LINK  iuplua  iupgl   ${OPENGL_LIBRARIES})
	ENDIF()

# iupluapplot
	install_lua_module ( iuplua_pplot
		srclua5/modules/pplot/iuplua_pplot.c
		srclua5/modules/il/il_pplot.c
		resources/iuplua_pplot.def
		LINK  iuplua  ${CD_LIBRARY_LUA} ${LUA_LIBRARY} iup_pplot)

# iupluamglplot
  IF(TARGET iup_mglplot)
    install_lua_module ( iuplua_mglplot
  		srclua5/modules/pplot/iuplua_mglplot.c
  		srclua5/modules/il/il_mglplot.c
  		resources/iuplua_mglplot.def
    LINK    iup_mglplot  iuplua )
  ENDIF()

# iupluaimglib
	install_lua_module ( iupluaimglib
		srclua5/modules/imglib/iuplua_imglib.c
		resources/iupluaimglib.def
		LINK  iuplua iupimglib  iupim)

# iupluaweb
	IF(TARGET iupweb )
		install_lua_module ( iupluaweb srclua5/modules/il/il_webbrowser.c LINK  iuplua iupweb )
	ENDIF()

# iupluatuio
	install_lua_module ( iupluatuio srclua5/modules/il/il_tuioclient.c LINK  iuplua iuptuio )

# iupluaole
	IF(TARGET iupole)
		install_lua_module ( iupluaole51
			srclua5/modules/ole/olecontrol.c
			resources/iupluaole5.def
			LINK  iuplua  iupole )
	ENDIF()

# iupluaim
	install_lua_module ( iupluaim
		srclua5/modules/im/iuplua_im.c
		resources/iupluaim.def
	LINK  iuplua  iupim ${IM_LIBRARY_LUA} )

# iupluacd
	install_lua_module ( iupluacd
		srclua5/modules/cd/iuplua_cd.c
		resources/iupluacd.def
		LINK  iuplua ${CD_LIBRARY_LUA} iupcd  ${CD_LIBRARY})

# iuplua executable
  FIND_PACKAGE(Readline)
  INCLUDE_DIRECTORIES(${READLINE_INCLUDE_DIRS})

	ADD_EXECUTABLE(iuplua_exe ${EXE_TYPE} srcconsole/iup_lua51.c resources/iuplua5.rc )
	SET_TARGET_PROPERTIES(iuplua_exe PROPERTIES OUTPUT_NAME "iuplua")
	TARGET_LINK_LIBRARIES(iuplua_exe  iuplua ${READLINE_LIBRARIES} )
#~ 	INSTALL (TARGETS iuplua_exe DESTINATION ${INSTALL_BIN})
	install_executable(iuplua_exe)

endif() # end of LUA_FOUND


###########################################################################


# bigtest app
	FILE(GLOB BIGTEST_SRC "test/*.c")
	FILE(GLOB BIGTEST_SRC_CONTROLS "test/iupcontrols/*.c")
	FILE(GLOB BIGTEST_SRC_CD "test/cd/*.c")
	FILE(GLOB BIGTEST_SRC_GL "test/gl/*.c")

	IF(CD_LIBRARY)
		SET(BIGTEST_SRC ${BIGTEST_SRC} ${BIGTEST_SRC_CD} )
	ENDIF()

	IF(OPENGL_FOUND)
		SET(BIGTEST_SRC ${BIGTEST_SRC} ${BIGTEST_SRC_GL})
	ENDIF()

	SET(BIGTEST_SRC ${BIGTEST_SRC}
		${BIGTEST_SRC_CONTROLS}
		test/pplot/pplot.c
		test/pplot/mglplot.c
		test/bigtest/bigtest.c
		)

	IF(OPENGL_FOUND)
		SET_SOURCE_FILES_PROPERTIES(${BIGTEST_SRC} PROPERTIES COMPILE_DEFINITIONS "BIG_TEST;PPLOT_TEST;MGLPLOT_TEST;USE_OPENGL")
	ELSEIF()
		SET_SOURCE_FILES_PROPERTIES(${BIGTEST_SRC} PROPERTIES COMPILE_DEFINITIONS "BIG_TEST;PPLOT_TEST;MGLPLOT_TEST")
	ENDIF()

	ADD_EXECUTABLE(iuptest ${EXE_TYPE} ${BIGTEST_SRC} test/iuptest.rc)
	TARGET_LINK_LIBRARIES (iuptest iupcontrols iup_pplot iup_mglplot iup)

	IF(CD_LIBRARY)
		TARGET_LINK_LIBRARIES (iuptest  ${CD_LIBRARY} ${CDGL_LIBRARY} )
	ENDIF()

	IF(OPENGL_FOUND)
		TARGET_LINK_LIBRARIES (iuptest iupgl)
	ENDIF()

	ADD_DEPENDENCIES(iuptest iup_pplot)
	INSTALL(TARGETS iuptest DESTINATION ${INSTALL_BIN} COMPONENT Example)


###########################################################################

# install headers
	install_header(include/ )

# install other
	install_data(COPYRIGHT README)

# install documentation
	install_doc ( doc/ )
