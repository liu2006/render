#ifndef M_COMMAND_POOL_H_
#define M_COMMAND_POOL_H_

#if defined(COMMAND_POOL_IMPLEMENTATION)
    int createCommandPool(Renderer *renderer) 
    {
	Vulkan *vulkan = renderer->vulkan;
	PhysicalDevice *physicalDevice = renderer->physicalDevice;
	
	const VkCommandPoolCreateInfo commandPoolInfo = {
	    .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
	    .pNext = NULL,
	    .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
	    .queueFamilyIndex = physicalDevice->graphicsIndex
	};
	vulkan->result = vkCreateCommandPool(vulkan->device, &commandPoolInfo, NULL, &vulkan->commandPool);
	if (vulkan->result != VK_SUCCESS) {
	    logMessage(LOG_ERROR, "Failed to create command pool [%d]\n", vulkan->result);
	    goto clean;
	}
	
	vulkan->result = VK_SUCCESS; 
 clean:	
	return vulkan->result;
    }
#endif

#endif
