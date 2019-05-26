#include <iostream>
#include <fstream>
#include <cstdint>
#include <random>
#include <cstring>

//create test

using namespace std;


void generate_file(const char * filename,const uint64_t NElems)
{
	ofstream file;
	file.open(filename, ios::binary);
	if (!file.is_open())
		throw runtime_error("File couldn't open!");

	for (uint64_t i = 0; i < NElems; i++){
		uint64_t  val =  static_cast<uint64_t>(rand()%200);
		char c[4];
		memcpy(c,&val,sizeof(uint64_t)); 
		file.write(c,sizeof(uint64_t));
		cout << val << endl;
	}
	file.close();
	return;
}

int main(void)
{
	generate_file("test", 5000);
	return 0;
}
