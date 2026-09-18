// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel;
// the screen should remain fully black as long as the key is pressed. 
// When no key is pressed, the program clears the screen, i.e. writes
// "white" in every pixel;
// the screen should remain fully clear as long as no key is pressed.

// Put your code here.
    // SCREENLAST = KBD - SCREEN
    @KBD
    D=A
    @SCREEN
    D=D-A
    @SCREENLAST
    M=D
(BLACK)
    // if (RAM[KBD] == 0) goto WHITE
    @KBD
    D=M
    @WHITE
    D;JEQ
    // n = 0
    @n
    M=0
(LOOPBLACK)
    // if (n == SCREENLAST) goto ENDBLACK
    @n
    D=M
    @SCREENLAST
    D=D-M
    @ENDBLACK
    D;JEQ
    // *(SCREEN + n) = 1111 1111 1111 1111
    @SCREEN
    D=A
    @n
    A=D+M
    M=-1
    // n = n + 1
    @n
    D=M
    D=D+1
    @n
    M=D
    // goto LOOPBLACK
    @LOOPBLACK
    0;JMP
(ENDBLACK)
    // goto BLACK
    @BLACK
    0;JMP
(WHITE)
    // if (RAM[KBD] != 0) goto BLACK
    @KBD
    D=M
    @BLACK
    D;JNE
    // n = 0
    @n
    M=0
(LOOPWHITE)
    // if (n == SCREENLAST) goto ENDWHITE
    @n
    D=M
    @SCREENLAST
    D=D-M
    @ENDWHITE
    D;JEQ
    // *(SCREEN + n) = 0000 0000 0000 0000
    @SCREEN
    D=A
    @n
    A=D+M
    M=0
    // n = n + 1
    @n
    D=M
    D=D+1
    @n
    M=D
    // goto LOOPWHITE
    @LOOPWHITE
    0;JMP
(ENDWHITE)
    // goto WHITE
    @WHITE
    0;JMP