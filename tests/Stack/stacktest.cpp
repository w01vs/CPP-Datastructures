#include "../../main/Stack.h"
#include <cassert>

int main()
{
	Stack<int> stack;
	assert(stack.Empty());
	stack.Push(5);
	assert(!stack.Empty());
	assert(stack.Size() == 1);
	const int t = stack.Peek();
	assert(stack.Size() == 1);
	assert(t == 5);
	const int p = stack.Pop();
	assert(stack.Size() == 0);
	assert(p == 5);
	assert(stack.Empty());
}