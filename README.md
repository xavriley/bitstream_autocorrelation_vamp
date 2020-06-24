# Bitstream Autocorrelation Pitch Tracking VAMP plugin

This is an experiment to implement [Bitstream
Autocorrelation](https://www.cycfi.com/2018/04/fast-and-efficient-pitch-detection-bliss/)
as a VAMP plugin.

This uses the [Q Audio DSP Library](https://github.com/cycfi/Q)

## Installation

TODO

```bash
$ make -f Makefile.osx 
```

## Examples

TODO

```bash
$ make -f Makefile.osx 
$ VAMP_PATH=. ~/Downloads/vamp-plugin-sdk-2.9.0-binaries-macos/vamp-simple-host myplugins:myplugin ./Q/test/audio_files/sin_440.wav

vamp-simple-host: Running...
Reading file: "/Users/xavierriley/Projects/Q/test/audio_files/sin_440.wav", writing to standard output
Running plugin: "myplugin"...
Using block size = 1024, step size = 1024
Plugin accepts 1 -> 1 channel(s)
Sound file has 1 (will mix/augment if necessary)
Output is: "output"
 0.000000000: 439.963
 0.023219955:
 0.046439909:
 0.069659864:
 0.092879819:
 0.116099773:
 0.139319728:
```

## Parameters

TODO

## Todo

-[x] bring in Q as submodule
-[ ] document build steps
-[ ] rename plugin
-[ ] fixup metadata
-[ ] implement params (lowest freq, highest freq)
-[ ] add some kind of smoothing
-[ ] look into octave jumps
