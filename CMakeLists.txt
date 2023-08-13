find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(Poco REQUIRED COMPONENTS Foundation Net Util)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/deps)

add_definitions(-DUNICODE -D_UNICODE -DMT)
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

aux_source_directory(. COMMON_SRCS)

add_library(common ${COMMON_SRCS})
target_link_libraries(common ${Boost_LIBRARIES}) 
target_link_libraries(common fmt::fmt)
target_link_libraries(common spdlog::spdlog_header_only)
target_link_libraries(common Poco::Foundation Poco::Util Poco::Net)
add_custom_command(TARGET common POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/log.conf"  "${CMAKE_BINARY_DIR}"
)