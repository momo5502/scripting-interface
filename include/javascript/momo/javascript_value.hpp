#pragma once

#include <string_view>
#include <cstddef>
#include <cstdint>
#include <version>
#include <vector>
#include <string>
#include <map>
#include <stdexcept>

#ifdef __cpp_lib_span
#include <span>
#endif

#include "napi_types.hpp"

namespace momo::javascript
{
	class javascript_interface;

	class javascript_value
	{
	public:
		javascript_value(javascript_interface& js);

		javascript_value(javascript_interface& js, napi_value value);
		javascript_value(javascript_interface& js, std::nullptr_t);

		javascript_value(javascript_interface& js, bool value);
		javascript_value(javascript_interface& js, int64_t value);
		javascript_value(javascript_interface& js, int32_t value);
		javascript_value(javascript_interface& js, uint32_t value);
		javascript_value(javascript_interface& js, double value);
		javascript_value(javascript_interface& js, std::string_view string);

		javascript_value(javascript_interface& js, const uint8_t* data, size_t length);

		javascript_value(javascript_interface& js, const void* data, const size_t length)
			: javascript_value(js, static_cast<const uint8_t*>(data), length)
		{
		}

		javascript_value(javascript_interface& js, const float value)
			: javascript_value(js, static_cast<double>(value))
		{
		}

#ifdef __cpp_lib_span
		javascript_value(javascript_interface& js, const std::span<const uint8_t>& data)
			: javascript_value(js, data.data(), data.size())
		{

		}
#endif

		javascript_value() : javascript_value(get_js())
		{
		}

		javascript_value(const napi_value value) : javascript_value(get_js(), value)
		{
		}

		javascript_value(const std::nullptr_t) : javascript_value(get_js(), nullptr)
		{
		}

		javascript_value(const bool value) : javascript_value(get_js(), value)
		{
		}

		javascript_value(const int64_t value) : javascript_value(get_js(), value)
		{
		}

		javascript_value(const int32_t value) : javascript_value(get_js(), value)
		{
		}

		javascript_value(const uint32_t value) : javascript_value(get_js(), value)
		{
		}

		javascript_value(const double value) : javascript_value(get_js(), value)
		{
		}

		javascript_value(const float value) : javascript_value(get_js(), static_cast<double>(value))
		{
		}

		javascript_value(const std::string_view string) : javascript_value(get_js(), string)
		{
		}

		javascript_value(const uint8_t* data, const size_t length) : javascript_value(get_js(), data, length)
		{
		}

		javascript_value(const void* data, const size_t length) : javascript_value(get_js(), data, length)
		{
		}

#ifdef __cpp_lib_span
		javascript_value(const std::span<const uint8_t> data) : javascript_value(get_js(), data) {}
#endif

		~javascript_value() = default;
		javascript_value(javascript_value&& obj) noexcept = default;
		javascript_value& operator=(javascript_value&& obj) noexcept = default;
		javascript_value(const javascript_value& obj) noexcept = default;
		javascript_value& operator=(const javascript_value& obj) noexcept = default;

		napi_value get() const
		{
			return this->value_;
		}

		javascript_interface& get_interface() const
		{
			return *this->js_;
		}

		napi_valuetype get_type() const;

		bool is_type(const napi_valuetype type) const
		{
			return this->get_type() == type;
		}

		void assert_type(const napi_valuetype type) const
		{
			if (!this->is_type(type))
			{
				throw std::runtime_error("Bad value type");
			}
		}

		bool is_null() const
		{
			return this->is_type(napi_null);
		}

		bool is_undefined() const
		{
			return this->is_type(napi_undefined);
		}

		template <typename T>
		T as() const;

		operator bool() const
		{
			return this->value_ && !this->is_null() && !this->is_undefined();
		}

		javascript_value get_property_names() const;
		void set_property(const std::string& key, const javascript_value& value) const;
		javascript_value get_property(const std::string& key) const;
		bool delete_property(const std::string& key) const;

		template <typename... Args>
		void set_property(const std::string& key, Args&&... args) const
		{
			const javascript_value value(this->get_interface(), std::forward<Args>(args)...);
			set_property(key, value);
		}

		size_t get_length() const;
		javascript_value get_element(size_t index) const;
		bool delete_element(size_t index) const;
		void set_element(size_t index, const javascript_value& value) const;

		template <typename... Args>
		void set_element(const size_t index, Args&&... args) const
		{
			const javascript_value value(this->get_interface(), std::forward<Args>(args)...);
			set_element(index, value);
		}

		template <typename... Args>
		void add_element(Args&&... args) const
		{
			set_element(get_length(), std::forward<Args>(args)...);
		}

		std::map<std::string, javascript_value, std::less<>> get_properties() const;
		std::vector<javascript_value> get_elements() const;

		javascript_value call(const javascript_value& this_value, const std::vector<javascript_value>& arguments) const;

		template <typename... Args>
		javascript_value operator()(const javascript_value& this_value, Args&&... args) const
		{
			const std::vector<javascript_value> arguments{ std::forward<Args>(args)... };
			return this->call(this_value, arguments);
		}

	private:
		javascript_interface* js_{};
		napi_value value_{};

		static javascript_interface& get_js();
	};

	template <>
	bool javascript_value::as<bool>() const;

	template <>
	int64_t javascript_value::as<int64_t>() const;

	template <>
	int32_t javascript_value::as<int32_t>() const;

	template <>
	uint32_t javascript_value::as<uint32_t>() const;

	template <>
	double javascript_value::as<double>() const;

	template <>
	inline float javascript_value::as<float>() const
	{
		return static_cast<float>(this->as<double>());
	}

	template <>
	std::string javascript_value::as<std::string>() const;

	template <>
	std::pair<uint8_t*, size_t> javascript_value::as<std::pair<uint8_t*, size_t>>() const;

	template <>
	inline std::vector<uint8_t> javascript_value::as<std::vector<uint8_t>>() const
	{
		const auto data = this->as<std::pair<uint8_t*, size_t>>();

		std::vector<uint8_t> result{};
		result.assign(data.first, data.first + data.second);

		return result;
	}

#ifdef __cpp_lib_span
	inline std::span<uint8_t> javascript_value::as<std::pair<uint8_t*, size_t>>() const
	{
		const auto data = this->as<std::pair<uint8_t*, size_t>>();
		return {data.first , data.second};
	}
#endif
}
