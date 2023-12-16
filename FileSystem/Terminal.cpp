#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include "Terminal.h"

void Terminal::command(string command, FileSys fileSys, std::vector<Users>& users)
{
	istringstream iss(command);
	vector<string> tokens;

	Users user;

	string token, param;
	while (getline(iss, token, ' ')) {
		tokens.push_back(token);
	}

	fileSys.set_active_user(active_user);

	if (tokens.size() == 0)
		return;

	if (tokens[0] == "fsinfo")
	{
		if (tokens.size() != 1)
		{
			cout << "Данная команда не принимает параметров: fsinfo" << endl;
		}
		else
		{
			cout << "Имя файла: " << fileSys.get_file_name() << endl << endl;
			cout << fileSys;
		}

		return;
	}

	if (tokens[0] == "mkfl")
	{
		if (tokens.size() != 2)
		{
			cout << "Данная команда принимает 1 параметр: mkfl <Название_файла>" << endl;
		}
		else
		{
			fileSys.mk_file(tokens[1], "", 0b000, 0b111111, false);
		}

		return;
	}

	if (tokens[0] == "mvfl")
	{
		if (tokens.size() != 3)
		{
			cout << "Данная команда принимает 2 параметра: mvfl <Название_файла> <Новое_название_файла>" << endl;
		}
		else
		{
			fileSys.mv_file(tokens[1], tokens[2]);
		}

		return;
	}

	if (tokens[0] == "wrfl")
	{
		if (tokens.size() < 2 || (tokens.size() == 3 && tokens[2] != "<") || tokens.size() > 3)
		{
			cout << "Данная команда принимает 1 или 2 параметра:\nwrfl <Название_файла>\nwrfl <Название_файла> <" << endl;
		}
		else if (tokens.size() == 2)
		{
			cout << "Введите текст:" << endl;
			getline(cin, param);
			fileSys.mk_file(tokens[1], param, 0b000, 0b111111, false);
		}
		else if (tokens.size() == 3 && tokens[2] == "<")
		{
			cout << "Введите текст:" << endl;
			getline(cin, param);
			fileSys.mk_file(tokens[1], param, 0b000, 0b111111, true);
		}

		return;
	}

	if (tokens[0] == "cpfl")
	{
		if (tokens.size() != 3)
		{
			cout << "Данная команда принимает 2 параметра: rmfl <Название_файла> <Название_нового_файла>" << endl;
		}
		else
		{
			fileSys.cp_file(tokens[1], tokens[2]);
		}

		return;
	}

	if (tokens[0] == "rdfl")
	{
		if (tokens.size() != 2)
		{
			cout << "Данная команда принимает 1 параметр: rdfl <Название_файла>" << endl;
		}
		else
		{
			cout << fileSys.rd_file(tokens[1]) << endl;
		}

		return;
	}

	if (tokens[0] == "rmfl")
	{
		if (tokens.size() != 2)
		{
			cout << "Данная команда принимает 1 параметр: rmfl <Название_файла>" << endl;
		}
		else
		{
			fileSys.rm_file(tokens[1]);
		}

		return;
	}

	if (tokens[0] == "files")
	{
		if (tokens.size() != 1)
		{
			cout << "Данная команда не принимает параметров: files" << endl;
		}
		else
		{
			printf("\033[21m%-11s\t%-3s", "Имя файла", "UID");
			cout << "\tДата создания\t\tДата открытия\t\tРазмер\tАтр.\tЗащита\033[0m\n";
			fileSys.files();
		}

		return;
	}

	if (tokens[0] == "apfl")
	{
		if (tokens.size() != 3)
		{
			cout << "Данная команда принимает 2 параметра: apfl <Название_файла> <0-63>" << endl;
		}
		else
		{
			fileSys.ap_file(tokens[1], stoull(tokens[2]));
		}

		return;
	}

	if (tokens[0] == "users")
	{
		if (tokens.size() != 1)
		{
			cout << "Данная команда не принимает параметров: users" << endl;
		}
		else
		{
			printf("\033[21m%-3s\t%-11s\033[0m\n", "UID", "Пользователь");
			fileSys.users_list(users);
		}

		return;
	}

	if (tokens[0] == "usr" && tokens[1] == ">")
	{
		if (tokens.size() != 4 && tokens[1] != "<")
		{
			cout << "Данная команда принимает 3 параметра:\nusr > <Логин> <Пароль>" << endl;
		}
		else
		{
			fileSys.users_switch(users, tokens[2], tokens[3]);
			set_active_user(fileSys.get_active_user());
		}

		return;
	}

	if (tokens[0] == "mkusr")
	{
		if (tokens.size() != 3)
		{
			cout << "Данная команда принимает 2 параметра:\nmkusr <Логин> <Пароль>" << endl;
		}
		else
		{
			user.set_login((char*)tokens[1].c_str());
			user.set_password((char*)tokens[2].c_str());
			user.set_uid((uint64_t)(users[users.size()-1].get_uid() + 1));

			users.push_back(user);
			fileSys.mk_file((char*)"users.sys", user.toString(), 0b010, 0b000110, true);
		}

		return;
	}

	if (tokens[0] == "clear")
	{
		if (tokens.size() != 1)
		{
			cout << "Данная команда не принимает параметров: clear" << endl;
		}
		else
		{
			system("cls");
		}

		return;
	}

	if (tokens[0] == "exit")
	{
		if (tokens.size() != 1)
		{
			cout << "Данная команда не принимает параметров: exit" << endl;
		}
		else
		{
			exit(3);
		}

		return;
	}

	cout << "Такой команды не существует!" << endl;
}
