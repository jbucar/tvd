#include "platform.h"
#include "ui/ozone/common/stub_client_native_pixmap_factory.h"

namespace media {

MediaOzonePlatform* CreateMediaOzonePlatformFbdev() {
	return tvd::createMediaOzonePlatform();
}

}

namespace ui {

OzonePlatform *CreateOzonePlatformFbdev() {
	return new tvd::Platform();
}

ClientNativePixmapFactory *CreateClientNativePixmapFactoryFbdev() {
	return CreateStubClientNativePixmapFactory();
}

}
