#pragma once
#include <iostream>
#include <ctime>
#include <string>

class File
{
	char name[39];			//39 байт
	uint8_t atr;			//1 байт
	time_t time_crt;		//8 байт
	time_t time_opn;		//8 байт
	uint8_t usr_id;			//1 байт
	uint8_t usr_rwx;		//1 байт
	uint16_t cl_first;		//2 байт
	uint32_t size;			//4 байт

public:
	File() { } ;

	File(char* s_name, uint8_t s_atr, time_t s_t1, time_t s_t2, uint8_t s_uid, uint8_t s_rwx, uint16_t s_cl, uint32_t s_size)
	{
		strcpy_s(name, s_name);
		atr = s_atr;
		time_crt = s_t1;
		time_opn = s_t2;
		usr_id = s_uid;
		usr_rwx = s_rwx;
		cl_first = s_cl;
		size = s_size;
	};

	//set
	void		set_name(char* n) { strcpy_s(name, n); };
	void		set_atr(uint8_t a) { atr = a; };
	void		set_usr_rwx(uint8_t a) { usr_rwx = a; };
	void		set_time_opn(time_t t) { time_opn = t; };
	void		set_size(uint32_t s) { size = s; };
	void		set_cl_first(uint16_t cl) { cl_first = cl; };

	//get
	const char*	get_name() { return name; };
	uint8_t		get_atr() { return atr; };
	uint8_t		get_usr_id() { return usr_id; };
	uint8_t		get_usr_rwx() { return usr_rwx; };
	uint16_t	get_cl_first() { return cl_first; };
	uint32_t	get_size() { return size; };

	//проверки
	bool isReadable(uint8_t g_usr_id)
	{
		if (usr_id == g_usr_id)
		{
			if ((usr_rwx & 0b100000) == 0)
				return false;
			else
				return true;
		}
		else
		{
			if ((usr_rwx & 0b000100) == 0)
				return false;
			else
				return true;
		}
	}

	bool isWriteable(uint8_t g_usr_id)
	{
		if (usr_id == g_usr_id)
		{
			if ((usr_rwx & 0b010000) == 0)
				return false;
			else
				return true;
		}
		else
		{
			if ((usr_rwx & 0b000010) == 0)
				return false;
			else
				return true;
		}
	}

	bool isExecutable(uint8_t g_usr_id)
	{
		if (usr_id == g_usr_id)
		{
			if ((usr_rwx & 0b001000) == 0)
				return false;
			else
				return true;
		}
		else
		{
			if ((usr_rwx & 0b000001) == 0)
				return false;
			else
				return true;
		}
	}

	bool isAtrSystem()
	{
		if ((atr & 0b010) == 0)
			return false;
		else
			return true;
	}
	
	bool isAtrHiden()
	{
		if ((atr & 0b001) == 0)
			return false;
		else
			return true;
	}

	//операторы
	friend std::ostream& operator<<(std::ostream& stream, File& file)
	{
		printf("%-11s", file.name);
		
		//дата и время
		char crt[80], opn[80];
		struct tm timeInfo;

		localtime_s(&timeInfo, &file.time_crt);
		strftime(crt, sizeof(crt), "%d/%m/%Y %H:%M:%S", &timeInfo);
		localtime_s(&timeInfo, &file.time_opn);
		strftime(opn, sizeof(opn), "%d/%m/%Y %H:%M:%S", &timeInfo);

		//атрибуты
		char atributes[4];
		if ((file.atr & 0b001) == 0) { atributes[0] = '-'; }
		else { atributes[0] = 'H'; }

		if ((file.atr & 0b010) == 0) { atributes[1] = '-'; }
		else { atributes[1] = 'S'; }

		if ((file.atr & 0b100) == 0) { atributes[2] = '-'; }
		else { atributes[2] = 'A'; }

		atributes[3] = '\0';

		//RWX
		char flags[7];
		if ((file.usr_rwx & 0b101000) == 0) { flags[0] = '-'; }
		else { flags[0] = 'r'; }

		if ((file.usr_rwx & 0b010000) == 0) { flags[1] = '-'; }
		else { flags[1] = 'w'; }

		if ((file.usr_rwx & 0b001000) == 0) { flags[2] = '-'; }
		else { flags[2] = 'x'; }

		if ((file.usr_rwx & 0b000100) == 0) { flags[3] = '-'; }
		else { flags[3] = 'r'; }

		if ((file.usr_rwx & 0b000010) == 0) { flags[4] = '-'; }
		else { flags[4] = 'w'; }

		if ((file.usr_rwx & 0b000001) == 0) { flags[5] = '-'; }
		else { flags[5] = 'x'; }

		flags[6] = '\0';

		stream << "\t" << (uint32_t)file.usr_id << "\t" << crt << "\t" << opn << "\t" << file.size << "\t" << atributes << "\t" << flags << "\t\n";
		return stream;
	}
};