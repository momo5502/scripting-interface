add_library(python
    python_interface.cpp
    python_functions.hpp
    python_library.hpp
    python_library.cpp
)

target_link_libraries(python PRIVATE library)
target_include_directories(python PUBLIC "${CMAKE_CURRENT_LIST_DIR}/../../include/python/")
