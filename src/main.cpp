#include <GLFW/glfw3.h>
#include "./libraries/web-gpu/web-gpu-include/webgpu.hpp"
#include <iostream>
#include <emscripten/html5.h>
#include <cassert>

// If using emscripten
#define WEBGPU_BACKEND_EMSCRIPTEN

WGPUInstance instance;
WGPUAdapter adapter;

void mainLoop() {
    // Adicione o código que precisa ser executado a cada frame aqui
    // Por exemplo, chame app.onFrame() se você tiver uma instância da sua aplicação
}

void onAdapterRequestEnded(WGPURequestAdapterStatus status, WGPUAdapter receivedAdapter, const char* message, void* userdata) {
    if (status == WGPURequestAdapterStatus_Success) {
        adapter = receivedAdapter;
    } else {
        std::cerr << "Failed to get WebGPU adapter: " << message << std::endl;
    }
}

WGPUAdapter requestAdapter(WGPUInstance instance, WGPURequestAdapterOptions const * options) {
    struct UserData {
        WGPUAdapter adapter = nullptr;
        bool requestEnded = false;
    };
    UserData userData;

    auto onAdapterRequestEnded = [](WGPURequestAdapterStatus status, WGPUAdapter adapter, const char* message, void* pUserData) {
        UserData& userData = *reinterpret_cast<UserData*>(pUserData);
        if (status == WGPURequestAdapterStatus_Success) {
            userData.adapter = adapter;
        } else {
            std::cout << "Could not get WebGPU adapter: " << message << std::endl;
        }
        userData.requestEnded = true;
    };

    wgpuInstanceRequestAdapter(
        instance,
        options,
        onAdapterRequestEnded,
        (void*)&userData
    );

    // Esperar até que o callback seja chamado
    // Em um ambiente real, isso precisaria de um loop de espera ou um mecanismo de sincronização adequado.
    while (!userData.requestEnded) {
        emscripten_sleep(10);
    }

    assert(userData.requestEnded);

    return userData.adapter;
}

int main(int, char**) {
    // Inicialização do GLFW
    if (!glfwInit()) {
        std::cerr << "Falha ao inicializar o GLFW" << std::endl;
        return -1;
    }

    // Criação da janela GLFW
    GLFWwindow* window = glfwCreateWindow(640, 480, "Learn WebGPU", NULL, NULL);
    if (!window) {
        std::cerr << "Falha ao criar a janela GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Inicialização do WebGPU
    WGPUChainedStruct dummyChain = {}; // Estrutura encadeada dummy para evitar ponteiro nulo
    dummyChain.sType = WGPUSType_InstanceDescriptor; // Tipo específico de estrutura

    WGPUInstanceDescriptor desc = {};
    desc.nextInChain = &dummyChain; // Apontando para a estrutura encadeada dummy

    instance = wgpuCreateInstance(&desc);
    if (!instance) {
        std::cerr << "Could not initialize WebGPU!" << std::endl;
        return 1;
    }

    std::cout << "WGPU instance: " << instance << std::endl;

    std::cout << "Requesting adapter..." << std::endl;

    WGPURequestAdapterOptions adapterOpts = {};
    adapterOpts.compatibleSurface = nullptr; // Se você tiver uma superfície compatível, defina-a aqui.
    adapter = requestAdapter(instance, &adapterOpts);

    if (!adapter) {
        std::cerr << "Failed to get WebGPU adapter!" << std::endl;
        return 1;
    }

    std::cout << "Got adapter: " << adapter << std::endl;

    // Configura o loop principal do Emscripten
    emscripten_set_main_loop(mainLoop, 0, 1);

    // Limpeza
    glfwDestroyWindow(window);
    glfwTerminate();
    wgpuInstanceRelease(instance);

    return 0;
}
