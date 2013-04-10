/*
 * TempVm.c
 *
 *  Created on: 2013/04/08
 *      Author: skgchxngsxyz-work
 */

#include <stdio.h>

/*
 * funcPointer .......stacktop
 * pc
 * argument number
 * arguments
 *
 *
 */
typedef enum {
	RET,
	POP,
	PUSH,
	ADD,
	SUB,
	MUL,
	DIV,
	PRINT,
	EQ,
	NEQ,
	LT,
	LTEQ,
	GT,
	GTEQ,
	IF_JUMP, // IF_JUMP label
	CALL,	// CALL argNum funcIndex
	LOAD_ARG, // LOAD_ARG n
} Code;

typedef struct {
	int stackTop;
	int stack[1024];
	int pc;
	int funcPointer;
} VM;

typedef union {
	int code[2];
	Code *address;
} Pointer;

#define EXEC_BINARY_OP(vm, OP) do{\
	int arg2 = vm->stack[vm->stackTop];\
	int arg1 = vm->stack[--vm->stackTop];\
	vm->stack[vm->stackTop] = arg1 OP arg2;\
}while(0)

void execute(VM *vm, Code *codeList)
{
	while(1){
		switch (codeList[vm->pc]) {
		case RET:
			if (vm->funcPointer == 0) {
				return;
			} else {
				int ret = vm->stack[vm->stackTop];
				vm->pc = vm->stack[vm->funcPointer - 1];
				int argNum = vm->stack[vm->funcPointer - 2];
				vm->stackTop = vm->funcPointer - 2 - argNum;
				vm->stack[vm->stackTop] = ret;
				vm->funcPointer = vm->stack[vm->funcPointer];
				return;
			}
		case POP:
			vm->stackTop--;
			break;
		case PUSH:
			vm->stack[++vm->stackTop] = codeList[++vm->pc];
			break;
		case ADD:
			EXEC_BINARY_OP(vm, +);
			break;
		case SUB:
			EXEC_BINARY_OP(vm, -);
			break;
		case MUL:
			EXEC_BINARY_OP(vm, *);
			break;
		case DIV:{
			int arg2 = vm->stack[vm->stackTop];
			int arg1 = vm->stack[--vm->stackTop];
			vm->stack[vm->stackTop] = (int)(arg1 / arg2);
			break;}
		case PRINT:
			printf("[%2d] %d\n", vm->stackTop, vm->stack[vm->stackTop]);
			break;
		case EQ:
			EXEC_BINARY_OP(vm, ==);
			break;
		case NEQ:
			EXEC_BINARY_OP(vm, !=);
			break;
		case LT:
			EXEC_BINARY_OP(vm, <);
			break;
		case LTEQ:
			EXEC_BINARY_OP(vm, <=);
			break;
		case GT:
			EXEC_BINARY_OP(vm, >);
			break;
		case GTEQ:
			EXEC_BINARY_OP(vm, >=);
			break;
		case IF_JUMP:{
			int arg = vm->stack[vm->stackTop--];
			int label = codeList[++vm->pc];
			if (arg) {
				vm->pc = label - 1;
			}
			break;}
		case CALL:{
			vm->stack[++vm->stackTop] = codeList[++vm->pc];
			int label1 = codeList[++vm->pc];
			int label2 = codeList[++vm->pc];
			Pointer pointer = {{label1, label2}};
			vm->stack[++vm->stackTop] = vm->pc;
			vm->stack[++vm->stackTop] = vm->funcPointer;
			vm->pc = 0;
			vm->funcPointer = vm->stackTop;
			execute(vm, pointer.address);
			break;}
		case LOAD_ARG:{
			int num = codeList[++vm->pc];
			int arg = vm->stack[vm->funcPointer - 2 - num];
			vm->stack[++vm->stackTop] = arg;
			break;}
		}
		vm->pc++;
	}
}

int main (void)
{
	VM vm = {0};
	/*
	 * PUSH,
	 * 1,
	 * PUSH,
	 * 2,
	 * CALL,
	 * 2, // number of args.
	 * &ADD, // index of entry point.
	 */

	Code factCode[30];
	Pointer factPointer;
	factPointer.address = factCode;
	factCode[0] = LOAD_ARG;
	factCode[1] = 1;
	factCode[2] = PUSH;
	factCode[3] = 2;
	factCode[4] = GTEQ;
	factCode[5] = IF_JUMP;
	factCode[6] = 11;
	factCode[7] = PUSH;
	factCode[8] = 1;
	factCode[9] = PRINT;
	factCode[10] = RET;
	factCode[11] = LOAD_ARG;
	factCode[12] = 1;
	factCode[13] = PUSH;
	factCode[14] = 1;
	factCode[15] = SUB;
	factCode[16] = CALL;
	factCode[17] = 1;
	factCode[18] = factPointer.code[0];
	factCode[19] = factPointer.code[1];
	factCode[20] = LOAD_ARG;
	factCode[21] = 1;
	factCode[22] = MUL;
	factCode[23] = PRINT;
	factCode[24] = RET;

	Code fact_mainCode[10];
	fact_mainCode[0] = PUSH;
	fact_mainCode[1] = 10; //arg
	fact_mainCode[2] = CALL;
	fact_mainCode[3] = 1;
	fact_mainCode[4] = factPointer.code[0];
	fact_mainCode[5] = factPointer.code[1];
	fact_mainCode[6] = PRINT;
	fact_mainCode[7] = RET;

	Code fibCode[40];
	Pointer fibPointer;
	fibPointer.address = fibCode;
	fibCode[0] = LOAD_ARG;
	fibCode[1] = 1;
	fibCode[2] = PUSH;
	fibCode[3] = 3;
	fibCode[4] = GTEQ;
	fibCode[5] = IF_JUMP;
	fibCode[6] = 10;
	fibCode[7] = PUSH;
	fibCode[8] = 1;
	fibCode[9] = RET;
	fibCode[10] = LOAD_ARG;
	fibCode[11] = 1;
	fibCode[12] = PUSH;
	fibCode[13] = 1;
	fibCode[14] = SUB;
	fibCode[15] = CALL;
	fibCode[16] = 1;
	fibCode[17] = fibPointer.code[0];
	fibCode[18] = fibPointer.code[1];
	fibCode[19] = LOAD_ARG;
	fibCode[20] = 1;
	fibCode[21] = PUSH;
	fibCode[22] = 2;
	fibCode[23] = SUB;
	fibCode[24] = CALL;
	fibCode[25] = 1;
	fibCode[26] = fibPointer.code[0];
	fibCode[27] = fibPointer.code[1];
	fibCode[28] = ADD;
	fibCode[29] = RET;

	Code fib_mainCode[10];
	fib_mainCode[0] = PUSH;
	fib_mainCode[1] = 36;
	fib_mainCode[2] = CALL;
	fib_mainCode[3] = 1;
	fib_mainCode[4] = fibPointer.code[0];
	fib_mainCode[5] = fibPointer.code[1];
	fib_mainCode[6] = PRINT;
	fib_mainCode[7] = RET;

//	Method *mtd1 = {&fact_code};
//	Method *mtd2 = {&main_code};
//
//	execute(mtd2);

	vm.pc = 0;
	vm.funcPointer = 0;

	//execute(&vm, fact_mainCode);
	execute(&vm, fib_mainCode);

	return 0;
}
