#include "../../main/DLinkedList.h"
#include <cassert>

int main()
{
	DLinkedList<int> l1;

	assert(l1.Empty());

	l1.Append(1);
	assert(!l1.Empty());
	assert(l1[0] == 1);
	assert(l1.Size() == 1);
	assert(l1.At(0) == 1);

	l1.Append(2);
	l1.Append(3);

	assert(l1.Size() == 3);
	assert(l1[0] == 1);
	assert(l1[1] == 2);
	assert(l1[2] == 3);

	l1.RemoveAt(0);
	assert(l1.Size() == 2);
	assert(l1[0] == 2);
	assert(l1[1] == 3);

	l1.InsertAt(1, 0);
	assert(l1.Size() == 3);
	assert(l1[0] == 1);
	assert(l1[1] == 2);
	assert(l1[2] == 3);

	l1.Clear();
	assert(l1.Empty());
	assert(l1.Size() == 0);

	l1.Append(1);

	l1.Append(2);
	l1.Append(3);

	l1.RemoveAt(1);
	assert(l1.Size() == 2);
	assert(l1[0] == 1);
	assert(l1[1] == 3);

	l1.InsertAt(2, 1);
	assert(l1.Size() == 3);
	assert(l1[0] == 1);
	assert(l1[1] == 2);
	assert(l1[2] == 3);

	return 0;
}
