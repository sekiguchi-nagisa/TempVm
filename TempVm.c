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
	Code* codeList;
} VM;

#define EXEC_BINARY_OP(vm, OP) do{\
	int arg2 = vm->stack[vm->stackTop];\
	int arg1 = vm->stack[--vm->stackTop];\
	vm->stack[vm->stackTop] = arg1 OP arg2;\
}while(0)

void execute(VM *vm)
{
	while(1){
		switch (vm->codeList[vm->pc]) {
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
				break;
			}
		case POP:
			vm->stackTop--;
			break;
		case PUSH:
			vm->stack[++vm->stackTop] = vm->codeList[++vm->pc];
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
			int label = vm->codeList[++vm->pc];
			if (arg) {
				vm->pc = label - 1;
			}
			break;}
		case CALL:{
			vm->stack[++vm->stackTop] = vm->codeList[++vm->pc];
			int label = vm->codeList[++vm->pc];
			vm->stack[++vm->stackTop] = vm->pc;
			vm->stack[++vm->stackTop] = vm->funcPointer;
			vm->pc = label - 1;
			vm->funcPointer = vm->stackTop;
			break;}
		case LOAD_ARG:{
			int num = vm->codeList[++vm->pc];
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

	Code add_example[40];
	add_example[0] = PUSH;
	add_example[1] = 10; //arg
	add_example[2] = CALL; //add_example[3] = mtd1;
	add_example[3] = 1;
	add_example[4] = 7;
	add_example[5] = PRINT;
	add_example[6] = RET;
	add_example[7] = LOAD_ARG;
	add_example[8] = 1;
	add_example[9] = PUSH;
	add_example[10] = 2;
	add_example[11] = GTEQ;
	add_example[12] = IF_JUMP;
	add_example[13] = 18;
	add_example[14] = PUSH;
	add_example[15] = 1;
	add_example[16] = PRINT;
	add_example[17] = RET;
	add_example[18] = LOAD_ARG;
	add_example[19] = 1;
	add_example[20] = PUSH;
	add_example[21] = 1;
	add_example[22] = SUB;
	add_example[23] = CALL;
	add_example[24] = 1;
	add_example[25] = 7;
	add_example[26] = LOAD_ARG;
	add_example[27] = 1;
	add_example[28] = MUL;
	add_example[29] = PRINT;
	add_example[30] = RET;
	/*
	 * PUSH,
	 * 1,
	 * PUSH,
	 * 2,
	 * CALL,
	 * 2, // number of args.
	 * &ADD, // index of entry point.
	 */


//	Method *mtd1 = {&fact_code};
//	Method *mtd2 = {&main_code};
//
//	execute(mtd2);


	vm.codeList = add_example;
	vm.pc = 0;
	vm.funcPointer = 0;

	execute(&vm);

	return 0;
}
