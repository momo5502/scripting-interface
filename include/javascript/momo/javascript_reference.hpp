#pragma once

#include "napi_types.hpp"

namespace momo::javascript
{
	class javascript_value;
	class javascript_interface;

	// Persistency wrapper for javascript values

	class javascript_reference
	{
	public:
		javascript_reference() = default;

		javascript_reference(const javascript_value& value);
		javascript_reference(javascript_interface& js, napi_value value);

		javascript_reference(const javascript_reference& obj);
		javascript_reference(javascript_reference&& obj) noexcept;

		javascript_reference& operator=(const javascript_reference& obj);
		javascript_reference& operator=(javascript_reference&& obj) noexcept;

		~javascript_reference()
		{
			this->release();
		}

		uint32_t get_ref_count() const
		{
			this->add_ref();
			return this->unref();
		}

		uint32_t add_ref() const;
		uint32_t release();

		javascript_value get_value() const;

		operator javascript_value() const
		{
			return this->get_value();
		}

		napi_ref get() const
		{
			return this->ref_;
		}

	private:
		napi_ref ref_{};
		javascript_interface* js_{};

		uint32_t unref() const;
	};
}
