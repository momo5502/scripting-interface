#pragma once

#include <functional>
#include <memory>

#include "javascript_value.hpp"

#include "export.h"

#define REGISTER_JAVASCRIPT_MODULE(init)                                                  \
MOMO_DLL_EXPORT_C::momo::javascript::napi_value napi_register_module_v1(                  \
	const ::momo::javascript::napi_env env, const ::momo::javascript::napi_value exports) \
{                                                                                         \
	return ::momo::javascript::javascript_interface::initialize(env, exports, init);      \
}

namespace momo::javascript
{
	struct functions;

	class javascript_interface
	{
	public:
		javascript_interface(javascript_interface&&) = delete;
		javascript_interface(const javascript_interface&) = delete;
		javascript_interface& operator=(javascript_interface&&) = delete;
		javascript_interface& operator=(const javascript_interface&) = delete;

		using initializer_func = javascript_value(javascript_interface& js, const javascript_value& exports);
		using initializer =	std::function<initializer_func>;

		using handler_complex_func = std::function<javascript_value(javascript_interface&, const javascript_value& this_value, const std::vector<javascript_value>& args)>;
		using handler_reduced_func = std::function<javascript_value(const javascript_value& this_value, const std::vector<javascript_value>& args)>;
		using handler_args_func = std::function<javascript_value(const std::vector<javascript_value>& args)>;
		using handler_slim_func = std::function<javascript_value()>;
		using handler_min_func = std::function<void()>;

		struct function_entry
		{
			handler_complex_func handler{};
		};

		static napi_value initialize(const napi_env env, const napi_value exports, const initializer& init);

		napi_env get_env() const;
		functions& get_function_interface() const;

		javascript_value create_function(handler_complex_func callback, std::string_view name = {});
		javascript_value create_object();
		javascript_value create_array(size_t length = 0);

		javascript_value get_global();

		javascript_value execute(const std::string_view code);

		bool is_exception_pending() const;
		javascript_value get_and_clear_exception();

		void throw_error(const std::string& message, const std::string& code = "momo") const;
		void throw_value(const javascript_value& error) const;

		javascript_value create_function(handler_reduced_func callback, std::string_view name = {})
		{
			return this->create_function([c = std::move(callback)](javascript_interface&, const javascript_value& this_value, const std::vector<javascript_value>& args)
				{
					return c(this_value, args);
				}, name);
		}

		javascript_value create_function(handler_args_func callback, std::string_view name = {})
		{
			return this->create_function([c = std::move(callback)](javascript_interface&, const javascript_value&, const std::vector<javascript_value>& args)
				{
					return c(args);
				}, name);
		}

		javascript_value create_function(handler_slim_func callback, std::string_view name = {})
		{
			return this->create_function([c = std::move(callback)](javascript_interface&, const javascript_value&, const std::vector<javascript_value>&)
				{
					return c();
				}, name);
		}

		javascript_value create_function(handler_min_func callback, std::string_view name = {})
		{
			return this->create_function([c = std::move(callback)](javascript_interface& js, const javascript_value&, const std::vector<javascript_value>&)
				{
					c();
					return javascript_value(js);
				}, name);
		}

	private:
		napi_env env_{};
		std::unique_ptr<functions> functions_{};
		std::list<std::unique_ptr<function_entry>> function_entries_{};

		javascript_interface(const napi_env env, std::unique_ptr<functions> functions);
		~javascript_interface();
	};
}
