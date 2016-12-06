ADD_HEADERS(
  src/aml.h
  src/decoder.h
  src/ts.h
  src/v4l.h
)

ADD_SOURCES(
  src/aml.cpp
  src/decoder.cpp
  src/ts.cpp
  src/v4l.cpp
  src/decoder/packet.h
  src/decoder/packet.cpp
  src/decoder/h264.h
  src/decoder/h264.cpp
  # src/decoder/divx3.h
  # src/decoder/divx3.cpp
  # src/decoder/divx5.h
  # src/decoder/divx5.cpp
  # src/decoder/mpeg.h
  # src/decoder/mpeg.cpp
  src/audio/audio.h
  src/audio/audio.cpp
  src/audio/audio_dsp.cpp
  src/audio/audio_dsp.h
  src/audio/audio_hw.cpp
  src/audio/audio_hw.h
  src/amports/aformat.h
  src/amports/amstream.h
  src/amports/vformat.h
)
