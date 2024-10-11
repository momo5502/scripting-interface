add_library(javascript
    javascript_interface.cpp
    javascript_library.cpp
    javascript_library.hpp
)

target_link_libraries(javascript PRIVATE library)
target_include_directories(javascript PUBLIC "${CMAKE_CURRENT_LIST_DIR}/../../include/javascript/")
