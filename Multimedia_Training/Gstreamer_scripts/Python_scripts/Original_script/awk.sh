#!/bin/bash

awk '{print}' logfile.txt


echo $1



# Another function that uses explicitly provided arguments
another_function() {
    local arg3=$1
    local arg4=$2
    echo "Arguments received in another function: arg3=$arg3, arg4=$arg4"
}

# Call another function with explicitly provided arguments
another_function "road" "transport"
