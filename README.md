# Lane Detection Internship [![Compile Documents](https://github.com/hypothermic/lane-detection/actions/workflows/compile-documents.yml/badge.svg?branch=master&event=push)](https://github.com/hypothermic/lane-detection/actions/workflows/compile-documents.yml)

This repository contains the documentation and code for all of the deliverables.

Automatically generated PDFs from the latest commit of the documents can be found on the [Actions Workflow](https://github.com/hypothermic/lane-detection/actions/workflows/compile-documents.yml).

## Documentation

Latexmk is used to simplify the build process.
The Makefile is a wrapper that calls Latexmk and the needed commands.
To compile the documentation to PDF/A: 

```
$ make compile-docs
```

To clean the generated files:

```
$ make clean-docs
```

