#include "../../main/List.h"
#include <cassert>
#include <iostream>

int main()
{
	using namespace std;
	List< List<int> > list;
	assert(list.Size() == 0);
	assert(list.Empty());

	const List<int> element1 = { 1, 2, 3 };
	const List<int> element2 = { 4, 5, 6 };
	const List<int> element3 = { 7, 8, 9 };
	list.Append(element1);
	assert(list.Size() == 1);
	assert(!list.Empty());
	assert(list[0] == element1);
	assert(list.At(0) == element1);
	assert(list.At(0)[0] == 1);
	assert(list.At(0)[1] == 2);
	assert(list.At(0)[2] == 3);

	assert(list == list);
	const List< List<int> > l2 {element2 };
	assert(list != l2);
	const List<List<int> > list2 = list;
	assert(list == list2);

	list.Append(element2);
	list.Append(element3);
	assert(list.Size() == 3);
	assert(list[0] == element1);
	assert(list.At(0)[0] == 1);
	assert(list.At(0)[1] == 2);
	assert(list.At(0)[2] == 3);
	assert(list[1] == element2);
	assert(list.At(1)[0] == 4);
	assert(list.At(1)[1] == 5);
	assert(list.At(1)[2] == 6);
	assert(list[2] == element3);
	assert(list.At(2)[0] == 7);
	assert(list.At(2)[1] == 8);
	assert(list.At(2)[2] == 9);

	list.RemoveAt(1);
	assert(list.Size() == 2);
	assert(list[0] == element1);
	assert(list[1] == element3);

	list.InsertAt(element2, 1);
	assert(list.Size() == 3);
	assert(list[0] == element1);
	assert(list[1] == element2);
	assert(list[2] == element3);

	list[0][0] = 10;

	assert(list[0][0] == 10);
	assert(list.At(0).At(0) == 10);
	list.Resize(2, true);
	cout << list.Size() << endl;
	cout << list << endl;
	list[0][0] = 1;

	assert(list.Size() == 2);
	assert(list[0] == element1);
	assert(list[1] == element2);

	List<int> v = list.At(0);

	try
	{
		v = list.At(2);
		assert(false);
	}
	catch([[maybe_unused]] const std::exception& e)
	{

	}

	return 0;
}