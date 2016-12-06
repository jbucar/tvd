set( CANVAS_MOC_HEADERS "src/impl/qt/mytimer.h" "src/impl/qt/myqioevent.h" )
QT4_WRAP_CPP( CANVAS_MOC_OUTFILES ${CANVAS_MOC_HEADERS} )

ADD_SOURCES(
	src/impl/qt/font.h
	src/impl/qt/font.cpp
	src/impl/qt/glyphrun.h
	src/impl/qt/glyphrun.cpp
	src/impl/qt/surface.h
	src/impl/qt/surface.cpp
	src/impl/qt/canvas.h
	src/impl/qt/canvas.cpp
	src/impl/qt/window.h
	src/impl/qt/window.cpp
	src/impl/qt/qlayer.h
	src/impl/qt/qlayer.cpp
	src/impl/qt/qview.h
	src/impl/qt/qview.cpp
	src/impl/qt/dispatcher.h
	src/impl/qt/dispatcher.cpp
	src/impl/qt/system.h
	src/impl/qt/system.cpp
	src/impl/qt/keys.cpp
	src/impl/qt/mytimer.h
	src/impl/qt/mytimer.cpp
	src/impl/qt/myqioevent.h
	src/impl/qt/myqioevent.cpp
	src/impl/qt/webviewer.h
	src/impl/qt/webviewer.cpp
	src/impl/qt/videooverlay.h
	src/impl/qt/videooverlay.cpp
	${CANVAS_MOC_OUTFILES}
)

