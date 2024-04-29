#include <iostream>
#include "List.h"
#include "SLinkedList.h"
#include <string>
#include "Stack.h"
#include "DLinkedList.h"
#include "Queue.h"
#include "HashMap.h"



int main()
{
    using namespace std;
    const List l1 { 1, 2, 3 };
    List<int> l { 1,2,3 };
    SLinkedList<int> pp { 0, 1, 2, 3, 4, 5};
    std::cout << pp << std::endl;
    pp[3] = 7;
    std::cout << pp.At(3) << std::endl;
    std::cout << pp << std::endl;
    bool t = true;
    while(t)
    {
        string s;
        cin >> s;
        if(s == "q")
        {
            t = false;
        }
        else if(s == "w")
        {
            l.Append(0);
        }
        else if(s == "r")
        {
			l.RemoveAt(0);
        }
        else if(s == "p")
		{
            cout << l << endl;
		}
        else if(s == "c")
        {
            l.Clear();
        }
        else if(s == "s")
        {
	        cout << l.Size() << endl;
        }
    }
}