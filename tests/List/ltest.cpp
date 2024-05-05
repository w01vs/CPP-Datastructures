#include "../../main/List.h"
#include <cassert>

int main()
{
	List<int> l1;

	assert(l1.empty());

	l1.append(1);
	assert(!l1.empty());
	assert(l1[0] == 1);
	assert(l1.size() == 1);
	assert(l1.at(0) == 1);

	l1.append(2);
	l1.append(3);

	assert(l1.size() == 3);
	assert(l1[0] == 1);
	assert(l1[1] == 2);
	assert(l1[2] == 3);

	l1.remove_at(0);
	assert(l1.size() == 2);
	assert(l1[0] == 2);
	assert(l1[1] == 3);

	l1.insert_at(1, 0);
	assert(l1.size() == 3);
	assert(l1[0] == 1);
	assert(l1[1] == 2);
	assert(l1[2] == 3);

	l1.clear();
	assert(l1.empty());
	assert(l1.size() == 0);

	l1.append(1);

	l1.append(2);
	l1.append(3);

	l1.remove_at(1);
	assert(l1.size() == 2);
	assert(l1[0] == 1);
	assert(l1[1] == 3);

	l1.insert_at(2, 1);
	assert(l1.size() == 3);
	assert(l1[0] == 1);
	assert(l1[1] == 2);
	assert(l1[2] == 3);

	return 0;
}