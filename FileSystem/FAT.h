#pragma once
#include <vector>
using namespace std;

class FAT
{
	int* fat;

public:
	//get
	int* get_fat() { return fat; };

	//set
	void set_size(int size) { fat = new int[size]; };
	
	//������ � FAT-��������
	int get_free_claster(int size);
	std::vector<int> get_free_clasters(int size, int count);;

	//���������
	int& operator[] (int i) { return fat[i]; };
};

