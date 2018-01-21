/************************************************************************-
 *	hackAsm.h, contains the classes to build the HACK Assembler.
 *
 *	Started: December 15
 *  Finished: December 22
 *  Updates:
 *		- 
 *	©2018 C. A. Acred all rights reserved.
 ----------------------------------------------------------*
*/

#ifndef HACKASM_H
#define HACKASM_H

#include <cstdlib>
#include <string>
#include <vector>

using namespace std;

class Resolver;
class Interpreter;
class Assembler;

/**
 * Resolves Labels and variables in the asm code. 
 * Removes whitespace and comments.
 * Saves resulting string in this->output.
 */
class Resolver
{
private:
    const string EMPTY_STR = "";
    const int VAR_ASSIGN_ADD_START = 16; // Starting register number for vars.
    
    int commandCount;
    int varCounter;
	int lineCounter;
    
    vector<string> varNames; // Names of variables 
    vector<string> varRegNums; // Corresponding register numbers.
    string output;
    
public:
    Resolver(string* input);
    ~Resolver();
	
	bool isNumber(string* input);
    
    string addVar(string name, bool isLabel);
	string addVar(string name, int reg);
	string resolveVar(string name, bool isLabel);
	string findVar(string name);
    
    string getOutput();
	
	void initializeVars();
	
	string resolveLabels(string* input);
	string resolveExcess(string* input);
	string resolveSymbols(string* input);
};


/**
 * Interprets asm code to .hack machine code.
 */
class Interpreter
{
private:
    vector<vector<string>> desCode; 
    vector<vector<string>> JMPCode;
    vector<vector<string>> compCode;
	
	string output;
	
	string getDesCode(string input);
	string getCompCode(string input);
	string getJMPCode(string input);
	
	void initializeCode();
    
public:
    Interpreter(string* input);
    ~Interpreter();
	
	string getOutput();
};

/**
 * Resolves and interprets asm code into hack code. 
 * Uses a Resolver for cleaning up the code of comment and whitespace and for resolving symbolic variables. 
 * Uses a Interpreter to change the asm code to hack machine code.
 * Saves this resulting hack code in a file of the same name in the same directory as the input path.
 */
 class Assembler 
 {
private: 
	string input; 
	
	int loadInput(char* input);
	
public:
	Assembler();
	~Assembler();
	
	int assemble(char* input);
	 
	static vector<string> getLine(string* input, int start);
    static vector<string> getLine(string* input, int start, char endChar);
 };

#endif