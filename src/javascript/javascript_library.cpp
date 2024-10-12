#include "javascript_library.hpp"

namespace momo::javascript
{
	namespace
	{
		bool is_javascript_library(const library& lib)
		{
			return lib && lib.get_symbol("napi_module_register") != nullptr;
		}
	}

	std::vector<library> get_javascript_libraries()
	{
		std::vector<library> libraries{};

		auto modules = get_loaded_libraries();

		for (auto& library : modules)
		{
			if (is_javascript_library(library))
			{
				libraries.emplace_back(std::move(library));
			}
		}

		return libraries;
	}

	library get_javascript_library()
	{
		auto libraries = get_javascript_libraries();
		if (libraries.empty())
		{
			return {};
		}

		return std::move(libraries.front());
	}
}
