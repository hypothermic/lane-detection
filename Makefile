#
# Usage:
# $ make {compile,clean}-docs
# 

#
# Options
#

LATEXMK_EXEC	?= /usr/bin/latexmk
LATEXMK_OUT	?= ./build/
LATEXMK_OPTS	?= -silent -emulate-aux-dir -auxdir=docs/ -xelatex -output-directory=$(LATEXMK_OUT)
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

