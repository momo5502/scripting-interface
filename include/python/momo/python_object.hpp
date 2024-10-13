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

#include "python_types.hpp"

namespace momo::python
{
	struct functions;
	class python_interface;

	class python_object
	{
	public:
		class property_accessor
		{
		public:
			friend python_object;

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

			property_accessor& operator=(const python_object& obj)
			{
				this->assign(obj);
				return *this;
			}

			operator python_object() const
			{
				return this->get();
			}

			void assign(const python_object& value) const
			{
				this->value_->set_property(this->property_, value);
			}

			python_object get() const
			{
				return this->value_->get_property(this->property_);
			}

		private:
			const python_object* value_{};
			std::string property_{};

			property_accessor(const python_object& value, std::string property)
				: value_(&value)
				, property_(std::move(property))
			{
			}
		};

		class element_iterator;

		class element_accessor
		{
		public:
			friend python_object;
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

			element_accessor& operator=(const python_object& obj)
			{
				this->assign(obj);
				return *this;
			}

			operator python_object() const
			{
				return this->get();
			}

			void assign(const python_object& value) const
			{
				this->value_->set_element(this->index_, value);
			}

			python_object get() const
			{
				return this->value_->get_element(this->index_);
			}

		private:
			const python_object* value_{};
			size_t index_{};

			element_accessor(const python_object& value, size_t index)
				: value_(&value)
				, index_(index)
			{
			}
		};

		class element_iterator
		{
		public:
			friend python_object;

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

			element_iterator(const python_object& value, const size_t index)
				: accessor_(value, index)
			{
			}

			element_iterator(const element_accessor& accessor)
				: element_iterator(accessor.value_, accessor.index_)
			{
			}
		};

		python_object(python_interface& py);
		python_object(python_interface& py, python_object obj);

		python_object(python_interface& py, PyObject* value);
		python_object(python_interface& py, PyObjectBorrowed* value);
		python_object(python_interface& py, std::nullptr_t);

		python_object(python_interface& py, bool value);
		python_object(python_interface& py, int64_t value);
		python_object(python_interface& py, uint64_t value);
		python_object(python_interface& py, double value);
		python_object(python_interface& py, float value);
		python_object(python_interface& py, std::string_view string);
		python_object(python_interface& py, const void* ptr);

		python_object(python_interface& py, const uint8_t* data, size_t length);

		python_object(python_interface& py, const char* string)
			: python_object(py, std::string_view(string))
		{
		}

		python_object(python_interface& py, const std::string& string)
			: python_object(py, std::string_view(string))
		{
		}

		python_object(python_interface& py, const void* data, const size_t length)
			: python_object(py, static_cast<const uint8_t*>(data), length)
		{
		}

#ifdef __cpp_lib_span
		python_object(python_interface& py, const std::span<const uint8_t>& data)
			: python_object(py, data.data(), data.size())
		{

		}
#endif

		python_object()
			: python_object(get_py())
		{
		}

		python_object(PyObject* value)
			: python_object(get_py(), value)
		{
		}

		python_object(PyObjectBorrowed* value)
			: python_object(get_py(), value)
		{
		}

		python_object(const std::nullptr_t)
			: python_object(get_py(), nullptr)
		{
		}

		python_object(const bool value)
			: python_object(get_py(), value)
		{
		}

		python_object(const int64_t value)
			: python_object(get_py(), value)
		{
		}

		python_object(const uint64_t value)
			: python_object(get_py(), value)
		{
		}

		python_object(const double value)
			: python_object(get_py(), value)
		{
		}

		python_object(const float value)
			: python_object(get_py(), value)
		{
		}

		python_object(const std::string_view string)
			: python_object(get_py(), string)
		{
		}

		python_object(const char* string)
			: python_object(get_py(), string)
		{
		}

		python_object(const std::string& string)
			: python_object(get_py(), string)
		{
		}

		python_object(const void* ptr)
			: python_object(get_py(), ptr)
		{
		}

		python_object(const uint8_t* data, const size_t length)
			: python_object(get_py(), data, length)
		{
		}

		python_object(const void* data, const size_t length)
			: python_object(get_py(), data, length)
		{
		}

#ifdef __cpp_lib_span
		python_object(const std::span<const uint8_t> data)
			: python_object(get_py(), data)
		{
		}
#endif

		~python_object()
		{
			this->release();
		}

		python_object(python_object&& obj) noexcept;
		python_object& operator=(python_object&& obj) noexcept;

		python_object(const python_object& obj);
		python_object& operator=(const python_object& obj);

		python_interface& get_interface() const
		{
			return *this->py_;
		}

		template <typename T>
		T as() const;

		explicit operator bool() const
		{
			return this->value_;
		}

		void inc_ref() const;
		void dec_ref() const;

		python_object as_tuple() const;
		python_object as_list() const;
		python_object as_string() const;

		bool is_list() const;
		bool is_tuple() const;
		bool is_dict() const;
		bool is_string() const;

		void set_attribute(const std::string& key, const python_object& value) const;
		python_object get_attribute(const std::string& key) const;
		void delete_attribute(const std::string& key) const;
		bool has_attribute(const std::string& key) const;

		python_object get_property_names() const;
		void set_property(const std::string& key, const python_object& value) const;
		python_object get_property(const std::string& key) const;
		bool delete_property(const std::string& key) const;

		size_t get_length() const;
		python_object get_element(size_t index) const;
		//bool delete_element(size_t index) const;
		void set_element(size_t index, const python_object& value) const;

		void add_element(const python_object& value) const;

		size_t get_property_count() const
		{
			return this->get_property_names().get_length();
		}

		std::map<std::string, python_object, std::less<>> get_properties() const;
		std::vector<python_object> get_elements() const;

		python_object call(const python_object& args, const python_object& kwargs) const;
		python_object call(const std::vector<python_object>& arguments = {}) const;

		template <typename... Args>
		python_object operator()(Args&&... args) const
		{
			const std::vector<python_object> arguments{python_object{*this->py_, std::forward<Args>(args)}...};
			return this->call(arguments);
		}

		PyObjectBorrowed* borrow() const
		{
			return reinterpret_cast<PyObjectBorrowed*>(this->value_);
		}

		PyObject* get_new_ref() const
		{
			this->inc_ref();
			return this->value_;
		}

		property_accessor operator[](std::string property) const
		{
			return { *this, std::move(property) };
		}

		element_accessor operator[](const size_t index) const
		{
			return { *this, index };
		}

		element_iterator begin() const
		{
			return { *this, 0 };
		}

		element_iterator end() const
		{
			return { *this, this->get_length() };
		}

	private:
		python_interface* py_{};
		PyObject* value_{};

		static python_interface& get_py();

		functions& get_functions() const;
		void release();
	};

	template <>
	bool python_object::as<bool>() const;

	template <>
	void* python_object::as<void*>() const;

	template <>
	int64_t python_object::as<int64_t>() const;

	template <>
	inline int32_t python_object::as<int32_t>() const
	{
		return static_cast<int32_t>(this->as<int64_t>());
	}

	template <>
	inline int16_t python_object::as<int16_t>() const
	{
		return static_cast<int16_t>(this->as<int64_t>());
	}

	template <>
	inline int8_t python_object::as<int8_t>() const
	{
		return static_cast<int8_t>(this->as<int64_t>());
	}

	template <>
	uint64_t python_object::as<uint64_t>() const;

	template <>
	inline uint32_t python_object::as<uint32_t>() const
	{
		return static_cast<uint32_t>(this->as<uint64_t>());
	}

	template <>
	inline uint16_t python_object::as<uint16_t>() const
	{
		return static_cast<uint16_t>(this->as<uint64_t>());
	}

	template <>
	inline uint8_t python_object::as<uint8_t>() const
	{
		return static_cast<uint8_t>(this->as<uint64_t>());
	}

	template <>
	double python_object::as<double>() const;

	template <>
	inline float python_object::as<float>() const
	{
		return static_cast<float>(this->as<double>());
	}

	template <>
	std::string python_object::as<std::string>() const;

	template <>
	std::pair<uint8_t*, size_t> python_object::as<std::pair<uint8_t*, size_t>>() const;

	template <>
	inline std::vector<uint8_t> python_object::as<std::vector<uint8_t>>() const
	{
		const auto data = this->as<std::pair<uint8_t*, size_t>>();

		std::vector<uint8_t> result{};
		result.assign(data.first, data.first + data.second);

		return result;
	}

#ifdef __cpp_lib_span
	inline std::span<uint8_t> python_object::as<std::pair<uint8_t*, size_t>>() const
	{
		const auto data = this->as<std::pair<uint8_t*, size_t>>();
		return {data.first , data.second};
	}
#endif
}
