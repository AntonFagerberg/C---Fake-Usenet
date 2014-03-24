#ifndef DB_H
#define DB_H

#include "newsgroup.h"
#include "article.h"
#include <vector>
#include <string>

class Db {
public:
	Db();
	std::vector<Newsgroup> list_ng();
	bool create_newsgroup(std::string, int=-1);
	bool delete_newsgroup(int);

	std::vector<Article> list_article(int);
	bool create_article(std::string, std::string, std::string, int, int=-1, time_t=-1);
	bool delete_article(int);
	std::vector<Article> articles;
	std::vector<Newsgroup> ngs;
	void save_to_file();
	void read_from_file();
private:
};

#endif
