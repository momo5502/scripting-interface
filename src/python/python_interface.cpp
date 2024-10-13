#include <momo/python_interface.hpp>

#include "python_library.hpp"
#include "python_functions.hpp"

#include "scoped_interface.hpp"
#include "momo/python_object.hpp"
#include "momo/python_object.hpp"

namespace momo::python
{
	namespace
	{
		std::unique_ptr<functions> get_functions()
		{
			const auto library = get_python_library();
			if (library)
			{
				return std::make_unique<functions>(library);
			}

			throw std::runtime_error("No python library found!");
		}

		template <typename... Args>
		PyObject* build_value(python_interface& py, Args... args)
		{
			auto& f = py.get_function_interface();
			python_interpreter_lock lock{f};
			return f.build_value(args...);
		}

		PyObject* native_function_handler(PyObjectBorrowed* self, PyObjectBorrowed* args, PyObjectBorrowed* kwargs)
		{
			auto& py = python_interface::get();
			scoped_interface _{py};

			const python_object self_obj{py, self};
			const python_object args_obj{py, args};
			const python_object kwargs_obj{py, kwargs};

			auto* ptr = self_obj.as<void*>();
			const auto& entry = *static_cast<python_interface::function_entry*>(ptr);

			//try
			//{
				const auto returnValue = entry.handler(py, args_obj, kwargs_obj);
				py.check_error();

				return returnValue.get_new_ref();
			//}
			/*catch (const PythonException& e)
			{
				e.Restore();
			}
			catch (const std::exception& e)
			{
				pythonInterface.SetException(e.what());
			}*/
		}
	}

	python_interface& python_interface::get()
	{
		static python_interface py{get_functions()};
		return py;
	}

	python_interface::python_interface(std::unique_ptr<functions> functions)
		: functions_(std::move(functions))
	{
	}

	python_interface::~python_interface() = default;

	functions& python_interface::get_function_interface() const
	{
		return *this->functions_;
	}

	python_object python_interface::create_function(handler_complex_func callback, const std::string_view name)
	{
		auto container = std::make_unique<function_entry>();
		container->name = name;
		container->handler = std::move(callback);
		container->method_def.ml_name = container->name.c_str();
		container->method_def.ml_meth = reinterpret_cast<void*>(&native_function_handler);
		container->method_def.ml_flags = METH_VARARGS | METH_KEYWORDS;
		container->method_def.ml_doc = nullptr;

		const auto lock = this->acquire_gil();

		const python_object ptr_obj{*this, static_cast<void*>(container.get())};
		auto* function_obj = this->get_function_interface().c_function_new_ex(
			&container->method_def, reinterpret_cast<PyObject*>(ptr_obj.borrow()), nullptr);

		python_object result{*this, function_obj};

		{
			std::lock_guard _{this->mutex_}; // Probably unnecessary because of GIL?
			this->function_entries_.push_back(std::move(container));
		}

		return result;
	}

	python_object python_interface::create_byte_array(const void* data, const size_t length)
	{
		const auto lock = this->acquire_gil();
		auto* obj = this->get_function_interface().byte_array_from_string_and_size(
			static_cast<const char*>(data), static_cast<long long>(length));

		return {*this, obj};
	}

	python_object python_interface::create_dict()
	{
		return python_object{*this, build_value(*this, "{}")};
	}

	python_object python_interface::create_list()
	{
		return python_object{*this, build_value(*this, "[]")};
	}

	python_object python_interface::create_tuple(const std::vector<python_object>& values)
	{
		const auto lock = this->acquire_gil();

		auto* obj = this->get_function_interface().tuple_new(static_cast<long long>(values.size()));

		python_object tuple{*this, obj};

		for (size_t i = 0; i < values.size(); ++i)
		{
			tuple.set_element(i, values[i]);
		}

		return tuple;
	}

	python_object python_interface::get_globals()
	{
		const auto lock = this->acquire_gil();
		auto* obj = this->get_function_interface().get_globals();
		return {*this, obj};
	}

	python_object python_interface::execute(const std::string& code, const python_object& globals)
	{
		const auto lock = this->acquire_gil();

		auto* obj = this->get_function_interface().run_string(code.c_str(), Py_file_input,
		                                                      reinterpret_cast<PyObject*>(globals.borrow()),
		                                                      nullptr);
		python_object result{*this, obj};

		this->check_error();

		return result;
	}

	void python_interface::check_error()
	{
		// TODO
	}
}
