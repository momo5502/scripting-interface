#include <momo/python_interface.hpp>
#include <momo/export.h>

using namespace momo::python;

python_object print_string(const python_object& args)
{
	if (args.get_length() != 1)
	{
		return false;
	}

	puts(args.get_element(0).as_string().as<std::string>().c_str());
	return true;
}

MOMO_DLL_EXPORT_C
void setup_python(PyObjectBorrowed* raw_dict)
{
	auto& py = python_interface::get();
	const python_object dict{py, raw_dict };

	dict.set_property("print_string", py.create_function(print_string));
}
