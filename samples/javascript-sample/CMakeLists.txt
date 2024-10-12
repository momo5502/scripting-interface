add_library(javascript-sample SHARED
    main.cpp
) 

target_link_libraries(javascript-sample PRIVATE javascript)
