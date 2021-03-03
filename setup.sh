#!/bin/bash -ex
# The -e option would make our script exit with an error if any command 
# fails while the -x option makes verbosely it output what it does

# Updating the apt-get
sudo apt-update

# Installing build-essential package to build the program.
sudo apt-get install build-essential