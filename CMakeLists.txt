# CMakeList.txt : CMake project for Antilatency.HardwareExtensionInterfaceStarterBoardDemoCpp, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Antilatency.HardwareExtensionInterfaceStarterBoardDemoCpp")

include_directories(${CMAKE_SOURCE_DIR}/AntilatencySDK/Api)

# Add source to this project's executable.
add_executable (Antilatency.HardwareExtensionInterfaceStarterBoardDemoCpp "main.cpp")

if (WIN32)
    file(GLOB ANTILATENCY_LIBS ${CMAKE_SOURCE_DIR}/AntilatencySdk/Bin/WindowsDesktop/x64/*.dll)
endif (WIN32)
if (UNIX)
    target_link_libraries(Antilatency.HardwareExtensionInterfaceStarterBoardDemoCpp dl stdc++fs pthread)

    EXECUTE_PROCESS( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE )
    message( STATUS "Architecture: ${ARCHITECTURE}" )

    if( ${ARCHITECTURE} STREQUAL "aarch64" )
        file(GLOB ANTILATENCY_LIBS ${CMAKE_SOURCE_DIR}/AntilatencySdk/Bin/Linux/aarch64_linux_gnu/*.so)
    elseif( ${ARCHITECTURE} STREQUAL "armv7l" )
        file(GLOB ANTILATENCY_LIBS ${CMAKE_SOURCE_DIR}/AntilatencySdk/Bin/Linux/arm_linux_gnueabihf/*.so)
    elseif( ${ARCHITECTURE} STREQUAL "x86_64" )
        file(GLOB ANTILATENCY_LIBS ${CMAKE_SOURCE_DIR}/AntilatencySdk/Bin/Linux/x86_64/*.so)
    endif()

endif (UNIX)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Antilatency.HardwareExtensionInterfaceStarterBoardDemoCpp PROPERTY CXX_STANDARD 20)
endif()

add_custom_command(TARGET Antilatency.HardwareExtensionInterfaceStarterBoardDemoCpp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ANTILATENCY_LIBS} $<TARGET_FILE_DIR:Antilatency.HardwareExtensionInterfaceStarterBoardDemoCpp>)
# TODO: Add tests and install targets if needed.
