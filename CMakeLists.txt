# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
set (EXTRA_COMPONENT_DIRS "${CMAKE_CURRENT_BINARY_DIR}/../components/esp-azure/port")
#set(EXTRA_COMPONENT_DIRS "${EXTRA_COMPONENT_DIRS} $ENV{ESP_AZURE}/port")
project(smagribot-app)