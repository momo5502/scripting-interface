add_library(library
    library.cpp
    library.hpp
    symbol.hpp
)

if(WIN32)
    target_link_libraries(library PRIVATE Psapi.lib)
elseif(UNIX)
    target_link_libraries(library PRIVATE -ldl)
endif()

target_include_directories(library INTERFACE "${CMAKE_CURRENT_LIST_DIR}")
