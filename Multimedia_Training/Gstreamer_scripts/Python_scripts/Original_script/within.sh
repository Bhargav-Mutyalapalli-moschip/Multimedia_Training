#!/bin/bash

# Function that uses command-line arguments
process_arguments() {
    local arg1=$1
    local arg2=$2
    echo "Arguments received in the function: arg1=$arg1, arg2=$arg2"
}

# Check if at least two command-line arguments are provided
if [ "$#" -ge 2 ]; then
    # Call the function with command-line arguments
    process_arguments "$1" "$2"
else
    echo "Usage: $0 <argument1> <argument2>"
    exit 1
fi

# Another function that uses explicitly provided arguments
another_function() {
    local arg3=$1
    local arg4=$2
    echo "Arguments received in another function: arg3=$arg3, arg4=$arg4"
}

# Call another function with explicitly provided arguments
another_function "road" "transport"

