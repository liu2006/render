#ifndef INDEX_BUFFER_H_
#define INDEX_BUFFER_H_

#if defined(INDEX_BUFFER_IMPLEMENTATION)
    int createIndexBuffer(Renderer *renderer)
    {
	Vulkan *vulkan = renderer->vulkan;

	uint32_t indices[] = { 0, 1, 2, 2, 3, 0}; 
	VkBuffer stagingBuffer = VK_NULL_HANDLE;
	VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;

	const Buffer stagingBufferInfo = {
	    .size = sizeof(indices), 
	    .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
	    .buffer = &stagingBuffer,
	    .memory = &stagingBufferMemory,
	    .propertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
	};
	if (!createBuffer(vulkan, &stagingBufferInfo)) goto clean;

	const Buffer indexBufferInfo = {
	    .size = sizeof(indices), 
	    .usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
	    .buffer = &vulkan->indexBuffer,
	    .memory = &vulkan->indexBufferMemory,
	    .propertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
	};
	if (!createBuffer(vulkan, &indexBufferInfo)) goto clean;


	
	vulkan->result = VK_SUCCESS;
 clean:
	vkFreeMemory(vulkan->device, stagingBufferMemory, NULL);
	vkDestroyBuffer(vulkan->device, stagingBuffer, NULL);
	return vulkan->result;
    }
    
#endif
    
#endif


