#ifndef CLEANUP_H_
#define CLEANUP_H_

#if defined(CLEANUP_IMPLEMENTATION)
    void cleanup(Renderer *renderer) {
	Glfw *glfw = renderer->glfw;
	Vulkan *vulkan = renderer->vulkan;
	Instance *instance = renderer->instance;
	PhysicalDevice *physicalDevice = renderer->physicalDevice;
	SwapChain *swapChain = renderer->swapChain;
	Pipeline *pipeline = renderer->pipeline;

	if (vkDeviceWaitIdle(vulkan->device) != VK_SUCCESS) {
	    logMessage(LOG_ERROR, "Failed to wait for device idle");
	}
	
	vkDestroyShaderModule(vulkan->device, vulkan->shaderModule, NULL);
	free(pipeline->spirvBuffer);

	vkDestroyBuffer(vulkan->device, vulkan->vertexBuffer, NULL);
	vkFreeMemory(vulkan->device, vulkan->vertexBufferMemory, NULL);

	vkDestroyCommandPool(vulkan->device, vulkan->commandPool, NULL);
	
	vkDestroyImageView(vulkan->device, vulkan->msaaImageView, NULL);
	vkDestroyImage(vulkan->device, vulkan->msaaImage, NULL);
	vkFreeMemory(vulkan->device, vulkan->msaaImageMemory, NULL);
	for (uint32_t i = 0; i < swapChain->imageCount; ++i) {
	    vkDestroyImageView(vulkan->device, vulkan->imageViews[i], NULL);
	}
	free(vulkan->imageViews);
	free(vulkan->images);
	vkDestroySwapchainKHR(vulkan->device, vulkan->swapChain, NULL);
	vkDestroyDevice(vulkan->device, NULL);
	vkDestroySurfaceKHR(vulkan->instance, vulkan->surface, NULL);
 	for (uint32_t i = 0; i < physicalDevice->deviceExtensionCount; ++i) {
	    free((void *)physicalDevice->deviceExtensions[i]);
	}
	vkDestroyInstance(vulkan->instance, NULL);
	for (uint32_t i = 0; i < instance->layerCount; ++i) {
	    free((void *)instance->layers[i]);
	}
	free(instance->layers);
	if (glfw->initialized) {
	    if (glfw->window) {
		glfwDestroyWindow(glfw->window);
	    }
	    glfwTerminate();
	}
    }
#endif

#endif
