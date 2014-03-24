#ifndef NEWSGROUP_H
#define NEWSGROUP_H

#include <string>

class Newsgroup {
public:
	Newsgroup(std::string);
	void print();
	int id;
	std::string name;
private:
	static int count;
};

#endif
