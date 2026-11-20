#define LOG_MESSAGE_IMPLEMENTATION
#define CLEANUP_IMPLEMENTATION
#define GLFW_IMPLEMENTATION
#define RUN_IMPLEMENTATION
#define INSTANCE_IMPLEMENTATION
#define PHYSICAL_DEVICE_IMPLEMENTATION
#define LOGICAL_DEVICE_IMPLEMENTATION
#define SWAP_CHAIN_IMPLEMENTATION
#define COMMAND_POOL_IMPLEMENTATION
#define BUFFER_IMPLEMENTATION
#define PIPELINE_IMPLEMENTATION

#include "common.h"

constexpr int width = 1920;
constexpr int height = 1080;
constexpr int rate = 165;

const char *const requiredLayers[] = { "VK_LAYER_KHRONOS_validation" };
const uint32_t layerCount = sizeof(requiredLayers) / sizeof(requiredLayers[0]);

const char *const requiredDeviceExtensions[] = { "VK_KHR_swapchain" };
const uint32_t deviceExtensionCount = sizeof(requiredDeviceExtensions) / sizeof(requiredDeviceExtensions[0]);

int main(int argc, const char **argv)
{
    if (argc < 2) {
        logMessage(LOG_ERROR, "Usage: %s <shader-file>\n", argv[0]);
        return false;
    }
    Glfw glfw = { .width = width, .height = height, .rate = rate};
    Vulkan vulkan = {0};
    Instance instance = {0};
    PhysicalDevice physicalDevice = {0};
    SwapChain swapChain = {0};
    Pipeline pipeline = {
	.fname = argv[1],
	.spirvSize = 0,
	.spirvBuffer = 0
    };
    Renderer renderer = {
	.glfw = &glfw,
	.vulkan = &vulkan,
	.instance = &instance,
	.physicalDevice = &physicalDevice,
	.swapChain = &swapChain,
	.pipeline = &pipeline
    };
    if (!run(&renderer)) return 1;

    struct timespec startTime, currentTime;
    clock_gettime(CLOCK_MONOTONIC, &startTime);
    while (!glfwWindowShouldClose(glfw.window)) { 
	glfwPollEvents();
	clock_gettime(CLOCK_MONOTONIC, &currentTime);
	float time = currentTime.tv_sec - startTime.tv_sec + (currentTime.tv_nsec - startTime.tv_nsec) * 1e-9f;
	printf("\rRun Time: %.1fs", time);
	fflush(stdout);
    }
    
    printf("\n");
    fflush(stdout);

    cleanup(&renderer);
    return 0;
}
