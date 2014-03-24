#ifndef NEWSGROUP_H
#define NEWSGROUP_H

#include <string>

class Newsgroup {
public:
	Newsgroup(std::string, int);
	void print();
	int id;
	std::string name;
	static void set_count(int);
private:
	static int count;
};

#endif
