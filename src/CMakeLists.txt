add_library(mizan)
add_library(Mizan::Mizan ALIAS mizan)

target_sources(mizan
  PRIVATE
    core/error.cpp
    core/handle.cpp
    core/config.cpp
)

target_include_directories(mizan
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<INSTALL_INTERFACE:include>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(mizan PUBLIC cxx_std_20)