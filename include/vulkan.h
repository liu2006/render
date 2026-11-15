#ifndef VULKAN_H
#define VULKAN_H

#if defined(VULKAN_IMPLEMENTATION)
    bool run(int argc, const char **argv, Renderer *renderer) {
	if (argc < 2) {
            logMessage(LOG_ERROR, "Usage: %s <shader-file>\n", argv[0]);
            return false;
	}
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
	if (createPipeline(renderer, argv[1]) != VK_SUCCESS) {
	    goto clean;
	}
	if (createVertexBuffer(renderer) != VK_SUCCESS) {
	    goto clean;
	}

	return true; 
 clean:
	clean(renderer);
	return false;
    }
#endif
#endif

