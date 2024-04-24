#include <iostream>
#include "List.h"
#include "SLinkedList.h"
#include <string>
#include "Stack.h"



int main()
{
    using namespace std;
    const List l1 { 1, 2, 3 };
    List<int> l { 1,2,3 };
    cout << "Hello" << endl;

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