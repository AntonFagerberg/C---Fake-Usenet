#include "db.h"
#include <vector>
#include <string>
#include <fstream>

using namespace std;

Db::Db(){
	/*create_newsgroup("Worldnews");
	create_newsgroup("Science");
	create_newsgroup("Nature");

	create_article("Ukrane crisis", "Anders Nylen", "KRIG!!!!", 0);
	create_article("Big bang waves", "Ture Karlsson", "bla bla bla", 1);
	create_article("Big foot", "Anders Nylen", "HJÄLP!?!?!?", 2);*/
}


vector<Newsgroup> Db::list_ng(){
	return ngs;
}
bool Db::create_newsgroup(string name){
	for(Newsgroup ng: ngs){
		if(ng.name == name){
			return false;
		}
	}
	Newsgroup ng(name);
	ngs.push_back(ng);
	return true;
}

bool Db::delete_newsgroup(int ng_id){
	std::vector<Newsgroup>::iterator pos;
	bool exists = false;
	for (auto it = ngs.begin() ; it != ngs.end(); ++it){
		if((*it).id == ng_id){
			pos = it;
			exists = true;
		}
	}
	if(exists){
		ngs.erase(pos);
		return true;
	}
	return false;
}

vector<Article> Db::list_article(int ng_id){
	vector<Article> temp_a;
	for(Article a: articles){
		if(a.ng_id == ng_id){
			temp_a.push_back(a);
		}
	}
	return temp_a;
}
Article Db::create_article(string title, string author, string text, int ng_id){
	Article a(title, author, text, ng_id);
	articles.push_back(a);
	return a;
}

bool Db::delete_article(int a_id){
	std::vector<Article>::iterator pos;
	bool exists = false;
	for (auto it = articles.begin() ; it != articles.end(); ++it){
		if((*it).id == a_id){
			pos = it;
			exists = true;
		}
	}
	if(exists){
		articles.erase(pos);
		return true;
	}
	return false;
}

//int main(){
	// Db db;
	// for(Newsgroup ng: db.ngs){
	// 	ng.print();
	// 	for(Article a: db.list_article(ng.id)){
	// 		a.print();
	// 	}
	// }
	// db.save_to_file();
	// return 0;
//}


void Db::save_to_file(){
  ofstream myfile;
  myfile.open ("article.txt");
  for(Article a: articles){
  	myfile << a.title << "||" << a.author << "||" << a.text << "||" << a.ng_id << "||" << a.updated << "||" << a.id << endl;
  }
  myfile.close();

  myfile.open ("newsgroup.txt");
  for(Newsgroup ng: ngs){
  	myfile << ng.name << "||" << ng.id << endl;
  }
  myfile.close();
}
void Db::read_from_file(){

}