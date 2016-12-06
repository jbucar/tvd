#*********************************************************#
#*  File: Apk.cmake                                      *
#*    Android apk tools
#*
#*  Copyright (C) 2002-2013 The PixelLight Team (http://www.pixellight.org/)
#*
#*  This file is part of PixelLight.
#*
#*  Permission is hereby granted, free of charge, to any person obtaining a copy of this software
#*  and associated documentation files (the "Software"), to deal in the Software without
#*  restriction, including without limitation the rights to use, copy, modify, merge, publish,
#*  distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
#*  Software is furnished to do so, subject to the following conditions:
#*
#*  The above copyright notice and this permission notice shall be included in all copies or
#*  substantial portions of the Software.
#*
#*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
#*  BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
#*  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
#*  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#*  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#*********************************************************#


##################################################
## Options
##################################################
set(ANDROID_APK_SIGNER_KEYSTORE      "~/.android/debug.keystore"        CACHE STRING    "Keystore for signing the apk file (only required for release apk)")
set(ANDROID_APK_SIGNER_KEYSTORE_PASS "android"                          CACHE STRING    "Keystore password (only required for release apk)")
set(ANDROID_APK_SIGNER_ALIAS         "androiddebugkey"                  CACHE STRING    "Alias for signing the apk file (only required for release apk)")

set(ARM_TARGET                       "armeabi"                          CACHE STRING    "API level to use.")

# Inherited from config/target/Platform_ANDROID.cmake
#set(ANDROID_API_LEVEL			"17"		CACHE STRING	"API level to use.")

##################################################
## Variables
##################################################
set(ANDROID_THIS_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})	# Directory this CMake file is in


##################################################
## MACRO: android_create_apk
##
## Create/copy Android apk related files
##
## @param name
##   Name of the project (e.g. "MyProject"), this will also be the name of the created apk file
## @param apk_directory
##   Directory were to construct the apk file in (e.g. "${CMAKE_BINARY_DIR}/apk")
## @param shared_libraries
##   List of shared libraries (absolute filenames) this application is using, these libraries are copied into the apk file and will be loaded automatically within a generated Java file - Lookout! The order is important due to shared library dependencies!
##
## @remarks
##   Requires the following tools to be found automatically
##   - "android" (part of the Android SDK)
##   - "adb" (part of the Android SDK)
##   - "ant" (type e.g. "sudo apt-get install ant" on your Linux system to install Ant)
##   - "jarsigner" (part of the JDK)
##   - "zipalign" (part of the Android SDK)
##################################################
macro(android_create_apk name apk_directory shared_libraries assets_dir)

	# Add apk target
	add_custom_target("package")

	# Construct the current package name and theme
	set(ANDROID_NAME ${name})
	set(ANDROID_APK_PACKAGE "ar.edu.unlp.info.lifia.tvd.${ANDROID_NAME}" )

	# Create "AndroidManifest.xml"
	configure_file("${ANDROID_THIS_DIRECTORY}/AndroidManifest.xml.in" "${apk_directory}/AndroidManifest.xml")

	configure_file("${ANDROID_THIS_DIRECTORY}/Android.mk.in" "${apk_directory}/jni/Android.mk")

	# Create the directory for the libraries
	add_custom_command(TARGET "package"
		PRE_BUILD
		COMMAND ${CMAKE_COMMAND} -E remove_directory "${apk_directory}/libs"
		COMMENT "[APK] Create the directory for the libraries"
	)
	add_custom_command(TARGET "package"
		PRE_BUILD
		COMMAND ${CMAKE_COMMAND} -E make_directory "${apk_directory}/libs/${ARM_TARGET}"
	)

	# Copy the used shared libraries
	foreach(value ${shared_libraries})
		add_custom_command(TARGET "package"
			POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy ${value} "${apk_directory}/libs/${ARM_TARGET}"
			COMMENT "[APK] Copy ${value} shared library"
		)
	endforeach()

	# Create the directory for the icons
	add_custom_command(TARGET "package"
		PRE_BUILD
		COMMAND ${CMAKE_COMMAND} -E remove_directory "${apk_directory}/res"
		COMMENT "[APK] Create the directory for the libraries"
	)
	add_custom_command(TARGET "package"
		PRE_BUILD
		COMMAND ${CMAKE_COMMAND} -E make_directory "${apk_directory}/res"
	)

	# Copy the icons
	add_custom_command(TARGET "package"
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/res"  "${apk_directory}/res/"
		COMMENT "[APK] Copy the icons form ${PROJECT_SOURCE_DIR}/res"
	)
	add_custom_command(TARGET "package"
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E remove "${apk_directory}/res/*.ico"
		COMMENT "[APK] Remove windows ico files"
	)

	# Create "build.xml", "default.properties", "local.properties" and "proguard.cfg" files
	add_custom_command(TARGET "package"
		COMMAND android update project -t android-${ANDROID_API_LEVEL} --name ${ANDROID_NAME} --path "${apk_directory}"
		COMMENT "[APK] Create \"build.xml\", \"default.properties\", \"local.properties\" and \"proguard.cfg\" files"
	)

	# Copy assets
	add_custom_command(TARGET "package"
		PRE_BUILD
		COMMAND ${CMAKE_COMMAND} -E remove_directory "${apk_directory}/assets"
		COMMENT "[APK] Removing assets directory"
	)
	if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
		add_custom_command(TARGET "package"
			PRE_BUILD
			COMMAND ${CMAKE_COMMAND} -E make_directory "${apk_directory}/assets"
			COMMENT "[APK] Creating assets directory"
		)
		add_custom_command(TARGET "package"
			POST_BUILD

			#COMMAND ${CMAKE_COMMAND} -E tar "cvf" "${apk_directory}/assets/assets.tar" "${CMAKE_INSTALL_PREFIX}/share" 
			COMMAND ${CMAKE_COMMAND} -E chdir "${CMAKE_INSTALL_PREFIX}" tar "cf" "${apk_directory}/assets/assets.tar" "${assets_dir}"
			COMMENT "[APK] Packing assets from ${assets_dir}"
		)
	endif()

	STRING(TOLOWER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_LOWER)
	# Build the apk file
	if(CMAKE_BUILD_TYPE MATCHES Debug)
		# In case of debug build, do also copy gdbserver
		add_custom_command(TARGET "package"
			POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy $ENV{ANDROID_NDK}/prebuilt/android-arm/gdbserver/gdbserver "${apk_directory}/libs/${ARM_TARGET}"
			COMMENT "[APK] In case of debug build, do also copy gdbserver\n${CMAKE_COMMAND} -E copy $ENV{ANDROID_NDK}/prebuilt/android-arm/gdbserver/gdbserver \"${apk_directory}/libs/${ARM_TARGET}\""
		)

		# Let Ant create the unsigned apk file
		add_custom_command(TARGET "package"
			COMMAND ant debug
			WORKING_DIRECTORY "${apk_directory}"
			COMMENT "[APK] Let Ant create the unsigned apk file"
		)
		# Add version number to apk file
		add_custom_command(TARGET "package"
			COMMAND ${CMAKE_COMMAND} -E copy bin/${ANDROID_NAME}-${CMAKE_BUILD_TYPE_LOWER}.apk ${CMAKE_BINARY_DIR}/${ANDROID_NAME}-${PROJECT_VERSION}.apk
			WORKING_DIRECTORY "${apk_directory}"
			COMMENT "[APK] Add version number to apk file"
		)
	else()
		# Let Ant create the unsigned apk file
		add_custom_command(TARGET "package"
			COMMAND ant release
			WORKING_DIRECTORY "${apk_directory}"
			COMMENT "[APK] Let Ant create the unsigned apk file"
		)

		# Sign the apk file
		add_custom_command(TARGET "package"
			COMMAND jarsigner -verbose -sigalg MD5withRSA -digestalg SHA1 -keystore ${ANDROID_APK_SIGNER_KEYSTORE} -storepass ${ANDROID_APK_SIGNER_KEYSTORE_PASS} bin/${ANDROID_NAME}-${CMAKE_BUILD_TYPE_LOWER}-unsigned.apk ${ANDROID_APK_SIGNER_ALIAS}
			WORKING_DIRECTORY "${apk_directory}"
			COMMENT "[APK] Sign the apk file"
		)

		# Align the apk file
		add_custom_command(TARGET "package"
			COMMAND zipalign -v -f 4 bin/${ANDROID_NAME}-${CMAKE_BUILD_TYPE_LOWER}-unsigned.apk ${CMAKE_BINARY_DIR}/${ANDROID_NAME}-${PROJECT_VERSION}.apk
			WORKING_DIRECTORY "${apk_directory}"
			COMMENT "[APK] Align the apk file"
		)
	endif()

endmacro(android_create_apk name apk_directory shared_libraries)
