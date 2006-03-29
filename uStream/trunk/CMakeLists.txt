# Include definitions ******************************************
# **************************************************************

INCLUDE( build.definition )
INCLUDE( build/Macros.cmake )



# First stage **************************************************
# **************************************************************

# init the output
MESSAGE( " " )
MESSAGE( "===============================================" )
MESSAGE( "               The DigiXound Project           " )
MESSAGE( "                   -- uStream --           " )
MESSAGE( "                  ${_PKG_VERSION}          " )
MESSAGE( "===============================================" )
MESSAGE( " " )

PROJECT(uStream)

# System type
MESSAGE( "-- System is: ${CMAKE_SYSTEM}" )

# Processor type
MESSAGE( "-- Processor is: ${CMAKE_SYSTEM_PROCESSOR}" )

# Generic requirements
CMAKE_MINIMUM_REQUIRED(VERSION 2.2.0)
MESSAGE( "-- Checking for Cmake >= 2.2.0 ............ ${CMAKE_CACHE_MAJOR_VERSION}.${CMAKE_CACHE_MINOR_VERSION}.${CMAKE_CACHE_RELEASE_VERSION}")

# Init config.h
FILE( WRITE config.h "// =====================================\n" )
FILE( APPEND config.h "// File generated automatically by cmake\n" )
FILE( APPEND config.h "// =====================================\n" )
FILE( APPEND config.h "\n" )
IF (DEBUG)
	WRITE_TO_CONFIG_H( "DEBUG" 1 )
	SET(CMAKE_BUILD_TYPE "Debug")
ELSE (DEBUG)
	SET(CMAKE_BUILD_TYPE "Release")
ENDIF (DEBUG)
WRITE_TO_CONFIG_H( "PACKAGE" \"${PACKAGE}\" )
WRITE_TO_CONFIG_H( "PACKAGE_BUGREPORT" \"${PACKAGE_BUGREPORT}\" )
WRITE_TO_CONFIG_H( "PACKAGE_STRING" \"${PACKAGE_STRING}\" )
WRITE_TO_CONFIG_H( "PACKAGE_VERSION" \"${PACKAGE_VERSION}\" )
WRITE_TO_CONFIG_H( "VERSION" \"${_PKG_VERSION}\" )

# HAVE_CONFIG_H seems to spawn some problems for us...
# ADD_DEFINITIONS( -DHAVE_CONFIG_H )
# Skip messages for depracated include usage...
#ADD_DEFINITIONS( -Wno-deprecated )

# Define code stage ********************************************
# **************************************************************
# SUBDIRS(PREORDER external)
# Main code
AUX_SOURCE_DIRECTORY(uStream/src USTREAM_SRC)
AUX_SOURCE_DIRECTORY(uOsUtil/src/core USTREAM_SRC)
INCLUDE_DIRECTORIES(uStream/include)
INCLUDE_DIRECTORIES(uOsUtil/include/core)
IF (UNIX)
	INCLUDE_DIRECTORIES(uOsUtil/include/linux)
	AUX_SOURCE_DIRECTORY(uOsUtil/src/core_linux USTREAM_SRC)
	ADD_DEFINITIONS( -D_LINUX )
ENDIF (UNIX)
IF (WIN32)
	INCLUDE_DIRECTORIES(uOsUtil/include/win32)
	AUX_SOURCE_DIRECTORY(uOsUtil/src/core_win32 USTREAM_SRC)
	ADD_DEFINITIONS( -D_WIN32 )
ENDIF (WIN32)
ADD_LIBRARY(uStream SHARED ${USTREAM_SRC} config.h)


# Install directives *******************************************
# **************************************************************
# Set path
SET(CMAKE_INSTALL_PREFIX ${PREFIX})
# # Search for headers
# IF (UNIX)
# 	INCLUDE_DIRECTORIES(${_PKG_SOURCE_DIR}/uOsUtil/include/linux)
# 	INCLUDE_DIRECTORIES(${_PKG_SOURCE_DIR}/uOsUtil/)
# 	AUX_SOURCE_DIRECTORY(uOsUtil/src/core_linux USTREAM_SRC)
# ENDIF (UNIX)
# IF (WIN32)
# 	INCLUDE_DIRECTORIES(${_PKG_SOURCE_DIR}/uOsUtil/include/win32)
# 	INCLUDE_DIRECTORIES(${_PKG_SOURCE_DIR}/uOsUtil/)
# 	AUX_SOURCE_DIRECTORY(uOsUtil/src/core_win32 USTREAM_SRC)
# ENDIF (WIN32)
# FILE(GLOB_RECURSE H_COMMON *.h)
# FILE(GLOB_RECURSE HXX_COMMON *.hxx)
# # Install rules
# INSTALL_FILES(/include/uStream FILES  ${H_COMMON} ${HXX_COMMON})
INSTALL_FILES(/lib/pkgconfig FILES uStream.pc)
# INSTALL_TARGETS(/lib uStream)


# Dependencies *************************************************
# **************************************************************

# Tests
INCLUDE(${CMAKE_ROOT}/Modules/TestForANSIForScope.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/TestForANSIStreamHeaders.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/TestForSTDNamespace.cmake)
# Find headers
FIND_HEADER_AND_SET_CONFIG_H_VALUE( sys/types.h HAVE_SYS_TYPES_H )

IF (UNIX)
	# Pkg-config check ==================================================
	INCLUDE( build/FindPkgconfig.cmake )
	# If Unix set, set up pkg-config facilities 
	IF (PKGCONFIG_FOUND MATCHES YES)
		FILE( READ uStream.pc.in PC_IN_FILE )
		SET( PC_FILE "prefix=${PREFIX}\n" ${PC_IN_FILE} )
		FILE( WRITE uStream.pc ${PC_FILE})
	ENDIF (PKGCONFIG_FOUND MATCHES YES)
ENDIF (UNIX)

