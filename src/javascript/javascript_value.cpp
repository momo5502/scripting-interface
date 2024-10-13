#include <momo/javascript_value.hpp>

#include "javascript_functions.hpp"
#include "scoped_interface.hpp"

#include <cassert>
#include <cstring>

namespace momo::javascript
{
	javascript_interface& javascript_value::get_js()
	{
		return get_active_interface();
	}

	napi_env javascript_value::get_env() const
	{
		return this->js_->get_env();
	}

	functions& javascript_value::get_functions() const
	{
		return this->js_->get_function_interface();
	}

	javascript_value::javascript_value(javascript_interface& js, const javascript_value& obj)
		: js_(&js)
		  , value_(obj.value_)
	{
		assert(this->js_ == obj.js_);
	}

	javascript_value::javascript_value(javascript_interface& js, const napi_value value)
		: js_(&js)
		  , value_(value)
	{
	}

	javascript_value::javascript_value(javascript_interface& js)
		: js_(&js)
	{
		this->get_functions().get_undefined(js.get_env(), &this->value_);
	}

	javascript_value::javascript_value(javascript_interface& js, std::nullptr_t)
		: js_(&js)
	{
		this->get_functions().get_null(js.get_env(), &this->value_);
	}

	javascript_value::javascript_value(javascript_interface& js, const bool value)
		: js_(&js)
	{
		this->get_functions().get_boolean(js.get_env(), value, &this->value_);
	}

	javascript_value::javascript_value(javascript_interface& js, const int64_t value)
		: js_(&js)
	{
		this->get_functions().create_int64(js.get_env(), value, &this->value_);
	}

	javascript_value::javascript_value(javascript_interface& js, const int32_t value)
		: js_(&js)
	{
		this->get_functions().create_int32(js.get_env(), value, &this->value_);
	}

	javascript_value::javascript_value(javascript_interface& js, const uint32_t value)
		: js_(&js)
	{
		this->get_functions().create_uint32(js.get_env(), value, &this->value_);
	}

	javascript_value::javascript_value(javascript_interface& js, const double value)
		: js_(&js)
	{
		this->get_functions().create_double(js.get_env(), value, &this->value_);
	}

	javascript_value::javascript_value(javascript_interface& js, const std::string_view string)
		: js_(&js)
	{
		this->get_functions().create_string_utf8(js.get_env(), string.data(), string.size(), &this->value_);
	}

	javascript_value::javascript_value(javascript_interface& js, const uint8_t* data, const size_t length)
		: js_(&js)
	{
		auto* buffer = malloc(std::max(length, static_cast<size_t>(1)));
		memcpy(buffer, data, length);

		auto* deleter = +[](napi_env, void* ptr, void*)
		{
			free(ptr);
		};

		this->get_functions().create_external_arraybuffer(js.get_env(), buffer, length, deleter, nullptr,
		                                                  &this->value_);
	}

	napi_valuetype javascript_value::get_type() const
	{
		napi_valuetype type{napi_undefined};
		this->get_functions().typeof_(this->get_env(), this->get(), &type);
		return type;
	}

	javascript_value javascript_value::get_property_names() const
	{
		this->assert_type(napi_object);

		napi_value result{};
		this->get_functions().get_property_names(this->get_env(), this->get(), &result);
		return javascript_value{*this->js_, result};
	}

	void javascript_value::set_property(const std::string& key, const javascript_value& value) const
	{
		this->assert_type(napi_object);

		const javascript_value key_value{*this->js_, key};
		this->get_functions().set_property(this->get_env(), this->get(), key_value.get(),
		                                   value.get());
	}

	javascript_value javascript_value::get_property(const std::string& key) const
	{
		this->assert_type(napi_object);

		napi_value result{};
		const javascript_value key_value{*this->js_, key};

		this->get_functions().get_property(this->get_env(), this->get(), key_value.get(), &result);
		return javascript_value{*this->js_, result};
	}

	bool javascript_value::delete_property(const std::string& key) const
	{
		this->assert_type(napi_object);

		bool result{};
		const javascript_value key_value{*this->js_, key};
		this->get_functions().
		      delete_property(this->get_env(), this->get(), key_value.get(), &result);
		return result;
	}

	size_t javascript_value::get_length() const
	{
		this->assert_type(napi_object);

		uint32_t length{};
		this->get_functions().get_array_length(this->get_env(), this->get(), &length);
		return length;
	}

	javascript_value javascript_value::get_element(const size_t index) const
	{
		this->assert_type(napi_object);

		napi_value result{};
		this->get_functions().get_element(this->get_env(), this->get(), static_cast<uint32_t>(index),
		                                  &result);
		return javascript_value{*this->js_, result};
	}

	bool javascript_value::delete_element(const size_t index) const
	{
		this->assert_type(napi_object);

		bool result{};
		this->get_functions().delete_element(this->get_env(), this->get(),
		                                     static_cast<uint32_t>(index), &result);
		return result;
	}

	void javascript_value::set_element(const size_t index, const javascript_value& value) const
	{
		this->assert_type(napi_object);

		this->get_functions().set_element(this->get_env(), this->get(), static_cast<uint32_t>(index),
		                                  value.get());
	}

	std::map<std::string, javascript_value, std::less<>> javascript_value::get_properties() const
	{
		std::map<std::string, javascript_value, std::less<>> properties{};

		const auto property_names = this->get_property_names().get_elements();
		for (const auto& property : property_names)
		{
			auto name = property.as<std::string>();
			auto value = this->get_property(name);
			properties[std::move(name)] = std::move(value);
		}

		return properties;
	}

	std::vector<javascript_value> javascript_value::get_elements() const
	{
		this->assert_type(napi_object);

		const auto length = this->get_length();

		std::vector<javascript_value> elements{};
		elements.reserve(length);

		for (size_t i = 0; i < length; ++i)
		{
			elements.push_back(this->get_element(i));
		}

		return elements;
	}

	javascript_value javascript_value::call(const javascript_value& this_value,
	                                        const std::vector<javascript_value>& arguments) const
	{
		this->assert_type(napi_function);

		std::vector<napi_value> args{};
		args.reserve(arguments.size());

		for (const auto& arg : arguments)
		{
			args.push_back(arg.get());
		}

		napi_value this_argument = this_value.get();
		if (!this_argument)
		{
			this_argument = javascript_value{*this->js_}.get();
		}

		napi_value result{};
		this->get_functions().call_function(this->get_env(), this_argument, this->get(),
		                                    arguments.size(),
		                                    args.data(), &result);

		return javascript_value{*this->js_, result};
	}

	template <>
	bool javascript_value::as<bool>() const
	{
		this->assert_type(napi_boolean);

		bool result{};
		this->get_functions().get_value_bool(this->get_env(), this->get(), &result);
		return result;
	}

	template <>
	int64_t javascript_value::as<int64_t>() const
	{
		this->assert_type(napi_number);

		int64_t result{};
		this->get_functions().get_value_int64(this->get_env(), this->get(), &result);
		return result;
	}

	template <>
	int32_t javascript_value::as<int32_t>() const
	{
		this->assert_type(napi_number);

		int32_t result{};
		this->get_functions().get_value_int32(this->get_env(), this->get(), &result);
		return result;
	}

	template <>
	uint32_t javascript_value::as<uint32_t>() const
	{
		this->assert_type(napi_number);

		uint32_t result{};
		this->get_functions().get_value_uint32(this->get_env(), this->get(), &result);
		return result;
	}

	template <>
	double javascript_value::as<double>() const
	{
		this->assert_type(napi_number);

		double result{};
		this->get_functions().get_value_double(this->get_env(), this->get(), &result);
		return result;
	}

	template <>
	std::string javascript_value::as<std::string>() const
	{
		this->assert_type(napi_string);

		size_t length{};
		this->get_functions().get_value_string_utf8(this->get_env(), this->get(), nullptr, 0,
		                                            &length);

		std::string result{};
		result.resize(length + 1);

		this->get_functions().get_value_string_utf8(this->get_env(), this->get(), result.data(),
		                                            result.size(), &length);

		result.pop_back();
		return result;
	}
}
