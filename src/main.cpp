/************************************************************************-
 *	HACKAssembler, the assembler that converts .asm files to .hack files by translating 
 *  hack asm language to binary strings. 
 *  Hack is a machine language code for the simulated HACK computer, from the class "From Nand2Tetris".
 *  
 *	HACK Code specification:
 *
 *		Two types of commands, A and C.
 *		A starts with @ in asm; 0 in hack. Used for addressing registers:
 *		"@R5"
 *		"0000000000000001"
 *		The rest of the bits are a 15 bit address.
 *
 *		C starts with anything else in asm; 1 in hack. used for computations and jumps:
 *		"D=M"
 *		"1110000010010000"
 *		The 2nd two bits are not used and are set to 1.
 *		The next 7 are computation bits.
 *		The next 3 are destination bits - what register(s) are going to receive input.
 *		The last 3 are jump bits, determining if the program should jump to the current address value set by a, 
 *      Depending on the previous calculation in the command.
 *			The comparisons are relative to zero (JLT = Jump less than zero, etc). JMP is unconditional. 
 *
 *	Started: December 15, 2017
 *  Finished: December 22, 2017
 *  Updates:
 *		- 
 *	©2018 C. A. Acred all rights reserved.
 ----------------------------------------------------------*
*/

// Compile: g++ main.cpp hackASM/hackASM.cpp -o hackAssembler -std=c++11 -static-libgcc -static-libstdc++
// Debug:   g++ -g main.cpp hackASM/hackASM.cpp -o hackAssembler -std=c++11 -static-libgcc -static-libstdc++

#include "hackASM/hackASM.h"
#include <iostream>

main(int argc, char** argv)
{
	if (argc != 2) // Make sure you got a path, and only one path.
	{
		cout << "Invalid usage; Usage: hackAssembler (path to .asm file)\n";
		return 1;
	}
	
	Assembler* assembler = new Assembler();
	int error = assembler->assemble(argv[1]);
	if (error == 1)
		return 1;
    return 0;
}