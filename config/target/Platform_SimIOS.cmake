# Standard settings
set (CMAKE_SYSTEM_NAME Darwin)
set (CMAKE_SYSTEM_VERSION 1)
set (UNIX True)
set (APPLE True)
set (IOS True)
set (PLATFORM SimIOS)

# Force the compilers to gcc for iOS
include (CMakeForceCompiler)
CMAKE_FORCE_C_COMPILER (clang clang)
CMAKE_FORCE_CXX_COMPILER (clang++ clang++)
# CMAKE_FORCE_C_COMPILER (${CMAKE_IOS_DEVELOPER_ROOT}/usr/bin/llvm-gcc llvm-gcc)
# CMAKE_FORCE_CXX_COMPILER (${CMAKE_IOS_DEVELOPER_ROOT}/usr/bin/llvm-g++ llvm-g++)

# Skip the platform compiler checks for cross compiling
set (CMAKE_CXX_COMPILER_WORKS TRUE)
set (CMAKE_C_COMPILER_WORKS TRUE)


# Required as of cmake 2.8.10
set (CMAKE_OSX_DEPLOYMENT_TARGET "" CACHE STRING "Force unset of the deployment target for iOS" FORCE)

# Determine the cmake host system version so we know where to find the iOS SDKs
find_program (CMAKE_UNAME uname /bin /usr/bin /usr/local/bin)
if (CMAKE_UNAME)
exec_program(uname ARGS -r OUTPUT_VARIABLE CMAKE_HOST_SYSTEM_VERSION)
string (REGEX REPLACE "^([0-9]+)\\.([0-9]+).*$" "\\1" DARWIN_MAJOR_VERSION "${CMAKE_HOST_SYSTEM_VERSION}")
endif (CMAKE_UNAME)

# All iOS/Darwin specific settings - some may be redundant
set (CMAKE_SHARED_LIBRARY_PREFIX "lib")
set (CMAKE_SHARED_LIBRARY_SUFFIX ".dylib")
set (CMAKE_SHARED_MODULE_PREFIX "lib")
set (CMAKE_SHARED_MODULE_SUFFIX ".so")
set (CMAKE_MODULE_EXISTS 1)
set (CMAKE_DL_LIBS "")

set (CMAKE_IPHONE_VER "-D__IPHONE_OS_VERSION_MIN_REQUIRED=60100")
set (CMAKE_C_OSX_COMPATIBILITY_VERSION_FLAG "-compatibility_version ")
set (CMAKE_C_OSX_CURRENT_VERSION_FLAG "-current_version ")
set (CMAKE_CXX_OSX_COMPATIBILITY_VERSION_FLAG "${CMAKE_C_OSX_COMPATIBILITY_VERSION_FLAG}")
set (CMAKE_CXX_OSX_CURRENT_VERSION_FLAG "${CMAKE_C_OSX_CURRENT_VERSION_FLAG}")


set (CMAKE_C_LINK_FLAGS "-Wl,-search_paths_first ${CMAKE_C_LINK_FLAGS}")
set (CMAKE_CXX_LINK_FLAGS "-Wl,-search_paths_first ${CMAKE_CXX_LINK_FLAGS}")

set (CMAKE_PLATFORM_HAS_INSTALLNAME 1)
# set (CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS "-dynamiclib -headerpad_max_install_names")
set (CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS "-dynamiclib")
# set (CMAKE_SHARED_MODULE_CREATE_C_FLAGS "-bundle -headerpad_max_install_names")
set (CMAKE_SHARED_MODULE_CREATE_C_FLAGS "-bundle")
set (CMAKE_SHARED_MODULE_LOADER_C_FLAG "-Wl,-bundle_loader,")
set (CMAKE_SHARED_MODULE_LOADER_CXX_FLAG "-Wl,-bundle_loader,")
set (CMAKE_FIND_LIBRARY_SUFFIXES ".dylib" ".so" ".a")

# hack: if a new cmake (which uses CMAKE_INSTALL_NAME_TOOL) runs on an old build tree
# (where install_name_tool was hardcoded) and where CMAKE_INSTALL_NAME_TOOL isn't in the cache
# and still cmake didn't fail in CMakeFindBinUtils.cmake (because it isn't rerun)
# hardcode CMAKE_INSTALL_NAME_TOOL here to install_name_tool, so it behaves as it did before, Alex
if (NOT DEFINED CMAKE_INSTALL_NAME_TOOL)
find_program(CMAKE_INSTALL_NAME_TOOL install_name_tool)
endif (NOT DEFINED CMAKE_INSTALL_NAME_TOOL)

# Setup iOS platform unless specified manually with IOS_PLATFORM
if (NOT DEFINED IOS_PLATFORM)
set (IOS_PLATFORM "SIMULATOR")
endif (NOT DEFINED IOS_PLATFORM)
set (IOS_PLATFORM ${IOS_PLATFORM} CACHE STRING "Type of iOS Platform")

# Check the platform selection and setup for developer root
if (${IOS_PLATFORM} STREQUAL "OS")
set (IOS_PLATFORM_LOCATION "iPhoneOS.platform")

# This causes the installers to properly locate the output libraries
set (CMAKE_XCODE_EFFECTIVE_PLATFORMS "-iphoneos")
elseif (${IOS_PLATFORM} STREQUAL "SIMULATOR")
set (IOS_PLATFORM_LOCATION "iPhoneSimulator.platform")

# This causes the installers to properly locate the output libraries
set (CMAKE_XCODE_EFFECTIVE_PLATFORMS "-iphonesimulator")
else (${IOS_PLATFORM} STREQUAL "OS")
message (FATAL_ERROR "Unsupported IOS_PLATFORM value selected. Please choose OS or SIMULATOR")
endif (${IOS_PLATFORM} STREQUAL "OS")

# Setup iOS developer location unless specified manually with CMAKE_IOS_DEVELOPER_ROOT
# Note Xcode 4.3 changed the installation location, choose the most recent one available
set (XCODE_POST_43_ROOT "/Applications/Xcode.app/Contents/Developer/Platforms/${IOS_PLATFORM_LOCATION}/Developer")
set (XCODE_PRE_43_ROOT "/Developer/Platforms/${IOS_PLATFORM_LOCATION}/Developer")
if (NOT DEFINED CMAKE_IOS_DEVELOPER_ROOT)
if (EXISTS ${XCODE_POST_43_ROOT})
set (CMAKE_IOS_DEVELOPER_ROOT ${XCODE_POST_43_ROOT})
elseif(EXISTS ${XCODE_PRE_43_ROOT})
set (CMAKE_IOS_DEVELOPER_ROOT ${XCODE_PRE_43_ROOT})
endif (EXISTS ${XCODE_POST_43_ROOT})
endif (NOT DEFINED CMAKE_IOS_DEVELOPER_ROOT)
set (CMAKE_IOS_DEVELOPER_ROOT ${CMAKE_IOS_DEVELOPER_ROOT} CACHE PATH "Location of iOS Platform")

# Find and use the most recent iOS sdk unless specified manually with CMAKE_IOS_SDK_ROOT
if (NOT DEFINED CMAKE_IOS_SDK_ROOT)
file (GLOB _CMAKE_IOS_SDKS "${CMAKE_IOS_DEVELOPER_ROOT}/SDKs/*")
if (_CMAKE_IOS_SDKS)
list (SORT _CMAKE_IOS_SDKS)
list (REVERSE _CMAKE_IOS_SDKS)
list (GET _CMAKE_IOS_SDKS 0 CMAKE_IOS_SDK_ROOT)
else (_CMAKE_IOS_SDKS)
message (FATAL_ERROR "No iOS SDK's found in default search path ${CMAKE_IOS_DEVELOPER_ROOT}. Manually set CMAKE_IOS_SDK_ROOT or install the iOS SDK.")
endif (_CMAKE_IOS_SDKS)
message (STATUS "Toolchain using default iOS SDK: ${CMAKE_IOS_SDK_ROOT}")
endif (NOT DEFINED CMAKE_IOS_SDK_ROOT)
set (CMAKE_IOS_SDK_ROOT ${CMAKE_IOS_SDK_ROOT} CACHE PATH "Location of the selected iOS SDK")

# Set the sysroot default to the most recent SDK
set (CMAKE_OSX_SYSROOT ${CMAKE_IOS_SDK_ROOT} CACHE PATH "Sysroot used for iOS support")

# set the architecture for iOS
# NOTE: Currently both ARCHS_STANDARD_32_BIT and ARCHS_UNIVERSAL_IPHONE_OS set armv7 only, so set both manually
if (${IOS_PLATFORM} STREQUAL "OS")
set (IOS_ARCH armv7)
else (${IOS_PLATFORM} STREQUAL "OS")
set (IOS_ARCH i386)
endif (${IOS_PLATFORM} STREQUAL "OS")

set (CMAKE_OSX_ARCHITECTURES ${IOS_ARCH} CACHE string "Build architecture for iOS")

# Set the find root to the iOS developer roots and to user defined paths
set (CMAKE_FIND_ROOT_PATH ${CMAKE_IOS_DEVELOPER_ROOT} ${CMAKE_IOS_SDK_ROOT} ${CMAKE_PREFIX_PATH} CACHE string "iOS find search path root")

# default to searching for frameworks first
set (CMAKE_FIND_FRAMEWORK FIRST)

# set up the default search directories for frameworks
set (CMAKE_SYSTEM_FRAMEWORK_PATH
${CMAKE_IOS_SDK_ROOT}/System/Library/Frameworks
${CMAKE_IOS_SDK_ROOT}/System/Library/PrivateFrameworks
${CMAKE_IOS_SDK_ROOT}/Developer/Library/Frameworks
)

# only search the iOS sdks, not the remainder of the host filesystem
set (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ONLY)
set (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)


# This little macro lets you set any XCode specific property
macro (set_xcode_property TARGET XCODE_PROPERTY XCODE_VALUE)
set_property (TARGET ${TARGET} PROPERTY XCODE_ATTRIBUTE_${XCODE_PROPERTY} ${XCODE_VALUE})
endmacro (set_xcode_property)


# This macro lets you find executable programs on the host system
macro (find_host_package)
set (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY NEVER)
set (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE NEVER)
set (IOS FALSE)

find_package(${ARGN})

set (IOS TRUE)
set (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ONLY)
set (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
endmacro (find_host_package)


SET(PLATFORM_FIND_ROOT_PATH /Users/lifia/ios/fs/)

# Hidden visibilty is required for cxx on iOS
set (CMAKE_C_FLAGS_INIT "-isysroot ${CMAKE_IOS_SDK_ROOT} ${CMAKE_IPHONE_VER}")
# set (CMAKE_CXX_FLAGS_INIT "-headerpad_max_install_names -fvisibility=hidden -fvisibility-inlines-hidden -isysroot ${CMAKE_IOS_SDK_ROOT} ${CMAKE_IPHONE_VER}")
set (CMAKE_CXX_FLAGS_INIT "-fvisibility=hidden -fvisibility-inlines-hidden -isysroot ${CMAKE_IOS_SDK_ROOT} ${CMAKE_IPHONE_VER}")

#Add base frameworks
set(CMAKE_EXE_LINKER_FLAGS "-framework CoreGraphics -framework QuartzCore -framework UIKit -framework CoreFoundation -framework Foundation -framework CoreText -framework MobileCoreServices -framework ImageIO")

# Generic toolchain setup - same as gnu.cmake, ignore add_system_executable
file(TO_CMAKE_PATH $ENV{DEPOT} DEPOT)
include( ${DEPOT}/config/target/gnu.cmake )

# Options
set(BACKTRACE_USE_GNU 0)
set(STORAGE_USE_BDB 0)
set(UTF8_CONVERTER_USE_ICONV 0)
set(UTF8_CONVERTER_USE_EMBED 1)
set(UTIL_BASE_USE_IOS 1)
set(LOG_USE_LOG 0)
set(LOG_USE_IOS 1)
set(IO_USE_EV 1)
set(CANVAS_SYSTEM_USE_GTK 0)
set(CANVAS_SYSTEM_USE_IOS 1)
set(CANVAS_2D_USE_CAIRO 0)
set(CANVAS_2D_USE_SKIA 1)
set(CANVAS_WINDOW_USE_REMOTE 1)
set(CANVAS_REMOTE_USE_SHARED 0)
set(CANVAS_REMOTE_USE_LOCAL 1)
set(CANVAS_PLAYER_USE_VLC 0)
set(CANVAS_AUDIO_USE_PULSE 0)