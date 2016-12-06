SET(CPACK_NSIS_PACKAGE_NAME "ginga.ar ${PROJECT_VERSION}")
SET_TOOLNAME( OUT_NAME ${PRE_BUILD_PROJECT_NAME} )
SET(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
	StrCpy $FONT_DIR $FONTS
	!insertmacro InstallTTF \\\"\\\${INST_DIR}\\\\share\\\\ginga\\\\fonts\\\\TIRESIAS.TTF\\\"
	!insertmacro InstallTTF \\\"\\\${INST_DIR}\\\\share\\\\ginga\\\\fonts\\\\TIRESIAS-ITALIC.TTF\\\"
	!insertmacro InstallTTF \\\"\\\${INST_DIR}\\\\share\\\\ginga\\\\fonts\\\\TIRESIAS-BOLD.TTF\\\"
	!insertmacro InstallTTF \\\"\\\${INST_DIR}\\\\share\\\\ginga\\\\fonts\\\\DEJAVUSANS.TTF\\\"
	!insertmacro InstallTTF \\\"\\\${INST_DIR}\\\\share\\\\ginga\\\\fonts\\\\DEJAVUSANS-BOLD.TTF\\\"
	!insertmacro InstallTTF \\\"\\\${INST_DIR}\\\\share\\\\ginga\\\\fonts\\\\DEJAVUSANS-ITALIC.TTF\\\"
	!insertmacro InstallTTF \\\"\\\${INST_DIR}\\\\share\\\\ginga\\\\fonts\\\\DEJAVUSANS-BOLDITALIC.TTF\\\"
	!insertmacro APP_ASSOCIATE \\\"ncl\\\" \\\"ginga.ncl\\\" \\\"\\\$INSTDIR\\\\bin\\\\${OUT_NAME}.exe,0\\\" \\\"NCL app\\\" \\\"\\\$INSTDIR\\\\bin\\\\${OUT_NAME}.exe --ncl \\\$\\\\\\\"%1\\\$\\\\\\\"\\\" \\\"\\\$INSTDIR\\\\bin\\\\${OUT_NAME}.exe --ncl \\\$\\\\\\\"%1\\\$\\\\\\\"\\\"
	!define env_hklm 'HKLM \\\"SYSTEM\\\\CurrentControlSet\\\\Control\\\\Session Manager\\\\Environment\\\"'
	WriteRegExpandStr \\\${env_hklm} \\\"GINGA_PROCESS\\\" \\\"\\\$INSTDIR\\\\bin\\\\${OUT_NAME}.exe\\\"
	WriteRegExpandStr \\\${env_hklm} \\\"GINGA_VERSION\\\" \\\"${PROJECT_VERSION}\\\"
")