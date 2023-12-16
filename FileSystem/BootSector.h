#pragma once
#include <cstdint>

class BootSector
{
	char nameFS[5]{ "KSys" };			//5 байт
	uint32_t sectTotal;					//4 байта
	uint16_t sectTotalCl;				//2 байта
	uint16_t sectSize;					//2 байта
	uint8_t	 fatTotal;					//1 байт
	uint16_t fatSize;					//2 байта

public:
	BootSector() { };
	
	char	 get_nameFS()		{ return *nameFS; };
	uint32_t get_sectTotal()	{ return sectTotal; };
	uint16_t get_sectTotalCl()	{ return sectTotalCl; };
	uint16_t get_sectSize()		{ return sectSize; };
	uint8_t  get_fatTotal()		{ return fatTotal; };
	uint16_t get_fatSize()		{ return fatSize; };

	void set_size(int size)
	{
		sectSize = 512;					//Размер сектора - 512 байт
		sectTotal = size / sectSize;	//Устанавливаем кол-во секторов всего
		sectTotalCl = 1;				//Кол-во секторов на кластер
		fatTotal = (uint8_t)1;			//Кол-во FAT-таблиц
		fatSize = sectTotal * 4;		//Устанавливаем размер FAT-таблицы
	};

	friend std::ostream& operator<<(std::ostream& stream, const BootSector& bootsector)
	{
		stream << "Имя ФС:              " << bootsector.nameFS << "\n";
		stream << "Размер FAT-таблицы:  " << bootsector.fatSize << "\n";
		stream << "Секторов:            " << bootsector.sectTotal << "\n";
		stream << "Кластеров на сектор: " << bootsector.sectTotalCl << "\n";
		stream << "Размер сектора:      " << bootsector.sectSize << "\n";
		return stream;
	}
};