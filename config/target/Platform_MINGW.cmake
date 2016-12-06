set(PLATFORM MINGW)

# the name of the target operating system
set(CMAKE_SYSTEM_NAME Windows)

# Generic toolchain setup
file(TO_CMAKE_PATH $ENV{DEPOT} DEPOT)
include( ${DEPOT}/config/target/gnu.cmake )

set(MINGW_PREFIX $ENV{MINGW_PREFIX})
IF(NOT MINGW_PREFIX)
	set(MINGW_PREFIX i686-w64-mingw32)
ENDIF(NOT MINGW_PREFIX)


# Set RootFS
set( PLATFORM_ROOTFS "/opt/MinGW/" )

# which compilers to use for C and C++
set(CMAKE_C_COMPILER ${PLATFORM_ROOTFS}/usr/bin/${MINGW_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${PLATFORM_ROOTFS}/usr/bin/${MINGW_PREFIX}-g++)
set(CMAKE_RC_COMPILER ${PLATFORM_ROOTFS}/usr/bin/${MINGW_PREFIX}-windres)
set(STRIP_TOOL ${PLATFORM_ROOTFS}/usr/bin/${MINGW_PREFIX}-strip)

set(CMAKE_CXX_FLAGS "-D_WIN32_WINNT=0x0601 -mms-bitfields")
set(CMAKE_CXX_STANDARD_LIBRARIES "-lwsock32 -lws2_32 -liphlpapi -lpsapi ${CMAKE_CXX_STANDARD_LIBRARIES}")

# Where is the target environment
set(PLATFORM_FIND_ROOT_PATH ${PLATFORM_ROOTFS}/usr/${MINGW_PREFIX}/sys-root/mingw/ )

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Boost
set(Boost_USE_MULTITHREADED  ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(Boost_USE_STATIC_LIBS    OFF)

# GLIB
set(ENV{GLIB_DIR} ${PLATFORM_FIND_ROOT_PATH})

# FREETYPE
set(ENV{FREETYPE_DIR} ${PLATFORM_FIND_ROOT_PATH}/include/freetype)

# Options
set(UTIL_BASE_USE_MINGW 1)
set(UTF8_CONVERTER_USE_EMBED 1)
set(UTF8_CONVERTER_USE_ICONV 0)
set(CANVAS_SYSTEM_USE_GTK 0)
set(CANVAS_2D_USE_CAIRO 0)
set(CANVAS_2D_USE_GL 1)
set(CANVAS_AUDIO_USE_PULSE 0)
set(GINGA_PACKAGE_USE_MINGW 1)
