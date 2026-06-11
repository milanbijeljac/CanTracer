add_executable(CanTracer
    Source/App.cpp
    Source/MainWindow.cpp
    Source/CanTableModel.cpp
)

target_include_directories(CanTracer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

target_link_libraries(CanTracer PRIVATE
    Core
    Qt6::Widgets
)
