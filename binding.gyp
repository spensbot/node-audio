{
  'targets': [
    {
      'target_name': 'node-audio-native',
      'sources': [ 'src/node_audio.cc' ],
      'include_dirs': [
        "<!@(node -p \"require('node-addon-api').include\")",
        "src",
        "src/engine",
        "lib",
        "lib/aubio/src",
        "lib/portaudio/include"
      ],
      # 'libraries': ['<!@(pkg-config jack2 --cflags --libs)'],
      # 'libraries': ['-ljack'],
      'libraries': [
          '../lib/aubio/build/src/libaubio.a',
        ],
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