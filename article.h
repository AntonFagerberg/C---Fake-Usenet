#ifndef ARTICLE_H
#define ARTICLE_H

#include <time.h>
#include <string>

class Article {
public:
	Article(std::string, std::string, std::string, int, int, time_t);
	void print();
	int ng_id;
	int id;
	std::string title;
	std::string author;
	std::string text;
	time_t updated;
	static void set_count(int);
private:
	static int count;
};

#endif
