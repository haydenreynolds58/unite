include(FetchContent)
# ENet 1.3.18, pinned to the release commit.
FetchContent_Declare(enet
    GIT_REPOSITORY https://github.com/lsalzman/enet.git
    GIT_TAG 2662c0de09e36f2a2030ccc2c528a3e4c9e8138a
)
FetchContent_MakeAvailable(enet)
add_executable(unite_boss main.cpp)
target_include_directories(unite_boss PRIVATE ${enet_SOURCE_DIR}/include)
target_link_libraries(unite_boss PRIVATE raylib enet)
if(WIN32)
    target_link_libraries(unite_boss PRIVATE winmm ws2_32)
    target_compile_definitions(unite_boss PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX NOGDI NOUSER)
endif()
if(MSVC)
    target_compile_options(unite_boss PRIVATE /W4)
else()
    target_compile_options(unite_boss PRIVATE -Wall -Wextra -Wpedantic)
endif()
add_executable(boss_tests tests.cpp)
add_test(NAME boss_simulation_and_protocol COMMAND boss_tests)
