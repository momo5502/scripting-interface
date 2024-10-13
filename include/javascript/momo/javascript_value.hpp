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
	struct functions;
	class javascript_interface;

	class javascript_value
	{
	public:
		class property_accessor
		{
		public:
			friend javascript_value;

			~property_accessor() = default;

			property_accessor(property_accessor&&) noexcept = default;
			property_accessor& operator=(property_accessor&&) noexcept = default;

			property_accessor(const property_accessor&) = delete;

			property_accessor& operator=(const property_accessor& obj)
			{
				if (this != &obj)
				{
					this->assign(obj.get());
				}

				return *this;
			}

			property_accessor& operator=(const javascript_value& obj)
			{
				this->assign(obj);
				return *this;
			}

			operator javascript_value() const
			{
				return this->get();
			}

			void assign(const javascript_value& value) const
			{
				this->value_->set_property(this->property_, value);
			}

			javascript_value get() const
			{
				return this->value_->get_property(this->property_);
			}

		private:
			const javascript_value* value_{};
			std::string property_{};

			property_accessor(const javascript_value& value, std::string property)
				: value_(&value)
				  , property_(std::move(property))
			{
			}
		};

		class element_iterator;

		class element_accessor
		{
		public:
			friend javascript_value;
			friend element_iterator;

			~element_accessor() = default;

			element_accessor(element_accessor&&) noexcept = default;
			element_accessor& operator=(element_accessor&&) noexcept = default;

			element_accessor(const element_accessor&) = delete;

			element_accessor& operator=(const element_accessor& obj)
			{
				if (this != &obj)
				{
					this->assign(obj.get());
				}

				return *this;
			}

			element_accessor& operator=(const javascript_value& obj)
			{
				this->assign(obj);
				return *this;
			}

			operator javascript_value() const
			{
				return this->get();
			}

			void assign(const javascript_value& value) const
			{
				this->value_->set_element(this->index_, value);
			}

			javascript_value get() const
			{
				return this->value_->get_element(this->index_);
			}

		private:
			const javascript_value* value_{};
			size_t index_{};

			element_accessor(const javascript_value& value, size_t index)
				: value_(&value)
				  , index_(index)
			{
			}
		};

		class element_iterator
		{
		public:
			friend javascript_value;

			~element_iterator() = default;

			element_iterator(element_iterator&& obj) noexcept
				: element_iterator(obj.accessor_)
			{
			}

			element_iterator& operator=(element_iterator&& obj) noexcept
			{
				this->operator=(obj);
				return *this;
			}

			element_iterator(const element_iterator& obj) noexcept
				: element_iterator(obj.accessor_)
			{
			}

			element_iterator& operator=(const element_iterator& obj) noexcept
			{
				if (this != &obj)
				{
					this->accessor_.value_ = obj.accessor_.value_;
					this->accessor_.index_ = obj.accessor_.index_;
				}

				return *this;
			}

			element_accessor& operator*()
			{
				return this->accessor_;
			}

			const element_accessor& operator*() const
			{
				return this->accessor_;
			}

			element_iterator& operator++()
			{
				++this->accessor_.index_;
				return *this;
			}

			element_iterator operator++(int)
			{
				element_iterator temp = *this;
				++(*this);
				return temp;
			}

			bool operator==(const element_iterator& other) const
			{
				return this->accessor_.index_ == other.accessor_.index_ && this->accessor_.value_ == other.accessor_.
					value_;
			}

			bool operator!=(const element_iterator& other) const
			{
				return !(*this == other);
			}

		private:
			element_accessor accessor_;

			element_iterator(const javascript_value& value, const size_t index)
				: accessor_(value, index)
			{
			}

			element_iterator(const element_accessor& accessor)
				: element_iterator(accessor.value_, accessor.index_)
			{
			}
		};

		javascript_value(javascript_interface& js);

		javascript_value(javascript_interface& js, const javascript_value& obj);

		javascript_value(javascript_interface& js, napi_value value);
		javascript_value(javascript_interface& js, std::nullptr_t);

		javascript_value(javascript_interface& js, bool value);
		javascript_value(javascript_interface& js, int64_t value);
		javascript_value(javascript_interface& js, int32_t value);
		javascript_value(javascript_interface& js, uint32_t value);
		javascript_value(javascript_interface& js, double value);
		javascript_value(javascript_interface& js, std::string_view string);

		javascript_value(javascript_interface& js, const uint8_t* data, size_t length);

		javascript_value(javascript_interface& js, const char* string)
			: javascript_value(js, std::string_view(string))
		{
		}

		javascript_value(javascript_interface& js, const std::string& string)
			: javascript_value(js, std::string_view(string))
		{
		}

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

		javascript_value()
			: javascript_value(get_js())
		{
		}

		javascript_value(const napi_value value)
			: javascript_value(get_js(), value)
		{
		}

		javascript_value(const std::nullptr_t)
			: javascript_value(get_js(), nullptr)
		{
		}

		javascript_value(const bool value)
			: javascript_value(get_js(), value)
		{
		}

		javascript_value(const int64_t value)
			: javascript_value(get_js(), value)
		{
		}

		javascript_value(const int32_t value)
			: javascript_value(get_js(), value)
		{
		}

		javascript_value(const uint32_t value)
			: javascript_value(get_js(), value)
		{
		}

		javascript_value(const double value)
			: javascript_value(get_js(), value)
		{
		}

		javascript_value(const float value)
			: javascript_value(get_js(), value)
		{
		}

		javascript_value(const std::string_view string)
			: javascript_value(get_js(), string)
		{
		}

		javascript_value(const char* string)
			: javascript_value(get_js(), string)
		{
		}

		javascript_value(const std::string& string)
			: javascript_value(get_js(), string)
		{
		}

		javascript_value(const uint8_t* data, const size_t length)
			: javascript_value(get_js(), data, length)
		{
		}

		javascript_value(const void* data, const size_t length)
			: javascript_value(get_js(), data, length)
		{
		}

#ifdef __cpp_lib_span
		javascript_value(const std::span<const uint8_t> data)
			: javascript_value(get_js(), data)
		{
		}
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

		explicit operator bool() const
		{
			return this->value_ && !this->is_null() && !this->is_undefined();
		}

		javascript_value get_property_names() const;
		void set_property(const std::string& key, const javascript_value& value) const;
		javascript_value get_property(const std::string& key) const;
		bool delete_property(const std::string& key) const;


		size_t get_length() const;
		javascript_value get_element(size_t index) const;
		bool delete_element(size_t index) const;
		void set_element(size_t index, const javascript_value& value) const;

		void add_element(const javascript_value& value) const
		{
			set_element(get_length(), value);
		}

		size_t get_property_count() const
		{
			return this->get_property_names().get_length();
		}

		property_accessor operator[](std::string property) const
		{
			return {*this, std::move(property)};
		}

		element_accessor operator[](const size_t index) const
		{
			return {*this, index};
		}

		element_iterator begin() const
		{
			return {*this, 0};
		}

		element_iterator end() const
		{
			return {*this, this->get_length()};
		}

		std::map<std::string, javascript_value, std::less<>> get_properties() const;
		std::vector<javascript_value> get_elements() const;

		javascript_value call(const javascript_value& this_value, const std::vector<javascript_value>& arguments) const;

		template <typename... Args>
		javascript_value operator()(const javascript_value& this_value, Args&&... args) const
		{
			const std::vector<javascript_value> arguments{ javascript_value{*this->js_, std::forward<Args>(args)}... };
			return this->call(this_value, arguments);
		}

	private:
		javascript_interface* js_{};
		napi_value value_{};

		static javascript_interface& get_js();

		napi_env get_env() const;
		functions& get_functions() const;
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
