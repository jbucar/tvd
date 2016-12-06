ADD_HEADERS(
	src/libraryinterface.h
	src/apis/browser.h
	src/apis/fs.h
	src/apis/system.h
	src/apis/web.h
	src/test/mocks.h
)

ADD_SOURCES(
	src/test/mocks.h
	src/test/mocks.cpp
	src/test/browserapimock.cpp
	src/test/fsapimock.cpp
	src/test/systemapimock.cpp
	src/test/webapimock.cpp
)
