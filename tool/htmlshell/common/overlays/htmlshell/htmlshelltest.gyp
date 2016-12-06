{
  'variables': {
    'pkg-config': 'pkg-config',
    'chromium_code': 1,
    'about_credits_file': '<(SHARED_INTERMEDIATE_DIR)/about_credits.html',
    'framework_name': 'HTML shell test',
    'revision': '1.0.0',
    'depot': '<!(echo $DEPOT)',
  },
  'targets': [
    {
      'target_name': 'htmlshelltest',
      'type': 'executable',
      'include_dirs': ['<(depot)/lib'],
      'dependencies': [
        'testinitfail',
        'testinitsucceed',
        'testinvalid',
        'testinteraction',
        'testwebapi',
        'testapis',
        '<(DEPTH)/htmlshell/htmlshell.gyp:htmlshellbase',
        '<(DEPTH)/base/base.gyp:test_support_base',
        '<(DEPTH)/testing/gtest.gyp:gtest',
        '<(DEPTH)/content/content_shell_and_tests.gyp:test_support_content',
      ],
      'sources': [
        'test/htmlshell.cpp',
        'test/library.cpp',
        'test/main.cpp',
        'test/processcomunication.cpp',
        'test/util.cpp',
        'test/apis/shellapi.cpp',
        'test/apis/browser/browserapi.cpp',
        'test/apis/fsapi/fsapi.cpp',
        'test/apis/system/systemapi.cpp',
        'test/apis/webapi/webapi.cpp',
        'test/apis/testapi/testapi.cpp',
      ],
    },

    # Htmlshell test extensions
    {
      'target_name': 'testinitsucceed',
      'type': 'loadable_module',
      'cflags': ['-fvisibility=default'],
      'sources': ['test/libs/initsucceed.cpp'],
      'include_dirs': ['<(depot)/lib'],
    },
    {
      'target_name': 'testinitfail',
      'type': 'loadable_module',
      'cflags': ['-fvisibility=default'],
      'include_dirs': ['<(depot)/lib'],
      'sources': ['test/libs/initfail.cpp'],
    },
    {
      'target_name': 'testinvalid',
      'type': 'loadable_module',
      'cflags': ['-fvisibility=default'],
      'include_dirs': ['<(depot)/lib'],
      'sources': ['test/libs/invalid.cpp'],
    },
    {
      'target_name': 'testinteraction',
      'type': 'loadable_module',
      'cflags': ['-fvisibility=default'],
      'include_dirs': ['<(depot)/lib'],
      'sources': ['test/libs/interaction.cpp'],
    },
    {
      'target_name': 'testwebapi',
      'type': 'loadable_module',
      'cflags': ['-fvisibility=default'],
      'include_dirs': ['<(depot)/lib'],
      'dependencies': ['<(DEPTH)/base/base.gyp:base'],
      'sources': ['test/libs/testwebapi.cpp']
    },
    {
      'target_name': 'testapis',
      'type': 'loadable_module',
      'cflags': ['-fvisibility=default'],
      'include_dirs': ['<(depot)/lib'],
      'sources': ['test/libs/testapis.cpp']
    },
  ],
}
