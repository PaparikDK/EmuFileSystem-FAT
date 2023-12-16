#include <iostream>
#include <cstdint>
#include <locale.h>
#include <conio.h>
#include <fstream>
#include "FileSys.h"
#include "File.h"
#include "Users.h"
using namespace std;

FileSys::FileSys() { }

//set
void FileSys::set_size(int s)
{
	int index = 1;

	//Размер файла системы в байтах
	size = s;

	//Устанавливаем размерности в системе
	bootsector.set_size(s);
	fat.set_size(bootsector.get_fatSize());

	//Помечаем занятые системой кластеры
	fat[0] = -2;
	for (int i = bootsector.get_fatSize(); i >= 0; i -= bootsector.get_sectSize(), index += 1)
	{
		fat[index] = -2;
	}

	//Выделяем каталог (10% кластеров)
	fsinfo.set_rootDirCl(index + 1);
	for (int i = 0; i < (bootsector.get_fatSize() >> 2) / 10; i += 1, index += 1)
	{
		fat[index] = -2;
	}

	fsinfo.set_numFreeCl(bootsector.get_sectTotal() - (index + 1));
	fsinfo.set_nxtFreeCl(index);
}

void FileSys::set_file_name(string name) { file_name = name; }

void FileSys::set_active_user(Users user) { active_user = user; }

//Работа с файлом ФС
void FileSys::create_filesys(string name, int size)
{
	//Создаём файл ФС
	ofstream file;
	file.open(name + ".fsys", std::ios::binary);
	file_name = name + ".fsys";

	//Задаём размер файлу ФС
	char cluster[512];
	streamsize remaining = (streamsize)size;
	for (int i = 0; i < (bootsector.get_fatSize() >> 2); i++) {
		if (fat[i] != -2) fat[i] = -1;

		streamsize toWrite = min(remaining, (streamsize)bootsector.get_sectSize());
		file.write(cluster, toWrite);
		remaining -= toWrite;
	}

	//Записываем суперблок и FAT в файл ФС
	if (file.is_open())
	{
		file.seekp(0);
		file.write((char*)&bootsector, sizeof(BootSector));
		file.write((char*)&fsinfo, sizeof(FSInfo));

		file.seekp(bootsector.get_sectSize());
		for (int i = 0; i < (bootsector.get_fatSize() >> 2); i++)
		{
			file.write((char*)&fat[i], sizeof(uint32_t));
		}
	}
	else
	{
		std::cout << "Не удалось создать файл\n" << endl;
	}

	file.close();
}

void FileSys::open_filesys(string name)
{
	file_name = name + ".fsys";

	ifstream file;
	file.open(name + ".fsys", std::ios::binary);
	
	if (file.is_open())
	{
		file.read((char*)&bootsector, sizeof(BootSector));
		file.read((char*)&fsinfo, sizeof(FSInfo));
		size = bootsector.get_sectTotal() * bootsector.get_sectSize();
		fat.set_size(bootsector.get_fatSize() / 4);

		file.seekg(bootsector.get_sectSize());
		for (int i = 0; i < (bootsector.get_fatSize() / 4); i++)
			file.read((char*)&fat[i], sizeof(4));
	}
	else
	{
		cout << "Не удалось прочитать файл\n" << endl;
	}

	file.close();
}

void FileSys::write(char* source, int offset, int size)
{
	std::fstream stream;

	stream.open(file_name, std::ios::binary | std::ios::out | std::ios::in);

	if (!stream.is_open())
	{
		cout << "Не удалось открыть файл ФС для записи.\n" << endl;
	}

	stream.seekp(offset, std::ios::beg);
	stream.write(source, size);

	stream.close();
}

void FileSys::read(char* output, int offset, int size)
{
	std::ifstream stream;
	stream.open(file_name, std::ios::binary);

	if (!stream.is_open())
	{
		cout << "Не удалось открыть файл ФС для чтения.\n" << endl;
	}

	stream.seekg(offset, std::ios::beg);
	stream.read(output, size);

	stream.close();
}

//Работа с ФС
void FileSys::mk_file(string new_name, string new_text, uint8_t new_atr, uint8_t new_rwx, bool isAdd_rec)
{
	File info_file;
	int index = -1, cluster[512];

	for (int i = 0; i < bootsector.get_sectSize(); i += sizeof(File))
	{
		read((char*)&info_file, fsinfo.get_rootDirCl() * bootsector.get_sectSize() + i, sizeof(File));

		if ((info_file.get_name()[0] == -52 || info_file.get_name()[0] == ' ') && isAdd_rec == false)
		{
			index = i;
			break;
		}

		if ((strcmp(info_file.get_name(), new_name.c_str()) == 0) && isAdd_rec == true)
		{
			if (info_file.isWriteable(active_user.get_uid()) == false)
			{
				std::cout << "\033[31mФайл " << info_file.get_name() << " не доступен к изменению.\033[0m\n";
				return;
			}

			index = i;
			break;
		}
		else if ((strcmp(info_file.get_name(), new_name.c_str()) == 0) && isAdd_rec == false)
		{
			std::cout << "\033[31mФайл " << new_name << " уже существует.\033[0m\n";
			return;
		}
	}
	
	if (index == -1)
	{
		std::cout << "\033[31mФайл " << new_name << " не найден.\033[0m\n";
		return;
	}

	int size_for_rec, capacity_text, size;
	vector<int> clasters;
	
	string substring, read_text, write_text;

	if (isAdd_rec == false)
	{
		size_for_rec = strlen(new_text.c_str());
		clasters = fat.get_free_clasters((bootsector.get_fatSize() >> 2), (size_for_rec / bootsector.get_sectSize() + 1));

		if (clasters.size() == 0)
		{
			std::cout << "\033[31mДля сохранения файла недостаточно места! Файл не сохранён.\033[0m\n";
			return;
		}

		//Записываем файл
		string substring;
		capacity_text = size_for_rec;
		size = strlen(new_text.c_str());
		write_text = new_text;

		info_file = File((char*)new_name.c_str(), (uint8_t)new_atr, time(nullptr), time(nullptr), (uint8_t)active_user.get_uid(), (uint8_t)new_rwx, (uint16_t)clasters[0], (uint32_t)size);
	}
	else
	{
		//Ищем индекс последнего кластера
		int index_lastCl = info_file.get_cl_first(), index_temp;
		int last_size = info_file.get_size();

		while (last_size > bootsector.get_sectSize())
			last_size -= bootsector.get_sectSize();

		do
		{
			index_temp = fat[index_lastCl];
			if (index_temp == -3)
				break;

			index_lastCl = index_temp;
		} while (true);

		//Формируем дозаписываемый текст
		read_text.resize(last_size);
		read(&read_text[0], bootsector.get_sectSize() * index_lastCl, last_size);
		write_text = read_text + new_text;

		size_for_rec = strlen(write_text.c_str());

		clasters = fat.get_free_clasters((bootsector.get_fatSize() >> 2), (size_for_rec / bootsector.get_sectSize() + 1) - 1);
		clasters.push_back(index_lastCl);
		std::rotate(clasters.rbegin(), clasters.rbegin() + 1, clasters.rend());

		if (clasters.size() == 0)
		{
			std::cout << "\033[31mДля сохранения файла недостаточно места! Файл не сохранён.\033[0m\n";
			return;
		}

		capacity_text = size_for_rec;
		size = strlen(write_text.c_str());

		info_file.set_time_opn(time(nullptr));
		info_file.set_size(info_file.get_size() + strlen(new_text.c_str()));
	}

	//Записываем файл
	if (clasters.size() > 1)
	{
		for (int i = 0; i < clasters.size(); i++)
		{
			if (capacity_text >= bootsector.get_sectSize())
			{
				substring = write_text.substr(bootsector.get_sectSize() * i, bootsector.get_sectSize());
				write((char*)substring.c_str(), bootsector.get_sectSize() * clasters[i], bootsector.get_sectSize());
				capacity_text -= bootsector.get_sectSize();

				fat[clasters[i]] = clasters[i + 1];
			}
			else
			{
				substring = write_text.substr(bootsector.get_sectSize() * i, capacity_text);
				write((char*)substring.c_str(), bootsector.get_sectSize() * clasters[i], sizeof(cluster));
			}
		}

		fat[clasters[clasters.size() - 1]] = -3;
	}
	else
	{
		fat[clasters[0]] = -3;
		write((char*)write_text.c_str(), bootsector.get_sectSize() * clasters[0], strlen(write_text.c_str()));
	}

	write((char*)&info_file, fsinfo.get_rootDirCl() * bootsector.get_sectSize() + index, sizeof(File));

	//Обновляем и сохраняем FAT-таблицу
	fstream FS;
	FS.open(file_name, std::ios::binary | std::ios::out | std::ios::in);
	FS.seekp(bootsector.get_sectSize() * 1, std::ios::beg);
	for (int i = 0; i < (bootsector.get_fatSize() >> 2); i++)
	{
		FS.write((char*)&fat[i], sizeof(int));
	}
	FS.close();

	std::cout << "\033[32mФайл " << info_file.get_name() << " успешно сохранён.\033[0m\n";
}

void FileSys::mv_file(string old_name, string new_name)
{
	//Получаем и сохраняем данные FAT-таблицы
	ifstream FS;
	FS.open(file_name, std::ios::binary);
	FS.seekg(bootsector.get_sectSize());
	for (int i = 0; i < (bootsector.get_fatSize() / 4); i++)
		FS.read((char*)&fat[i], sizeof(int));
	FS.close();

	File info_file;

	//Проверяем на существование файла с новым именем
	for (int i = 0; i < bootsector.get_sectSize(); i += sizeof(File))
	{
		read((char*)&info_file, fsinfo.get_rootDirCl() * bootsector.get_sectSize() + i, sizeof(File));

		if ((strcmp(info_file.get_name(), new_name.c_str()) == 0))
		{
			cout << "\033[31mФайл " << new_name << " уже существует.\033[0m\n";
			return;
		}
	}

	//Ищем файл со старым именем
	for (int i = 0; i < bootsector.get_sectSize(); i += sizeof(File))
	{
		read((char*)&info_file, fsinfo.get_rootDirCl() * bootsector.get_sectSize() + i, sizeof(File));

		if ((strcmp(info_file.get_name(), old_name.c_str()) == 0))
		{
			info_file.set_name((char*)new_name.c_str());
			write((char*)&info_file, fsinfo.get_rootDirCl() * bootsector.get_sectSize() + i, sizeof(File));
			cout << "\033[32mФайл " << old_name << " переименован в " << new_name << " успешно!\033[0m" << endl;
			return;
		}

	}

	//Выводим сообщение об ошибке
	cout << "\033[31mФайл " << old_name << " не найден!\033[0m" << endl;

}

void FileSys::cp_file(string old_name, string new_name)
{
	File info_file;
	string text;

	//Ищем файл со старым именем
	for (int i = 0; i < bootsector.get_sectSize(); i += sizeof(File))
	{
		read((char*)&info_file, fsinfo.get_rootDirCl() * bootsector.get_sectSize() + i, sizeof(File));

		if ((strcmp(info_file.get_name(), old_name.c_str()) == 0))
		{
			text.resize(info_file.get_size());
			int index = info_file.get_cl_first();
			int last_size = info_file.get_size();

			while (last_size > bootsector.get_sectSize())
				last_size -= bootsector.get_sectSize();

			for (int j = 0; ; j += bootsector.get_sectSize())
			{
				if (fat[index] == -3)
				{
					read(&text[j], bootsector.get_sectSize() * index, last_size);
					break;
				}
				else
				{
					read(&text[j], bootsector.get_sectSize() * index, bootsector.get_sectSize());
					index = fat[index];

					if (index == -3)
						index = info_file.get_cl_first();
				}
			}

			mk_file(new_name, text, info_file.get_atr(), info_file.get_usr_rwx(), false);
			return;
		}
	}

	cout << "\033[31mФайл " << old_name << " не найден.\033[0m\n";
}

string FileSys::rd_file(string name)
{
	//Получаем и сохраняем данные FAT-таблицы
	ifstream FS;
	FS.open(file_name, std::ios::binary);
	FS.seekg(bootsector.get_sectSize());
	for (int i = 0; i < (bootsector.get_fatSize() / 4); i++)
		FS.read((char*)&fat[i], sizeof(int));
	FS.close();

	File info_file;
	string text;

	//Ищем файл с таким именем
	for (int i = 0; i < bootsector.get_sectSize(); i += sizeof(File))
	{
		read((char*)&info_file, fsinfo.get_rootDirCl() * bootsector.get_sectSize() + i, sizeof(File));

		if ((strcmp(info_file.get_name(), name.c_str()) == 0))
		{
			if (!(info_file.isAtrSystem() == true && active_user.get_uid() == 0))
			{
				if (info_file.isReadable(active_user.get_uid()) == false)
				{
					std::cout << "\033[31mФайл " << name << " не доступен к чтению.\033[0m\n";
					return "";
				}
			}
			text.resize(info_file.get_size());
			int index = info_file.get_cl_first();
			int last_size = info_file.get_size();

			while (last_size > bootsector.get_sectSize())
				last_size -= bootsector.get_sectSize();

			for (int j = 0; ; j += bootsector.get_sectSize())
			{
				if (fat[index] == -3)
				{
					read(&text[j], bootsector.get_sectSize() * index, last_size);
					break;
				}
				else
				{
					read(&text[j], bootsector.get_sectSize() * index, bootsector.get_sectSize());
					index = fat[index];

					if (index == -3)
						index = info_file.get_cl_first();
				}
			}

			info_file.set_time_opn(time(nullptr));
			write((char*)&info_file, fsinfo.get_rootDirCl() * bootsector.get_sectSize() + i, sizeof(File));

			return text;
		}
	}

	std::cout << "\033[31mФайл " << name << " не найден.\033[0m\n";
	return "";
}

void FileSys::rm_file(string name)
{
	//Получаем и сохраняем данные FAT-таблицы
	fstream FS;
	FS.open(file_name, std::ios::binary);
	FS.seekg(bootsector.get_sectSize());
	for (int i = 0; i < (bootsector.get_fatSize() / 4); i++)
		FS.read((char*)&fat[i], sizeof(int));
	FS.close();

	File info_file;
	int empty_info_file[64];

	//Ищем файл с таким именем
	for (int i = 0; i < bootsector.get_sectSize(); i += sizeof(File))
	{
		read((char*)&info_file, fsinfo.get_rootDirCl() * bootsector.get_sectSize() + i, sizeof(File));

		if ((strcmp(info_file.get_name(), name.c_str()) == 0))
		{
			if (active_user.get_uid() != 0)
			{
				if (info_file.isWriteable(active_user.get_uid()) == false)
				{
					std::cout << "\033[31mФайл " << name << " не доступен к удалению.\033[0m\n";
					return;
				}
			}

			if (info_file.isAtrSystem() == true)
			{
				std::cout << "\033[31mФайл " << name << " не доступен к удалению.\033[0m\n";
				return;
			}

			write((char*)&empty_info_file, fsinfo.get_rootDirCl() * bootsector.get_sectSize() + i, sizeof(File));

			int index = info_file.get_cl_first(), index_temp;
			do
			{
				index_temp = fat[index];
				fat[index] = -1;
				fsinfo.set_numFreeCl(fsinfo.get_numFreeCl() + 1);
				index = index_temp;

				if (index_temp == -3)
					break;
			} 
			while (true);

			//Указываем обновление в FAT-таблице
			write((char*)&fsinfo, sizeof(bootsector), sizeof(FSInfo));
			FS.open(file_name, std::ios::binary | std::ios::out | std::ios::in);
			FS.seekp(bootsector.get_sectSize() * 1, std::ios::beg);
			for (int i = 0; i < (bootsector.get_fatSize() / 4); i++)
			{
				FS.write((char*)&fat[i], sizeof(int));
			}
			FS.close();

			cout << "\033[32mФайл " << name << " успешно удалён.\033[0m" << endl;
			return;
		}
	}

	//Выводим сообщение об ошибке
	cout << "\033[31mФайл " << name << " не найден!\033[0m" << endl;
}

void FileSys::files()
{
	ifstream FS;
	FS.open(file_name, std::ios::binary);
	FS.seekg(bootsector.get_sectSize());
	for (int i = 0; i < (bootsector.get_fatSize() / 4); i++)
		FS.read((char*)&fat[i], sizeof(int));
	FS.close();

	File info_file;
	for (int i = 0; i < bootsector.get_sectSize(); i += sizeof(File))
	{
		read((char*)&info_file, fsinfo.get_rootDirCl() * bootsector.get_sectSize() + i, sizeof(File));

		if (info_file.get_name()[0] != -52 && info_file.get_name()[0] != ' ')
		{
			if (info_file.isAtrHiden() == false) cout << info_file;
		}
	}
}

void FileSys::ap_file(string name, uint8_t rwx)
{
	//Получаем и сохраняем данные FAT-таблицы
	ifstream FS;
	FS.open(file_name, std::ios::binary);
	FS.seekg(bootsector.get_sectSize());
	for (int i = 0; i < (bootsector.get_fatSize() / 4); i++)
		FS.read((char*)&fat[i], sizeof(int));
	FS.close();

	File info_file;

	//Проверяем ограничение
	if (rwx > (uint8_t)63 || rwx < (uint8_t)0)
	{
		std::cout << "\033[31mЧисло для установки прав должно быть в диапазоне 0-63.\033[0m\n";
		return;
	}

	//Ищем файл с таким именем
	for (int i = 0; i < bootsector.get_sectSize(); i += sizeof(File))
	{
		read((char*)&info_file, fsinfo.get_rootDirCl() * bootsector.get_sectSize() + i, sizeof(File));

		if ((strcmp(info_file.get_name(), name.c_str()) == 0))
		{
			if (info_file.isReadable(active_user.get_uid()) == false)
			{
				std::cout << "\033[31mФайл " << name << " не доступен к изменению.\033[0m\n";
				return;
			}

			info_file.set_usr_rwx(rwx);
			info_file.set_time_opn(time(nullptr));
			write((char*)&info_file, fsinfo.get_rootDirCl() * bootsector.get_sectSize() + i, sizeof(File));

			return;
		}
	}

	std::cout << "\033[31mФайл " << name << " не найден.\033[0m\n";
	return;
}

void FileSys::users_list(const std::vector<Users>& users)
{
	for (auto user : users) {
		if (user.get_uid() == active_user.get_uid()) cout << "\033[32m";
		cout << user.get_uid() << "\t" << user.get_login() << endl;
		if (user.get_uid() == active_user.get_uid()) cout << "\033[0m";
	}
}

void FileSys::users_switch(const std::vector<Users>& users, string login, string password)
{
	Users temp;
	temp.set_uid(active_user.get_uid());
	temp.set_login((char*)login.c_str());
	temp.set_password((char*)sha256(password).c_str());

	active_user.set_uid((uint8_t)0);

	for (auto user : users) {
		if (user == temp) 
		{
			active_user = user;
			return;
		}
	}

	active_user.set_uid(temp.get_uid());

	std::cout << "\033[31mОшибка входа! Проверьте правильность ввода логина или пароля.\033[0m\n";
	return;
}