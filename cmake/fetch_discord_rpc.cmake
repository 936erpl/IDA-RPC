# Use a zip download approach to avoid FetchContent cmake_minimum_required issues with CMake 4.x
# We download, patch (add buttons), and add_subdirectory ourselves.
set(DISCORD_RPC_URL "https://github.com/discord/discord-rpc/archive/refs/tags/v3.4.0.zip")
set(DISCORD_RPC_DIR "${CMAKE_BINARY_DIR}/_deps/discord-rpc-src")

if(NOT EXISTS "${DISCORD_RPC_DIR}/include/discord_rpc.h")
    message(STATUS "Downloading discord-rpc v3.4.0...")
    file(DOWNLOAD "${DISCORD_RPC_URL}" "${CMAKE_BINARY_DIR}/_deps/discord-rpc.zip")
    file(ARCHIVE_EXTRACT INPUT "${CMAKE_BINARY_DIR}/_deps/discord-rpc.zip" DESTINATION "${CMAKE_BINARY_DIR}/_deps/")
    file(RENAME "${CMAKE_BINARY_DIR}/_deps/discord-rpc-3.4.0" "${DISCORD_RPC_DIR}")
    file(REMOVE "${CMAKE_BINARY_DIR}/_deps/discord-rpc.zip")

    # patch header: add button fields to DiscordRichPresence
    file(READ "${DISCORD_RPC_DIR}/include/discord_rpc.h" _rpc_h)
    string(REPLACE
        "const char* smallImageText; /* max 128 bytes */"
        "const char* smallImageText; /* max 128 bytes */\n    const char* button1_label;\n    const char* button1_url;\n    const char* button2_label;\n    const char* button2_url;"
        _rpc_h "${_rpc_h}")
    file(WRITE "${DISCORD_RPC_DIR}/include/discord_rpc.h" "${_rpc_h}")

    # patch serialization: add button JSON output after assets, before party
    file(READ "${DISCORD_RPC_DIR}/src/serialization.cpp" _ser)
    string(REPLACE
        "WriteOptionalString(writer, \"small_text\", presence->smallImageText);\n                }\n\n                if ((presence->partyId"
        "WriteOptionalString(writer, \"small_text\", presence->smallImageText);\n                }\n\n                if ((presence->button1_label && presence->button1_label[0]) ||\n                    (presence->button2_label && presence->button2_label[0])) {\n                    WriteArray buttons(writer, \"buttons\");\n                    if (presence->button1_label && presence->button1_label[0]) {\n                        writer.StartObject();\n                        WriteOptionalString(writer, \"label\", presence->button1_label);\n                        WriteOptionalString(writer, \"url\", presence->button1_url);\n                        writer.EndObject();\n                    }\n                    if (presence->button2_label && presence->button2_label[0]) {\n                        writer.StartObject();\n                        WriteOptionalString(writer, \"label\", presence->button2_label);\n                        WriteOptionalString(writer, \"url\", presence->button2_url);\n                        writer.EndObject();\n                    }\n                }\n\n                if ((presence->partyId"
        _ser "${_ser}")
    file(WRITE "${DISCORD_RPC_DIR}/src/serialization.cpp" "${_ser}")

    # fix cmake_minimum_required for CMake 4.x
    file(READ "${DISCORD_RPC_DIR}/CMakeLists.txt" _cmake_rpc)
    string(REPLACE "cmake_minimum_required (VERSION 3.2.0)"
        "cmake_minimum_required (VERSION 3.5)" _cmake_rpc "${_cmake_rpc}")
    file(WRITE "${DISCORD_RPC_DIR}/CMakeLists.txt" "${_cmake_rpc}")

    message(STATUS "discord-rpc patched with button support")
endif()

add_subdirectory(${DISCORD_RPC_DIR} ${CMAKE_BINARY_DIR}/_deps/discord-rpc-build EXCLUDE_FROM_ALL)