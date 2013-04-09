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

	Code mainCode[10];
	mainCode[0] = PUSH;
	mainCode[1] = 10; //arg
	mainCode[2] = CALL;
	mainCode[3] = 1;
	mainCode[4] = factPointer.code[0];
	mainCode[5] = factPointer.code[1];
	mainCode[6] = PRINT;
	mainCode[7] = RET;

//	Method *mtd1 = {&fact_code};
//	Method *mtd2 = {&main_code};
//
//	execute(mtd2);

	vm.pc = 0;
	vm.funcPointer = 0;

	execute(&vm, mainCode);

	return 0;
}
