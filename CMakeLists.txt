cmake_minimum_required(VERSION 3.10)
project(todo-bbs VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
        main.cpp
        boxes.cpp
)

# Executable
add_executable(todo-bbs ${SOURCES})

# Installation
install(TARGETS todo-bbs
        RUNTIME DESTINATION bin
        COMPONENT runtime
)

# Install documentation
install(FILES README.md
        DESTINATION share/doc/${PROJECT_NAME}
        COMPONENT documentation
)

# CPack configuration for creating release tarballs
set(CPACK_PACKAGE_NAME "todo-bbs")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A retro-styled BBS-inspired TODO list manager")
set(CPACK_GENERATOR "TGZ")
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_IGNORE_FILES
        "/\\.git/"
        "/build/"
        "\\.swp$"
        "\\.orig$"
        "/CMakeLists\\.txt\\.user$"
)

include(CPack)