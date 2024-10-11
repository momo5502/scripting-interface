#pragma once

#include <vector>

namespace momo
{
	using native_handle = void*;
	class library;

	library open_library(const char* name);
	library load_library(const char* name);
	void free_library(native_handle handle);

	void* get_library_symbol(native_handle handle, const char* symbol);
	std::vector<library> get_loaded_libraries();

	class library
	{
	public:
		library() = default;

		library(const native_handle h, const bool own)
			: handle_(h)
			  , owns_handle_(own)
		{
		}

		library(library&& obj) noexcept
			: library()
		{
			this->operator=(std::move(obj));
		}

		library& operator=(library&& obj) noexcept
		{
			if (this != &obj)
			{
				this->release();

				this->owns_handle_ = obj.owns_handle_;
				this->handle_ = obj.handle_;

				obj.owns_handle_ = false;
				obj.handle_ = nullptr;
			}

			return *this;
		}

		library(const library& obj)
			: library()
		{
			this->operator=(obj);
		}

		library& operator=(const library& obj)
		{
			if (this != &obj)
			{
				this->release();

				this->owns_handle_ = false;
				this->handle_ = obj.handle_;
			}

			return *this;
		}

		~library()
		{
			this->release();
		}

		operator bool() const
		{
			return this->handle_;
		}

		template <typename T = void>
		T* get_symbol(const char* symbol) const
		{
			return static_cast<T*>(get_library_symbol(this->handle_, symbol));
		}

		native_handle get_native_handle() const
		{
			return this->handle_;
		}

	private:
		native_handle handle_{nullptr};
		bool owns_handle_{false};

		void release()
		{
			if (this->handle_ && this->owns_handle_)
			{
				free_library(this->handle_);
			}

			this->handle_ = nullptr;
			this->owns_handle_ = false;
		}
	};
}
