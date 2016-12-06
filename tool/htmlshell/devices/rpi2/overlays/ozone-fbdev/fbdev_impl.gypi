{
  'link_settings': {
    'libraries': ['-lbcm_host', '-lvcos', '-lvchostif'],
  },
  'include_dirs': [
    '..',
    '<(sysroot)/usr/include/vcinclude',
    '<(sysroot)/usr/include/interface/vcos/pthreads',
    '<(sysroot)/usr/include/interface/vmcs_host/linux',
  ],
  'sources': [
    'rpi2.cpp',
  ]
}
