#include <momo/python_interface.hpp>
#include <momo/export.h>

using namespace momo::python;

python_object print_string(python_interface& py, const python_object& args, const python_object&)
{
	if (args.get_length() != 1)
	{
		return false;
	}

	puts(args.get_element(0).as<std::string>().c_str());

	py.execute("print('Hello World 2')");

	return true;
}

python_object run_callback(const python_object& args)
{
	if (args.get_length() != 1)
	{
		return false;
	}

	const python_object callback = args[0];
	callback("Hello from C++");

	return true;
}

MOMO_DLL_EXPORT_C
void setup_python(PyObjectBorrowed* raw_dict)
{
	auto& py = python_interface::get();
	const python_object dict{py, raw_dict};

	dict["print_string"] = py.create_function(print_string);
	dict["run_callback"] = py.create_function(run_callback);
}
