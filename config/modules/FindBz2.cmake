find_path( BZ2_INCLUDE_DIR bzlib.h )
SET(BZ2_LIBRARIES "libbz2.dylib")

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set BZ2_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(BZ2 DEFAULT_MSG BZ2_LIBRARIES BZ2_INCLUDE_DIR)
mark_as_advanced( BZ2_INCLUDE_DIR BZ2_LIBRARIES)
