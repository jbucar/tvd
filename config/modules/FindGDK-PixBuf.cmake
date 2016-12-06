# - Try to find gdk-pixbuf 2.0
# Once done, this will define
#
#  GDK-PIXBUF_FOUND - system has GDK-PixBuf
#  GDK-PIXBUF_INCLUDE_DIRS - the GDK-PixBuf include directories
#  GDK-PIXBUF_LIBRARIES - link these to use GDK-PixBuf

# Main include dir
find_path(GDK-PIXBUF_INCLUDE_DIRS
  NAMES gdk-pixbuf/gdk-pixbuf.h
  PATH_SUFFIXES gdk-pixbuf-2.0
  PATHS /usr/lib /usr/local/lib $ENV{GDK-PIXBUF_DIR}/include
)

# Finally the library itself
find_library(GDK-PIXBUF_LIBRARIES
  NAMES gdk_pixbuf-2.0
  PATHS $ENV{GDK-PIXBUF_DIR}/lib
)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set DB_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(GDK-PIXBUF DEFAULT_MSG GDK-PIXBUF_LIBRARIES GDK-PIXBUF_INCLUDE_DIRS)
mark_as_advanced( GDK-PIXBUF_INCLUDE_DIRS GDK-PIXBUF_LIBRARIES )