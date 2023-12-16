#pragma once
#include <cstdint>

class BootSector
{
	char nameFS[5]{ "KSys" };			//5 ����
	uint32_t sectTotal;					//4 �����
	uint16_t sectTotalCl;				//2 �����
	uint16_t sectSize;					//2 �����
	uint8_t	 fatTotal;					//1 ����
	uint16_t fatSize;					//2 �����

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
		sectSize = 512;					//������ ������� - 512 ����
		sectTotal = size / sectSize;	//������������� ���-�� �������� �����
		sectTotalCl = 1;				//���-�� �������� �� �������
		fatTotal = (uint8_t)1;			//���-�� FAT-������
		fatSize = sectTotal * 4;		//������������� ������ FAT-�������
	};

	friend std::ostream& operator<<(std::ostream& stream, const BootSector& bootsector)
	{
		stream << "��� ��:              " << bootsector.nameFS << "\n";
		stream << "������ FAT-�������:  " << bootsector.fatSize << "\n";
		stream << "��������:            " << bootsector.sectTotal << "\n";
		stream << "��������� �� ������: " << bootsector.sectTotalCl << "\n";
		stream << "������ �������:      " << bootsector.sectSize << "\n";
		return stream;
	}
};