#===============================================================================
# @brief    cmake toolchains
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
macro(return_if_not_build)
    if(NOT ${COMPONENT_NAME} IN_LIST TARGET_COMPONENT)
        return()
    endif()
endmacro()

macro(add_subdirectory_if_exist _path)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${_path}/CMakeLists.txt)
        add_subdirectory(${_path})
    endif()
endmacro()

macro(add_subdirectory_with_alias_if_exist _path _path_alias)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${_path}/CMakeLists.txt)
        add_subdirectory(${_path} ${_path_alias})
    endif()
endmacro()

function(add_path_with_alias_if_exist _path _name)
    if(EXISTS ${_path}/CMakeLists.txt)
        add_subdirectory(${_path} ${_name})
    endif()
endfunction()

function(add_path_if_exist _path)
    if(EXISTS ${_path}/CMakeLists.txt)
        add_subdirectory(${_path})
    endif()
endfunction()

function(KCONFIG_GET_PARAMS KCONFIG_PATH)
    file(STRINGS ${KCONFIG_PATH} LOS_CONFIG_LIST ENCODING "UTF-8")

    foreach (CONFIG ${LOS_CONFIG_LIST})
        # CONFIG maybe: CONFIG_CC_STACKPROTECTOR_STRONG=y

        # The first: variable name
        string(REGEX MATCH "[^=]+" CONFIG_VAR_NAME ${CONFIG})

        # The second: variable value
        string(REGEX MATCH "=(.+$)" CONFIG_VAR_VAL ${CONFIG})
        set(CONFIG_VAR_VAL ${CMAKE_MATCH_1})
        if(CONFIG_VAR_VAL MATCHES "^y$" OR CONFIG_VAR_VAL MATCHES "^\".*\"$")
            string(REPLACE "\"" "" CONFIG_VAR_VAL ${CONFIG_VAR_VAL})
            set(${CONFIG_VAR_NAME} ${CONFIG_VAR_VAL} PARENT_SCOPE)
        endif()
    endforeach()
endfunction()

macro(check_if_closed_component component_name)
    set(COMPONENT_IS_CLOSED false)
    if("${component_name}" IN_LIST CLOSED_COMPONENTS)
        set(COMPONENT_IS_CLOSED true)
    endif()
    if(DEFINED OPEN_COMPONENTS AND NOT ("${component_name}" IN_LIST OPEN_COMPONENTS))
        set(COMPONENT_IS_CLOSED true)
    endif()
endmacro()

macro(add_glob_include_path INCLUDE_PATHS)
    get_property(GLOB_TARGET_INCLUDE GLOBAL PROPERTY TARGET_INCLUDE)
    list(APPEND GLOB_TARGET_INCLUDE "${INCLUDE_PATHS}")
    list(REMOVE_DUPLICATES GLOB_TARGET_INCLUDE)
    set_property(GLOBAL PROPERTY TARGET_INCLUDE ${GLOB_TARGET_INCLUDE})
endmacro(add_glob_include_path)

macro(add_fsm_msgdef_headers MSGDEF_HEADERS)
    get_property(GLOB_MSGDEF_HEADERS GLOBAL PROPERTY TARGET_MSGDEF_HEADERS)
    list(APPEND GLOB_MSGDEF_HEADERS "${MSGDEF_HEADERS}")
    list(REMOVE_DUPLICATES GLOB_MSGDEF_HEADERS)
    set_property(GLOBAL PROPERTY TARGET_MSGDEF_HEADERS ${GLOB_MSGDEF_HEADERS})
endmacro()

macro(ut_autogen_fsms FSM_FILES AUTOGEN_SOURCES AUTOGEN_INCLUDE_PATH)
    set(FSM_AUTOGEN_INCLUDE_PATH)
    foreach(FILE_PATH ${FSM_FILES})
        if(EXISTS ${CMAKE_SOURCE_DIR}/../../../../../../protocol/cat1/lte/${FILE_PATH})
            get_filename_component(FILE_REL_PATH ${FILE_PATH} ABSOLUTE BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
            get_filename_component(FILE_DIRECTORY ${FILE_REL_PATH} DIRECTORY)
            string(REGEX MATCH "${CMAKE_CURRENT_SOURCE_DIR}/" MATCH_RES ${FILE_DIRECTORY})
            if(MATCH_RES STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}/")
                set(AUTOGEN_PATH ${CMAKE_CURRENT_SOURCE_DIR}/stub/fsm_autogen)
                file(MAKE_DIRECTORY ${AUTOGEN_PATH})
                file(RELATIVE_PATH FILE_RELATIVE_PATH ${CMAKE_SOURCE_DIR} ${FILE_REL_PATH})
                set(FSM_FILE_ABSOLUTE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../protocol/cat1/lte/${FSM_FILES})
                execute_process(
                    COMMAND python ${CMAKE_SOURCE_DIR}/../../../../../../tools/fsmgen/fsmgen.py ${FSM_FILE_ABSOLUTE_PATH} --out_dir ${AUTOGEN_PATH}
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                    )
                list(APPEND FSM_AUTOGEN_INCLUDE_PATH ${AUTOGEN_PATH})
            endif()
        endif()
    endforeach()

    list(REMOVE_DUPLICATES FSM_AUTOGEN_INCLUDE_PATH)
    foreach(FSM_GEN_PATCH ${FSM_AUTOGEN_INCLUDE_PATH})
        aux_source_directory(${FSM_GEN_PATCH} FSM_SOURCES)
        list(APPEND ${AUTOGEN_SOURCES} ${FSM_SOURCES})
        file(GLOB FSM_MSGDEF_PUB_HEADERS "${FSM_AUTOGEN_INCLUDE_PATH}/*public.h")
        file(GLOB FSM_MSGDEF_PRV_HEADERS "${FSM_AUTOGEN_INCLUDE_PATH}/*private.h")
        foreach(FSM_MSGDEF_PRV_HEADER ${FSM_MSGDEF_PRV_HEADERS})
            string(REPLACE "${FSM_AUTOGEN_INCLUDE_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}/private" RET "${FSM_MSGDEF_PRV_HEADER}")
            string(REPLACE "fsm_gen_private.h" "fsm_private.h" FSM_SRC_PRV_HEADER "${RET}")
            string(REPLACE "fsm_gen_private.h" "fsm.h" FSM_SRC_HEADER "${RET}")
            file(GLOB FSM_SRC_PRV_HEADERS "${FSM_SRC_PRV_HEADER}")
            file(GLOB FSM_SRC_HEADERS "${FSM_SRC_HEADER}")
            add_fsm_msgdef_headers("${FSM_SRC_PRV_HEADERS}")
            add_fsm_msgdef_headers("${FSM_SRC_HEADERS}")
        endforeach()
        add_fsm_msgdef_headers("${FSM_MSGDEF_PUB_HEADERS}")
        add_fsm_msgdef_headers("${FSM_MSGDEF_PRV_HEADERS}")
    endforeach(FSM_GEN_PATCH)
    set(${AUTOGEN_INCLUDE_PATH} ${FSM_AUTOGEN_INCLUDE_PATH})
endmacro(ut_autogen_fsms)

macro(autogen_fsms FSM_FILES AUTOGEN_SOURCES AUTOGEN_INCLUDE_PATH)
    set(FSM_AUTOGEN_INCLUDE_PATH)
    foreach(FILE_PATH ${FSM_FILES})
        if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${FILE_PATH})
            get_filename_component(FILE_REL_PATH ${FILE_PATH} ABSOLUTE BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
            get_filename_component(FILE_DIRECTORY ${FILE_REL_PATH} DIRECTORY)
            string(REGEX MATCH "${CMAKE_CURRENT_SOURCE_DIR}/" MATCH_RES ${FILE_DIRECTORY})
            if(MATCH_RES STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}/")
                set(AUTOGEN_PATH ${CMAKE_CURRENT_BINARY_DIR}/private)
                file(MAKE_DIRECTORY ${AUTOGEN_PATH})
                # message(STATUS "Autogen fsm code form ${FILE_PATH}")
                file(RELATIVE_PATH FILE_RELATIVE_PATH ${CMAKE_SOURCE_DIR} ${FILE_REL_PATH})
                execute_process(
                    COMMAND python ${CMAKE_SOURCE_DIR}/tools/fsmgen/fsmgen.py ${FILE_RELATIVE_PATH} --out_dir ${AUTOGEN_PATH}
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                    )
                list(APPEND FSM_AUTOGEN_INCLUDE_PATH ${AUTOGEN_PATH})
            endif()
        endif()
    endforeach()

    list(REMOVE_DUPLICATES FSM_AUTOGEN_INCLUDE_PATH)
    foreach(FSM_GEN_PATCH ${FSM_AUTOGEN_INCLUDE_PATH})
        aux_source_directory(${FSM_GEN_PATCH} FSM_SOURCES)
        list(APPEND ${AUTOGEN_SOURCES} ${FSM_SOURCES})
        file(GLOB FSM_MSGDEF_PUB_HEADERS "${FSM_AUTOGEN_INCLUDE_PATH}/*public.h")
        file(GLOB FSM_MSGDEF_PRV_HEADERS "${FSM_AUTOGEN_INCLUDE_PATH}/*private.h")
        foreach(FSM_MSGDEF_PRV_HEADER ${FSM_MSGDEF_PRV_HEADERS})
            string(REPLACE "${FSM_AUTOGEN_INCLUDE_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}/private" RET "${FSM_MSGDEF_PRV_HEADER}")
            string(REPLACE "fsm_gen_private.h" "fsm_private.h" FSM_SRC_PRV_HEADER "${RET}")
            string(REPLACE "fsm_gen_private.h" "fsm.h" FSM_SRC_HEADER "${RET}")
            file(GLOB FSM_SRC_PRV_HEADERS "${FSM_SRC_PRV_HEADER}")
            file(GLOB FSM_SRC_HEADERS "${FSM_SRC_HEADER}")
            add_fsm_msgdef_headers("${FSM_SRC_PRV_HEADERS}")
            add_fsm_msgdef_headers("${FSM_SRC_HEADERS}")
        endforeach()
        add_fsm_msgdef_headers("${FSM_MSGDEF_PUB_HEADERS}")
        add_fsm_msgdef_headers("${FSM_MSGDEF_PRV_HEADERS}")
    endforeach(FSM_GEN_PATCH)
    set(${AUTOGEN_INCLUDE_PATH} ${FSM_AUTOGEN_INCLUDE_PATH})
endmacro(autogen_fsms)

macro(autogen_rpc_code RPC_JSON_PATH)
    if(${CORE} STREQUAL "protocol_core")
        set(RPC_CORE "protocol")
    elseif(${CORE} STREQUAL "acore")
        set(RPC_CORE "application")
    endif()
    if(DEFINED RPC_CORE)
        string(REPLACE "_" "-" TARGET_DIR ${BUILD_TARGET_NAME})
        set(RPC_BASE_PATH ${CMAKE_SOURCE_DIR}/output/${CHIP}/${CORE}/${TARGET_DIR}/rpc_autogen)
        set(PRC_INCLUDE_DIR_PATH ${RPC_BASE_PATH}/public/${RPC_CORE})
        set(PRC_INCLUDE_FILE_PATH ${PRC_INCLUDE_DIR_PATH}/rpc_auto_generated.h)
        set(RPC_SOURCE_FILE_PATH ${RPC_BASE_PATH}/private/${RPC_CORE}/rpc_auto_generated_${RPC_CORE}.c)
        set(RPC_OPT_FILE_PATH ${RPC_BASE_PATH}/public/${RPC_CORE}/rpc_auto_generated_call_tree.opt)
        set(RPC_SCRIPT_PATH ${CMAKE_SOURCE_DIR}/tools/fsmgen/autogen/rpc/json_to_rpc_code.py)
        execute_process(
            COMMAND python3 ${RPC_SCRIPT_PATH} ${RPC_CORE} ${RPC_SOURCE_FILE_PATH} ${PRC_INCLUDE_FILE_PATH} ${RPC_OPT_FILE_PATH} ${RPC_JSON_PATH}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        )
    endif()
endmacro(autogen_rpc_code)

macro(autogen_at_json_code)
    string(REPLACE "_" "-" TARGET_DIR ${BUILD_TARGET_NAME})
    set(AT_GEN_DIR ${CMAKE_SOURCE_DIR}/output/${CHIP}/${CORE}/${TARGET_DIR}/at_autogen)
    set(AT_GEN_TOOL ${CMAKE_SOURCE_DIR}/tools/fsmgen/autogen/at/json_to_at_code.py)
    if((DEFINED TARGET_AT_JSON_FILES) AND (DEFINED AT_GEN_DIR))
        if(NOT EXISTS ${AT_GEN_DIR})
            file(MAKE_DIRECTORY ${AT_GEN_DIR})
        endif()
        foreach(json_path ${TARGET_AT_JSON_FILES})
            execute_process(COMMAND python3 ${AT_GEN_TOOL} --outdir ${AT_GEN_DIR} --inputfile  ${json_path})
        endforeach()
    endif()
endmacro(autogen_at_json_code)
