#pragma once

#include <list>
#include <memory>
#include <functional>

#include "python_object.hpp"
#include "python_interpreter_lock.hpp"

namespace momo::python
{
	struct functions;

	class python_interface
	{
	public:
		python_interface(python_interface&&) = delete;
		python_interface(const python_interface&) = delete;
		python_interface& operator=(python_interface&&) = delete;
		python_interface& operator=(const python_interface&) = delete;

		/*using initializer_func = javascript_value(javascript_interface& js, const javascript_value& exports);
		using initializer =	std::function<initializer_func>;

		using handler_complex_func = std::function<javascript_value(javascript_interface&, const javascript_value& this_value, const std::vector<javascript_value>& args)>;
		using handler_reduced_func = std::function<javascript_value(const javascript_value& this_value, const std::vector<javascript_value>& args)>;
		using handler_args_func = std::function<javascript_value(const std::vector<javascript_value>& args)>;
		using handler_slim_func = std::function<javascript_value()>;
		using handler_min_func = std::function<void()>;*/

		struct function_entry
		{
			//handler_complex_func handler{};
		};

		functions& get_function_interface() const;

		python_interpreter_lock acquire_gil() const
		{
			return python_interpreter_lock{*this};
		}

		//python_object create_function(handler_complex_func callback, std::string_view name = {});

		python_object create_byte_array(const void* data, size_t length);
		python_object create_dict();
		python_object create_list();
		python_object create_tuple(const std::vector<python_object>& values);

		python_object get_globals();

		python_object get_none()
		{
			return { *this, nullptr };
		}

		python_object execute(const std::string_view code, const python_object& globals);

		python_object execute(const std::string_view code)
		{
			return this->execute(code, this->get_globals());
		}

		void check_error();

	private:
		std::unique_ptr<functions> functions_{};
		std::list<std::unique_ptr<function_entry>> function_entries_{};
	};
}
