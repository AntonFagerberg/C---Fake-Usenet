#include "newsgroup.h"
#include <string>
#include <iostream>

using namespace std;
int Newsgroup::count = 0;

Newsgroup::Newsgroup(string n, int i) : name(n){
	if(i==-1){
		id = count++;
	}else{
		id = i;
	}
}

void Newsgroup::print(){
	cout << "Name:" << name << " id:" << id << endl;
}

void Newsgroup::set_count(int val){
	count = val;
}