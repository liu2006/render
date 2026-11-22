#ifndef M_BUFFER_H_
#define M_BUFFER_H_

#if defined(BUFFER_IMPLEMENTATION)
    bool createBuffer(Vulkan *vulkan, Buffer *buffer) {
	const VkBufferCreateInfo bufferInfo = {
	    .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
	    .pNext = NULL,
	    .flags = 0,
	    .size = buffer->size,
	    .usage = buffer->usage,
	    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
	    .queueFamilyIndexCount = 0,
	    .pQueueFamilyIndices = NULL
	};
	
	vulkan->result = vkCreateBuffer(vulkan->device, &bufferInfo, NULL, buffer->buffer);
	if (vulkan->result != VK_SUCCESS) {
	    logMessage(LOG_ERROR, "Failed to create buffer\n");
	    return false;
	}

	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(vulkan->device, *buffer->buffer, &memoryRequirements);
	buffer->requirementSize = memoryRequirements.size;
	
	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(vulkan->physicalDevice, &memoryProperties);

	uint32_t memoryTypeIndex = UINT32_MAX;
	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i) {
	    if (!(memoryRequirements.memoryTypeBits & (1u << i)))
		continue;
	    if ((memoryProperties.memoryTypes[i].propertyFlags & buffer->propertyFlags) != buffer->propertyFlags)
		continue;
	    memoryTypeIndex = i;
	    break;
	}
	if (memoryTypeIndex == UINT32_MAX) {
	    logMessage(LOG_ERROR, "Failed to find suitable memory type\n");
	    return false;
	}
	const VkMemoryAllocateInfo allocateInfo = {
	    .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
	    .pNext = NULL,
	    .allocationSize = memoryRequirements.size,
	    .memoryTypeIndex = memoryTypeIndex,
	};
	vulkan->result = vkAllocateMemory(vulkan->device, &allocateInfo, NULL, buffer->memory);
	if (vulkan->result != VK_SUCCESS) {
	    logMessage(LOG_ERROR, "Failed to allocate buffer memory\n");
	    return false;
	}

	vulkan->result = vkBindBufferMemory(vulkan->device, *buffer->buffer, *buffer->memory, 0);
	if (vulkan->result != VK_SUCCESS) {
	    logMessage(LOG_ERROR, "Failed to bind buffer memory\n");
	    return false;
	}
	return true;
    }

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
	
	Buffer stagingBufferInfo = {
	    .size = sizeof(vertices),
	    .requirementSize = 0,
	    .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
	    .buffer = &stagingBuffer,
	    .memory = &stagingBufferMemory,
	    .propertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
	};
	if (!createBuffer(vulkan, &stagingBufferInfo)) goto clean;
	void *data = NULL;
	
	vulkan->result = vkMapMemory(vulkan->device, stagingBufferMemory, 0, stagingBufferInfo.requirementSize, 0, &data);
	if (vulkan->result != VK_SUCCESS) {
	    logMessage(LOG_ERROR, "Failed to map buffer memory\n");
	    goto clean; 
	}
	memcpy(data, vertices, stagingBufferInfo.size);
	vkUnmapMemory(vulkan->device, stagingBufferMemory);

	Buffer vertexBufferInfo = {
	    .size = sizeof(vertices), 
	    .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
	    .buffer = &vulkan->vertexBuffer,
	    .memory = &vulkan->vertexBufferMemory,
	    .propertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
	};
	if (!createBuffer(vulkan, &vertexBufferInfo)) goto clean;
	
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
	    logMessage(LOG_ERROR, "Failed to allocate command buffer\n");
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
	    logMessage(LOG_ERROR, "Failed to begin command buffer\n");
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
	    logMessage(LOG_ERROR, "Failed to end command buffer\n");
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
	VkFence fence = VK_NULL_HANDLE;
	const VkFenceCreateInfo fenceInfo = {
	    .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
	    .pNext = NULL,
	    .flags = 0
	};
	vulkan->result = vkCreateFence(vulkan->device, &fenceInfo, NULL, &fence);
	if (vulkan->result != VK_SUCCESS) {
	    logMessage(LOG_ERROR, "Failed to create fence\n");
	    goto clean;
	}
	
	vulkan->result = vkQueueSubmit(vulkan->graphicsQueue, 1, &submitInfo, fence);
	if (vulkan->result != VK_SUCCESS) {
	    logMessage(LOG_ERROR, "Failed to submit command buffer\n");
	    goto clean; 
	}
	
	vulkan->result = vkWaitForFences(vulkan->device, 1, &fence, VK_TRUE, UINT64_MAX);
	if (vulkan->result != VK_SUCCESS) {
	    logMessage(LOG_ERROR, "Failed to wait for fence\n");
	    goto clean; 
	}	
	vulkan->result = VK_SUCCESS;
	
 clean:
	vkDestroyFence(vulkan->device, fence, NULL);
	vkFreeCommandBuffers(vulkan->device, vulkan->commandPool, 1, &commandCopyBuffer);
	vkDestroyBuffer(vulkan->device, stagingBuffer, NULL);
	vkFreeMemory(vulkan->device, stagingBufferMemory, NULL);
	return vulkan->result;
    }

    int createIndexBuffer(Renderer *renderer)
    {
	Vulkan *vulkan = renderer->vulkan;

	uint32_t indices[] = { 0, 1, 2, 2, 3, 0}; 
	VkBuffer stagingBuffer = VK_NULL_HANDLE;
	VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;

	Buffer stagingBufferInfo = {
	    .size = sizeof(indices), 
	    .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
	    .buffer = &stagingBuffer,
	    .memory = &stagingBufferMemory,
	    .propertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
	};
	if (!createBuffer(vulkan, &stagingBufferInfo)) goto clean;

	Buffer indexBufferInfo = {
	    .size = sizeof(indices), 
	    .usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
	    .buffer = &vulkan->indexBuffer,
	    .memory = &vulkan->indexBufferMemory,
	    .propertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
	};
	if (!createBuffer(vulkan, &indexBufferInfo)) goto clean;
	
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
	    logMessage(LOG_ERROR, "Failed to allocate command buffer\n");
	    goto clean;
	}
	const VkCommandBufferBeginInfo beginInfo = {
	    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
	    .pNext = NULL,
	    .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
	    .pInheritanceInfo = NULL
	};
	vulkan->result = vkBeginCommandBuffer(commandCopyBuffer, &beginInfo);
	if (vulkan->result != VK_SUCCESS) {
	    logMessage(LOG_ERROR, "Failed to begin command buffer\n");
	    goto clean;
	}
	const VkBufferCopy bufferCopy = {
	    .srcOffset = 0,
	    .dstOffset = 0,
	    .size = stagingBufferInfo.size
	};
	vkCmdCopyBuffer(commandCopyBuffer, stagingBuffer, vulkan->indexBuffer, 1, &bufferCopy);
	vulkan->result = vkEndCommandBuffer(commandCopyBuffer);
	if (vulkan->result != VK_SUCCESS) {
	    logMessage(LOG_ERROR, "Failed to end command buffer\n");
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
	VkFence fence = VK_NULL_HANDLE;
	const VkFenceCreateInfo fenceInfo = {
	    .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
	    .pNext = NULL,
	    .flags = 0
	};
	vulkan->result = vkCreateFence(vulkan->device, &fenceInfo, NULL, &fence);
	if (vulkan->result != VK_SUCCESS) {
	    logMessage(LOG_ERROR, "Failed to create fence\n");
	    goto clean; 
	}
	vulkan->result = vkQueueSubmit(vulkan->graphicsQueue, 1, &submitInfo, fence);
	if (vulkan->result != VK_SUCCESS) {
	    logMessage(LOG_ERROR, "Failed to submit command buffer\n");
	    goto clean; 
	}
	vulkan->result = vkWaitForFences(vulkan->device, 1, &fence, VK_TRUE, UINT64_MAX);
	if (vulkan->result != VK_SUCCESS) {
	    logMessage(LOG_ERROR, "Failed to wait for fence\n");
	    goto clean; 
	}
	vulkan->result = VK_SUCCESS;
 clean:
	vkDestroyFence(vulkan->device, fence, NULL);
	vkFreeCommandBuffers(vulkan->device, vulkan->commandPool, 1, &commandCopyBuffer);
	vkFreeMemory(vulkan->device, stagingBufferMemory, NULL);
	vkDestroyBuffer(vulkan->device, stagingBuffer, NULL);
	return vulkan->result;
    }
#endif 
#endif
