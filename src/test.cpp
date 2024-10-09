#include "library.hpp"

#include <cstdio>

int main()
{
	const auto libs = momo::get_loaded_libraries();

	for(auto& l : libs)
	{
		puts("Library!");
	}
}
