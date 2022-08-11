{
  'targets': [
    {
      'target_name': 'jack-library',

    },
    {
      'target_name': 'node-audio-native',
      'sources': [ 'src/node_audio.cc' ],
      'include_dirs': [
        "<!@(node -p \"require('node-addon-api').include\")",
        "src/btrack/src",
        "src/jack2/common",
        "src/engine",
        "src"
      ],
      # 'libraries': ['<!@(pkg-config jack2 --cflags --libs)'],
      # 'libraries': ['-ljack'],
      'dependencies': ["<!(node -p \"require('node-addon-api').gyp\")"],
      'cflags!': [ '-fno-exceptions', '-std=c++17' ],
      'cflags_cc!': [ '-fno-exceptions', '-std=c++17' ],
      'xcode_settings': {
        'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
        'CLANG_CXX_LIBRARY': 'libc++',
        'MACOSX_DEPLOYMENT_TARGET': '10.7',
        'OTHER_CFLAGS': [
          '-std=c++17'
        ]
      },
      'msvs_settings': {
        'VCCLCompilerTool': { 
          'ExceptionHandling': 1,
          'AdditionalOptions': [ '-std:c++17' ]
        },
      }
    }
  ]
}