#pragma once
#include "BootSector.h"
#include "FSInfo.h"
#include "FAT.h"
#include "Users.h"
using namespace std;

class FileSys
{
	int size;
	BootSector bootsector;
	FSInfo fsinfo;
	FAT fat;
	string file_name;
	Users active_user;

public:
	FileSys();

	//get
	BootSector	get_bootsector() { return bootsector; };
	string		get_file_name() { return file_name; };
	Users		get_active_user() { return active_user; };

	//set
	void set_size(int size);
	void set_file_name(string name);
	void set_active_user(Users user);

	//работа с файлом ФС
	void create_filesys(string name, int size);
	void open_filesys(string name);
	void write(char* source, int offset, int size);
	void read(char* output, int offset, int size);

	//работа с ФС
	void   mk_file(string new_name, string new_text, uint8_t new_atr, uint8_t new_rwx, bool isAdd_rec);
	void   mv_file(string old_name, string new_name);
	void   cp_file(string old_name, string new_name);
	string rd_file(string name);
	void   rm_file(string name);
	void   files();

	void   ap_file(string name, uint8_t rwx);

	void   users_list(const std::vector<Users>& users);
	void   users_switch(const std::vector<Users>& users, string login, string password);

	//операторы
	friend std::ostream& operator<<(std::ostream& stream, const FileSys& filesys)
	{
		stream << "Размер:              " << filesys.size << "\n";
		stream << filesys.bootsector;
		stream << filesys.fsinfo;

		return stream;
	}
};
