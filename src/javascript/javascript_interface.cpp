#include <momo/javascript_interface.hpp>

#include "javascript_library.hpp"
#include "javascript_functions.hpp"

#include "scoped_interface.hpp"

#include <mutex>
#include <shared_mutex>

namespace momo::javascript
{
	namespace
	{
		std::shared_mutex mutex{};
		std::unordered_map<napi_env, javascript_interface*> active_interfaces{};

		std::unique_ptr<functions> get_functions()
		{
			const auto library = get_javascript_library();
			if (library)
			{
				return std::make_unique<functions>(library);
			}

			throw std::runtime_error("No javascript library found!");
		}

		javascript_interface* get_interface_for_env(const napi_env env)
		{
			std::shared_lock _{mutex};

			const auto entry = active_interfaces.find(env);
			if (entry != active_interfaces.end())
			{
				return entry->second;
			}

			return nullptr;
		}

		napi_value native_function_handler(const napi_env env, const napi_callback_info info)
		{
			auto* js = get_interface_for_env(env);
			if (!js)
			{
				return nullptr;
			}

			scoped_interface _{*js};

			size_t argc{};
			js->get_function_interface().get_cb_info(env, info, &argc, nullptr, nullptr, nullptr);


			void* entry_ptr{};
			napi_value this_value{};

			std::vector<napi_value> args{};
			args.resize(argc);

			js->get_function_interface().get_cb_info(env, info, &argc, args.data(), &this_value, &entry_ptr);

			const auto* entry = static_cast<javascript_interface::function_entry*>(entry_ptr);
			if (!entry)
			{
				return nullptr;
			}

			try
			{
				std::vector<javascript_value> arguments{};
				arguments.reserve(args.size());

				for (const auto& arg : args)
				{
					arguments.emplace_back(*js, arg);
				}

				const javascript_value this_arg{*js, this_value};
				return entry->handler(*js, this_arg, arguments).get();
			}
			catch (std::exception& exception)
			{
				js->throw_error(exception.what());
			}
			catch (...)
			{
				js->throw_error("An unknown error occured during JavaScript handler execution");
			}

			return nullptr;
		}
	}

	napi_value javascript_interface::initialize(const napi_env env, const napi_value exports, const initializer& init)
	{
		auto* js = new javascript_interface(env, get_functions());
		js->get_function_interface().set_instance_data(env, js, +[](const napi_env, void* js_ptr, void*)
		{
			delete static_cast<javascript_interface*>(js_ptr);
		}, nullptr);

		scoped_interface _{*js};
		return init(*js, javascript_value{*js, exports}).get();
	}

	javascript_interface::javascript_interface(const napi_env env, std::unique_ptr<functions> functions)
		: env_(env)
		  , functions_(std::move(functions))
	{
		std::lock_guard _{mutex};
		active_interfaces[env] = this;
	}

	javascript_interface::~javascript_interface()
	{
		std::lock_guard _{mutex};
		const auto entry = active_interfaces.find(this->env_);
		if (entry != active_interfaces.end())
		{
			active_interfaces.erase(entry);
		}
	}

	napi_env javascript_interface::get_env() const
	{
		return this->env_;
	}

	functions& javascript_interface::get_function_interface() const
	{
		return *this->functions_;
	}

	javascript_value javascript_interface::create_function(handler_complex_func callback, const std::string_view name)
	{
		auto entry = std::make_unique<function_entry>();
		entry->handler = std::move(callback);

		napi_value value{};
		this->get_function_interface().create_function(this->get_env(), name.data(), name.size(),
		                                               native_function_handler, entry.get(), &value);

		this->function_entries_.push_back(std::move(entry));

		return {*this, value};
	}

	javascript_value javascript_interface::create_object()
	{
		napi_value value{};
		this->get_function_interface().create_object(this->get_env(), &value);
		return {*this, value};
	}

	javascript_value javascript_interface::create_array(size_t length)
	{
		napi_value value{};
		this->get_function_interface().create_array(this->get_env(), length, &value);
		return {*this, value};
	}

	javascript_value javascript_interface::get_global()
	{
		napi_value value{};
		this->get_function_interface().get_global(this->get_env(), &value);
		return {*this, value};
	}

	javascript_value javascript_interface::execute(const std::string_view code)
	{
		napi_value result{};
		const javascript_value code_value{*this, code};

		this->get_function_interface().run_script(this->get_env(), code_value.get(), &result);

		return {*this, result};
	}

	bool javascript_interface::is_exception_pending() const
	{
		bool result{};
		this->get_function_interface().is_exception_pending(this->get_env(), &result);
		return result;
	}

	javascript_value javascript_interface::get_and_clear_exception()
	{
		if (!this->is_exception_pending())
		{
			return {*this};
		}

		napi_value result{};
		this->get_function_interface().get_and_clear_last_exception(this->get_env(), &result);
		return {*this, result};
	}

	void javascript_interface::throw_error(const std::string& message, const std::string& code) const
	{
		this->get_function_interface().throw_error(this->get_env(), code.c_str(), message.c_str());
	}

	void javascript_interface::throw_value(const javascript_value& error) const
	{
		this->get_function_interface().throw_(this->get_env(), error.get());
	}
}
