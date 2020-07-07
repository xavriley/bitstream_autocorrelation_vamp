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
Output is: "frequency"
 0.020589570: 440.063
 0.030612245: 439.953
 0.041043084: 439.953
 0.050929705: 439.939
 0.061088435: 439.719
 0.071247166: 439.304
```

## Parameters

Changing these affects the output slightly due to how the algorithm works.

One constraint is that they need to be within 4 octaves of each other.

* `lowestPitch`: Lowest pitch to be tracked. Defaults to 100Hz
* `highestPitch`: Highest pitch to be tracked. Defaults to 800Hz
* `threshold`: Decibel level at which to give up detecting pitches. Defaults to -45dB
* `regularOutput`: Whether to output frequencies at regular intervals, or as and when they are detected. When enabled this can help to line up with output of similarly generated datasets. Off by default.
* `regularOutputStep`: Number of samples to use as a step size with regularOutput - defaults to 128

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
- [x] Optionally output frequencies at regular intervals (for MedleyDB comparison) - fix to work with offset
- [ ] Make pre-processing code optional via a param

