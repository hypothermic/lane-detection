#
# Usage:
# $ make {compile,clean}-{docs,lane,rtl}
# 

.PHONY: all
all: clean compile test

#
# Options
#

LATEXMK_OUT	?= ./build/
LATEXMK_OPTS	?= -silent -emulate-aux-dir -auxdir=docs/ -xelatex -output-directory=$(LATEXMK_OUT)

LANE_SRCS	?= $(wildcard ./src/lane_*.c)
LANE_TESTS	?= $(wildcard ./test/lane_*_test.c)
LANE_OUT	?= ./build/lane
LANE_OPTS	?= -Wall -Werror -Wno-error=unknown-pragmas

LATEXMK_EXEC	?= /usr/bin/latexmk
GCC_EXEC	?= /usr/bin/gcc
RM_EXEC		?= /usr/bin/rm
MKDIR_EXEC	?= /usr/bin/mkdir

#
# Compilation-related targets
#

compile-docs-project:
	$(LATEXMK_EXEC) $(LATEXMK_OPTS) docs/project.tex

compile-docs-timetable:
	$(LATEXMK_EXEC) $(LATEXMK_OPTS) docs/timetable.tex

make-out-dir:
	$(MKDIR_EXEC) -p $(LATEXMK_OUT)

compile-docs: make-out-dir compile-docs-project compile-docs-timetable

compile-lane: make-out-dir
	$(GCC_EXEC) src/lane_*.c -o $(LANE_OUT)

compile: compile-docs compile-lane

#
# Cleaning-related targets
#

clean-docs-project:
	$(LATEXMK_EXEC) -CA docs/project.tex

clean-docs-timetable:
	$(LATEXMK_EXEC) -CA docs/timetable.tex

delete-out-dir:
	$(RM_EXEC) -r $(LATEXMK_OUT)

clean-docs: delete-out-dir clean-docs-project clean-docs-timetable

clean-lane:
	$(RM_EXEC) -f $(LANE_OUT)

clean: clean-docs clean-lane

#
# Program execution target
#

run-lane:
	$(LANE_OUT) data/0a0a0b1a-7c39d841.ppm data/0a0a0b1a-7c39d841.out.ppm

run: run-lane

#
# Test
#

test-lane-compile: compile-lane
	$(GCC_EXEC) $(LANE_TESTS) $(filter-out ./src/lane_main.c, $(LANE_SRCS)) -I ./src/ -o build/lane_copy_test

test-lane-exec: test-lane-compile
	build/lane_copy_test data/0a0a0b1a-7c39d841.ppm data/0a0a0b1a-7c39d841.out.ppm

test-lane-verify: test-lane-exec
	test/lane_copy_test.sh

test-lane: test-lane-verify

test: test-lane

