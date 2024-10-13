#include "momo/python_exception.hpp"

#include <momo/python_interface.hpp>

#include "python_functions.hpp"

namespace momo::python
{
	namespace
	{
		void normalize_exception(python_interface& py, python_object& type, python_object& value, python_object& trace)
		{
			auto* type_ref = type.get_new_ref();
			auto* value_ref = value.get_new_ref();
			auto* trace_ref = trace.get_new_ref();

			py.get_function_interface().err_normalize_exception(&type_ref, &value_ref, &trace_ref);

			type = {py, type_ref};
			value = {py, value_ref};
			trace = {py, trace_ref};
		}
	}


	python_exception::python_exception(python_interface& py)
		: py_(&py)
	{
		const auto lock = this->py_->acquire_gil();
		const auto& f = this->py_->get_function_interface();

		PyObject* type{};
		PyObject* value{};
		PyObject* trace{};

		f.err_fetch(&type, &value, &trace);

		this->type_ = {py, type};
		this->value_ = {py, value};
		this->trace_ = {py, trace};

		f.err_clear();

		normalize_exception(py, this->type_, this->value_, this->trace_);

		this->what_ = this->value_.as<std::string>();
	}

	void python_exception::restore() const
	{
		const auto lock = this->py_->acquire_gil();
		this->py_->get_function_interface().err_restore(this->type_.get_new_ref(),
		                                                this->value_.get_new_ref(),
		                                                this->trace_.get_new_ref());
	}
}
