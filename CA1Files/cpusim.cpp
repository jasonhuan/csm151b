#include <stdio.h>
#include <string>

using namespace std;

/* 
Add all the required standard and developed libraries here
*/


class CPU {

public:

	CPU() {
		// constructor
	}

	void Fetch() {
		// fetch the next instruction from the input
	}

	void Decode() {
		// decode the currently fetched instruction
	}
};

struct Instruction {
	string opcode;
	string rd;
	string rs1;
	string rs2;
	string imm;
	string funct3;
	string funct7;
	string type;
};

class CPUStat {

};


/*
Put/Define any helper function/definitions you need here
*/

int main (int argc, char* argv[])
{
	printf("argc: %d\nargv[0]: %s, argv[1]: %s\n", argc, argv[0], argv[1]); // sanity check
	
	/* This is the front end of your project. 
	You need to first read the instructions that are stored in a file and load them into an instruction memory. 
	*/

	char decWord[4];
	char binWord[8];
	FILE* fin = fopen(argv[1], "r");
	if(fin! = NULL){
    	fgets(decWord, 4, fin); // read in the decimal representation of 8 bits (max 3 decimal places, plus a newline)
    	for(int i = 1; i < 4; i++){ // replace any newline characters
    		if(decWord[i] == '\n'){
    			decWord[i] = 0;
    		}
    	}
    	puts(decWord); // print
  	}
  	fclose(fin);
	

	/* Define your Instruction memory here. Each cell should store 1 byte. You can define the memory either dynamically, or define it as a fixed size with size 4MB (i.e., 4096 lines). Each instruction is 32 bits (i.e., 4 lines, saved in little-endian mode). 
	
	Each line in the input file is stored as an unsigned char and is 1 byte (each four lines are one instruction). You need to read the file line by line and store it into the memory. You may need a mechanism to convert these values to bits so that you can read opcodes, operands, etc. 
	*/ 
	//instMem = ... 

	

	/* Instantiate your CPU object here.  CPU class is the main class in this project that defines different components of the processor. 
	CPU class also has different functions for each stage (e.g., fetching an instruction, decoding, etc.). 
	*/
    //CPU myCPU ...  // call the approriate constructor here to initialize the processor...  
	// make sure to create a variable for PC and resets it to zero (e.g., unsigned int PC = 0); 

	/* Instatiate your CPUStat object here. CPUStat class is responsible to keep track of all the required statistics. */
    //CPUStat myStat ... 


	/* OPTIONAL: Instantiate your Instruction object here. */
	//Instruction myInst; 

	/*
	while (1) // processor's main loop. Each iteration is equal to one clock cycle.  
	{
		//fetch
		... = myCPU.Fetch(...) // fetching the instruction

		// decode
		... = myCPU.Decode(...) // decoding

		// rest will be added in the next projects ... 

		// we should break the loop if the current instruction is BREAK instruction (i.e., if opcode == 0)
	}
	*/

	// clean up the memory (if any)

	// print the stats

	return 0; 

}