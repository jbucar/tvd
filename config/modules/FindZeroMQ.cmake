# - Try to find ZeroMQ headers and libraries
# Variables defined by this module:
#  ZEROMQ_FOUND              System has ZeroMQ libs/headers
#  ZEROMQ_LIBRARIES          The ZeroMQ libraries
#  ZEROMQ_INCLUDE_DIR        The location of ZeroMQ headers

# Include dir
find_path(ZEROMQ_INCLUDE_DIR
	NAMES zmq.hpp
)

# Finally the library itself
find_library(ZEROMQ_LIBRARIES
	NAMES zmq
)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set ZLIB_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(ZEROMQ DEFAULT_MSG ZEROMQ_LIBRARIES ZEROMQ_INCLUDE_DIR)
mark_as_advanced( ZEROMQ_INCLUDE_DIR ZEROMQ_LIBRARIES )
