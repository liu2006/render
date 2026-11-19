#ifndef BUFFER_H_
#define BUFFER_H_

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
	    logMessage(LOG_ERROR, "Failed to allocate buffer memory\m");
	    return false;
	}

	vulkan->result = vkBindBufferMemory(vulkan->device, *buffer->buffer, *buffer->memory, 0);
	if (vulkan->result != VK_SUCCESS) {
	    logMessage(LOG_ERROR, "Failed to bind buffer memory\n");
	    return false;
	}
	return true;
    }

#endif 
#endif
