// ABHINAV SINGHAL 2019CS50768
// ABHISHEK KUMAR 2019CS10458

#include "main.hpp"


int main(int argc, char **argv)
{
	initialize_registers();
	registers[rmap["$sp"]] = (1 << 20) - 4;

	// argv = 0 -  :: 1 - inputfile :: 2 - ROW_ACCESS_DELAY :: 3 - COL_ACCESS_DELAY :: 4 - (1) for part1 (2) for part2  ::5 - OUTFILE (optional)
	if (argc == 1)
	{
		cout << "Input File should be given as argument" << endl;
		return 0;
	}
	else if (argc == 2 || argc == 3 || argc == 4)
	{
		cout << "Please give ROW_ACCESS_DELAY, COL_ACCESS_DELAY and Assignment part (1 or 2) as arguments" << endl;
		return 0;
	}
	else
	{
		file = argv[1];
		ROW_ACCESS_DELAY = stoi(argv[2]);
		COL_ACCESS_DELAY = stoi(argv[3]);
		if (stoi(argv[4]) == 2)
			part2enabled = true;
		if (argc == 6)
			outfile = argv[5];
	}

	readFile(file);
	preprocess(); // all input has been read and stored in iset.
	sizeOfData = (1 << 18) - numberofInst;

	maxInstructions = numberofInst;
	ofstream outstream(outfile);

	if (error.size() > 0)
	{
		for (int i = 0; i < error.size(); i++)
		{
			cout << error[i] << "\n\n";
			outstream << error[i] << "\n\n";
		}
		outstream.close();
		return 0;
	}

	if (part2enabled)
		execute2(outstream);
	else 
		execute1a4 (outstream);
		//execute1(outstream);

	if (currentRowInRowBuffer != -1)
	{
		exectutionOutput.push_back("cycle " + to_string(max(numClockCycles, uptoClkCyc) + 1) + "-" + to_string(max(numClockCycles, uptoClkCyc) + ROW_ACCESS_DELAY) + ": " + "WriteBack Row " + to_string(currentRowInRowBuffer) + " to DRAM");
		numClockCycles = max(numClockCycles, uptoClkCyc) + ROW_ACCESS_DELAY;
	}

	if (error1 == "")
	{
		cout << endl;
		outstream << endl;
		cout << "Total Number of Clock Cycles: " << max(numClockCycles, uptoClkCyc) << endl
			 << endl;
		outstream << "Total Number of Clock Cycles: " << max(numClockCycles, uptoClkCyc) << endl
				  << endl;

		cout << "Total Number of Row Buffer Updates: " << numRowBufferUpdates << endl
			 << endl;
		outstream << "Total Number of Row Buffer Updates: " << numRowBufferUpdates << endl
				  << endl;

		cout << "Memory content at the end of the execution:\n\n";
		outstream << "Memory content at the end of the execution:\n\n";

		for (int i = numberofInst; i < (1 << 18); i++)
		{
			if (data123[i - numberofInst] != 0)
			{
				cout << (i)*4 << "-" << (i)*4 + 3 << ": " << hex << data123[i - numberofInst] << dec << endl;
				outstream << (i)*4 << "-" << (i)*4 + 3 << ": " << hex << data123[i - numberofInst] << dec << endl;
			}
		}
		cout << endl;
		outstream << endl;

		cout << "Every cycle description: \n\n";
		outstream << "Every cycle description: \n\n";

		for (string x : exectutionOutput)
		{
			cout << x << endl;
			outstream << x << endl;
		}

		cout << endl;
		outstream << endl;
		cout << "END OF EXECUTION. META DATA: \n\n";
		outstream << "END OF EXECUTION. META DATA: \n\n";

		int totalValidinstruction = 0;

		cout << "Number of times each instruction was executed: \n\n";
		outstream << "Number of times each instruction was executed: \n\n";

		cout << "[";
		outstream << "[";
		for (int i = 0; i < numOfInst.size() - 1; i++)
		{
			totalValidinstruction += numOfInst[i];
			cout << myset[i] << ": " << numOfInst[i] << ",";
			outstream << myset[i] << ": " << numOfInst[i] << ",";
		}
		cout << myset[numOfInst.size() - 1] << ": " << numOfInst[numOfInst.size() - 1] << "]\n\n";
		outstream << myset[numOfInst.size() - 1] << ": " << numOfInst[numOfInst.size() - 1] << "]\n\n";
	}

	outstream.close();
	return 0;
}