set( ANDROID_CANVAS_LIBRARIES "OpenMAXAL" )
set( ANDROID_CANVAS_FOUND 1 )
ADD_DEPENDENTS( ANDROID_CANVAS )

# set( ANDROID_TOOLCHAIN_SYSROOT $ENV{ANDROID_TOOLCHAIN_SYSROOT})
# set( ANDROID_STAGEFRIGHT_INCLUDE_DIR
#   "${ANDROID_TOOLCHAIN_SYSROOT}/usr/include/stagefright/frameworks/base/include"
#   "${ANDROID_TOOLCHAIN_SYSROOT}/usr/include/stagefright/frameworks/base/include/media/stagefright/openmax"
#   "${ANDROID_TOOLCHAIN_SYSROOT}/usr/include/stagefright/system/core/include"
#   "${ANDROID_TOOLCHAIN_SYSROOT}/usr/include/stagefright/libhardware/include"
#   )
# set( ANDROID_STAGEFRIGHT_LIBRARIES
#   "${ANDROID_TOOLCHAIN_SYSROOT}/usr/lib/stagefright/libbinder.so"
#   "${ANDROID_TOOLCHAIN_SYSROOT}/usr/lib/stagefright/libcutils.so"
#   "${ANDROID_TOOLCHAIN_SYSROOT}/usr/lib/stagefright/libgui.so"
#   "${ANDROID_TOOLCHAIN_SYSROOT}/usr/lib/stagefright/libstagefright.so"
#   "${ANDROID_TOOLCHAIN_SYSROOT}/usr/lib/stagefright/libutils.so"
#   "${ANDROID_TOOLCHAIN_SYSROOT}/usr/lib/stagefright/libdtvmediasource.so"
# )
# set( ANDROID_STAGEFRIGHT_FOUND 1 )

# ADD_DEPENDENTS( ANDROID_CANVAS ANDROID_STAGEFRIGHT )

# Find_Package( FFMPEG REQUIRED avformat avcodec avutil )
# ADD_DEPENDENTS( FFMPEG-AVFORMAT FFMPEG-AVCODEC FFMPEG-AVUTIL )

