#include <iostream>
#include <cstdint>
#include <locale.h>
#include <conio.h>
#include <string>
#include <fstream>
#include "FileSys.h"
#include "Terminal.h"
#include "Users.h"
using namespace std;

int main()
{
	FileSys fileSys;

	//Меню программы
	int key, size, index_user;
	string name_file, temp;
	ifstream diskName;

	Users user;
	Users active_user;
	std::vector<Users> users_list;

	stringstream ss;
	string substring;
	bool isBreak = false;

	setlocale(0, "RUS");
	cout << "KSys FileSystem:" << endl;
	cout << "[1] Смонтировать диск" << endl;
	cout << "[2] Открыть диск" << endl;
	cout << "Нажмите другую клавишу, чтобы выйти. . ." << endl << endl;
	key = _getch();
	switch (key)
	{
	case '1':
		//Создание файла ФС
		system("cls");
		cout << "Создание файловой системы [...] => Задание пользователя root [] => Создание нового пользователя []" << endl;
		cout << "Название файла ФС: ";
		std::cin >> name_file;
		cout << "Размер (Мб): ";
		std::cin >> size;
		fileSys.set_size(size << 20);
		fileSys.create_filesys(name_file, size << 20);

		//Создание пользователя root
		system("cls");
		cout << "Создание файловой системы [x] => Задание пользователя root [...] => Создание нового пользователя []" << endl;
		cout << "Пароль: ";
		std::cin >> temp;

		user.set_uid((uint64_t)0);
		user.set_login((char*)"root");
		user.set_password((char*)temp.c_str());
		
		users_list.push_back(user);
		fileSys.set_active_user(user);
		fileSys.mk_file((char*)"users.sys", user.toString(), 0b011, 0b110000, false);

		active_user = user;

		//Создание нового пользователя
		system("cls");
		cout << "Создание файловой системы [x] => Задание пользователя root [x] => Создание нового пользователя [...]" << endl;
		cout << "Логин: ";
		std::cin >> temp;
		user.set_login((char*)temp.c_str());
		cout << "Пароль: ";
		std::cin >> temp;
		user.set_password((char*)temp.c_str());
		user.set_uid((uint64_t)1);

		users_list.push_back(user);
		fileSys.mk_file((char*)"users.sys", user.toString(), 0b011, 0b110000, true);

		users_list.clear();
		user.set_uid(0);
		user.set_login((char*)"");
		user.set_password((char*)"");
		fileSys.set_active_user(user);
		ss.str(fileSys.rd_file("users.sys"));
		while (std::getline(ss, substring, '\n')) {
			std::stringstream word_ss(substring);
			std::string word;

			std::getline(word_ss, word, ' ');
			user.set_uid(stoull(word));

			std::getline(word_ss, word, ' ');
			user.set_login((char*)word.c_str());

			std::getline(word_ss, word, ' ');
			user.set_password((char*)word.c_str());

			users_list.push_back(user);
		}

		if (users_list.empty())
		{
			cout << "\033[1;31mПользователи ФС не были найдены! Вход невозможен.\033[0m\n";
			system("pause");
			return -1;
		}
		break;

	case '2':
		do
		{
			system("cls");
			cout << "Открытие файловой системы [...] => Вход в пользователя []" << endl;

			cout << "Название файла: ";
			cin >> name_file;
			diskName.open((name_file + ".fsys").c_str());

			if (diskName.fail())
			{
				cout << "\033[1;31mТакого файла не существует! Проверьте правильность ввода.\033[0m\n";
				system("pause");
			}
		} 
		while (diskName.fail());
		diskName.close();
		fileSys.open_filesys(name_file);

		//Читаем, какие есть пользователи в ФС
		user.set_uid(0);
		user.set_login((char*)"");
		user.set_password((char*)"");
		fileSys.set_active_user(user);
		ss.str(fileSys.rd_file("users.sys"));
		while (std::getline(ss, substring, '\n')) {
			std::stringstream word_ss(substring);
			std::string word;

			std::getline(word_ss, word, ' ');
			user.set_uid(stoull(word));
			
			std::getline(word_ss, word, ' ');
			user.set_login((char*)word.c_str());

			std::getline(word_ss, word, ' ');
			user.set_password((char*)word.c_str());

			users_list.push_back(user);
		}

		if (users_list.empty())
		{
			cout << "\033[1;31mПользователи ФС не были найдены! Вход невозможен.\033[0m\n";
			system("pause");
			return -1;
		}

		//Вход в ФС
		do
		{
			system("cls");
			cout << "Открытие файловой системы [x] => Вход в пользователя [...]" << endl;

			cout << "Логин: ";
			std::cin >> temp;
			user.set_login((char*)temp.c_str());
			cout << "Пароль: ";
			std::cin >> temp;
			user.set_password((char*)sha256(temp).c_str());

			for (index_user = 0; index_user < users_list.size(); index_user++)
			{
				if (users_list[index_user] == user)
				{
					isBreak = true;
					break;
				}
			}
		} 
		while (isBreak != true);

		active_user = users_list[index_user];

		break;

	default: exit(3);
	}

	//Работа с терминалом
	system("cls");

	Terminal terminal;
	string command;
	
	terminal.set_active_user(active_user);
	fileSys.set_active_user(active_user);

	cin.ignore(1);
	while (true)
	{
		active_user = terminal.get_active_user();

		cout << "\033[1m";
		printf_s("%s@%lu: ", active_user.get_login(), (int)active_user.get_uid());
		cout << "\033[0m";
		getline(cin, command);
		terminal.command(command, fileSys, users_list);
		cout << endl;
	}
	return 0;
}