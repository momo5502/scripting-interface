#pragma once

#include <list>
#include <mutex>
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

		using handler_complex_func = std::function<python_object(python_interface&, const python_object& args,
		                                                         const python_object& kwargs)>;
		using handler_reduced_func = std::function<python_object
			(const python_object& args, const python_object& kwargs)>;
		using handler_args_func = std::function<python_object(const python_object& args)>;
		using handler_slim_func = std::function<python_object()>;
		using handler_min_func = std::function<void()>;

		struct function_entry
		{
			std::string name{};
			PyMethodDef method_def{};
			handler_complex_func handler{};
		};

		functions& get_function_interface() const;

		python_interpreter_lock acquire_gil() const
		{
			return python_interpreter_lock{*this};
		}

		python_object create_function(handler_complex_func callback, std::string_view name = {});

		python_object create_byte_array(const void* data, size_t length);
		python_object create_dict();
		python_object create_list();
		python_object create_tuple(const std::vector<python_object>& values);

		python_object get_globals();

		python_object get_none()
		{
			return {*this, nullptr};
		}

		python_object execute(const std::string& code, const python_object& globals);

		python_object execute(const std::string& code)
		{
			return this->execute(code, this->get_globals());
		}

		bool is_exception_pending() const;

		void check_error();
		void throw_exception(const std::string& message) const;

		python_object create_function(handler_reduced_func callback, const std::string_view name = {})
		{
			return this->create_function(
				[c = std::move(callback)](python_interface& args, const python_object& kwargs, const python_object&)
				{
					return c(args, kwargs);
				}, name);
		}

		python_object create_function(handler_args_func callback, const std::string_view name = {})
		{
			return this->create_function(
				[c = std::move(callback)](python_interface&, const python_object& args, const python_object&)
				{
					return c(args);
				}, name);
		}

		python_object create_function(handler_slim_func callback, const std::string_view name = {})
		{
			return this->create_function(
				[c = std::move(callback)](python_interface&, const python_object&, const python_object&)
				{
					return c();
				}, name);
		}

		python_object create_function(handler_min_func callback, const std::string_view name = {})
		{
			return this->create_function(
				[c = std::move(callback)](python_interface& js, const python_object&, const python_object&)
				{
					c();
					return js.get_none();
				}, name);
		}

		static python_interface& get();

	private:
		std::unique_ptr<functions> functions_{};

		std::mutex mutex_{};
		std::list<std::unique_ptr<function_entry>> function_entries_{};

		python_interface(std::unique_ptr<functions> functions);
		~python_interface();
	};
}
