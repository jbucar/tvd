{
  'targets': [
    {
      'target_name': 'fbdev',
      'type': 'static_library',
      'dependencies': [
        '<(DEPTH)/base/base.gyp:base',
        '<(DEPTH)/ui/gfx/gfx.gyp:gfx',
        '<(DEPTH)/ui/events/events.gyp:events',
        '<(DEPTH)/ui/events/ozone/events_ozone.gyp:events_ozone_evdev',
      ],
      'includes': [
        './fbdev_impl.gypi',
      ],
      'sources': [
        'cursordelegate.h',
        'cursordelegate.cpp',
        'fbdev.cpp',
        'platform.h',
        'platform.cpp',
        'surface.h',
        'surface.cpp',
        'surfacefactory.h',
        'surfacefactory.cpp',
        'window.h',
        'window.cpp',
      ],
    }
  ]
}
