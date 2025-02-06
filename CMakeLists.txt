cmake_minimum_required(VERSION 3.12)
project(ens CXX)


# Adding userver dependency
find_package(userver COMPONENTS core postgresql chaotic QUIET)
if(NOT userver_FOUND)  # Fallback to subdirectory usage
    # Enable userver libraries that are needed in this project
    set(USERVER_FEATURE_POSTGRESQL ON CACHE BOOL "" FORCE)
    set(USERVER_FEATURE_CHAOTIC ON CACHE BOOL "" FORCE)

    # Compatibility mode: some systems don't support these features
    set(USERVER_FEATURE_CRYPTOPP_BLAKE2 OFF CACHE BOOL "" FORCE)
    set(USERVER_FEATURE_GRPC_CHANNELZ OFF CACHE BOOL "" FORCE)
    set(USERVER_FEATURE_REDIS_HI_MALLOC ON CACHE BOOL "" FORCE)

    if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/userver)
        message(STATUS "Using userver framework from third_party/userver")
        add_subdirectory(third_party/userver)
    else()
        message(FATAL_ERROR "Either install the userver or provide a path to it")
    endif()
endif()

userver_setup_environment()


# Common sources
include_directories(src)
add_library(${PROJECT_NAME}_objs OBJECT
        src/schemas/schemas.cpp
        src/schemas/schemas.hpp
        src/schemas/schemas_fwd.hpp
        src/schemas/schemas_parsers.ipp
        src/user/user.hpp
        src/user/user.cpp
        src/user/auth.cpp
        src/user/auth.hpp
        src/user/handlers.cpp
        src/user/handlers.hpp
        src/recipients/recipients.cpp
        src/recipients/recipients.hpp
        src/recipients/handlers.cpp
        src/recipients/handlers.hpp
        src/templates/templates.cpp
        src/templates/templates.hpp
        src/templates/handlers.cpp
        src/templates/handlers.hpp
        src/groups/groups.cpp
        src/groups/groups.hpp
        src/groups/handlers.cpp
        src/groups/handlers.hpp
        src/utils/utils.cpp
        src/utils/utils.hpp
        src/notifications/telegram/telegram_bot.cpp
        src/notifications/telegram/telegram_bot.hpp
        src/notifications/notifications.cpp
        src/notifications/notifications.hpp
        src/notifications/handlers.cpp
        src/notifications/handlers.hpp
)
target_link_libraries(${PROJECT_NAME}_objs PUBLIC userver::postgresql)


# Third-party
set(JWT_BUILD_EXAMPLES OFF)
add_subdirectory(third_party/jwt-cpp)
target_include_directories(${PROJECT_NAME}_objs PUBLIC jwt-cpp)
target_link_libraries(${PROJECT_NAME}_objs PUBLIC jwt-cpp)
add_subdirectory(third_party/haufont-telegram)
target_include_directories(${PROJECT_NAME}_objs PUBLIC userver-telegram)
target_link_libraries(${PROJECT_NAME}_objs PUBLIC userver-telegram)


# The Service
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_objs)


# Functional Tests
userver_testsuite_add(
        PYTHONPATH "${CMAKE_CURRENT_SOURCE_DIR}/tests"
        SERVICE_TARGET ${PROJECT_NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests
        PYTHON_BINARY ${TESTSUITE_PYTHON_BINARY}
        PYTEST_ARGS
        --service-config=${CMAKE_CURRENT_SOURCE_DIR}/configs/static_config.yaml
        --service-binary=${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}
        --service-config-vars=${CMAKE_CURRENT_SOURCE_DIR}/configs/config_vars.testing.yaml
)


# Install
include(GNUInstallDirs)

if(DEFINED ENV{PREFIX})
  message(STATUS "Set install prefix: $ENV{PREFIX}")
  file(TO_CMAKE_PATH "$ENV{PREFIX}" PREFIX_PATH)
  set(CMAKE_INSTALL_PREFIX ${PREFIX_PATH})
endif()

file(GLOB CONFIGS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.yaml ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.json)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ${PROJECT_NAME})
install(FILES ${CONFIGS_FILES} DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME} COMPONENT ${PROJECT_NAME})


# Chaotic codegen
target_link_libraries(${PROJECT_NAME}_objs PUBLIC userver::chaotic)
#file(GLOB_RECURSE SCHEMAS ${CMAKE_CURRENT_SOURCE_DIR}/swagger/schemas.yaml)
#userver_target_generate_chaotic(${PROJECT_NAME}-chgen
#        ARGS
#        # Map '/components/schemas/*' JSONSchema types to C++ types in 'samples::hello' namespace
#        -n "/components/schemas/([^/]*)/=schemas::{0}"
#        -f "(.*)={0}"
#        # Don't call clang-format
#        --clang-format=
#        # Generate serializers for responses
#        --generate-serializers
#        OUTPUT_DIR
#        ${CMAKE_CURRENT_BINARY_DIR}/src
#        SCHEMAS
#        ${SCHEMAS}
#        RELATIVE_TO
#        ${CMAKE_CURRENT_SOURCE_DIR}
#)
#target_link_libraries(${PROJECT_NAME}_objs PUBLIC ${PROJECT_NAME}-chgen)
