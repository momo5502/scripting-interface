#pragma once

#include <symbol.hpp>
#include "napi_types.hpp"

namespace momo::javascript
{
	struct functions
	{
		symbol<napi_status(napi_env env, napi_value value, napi_valuetype* result)> typeof;
		symbol<napi_status(napi_env env, napi_value script, napi_value* result)> run_script;
		symbol<napi_status(napi_env env, napi_value recv, napi_value func, size_t argc, const napi_value* argv,
		                   napi_value* result)> call_function;

		symbol<napi_status(napi_env env, const char* code, const char* msg)> throw_error;
		symbol<napi_status(napi_env env, napi_value error)> throw_;
		symbol<napi_status(napi_env env, bool* result)> is_exception_pending;
		symbol<napi_status(napi_env env, napi_value* result)> get_and_clear_last_exception;
		symbol<napi_status(napi_env env, napi_value object, napi_value key, napi_value value)> set_property;
		symbol<napi_status(napi_env env, napi_value object, napi_value key, napi_value* result)> get_property;
		symbol<napi_status(napi_env env, napi_value object, napi_value key, bool* result)> delete_property;
		symbol<napi_status(napi_env env, napi_ref ref)> delete_reference;
		symbol<napi_status(napi_env env, napi_value value, uint32_t initial_refcount, napi_ref* result)>
		create_reference;
		symbol<napi_status(napi_env env, napi_ref ref, uint32_t* result)> reference_ref;
		symbol<napi_status(napi_env env, napi_ref ref, uint32_t* result)> reference_unref;
		symbol<napi_status(napi_env env, napi_ref ref, napi_value* result)> get_reference_value;
		symbol<napi_status(napi_env env, napi_value* result)> create_object;
		symbol<napi_status(napi_env env, size_t length, napi_value* result)> create_array;
		symbol<napi_status(napi_env env, bool value, napi_value* result)> get_boolean;
		symbol<napi_status(napi_env env, int64_t value, napi_value* result)> create_int64;
		symbol<napi_status(napi_env env, int32_t value, napi_value* result)> create_int32;
		symbol<napi_status(napi_env env, uint32_t value, napi_value* result)> create_uint32;
		symbol<napi_status(napi_env env, double value, napi_value* result)> create_double;
		symbol<napi_status(napi_env env, const char* utf8name, size_t length, napi_callback cb, void* data,
		                   napi_value* result)> create_function;
		symbol<napi_status(napi_env env, const char* str, size_t length, napi_value* result)> create_string_utf8;
		symbol<napi_status(napi_env env, napi_value object, napi_value* result)> get_property_names;
		symbol<napi_status(napi_env env, napi_value* result)> get_global;
		symbol<napi_status(napi_env env, napi_value* result)> get_undefined;
		symbol<napi_status(napi_env env, napi_value value, bool* result)> get_value_bool;
		symbol<napi_status(napi_env env, napi_value value, int64_t* result)> get_value_int64;
		symbol<napi_status(napi_env env, napi_value value, int32_t* result)> get_value_int32;
		symbol<napi_status(napi_env env, napi_value value, uint32_t* result)> get_value_uint32;
		symbol<napi_status(napi_env env, napi_value value, double* result)> get_value_double;
		symbol<napi_status(napi_env env, napi_value value, char* buf, size_t bufsize, size_t* result)>
		get_value_string_utf8;
		symbol<napi_status(napi_env env, napi_value value, uint32_t* result)> get_array_length;
		symbol<napi_status(napi_env env, napi_value object, uint32_t index, napi_value* result)> get_element;
		symbol<napi_status(napi_env env, napi_value arraybuffer, void** data, size_t* byte_length)>
		get_arraybuffer_info;
		symbol<napi_status(napi_env env, napi_callback_info cbinfo, size_t* argc, napi_value* argv, napi_value* thisArg,
		                   void** data)> get_cb_info;
		symbol<napi_status(napi_env env, napi_value object, uint32_t index, napi_value value)> set_element;
		symbol<napi_status(napi_env env, napi_value object, uint32_t index, bool* result)> delete_element;
		symbol<napi_status(napi_env env, void* external_data, size_t byte_length, napi_finalize finalize_cb,
		                   void* finalize_hint, napi_value* result)> create_external_arraybuffer;
		symbol<napi_status(node_api_basic_env env, void* data, napi_finalize finalize_cb, void* finalize_hint)>
		set_instance_data;
		symbol<napi_status(node_api_basic_env env, void** data)> get_instance_data;


		functions(const library& lib)
			: typeof(lib, "napi_typeof")
			  , run_script(lib, "napi_run_script")
			  , call_function(lib, "napi_call_function")
			  , throw_error(lib, "napi_throw_error")
			  , throw_(lib, "napi_throw")
			  , is_exception_pending(lib, "napi_is_exception_pending")
			  , get_and_clear_last_exception(lib, "napi_get_and_clear_last_exception")
			  , set_property(lib, "napi_set_property")
			  , get_property(lib, "napi_get_property")
			  , delete_property(lib, "napi_delete_property")
			  , delete_reference(lib, "napi_delete_reference")
			  , create_reference(lib, "napi_create_reference")
			  , reference_ref(lib, "napi_reference_ref")
			  , reference_unref(lib, "napi_reference_unref")
			  , get_reference_value(lib, "napi_get_reference_value")
			  , create_object(lib, "napi_create_object")
			  , create_array(lib, "napi_create_array")
			  , get_boolean(lib, "napi_get_boolean")
			  , create_int64(lib, "napi_create_int64")
			  , create_int32(lib, "napi_create_int32")
			  , create_uint32(lib, "napi_create_uint32")
			  , create_double(lib, "napi_create_double")
			  , create_function(lib, "napi_create_function")
			  , create_string_utf8(lib, "napi_create_string_utf8")
			  , get_property_names(lib, "napi_get_property_names")
			  , get_global(lib, "napi_get_global")
			  , get_undefined(lib, "napi_get_undefined")
			  , get_value_bool(lib, "napi_get_value_bool")
			  , get_value_int64(lib, "napi_get_value_int64")
			  , get_value_int32(lib, "napi_get_value_int32")
			  , get_value_uint32(lib, "napi_get_value_uint32")
			  , get_value_double(lib, "napi_get_value_double")
			  , get_value_string_utf8(lib, "napi_get_value_string_utf8")
			  , get_array_length(lib, "napi_get_array_length")
			  , get_element(lib, "napi_get_element")
			  , get_arraybuffer_info(lib, "napi_get_arraybuffer_info")
			  , get_cb_info(lib, "napi_get_cb_info")
			  , set_element(lib, "napi_set_element")
			  , delete_element(lib, "napi_delete_element")
			  , create_external_arraybuffer(lib, "napi_create_external_arraybuffer")
			  , set_instance_data(lib, "napi_set_instance_data")
			  , get_instance_data(lib, "napi_get_instance_data")
		{
		}
	};
}
