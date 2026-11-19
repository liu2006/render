#ifndef RUN_H_
#define RUN_H_

#if defined(RUN_IMPLEMENTATION)
    bool run(Renderer *renderer) {
	if (!createWindow(renderer)) goto clean;
	if (createInstance(renderer) != VK_SUCCESS) goto clean;
	if (pickPhysicalDevice(renderer) != VK_SUCCESS) goto clean;
	if (createLogicalDevice(renderer) != VK_SUCCESS) goto clean;
	if (createSwapChain(renderer) != VK_SUCCESS) goto clean;
	if (createCommandPool(renderer) != VK_SUCCESS) goto clean;
	if (createVertexBuffer(renderer) != VK_SUCCESS) goto clean;
	if (createIndexBuffer(renderer) != VK_SUCCESS) goto clean;
	if (createPipeline(renderer) != VK_SUCCESS) goto clean;
	return true; 
 clean:
	cleanup(renderer);
	return false;
    }
#endif

#endif

