#ifndef VULKAN_H
#define VULKAN_H

#if defined(VULKAN_IMPLEMENTATION)
    bool run(Renderer *renderer) {
	if (!createWindow(renderer)) goto clean;
	if (createInstance(renderer) != VK_SUCCESS) {
	    goto clean;
	};
	if (pickPhysicalDevice(renderer) != VK_SUCCESS) {
	    goto clean;
	}
	if (createLogicalDevice(renderer) != VK_SUCCESS) {
	    goto clean;
	}
	if (createSwapChain(renderer) != VK_SUCCESS) {
	    goto clean;
	}
	if (createPipeline(renderer) != VK_SUCCESS) {
	    goto clean;
	}
	if (createVertexBuffer(renderer) != VK_SUCCESS) {
	    goto clean;
	}
	return true; 
 clean:
	cleanup(renderer);
	return false;
    }
#endif

#endif

