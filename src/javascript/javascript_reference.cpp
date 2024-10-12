#include <momo/javascript_interface.hpp>
#include <momo/javascript_reference.hpp>
#include <momo/javascript_value.hpp>

#include "javascript_functions.hpp"

namespace momo::javascript
{
	javascript_reference::javascript_reference(const javascript_value& value)
		: javascript_reference(value.get_interface(), value.get())
	{
	}

	javascript_reference::javascript_reference(javascript_interface& js, const napi_value value)
		: js_(&js)
	{
		js.get_function_interface().create_reference(js.get_env(), value, 1, &this->ref_);
	}

	javascript_reference::javascript_reference(const javascript_reference& obj)
		: javascript_reference()
	{
		this->operator=(obj);
	}

	javascript_reference::javascript_reference(javascript_reference&& obj) noexcept
		: javascript_reference()
	{
		this->operator=(std::move(obj));
	}

	javascript_reference& javascript_reference::operator=(const javascript_reference& obj)
	{
		if (this != &obj)
		{
			this->release();
			obj.add_ref();

			this->ref_ = obj.ref_;
			this->js_ = obj.js_;
		}

		return *this;
	}

	javascript_reference& javascript_reference::operator=(javascript_reference&& obj) noexcept
	{
		if (this != &obj)
		{
			this->release();

			this->ref_ = obj.ref_;
			this->js_ = obj.js_;

			obj.ref_ = nullptr;
		}

		return *this;
	}

	uint32_t javascript_reference::add_ref() const
	{
		if (!this->ref_)
		{
			return 0;
		}

		uint32_t count{};
		this->js_->get_function_interface().reference_ref(this->js_->get_env(), this->ref_, &count);
		return count;
	}

	uint32_t javascript_reference::unref() const
	{
		if (!this->ref_)
		{
			return 0;
		}

		uint32_t count{};
		this->js_->get_function_interface().reference_unref(this->js_->get_env(), this->ref_, &count);
		return count;
	}

	uint32_t javascript_reference::release()
	{
		if (!this->ref_)
		{
			return 0;
		}

		const auto count = this->unref();
		if (count == 0)
		{
			this->js_->get_function_interface().delete_reference(this->js_->get_env(), this->ref_);
			this->ref_ = nullptr;
		}

		return count;
	}

	javascript_value javascript_reference::get_value() const
	{
		napi_value value{};
		this->js_->get_function_interface().get_reference_value(this->js_->get_env(), this->ref_, &value);
		return {*this->js_, value};
	}
}
