find_package( DirectFB 1.4.12 REQUIRED )
find_package( PNG  REQUIRED )
find_package( ZLIB REQUIRED )

ADD_DEPENDENTS( DIRECTFB PNG ZLIB )

