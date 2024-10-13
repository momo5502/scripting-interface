#pragma once

#include "python_types.hpp"

namespace momo::python
{
	struct functions;
	class python_interface;

	class python_interpreter_lock
	{
	public:
		explicit python_interpreter_lock(functions& f);
		explicit python_interpreter_lock(const python_interface& py);

		~python_interpreter_lock()
		{
			this->release();
		}

		python_interpreter_lock(const python_interpreter_lock&) = delete;
		python_interpreter_lock& operator=(const python_interpreter_lock&) = delete;

		python_interpreter_lock(python_interpreter_lock&& obj) noexcept;
		python_interpreter_lock& operator=(python_interpreter_lock&& obj) noexcept;

	private:
		functions* functions_{};
		PyGILState_STATE state_{};

		void release();
	};
}
