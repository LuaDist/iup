# Copyright (C) 2012 LuaDist.
# Created by Peter Kapec <kapecp@gmail.com>
# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with LuaDist.
#	Note:
#		Searching headers and libraries is very simple and is NOT as powerful as scripts
#		distributed with CMake, because LuaDist defines directories to search for.
#		Everyone is encouraged to contact the author with improvements. Maybe this file
#		becomes part of CMake distribution sometimes.

# - Find IM
# Find the native IM headers and libraries.
#
# IM_INCLUDE_DIRS	- where to find im.h.
# IM_LIBRARIES	- List of libraries when using im.
# IM_FOUND	- True if IM found.

# Look for the header file.
FIND_PATH( IM_INCLUDE_DIR NAMES im.h )

# Look for the library.
FIND_LIBRARY( IM_LIBRARY NAMES im )

FIND_LIBRARY( IM_LIBRARY_LUA NAMES imlua )

FIND_LIBRARY( IM_LIBRARY_PROCESS NAMES im_process )
FIND_LIBRARY( IM_LIBRARY_PROCESS_OMP NAMES im_process_omp )
FIND_LIBRARY( IM_LIBRARY_FFTW NAMES im_fftw )
FIND_LIBRARY( IM_LIBRARY_AVI NAMES im_avi )
FIND_LIBRARY( IM_LIBRARY_CAPTURE NAMES im_capture )
FIND_LIBRARY( IM_LIBRARY_WMV NAMES im_wmv )
FIND_LIBRARY( IM_LIBRARY_JP2 NAMES im_jp2 )

# Handle the QUIETLY and REQUIRED arguments and set IM_FOUND to TRUE if all listed variables are TRUE.
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(IM DEFAULT_MSG IM_LIBRARY IM_INCLUDE_DIR)

# Copy the results to the output variables.
IF(IM_FOUND)
	SET(IM_LIBRARIES ${IM_LIBRARY})
	SET(IM_INCLUDE_DIRS ${IM_INCLUDE_DIR})
ENDIF(IM_FOUND)

MARK_AS_ADVANCED(IM_INCLUDE_DIRS IM_LIBRARIES)
