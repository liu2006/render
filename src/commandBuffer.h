#ifndef M_COMMAND_BUFFER_H_
#define M_COMMAND_BUFFER_H_

#if defined(COMMAND_BUFFER_IMPLEMENTATION)
    int recordCommandBuffer(Renderer *renderer)
    {
	Vulkan *vulkan = renderer->vulkan;
	const VkCommandBufferAllocateInfo allocateInfo = {
	    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
	    .pNext = NULL,
	    .commandPool = vulkan->commandPool,
	    .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
	    .commandBufferCount = 1
	};
	vulkan->result = vkAllocateCommandBuffers(vulkan->device, &allocateInfo, &vulkan->commandBuffer);
	if (vulkan->result != VK_SUCCESS) {
	    logMessage(LOG_ERROR, "Failed to allocate command buffers [%d]\n", vulkan->result);
	    goto clean;
	}
	const VkCommandBufferBeginInfo beginInfo = {
	    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
	    .pNext = NULL,
	    .flags = 0,
	    .pInheritanceInfo = NULL
	};
	vulkan->result = vkBeginCommandBuffer(vulkan->commandBuffer, &beginInfo);
	if (vulkan->result != VK_SUCCESS) {
	    logMessage(LOG_ERROR, "Failed to begin command buffer [%d]\n", vulkan->result);
	    goto clean;
	}
	vulkan->result = VK_SUCCESS;
 clean:
	return vulkan->result;
    }
#endif

#endif
