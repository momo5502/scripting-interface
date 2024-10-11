#include "scoped_interface.hpp"

#include <stdexcept>

namespace momo::javascript
{
	namespace
	{
		thread_local javascript_interface* active_interface{nullptr};
	}

	javascript_interface& get_active_interface()
	{
		const auto current_interface = active_interface;
		if (current_interface)
		{
			return *current_interface;
		}

		throw std::runtime_error("No active JavaScript interface");
	}

	scoped_interface::scoped_interface(javascript_interface& js)
		: previous_interface_(active_interface)
	{
		active_interface = &js;
	}

	scoped_interface::~scoped_interface()
	{
		active_interface = this->previous_interface_;
	}
}
