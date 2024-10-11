#include "library.hpp"

#include <string>

#ifdef _WIN32

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <Psapi.h>

#else

#include <dlfcn.h>

#ifdef __APPLE__

#include <mach-o/dyld.h>

#else

#include <link.h>
#endif

#endif

namespace momo
{
	namespace
	{
#if defined( _WIN32)
		bool enum_process_modules(std::vector<HMODULE>& modules, size_t& needed_size)
		{
			DWORD required_size{};
			const auto size = static_cast<DWORD>(modules.size() * sizeof(HMODULE));

			const auto res = EnumProcessModules(GetCurrentProcess(), modules.data(), size, &required_size) != false;

			needed_size = required_size;
			return res;
		}

		std::vector<HMODULE> get_loaded_modules()
		{
			size_t needed_size{};

			std::vector<HMODULE> modules{};
			modules.resize(1);

			if (!enum_process_modules(modules, needed_size))
			{
				return {};
			}

			modules.resize((needed_size << 1) / sizeof(HMODULE));

			if (!enum_process_modules(modules, needed_size))
			{
				return {};
			}

			const auto count = std::min(needed_size / sizeof(HMODULE), modules.size());
			modules.resize(count);

			return modules;
		}

		std::vector<library> get_loaded_libraries_internal()
		{
			const auto modules = get_loaded_modules();

			std::vector<library> result{};
			result.reserve(modules.size());

			for (const auto& handle : modules)
			{
				result.emplace_back(handle, false);
			}

			return result;
		}
#else
		struct library_info
		{
			std::string name;
		};

		library get_library(const char* name)
		{
			auto lib = open_library(name);
			if (lib)
			{
				return lib;
			}

			return load_library(name);
		}

#ifdef __APPLE__
		std::vector<library_info> get_loaded_modules()
		{
			std::vector<library_info> libraries{};

			const auto count = _dyld_image_count();
			libraries.reserve(count);

			for (auto i = 0u; i < count; ++i)
			{
				library_info lib{};
				lib.name = _dyld_get_image_name(i);

				libraries.emplace_back(std::move(lib));
			}

			return libraries;
		}
#else
		int find_loaded_library(struct dl_phdr_info* info, size_t, void* data)
		{
			if (!info || !info->dlpi_name) {
				return 0;
			}

			auto& infos = *static_cast<std::vector<library_info>*>(data);

			library_info i{};
			i.name = info->dlpi_name;

			infos.emplace_back(std::move(i));

			return 0;
		}

		std::vector<library_info> get_loaded_modules()
		{
			std::vector<library_info> libraries{};
			dl_iterate_phdr(find_loaded_library, &libraries);

			return libraries;
		}
#endif

		std::vector<library> get_loaded_libraries_internal()
		{
			const auto modules = get_loaded_modules();

			std::vector<library> result{};
			result.reserve(modules.size());

			for (const auto& handle : modules)
			{
				auto lib = get_library(handle.name.empty() ? nullptr : handle.name.c_str());
				if (lib)
				{
					result.emplace_back(std::move(lib));
				}
			}

			return result;
		}
#endif
	}

	library open_library(const char* name)
	{
#ifdef _WIN32
		return library{GetModuleHandleA(name), false};
#else
		return library{dlopen(name, RTLD_NOW | RTLD_NOLOAD), true};
#endif
	}

	library load_library(const char* name)
	{
#ifdef _WIN32
		return library{LoadLibraryA(name), true};
#else
		return library{dlopen(name, RTLD_NOW), true};
#endif
	}

	void free_library(const native_handle handle)
	{
#ifdef _WIN32
		FreeLibrary(static_cast<HMODULE>(handle));
#else
		dlclose(handle);
#endif
	}

	void* get_library_symbol(native_handle handle, const char* symbol)
	{
#ifdef _WIN32
		return GetProcAddress(static_cast<HMODULE>(handle), symbol);
#else
		return dlsym(handle, symbol);
#endif
	}

	std::vector<library> get_loaded_libraries()
	{
		return get_loaded_libraries_internal();
	}
}
