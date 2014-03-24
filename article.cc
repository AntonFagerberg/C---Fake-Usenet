#include "article.h"
#include <string>
#include <time.h>
#include <iostream>

using namespace std;
int Article::count = 0;

Article::Article(string ti, string a, string te, int nid, int i, time_t tim) :  ng_id(nid), title(ti), author(a), text(te) {
 	if (i==-1){
 		id = count++;
 	}else{
 		id = i;
 	}
 	if(tim == -1){
 		time(&updated);
 	}else{
 		updated = tim;
 	}
}

void Article::print(){
	cout << "  Title:" << title << " Articleid:" << id << " text:" << text << " author:" << author << " time:" << updated << endl;
}

void Article::set_count(int val){
	count = val;
}