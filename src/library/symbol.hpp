#pragma once

#include "library.hpp"

#include <stdexcept>

namespace momo
{
	template <typename Type>
	class symbol
	{
	public:
		symbol(const library& lib, const char* name)
		{
			if (!lib)
			{
				throw std::runtime_error("Bad library for symbol: " + std::string(name));
			}

			this->symbol_ = lib.get_symbol<Type>(name);

			if (!this->symbol_)
			{
				throw std::runtime_error("Bad symbol: " + std::string(name));
			}
		}

		Type* get() const
		{
			return this->symbol_;
		}

		operator Type*() const
		{
			return this->get();
		}

	private:
		Type* symbol_{};
	};
}
