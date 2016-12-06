{
  'variables': {
    'pkg-config': 'pkg-config',
    'chromium_code': 1,
    'grit_out_dir': '<(SHARED_INTERMEDIATE_DIR)/htmlshell',
    'about_credits_file': '<(SHARED_INTERMEDIATE_DIR)/about_credits.html',
    'framework_name': 'HTML shell',
    'revision': '1.0.0',
    'chrome_version': '<!(python ../build/util/version.py -f ../chrome/VERSION -t "@MAJOR@.@MINOR@.@BUILD@.@PATCH@")',
    'depot': '<!(echo $DEPOT)',
  },
  'targets': [
    {
      'target_name': 'htmlshell',
      'type': 'executable',
      'include_dirs': ['<(depot)/lib'],
      'dependencies': [
        'htmlshellbase',
        'htmlshell_pak',
      ],
      'sources': ['src/main.cpp'],
    },

    {
      'target_name': 'htmlshellbase',
      'type': 'static_library',
      'defines': ['CHROMIUM_VERSION="<(chrome_version)"'],
      'include_dirs': ['<(depot)/lib'],
      'dependencies': [
        'htmlshell_resources',
        '<(DEPTH)/base/base.gyp:base',
        '<(DEPTH)/skia/skia.gyp:skia',
        '<(DEPTH)/third_party/WebKit/public/blink.gyp:blink',
        '<(DEPTH)/third_party/WebKit/Source/core/core.gyp:webcore',
        '<(DEPTH)/third_party/widevine/cdm/widevine_cdm.gyp:widevinecdmadapter',
        '<(DEPTH)/third_party/widevine/cdm/widevine_cdm.gyp:widevine_cdm_version_h',
        '<(DEPTH)/content/content.gyp:content',
        '<(DEPTH)/content/content.gyp:content_app_both',
        '<(DEPTH)/content/content.gyp:content_common',
        '<(DEPTH)/content/content.gyp:content_app_both',
        '<(DEPTH)/content/content.gyp:content_browser',
        '<(DEPTH)/content/content.gyp:content_gpu',
        '<(DEPTH)/content/content.gyp:content_plugin',
        '<(DEPTH)/content/content.gyp:content_ppapi_plugin',
        '<(DEPTH)/content/content.gyp:content_renderer',
        '<(DEPTH)/content/content.gyp:content_resources',
        '<(DEPTH)/content/content.gyp:content_utility',
        '<(DEPTH)/components/components.gyp:cdm_renderer',
        '<(DEPTH)/components/components.gyp:crash_component',
        '<(DEPTH)/components/components.gyp:devtools_discovery',
        '<(DEPTH)/components/components.gyp:devtools_http_handler',
        '<(DEPTH)/components/components.gyp:web_cache_renderer',
        '<(DEPTH)/gpu/gpu.gyp:gpu',
        '<(DEPTH)/ipc/ipc.gyp:ipc',
        '<(DEPTH)/media/blink/media_blink.gyp:media_blink',
        '<(DEPTH)/media/media.gyp:media',
        '<(DEPTH)/ui/aura/aura.gyp:aura',
        '<(DEPTH)/ui/ozone/ozone.gyp:ozone',
        '<(DEPTH)/ui/wm/wm.gyp:wm',
      ],
      'sources': [
        'src/library.cpp',
        'src/util.cpp',
        'src/apis/shellapi.cpp',

        # HtmlShellSdk implementation:
        'src/apis/browser/browserapi.cpp',
        'src/apis/browser/util.cpp',
        'src/apis/fs/fsapi.cpp',
        'src/apis/web/webapi.cpp',
        'src/apis/system/systemapi.cpp',

        # Content api implementation:
        'src/impl/app/maindelegate.cpp',
        'src/impl/app/crashreporterclient.cpp',
        'src/impl/browser/accesstokenstore.cpp',
        'src/impl/browser/browserclient.cpp',
        'src/impl/browser/browsercontext.cpp',
        'src/impl/browser/browsermainparts.cpp',
        'src/impl/browser/browserpepperhostfactory.cpp',
        'src/impl/browser/configuredisplaytask.cpp',
        'src/impl/browser/cursorwindowdelegate.cpp',
        'src/impl/browser/displayconfigurator.cpp',
        'src/impl/browser/downloadmanagerdelegate.cpp',
        'src/impl/browser/javascriptdialogmanager.cpp',
        'src/impl/browser/messagefilter.cpp',
        'src/impl/browser/networkdelegate.cpp',
        'src/impl/browser/pepperisolatedfilesystemmessagefilter.cpp',
        'src/impl/browser/permissionmanager.cpp',
        'src/impl/browser/platformdataaura.cpp',
        'src/impl/browser/pluginservicefilter.cpp',
        'src/impl/browser/resourcecontext.cpp',
        'src/impl/browser/udpprotocolhandler.cpp',
        'src/impl/browser/udpurlrequestjob.cpp',
        'src/impl/browser/urlrequestcontextgetter.cpp',
        'src/impl/browser/screen.cpp',
        'src/impl/browser/siteinstancehost.cpp',
        'src/impl/common/client.cpp',
        'src/impl/common/message_generator.cpp',
        'src/impl/renderer/rendererclient.cpp',
        'src/impl/renderer/siteinstance.cpp',
        'src/impl/ui/aura/testfocusclient.cpp',

        # Devtools remote debugging
        'src/impl/browser/devtools/delegate.h',
        'src/impl/browser/devtools/delegate.cpp',
        'src/impl/browser/devtools/tcpserversocketfactory.h',
        'src/impl/browser/devtools/tcpserversocketfactory.cpp',
      ],
      'conditions': [
        ['use_video_proxy=="dummy" or use_video_proxy=="aml"', {
          'defines': ['TAC_USE_VIDEO_PROXY'],
          'sources': [
            'src/media/webmediaplayer_proxy.h',
            'src/media/webmediaplayer_proxy.cpp',
            'src/media/video_frame_provider.h',
            'src/media/video_frame_provider.cpp',
            'src/media/video_proxy.h'
          ]}
        ],
        ['use_video_proxy=="dummy"', {
          'sources': ['src/media/video_proxy_dummy.cc']}
        ],
        ['use_video_proxy=="aml"', {
          'sources': ['src/media/video_proxy_aml.cc']}
        ],
        ['os_posix == 1 and OS != "mac"', {
          'dependencies': [
            '../components/components.gyp:breakpad_host',
          ],
        }],
      ]
    },

    # Resource.pak
    {
      'target_name': 'htmlshell_pak',
      'type': 'none',
      'dependencies': [
        'htmlshell_resources',
        '<(DEPTH)/content/content.gyp:content_resources',
        '<(DEPTH)/content/app/resources/content_resources.gyp:content_resources',
        '<(DEPTH)/content/app/strings/content_strings.gyp:content_strings',
        '<(DEPTH)/content/browser/tracing/tracing_resources.gyp:tracing_resources',
        '<(DEPTH)/content/browser/devtools/devtools_resources.gyp:devtools_resources',
        '<(DEPTH)/net/net.gyp:net_resources',
        '<(DEPTH)/third_party/WebKit/public/blink_resources.gyp:blink_resources',
        '<(DEPTH)/ui/resources/ui_resources.gyp:ui_resources',
        '<(DEPTH)/ui/strings/ui_strings.gyp:ui_strings',
      ],
      'actions': [
        {
          'action_name': 'repack_htmlshell_pack',
          'variables': {
            'pak_inputs': [
              '<(SHARED_INTERMEDIATE_DIR)/blink/public/resources/blink_resources.pak',
              '<(SHARED_INTERMEDIATE_DIR)/blink/public/resources/blink_image_resources_100_percent.pak',
              '<(SHARED_INTERMEDIATE_DIR)/content/app/resources/content_resources_100_percent.pak',
              '<(SHARED_INTERMEDIATE_DIR)/content/app/strings/content_strings_en-US.pak',
              '<(SHARED_INTERMEDIATE_DIR)/content/browser/tracing/tracing_resources.pak',
              '<(SHARED_INTERMEDIATE_DIR)/content/content_resources.pak',
              '<(SHARED_INTERMEDIATE_DIR)/net/net_resources.pak',
              '<(SHARED_INTERMEDIATE_DIR)/ui/resources/ui_resources_100_percent.pak',
              '<(SHARED_INTERMEDIATE_DIR)/ui/resources/webui_resources.pak',
              '<(SHARED_INTERMEDIATE_DIR)/ui/strings/app_locale_settings_en-US.pak',
              '<(SHARED_INTERMEDIATE_DIR)/ui/strings/ui_strings_en-US.pak',
              '<(SHARED_INTERMEDIATE_DIR)/htmlshell/htmlshell_resources.pak',
              '<(SHARED_INTERMEDIATE_DIR)/blink/devtools_resources.pak',
            ],
            'pak_output': '<(PRODUCT_DIR)/htmlshell.pak',
          },
          'includes': [ '../build/repack_action.gypi' ],
        },
      ],
    },

    # HtmlShell resources
    {
      'target_name': 'htmlshell_resources',
      'type': 'none',
      'actions': [
        {
          'action_name': 'generate_htmlshell_resources',
          'variables': {
            'grit_grd_file': 'htmlshell_resources.grd',
          },
          'includes': [ '../build/grit_action.gypi' ],
        },
      ],
      'includes': [ '../build/grit_target.gypi' ],
      'copies': [
        {
          'destination': '<(PRODUCT_DIR)',
          'files': [
            '<(SHARED_INTERMEDIATE_DIR)/htmlshell/htmlshell_resources.pak'
          ],
        },
      ],
    },
  ],
}
