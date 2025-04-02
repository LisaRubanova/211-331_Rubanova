cmake_minimum_required(VERSION 3.16)

project(fake_debugger LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(fake_debugger main.cpp)

include(GNUInstallDirs)
install(TARGETS fake_debugger
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
