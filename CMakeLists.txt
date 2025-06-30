cmake_minimum_required(VERSION 3.31.7)
project(hyprland-workspace-indicator
	VERSION 0.9.1
	DESCRIPTION "A multi-monitor workspace indicator JSON output for use with EWW and Hyprland."
	HOMEPAGE_URL "https://github.com/Jayden876212/hyprland-workspace-indicator"
	LANGUAGES C
)

# We need to get the `compile_commands.json` and copy it to the project root
# to configure CI hooks like `clang-tidy-review`.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
execute_process(COMMAND cp compile_commands.json ../compile_commands.json)
execute_process(COMMAND sed -i "s@/build@@" ../compile_commands.json)

include_directories(include)
find_package(cJSON REQUIRED)
add_executable(hyprland-workspaces
	src/main.c
	src/helpers/array_handling.c
	src/helpers/json_handling.c
	src/helpers/hyprland_struct_handling.c
	src/utils/bit_handling.c
	src/utils/event_handling.c
	src/utils/hyprland_socket_handling.c
)
target_link_libraries(hyprland-workspaces cjson)
install(TARGETS hyprland-workspaces)
