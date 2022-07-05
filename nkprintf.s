; MakeFunction sets up everything you need to make an assembly function
; callable from C and debuggable with a symbolic debugger. It does the following:
; - export the function's transition vector
; - export the function name
; - create a toc entry for the function's transition vector
; - create the transition vector, which must contain
;     - the function entry point (the name of the function)
;     - the TOC anchor (the predefined variable TOC[tc0])
; - tell PPCAsm to create a function entry point symbol for symbolic debuggers
; - create a csect for the function (one csect per function lets the
;   linker do dead code stripping, resulting in smaller executables)

	MACRO
	MakeFunction &fnName
		EXPORT &fnName[DS]
		EXPORT .&fnName[PR]

		TC &fnName[TC], &fnName[DS]

		CSECT &fnName[DS]
			DC.L .&fnName[PR]
			DC.L TOC[tc0]

		CSECT .&fnName[PR]
		FUNCTION .&fnName[PR]

	ENDM


; void nkprint(const char *);
	MakeFunction nkprint
	li      r0,96
	sc
	blr


; void nkprintf(const char *fmt, ...);
	MakeFunction nkprintf
	import .vsprintf

	; standard prolog
	mflr    r0
	stw     r0,8(sp)
	stwu    sp,-0x300(sp)

	stmw    r4,0x280(sp)        ; store varargs
	mr      r4,r3               ; fmt ptr
	addi    r3,sp,0x80          ; buffer ptr, 512 bytes
	addi    r5,sp,0x280         ; vararg ptr
	bl      .vsprintf
	nop

	addi    r3,sp,0x80          ; NanoKernel call
	li      r0,96
	sc

	addi    sp,sp,0x300
	lwz     r0,8(sp)
	mtlr    r0
	blr
