cmake_minimum_required(VERSION 3.5)

# Set extension name here
set(TARGET_NAME ducklake)

set(EXTENSION_NAME ${TARGET_NAME}_extension)
set(LOADABLE_EXTENSION_NAME ${TARGET_NAME}_loadable_extension)

project(${TARGET_NAME})
include_directories(src/include)

add_subdirectory(src)
set(EXTENSION_SOURCES ${ALL_OBJECT_FILES})

build_static_extension(${TARGET_NAME} ${EXTENSION_SOURCES})
build_loadable_extension(${TARGET_NAME} " " ${EXTENSION_SOURCES})


# Check build type and define preprocessor macros
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  message(STATUS "NDEBUG set") 
  target_compile_definitions(${EXTENSION_NAME} PUBLIC -DNDEBUG) 
endif()

install(
  TARGETS ${EXTENSION_NAME}
  EXPORT "${DUCKDB_EXPORT_SET}"
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}"
  ARCHIVE DESTINATION "${INSTALL_LIB_DIR}")
