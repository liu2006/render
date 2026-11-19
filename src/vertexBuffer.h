#ifndef VERTEX_BUFFER_H_
#define VERTEX_BUFFER_H_

#if defined(VERTEX_BUFFER_IMPLEMENTATION)
    int createVertexBuffer(Renderer *renderer)
    {
	Vulkan *vulkan = renderer->vulkan;

	Vertex vertices[] = {
	    { .position = { -0.5f, -0.5f }, .color = { 1.0f, 0.0f, 0.0f} },
	    { .position = { 0.5f, -0.5f }, .color = { 0.0f, 1.0f, 0.0f} },
	    { .position = { 0.5f, 0.5f }, .color = { 0.0f, 0.0f, 1.0f} },
	    { .position = { -0.5f, 0.5f }, .color = { 0.3f, 0.7f, 0.5f} },
	};

	VkBuffer stagingBuffer = VK_NULL_HANDLE;
	VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;
	const VkBufferCreateInfo stagingBufferInfo = {
	    .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
	    .pNext = NULL,
	    .flags = 0,
	    .size = sizeof(vertices),
	    .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
	    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
	    .queueFamilyIndexCount = 0,
	    .pQueueFamilyIndices = NULL
	};
	vulkan->result = vkCreateBuffer(vulkan->device, &stagingBufferInfo, NULL, &stagingBuffer);
	if (vulkan->result != VK_SUCCESS) {
	    logMessage(LOG_ERROR, "Failed to create staging buffer for vertices\n");
	    goto clean;
	} 
	
	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(vulkan->device, stagingBuffer, &memoryRequirements);

	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(vulkan->physicalDevice, &memoryProperties);
	uint32_t stagingMemoryTypeIndex = UINT32_MAX;
	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i) {
	    if (!(memoryRequirements.memoryTypeBits & (1u << i)))
		continue;
	    if (!(memoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT))
		continue;
	    if (!(memoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
		continue;
	    stagingMemoryTypeIndex = i;
	    break;
	}
	if (stagingMemoryTypeIndex == UINT32_MAX) {
	    logMessage(LOG_ERROR, "Failed to find suitable staging memory type\n");
	    goto clean;
	}
	VkMemoryAllocateInfo memoryAllocateInfo = {
	    .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
	    .pNext = 0,
	    .allocationSize = memoryRequirements.size,
	    .memoryTypeIndex = stagingMemoryTypeIndex
	};
	vulkan->result = vkAllocateMemory(vulkan->device, &memoryAllocateInfo, NULL, &stagingBufferMemory);
	if (vulkan->result != VK_SUCCESS) {
	    logMessage(LOG_ERROR, "Failed to allocate memory for staging buffer\n");
	    goto clean;
	}
	vulkan->result = vkBindBufferMemory(vulkan->device, stagingBuffer, stagingBufferMemory, 0);
	if (vulkan->result != VK_SUCCESS) {
	    logMessage(LOG_ERROR, "Failed to bind memory for staging buffer\n");
	    goto clean;
	}
	
	void *data;
	vulkan->result = vkMapMemory(vulkan->device, stagingBufferMemory, 0, memoryRequirements.size, 0, &data);
	if (vulkan->result != VK_SUCCESS) {
	    logMessage(LOG_ERROR, "Failed to map stagging buffer memory\n");
	}
	memcpy(data, vertices, stagingBufferInfo.size);
	vkUnmapMemory(vulkan->device, stagingBufferMemory);

	const VkBufferCreateInfo vertexBufferInfo = {
	    .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
	    .pNext = NULL,
	    .flags = 0,
	    .size = sizeof(vertices),
	    .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
	    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
	    .queueFamilyIndexCount = 0,
	    .pQueueFamilyIndices = NULL
	};

	vulkan->result = vkCreateBuffer(vulkan->device, &vertexBufferInfo, NULL, &vulkan->vertexBuffer);
	if (vulkan->result != VK_SUCCESS) {
	    logMessage(LOG_ERROR, "Failed to create vertex buffer\n");
	    goto clean;
	}
	
	vkGetBufferMemoryRequirements(vulkan->device, vulkan->vertexBuffer, &memoryRequirements);	
	vkGetPhysicalDeviceMemoryProperties(vulkan->physicalDevice, &memoryProperties);
	uint32_t vertexMemoryTypeIndex = UINT32_MAX;
	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i) {
	    if (!(memoryRequirements.memoryTypeBits & (1u << i)))
		continue;
	    if (!(memoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT))
		continue;
	    vertexMemoryTypeIndex = i;
	    break;
	}
	if (vertexMemoryTypeIndex == UINT32_MAX) {
	    logMessage(LOG_ERROR, "Failed to find suitable vertex memory type\n");
	    goto clean;
	}
	memoryAllocateInfo = (VkMemoryAllocateInfo){
	    .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
	    .pNext = 0,
	    .allocationSize = memoryRequirements.size,
	    .memoryTypeIndex = vertexMemoryTypeIndex
	};

	vulkan->result = vkAllocateMemory(vulkan->device, &memoryAllocateInfo, NULL, &vulkan->vertexBufferMemory);
	if (vulkan->result != VK_SUCCESS) {
	    logMessage(LOG_ERROR, "Failed to allocate memory for vertex buffer\n");
	    goto clean;
	}
	vulkan->result = vkBindBufferMemory(vulkan->device, vulkan->vertexBuffer, vulkan->vertexBufferMemory, 0);
	if (vulkan->result != VK_SUCCESS) {
	    logMessage(LOG_ERROR, "Failed to bind memory for vertex buffer\n");
	    goto clean;
	}

	VkCommandBuffer commandCopyBuffer = VK_NULL_HANDLE; 
	const VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
	    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
	    .pNext = NULL,
	    .commandPool = vulkan->commandPool,
	    .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
	    .commandBufferCount = 1
	};
	vulkan->result = vkAllocateCommandBuffers(vulkan->device, &commandBufferAllocateInfo, &commandCopyBuffer);
	if (vulkan->result != VK_SUCCESS) {
	    logMessage(LOG_ERROR, "Failed to allocate copy command buffer\n");
	    goto clean;
	}
	const VkCommandBufferBeginInfo beginInfo ={
	    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
	    .pNext = NULL,
	    .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
	    .pInheritanceInfo = NULL
	};

	vulkan->result = vkBeginCommandBuffer(commandCopyBuffer, &beginInfo);
	if (vulkan->result != VK_SUCCESS) {
	    logMessage(LOG_ERROR, "Failed to begin copy command buffer\n");
	    goto clean;
	}
	const VkBufferCopy bufferCopy = {
	    .srcOffset = 0,
	    .dstOffset = 0,
	    .size = stagingBufferInfo.size
	};
	vkCmdCopyBuffer(commandCopyBuffer, stagingBuffer, vulkan->vertexBuffer, 1, &bufferCopy);
	vulkan->result = vkEndCommandBuffer(commandCopyBuffer);
	if (vulkan->result != VK_SUCCESS) {
	    logMessage(LOG_ERROR, "Failed to end copy command buffer\n");
	    goto clean;
	}
	VkFence fence = VK_NULL_HANDLE;
	const VkFenceCreateInfo fenceInfo = {
	    .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
	    .pNext = NULL,
	    .flags = 0
	};
	vulkan->result = vkCreateFence(vulkan->device, &fenceInfo, NULL, &fence);
	if (vulkan->result != VK_SUCCESS) {
	    logMessage(LOG_ERROR, "Failed to create fence for buffer copy\n");
	    goto clean;
	}
	const VkSubmitInfo submitInfo = {
	    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = NULL,
	    .waitSemaphoreCount = 0,
	    .pWaitSemaphores = NULL,
	    .pWaitDstStageMask = NULL,
	    .commandBufferCount = 1,
	    .pCommandBuffers = &commandCopyBuffer,
	    .signalSemaphoreCount = 0,
	    .pSignalSemaphores = NULL
	};
	vulkan->result = vkQueueSubmit(vulkan->graphicsQueue, 1, &submitInfo, fence);
	if (vulkan->result != VK_SUCCESS) {
	    logMessage(LOG_ERROR, "Failed to submit to copy command buffer\n");
	    goto clean; 
	}
	vulkan->result = vkWaitForFences(vulkan->device, 1, &fence, VK_TRUE, UINT64_MAX);
	if (vulkan->result != VK_SUCCESS) {
	    logMessage(LOG_ERROR, "Failed to wait for copy fence\n");
	    goto clean; 
	}	
	vulkan->result = VK_SUCCESS;
	
 clean:
	vkDestroyFence(vulkan->device, fence, NULL);
	vkDestroyBuffer(vulkan->device, stagingBuffer, NULL);
	vkFreeMemory(vulkan->device, stagingBufferMemory, NULL);
	return vulkan->result;
    }
#endif

#endif
