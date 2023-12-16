#pragma once
#include <iostream>
#include <cstdint>
#include <locale.h>
#include <conio.h>
#include <fstream>
#include "FileSys.h"
#include "Users.h"
using namespace std;

class Terminal
{
	Users active_user;

public:

	void  set_active_user(Users user) { active_user = user; };
	Users get_active_user() { return active_user; };

	void command(string command, FileSys fileSys, std::vector<Users>& users);
};

