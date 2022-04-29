## Lane Detection Internship [![Compile Documents](https://github.com/hypothermic/lane-detection/actions/workflows/compile-documents.yml/badge.svg?branch=master&event=push)](https://github.com/hypothermic/lane-detection/actions/workflows/compile-documents.yml)

The goal of this project is to accelerate road lane detection using an FPGA.

![Preview picture](./data/preview.png)

This repository contains the documentation and code for all of the deliverables.

Automatically generated PDFs from the latest commit of the documents can be found on the [Actions Workflow](https://github.com/hypothermic/lane-detection/actions/workflows/compile-documents.yml).

## Documentation

Latexmk is used to simplify the build process.
The Makefile is a wrapper that calls Latexmk and the needed commands.
To compile the documentation to PDF/A: 

```shell
$ make compile-docs
$ find ./build/ -iname "*.pdf"
```

To clean the generated files:

```shell
$ make clean-docs
```

## Test Program

To compile the program and run the automatic unit tests,
you can call the following Make targets:

```shell
$ make clean-lane compile-lane test-lane
```

To manually run a test and view the resulting image in your
default image viewing program, run something like...

```shell
$ make DEBUG=true test-lane-man ARG_TEST=sample ARG_SAMPLE=0a0a0b1a-7c39d84
```

... where ARG\_TEST is the name of a test
and ARG\_SAMPLE is the name of a photo in the data directory.
Please note that the DEBUG variable determines if Valgrind is
used when executing the program.
Images using the manual test target are opened with xdg-open.

And to run multiple tests (e.g. a pipeline of multiple filters)
you can use the helper script:

```shell
$ DEBUG=true SAMPLE=0a0a0b1a-7c39d841 ./test.sh grayscale kmseg gaussian sobel hough_kmeans
```

The sample images were extracted from the Berkeley DeepDrive
dataset. Make sure to extract them using `./data/extract.sh`

## FPGA Hardware

The hardware build is separated into two parts; the VPU IP core
and the total system implementation with AXI infra and Zynq PS.

To run (co-)simulation tests for the IP core:

```shell
$ make test-hls-sim test-hls-cosim
```

To build the IP Core standalone:

```shell
$ make compile-hls
$ file build/lane_vpu.prj/lane_vpu/impl/export.zip
```

To build the total system and implement to bitstream:

```shell
$ make compile-hw
$ find -iname "*.bit" -o -iname "*.hwh"
```

If you use the PYNQ framework on the TUL Z2, copy the Jupyter
notebook and the Python modules along with the bitstream and
hardware handoff file to the development board.

