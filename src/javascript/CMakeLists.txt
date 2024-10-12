add_library(javascript
    javascript_interface.cpp
    javascript_library.cpp
    javascript_library.hpp
    javascript_value.cpp
    javascript_functions.hpp
    scoped_interface.cpp
    scoped_interface.hpp
)

target_link_libraries(javascript PRIVATE library)
target_include_directories(javascript PUBLIC "${CMAKE_CURRENT_LIST_DIR}/../../include/javascript/")
