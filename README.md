# Lane Detection Internship [![Compile Documents](https://github.com/hypothermic/lane-detection/actions/workflows/compile-documents.yml/badge.svg?branch=master&event=push)](https://github.com/hypothermic/lane-detection/actions/workflows/compile-documents.yml)

This repository contains the documentation and code for all of the deliverables.

Automatically generated PDFs from the latest commit of the documents can be found on the [Actions Workflow](https://github.com/hypothermic/lane-detection/actions/workflows/compile-documents.yml).

## Documentation

Latexmk is used to simplify the build process.
The Makefile is a wrapper that calls Latexmk and the needed commands.
To compile the documentation to PDF/A: 

```shell
$ make compile-docs
```

To clean the generated files:

```shell
$ make clean-docs
```

# Lane Program

To compile the program and run the automatic unit tests,
you can call the following Make targets:

```shell
$ make clean-lane compile-lane test
```

To manually run a test and view the resulting image in your
image viewing program, run something like...

```shell
make DEBUG=true test-lane-man ARG_TEST=sample ARG_SAMPLE=0a0a0b1a-7c39d84
```

... where ARG\_TEST is the name of a test
and ARG\_SAMPLE is the name of a photo in the data directory.

