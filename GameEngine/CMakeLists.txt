cmake_minimum_required(VERSION 3.8)

set(PROJECT_NAME GameEngine)
set (CMAKE_CXX_STANDARD 20)
set(ENV{ENGINE_BUILD_TYPE} Debug)
project(${PROJECT_NAME})

#Copying assets
file(COPY ${CMAKE_SOURCE_DIR}/Engine/Assets
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Engine)
file(COPY ${CMAKE_SOURCE_DIR}/Game/Assets
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Game)
file(COPY ${CMAKE_SOURCE_DIR}/imgui.ini
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)

#Source files
file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/Engine/*.cpp ${CMAKE_SOURCE_DIR}/Engine/*.h ${CMAKE_SOURCE_DIR}/Engine/*.hlsl)
source_group(TREE ../GameEngine PREFIX GameEngine FILES ${SOURCES}) 

#Excluding shaders from build
file (GLOB_RECURSE SHADERS ${CMAKE_SOURCE_DIR}/Engine/Assets/Shaders/*.hlsl)
set_property(SOURCE ${SHADERS} PROPERTY VS_SETTINGS "ExcludedFromBuild=true")

#ImGui
file(GLOB_RECURSE SOURCES_IMGUI ${CMAKE_SOURCE_DIR}/vendor/ImGui/*.cpp)
file(GLOB_RECURSE SOURCES_IMGUI_EXAMPLES ${CMAKE_SOURCE_DIR}/vendor/ImGui/examples/*.cpp)
set_property(SOURCE ${SOURCES_IMGUI_EXAMPLES} PROPERTY VS_SETTINGS "ExcludedFromBuild=true")

#ImGuizmo
file(GLOB_RECURSE SOURCES_IMGUIZMO ${CMAKE_SOURCE_DIR}/vendor/ImGuizmo/*.cpp ${CMAKE_SOURCE_DIR}/vendor/ImGuizmo/*.h)
file(GLOB_RECURSE SOURCES_IMGUIZMO_EXAMPLE ${CMAKE_SOURCE_DIR}/vendor/ImGuizmo/example/*.cpp)
set_property(SOURCE ${SOURCES_IMGUIZMO_EXAMPLE} PROPERTY VS_SETTINGS "ExcludedFromBuild=true")

#Entt
file(GLOB_RECURSE SOURCES_ENTT ${CMAKE_SOURCE_DIR}/vendor/entt/entt.hpp)


#file(GLOB_RECURSE SOURCES_ASIMP ${CMAKE_SOURCE_DIR}/vendor/assimp/*.h ${CMAKE_SOURCE_DIR}/vendor/assimp/*.hpp ${CMAKE_SOURCE_DIR}/vendor/assimp/*.cpp ${CMAKE_SOURCE_DIR}/vendor/assimp/*.inl)

file(GLOB ANOTHER_SRCS ${CMAKE_SOURCE_DIR}/vendor/*.h ${CMAKE_SOURCE_DIR}/vendor/*.cpp ${CMAKE_SOURCE_DIR}/vendor/*.inl)




add_executable(${PROJECT_NAME} ${SOURCES} ${SOURCES_ENTT} ${SOURCES_IMGUI} ${SOURCES_IMGUIZMO} ${ANOTHER_SRCS})


target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/vendor/)

add_subdirectory(${CMAKE_SOURCE_DIR}/vendor/DirectXTK ${CMAKE_BINARY_DIR}/DirectXTK)
target_link_libraries(${PROJECT_NAME} PRIVATE DirectXTK)

add_subdirectory(${CMAKE_SOURCE_DIR}/vendor/assimp ${CMAKE_BINARY_DIR}/assimp)
target_link_libraries(${PROJECT_NAME} PRIVATE assimp)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   ${CMAKE_BINARY_DIR}/assimp/bin/$ENV{ENGINE_BUILD_TYPE}/assimp-vc143-mtd.dll
                   $<TARGET_FILE_DIR:${PROJECT_NAME}>/assimp-vc143-mtd.dll)
				   
#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#                   COMMAND ${CMAKE_COMMAND} -E copy
#                   ${CMAKE_SOURCE_DIR}/Engine/Core/Rendering/Shaders/
#                   $<TARGET_FILE_DIR:${PROJECT_NAME}>/Engine/Core/Rendering/Shaders/)