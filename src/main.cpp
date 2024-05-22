#include <GLFW/glfw3.h>
#include "./libraries/web-gpu/web-gpu-include/webgpu.hpp"
#include <iostream>
#include <emscripten/html5.h>
#include <cassert>


// If using emscripten
#define WEBGPU_BACKEND_EMSCRIPTEN

GLFWwindow* window;

void mainLoop() {
  // Check whether the user clicked on the close button (and any other
  // mouse/key event, which we don't use so far)
  glfwPollEvents();
  
  if (glfwWindowShouldClose(window)) {
    emscripten_cancel_main_loop();
    glfwDestroyWindow(window);
    glfwTerminate();
    return;
  }

}



int main(int, char**) {
 
 std::cerr << "Started" << std::endl;

  /* GLFW */
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

  std::cerr << "The window is open" << std::endl;

  wgpu::SupportedLimits supportedLimits;
  supportedLimits.limits.minStorageBufferOffsetAlignment = 256;
  supportedLimits.limits.minUniformBufferOffsetAlignment = 256;

  // 1. We create a descriptor
  WGPUInstanceDescriptor desc = {};
  desc.nextInChain = nullptr;

  // 2. We create the instance using this descriptor
  WGPUInstance instance = wgpuCreateInstance(&desc);

  



  /* EMSCRIPTEN */  
  // Configura o loop principal do Emscripten
  emscripten_set_main_loop(mainLoop, 0, 1);

  return 0;
}


