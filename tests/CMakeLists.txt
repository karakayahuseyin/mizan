# Copyright (c) 2025 Hüseyin Karakaya
# This file is a part of the Mizan project
# Licensed under the MIT License.

include(FetchContent)
FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG v2.4.11
)
FetchContent_MakeAvailable(doctest)

add_executable(mizan_tests main.cpp)
target_link_libraries(mizan_tests PRIVATE Mizan::Mizan doctest::doctest)

add_test(NAME mizan_tests COMMAND mizan_tests)