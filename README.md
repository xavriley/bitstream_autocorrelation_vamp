# Bitstream Autocorrelation Pitch Tracking VAMP plugin

This is an experiment to implement [Bitstream
Autocorrelation](https://www.cycfi.com/2018/04/fast-and-efficient-pitch-detection-bliss/)
as a VAMP plugin.

This uses the [Q Audio DSP Library](https://github.com/cycfi/Q)

## Installation

* in a parent folder, clone and build the vamp-sdk
* move back to the parent folder
* clone this repo like so:

```bash
git clone --recurse-submodules git@github.com:xavriley/bistream_autocorrelation_vamp.git
```

* cd into the repo and run

```bash

$ make -f Makefile.osx 
```

## Examples

```bash
$ make -f Makefile.osx && VAMP_PATH=. ~/Downloads/vamp-plugin-sdk-2.9.0-binaries-macos/vamp-simple-host qlib_pitch:qlib_pitch ./Q/test/audio_files/sin_440.wav

vamp-simple-host: Running...
Reading file: "/Users/xavierriley/Projects/Q/test/audio_files/sin_440.wav", writing to standard output
Running plugin: "qlib_pitch"...
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

*Warning* these are a work in progress - changing them affects the output of the tracker. This is being looked at.

* `lowestPitch`: Lowest pitch to be tracked. Defaults to 100Hz
* `highestPitch`: Highest pitch to be tracked. Defaults to 800Hz

## Todo

- [x] bring in Q as submodule
- [x] document build steps
- [x] rename plugin
- [x] fixup metadata
- [x] implement params (lowest freq, highest freq)
- [ ] add some kind of smoothing
- [ ] look into octave jumps
