#pragma once
#include <cstdint>

class FSInfo
{
	uint16_t numFreeCl;		//2 байта
	uint16_t nxtFreeCl;		//2 байта
	uint16_t rootDirCl;		//2 байта

public:
	FSInfo() {};

	//get
	uint16_t get_numFreeCl() { return numFreeCl; };
	uint16_t get_nxtFreeCl() { return nxtFreeCl; };
	uint16_t get_rootDirCl() { return rootDirCl; };

	//set
	void set_numFreeCl(uint16_t set_numFreeCl) { numFreeCl = set_numFreeCl; };
	void set_nxtFreeCl(uint16_t set_nxtFreeCl) { nxtFreeCl = set_nxtFreeCl; };
	void set_rootDirCl(uint16_t set_rootDirCl) { rootDirCl = set_rootDirCl; };

	//операторы
	friend std::ostream& operator<<(std::ostream& stream, const FSInfo& fsinfo)
	{
		stream << "—вободных кластеров: " << fsinfo.numFreeCl << "\n";
		return stream;
	}
};