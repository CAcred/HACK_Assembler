g++ -g main.cpp hackASM/hackASM.cpp -o hackAssembler -std=c++11 -static-libgcc -static-libstdc++
gdb --args hackAssembler.exe C:\Users\Night_Blader\Desktop\nand2tetris\nand2tetris\projects\06\pong\Pong.asm
