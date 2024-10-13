#pragma once

#include <cstdint>
#include <cstddef>

#include <symbol.hpp>

#include <momo/python_types.hpp>

namespace momo::python
{
	struct functions
	{
		symbol<int (void (*func)())> at_exit;
		symbol<void()> initialize;
		symbol<PyGILState_STATE()> gil_state_ensure;
		symbol<void(PyGILState_STATE)> gil_state_release;
		symbol<PyObject*(const char* str, int start, PyObject* globals, PyObject* locals)> run_string;
		symbol<PyObject*(PyObject* callable, PyObject* args, PyObject* kwargs)> call_object;
		symbol<PyObject*(const char* format, ...)> build_value;
		symbol<PyObject*()> err_occurred;
		symbol<void()> err_clear;
		symbol<void(PyObject** ptype, PyObject** pvalue, PyObject** ptraceback)> err_fetch;
		symbol<void(PyObject* type, PyObject* value, PyObject* traceback)> err_restore;
		symbol<void(PyObject** exc, PyObject** val, PyObject** tb)> err_normalize_exception;
		symbol<void(PyObject* o)> inc_ref;
		symbol<void(PyObject* o)> dec_ref;
		symbol<PyObject*()> get_globals;
		symbol<PyObject*(const char* string, Py_ssize_t len)> byte_array_from_string_and_size;
		symbol<PyObject*(const char* data, Py_ssize_t len)> marshal_read_object_from_string;
		symbol<PyObject*(PyObject* code, PyObject* globals)> function_new;
		symbol<PyThreadState*()> save_thread;
		symbol<void (PyThreadState* tstate)> restore_thread;
		symbol<Py_ssize_t ()> gc_collect;
		symbol<PyObject*(PyObject* co, PyObject* globals, PyObject* locals)> eval_code;
		symbol<long long (PyObject* obj)> long_as_long_long;
		symbol<long (PyObject* obj)> long_as_long;
		symbol<unsigned long (PyObject* pylong)> long_as_unsigned_long;
		symbol<unsigned long long (PyObject* pylong)> long_as_unsigned_long_long;
		symbol<PyObject*(void* p)> long_from_void_ptr;
		symbol<void*(PyObject* pylong)> long_as_void_ptr;
		symbol<PyObject*(PyMethodDef* ml, PyObject* self, PyObject* module)> c_function_new_ex;
		symbol<int (PyObject* p, const char* key, PyObject* val)> dict_set_item_string;
		symbol<int (PyObject* p, const char* key)> dict_del_item_string;
		symbol<PyObjectBorrowed*(PyObject* p, const char* key)> dict_get_item_string;
		symbol<PyObject*(PyObject* p)> dict_keys;
		symbol<PyObject*(long v)> bool_from_long;
		symbol<double (PyObject* pyfloat)> float_as_double;
		symbol<int (PyObject* o)> object_is_true;
		symbol<int (PyObject* obj, char** buffer, Py_ssize_t* length)> bytes_as_string_and_size;
		symbol<PyObject*(Py_ssize_t len)> tuple_new;
		symbol<Py_ssize_t (PyObject* p)> tuple_size;
		symbol<PyObjectBorrowed*(PyObject* p, Py_ssize_t pos)> tuple_get_item;
		symbol<int (PyObject* p, Py_ssize_t pos, PyObject* o)> tuple_set_item;
		symbol<PyObject*(Py_ssize_t len)> list_new;
		symbol<Py_ssize_t(PyObject* list)> list_size;
		symbol<PyObjectBorrowed*(PyObject* list, Py_ssize_t index)> list_get_item;
		symbol<int (PyObject* list, Py_ssize_t index, PyObject* item)> list_insert;
		symbol<PyObject*(PyObject* list)> list_as_tuple;
		symbol<int (PyObject* list, PyObject* item)> list_append;
		symbol<int (PyObject* list, Py_ssize_t index, PyObject* item)> list_set_item;
		symbol<int (PyObject* inst, PyObject* cls)> object_is_instance;
		symbol<const char*(PyObject* unicode, Py_ssize_t* size)> unicode_as_utf8_and_size;
		symbol<PyObject*(const char* str, Py_ssize_t size)> unicode_from_string_and_size;
		symbol<PyObject*(PyObject* o)> object_str;
		symbol<void (PyObject* type, const char* message)> err_set_string;
		symbol<PyObject*(PyObject* o, const char* attr_name)> object_get_attr_string;
		symbol<int (PyObject* o, const char* attr_name)> object_has_attr_string;
		symbol<int (PyObject* o, const char* attr_name, PyObject* v)> object_set_attr_string;
		symbol<PyObject*(const char* name)> import_import_module;
		symbol<PyObject*(const char* v, Py_ssize_t len)> bytes_from_string_and_size;

		symbol<PyObject> tuple_type;
		symbol<PyObject> list_type;
		symbol<PyObject> dict_type;
		symbol<PyObject> unicode_type;
		symbol<PyObject> exc_runtime_error;

		functions(const library& lib)
			: at_exit(lib, "Py_AtExit")
			  , initialize(lib, "Py_Initialize")
			  , gil_state_ensure(lib, "PyGILState_Ensure")
			  , gil_state_release(lib, "PyGILState_Release")
			  , run_string(lib, "PyRun_String")
			  , call_object(lib, "PyObject_Call")
			  , build_value(lib, "Py_BuildValue")
			  , err_occurred(lib, "PyErr_Occurred")
			  , err_clear(lib, "PyErr_Clear")
			  , err_fetch(lib, "PyErr_Fetch")
			  , err_restore(lib, "PyErr_Restore")
			  , err_normalize_exception(lib, "PyErr_NormalizeException")
			  , inc_ref(lib, "Py_IncRef")
			  , dec_ref(lib, "Py_DecRef")
			  , get_globals(lib, "PyEval_GetGlobals")
			  , byte_array_from_string_and_size(lib, "PyByteArray_FromStringAndSize")
			  , marshal_read_object_from_string(lib, "PyMarshal_ReadObjectFromString")
			  , function_new(lib, "PyFunction_New")
			  , save_thread(lib, "PyEval_SaveThread")
			  , restore_thread(lib, "PyEval_RestoreThread")
			  , gc_collect(lib, "PyGC_Collect")
			  , eval_code(lib, "PyEval_EvalCode")
			  , long_as_long_long(lib, "PyLong_AsLongLong")
			  , long_as_long(lib, "PyLong_AsLong")
			  , long_as_unsigned_long(lib, "PyLong_AsUnsignedLong")
			  , long_as_unsigned_long_long(lib, "PyLong_AsUnsignedLongLong")
			  , long_from_void_ptr(lib, "PyLong_FromVoidPtr")
			  , long_as_void_ptr(lib, "PyLong_AsVoidPtr")
			  , c_function_new_ex(lib, "PyCFunction_NewEx")
			  , dict_set_item_string(lib, "PyDict_SetItemString")
			  , dict_del_item_string(lib, "PyDict_DelItemString")
			  , dict_get_item_string(lib, "PyDict_GetItemString")
			  , dict_keys(lib, "PyDict_Keys")
			  , bool_from_long(lib, "PyBool_FromLong")
			  , float_as_double(lib, "PyFloat_AsDouble")
			  , object_is_true(lib, "PyObject_IsTrue")
			  , bytes_as_string_and_size(lib, "PyBytes_AsStringAndSize")
			  , tuple_new(lib, "PyTuple_New")
			  , tuple_size(lib, "PyTuple_Size")
			  , tuple_get_item(lib, "PyTuple_GetItem")
			  , tuple_set_item(lib, "PyTuple_SetItem")
			  , list_new(lib, "PyList_New")
			  , list_size(lib, "PyList_Size")
			  , list_get_item(lib, "PyList_GetItem")
			  , list_insert(lib, "PyList_Insert")
			  , list_as_tuple(lib, "PyList_AsTuple")
			  , list_append(lib, "PyList_Append")
			  , list_set_item(lib, "PyList_SetItem")
			  , object_is_instance(lib, "PyObject_IsInstance")
			  , unicode_as_utf8_and_size(lib, "PyUnicode_AsUTF8AndSize")
			  , unicode_from_string_and_size(lib, "PyUnicode_FromStringAndSize")
			  , object_str(lib, "PyObject_Str")
			  , err_set_string(lib, "PyErr_SetString")
			  , object_get_attr_string(lib, "PyObject_GetAttrString")
			  , object_has_attr_string(lib, "PyObject_HasAttrString")
			  , object_set_attr_string(lib, "PyObject_SetAttrString")
			  , import_import_module(lib, "PyImport_ImportModule")
			  , bytes_from_string_and_size(lib, "PyBytes_FromStringAndSize")
			  , tuple_type(lib, "PyTuple_Type")
			  , list_type(lib, "PyList_Type")
			  , dict_type(lib, "PyDict_Type")
			  , unicode_type(lib, "PyUnicode_Type")
			  , exc_runtime_error(lib, "PyExc_RuntimeError")

		{
		}
	};
}
