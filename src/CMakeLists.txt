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




if (NOT EMSCRIPTEN)
    # Do not include this with emscripten, it provides its own version.
    MENSAGE(STATUS "Sem emscripten")
    add_subdirectory("libraries/glfw")
endif()


add_executable(${PROJECT_NAME} main.cpp)

# Altere o nome do arquivo de saída para .html
set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")

target_link_options(${PROJECT_NAME} PRIVATE  
    "-sUSE_WEBGPU=1"
    "-sUSE_GLFW=3"
    "-sWASM=1"
    "-sALLOW_MEMORY_GROWTH=1"
    "-sNO_EXIT_RUNTIME=0"
    "-sASSERTIONS=1"
    "-sDISABLE_EXCEPTION_CATCHING=1"
    "-sNO_FILESYSTEM=1"
    "-sVERBOSE=0"
    "-sASYNCIFY"
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
