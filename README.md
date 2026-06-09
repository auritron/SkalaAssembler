# SkalaAssembler
*Simple*, *modular* and *minimalist* ISA + Assembler :P  
Carved out of a custom ISA, programmed in C++, and built with blood, sweat, and tears... ;-;

## Specifications
- **C++ version** - C++23  
- **Word size** - 16 bit  
- **Instruction size** - 32 bit  
- **Number of GPR Registers** - 16  
- **Accessible address range** - 0x0000 to 0xFFFF  
- **Immediate value range** - 0 to 65535  
- **Address offset** - 0x0100 *(256B preallocated memory)*  
- **Magic number** - 0x3cDECODE  

## How to use
1. **Clone the repository** as follows -  
```bash
cd path/to/your/folder
git clone https://github.com/auritron/SkalaAssembler
```
2. Create an assembly program with the respective syntax  
3. Save the file name as **demo.skasm** in **root/progs** (or save it with a custom file name, and change prog_name in **assembler.hpp** to the file name you want the program to assemble)
4. Build the assembler from the **root folder** (Given below is for GCC, **C++23 required**) -  
```bash
cd path/to/ROOT
g++ -g -std=c++23 src/*.cpp src/assembler/*.cpp -I C:/src/vcpkg/vcpkg/installed/x64-windows/include -o main.exe
```
5. Assembled binary will be present in **root/progs**. If the program contains syntactical or semantic errors, the assembler will give a detailed line-by-line list of all related errors present in the program, and the binary will not be generated.  

**NOTE: If the program finishes and says there are zero errors but a binary is not generated, or the program panics (*indicated by the message mentioning a unknown error*), please raise an detailed issue on this repository.**  

## Syntax
- **Instruction Format** - `OPCODE [OPERAND1] [OPERAND2] [OPERAND3]`  
- **Label Declaration** - `@LABEL`  
- **OpCode** - `OPCODE` *(in full UPPERCASE, must match a valid opcode as per documentation)*  
- **Register** - `RX` *(0 <= X < 16)*  
- **Immediate** - `#VAL` *(0 <= VAL <= 65535)*  
- **Address** - `0x[VALUE in hex]` *(0x0000 <= VALUE <= 0xFFFF)*  
- **Variable** - `my_variable` *(Any alpha-numeric string beginning with a letter, not covered by the previous rules)*  
- **Comments** - `;Begin your comment with a single semicolon, ends at newline` *(only single line comments supported)*  

## Instructions

1. LOAD - `LOAD Rdst ADRsrc`  
- Loads data from memory address ADRsrc to Register Rdst  

2. SEND - `SEND Rsrc ADRdst`  
- Sends data from Register Rsrc to memory address ADRdst  

3. SET - `SET Rdst [Rsrc | IMM]`  
- Copies the value of Rsrc/IMM into Rdst  

4. NOT - `NOT Rdst [Rax | IMM]`  
- Performs a NOT operation on Rax/IMM and stores the result in Rdst  

5. AND - `AND Rdst Rax [Rbx | IMM]`  
- Performs an AND operation between Rax and Rbx/IMM and stores the result in Rdst  

6. OR - `OR Rdst Rax [Rbx | IMM]`  
- Performs an OR operation between Rax and Rbx/IMM Rax and stores the result in Rdst  

7. XOR - `XOR Rdst Rax [Rbx | IMM]`  
- Performs a XOR operation between Rax and Rbx/IMM Rax and stores the result in Rdst  

8. STL - `STL Rdst Rax [Rbx | IMM]`  
- Shifts left Rax value by Rbx/IMM bits and stores the result in Rdst  

9. STR - `STR Rdst Rax [Rbx | IMM]`  
- Shifts left Rax value by Rbx/IMM bits and stores the result in Rdst  

10. RTL - `RTL Rdst Rax [Rbx | IMM]`  
- Shifts left Rax value by Rbx/IMM bits and stores the result in Rdst  

11. RTR - `RTR Rdst Rax [Rbx | IMM]`  
- Shifts left Rax value by Rbx/IMM bits and stores the result in Rdst 

12. NEG - `NEG Rdst [Rax | IMM]`  
- Negates the value of Rax/IMM and stores the result in Rdst  

13. ADD - `ADD Rdst Rax [Rbx | IMM]` 
- Adds the values of Rax and Rbx/IMM and stores the value in Rdst, while setting the corresponding flag  

14. SUB - `SUB Rdst Rax [Rbx | IMM]`  
- Subtracts the values of Rax and Rbx/IMM and stores the value in Rdst, while setting the corresponding flag  

15. MUL - `MUL Rdst Rax [Rbx | IMM]`  
- Multiplies the values of Rax and Rbx/IMM and stores the value in Rdst, while setting the corresponding flag  

16. DIV - `DIV Rdst Rax [Rbx | IMM]`  
- Divides the values of Rax and Rbx/IMM and stores the value in Rdst, while setting the corresponding flag  

17. MOD - `MOD Rdst Rax [Rbx | IMM]`   
- Finds the remainder of the division between the values of Rax and Rbx/IMM and stores the value in Rdst, while setting the corresponding flag  

18. CMP - `CMP Rax [Rbx | IMM]`  
- Compares the values of Rax and Rbx/IMM and sets the corresponding flag  

19. GOTO - `GOTO @LBL`  
- Unconditional jump to label @LBL  

20. WEQ - `WEQ @LBL`  
- Conditionally jumps to label @LBL when the Z flag is set to 1  

21. WNE - `WNE @LBL`
- Conditionally jumps to label @LBL when the Z flag is set to 0  

22. WGT - `WGT @LBL`  
- Conditionally jumps to label @LBL when the GT flag is set to 1  

23. WLT - `WLT @LBL`  
- Conditionally jumps to label @LBL when the LT flag is set to 1  

24. WCY - `WCY @LBL`  
- Conditionally jumps to label @LBL when the C flag is set to 1  

25. WOV - `WOV @LBL`  
- Conditionally jumps to label @LBL when the O flag is set to 1  

26. WDZ - `WDZ @LBL`  
- Conditionally jumps to label @LBL when the DZ flag is set to 1  

27. CAL - `CAL @LBL`  
- Unconditionally jumps to @LBL and pushes the return address of the label @LBL to the stack  

28. RET - `RET`  
- Returns control flow to the address saved to the stack and pops the value  

29. PUSH - `PUSH Rsrc`
- Pushes the value of a register Rsrc to the stack  

30. POP - `POP Rdst`
- Pops the value at the top of the stack and stores it in Rdst  

31. PRINT - `PRINT Rsrc`  
- Prints the value of Rsrc to the console *(to be used for debugging purpose)*  

32. PRINTC - `PRINTC Rsrc`  
- Prints the value of Rsrc to the console as an ASCII character *(to be used for debugging purpose)*  

33. END - `END`  
- Stops execution of the program  

34. CLR - `CLR`
- Clears all the registers *(to be used for debugging purpose)*  

## Example program -
```ini
GOTO @OPERATIONS
@BACK
GOTO @END

@OPERATIONS
	LOAD R5 x
	LOAD R6 y
	ADD R18 R5 R6
	SEND R5 #5
	SUB R1 R5 R6
	SEND R1 y
	GOTO @BACK

@END
```