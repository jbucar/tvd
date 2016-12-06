Find_Package( GDK-PixBuf REQUIRED )
Find_Package( Glib REQUIRED )

IF ("${PLATFORM}" STREQUAL "VS")
	find_path(FREETYPE_INCLUDE_DIR_ft2build ft2build.h
		HINTS
			ENV FREETYPE_DIR
		PATHS
			ENV GTKMM_BASEPATH
			[HKEY_CURRENT_USER\\SOFTWARE\\gtkmm\\2.4;Path]
			[HKEY_LOCAL_MACHINE\\SOFTWARE\\gtkmm\\2.4;Path]
		PATH_SUFFIXES include/freetype2 include
	)
	find_path(FREETYPE_INCLUDE_DIR_freetype2 freetype/config/ftheader.h
		HINTS
			ENV FREETYPE_DIR
		PATHS
			ENV GTKMM_BASEPATH
			[HKEY_CURRENT_USER\\SOFTWARE\\gtkmm\\2.4;Path]
			[HKEY_LOCAL_MACHINE\\SOFTWARE\\gtkmm\\2.4;Path]
		PATH_SUFFIXES include/freetype2 include
	)
	set( FREETYPE_INCLUDE_DIRS "${FREETYPE_INCLUDE_DIR_ft2build};${FREETYPE_INCLUDE_DIR_freetype2}")
	set( FREETYPE_FOUND 1)
ELSE()
	Find_Package( Freetype REQUIRED )
ENDIF()

Find_Package( Cairo REQUIRED )

ADD_DEPENDENTS( GDK-PIXBUF GLIB FREETYPE CAIRO )
