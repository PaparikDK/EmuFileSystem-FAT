#pragma once
#include <cstdint>
#include <string>
#include <sstream>
#include "sha256.h"

class Users
{
	uint64_t uid;			//8 байт
	char login[55];			//55 байт
	char password[65];		//65 байт

public:
	Users() 
	{
		uid = (uint8_t)0;
		strcpy_s(login, "");
		strcpy_s(password, "");
	};

	Users(uint64_t s_uid, char* s_login, char* s_password)
	{
		uid = s_uid;
		strcpy_s(login, s_login);
		strcpy_s(password, s_password);
	};

	//get
	uint64_t	get_uid()		{ return uid; };
	const char* get_login()		{ return login; };
	const char* get_password()	{ return password; };

	//set
	void set_uid(uint64_t s_uid)	 { uid = s_uid; };
	void set_login(char* s_login)	 { strcpy_s(login, s_login); };
	void set_password(char* s_password) 
	{ 
		std::string sha_pass = sha256(s_password);

		strcpy_s(password, sha_pass.c_str());
	};

	//операторы
	std::string toString()
	{
		std::stringstream oos;
		oos << uid << " " << login << " " << password << '\n';

		std::string temp = oos.str();
		return temp;
	}

	Users& operator=(const Users& other) 
	{
		if (this != &other) 
		{
			uid = other.uid;
			strcpy_s(login, other.login);
			strcpy_s(password, other.password);
		}
		return *this;
	}

	bool operator==(const Users& other)
	{
		if (strcmp(login, other.login) == 0 && strcmp(password,other.password) == 0) 
			return true;
		else
			return false;
	}
};

