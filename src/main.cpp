#include <GLFW/glfw3.h>
#include <iostream>
#include <emscripten/html5.h>
#include <cassert>
#include <vector>

#define WEBGPU_BACKEND_EMSCRIPTEN 
#define WEBGPU_CPP_IMPLEMENTATION
#include "./libraries/web-gpu/web-gpu-include/webgpu.hpp"

GLFWwindow* window;

void mainLoop() {
    glfwPollEvents();
    
    if (glfwWindowShouldClose(window)) {
        emscripten_cancel_main_loop();
        glfwDestroyWindow(window);
        glfwTerminate();
        return;
    }
}

using namespace wgpu;

void inspectAdapter(WGPUAdapter adapter) {

    std::vector<WGPUFeatureName> features;

    // Call the function a first time with a null return address, just to get
    // the entry count.
    size_t featureCount = wgpuAdapterEnumerateFeatures(adapter, nullptr);

    // Allocate memory (could be a new, or a malloc() if this were a C program)
    features.resize(featureCount);

    // Call the function a second time, with a non-null return address
    wgpuAdapterEnumerateFeatures(adapter, features.data());

    std::cout << "Adapter features:" << std::endl;
    for (auto f : features) {
        std::cout << " - " << f << std::endl;
    }

    WGPUSupportedLimits limits = {};
    limits.nextInChain = nullptr;
    bool success = wgpuAdapterGetLimits(adapter, &limits);
    if (success) {
        std::cout << "Adapter limits:" << std::endl;
        std::cout << " - maxTextureDimension1D: " << limits.limits.maxTextureDimension1D << std::endl;
        std::cout << " - maxTextureDimension2D: " << limits.limits.maxTextureDimension2D << std::endl;
        std::cout << " - maxTextureDimension3D: " << limits.limits.maxTextureDimension3D << std::endl;
        std::cout << " - maxTextureArrayLayers: " << limits.limits.maxTextureArrayLayers << std::endl;
        std::cout << " - maxBindGroups: " << limits.limits.maxBindGroups << std::endl;
        std::cout << " - maxDynamicUniformBuffersPerPipelineLayout: " << limits.limits.maxDynamicUniformBuffersPerPipelineLayout << std::endl;
        std::cout << " - maxDynamicStorageBuffersPerPipelineLayout: " << limits.limits.maxDynamicStorageBuffersPerPipelineLayout << std::endl;
        std::cout << " - maxSampledTexturesPerShaderStage: " << limits.limits.maxSampledTexturesPerShaderStage << std::endl;
        std::cout << " - maxSamplersPerShaderStage: " << limits.limits.maxSamplersPerShaderStage << std::endl;
        std::cout << " - maxStorageBuffersPerShaderStage: " << limits.limits.maxStorageBuffersPerShaderStage << std::endl;
        std::cout << " - maxStorageTexturesPerShaderStage: " << limits.limits.maxStorageTexturesPerShaderStage << std::endl;
        std::cout << " - maxUniformBuffersPerShaderStage: " << limits.limits.maxUniformBuffersPerShaderStage << std::endl;
        std::cout << " - maxUniformBufferBindingSize: " << limits.limits.maxUniformBufferBindingSize << std::endl;
        std::cout << " - maxStorageBufferBindingSize: " << limits.limits.maxStorageBufferBindingSize << std::endl;
        std::cout << " - minUniformBufferOffsetAlignment: " << limits.limits.minUniformBufferOffsetAlignment << std::endl;
        std::cout << " - minStorageBufferOffsetAlignment: " << limits.limits.minStorageBufferOffsetAlignment << std::endl;
        std::cout << " - maxVertexBuffers: " << limits.limits.maxVertexBuffers << std::endl;
        std::cout << " - maxVertexAttributes: " << limits.limits.maxVertexAttributes << std::endl;
        std::cout << " - maxVertexBufferArrayStride: " << limits.limits.maxVertexBufferArrayStride << std::endl;
        std::cout << " - maxInterStageShaderComponents: " << limits.limits.maxInterStageShaderComponents << std::endl;
        std::cout << " - maxComputeWorkgroupStorageSize: " << limits.limits.maxComputeWorkgroupStorageSize << std::endl;
        std::cout << " - maxComputeInvocationsPerWorkgroup: " << limits.limits.maxComputeInvocationsPerWorkgroup << std::endl;
        std::cout << " - maxComputeWorkgroupSizeX: " << limits.limits.maxComputeWorkgroupSizeX << std::endl;
        std::cout << " - maxComputeWorkgroupSizeY: " << limits.limits.maxComputeWorkgroupSizeY << std::endl;
        std::cout << " - maxComputeWorkgroupSizeZ: " << limits.limits.maxComputeWorkgroupSizeZ << std::endl;
        std::cout << " - maxComputeWorkgroupsPerDimension: " << limits.limits.maxComputeWorkgroupsPerDimension << std::endl;
    }

    WGPUAdapterProperties properties = {};
    properties.nextInChain = nullptr;
    wgpuAdapterGetProperties(adapter, &properties);
    std::cout << "Adapter properties:" << std::endl;
    std::cout << " - vendorID: " << properties.vendorID << std::endl;
    std::cout << " - deviceID: " << properties.deviceID << std::endl;
    std::cout << " - name: " << properties.name << std::endl;
    if (properties.driverDescription) {
        std::cout << " - driverDescription: " << properties.driverDescription << std::endl;
    }
    std::cout << " - adapterType: " << properties.adapterType << std::endl;
    std::cout << " - backendType: " << properties.backendType << std::endl;
}

WGPUAdapter requestAdapter(WGPUInstance instance, WGPURequestAdapterOptions const* options){
        
    struct UserData {
    WGPUAdapter adapter = nullptr;
    bool requestEnded = false;
    };  

    UserData userData;    

    auto onAdapterRequestEnded = [](WGPURequestAdapterStatus status, WGPUAdapter adapter, char const* message, void* pUserData){

        UserData& userData = *reinterpret_cast<UserData*>(pUserData);

        if (status == WGPURequestAdapterStatus_Success){
            userData.adapter = adapter;

        }else{
            std::cout << "could not get webGPU adapter:" << message <<std::endl;
        }
        userData.requestEnded = true;
    };

    wgpuInstanceRequestAdapter(
        instance,
        options,
        onAdapterRequestEnded,
        (void*)&userData
    );

    // Espera até que a solicitação do adaptador seja concluída
    while (!userData.requestEnded) {
        emscripten_sleep(1); // Para evitar um loop de espera ativo
    }
    
    assert(userData.adapter);

    return userData.adapter;
        
}

WGPUDevice requestDevice(WGPUAdapter adapter, WGPUDeviceDescriptor const * descriptor) {
    struct UserData {
        WGPUDevice device = nullptr;
        bool requestEnded = false;
    };
    UserData userData;

    auto onDeviceRequestEnded = [](WGPURequestDeviceStatus status, WGPUDevice device, char const * message, void * pUserData) {
        UserData& userData = *reinterpret_cast<UserData*>(pUserData);
        if (status == WGPURequestDeviceStatus_Success) {
            userData.device = device;
        } else {
            std::cout << "Could not get WebGPU device: " << message << std::endl;
        }
        userData.requestEnded = true;
    };

    wgpuAdapterRequestDevice(
        adapter,
        descriptor,
        onDeviceRequestEnded,
        (void*)&userData
    );

    assert(userData.requestEnded);

    return userData.device;
}

int main(int, char**) {
    SupportedLimits supportedLimits;
    supportedLimits.limits.minStorageBufferOffsetAlignment = 256;
    supportedLimits.limits.minUniformBufferOffsetAlignment = 256;
    
    std::cout << "Started" << std::endl;



    // GLFW
    if (!glfwInit()) {
        std::cerr << "Could not initialize GLFW!" << std::endl;
        return 1;
    }

    window = glfwCreateWindow(640, 480, "Learn WebGPU", NULL, NULL);

    if (!window) {
        std::cerr << "Could not open window!" << std::endl;
        glfwTerminate();
        return 1;
    }

    std::cout << "The window is  open" << std::endl;

    /* GLFW> */

    /*WEB_GPU*/

    WGPUInstanceDescriptor* iDescriptor = nullptr;
    WGPUInstance instance = wgpuCreateInstance(iDescriptor);
    
    if (instance == nullptr) {
        printf("instância não é nullprt\n");
        return -1;
    }

    // 4. Check if the instance was created successfully
    if (!instance) {
        std::cerr << "Could not initialize WebGPU!" << std::endl;
        return 1;
    }

    std::cout << "WGPU instance: " << instance << std::endl;


    std::cout << "Requesting adapter..." << std::endl;

    WGPURequestAdapterOptions* adapterOpts = nullptr;
    WGPUAdapter adapter = requestAdapter(instance, adapterOpts);

    std::cout << "Got adapter: " << adapter << std::endl;

    inspectAdapter(adapter);

    std::cout << "Requesting device..." << std::endl;

    WGPUDeviceDescriptor* DDescriptor = nullptr;
    WGPUDevice device = requestDevice(adapter,DDescriptor);

    std::cout << "Got device: " << device << std::endl;

    
    wgpuAdapterRelease(adapter);
    wgpuInstanceRelease(instance);

    /* WEB_GPU> */

    // Set up the main loop for Emscripten
    emscripten_set_main_loop(mainLoop, 0, 1);

    return 0;
}
