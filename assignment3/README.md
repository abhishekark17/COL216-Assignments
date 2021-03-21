First unzip the folder.<br/>
Put the input file in the same folder.<br/>
For compiling the code use `make` or `make all`.<br/>

The first argument will be the input file to be simulated. <br/>
If the input file name is `test.txt` then for execution use `./main.out test.txt`.<br/>

The second and third argument will be ROW_ACCESS_DELAY and COL_ACCESS_DELAY respectively.<br/>
The fourth argument of command line will be whether to use Part-1 of Assignment or Part-2 of Assignment.<br/>
Give the fourth argument as 1 for part-1 and 2 for part-2. <br/>

An additional fifth argument can also be given at the end of the command line argument if you want to store the output in a file. <br/>
Example: <br/>
`./main.out <inputFilename>.txt 10 2 1 <optionalOutputFilename>.txt`.<br/>
 
 Any further arguments will simply be ignored. <br/>
 