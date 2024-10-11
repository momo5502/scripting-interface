#pragma once

namespace momo::javascript
{
	// See NodeJS's js_native_api_types.h

	enum napi_valuetype
	{
		// ES6 types (corresponds to typeof)
		napi_undefined,
		napi_null,
		napi_boolean,
		napi_number,
		napi_string,
		napi_symbol,
		napi_object,
		napi_function,
		napi_external,
		napi_bigint,
	};

	enum napi_status
	{
		napi_ok,
		napi_invalid_arg,
		napi_object_expected,
		napi_string_expected,
		napi_name_expected,
		napi_function_expected,
		napi_number_expected,
		napi_boolean_expected,
		napi_array_expected,
		napi_generic_failure,
		napi_pending_exception,
		napi_cancelled,
		napi_escape_called_twice,
		napi_handle_scope_mismatch,
		napi_callback_scope_mismatch,
		napi_queue_full,
		napi_closing,
		napi_bigint_expected,
		napi_date_expected,
		napi_arraybuffer_expected,
		napi_detachable_arraybuffer_expected,
		napi_would_deadlock // unused
	};

	using napi_env = struct napi_env__*;
	using napi_value = struct napi_value__*;
	using napi_ref = struct napi_ref__*;
	using napi_callback_info = struct napi_callback_info__*;
	using node_api_basic_env = napi_env;

	using napi_finalize = void (*)(napi_env, void*, void*);
	using napi_addon_register_func = napi_value(*)(napi_env, napi_value);
	using napi_callback = napi_value(*)(napi_env, napi_callback_info);
}
