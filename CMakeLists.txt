cmake_minimum_required(VERSION 3.28.3)
project(hyprland-workspace-indicator
	VERSION 0.9.1
	DESCRIPTION "A multi-monitor workspace indicator JSON output for use with EWW and Hyprland."
	HOMEPAGE_URL "https://github.com/Jayden876212/hyprland-workspace-indicator"
	LANGUAGES C
)

# We need to get the `compile_commands.json` to configure CI hooks like
# `clang-tidy-review` as well as language servers like clangd.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
