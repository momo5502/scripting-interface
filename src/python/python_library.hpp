#pragma once

#include <library.hpp>

namespace momo::python
{
	std::vector<library> get_python_libraries();
	library get_python_library();
}
