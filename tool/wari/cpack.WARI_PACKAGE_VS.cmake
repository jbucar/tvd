SET(CPACK_NSIS_PACKAGE_NAME "wari ${PROJECT_VERSION}")
SET_TOOLNAME( OUT_NAME ${PRE_BUILD_PROJECT_NAME} )
SET(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
	StrCpy $FONT_DIR $FONTS
	!insertmacro InstallTTF \\\"\\\${INST_DIR}\\\\share\\\\wari\\\\fonts\\\\TIRESIAS.TTF\\\"
	!insertmacro InstallTTF \\\"\\\${INST_DIR}\\\\share\\\\wari\\\\fonts\\\\he.TTF\\\"
")