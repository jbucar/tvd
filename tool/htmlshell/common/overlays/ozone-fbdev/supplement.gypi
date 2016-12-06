{
  'variables':  {
    'ozone_platform_fbdev%': 1,
  },
  'conditions': [
    ['<(ozone_platform_fbdev) == 1', {
      'variables':  {
        'ozone_platform%': 'fbdev',
        'external_ozone_platforms': ['fbdev'],
        'external_ozone_platform_deps': [
          '<(DEPTH)/ozone-fbdev/fbdev.gyp:fbdev',
        ]
      }
    }]
  ],
}
