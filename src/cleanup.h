#ifndef M_CLEANUP_H_
#define M_CLEANUP_H_

#if defined(CLEANUP_IMPLEMENTATION)
    void cleanup(Renderer *renderer) {
	Glfw *glfw = renderer->glfw;
	Vulkan *vulkan = renderer->vulkan;
	SwapChain *swapChain = renderer->swapChain;

	if (vkDeviceWaitIdle(vulkan->device) != VK_SUCCESS) {
	    logMessage(LOG_ERROR, "Failed to wait for device idle");
	}
	
	vkDestroyPipeline(vulkan->device, vulkan->graphicsPipeline, NULL);
	vkDestroyPipelineLayout(vulkan->device, vulkan->pipelineLayout, NULL);
	vkDestroyShaderModule(vulkan->device, vulkan->shaderModule, NULL);

	vkFreeMemory(vulkan->device, vulkan->indexBufferMemory, NULL);
	vkDestroyBuffer(vulkan->device, vulkan->indexBuffer, NULL);

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
	vkDestroyInstance(vulkan->instance, NULL);
	if (glfw->initialized) {
	    if (glfw->window) {
		glfwDestroyWindow(glfw->window);
	    }
	    glfwTerminate();
	}
    }
#endif

#endif
