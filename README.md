# OperationSystems-FileComperation_part2
program that accepts the name and path of a configuration file as a parameter to main.
This configuration file contains 3 lines:
Line 1: The location of a folder containing sub-folders (each subfolder is actually a user name) containing C files.
Line 2: The location and name of a file that contains input
Line 3: The location and name of a file that contains the correct output for input from row 2.

The program must enter all sub-folders within the folder from row 1, search the folder (and subdirectory)
C file, compile the C file that appears in the folder and run it.
*can assume that there will be at most one C file in a folder (there may be no C file at all)

As a result of the runing the c file with the input contained in the position from row 2.
The output of the program should be compared with the desired output whose position comes from line 3 using the program
out.comp that are actually in my github repository called OperationSystem-FileComperation) simply run the program and give it
as parameters to the main the position of the correct output [found in the configuration file in row 3] and
the output of the program of the user you ran.

The program needs to create a file (in the folder from which your program was run) called results.csv
Which contains for each user name (sub-folder name) its score according to the answer that out.comp returned
(Zero to 100) and the reason, the character "," must be entered between the user's name and his or her mark and between his or her reason mark.

possible reasons:
1. NO_C_FILE - There is no file (or subfolders) in the file with the extension c.
2. COMPILATION_ERROR - Compilation problem (file not compiled).
3. TIMEOUT - Exercise runs more than 5 seconds. The grade given, 0.
4. BAD_OUTPUT - The output is different from the output you want. The grade given, 60.
5. SIMILAR_OUTPUT - The output is different from the desired but similar output. The grade to be given, 80.
6. GREAT_JOB - The output is the same as the desired output. The score given is 100.



