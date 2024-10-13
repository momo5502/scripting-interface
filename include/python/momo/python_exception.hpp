#pragma once

#include <stdexcept>
#include "python_object.hpp"

namespace momo::python
{
	class python_interface;
	class python_object;

	class python_exception : public std::exception
	{
	public:
		explicit python_exception(python_interface& py);

		void restore() const;

		const python_object& get_value()
		{
			return this->value_;
		}

		char const* what() const noexcept override
		{
			return this->what_.c_str();
		}

	private:
		python_interface* py_{};

		std::string what_{};


		python_object type_{};
		python_object value_{};
		python_object trace_{};
	};
}
