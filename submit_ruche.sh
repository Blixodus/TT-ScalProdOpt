#!/bin/bash

# Define the config path
DIR_PATH="/gpfs/workdir/torria/pdominik/OptiTenseurs/tools/configs"

# Check if the directory exists
if [ -d "$DIR_PATH" ]; then
  # Loop through all .ini files in the directory
  for ini_file in "$DIR_PATH"/*.ini; 
  do
    # Check if the file exists to handle cases where no .ini files are found
    if [ -f "$ini_file" ]; then
      # Execute the command.sh script with the ini file as an argument
      echo "Submitting task for $ini_file"
      sbatch tensor_experiment_dim2.sh "$ini_file"
    else
      echo "No .ini files found in $DIR_PATH"
    fi
  done
else
  echo "Directory $DIR_PATH does not exist"
fi