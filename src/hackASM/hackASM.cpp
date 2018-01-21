/************************************************************************-
 *	hackASM.cpp, the implementation for hackASM.h.
 *  
 * 
 *	Started: December 15, 2017
 *  Finished: December 22, 2017
 *  Updates:
 *		- 
 *	©2018 C. A. Acred all rights reserved.
 ----------------------------------------------------------*
*/
#include "hackASM.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <bitset>

// Resolver Implementations: 
/**
 * Resolves input to be without whitespace, comments and variables/labels.
 * Saves result in this->output.
 * 
 * @param input unresolved asm code as string pointer.
 */
Resolver::Resolver(string* input)
{
	initializeVars(); // Add built-in variables.
	
    varCounter = 0;
	lineCounter = 0;
	
	*input = resolveExcess(input); // Find and remove all white space, excess newlines, and comments.
	
	*input = resolveLabels(input); // Find, add, and remove labels from input.
	
	*input = resolveSymbols(input); // Find, add, and replace all symbols from input.
	
	output = *input;
	
	return;
}

/**
 * Returns true if *input is a number.
 *
 * @param input Pointer to the string that is being tested.
 * @return true if *input is a number.
 */
bool Resolver::isNumber(string* input)
{
    return !(*input).empty() && std::find_if((*input).begin(), 
        (*input).end(), [](char c) { return !std::isdigit(c); }) == (*input).end();
}

/**
 * Adds a variable to both varNames and varRegNums.
 * The number associated with the variable will be VAR_ASSIGN_ADD_START + varCounter. 
 * This number correlates to the register the code will point to.
 * 
 * @param name The name of the variable
 * @param isLabel If name is a label, should be true.
 * @return The value of the new variable's register number as a string.
 */
string Resolver::addVar(string name, bool isLabel)
{
	string temp;
	varNames.push_back(name);
	if (!isLabel)
	{
		temp = to_string(this->VAR_ASSIGN_ADD_START + varCounter);
		varRegNums.push_back(temp);
		varCounter++;
	}
	else 
	{
		temp = to_string(lineCounter);
		varRegNums.push_back(temp);
	}
	
	return temp;
}

/**
 * Adds a variable to both varNames and varRegNumm.
 * The number value will be the value in register.
 * 
 * @param name The name of the variable
 * @param register The value of the register correlating to this variable.
 * @return The value of the new variable's register number as a string.
 */
string Resolver::addVar(string name, int reg)
{
	string temp;
	varNames.push_back(name);
	temp = to_string(reg);
	varRegNums.push_back(temp);
	
	return temp;
}

/**
 * Resolves the variable name. If it is a number, it simply returns the number as it is not a variable.
 *
 * @param name The name of the potential variable
 * @param isLabel true if the var is a label.
 * @return Returns as a string the int value associated with this variable, or the int value from the asm file if it is not a variable.
 */
string Resolver::resolveVar(string name, bool isLabel)
{
	if(!isNumber(&name)) // If the name is not a number, but a symbol
	{
		string varRegVal = findVar(name);
		if (varRegVal == "-1") // If the var does not exist, add it.
		{
			return addVar(name, isLabel);
		}
		else
			return varRegVal;
	}
	else 
	{
		return name;
	}
}

/**
 * Finds a variable in varNames and returns the corresponding value from varRegNums.
 * Returns "-1" if not found.
 *
 * @param name The string of the variable's name.
 * @return The register number of the variable as a string.
 */
string Resolver::findVar(string name)
{
	for (int i = 0; i < varNames.size(); i++)
	{
		if (varNames.at(i) == name)
			return varRegNums.at(i);
	}
	return "-1";
}

/**
 * Gets the string output.
 * 
 * @return string output
 */
string Resolver::getOutput()
{
    return this->output;
}

/**
 * Initializes the built in symbols. 
 */
void Resolver::initializeVars()
{
	string temp = "";
	for(int i = 0; i < 16; i++)
	{
		temp = "R" + std::to_string(i);
		addVar(temp, i);
	}
	addVar("SCREEN", 16384);
	addVar("KBD", 24576);
	addVar("SP", 0);
	addVar("LCL", 1);
	addVar("ARG", 2);
	addVar("THIS", 3);
	addVar("THAT", 4);
	return;
}

/**
 * Finds, adds, and removes label declarations from input.
 *
 * @param input The pointer to the string you wish to resolve.
 * @return The resolved version of input.
 */
string Resolver::resolveLabels(string* input)
{
	int i = 0;
	char curChar = ' ';
	vector<string> temp;
	string output = "";
	
    while (curChar != '\0')// Iterate through input until it ends.
    {
		curChar = input->at(i);
		if (curChar == '(') // Check for labels, add them.
		{
			temp = Assembler::getLine(input, i+1, ')');
			resolveVar(temp.at(0), true);
			i += atoi(temp.at(1).c_str()); // Adjust the i to properly skip the aforementioned line.
			i += 2; // Skip the closing ')' and the '\n'.
		}
		else
		{
			if (curChar == '\n') // If we are at a new line, increase line counter.
				lineCounter++;
			output.append(1, curChar);
			i++;
		}
	}
	output.append(1, '\0');
	return output;
}

/**
 * Removes whitespace and comments from input.
 *
 * @param input The pointer to the string you wish to resolve.
 * @return The resolved version of input.
 */
string Resolver::resolveExcess(string* input)
{
	int i = 0;
	char curChar = input->at(0);
	vector<string> temp;
	string output = "";
	string curLine = this->EMPTY_STR;
	
	while (curChar != '\0')// Iterate through input until it ends.
    {
        if (curChar == '/' && input->at(i+1) == '/') // If this line is a comment, skip it.
        {
            temp = Assembler::getLine(input, i);
            i += atoi(temp.at(1).c_str()); // Skip '\n' char
			if (curLine != this->EMPTY_STR) // If this is a comment after a command:
			{
				output.append(curLine);  // Add curLine to output plus '\n'.
				output.append(1, '\n');
				curLine = this->EMPTY_STR;
			}
        }
		else if (curChar == '\n') // If it's a new line char:
		{
			if (curLine != this->EMPTY_STR) // If we are not on a new line (If this is not just an extra new line).
			{
				output.append(curLine);  // Add curLine to output plus '\n'.
				output.append(1, curChar);
				curLine = this->EMPTY_STR;
			}
			i++;
		}
		else
		{
			if (curChar != ' ') // Skip white space.
				curLine.append(1, curChar);
			i++;
		}
        curChar = input->at(i);
	}
	output.append(1, '\0');
	
	return output;
}

/**
 * Removes symbols and replaces them with their proper numbers.
 *
 * @param input The pointer to the string you wish to resolve.
 * @return The resolved version of input.
 */
string Resolver::resolveSymbols(string* input)
{
	string output = "";
	char curChar = input->at(0);
	string curLine = this->EMPTY_STR;
    vector<string> temp;
    int i = 0;
	
	while (curChar != '\0')// Iterate through input until it ends.
	{
		if (curChar == '@') // Check for symbols(only used in A commands), add them.
		{
			temp = Assembler::getLine(input, i+1);
			curLine.append(1, '@');
			curLine.append(resolveVar(temp.at(0), false));
			i += atoi(temp.at(1).c_str()); // Adjust the i to properly skip the aforementioned line.
		}
		else if (curChar == '\n') // If it's a new line char:
		{
			if (curLine != this->EMPTY_STR) // If we have been loading an asm line:
			{
				output.append(curLine);  // Add curLine to output plus '\n' if it is not the end of the file.
				output.append(1, curChar);
				curLine = this->EMPTY_STR;
				lineCounter++; // We have reached a new line in the resolved asm file.
			}
			i++;
		}
		else
		{
			curLine.append(1, curChar); // If it is none of the above, add to curLine.
			i++;
		}
		curChar = input->at(i);
	}
	output.append(1, '\0');
	
	return output;
}


// Interpreter: 
/**
 * Interpretation logic is done here.
 * Requires input to be resolved by Resolver.
 * 
 * @param input The pointer to the string you wish to interpret. It MUST have been resolved with Resolver.
 */
Interpreter::Interpreter(string* input)
{
	// Initialize Code Tables:
	initializeCode();
	
	
    output = "";
	char curChar = input->at(0);
	string curLine;
	string curCommand = "";
    vector<string> temp;
	string tempS;
    int i = 0;
	
	// C command vars:
	string des = "";
	string JMP = "";
	string comp = "";
	
	while (true)// Simply loop; There is a break once a null char is reached.
	{
		if (input->at(i) == '\0')
			return;
		temp = Assembler::getLine(input, i); // Get first command.
		curLine = temp.at(0);
		i += atoi(temp.at(1).c_str()); // Adjust the i to properly skip the aforementioned line.
		
		// Interpretation logic:
		if (curLine.at(0) == '@')
		{
			// A command logic:
			curCommand.append(1, '0'); // MostSignificatnBit is OP code 0, denoting A command.
			curLine = curLine.substr(1, curLine.size());
			tempS = std::bitset<15>(atoi(curLine.c_str())).to_string(); // Convert remaining line to binary, as it is an address number.
			curCommand += tempS;
		}
		else
		{
			// C command logic:
			curCommand.append("111");
			if (curLine.find(';') != string::npos) // If there is a jump:
			{
				des = curLine.substr(0, curLine.find_first_of(';')); // Des is from start to ;.
				JMP = curLine.substr(curLine.find_first_of(';')+1, curLine.size()); // JMP is from ; to end.
			}
			else 
			{
				JMP = "";
				des = curLine;
			}
			if (curLine.find('=') != string::npos) // If there is a computation:
			{
				comp = des.substr(curLine.find('=')+1, curLine.size());
				des = des.substr(0, curLine.find('='));
			}
			else
			{
				comp = des;
				des = "";
			}
			curCommand += getCompCode(comp);
			curCommand += getDesCode(des);
			curCommand += getJMPCode(JMP);
		}
		output.append(curCommand);
		if (input->at(i) != '\0')
			output.append(1, '\n'); 
		
		curCommand = "";
	}
	
	return;
}

/**
 * Gets the corresponding hack code for input, which should be an asm des command.
 *
 * @param input The pointer to the string of the des command.
 * @return The string for the corresponding hack code.
 */
 string Interpreter::getDesCode(string input)
 {
	for (int i = 0; i < this->desCode.size(); i++) // For every entry in desCode:
	{
		if (desCode.at(i).at(0) == input) // If the first part of the entry matches the input:
			return desCode.at(i).at(1); // Return the second part, which is the corresponding hack code.
	}
	return "DesERROR";
 }
 
 /**
 * Gets the corresponding hack code for input, which should be an asm comp command.
 *
 * @param input The pointer to the string of the comp command.
 * @return The string for the corresponding hack code.
 */
 string Interpreter::getCompCode(string input)
 {
	for (int i = 0; i < this->compCode.size(); i++) // For every entry in compCode:
	{
		if (compCode.at(i).at(0) == input) // If the first part of the entry matches the input:
			return compCode.at(i).at(1); // Return the second part, which is the corresponding hack code.
	}
	return "CompERROR";
 }
 
 /**
 * Gets the corresponding hack code for input, which should be an asm JMP command.
 *
 * @param input The pointer to the string of the JMP command.
 * @return The string for the corresponding hack code.
 */
 string Interpreter::getJMPCode(string input)
 {
	for (int i = 0; i < this->JMPCode.size(); i++) // For every entry in JMPCode:
	{
		if (JMPCode.at(i).at(0) == input) // If the first part of the entry matches the input:
			return JMPCode.at(i).at(1); // Return the second part, which is the corresponding hack code.
	}
	return "JMPERROR";
 }

/**
 * Initializes the code vectors. 
 * Each entry is also a vector. At(0), the asm code. At(1), the corresponding hack code.
 *
 * @return void.
 */
 void Interpreter::initializeCode()
 {
	 vector<string> temp;
	// Initialize comp Code:
	temp.push_back("0");
	temp.push_back("0101010");
	compCode.push_back(temp);
	temp.clear();
	
	temp.push_back("1");
	temp.push_back("0111111");
	compCode.push_back(temp);
	temp.clear();
	
	temp.push_back("-1");
	temp.push_back("0111010");
	compCode.push_back(temp);
	temp.clear();
	
	temp.push_back("D");
	temp.push_back("0001100");
	compCode.push_back(temp);
	temp.clear();
	
	temp.push_back("A");
	temp.push_back("0110000");
	compCode.push_back(temp);
	temp.clear();
	
	temp.push_back("!D");
	temp.push_back("0001101");
	compCode.push_back(temp);
	temp.clear();
	
	temp.push_back("!A");
	temp.push_back("0110011");
	compCode.push_back(temp);
	temp.clear();
	
	temp.push_back("-D");
	temp.push_back("0001111");
	compCode.push_back(temp);
	temp.clear();
	
	temp.push_back("-A");
	temp.push_back("0110011");
	compCode.push_back(temp);
	temp.clear();
	
	temp.push_back("D+1");
	temp.push_back("0011111");
	compCode.push_back(temp);
	temp.clear();
	
	temp.push_back("A+1");
	temp.push_back("0110111");
	compCode.push_back(temp);
	temp.clear();
	
	temp.push_back("D-1");
	temp.push_back("0001110");
	compCode.push_back(temp);
	temp.clear();
	
	temp.push_back("A-1");
	temp.push_back("0110010");
	compCode.push_back(temp);
	temp.clear();
	
	temp.push_back("D+A");
	temp.push_back("0000010");
	compCode.push_back(temp);
	temp.clear();
	
	temp.push_back("D-A");
	temp.push_back("0010011");
	compCode.push_back(temp);
	temp.clear();
	
	temp.push_back("A-D");
	temp.push_back("0000111");
	compCode.push_back(temp);
	temp.clear();
	
	temp.push_back("D&A");
	temp.push_back("0000000");
	compCode.push_back(temp);
	temp.clear();
	
	temp.push_back("D|A");
	temp.push_back("0010101");
	compCode.push_back(temp);
	temp.clear();
	
	temp.push_back("M");
	temp.push_back("1110000");
	compCode.push_back(temp);
	temp.clear();
	
	temp.push_back("!M");
	temp.push_back("1110001");
	compCode.push_back(temp);
	temp.clear();
	
	temp.push_back("-M");
	temp.push_back("1110011");
	compCode.push_back(temp);
	temp.clear();
	
	temp.push_back("M+1");
	temp.push_back("1110111");
	compCode.push_back(temp);
	temp.clear();
	
	temp.push_back("M-1");
	temp.push_back("1110010");
	compCode.push_back(temp);
	temp.clear();
	
	temp.push_back("D+M");
	temp.push_back("1000010");
	compCode.push_back(temp);
	temp.clear();
	
	temp.push_back("D-M");
	temp.push_back("1010011");
	compCode.push_back(temp);
	temp.clear();
	
	temp.push_back("M-D");
	temp.push_back("1000111");
	compCode.push_back(temp);
	temp.clear();
	
	temp.push_back("D&M");
	temp.push_back("1000000");
	compCode.push_back(temp);
	temp.clear();
	
	temp.push_back("D|M");
	temp.push_back("1010101");
	compCode.push_back(temp);
	temp.clear();
	
	// Initialize dest code:
	temp.push_back("");
	temp.push_back("000");
	desCode.push_back(temp);
	temp.clear();
	
	temp.push_back("M");
	temp.push_back("001");
	desCode.push_back(temp);
	temp.clear();
	
	temp.push_back("D");
	temp.push_back("010");
	desCode.push_back(temp);
	temp.clear();
	
	temp.push_back("MD");
	temp.push_back("011");
	desCode.push_back(temp);
	temp.clear();
	
	temp.push_back("A");
	temp.push_back("100");
	desCode.push_back(temp);
	temp.clear();
	
	temp.push_back("AM");
	temp.push_back("101");
	desCode.push_back(temp);
	temp.clear();
	
	temp.push_back("AD");
	temp.push_back("110");
	desCode.push_back(temp);
	temp.clear();
	
	temp.push_back("AMD");
	temp.push_back("111");
	desCode.push_back(temp);
	temp.clear();
	
	// Initialize JMP code:
	
	temp.push_back("");
	temp.push_back("000");
	JMPCode.push_back(temp);
	temp.clear();
	
	temp.push_back("JGT");
	temp.push_back("001");
	JMPCode.push_back(temp);
	temp.clear();
	
	temp.push_back("JEQ");
	temp.push_back("010");
	JMPCode.push_back(temp);
	temp.clear();
	
	temp.push_back("JGE");
	temp.push_back("011");
	JMPCode.push_back(temp);
	temp.clear();
	
	temp.push_back("JLT");
	temp.push_back("100");
	JMPCode.push_back(temp);
	temp.clear();
	
	temp.push_back("JNE");
	temp.push_back("101");
	JMPCode.push_back(temp);
	temp.clear();
	
	temp.push_back("JLE");
	temp.push_back("110");
	JMPCode.push_back(temp);
	temp.clear();
	
	temp.push_back("JMP");
	temp.push_back("111");
	JMPCode.push_back(temp);
	temp.clear();
	
	return;
 }


/**
 * Gets the output of the interpretation.
 *
 * @return The interpreted output as a NULL terminated string.
 */
 string Interpreter::getOutput()
 {
	 return output;
 }

// Assembler:
Assembler::Assembler(){}

/**
 * Assembles the input at path. Once done, outputs the result to a .hack file in the same dir as the input path.
 */
 int Assembler::assemble(char* path)
 {
	if (loadInput(path) == 1)
	{
		return 1;
	}
	// Logic:
	Resolver* resolvedASM = new Resolver(&input); // Resolve white space, comments and symbols.
	string output = resolvedASM->getOutput();
	
	Interpreter* interpreter = new Interpreter(&output);
	output = interpreter->getOutput();
	
	//Output:
	string outputPath = string(path); // Get input path.
	outputPath = outputPath.substr(0, outputPath.find_last_of(".")) + ".hack"; // Change file extension to hack.
	ofstream outputFile;
	outputFile.open(outputPath, ios::out);
	outputFile << output;
	outputFile.close();
	return 0;
 }
 
/**
 * Load file at input into this->input.
 *
 * @param input The path as a char*.
 */
 int Assembler::loadInput(char* path)
 {
	ifstream asmFile;
	asmFile.open(path, ios::in);
	if (!asmFile.is_open()) // If path does not open properly.
	{
		cout << "Path invalid; Usage: hackAssembler (path to .asm file)\n";
		return 1;
	}
	
	string temp; // Get the contents of the asm file into asmSource.
	this->input = "";
	while (std::getline(asmFile, temp))
	{
		this->input.append(temp);
		this->input.append("\n");
	}
	asmFile.close();
	this->input.append(1, '\0'); // Add NULL terminator char.
	return 0;
 }
 
 /**
 * Takes input, starts at start, and returns a string with the value to offset the received string and \n char.
 * 
 * @param input the string* of which you want the line.
 * @param start
 * @return The line as output.at(0), the value needed to offset the received string + \n at output.at(1) as a string.
 */
vector<string> Assembler::getLine(string* input, int start)
{
    char curChar = input->at(start);
    vector<string> output = {"", ""};
    int i = start;
    while (curChar != '\n')
    {
		output.at(0).append(1, curChar);
		i++;
        curChar = input->at(i);
    }
	i++;
    output.at(1) = std::to_string(i - start);
    
    return output;
}

/**
 * Takes input, starts at start, and returns a string with the value to offset the received string and endChar.
 * 
 * @param input the string* of which you want the line.
 * @param start the position of the char you wish to start with.
 * @param endChar the char which you wish to end with. The char is not included in output.at(0).
 * @return The line as output.at(0), the value needed to offset the received string and endChar at output.at(1) as a string.
 */
vector<string> Assembler::getLine(string* input, int start, char endChar)
{
    char curChar = input->at(start);
    vector<string> output = {"", ""};
    int i = start;
    while (curChar != endChar)
    {
		output.at(0).append(1, curChar);
		i++;
        curChar = input->at(i);
    }
	i++;
    output.at(1) = std::to_string(i - start);
    
    return output;
}






