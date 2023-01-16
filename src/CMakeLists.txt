
add_executable(${PROJECT_NAME} WIN32 "win_platform.cpp" "renderer.cpp" "app.cpp" "texture_manager.cpp" "settings_parser.cpp")
target_link_options(${PROJECT_NAME} PUBLIC -static -static-libgcc -static-libstdc++ -pthread)