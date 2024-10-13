#include "momo/python_object.hpp"

#include <momo/python_object.hpp>
#include <momo/python_interpreter_lock.hpp>

#include "python_functions.hpp"
#include "scoped_interface.hpp"

#include <cstring>

#include "python_functions.hpp"

namespace momo::python
{
	namespace
	{
		template <typename... Args>
		PyObject* build_value(python_interface& py, Args... args)
		{
			auto& f = py.get_function_interface();
			python_interpreter_lock lock{f};
			return f.build_value(args...);
		}
	}

	python_interface& python_object::get_py()
	{
		return get_active_interface();
	}

	functions& python_object::get_functions() const
	{
		return this->py_->get_function_interface();
	}

	python_object::python_object(python_interface& py, PyObject* value)
		: py_(&py)
		  , value_(value)
	{
		if (!this->value_)
		{
			this->value_ = build_value(py, "");
		}
	}

	python_object::python_object(python_interface& py, PyObjectBorrowed* value)
		: python_object(py, reinterpret_cast<PyObject*>(value))
	{
		this->inc_ref();
	}

	python_object::python_object(python_interface& py)
		: python_object(py, nullptr)
	{
	}

	python_object::python_object(python_interface& py, python_object obj)
		: python_object(py)
	{
		this->operator=(std::move(obj));
		this->py_ = &py;
	}

	python_object::python_object(python_interface& py, std::nullptr_t)
		: python_object(py, static_cast<PyObject*>(nullptr))
	{
	}

	python_object::python_object(python_interface& py, const bool value)
		: py_(&py)
	{
		auto& f = py.get_function_interface();

		python_interpreter_lock lock{f};
		this->value_ = f.bool_from_long(value ? 1 : 0);
	}

	python_object::python_object(python_interface& py, const int64_t value)
		: py_(&py)
		  , value_(build_value(py, "L", static_cast<long long>(value)))
	{
		static_assert(sizeof(int64_t) == sizeof(long long));
	}

	python_object::python_object(python_interface& py, const uint64_t value)
		: py_(&py)
		  , value_(build_value(py, "K", static_cast<unsigned long long>(value)))
	{
		static_assert(sizeof(uint64_t) == sizeof(unsigned long long));
	}

	python_object::python_object(python_interface& py, const double value)
		: py_(&py)
		  , value_(build_value(py, "d", value))
	{
	}

	python_object::python_object(python_interface& py, const float value)
		: py_(&py)
		  , value_(build_value(py, "f", value))
	{
	}

	python_object::python_object(python_interface& py, const std::string_view string)
		: py_(&py)
		  , value_(build_value(py, "s#", string.data(), string.size()))
	{
	}


	python_object::python_object(python_interface& py, const void* ptr)
		: py_(&py)
	{
		auto& f = py.get_function_interface();

		python_interpreter_lock lock{f};
		this->value_ = f.long_from_void_ptr(const_cast<void*>(ptr));
	}

	python_object::python_object(python_interface& py, const uint8_t* data, const size_t length)
		: py_(&py)
		  , value_(build_value(py, "y#", data, length))
	{
	}

	python_object::python_object(python_object&& obj) noexcept
	{
		this->operator=(std::move(obj));
	}

	python_object& python_object::operator=(python_object&& obj) noexcept
	{
		if (this != &obj)
		{
			this->release();

			this->py_ = obj.py_;
			this->value_ = obj.value_;

			obj.value_ = nullptr;
		}

		return *this;
	}

	python_object::python_object(const python_object& obj)
	{
		this->operator=(obj);
	}

	python_object& python_object::operator=(const python_object& obj)
	{
		if (this != &obj)
		{
			this->release();

			this->py_ = obj.py_;
			this->value_ = obj.get_new_ref();
		}

		return *this;
	}

	void python_object::release()
	{
		if (this->value_)
		{
			this->dec_ref();
			this->value_ = nullptr;
		}
	}

	void python_object::inc_ref() const
	{
		if (!this->value_)
		{
			return;
		}

		python_interpreter_lock lock{*this->py_};
		this->get_functions().inc_ref(this->value_);
	}

	void python_object::dec_ref() const
	{
		if (!this->value_)
		{
			return;
		}

		python_interpreter_lock lock{*this->py_};
		this->get_functions().dec_ref(this->value_);
	}

	python_object python_object::as_tuple() const
	{
		python_interpreter_lock lock{*this->py_};

		if (this->is_tuple())
		{
			return *this;
		}

		PyObject* new_value = nullptr;

		if (this->is_list())
		{
			new_value = this->get_functions().list_as_tuple(this->value_);
		}

		return {*this->py_, new_value};
	}

	python_object python_object::as_list() const
	{
		python_interpreter_lock lock{*this->py_};

		if (this->is_list())
		{
			return *this;
		}

		if (!this->is_tuple())
		{
			return {*this->py_, nullptr};
		}

		auto list = this->py_->create_list();
		const auto length = this->get_length();

		for (size_t i = 0; i < length; ++i)
		{
			list.add_element(this->get_element(i));
		}

		return list;
	}

	python_object python_object::as_string() const
	{
		python_interpreter_lock lock{*this->py_};

		if (this->is_string())
		{
			return *this;
		}

		auto* str_obj = this->get_functions().object_str(this->value_);
		return {*this->py_, str_obj};
	}

	bool python_object::is_list() const
	{
		auto& f = this->get_functions();

		python_interpreter_lock lock{f};
		return f.object_is_instance(this->value_, f.list_type) != 0;
	}

	bool python_object::is_tuple() const
	{
		auto& f = this->get_functions();

		python_interpreter_lock lock{f};
		return f.object_is_instance(this->value_, f.tuple_type) != 0;
	}

	bool python_object::is_dict() const
	{
		auto& f = this->get_functions();

		python_interpreter_lock lock{f};
		return f.object_is_instance(this->value_, f.dict_type) != 0;
	}

	bool python_object::is_string() const
	{
		auto& f = this->get_functions();

		python_interpreter_lock lock{f};
		return f.object_is_instance(this->value_, f.unicode_type) != 0;
	}

	void python_object::set_attribute(const std::string& key, const python_object& value) const
	{
		auto& f = this->get_functions();

		python_interpreter_lock lock{f};
		f.object_set_attr_string(this->value_, key.c_str(), value.value_);
	}

	python_object python_object::get_attribute(const std::string& key) const
	{
		auto& f = this->get_functions();

		python_interpreter_lock lock{f};
		auto* obj = f.object_get_attr_string(this->value_, key.c_str());

		return {*this->py_, obj};
	}

	void python_object::delete_attribute(const std::string& key) const
	{
		this->set_attribute(key, this->py_->get_none());
	}

	bool python_object::has_attribute(const std::string& key) const
	{
		auto& f = this->get_functions();

		python_interpreter_lock lock{f};
		return f.object_has_attr_string(this->value_, key.c_str()) != 0;
	}

	python_object python_object::get_property_names() const
	{
		auto& f = this->get_functions();

		python_interpreter_lock lock{f};

		if (!this->is_dict())
		{
			return this->py_->get_none();
		}

		auto* obj = f.dict_keys(this->value_);

		return {*this->py_, obj};
	}

	void python_object::set_property(const std::string& key, const python_object& value) const
	{
		auto& f = this->get_functions();

		python_interpreter_lock lock{f};

		if (this->is_dict())
		{
			f.dict_set_item_string(this->value_, key.c_str(), value.value_);
		}
	}

	python_object python_object::get_property(const std::string& key) const
	{
		auto& f = this->get_functions();

		python_interpreter_lock lock{f};

		if (!this->is_dict())
		{
			return this->py_->get_none();
		}

		auto* obj = f.dict_get_item_string(this->value_, key.c_str());

		return {*this->py_, obj};
	}

	bool python_object::delete_property(const std::string& key) const
	{
		auto& f = this->get_functions();

		python_interpreter_lock lock{f};

		if (!this->is_dict())
		{
			return false;
		}

		return f.dict_del_item_string(this->value_, key.c_str()) != 0;
	}

	size_t python_object::get_length() const
	{
		Py_ssize_t size{0};

		auto& f = this->get_functions();
		python_interpreter_lock lock{f};

		if (this->is_tuple())
		{
			size = f.tuple_size(this->value_);
		}
		else if (this->is_list())
		{
			size = f.list_size(this->value_);
		}
		else if (this->is_dict())
		{
			return this->get_property_count();
		}

		if (size < 0)
		{
			return 0;
		}

		return static_cast<size_t>(size);
	}

	python_object python_object::get_element(const size_t index) const
	{
		auto& f = this->get_functions();
		python_interpreter_lock lock{f};

		PyObjectBorrowed* obj{nullptr};

		if (this->is_tuple())
		{
			obj = f.tuple_get_item(this->value_, static_cast<long long>(index));
		}
		else if (this->is_list())
		{
			obj = f.list_get_item(this->value_, static_cast<long long>(index));
		}

		return {*this->py_, obj};
	}

	void python_object::set_element(const size_t index, const python_object& value) const
	{
		auto& f = this->get_functions();
		python_interpreter_lock lock{f};

		if (this->is_tuple())
		{
			f.tuple_set_item(this->value_, static_cast<Py_ssize_t>(index), value.get_new_ref());
		}
		else if (this->is_list())
		{
			f.list_set_item(this->value_, static_cast<Py_ssize_t>(index), value.get_new_ref());
		}
	}

	void python_object::add_element(const python_object& value) const
	{
		auto& f = this->get_functions();
		python_interpreter_lock lock{f};

		if (!this->is_list())
		{
			return;
		}

		f.list_append(this->value_, value.value_);
	}

	std::map<std::string, python_object, std::less<>> python_object::get_properties() const
	{
		std::map<std::string, python_object, std::less<>> properties{};

		const auto property_names = this->get_property_names().get_elements();
		for (const auto& property : property_names)
		{
			auto name = property.as<std::string>();
			auto value = this->get_property(name);
			properties[std::move(name)] = std::move(value);
		}

		return properties;
	}

	std::vector<python_object> python_object::get_elements() const
	{
		python_interpreter_lock lock{*this->py_};

		const auto length = this->get_length();

		std::vector<python_object> elements{};
		elements.reserve(length);

		for (size_t i = 0; i < length; ++i)
		{
			elements.push_back(this->get_element(i));
		}

		return elements;
	}

	python_object python_object::call(const python_object& args, const python_object& kwargs) const
	{
		python_interpreter_lock lock{*this->py_};

		auto* obj = this->get_functions().call_object(this->value_, args.value_, kwargs.value_);
		python_object result{*this->py_, obj};

		this->py_->check_error();

		return result;
	}

	python_object python_object::call(const std::vector<python_object>& arguments) const
	{
		python_interpreter_lock lock{*this->py_};
		const auto args = this->py_->create_tuple(arguments);
		return this->call(args, this->py_->create_dict());
	}

	template <>
	bool python_object::as<bool>() const
	{
		auto& f = this->py_->get_function_interface();

		python_interpreter_lock lock{f};
		return f.object_is_true(this->value_) != 0;
	}

	template <>
	void* python_object::as<void*>() const
	{
		auto& f = this->py_->get_function_interface();

		python_interpreter_lock lock{f};
		return f.long_as_void_ptr(this->value_);
	}

	template <>
	int64_t python_object::as<int64_t>() const
	{
		auto& f = this->py_->get_function_interface();

		python_interpreter_lock lock{f};
		return f.long_as_long_long(this->value_);
	}

	template <>
	uint64_t python_object::as<uint64_t>() const
	{
		auto& f = this->py_->get_function_interface();

		python_interpreter_lock lock{f};
		return f.long_as_unsigned_long_long(this->value_);
	}

	template <>
	double python_object::as<double>() const
	{
		auto& f = this->py_->get_function_interface();

		python_interpreter_lock lock{f};
		return f.float_as_double(this->value_);
	}

	template <>
	std::string python_object::as<std::string>() const
	{
		auto& f = this->py_->get_function_interface();

		python_interpreter_lock lock{f};

		if (!this->is_string())
		{
			return this->as_string().as<std::string>();
		}

		Py_ssize_t length{};

		const auto* val = f.unicode_as_utf8_and_size(this->value_, &length);
		if (!val || length <= 0)
		{
			return {};
		}

		return {val, static_cast<size_t>(length)};
	}
}
