#pragma once

#include <momo/python_interface.hpp>

namespace momo::python
{
	python_interface& get_active_interface();

	class scoped_interface
	{
	public:
		scoped_interface(python_interface& js);
		~scoped_interface();

		scoped_interface(scoped_interface&&) = delete;
		scoped_interface(const scoped_interface&) = delete;
		scoped_interface& operator=(scoped_interface&&) = delete;
		scoped_interface& operator=(const scoped_interface&) = delete;

	private:
		python_interface* previous_interface_{};
	};
}
