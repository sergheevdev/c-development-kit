#!/bin/bash

# Cleanup old files
rm -rf main
rm -rf report.txt

# Compile and run with valgrind
gcc -O0 -g -o main string-builder.c
valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         --log-file=report.txt \
         ./main

echo "Compilation & run successful!"
read -r -p "Press any key to open the report..."

# Open report at the last line
LINES_AMOUNT=$(wc -l < report.txt)
nano +"$LINES_AMOUNT" report.txt

# Goodbye
echo "All done! Bye bye!"