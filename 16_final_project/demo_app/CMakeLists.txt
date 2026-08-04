find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(CMAKE_AUTOMOC ON)

add_executable(demo_app
    main.cpp
    mainwindow.cpp
    mainwindow.h
)

target_link_libraries(demo_app PRIVATE Qt6::Widgets license_core)
