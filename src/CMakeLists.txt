cmake_minimum_required(VERSION 3.12)

# Defina o diretório de saída para os arquivos de output do Emscripten
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/../emscripten-out")
#set(MAIN_DIRECTORY "C:/Users/CREA-PE/Desktop/Fabrício/Arquivos/Projects/web-assembly/Protocol_Monitor/Main")

project(
    #name
    ProtocolMonitor 

    DESCRIPTION "Simple test for setup configurations"

    LANGUAGES CXX 

    VERSION 0.1.0
)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory("libraries/web-gpu")

add_subdirectory("libraries/dear-im-gui")

# (In 'if (EMSCRIPTEN)')
set(SHELL_FILE shell_minimal.html)

if (NOT EMSCRIPTEN)
    # Do not include this with emscripten, it provides its own version.
    MENSAGE(STATUS "\nSem emscripten\n")
    add_subdirectory("libraries/glfw")
endif()

add_executable(${PROJECT_NAME} main.cpp)

# Altere o nome do arquivo de saída para .html
set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")

target_compile_options(${PROJECT_NAME} PRIVATE
    "-Wall"
    "-g3"
)

target_link_options(${PROJECT_NAME} PRIVATE  

    "-sUSE_WEBGPU"
    "-jsDWEBGPU_DEBUG=1"
    "-sUSE_GLFW=3"
    "-sWASM=1"
    "-sALLOW_MEMORY_GROWTH=1"
    "-sNO_EXIT_RUNTIME=1"
    "-sASSERTIONS=1"
    "-sDISABLE_EXCEPTION_CATCHING=0"
    "-sNO_FILESYSTEM=1"
    "-sVERBOSE=0"
    "-sASYNCIFY"
    "-sDYNAMIC_EXECUTION=0"
    #"-sEMBIND_AOT=1"
    "--emrun"
    "-Wall"
    "-g3"
    --shell-file "${CMAKE_CURRENT_SOURCE_DIR}/${SHELL_FILE}"
    
   

) 


target_compile_options(${PROJECT_NAME} PRIVATE)

# Obtenha as opções de linkagem do target
get_property(LINK_OPTIONS TARGET ${PROJECT_NAME} PROPERTY LINK_OPTIONS)
message(STATUS "Link Options:")
foreach(opt ${LINK_OPTIONS})
    message(STATUS "  ${opt}")
endforeach()

get_property(COMPILE_OPTIONS TARGET ${PROJECT_NAME} PROPERTY COMPILE_OPTIONS)
message(STATUS "Compile Options:")
foreach(opt ${COMPILE_OPTIONS})
    message(STATUS "  ${opt}")
endforeach()

message(STATUS "${CMAKE_CURRENT_BINARY_DIR} - Mensagem 1")
message(STATUS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY} - Mensagem 2")
