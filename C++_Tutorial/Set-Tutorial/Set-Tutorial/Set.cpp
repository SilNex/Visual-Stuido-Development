#include <iostream>
#include <set>

using namespace std;

int main(void) {
	char * t = "abc";
	set<char *> s;
	s.insert(t);

	for (auto i = s.begin(); i != s.end(); i++)
		cout << *i << endl;
	s.erase(t);
	for (auto i = s.begin(); i != s.end(); i++)
		cout << *i << endl;
}