#ifndef COMMON_H_
#define COMMON_H_
#define _POSIX_C_SOURCE 200809L 

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <stdio.h> 
#include <stdlib.h>
#include <stdarg.h>
#include <pthread.h> 
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <time.h>

typedef enum {
    LOG_INFO = 0,
    LOG_WARNING = 1,
    LOG_ERROR = 2
} LogLevel;

typedef struct {
    bool initialized;
    GLFWwindow *window;
    int width;
    int height;
    int rate;
} Glfw;

typedef struct {
    VkResult result;
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkQueue graphicsQueue;
    VkSurfaceKHR surface;
    VkSwapchainKHR swapChain;
    VkImage *images;
    VkImageView *imageViews;
    
    VkImage msaaImage;
    VkDeviceMemory msaaImageMemory;
    VkImageView msaaImageView;

    VkCommandPool commandPool;

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;

    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;

    VkShaderModule shaderModule;
} Vulkan;

typedef struct {
    uint32_t extensionCount;
    const char **extensions;

    uint32_t layerCount;
    const char **layers;
} Instance;

typedef struct {
    Instance *instance;
    VkResult taskRes;
} LayerTask;

typedef struct {
    uint32_t graphicsIndex;

    uint32_t deviceExtensionCount;
    const char **deviceExtensions;

    VkSampleCountFlagBits msaaSamples;
} PhysicalDevice;

typedef struct {
    VkResult taskRes;
    Vulkan *vulkan;
    PhysicalDevice *physicalDevice;
} QueueTask;

typedef struct {
    VkExtent2D surfaceExtent;
    uint32_t imageCount;

    uint32_t formatCount;
    uint32_t formatIndex;
    VkSurfaceFormatKHR *formats;
    
    uint32_t presentModeCount;
    uint32_t presentModeIndex;
    VkPresentModeKHR *presentModes;
} SwapChain;

typedef struct {
    Vulkan *vulkan;
    SwapChain *swapChain;
    VkResult taskRes;
} SurfaceTask;

typedef struct {
    float position[2];
    float color[3];
} Vertex;

typedef struct {
    VkDeviceSize size;
    VkDeviceSize requirementSize;
    VkBufferUsageFlags usage;
    VkBuffer *buffer;
    VkDeviceMemory *memory;
    VkMemoryPropertyFlags propertyFlags;
} Buffer;

typedef struct {
    const char *const fname;
    size_t spirvSize;
    uint32_t *spirvBuffer;
} Pipeline;

typedef struct {
    Glfw *glfw;
    Vulkan *vulkan;
    Instance *instance;
    PhysicalDevice *physicalDevice;
    SwapChain *swapChain;
    Pipeline *pipeline;
} Renderer;

/* void logMessage(LogLevel level, ...); */
void logMessageImpl(LogLevel level, const char *file, int line, const char *format, ...);
bool run(Renderer *renderer);
void cleanup(Renderer *renderer);

bool createWindow(Renderer *renderer);
void errorCallback(int errorCode, const char *description);
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

int createInstance(Renderer *renderer);

int pickPhysicalDevice(Renderer *renderer); 

int createLogicalDevice(Renderer *renderer);

int createSwapChain(Renderer *renderer);

int createCommandPool(Renderer *renderer);

bool createBuffer(Vulkan *vulkan, Buffer *buffer);

int createVertexBuffer(Renderer *renderer);

int createIndexBuffer(Renderer *renderer);

int createPipeline(Renderer *renderer);

#ifdef NDEBUG
#define ENABLE_VALIDATION 0
#else 
#define ENABLE_VALIDATION 1
#endif

#if defined(LOG_MESSAGE_IMPLEMENTATION)
    #include "logMessage.h"
#endif
    
#define logMessage(level, ...)\
logMessageImpl(level, __FILE__, __LINE__, __VA_ARGS__)

#if defined(CLEANUP_IMPLEMENTATION)
    #include "cleanup.h"
#endif

#if defined(GLFW_IMPLEMENTATION)
    #include "glfw.h"
#endif

#if defined(RUN_IMPLEMENTATION)
    #include "run.h"
#endif

#if defined(INSTANCE_IMPLEMENTATION)
    #include "instance.h"
#endif

#if defined(PHYSICAL_DEVICE_IMPLEMENTATION)
    #include "physicalDevice.h"
#endif

#if defined(LOGICAL_DEVICE_IMPLEMENTATION)
    #include "logicalDevice.h"
#endif

#if defined(SWAP_CHAIN_IMPLEMENTATION)
    #include "swapChain.h"
#endif

#if defined(COMMAND_POOL_IMPLEMENTATION)
    #include "commandPool.h"
#endif

#if defined(BUFFER_IMPLEMENTATION)
    #include "buffer.h"
#endif
    
#if defined(PIPELINE_IMPLEMENTATION)
    #include "pipeline.h"
#endif

#endif 
