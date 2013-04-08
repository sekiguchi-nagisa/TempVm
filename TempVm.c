/*
 * TempVm.c
 *
 *  Created on: 2013/04/08
 *      Author: skgchxngsxyz-work
 */

#include <stdio.h>

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
	IF_JUMP,
	CALL,
	SET_ARG,
	LOAD_ARG,
	STORE_ARG,
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
			}

			break;
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
		case DIV:
			EXEC_BINARY_OP(vm, /);
			break;
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
			int label = vm->codeList[++vm->pc];
			vm->stack[++vm->stackTop] = vm->pc;
			vm->stack[++vm->stackTop] = vm->funcPointer;
			vm->pc = label - 1;
			vm->funcPointer = vm->stackTop;
			break;}
		case SET_ARG:
			vm->stack[++vm->stackTop] = vm->codeList[++vm->pc];
			break;
		}
		vm->pc++;
	}
}

int main (void)
{
	VM vm = {0};
	Code add_example[] = {
			PUSH,
			22,
			PUSH,
			11,
			PUSH,
			2,
			GT,
			IF_JUMP,
			11,
			PRINT,
			RET,
			PUSH,
			11,
			PUSH,
			2,
			SUB,
			PRINT,
			RET,
	};
	vm.codeList = add_example;
	vm.pc = 0;

	execute(&vm); // 3

	return 0;
}
