{
  'targets': [
    {
      'target_name': 'node-audio-native',
      'sources': [ 'src/node_audio.cc' ],
      'include_dirs': [
        "<!@(node -p \"require('node-addon-api').include\")",
        "src/engine",
        "src",
        "src/aubio/src",
        "src/portaudio/include"
      ],
      # 'libraries': ['<!@(pkg-config jack2 --cflags --libs)'],
      # 'libraries': ['-ljack'],
      'libraries': [
          '../src/aubio/build/src/libaubio.a',
          '../src/portaudio/lib/.libs/libportaudio.a',
          # '-L/src/aubio/build/src',
          # '-L/opt/homebrew/Cellar/portaudio/19.7.0/lib',
          # '-L/opt/homebrew/Cellar/aubio/0.4.9_3/lib',
          # '-lportaudio', 
          # '-laubio',
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