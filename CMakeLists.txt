﻿# CMakeList.txt: проект CMake для Chat_Cross_platform; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.8)

# Включение горячей перезагрузки для компиляторов MSVC, если поддерживается.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Chat_Server")

# Добавьте источник в исполняемый файл этого проекта.
add_executable (Chat_Server "Chat_Server.cpp" "Chat_Server.h" "Hash.h" "Hash.cpp" "Message.h" "Message.cpp" "User.h" "User.cpp"  "Chat.h" "Chat.cpp" "Get.h" "Get.cpp" "Logger.h" "Logger.cpp")
target_link_libraries(Chat_Server mysqlclient)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Chat_Server PROPERTY CXX_STANDARD 20)
endif()

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
