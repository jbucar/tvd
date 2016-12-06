# Set install directory, it is the build directory
setIfEmpty( TVD_INSTALL_PREFIX ${DEPOT}/install.${PLATFORM}/rootfs )
# Set prefix directory, it is the prefix directory for runtime
setIfEmpty( TVD_CONFIG_INSTALL_PREFIX ${DEPOT}/install.${PLATFORM}/rootfs )

# Set build type Release, Debug, RelWithDebInfo if wasnt setted before, you use this to override build_type param
#   set(CMAKE_BUILD_TYPE "Release")
#   set(CMAKE_BUILD_TYPE "Debug")
#   set(CMAKE_BUILD_TYPE "RelWithDebInfo")
#   set(CMAKE_BUILD_TYPE "MinSizeRel")
setIfEmptyEnv( CMAKE_BUILD_TYPE TVD_CMAKE_BUILD_TYPE "Release" )

# Set default platform binary type
setIfEmpty(PLATFORM_BINARY_TYPE "tool")

# Setup User config
setIfEmptyEnv( SETUP_USER USER_CONFIG_FILE ${CONFIG_PATH}/SetupUser.cmake )
FILE(TO_CMAKE_PATH ${SETUP_USER} SETUP_USER)

# Set compile tests in true
setIfEmptyEnv(COMPILE_TESTS TVD_COMPILE_TESTS 0)

# Set run tests in false
setIfEmptyEnv(RUN_TESTS TVD_RUN_TESTS 0)

# Set make documentation in false
setIfEmptyEnv( MAKE_DOCUMENTATION TVD_MAKE_DOCUMENTATION 0 )

# Set make doxygen documentation in false
setIfEmptyEnv( MAKE_DOXYGEN TVD_MAKE_DOXYGEN 0 )

# Set info mode to off
setIfEmptyEnv(SILENT TVD_SILENT 1)

# Set project prefix to tvd
setIfEmptyEnv(PROJECT_PREFIX TVD_PROJECT_PREFIX dtv)

# Set the project vendor
setIfEmptyEnv(PROJECT_VENDOR TVD_PROJECT_VENDOR LIFIA )

# Set to 0 when not a release candidate, non zero means that any generated
#  svn tags will be treated as release candidates of given number
setIfEmptyEnv(PROJECT_RELEASE_CANDIDATE TVD_PROJECT_RELEASE_CANDIDATE 0)

# Set build to [shared|static]
OPTION_IF_EMPTY(BUILD_SHARED "Build the library shared." OFF)

