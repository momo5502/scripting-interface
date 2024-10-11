#pragma once

#include <momo/javascript_interface.hpp>

namespace momo::javascript
{
	javascript_interface& get_active_interface();

	class scoped_interface
	{
	public:
		scoped_interface(javascript_interface& js);
		~scoped_interface();

		scoped_interface(scoped_interface&&) = delete;
		scoped_interface(const scoped_interface&) = delete;
		scoped_interface& operator=(scoped_interface&&) = delete;
		scoped_interface& operator=(const scoped_interface&) = delete;

	private:
		javascript_interface* previous_interface_{};
	};
}
