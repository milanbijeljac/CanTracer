add_library(Core
    Source/Core/Core.cpp
    Source/Core/Linux_UART.cpp
)

target_include_directories(Core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
)
