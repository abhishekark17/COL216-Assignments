
# Instructions

First unzip the folder and open terminal into the current directory.</br>
It is recommended to put all the test files in a single folder in the INPUT folder and obtain the output in a single folder in the OUTPUT folder for clarity.</br>
Go into the code folder by using:</br>
`cd code`</br>

For compiling the code use: `make` or `make all`.</br>
The Makefile will generate the executable named `main.out`</br>

There should be exactly 7 arguments in the following order: </br>
`inputFolder outputFolder maxClockCycles numberOfCores coreQueueLength rowAccessDelay colAccessDelay` </br>

First argument: the input folder to be simulated which will have mips instruction files for all the cores named 1.txt, 2.txt and so on.</br>

Second argument: the output folder in which the program will give the outputs for all the cores named 1o.txt, 2o.txt and so on along with the DRAM output. </br>

Third argument: maximum number of clock cycles for which the program should run. </br>
Fourth argument: number of cores the CPU should have.</br>
Fifth argument: length of each core's buffer.</br>
Sixth argument: row access delay for the DRAM.</br>
Seventh argument: column access delay for the DRAM.</br>

Example:</br>
`./main.out ../INPUT/tc1 ../OUTPUT/tc1 200 4 3 10 2`</br>

Note: if there aren't exactly 7 arguments then program won't be executed.</br>
