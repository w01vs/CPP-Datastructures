#include "../../main/List.h"
#include <cassert>
#include <iostream>

int main()
{
	using namespace std;
	List< List<int> > list;
	assert(list.size() == 0);
	assert(list.empty());

	const List<int> element1 = { 1, 2, 3 };
	const List<int> element2 = { 4, 5, 6 };
	const List<int> element3 = { 7, 8, 9 };
	list.append(element1);
	assert(list.size() == 1);
	assert(!list.empty());
	assert(list[0] == element1);
	assert(list.at(0) == element1);
	assert(list.at(0)[0] == 1);
	assert(list.at(0)[1] == 2);
	assert(list.at(0)[2] == 3);

	assert(list == list);
	const List< List<int> > l2 {element2 };
	assert(list != l2);
	const List<List<int> > list2 = list;
	assert(list == list2);

	list.append(element2);
	list.append(element3);
	assert(list.size() == 3);
	assert(list[0] == element1);
	assert(list.at(0)[0] == 1);
	assert(list.at(0)[1] == 2);
	assert(list.at(0)[2] == 3);
	assert(list[1] == element2);
	assert(list.at(1)[0] == 4);
	assert(list.at(1)[1] == 5);
	assert(list.at(1)[2] == 6);
	assert(list[2] == element3);
	assert(list.at(2)[0] == 7);
	assert(list.at(2)[1] == 8);
	assert(list.at(2)[2] == 9);

	list.remove_at(1);
	assert(list.size() == 2);
	assert(list[0] == element1);
	assert(list[1] == element3);

	list.insert_at(element2, 1);
	assert(list.size() == 3);
	assert(list[0] == element1);
	assert(list[1] == element2);
	assert(list[2] == element3);

	list[0][0] = 10;

	assert(list[0][0] == 10);
	assert(list.at(0).at(0) == 10);
	list.resize(2, true);
	list[0][0] = 1;

	assert(list.size() == 2);
	assert(list[0] == element1);
	assert(list[1] == element2);

	List<int> v = list.at(0);

	try
	{
		v = list.at(2);
		assert(false);
	}
	catch([[maybe_unused]] const std::exception& e)
	{

	}

	return 0;
}