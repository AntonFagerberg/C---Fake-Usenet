#include "db.h"
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

Db::Db(){
	// create_newsgroup("Worldnews");
	// create_newsgroup("Science");
	// create_newsgroup("Nature");

	// create_article("Ukrane crisis", "Anders Nylen", "KRIG!!!!", 0);
	// create_article("Big bang waves", "Ture Karlsson", "bla bla bla", 1);
	// create_article("Big foot", "Anders Nylen", "HJÄLP!?!?!?", 2);
}


vector<Newsgroup> Db::list_ng(){
	return ngs;
}
bool Db::create_newsgroup(string name, int i){
	for(Newsgroup ng: ngs){
		if(ng.name == name){
			return false;
		}
	}
	Newsgroup ng(name, i);
	ngs.push_back(ng);
	if(i==-1){
		save_to_file();
	}
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
		vector<int> a_remove;
		for(Article a: articles){
			if(a.ng_id == ng_id){
				a_remove.push_back(a.id);
			}
		}
		for(int a_id: a_remove){
			delete_article(a_id);
		}

		ngs.erase(pos);
		save_to_file();
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
bool Db::create_article(string title, string author, string text, int ng_id, int id, time_t upd){
	bool exists = false;
	for (auto it = ngs.begin() ; it != ngs.end(); ++it){
		if((*it).id == ng_id){
			exists = true;
		}
	}

	if(exists){
		articles.push_back(Article(title, author, text, ng_id, id, upd));
		if(id==-1){
			save_to_file();
		}
		return true;
	}
	return false;
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
		save_to_file();
		return true;
	}
	return false;
}

/*int main(){
	Db db;
	db.read_from_file();

	// db.create_newsgroup("Worldnews1");
	// db.create_newsgroup("Science1");
	// db.create_newsgroup("Nature1");

	// db.create_article("Ukrane crisis", "Anders Nylen", "KRIG!!!!", 1);
	// db.create_article("Big bang waves", "Ture Karlsson", "bla bla bla", 2);
	// db.create_article("Big foot", "Anders Nylen", "HJÄLP!?!?!?", 0);

	// cout << endl;
	// for(Newsgroup ng: db.ngs){
	// 	ng.print();
	// 	for(Article a: db.list_article(ng.id)){
	// 		a.print();
	// 	}
	// }
	// db.save_to_file();
	return 0;
}*/


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
	ifstream file2("newsgroup.txt");
	int largest_id = -1;
	string str;
	while (getline(file2, str)){
    vector<string> attr;
    string::size_type start = 0;
    auto pos = str.find_first_of("||", start);
    while(pos != string::npos) {
        if(pos != start) // ignore empty tokens
            attr.emplace_back(str, start, pos - start);
        start = pos + 2;
        pos = str.find_first_of("||", start);
    }
    if(start < str.length()) // ignore trailing delimiter
        attr.emplace_back(str, start, str.length() - start); // add what's left of the string

		string name = attr[0];
		int id = atoi(attr[1].c_str());
		if(id > largest_id){
			largest_id = id;
		}
		create_newsgroup(name, id);
	}

	Newsgroup::set_count(largest_id+1);

	ifstream file("article.txt");
	largest_id = -1;
	while (getline(file, str)){
    vector<string> attr;
    string::size_type start = 0;
    auto pos = str.find_first_of("||", start);
    while(pos != string::npos) {
        if(pos != start) // ignore empty tokens
            attr.emplace_back(str, start, pos - start);
        start = pos + 2;
        pos = str.find_first_of("||", start);
    }
    if(start < str.length()) // ignore trailing delimiter
        attr.emplace_back(str, start, str.length() - start); // add what's left of the string

		string title = attr[0];
		string author = attr[1];
		string text = attr[2];
		int ng_id = atoi(attr[3].c_str());
		time_t updated = atoi(attr[4].c_str());
		int id = atoi(attr[5].c_str());
		if(id > largest_id){
			largest_id = id;
		}
		create_article(title, author, text, ng_id, id, updated);
	}
	Article::set_count(largest_id+1);


}