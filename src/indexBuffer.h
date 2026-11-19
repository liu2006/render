#ifndef INDEX_BUFFER_H_
#define INDEX_BUFFER_H_

#if defined(INDEX_BUFFER_IMPLEMENTATION)
    int createIndexBuffer(Renderer *renderer)
    {
	Vulkan *vulkan = renderer->vulkan;

	uint32_t indices[] = { 0, 1, 2, 2, 3, 0}; 
	VkBuffer stagingBuffer = VK_NULL_HANDLE;
	VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;

	const VkBufferCreateInfo bufferInfo = {
	    .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
	    .pNext = NULL,
	    .flags = 0,
	    .size = sizeof(indices),
	    .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
	    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
	    .queueFamilyIndexCount = 0,
	    .pQueueFamilyIndices = NULL
	};
	
	vulkan->result = vkCreateBuffer(vulkan->device, &bufferInfo, NULL, &stagingBuffer);
	if (vulkan->result != VK_SUCCESS) {
	    logMessage(LOG_ERROR, "Failed to create staging buffer for indices\n");
	    goto clean;
	}

	/* typedef struct VkMemoryRequirements { */
	/*     VkDeviceSize    size; */
	/*     VkDeviceSize    alignment; */
	/*     uint32_t        memoryTypeBits; */
	/* } */
	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(vulkan->device, stagingBuffer, &memoryRequirements);

	/* VkPhysicalDeviceMemoryProperties { */
	/*     uint32_t        memoryTypeCount; */
	/*     VkMemoryType    memoryTypes[VK_MAX_MEMORY_TYPES]; */
	/*     uint32_t        memoryHeapCount; */
	/*     VkMemoryHeap    memoryHeaps[VK_MAX_MEMORY_HEAPS]; */
	/* } VkPhysicalDeviceMemoryProperties; */
	
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
	const VkMemoryAllocateInfo allocateInfo = {
	    .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
	    .pNext = NULL,
	    .allocationSize = memoryRequirements.size,
	    .memoryTypeIndex = stagingMemoryTypeIndex,
	};
	vulkan->result = vkAllocateMemory(vulkan->device, &allocateInfo, NULL, &stagingBufferMemory);
	if (vulkan->result != VK_SUCCESS) {
	    logMessage(LOG_ERROR, "Failed to allocate memory for staging buffer\m");
	    goto clean;
	}
 clean:
	vkFreeMemory(vulkan->device, stagingBufferMemory, NULL);
	vkDestroyBuffer(vulkan->device, stagingBuffer, NULL);
	return vulkan->result;
    }
    
#endif
    
#endif


