#include "newsgroup.h"
#include <string>
#include <iostream>

using namespace std;
int Newsgroup::count = 0;

Newsgroup::Newsgroup(string n) : name(n){
	id = count++;
}

void Newsgroup::print(){
	cout << "Name:" << name << " id:" << id << endl;
}