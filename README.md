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
$ cd Q && mkdir build && cd build && cmake -B ./build -S ./ -G"Unix Makefiles" && cd build && make && cd ../../
# Assumes you are on OS X 
# Use the appropriate Makefile for your operating system
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
 0.000000000: 0
 0.030589570: 440.063
 0.040612245: 439.953
 0.051043084: 439.953
 0.060929705: 439.939
 0.071088435: 439.719
 0.081247166: 439.304
 0.092879819: 0
 0.116099773: 0
 0.139319728: 0
```

## Parameters

Changing these affects the output slightly due to how the algorithm works.

One constraint is that they need to be within 4 octaves of each other.

* `lowestPitch`: Lowest pitch to be tracked. Defaults to 100Hz
* `highestPitch`: Highest pitch to be tracked. Defaults to 800Hz

## Todo

- [x] bring in Q as submodule
- [x] document build steps
- [x] rename plugin
- [x] fixup metadata
- [x] implement params (lowest freq, highest freq)
- [x] add some kind of smoothing
- [x] look into octave jumps
- [x] Make hysterisis a parameter
- [x] Output frequencies with timestamps rather than having them grouped by sample windows
- [ ] Optionally output frequencies at regular intervals (for MedleyDB comparison)
- [ ] Make pre-processing code optional via a param

