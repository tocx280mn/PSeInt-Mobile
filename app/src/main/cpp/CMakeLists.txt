cmake_minimum_required(VERSION 3.22.1)

project("pseint")

# Add the PSeInt core sources
file(GLOB PSEINT_SOURCES
    "../../../pseint/pseint/SynCheck.cpp"
    "../../../pseint/pseint/intercambio.cpp"
    "../../../pseint/pseint/main.cpp"
    "../../../pseint/pseint/Ejecutar.cpp"
    "../../../pseint/pseint/global.cpp"
    "../../../pseint/pseint/zockets.cpp"
    "../../../pseint/pseint/utils.cpp"
    "../../../pseint/pseint/zcurlib.cpp"
    "../../../pseint/pseint/Evaluar.cpp"
    "../../../pseint/pseint/new_memoria.cpp"
    "../../../pseint/pseint/FuncsManager.cpp"
    "../../../pseint/pseint/DataValue.cpp"
    "../../../pseint/pseint/case_map.cpp"
    "../../../pseint/pseint/zcurlib_lnx.cpp"
    "../../../pseint/pseint/zcurlib_win.cpp"
    "../../../pseint/pseint/LangSettings.cpp"
    "../../../pseint/pseint/ErrorHandler.cpp"
    "../../../pseint/pseint/FuncsPredefs.cpp"
    "../../../pseint/pseint/ProgramaDump.cpp"
    "../../../pseint/pseint/Keywords.cpp"
    "../../../pseint/pseint/strFuncs.cpp"
    "pseint_jni.cpp"
)

add_library(
        ${CMAKE_PROJECT_NAME}
        SHARED
        ${PSEINT_SOURCES}
)

# Add Android logging library
find_library(
        log-lib
        log
)

target_link_libraries(
        ${CMAKE_PROJECT_NAME}
        ${log-lib}
)
