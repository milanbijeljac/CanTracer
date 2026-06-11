add_library(Core
    Source/Core/SerialPort.cpp
    Source/Core/CanFrameParser.cpp
)

target_include_directories(Core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

target_link_libraries(Core PUBLIC
    Qt6::Core
    Qt6::SerialPort
)
