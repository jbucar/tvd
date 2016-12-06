CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
INCLUDE($ENV{DEPOT}/config/Tvd.cmake)

set(CMAKE_CXX_STANDARD 11)

Add_Bin (
	PROJECT_NAME "ioctlmodule"
	BIN_TYPE "lib"
	PROJECT_DESCRIPTION "TVD ioctl node module to use in the TAC"
)
