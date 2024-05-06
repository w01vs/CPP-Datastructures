#include <iostream>
#include "List.h"
#include "SLinkedList.h"
#include <string>
#include "Stack.h"
#include "DLinkedList.h"
#include "Queue.h"
#include "HashMap.h"
#include "Tree.h"



int main()
{
    using namespace std;
    const List l1 { 1, 2, 3 };
    List l { 1,2,3 };
    HashMap<std::string, int> map;
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
            l.append(0);
        }
        else if(s == "r")
        {
			l.remove_at(0);
        }
        else if(s == "p")
		{
            cout << l << endl;
		}
        else if(s == "c")
        {
            l.clear();
        }
        else if(s == "s")
        {
	        cout << l.size() << endl;
        }
    }
}