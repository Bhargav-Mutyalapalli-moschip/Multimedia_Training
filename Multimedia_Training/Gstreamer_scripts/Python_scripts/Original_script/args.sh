#!/bin/bash

# Function that uses command-line arguments
process_arguments() {
    local arg1=$1
    local arg2=$2
    echo "Arguments received in the function: arg1=$arg1, arg2=$arg2"
}

process_arguments "road" "transport"

# Another function that uses command-line arguments
another_function() {
	local arg3=$1
	local arg4=$2
	echo "Arguments received in another function: arg3=$arg3, arg4=$arg4"
}

# Call another function with command-line arguments
another_function "$3" "$4"

