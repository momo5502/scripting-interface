#include "scoped_interface.hpp"

#include <stdexcept>

namespace momo::python
{
	namespace
	{
		thread_local python_interface* active_interface{nullptr};
	}

	python_interface& get_active_interface()
	{
		const auto current_interface = active_interface;
		if (current_interface)
		{
			return *current_interface;
		}

		throw std::runtime_error("No active Python interface");
	}

	scoped_interface::scoped_interface(python_interface& py)
		: previous_interface_(active_interface)
	{
		active_interface = &py;
	}

	scoped_interface::~scoped_interface()
	{
		active_interface = this->previous_interface_;
	}
}
