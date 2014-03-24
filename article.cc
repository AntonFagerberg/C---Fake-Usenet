#include "article.h"
#include <string>
#include <time.h>
#include <iostream>

using namespace std;
int Article::count = 0;

Article::Article(string ti, string a, string te, int nid) : title(ti), author(a), text(te), ng_id(nid){
 	id = count++;
 	time(&updated);
}

void Article::print(){
	cout << "Title:" << title << " Articleid:" << id << " text:" << text << " author:" << author << endl;
}