#include "FAT.h"
#include <fstream>

int FAT::get_free_claster(int size)
{
	for (int i = 0; i < size; i++)
		if (fat[i] == -1) { return i; }

	return -1;
}

std::vector<int> FAT::get_free_clasters(int size, int count)
{
    std::vector<int> free_clasters_index;

    for (int i = 0; i < size; i++) 
    {
        if (fat[i] == -1) 
        {
            free_clasters_index.push_back(i);
        }

        if (free_clasters_index.size() == count)
            break;
    }

    return free_clasters_index;
}
