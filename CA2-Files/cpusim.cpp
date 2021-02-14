#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace std;

/* 
Add all the required standard and developed libraries here
*/

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

struct ControlSignals {
	int RegWrite;
	int ALUSrc;
	int Branch;
	int MemRe;
	int MemWr;
	int MemtoReg;
	string ALUOp;
};

// output of RegisterFile()
struct ReadData {
	string data1; // 32 bits
	string data2; // 32 bits
};

class CPU {

public:
	unsigned char fetchedInstruction[32]; // store fetched instruction in 32-bit unsigned char array
	Instruction decodedInstruction;
	ControlSignals control;
	unsigned int PC;
	char(* CPUinstMem)[4096][8];
	char RF[32][32];
	ReadData RF_output;

	CPU(int _PC, char (&_instMem)[4096][8]) {
		PC = _PC;
		CPUinstMem = &(_instMem);

		//initialize x0 to 0;
		for(int i = 0; i < 32; i++){
			RF[0][i] = 0;
		}
	}

	void Fetch() {
		// fetch the next instruction from the input
		for(int i = PC; i < PC+4; i++){ // each byte
			for(int j = 0; j < 8; j++){ // each bit
				fetchedInstruction[((i-PC)*8)+j] = (*CPUinstMem)[i][j];
			}
		}
		PC += 4; // increment to next PC
	}

	bool Decode() {
		int endCounter = 0;

		string _opcode;
		string _rd;
		string _funct3;
		string _rs1;
		string _rs2;
		string _funct7;

		// parse instruction into fields
		for(int i = 0; i < 32; i++){
			if(i < 7){

				//count number of 0's for END opcode
				if(fetchedInstruction[i] == 0){
					endCounter++;
				}

				if(fetchedInstruction[i] == 1){
					_opcode.insert(0, "1");
				} else if (fetchedInstruction[i] == 0){
					_opcode.insert(0, "0");
				}
			} else if (i >= 7 && i < 12){
				if(fetchedInstruction[i] == 1){
					_rd.insert(0, "1");
				} else if (fetchedInstruction[i] == 0){
					_rd.insert(0, "0");
				}
			} else if (i >= 12 && i < 15){
				if(fetchedInstruction[i] == 1){
					_funct3.insert(0, "1");
				} else if (fetchedInstruction[i] == 0){
					_funct3.insert(0, "0");
				}
			} else if (i >= 15 && i < 20){
				if(fetchedInstruction[i] == 1){
					_rs1.insert(0, "1");
				} else if (fetchedInstruction[i] == 0){
					_rs1.insert(0, "0");
				}
			} else if (i >= 20 && i < 25){
				if(fetchedInstruction[i] == 1){
					_rs2.insert(0, "1");
				} else if (fetchedInstruction[i] == 0){
					_rs2.insert(0, "0");
				}
			} else if (i >= 25 && i < 32){
				if(fetchedInstruction[i] == 1){
					_funct7.insert(0, "1");
				} else if (fetchedInstruction[i] == 0){
					_funct7.insert(0, "0");
				}
			}

			if(fetchedInstruction[i] == 1){
				//printf("fetchedInstruction[%d]: 1\n", i);
			} else if (fetchedInstruction[i] == 0){
				//printf("fetchedInstruction[%d]: 0\n", i);
			}
		}

		//printf("_opcode:%s\n\n", _opcode.c_str());
		//printf("_rd:%s\n", _rd.c_str());

		decodedInstruction.opcode = _opcode;
		decodedInstruction.funct3 = _funct3;
		decodedInstruction.rs1 = _rs1;

		if(_opcode == "0110011") { // R-type
			decodedInstruction.type = "R";
			decodedInstruction.rd = _rd;
			decodedInstruction.rs2 = _rs2;
			decodedInstruction.funct7 = _funct7;
		} else if (_opcode == "0010011"){ // I-type
			decodedInstruction.type = "I";
			decodedInstruction.rd = _rd;
			decodedInstruction.imm = _funct7 + _rs2;
		} else if (_opcode == "0000011"){ // LW
			decodedInstruction.type = "I";
			decodedInstruction.rd = _rd;
			decodedInstruction.imm = _funct7 + _rs2;
		} else if (_opcode == "0100011"){ // SW
			decodedInstruction.type = "S";
			decodedInstruction.imm = _funct7 + _rd;
			decodedInstruction.rs2 = _rs2;
		} else if (_opcode == "1100011"){ // B-type
			decodedInstruction.type = "B";
			decodedInstruction.rs2 = _rs2;
			decodedInstruction.imm = _funct7 + _rd;
		}

		//printf("CPU's decodedInstruction.opcode:%s\n", decodedInstruction.opcode.c_str());
		//printf("endCounter:%d\n", endCounter);

		Controller(decodedInstruction);

		//printf("_rs1: %s, rs2: %s\n", _rs1.c_str(), _rs2.c_str());
		RegisterFile(_rs1, _rs2, "", "", 0);

		if(endCounter == 7){ // OPCODE = 000 0000 (END signal)
			return false;
		}

		return true;
	}

	// take instruction as input and create all required control signals
	void Controller(Instruction& _Instruction) {
		if(_Instruction.opcode == "0110011"){ // R-type
			control.RegWrite = 1;
			control.ALUSrc = 0;
			control.Branch = 0;
			control.MemRe = 0;
			control.MemWr = 0;
			control.MemtoReg = 0;
			if(_Instruction.funct3 == "000"){ // ADD or SUB
				if(_Instruction.funct7 == "0000000"){ // ADD
					control.ALUOp = "0010"; // add
				} else if(_Instruction.funct7 == "0100000"){ // SUB
					control.ALUOp = "0110"; // subtract
				}
			} else if (_Instruction.funct3 == "110"){ // OR
				control.ALUOp = "0001"; // OR
			} else if (_Instruction.funct3 == "111"){ // AND
				control.ALUOp = "0000"; // AND
			}

		} else if (_Instruction.opcode == "0010011"){ // I-type (excl. LW)
			control.RegWrite = 1;
			control.ALUSrc = 1;
			control.Branch = 0;
			control.MemRe = 0;
			control.MemWr = 0;
			control.MemtoReg = 0;
			if(_Instruction.funct3 == "000") { // ADDI
				control.ALUOp = "0010"; // add
			} else if(_Instruction.funct3 == "110"){ // ORI
				control.ALUOp = "0001"; // OR
			} else if(_Instruction.funct3 == "111"){ // ANDI
				control.ALUOp = "0000"; // AND
			}

		} else if (_Instruction.opcode == "0000011"){ // LW
			control.RegWrite = 1;
			control.ALUSrc = 1;
			control.Branch = 0;
			control.MemRe = 1;
			control.MemWr = 0;
			control.MemtoReg = 1;
			control.ALUOp = "0010"; // add
		} else if (_Instruction.opcode == "0100011"){ // S-type
			control.RegWrite = 0;
			control.ALUSrc = 1;
			control.Branch = 0;
			control.MemRe = 0;
			control.MemWr = 1;
			control.MemtoReg = 0;
			control.ALUOp = "0010"; // add
		} else if (_Instruction.opcode == "1100011"){ // B-type
			control.RegWrite = 0;
			control.ALUSrc = 0;
			control.Branch = 1;
			control.MemRe = 0;
			control.MemWr = 0;
			control.MemtoReg = 0;
			control.ALUOp = "0110"; // subtract
		}

		// compute correct immediate once instruction is decoded
	}

	// _read_reg1[5], _read_reg2[5], _write_data[32], _write_reg[5], _RegWrite[1]
	ReadData RegisterFile(string _read_reg1, string _read_reg2, string _write_data, string _write_reg, int _RegWrite) {
		int ulli1, ulli2;
		ulli1 = (int) strtoull(_read_reg1.c_str(), NULL, 2);
		ulli2 = (int) strtoull(_read_reg2.c_str(), NULL, 2);
		//printf("The decimal equivalent is: %d, %d.\n", ulli1, ulli2);
		RF_output.data1 = RF[ulli1];
		RF_output.data2 = RF[ulli2];
		return RF_output;
	}

};

class CPUStat {
public:
	int numFetched;
	int numR_type;
	int numI_type;
	int numS_type;
	int numB_type;
	int numU_type;
	int numJ_type;
	int numSW;
	int numLW;
	int numADD;

	CPU* trackCPU;

	CPUStat(CPU& _trackCPU){
		trackCPU = &(_trackCPU);

		numFetched = 0;
		numR_type = 0;
		numI_type = 0;
		numS_type = 0;
		numB_type = 0;
		numU_type = 0;
		numJ_type = 0;
		numSW = 0;
		numLW = 0;
		numADD = 0;
	}

	void log(){
		numFetched++;
		//printf("decodedInstruction.opcode: %s\n", trackCPU->decodedInstruction.opcode.c_str()); //debug
		if(trackCPU->decodedInstruction.type == "R"){
			numR_type++;

			//check for ADD
			if(trackCPU->decodedInstruction.funct3 == "000" && trackCPU->decodedInstruction.funct7 == "0000000"){
				numADD++;
			}
		} else if(trackCPU->decodedInstruction.type == "I"){
			numI_type++;

			//check for LW
			if(trackCPU->decodedInstruction.opcode == "0000011"){
				numLW++;
			}
		} else if(trackCPU->decodedInstruction.type == "S"){
			numS_type++;
			numSW++;
		} else if(trackCPU->decodedInstruction.type == "B"){
			numB_type++;
		}
	}

};

int main (int argc, char* argv[])
{
	//printf("argc: %d\nargv[0]: %s, argv[1]: %s\n", argc, argv[0], argv[1]); // sanity check
	
	/* This is the front end of your project. 
	You need to first read the instructions that are stored in a file and load them into an instruction memory. 
	*/
	FILE* fin = fopen(argv[1], "r");

	/* Define your Instruction memory here. Each cell should store 1 byte. You can define the memory either dynamically, or define it as a fixed size with size 4MB (i.e., 4096 lines). Each instruction is 32 bits (i.e., 4 lines, saved in little-endian mode). 
	Each line in the input file is stored as an unsigned char and is 1 byte (each four lines are one instruction). You need to read the file line by line and store it into the memory. You may need a mechanism to convert these values to bits so that you can read opcodes, operands, etc. 
	*/ 
	char instMem[4096][8]; //static allocation
	char decWord[5];
	int count = 0;
	int num = 0;
	string x;
	while(fgets(decWord, 5, fin)){// read in the decimal representation of 8 bits (5 values: max 3 decimal places, plus a newline, plus another to flush the newlines)
    	//printf("line %d:\n", count);
    	for(int i = 0; i < 4; i++){ // replace any newline characters
    		if(decWord[i] == '\n'){
    			decWord[i] = 0;
    		}
    		//printf("decWord[%d]: %c\n", i, decWord[i]);
    	}
    	
    	x = string(decWord);
    	num = stoi(x);
    	//printf("num: %d\n", num);
    	int binaryLoop = 0;
    	while(num != 0){ // convert decimal number into binary and store in instMem
    		if(num % 2 == 1){
    			instMem[count][binaryLoop] = 1;
    			//printf("1");
    		} else {
    			instMem[count][binaryLoop] = 0;
    			//printf("0");
    		}
    		num = num / 2;
    		binaryLoop++;
    	}
    	//printf("\ninput: %s\n", decWord);
    	count++;
  	}
  	fclose(fin);

  	//printf("number of lines read:%d\n", count);

  	// this loop prints memory contents
  	/*
  	printf("\n");
  	for(int a = 0; a < count; a++){
		for(int i = 7; i >= 0; i--){
			if(instMem[a][i] == 1){
				printf("1");
			} else {
				printf("0");
			}
			if(i == 4)
				printf(" ");
		}	
		printf("\n");
		if(a % 4 == 3)
			printf("\n");
	}
	*/
	

	/* Instantiate your CPU object here.  CPU class is the main class in this project that defines different components of the processor. 
	CPU class also has different functions for each stage (e.g., fetching an instruction, decoding, etc.). 
	*/
    CPU myCPU = CPU(0, instMem);  // call the approriate constructor here to initialize the processor...  
	// make sure to create a variable for PC and resets it to zero (e.g., unsigned int PC = 0); 

	/* Instatiate your CPUStat object here. CPUStat class is responsible to keep track of all the required statistics. */
    CPUStat myStat = CPUStat(myCPU);


	/* OPTIONAL: Instantiate your Instruction object here. */
	//Instruction myInst; 

    int counter = 0;
    bool keepGoing = true;
	while (1) // processor's main loop. Each iteration is equal to one clock cycle.  
	{
		//printf("counter: %d\n", counter++);

		//fetch
		myCPU.Fetch();

		// decode
		keepGoing = myCPU.Decode();

		if(keepGoing){
			myStat.log();
		} else { // we should break the loop if the current instruction is BREAK instruction (i.e., if opcode == 0)
			break;
		}

		// rest will be added in the next projects ... 
	}


	// clean up the memory (if any)

	// print the stats
	printf("%d\n", myStat.numFetched);

	printf("%d,", myStat.numR_type);
	printf("%d,", myStat.numI_type);
	printf("%d,", myStat.numS_type);
	printf("%d,", myStat.numB_type);
	printf("%d,", myStat.numU_type);
	printf("%d\n", myStat.numJ_type);

	printf("%d,", myStat.numSW);
	printf("%d,", myStat.numLW);
	printf("%d\n", myStat.numADD);

	return 0; 
}