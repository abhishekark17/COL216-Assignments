First unzip the folder and open terminal into the current directory.</br>
It is recommended to put the test case in the `tcs` folder and obtain the output in the `outputs` folder for clarity. </br>
Go into the `code` folder by using:</br>
    `cd code` </br>

For compiling the code use make or make all.</br>
    `make` </br>
The Makefile will generate the executable named `main.out`</br>

The first argument will be the input file to be simulated.</br>
If the input file name is test.txt (assuming in `tcs` folder) then for execution use: `./main.out ../tcs/test.txt.`</br>

The second and third argument (both necessary) will be ROW_ACCESS_DELAY and COL_ACCESS_DELAY respectively.</br>
The fourth argument (necessary) of command line will be whether to use Part-1 of Assignment or Part-2 of Assignment.</br>
Give the fourth argument as 1 for part-1 and 2 for part-2.</br>

An additional fifth argument (optional) can also be given at the end of the command line argument if you want to store the output in a file.</br>
Example: </br>
`./main.out ../tcs/<inputFilename>.txt 10 2 1 ../outputs/<optionalOutputFilename>.txt` </br>

Any further arguments will simply be ignored. </br>