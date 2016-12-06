# --------------------------------------------
# --------------- CMAKE options   ------------
# --------------------------------------------

# Set build type Release, Debug, RelWithDebInfo
set(CMAKE_BUILD_TYPE "Debug")

# User libs (external libs)
set(CMAKE_PREFIX_PATH $ENV{HOME}/usr) # para librerias personales

# Compilation output
#set(CMAKE_INSTALL_PREFIX $ENV{DEPOT}/install/UNIX/)

# Set root path for compilation output
#set(TVD_CONFIG_INSTALL_PREFIX $ENV{DEPOT}/install/UNIX/rootfs)

# --------------------------------------------
# --------------- COMPILATION TYPE -----------
# --------------------------------------------

set(BUILD_SHARED 1)

# --------------------------------------------
# --------------- TESTS options --------------
# --------------------------------------------

set(COMPILE_TESTS 1)
set(RUN_TESTS 0)

# --------------------------------------------
# --------------- UTIL options ---------------
# --------------------------------------------

# IO impl to use
set(IO_USE_EV 1)
set(IO_USE_UV 0)

# Enable/disable log system
set(LOG_USE_LOG 1)

# Storage impl to use
set(STORAGE_USE_BDB 1)
set(STORAGE_USE_XML 1)
set(STORAGE_USE_MEMORY 1)

# Backtrace impl to use
SET(BACKTRACE_USE_GNU 1)
SET(BACKTRACE_USE_DUMMY 0)

# --------------------------------------------
# -------------- CANVAS options --------------
# --------------------------------------------

# System impl to use
set(CANVAS_SYSTEM_USE_DIRECTFB 1)
set(CANVAS_SYSTEM_USE_GTK 1)
set(CANVAS_SYSTEM_USE_X11 1)
set(CANVAS_SYSTEM_USE_QT 1)

# Canvas impl to use
set(CANVAS_2D_USE_GL 1)
set(USE_GLES2 1)
set(CANVAS_2D_USE_CAIRO 1)
set(CANVAS_2D_USE_SKIA 1)

# FontManager impl to use
set(CANVAS_FONTMANAGER_USE_FONTCONFIG 1)

# Window impl to use
set(CANVAS_WINDOW_USE_FB 1)
set(CANVAS_WINDOW_USE_REMOTE 1)

# Remote memory impl to use
set(CANVAS_REMOTE_USE_SHARED 1)
set(CANVAS_REMOTE_USE_LOCAL 1)

# X11 render impl to use
set(CANVAS_X11_RENDER_USE_VDPAU 1)
set(CANVAS_X11_RENDER_USE_VAAPI 1)
set(CANVAS_X11_RENDER_USE_CAIRO 1)
set(CANVAS_X11_RENDER_USE_SKIA 1)
set(CANVAS_X11_RENDER_USE_GL 1)

# Input impl to use
set(CANVAS_INPUT_USE_LIRC 1)
set(CANVAS_INPUT_USE_LINUX 1)

# HTML impl to use
set(CANVAS_HTML_USE_CEF 0)
set(CANVAS_HTML_USE_GTK 1)

# Video player to use
set(CANVAS_PLAYER_USE_VLC 1)
set(CANVAS_PLAYER_USE_XINE 1)
set(CANVAS_PLAYER_USE_FFMPEG 1)
set(CANVAS_PLAYER_USE_GST 1)

# --------------------------------------------
# ----------- MPEG_PARSER options ------------
# --------------------------------------------

set(DVB_USE_DVB 1)
set(RTP_USE_RTP 1)

# --------------------------------------------
# ----------- ZAPPER options -----------------
# --------------------------------------------

set(ZAPPER_MOUNT_USE_MDEV 0)
set(ZAPPER_MIXER_USE_PULSEAUDIO 1)
