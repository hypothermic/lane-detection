#!/usr/bin/zsh
#
# This is a script interpreter that executes the argument Tcl file in Vivado Tcl mode.
# Put the shebang "#!/path/to/vinterp.sh" at the top of your Tcl file to use it.
# If the Xilinx environment is not sourced, this script tries to source it by checking
# if it's installed at the default path (/opt/Xilinx) or else exits with an error.
# -- mbr 2021
#

# Working directory of the target script
WORK_DIR=$(dirname "$1")

# Relative path to the directory to store the log and journal in
LOG_DIR=${2:=/build}

# The default path of the Vivado installation
VIVADO_DIR_DEFAULT=/opt/Xilinx/Vivado

# The user specified Vivado installation path (may be undefined)
VIVADO_DIR_ARG=${3:=${VIVADO_DIR_DEFAULT}}

# Use a regex to find all Vivado installations (yyyy.v) and sort it in descending order
# so that the latest version appears as the first element in the array
VIVADO_DIR_GLOB=(${(Ofi)"$(find "$VIVADO_DIR_DEFAULT" -maxdepth 1 -type d | grep -P "\d{4}.\d{1}")"})

# Check if the input file exists and is a regular file
if [ ! -f "$1" ]; then
	echo "No such file: \'$1\'"
	exit 1
fi

# Check if Vivado environment is already sourced
if (( ! $+commands[vivado] )); then
	# If no installation paths were found
	if [ 0 -eq ${#VIVADO_DIR_GLOB[@]} ]; then
		echo "Vivado not found, please source the environment first"
		exit 2
	fi

	# Check if the settings file exists before we source it
	SETTINGS="${VIVADO_DIR_GLOB[1]}/settings64.sh"
	if [ ! -f "${SETTINGS}" ]; then
		echo "File settings64.sh not found at expected path \'${SETTINGS}\'"
		exit 3
	fi

	# We found the latest environment correctly, now source the settings file.
	source "${SETTINGS}"
fi

# Change directory to the working directory
cd $WORK_DIR

# Launch vivado in Tcl mode with the log and journal outputting to LOG_DIR
vivado -mode tcl \
	-source $1 \
	-verbose \
	-log $WORK_DIR/$LOG_DIR/vivado.log \
	-journal $WORK_DIR/$LOG_DIR/vivado.jou

times

