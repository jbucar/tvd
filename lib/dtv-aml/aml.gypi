# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'include_dirs': [
    './src',
  ],
  'defines': [
    'HTMLSHELL_BUILD',
    'AML_AUDIO_USE_ALSA=0',
    'AML_AUDIO_USE_PULSE=1'
  ],
  'sources': [
    'src/aml.h',
    'src/decoder.h',
    'src/ts.h',
    'src/v4l.h',
    'src/aml.cpp',
    'src/decoder.cpp',
    'src/ts.cpp',
    'src/v4l.cpp',
    'src/decoder/packet.h',
    'src/decoder/packet.cpp',
    'src/decoder/h264.h',
    'src/decoder/h264.cpp',
    'src/audio/audio.h',
    'src/audio/audio.cpp',
    'src/audio/audio_dsp.cpp',
    'src/audio/audio_dsp.h',
    'src/audio/audio_hw.cpp',
    'src/audio/audio_hw.h',
    'src/audio/audio_alsa.cpp',
    'src/audio/audio_alsa.h',
    'src/amports/aformat.h',
    'src/amports/amstream.h',
    'src/amports/vformat.h'
  ],
}
