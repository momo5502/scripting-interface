#include <momo/python_interpreter_lock.hpp>
#include <momo/python_interface.hpp>

#include "python_functions.hpp"

namespace momo::python
{
	python_interpreter_lock::python_interpreter_lock(functions& f)
		: functions_(&f)
	{
		this->state_ = f.gil_state_ensure();
	}

	python_interpreter_lock::python_interpreter_lock(const python_interface& py)
		: python_interpreter_lock(py.get_function_interface())
	{
	}

	python_interpreter_lock::python_interpreter_lock(python_interpreter_lock&& obj) noexcept
	{
		this->operator=(std::move(obj));
	}

	python_interpreter_lock& python_interpreter_lock::operator=(python_interpreter_lock&& obj) noexcept
	{
		if (this != &obj)
		{
			this->release();

			this->state_ = obj.state_;
			this->functions_ = obj.functions_;

			obj.state_ = nullptr;
			obj.functions_ = nullptr;
		}

		return *this;
	}

	void python_interpreter_lock::release()
	{
		if (this->functions_)
		{
			this->functions_->gil_state_release(this->state_);
		}

		this->functions_ = nullptr;
		this->state_ = nullptr;
	}
}
