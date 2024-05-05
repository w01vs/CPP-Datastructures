#include "../../main/Stack.h"
#include <cassert>

int main()
{
	Stack<int> stack;
	assert(stack.empty());
	stack.push(5);
	assert(!stack.empty());
	assert(stack.size() == 1);
	const int t = stack.peek();
	assert(stack.size() == 1);
	assert(t == 5);
	const int p = stack.pop();
	assert(stack.size() == 0);
	assert(p == 5);
	assert(stack.empty());
}