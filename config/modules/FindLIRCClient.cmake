# This module defines
#  LIRCCLIENT_LIBRARY, the library to link against
#  LIRCCLIENT_FOUND, if false, do not try to link to lirc_client
#  LIRCCLIENT_INCLUDE_DIR, where to find lirc/lirc_client.h

FIND_PATH(LIRCCLIENT_INCLUDE_DIR lirc/lirc_client.h
  /usr/local/include/
  /usr/include/
)

FIND_LIBRARY(LIRCCLIENT_LIBRARY lirc_client
  /usr/local/lib
  /usr/lib
)

SET(LIRCCLIENT_FOUND "NO")
IF(LIRCCLIENT_LIBRARY)
  SET(LIRCCLIENT_FOUND "YES")
ENDIF(LIRCCLIENT_LIBRARY)

SET(LIRCCLIENT_LIBRARIES ${LIRCCLIENT_LIBRARY})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIRCCLIENT_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(LIRCCLIENT DEFAULT_MSG LIRCCLIENT_LIBRARIES LIRCCLIENT_INCLUDE_DIR)
mark_as_advanced( LIRCCLIENT_INCLUDE_DIR LIRCCLIENT_LIBRARIES)
