add_executable(library
    library.cpp
    library.hpp
    test.cpp
)

if(WIN32)
target_link_libraries(library PRIVATE Psapi.lib)
elseif(UNIX)
target_link_libraries(library PRIVATE -ldl)
endif()

#target_include_directories(html-ui PUBLIC "${CMAKE_CURRENT_LIST_DIR}/../include")