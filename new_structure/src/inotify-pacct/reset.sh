#!/bin/bash

# Directory where the files are located
DIR="/var/log/pacct"

# Files to be checked
FILE1="$DIR/acct"

# Function to remove a file if it exists
remove_file_if_exists() {
    if [ -f "$1" ]; then
        sudo rm "$1"
        echo "Removed $1"
    fi
}

# Create the directory if it does not exist
if [ ! -d "$DIR" ]; then
    sudo mkdir -p "$DIR"
    echo "Created directory $DIR"
fi

# Remove the files if they exist
remove_file_if_exists "$FILE1"

# Recreate the acct file
sudo touch "$FILE1"
echo "Recreated $FILE1"
