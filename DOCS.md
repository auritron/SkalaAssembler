# Instruction Types

## R-instruction
- Register only  
- OPCODE REG REG (REG)  
- SET, NOT, AND, OR, XOR, STL, STR, RTL, RTR, NEG, ADD, SUB, MUL, DIV, MOD, CMP  

## I-instruction
- Register + Immediate  
- OPCODE REG (REG) IMM  
- SET, NOT, AND, OR, XOR, STL, STR, RTL, RTR, NEG, ADD, SUB, MUL, DIV, MOD, CMP

## M-instruction
- Memory Management  
- OPCODE REG ADR
- LOAD, SEND  

## J-instruction
- Jumping and Branching  
- OPCODE @LABEL  
- WEQ, WNE, WGT, WLT, WCY, WOV, WDZ, GOTO, CAL  

## S-instruction
- Single Register Operation  
- OPCODE REG  
- PUSH, POP, PRINT(VM only), PRINTC(VM only)  

## N-instruction
- No Operands  
- OPCODE  
- RET, END, CLR  

# Instruction Binary Format

Instruction Length - 32 bits (4 bytes); Word Length - 16 bits (2 bytes)
Opcode - 3 bits for type + 5 bits for instruction (8 bits total)
- R-type: (opcode 8) (dest 4 | src1 4)   (src2 4 | unused 4) (unused 8)  
- I-type: (opcode 8) (dest 4 | src  4)   (imm high 8)        (imm low 8)  
- M-type: (opcode 8) (reg  4 | unused 4) (addr high 8)       (addr low 8)  
- J-type: (opcode 8) (unused 8)          (addr high 8)       (addr low 8)
- S-type: (opcode 8) (reg  4 | unused 4) (unused 8)          (unused 8)  
- N-type: (opcode 8) (unused 8)          (unused 8)          (unused 8)  

# Instruction Opcodes

## memory and registers
LOAD Rdst, ADRsrc  
SEND Rsrc, ADRdst  
SET  Rdst, Rsrc  

## bitwise
NOT  Rdst, Rax  
AND  Rdst, Rax, Rbx  
OR   Rdst, Rax, Rbx  
XOR  Rdst, Rax, Rbx  

## shift and rotation
STL  Rdst, Rax  
STR  Rdst, Rax  
RTL  Rdst, Rax  
RTR  Rdst, Rax  

## arithmetic
NEG  Rdst, Rax  
ADD  Rdst, Rax, Rbx  
SUB  Rdst, Rax, Rbx  
MUL  Rdst, Rax, Rbx  
DIV  Rdst, Rax, Rbx  
MOD  Rdst, Rax, Rbx  

## comparison and branching
CMP  Rax, Rbx  
GOTO @LBL  
WEQ  @LBL  
WNE  @LBL
WGT  @LBL  
WLT  @LBL  
WCY  @LBL  
WOV  @LBL  
WDZ  @LBL  

## stacks and sub-routines
CAL  @LBL  
RET  
PUSH Rsrc
POP  Rdst

## i/o
PRINT  Rsrc
PRINTC Rsrc

## misc. and debugging
END  
CLR  

# OpCode Binary Codes

## Instruction Types - (first 3 bits)
- R - 000  
- I - 001  
- M - 010  
- J - 011  
- S - 100  
- N - 101  

## OpCodes - (3 bits from instruction type + 5 bits for instruction)

### R-types -
- SET - 000 10000  
- NOT - 000 00000  
- AND - 000 00001  
- OR  - 000 00010  
- XOR - 000 00011  
- STL - 000 00100  
- STR - 000 00101  
- RTL - 000 00110  
- RTR - 000 00111  
- ADD - 000 01000  
- SUB - 000 01001  
- NEG - 000 01011  
- MUL - 000 01100  
- DIV - 000 01110  
- MOD - 000 01111  
- CMP - 000 10001  

### I-types -
- SET - 001 10000  
- NOT - 001 00000  
- AND - 001 00001  
- OR  - 001 00010  
- XOR - 001 00011  
- STL - 001 00100  
- STR - 001 00101  
- RTL - 001 00110  
- RTR - 001 00111  
- ADD - 001 01000  
- SUB - 001 01001  
- NEG - 001 01011  
- MUL - 001 01100  
- DIV - 001 01110  
- MOD - 001 01111  
- CMP - 001 10001  

### M-types -
- LOAD - 010 00000  
- SEND - 010 00001  

### S-types -
- PUSH - 011 00000  
- POP  - 011 00001  
- PRINT - 011 10000  
- PRINTC - 011 10001  

## J-instruction
- WEQ  - 100 00000  
- WNE  - 100 00001  
- WGT  - 100 00010  
- WLT  - 100 00011  
- WCY  - 100 00100  
- WOV  - 100 00101  
- WDZ  - 100 00110  
- GOTO - 100 01000  
- CAL  - 100 01001  

## N-instruction
- RET - 101 00000  
- END - 101 11111  
- CLR - 101 10000  