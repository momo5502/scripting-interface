#include "python_library.hpp"

namespace momo::python
{
	namespace
	{
		bool is_python_library(const library& lib)
		{
			return lib && lib.get_symbol("PyObject_Call") != nullptr;
		}
	}

	std::vector<library> get_python_libraries()
	{
		std::vector<library> libraries{};

		auto modules = get_loaded_libraries();

		for (auto& library : modules)
		{
			if (is_python_library(library))
			{
				libraries.emplace_back(std::move(library));
			}
		}

		return libraries;
	}

	library get_python_library()
	{
		auto libraries = get_python_libraries();
		if (libraries.empty())
		{
			return {};
		}

		return std::move(libraries.front());
	}
}
