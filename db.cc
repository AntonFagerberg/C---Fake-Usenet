#include "db.h"
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

Db::Db(bool mem) : memory(mem) {}

vector<Newsgroup> Db::list_ng(){
	return ngs;
}
bool Db::create_newsgroup(string name, int i){
	for(Newsgroup ng: ngs){
		if(ng.name == name)	return false;
	}
	ngs.push_back(Newsgroup(name, i));
	if(i==-1 && !memory) save_to_file();
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
			if(a.ng_id == ng_id) a_remove.push_back(a.id);
		}
		for(int a_id: a_remove){
			delete_article(a_id);
		}
		ngs.erase(pos);
		if(!memory) save_to_file();
		return true;
	}
	return false;
}

vector<Article> Db::list_article(int ng_id){
	vector<Article> temp_a;
	for(Article a: articles){
		if(a.ng_id == ng_id) temp_a.push_back(a);
	}
	return temp_a;
}
bool Db::create_article(string title, string author, string text, int ng_id, int id, time_t upd){
	bool exists = false;
	for (auto it = ngs.begin() ; it != ngs.end(); ++it){
		if((*it).id == ng_id) exists = true;
	}
	if(exists){
		articles.push_back(Article(title, author, text, ng_id, id, upd));
		if(id==-1  && !memory) save_to_file();
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
		if(!memory) save_to_file();
		return true;
	}
	return false;
}


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
	ifstream nFile("newsgroup.txt");
	int largest_id = -1;
	string str;
	while (getline(nFile, str)){
    vector<string> attr;
    string::size_type start = 0;
    auto pos = str.find_first_of("||", start);
    while(pos != string::npos) {
        if(pos != start) attr.emplace_back(str, start, pos - start);
        start = pos + 2;
        pos = str.find_first_of("||", start);
    }
    if(start < str.length()) attr.emplace_back(str, start, str.length() - start);
		string name = attr[0];
		int id = atoi(attr[1].c_str());
		if(id > largest_id) largest_id = id;
		create_newsgroup(name, id);
	}
	Newsgroup::set_count(largest_id+1);

	ifstream aFile("article.txt");
	largest_id = -1;
	while (getline(aFile, str)){
    vector<string> attr;
    string::size_type start = 0;
    auto pos = str.find_first_of("||", start);
    while(pos != string::npos) {
        if(pos != start) attr.emplace_back(str, start, pos - start);
        start = pos + 2;
        pos = str.find_first_of("||", start);
    }
    if(start < str.length()) attr.emplace_back(str, start, str.length() - start);

		string title = attr[0];
		string author = attr[1];
		string text = attr[2];
		int ng_id = atoi(attr[3].c_str());
		time_t updated = atoi(attr[4].c_str());
		int id = atoi(attr[5].c_str());
		if(id > largest_id) largest_id = id;
		create_article(title, author, text, ng_id, id, updated);
	}
	Article::set_count(largest_id+1);
}

void Db::print(){
	cout << endl;
	for(Newsgroup ng: ngs){
		ng.print();
		for(Article a: list_article(ng.id)){
			a.print();
		}
	}
}