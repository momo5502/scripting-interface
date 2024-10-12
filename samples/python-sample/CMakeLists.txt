add_library(python-sample SHARED
    main.cpp
) 

target_link_libraries(python-sample PRIVATE python)
