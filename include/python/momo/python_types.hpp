#pragma once

#include <cstddef>

namespace momo::python
{
#define METH_VARARGS 0x0001
#define METH_KEYWORDS 0x0002

#define Py_file_input 257
#define Py_eval_input 258


	using Py_ssize_t = ptrdiff_t;

	struct PyObject;
	struct PyThreadState;

	using PyGILState_STATE = struct PyGILState_STATE__*;

	struct PyMethodDef
	{
		const char* ml_name; /* The name of the built-in function/method */
		void* ml_meth;       /* The C function that implements it */
		int ml_flags;        /* Combination of METH_xxx flags, which mostly
								describe the args expected by the C func */
		const char* ml_doc;  /* The __doc__ attribute, or NULL */
	};
}
