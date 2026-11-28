#ifndef M_PHYSICAL_DEVICE_H_
#define M_PHYSICAL_DEVICE_H_

#if defined(PHYSICAL_DEVICE_IMPLEMENTATION)
    extern const char *const requiredDeviceExtensions[];
    extern const size_t deviceExtensionCount;

    typedef struct {
	VkResult taskRes;
	Vulkan *vulkan;
	PhysicalDevice *physicalDevice;
    } QueueTask;

    void *findQueueFamily(void *arg) 
    {
	QueueTask *queueTask = (QueueTask *)arg;
	Vulkan *vulkan = queueTask->vulkan;
	PhysicalDevice *physicalDevice = queueTask->physicalDevice;
	VkResult localRes = queueTask->taskRes;

	uint32_t queueFamilyPropertyCount;
	VkQueueFamilyProperties *queueFamilyProperties = NULL;

	vkGetPhysicalDeviceQueueFamilyProperties(vulkan->physicalDevice, &queueFamilyPropertyCount, NULL);
	queueFamilyProperties = malloc(sizeof(*queueFamilyProperties) * queueFamilyPropertyCount);
	if (!queueFamilyProperties) {
	    localRes = VK_ERROR_OUT_OF_HOST_MEMORY;
	    logMessage(LOG_ERROR, "Failed to malloc memory\n");
	    goto clean;
	}
	vkGetPhysicalDeviceQueueFamilyProperties(vulkan->physicalDevice, &queueFamilyPropertyCount, queueFamilyProperties);
	bool supportGraphics = false;
	for (uint32_t i = 0; i < queueFamilyPropertyCount; ++i) {
	    if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
		physicalDevice->graphicsIndex = i;
		supportGraphics = true;
		break;
	    }
	}
	if (!supportGraphics) {
	    localRes = VK_ERROR_INITIALIZATION_FAILED;
	    logMessage(LOG_ERROR, "Graphics queue not supported\n");
	    goto clean;
	}
	
 clean:
	free(queueFamilyProperties);    
	queueTask->taskRes = localRes; 
	return NULL;
    }

    int pickPhysicalDevice(Renderer *renderer) 
    {
	Vulkan *vulkan = renderer->vulkan;
	PhysicalDevice *physicalDevice = renderer->physicalDevice;

	uint32_t physicalDeviceCount;
	VkPhysicalDevice *physicalDevices = NULL;    

	uint32_t extensionPropertyCount;
	VkExtensionProperties *extensionProperties = NULL;

	vulkan->result = vkEnumeratePhysicalDevices(vulkan->instance, &physicalDeviceCount, NULL);
    
	if (vulkan->result != VK_SUCCESS || physicalDeviceCount == 0) {
	    if (vulkan->result == VK_SUCCESS) vulkan->result = VK_ERROR_INITIALIZATION_FAILED;
	    logMessage(LOG_ERROR, "Physical Device not found\n");
	    goto clean;
	}
	physicalDevices = malloc(sizeof(*physicalDevices) * physicalDeviceCount);
	if (!physicalDevices) {
	    vulkan->result = VK_ERROR_OUT_OF_HOST_MEMORY;
	    logMessage(LOG_ERROR, "Failed to malloc memory\n");
	    goto clean;
	}
	
	vulkan->result = vkEnumeratePhysicalDevices(vulkan->instance, &physicalDeviceCount, physicalDevices);
	if (vulkan->result != VK_SUCCESS || physicalDeviceCount == 0) {
	    if (vulkan->result == VK_SUCCESS) vulkan->result = VK_ERROR_INITIALIZATION_FAILED;
	    logMessage(LOG_ERROR, "Physical Device not found\n");
	    goto clean;
	}
	VkPhysicalDeviceProperties physicalDeviceProperties;
	for (uint32_t i = 0; i < physicalDeviceCount; ++i) {
	    vkGetPhysicalDeviceProperties(physicalDevices[i], &physicalDeviceProperties);
	    logMessage(LOG_INFO, "Available GPU: [%s]\n", physicalDeviceProperties.deviceName);
	    if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
		vulkan->physicalDevice = physicalDevices[i];
		break;
	    }
	}
	if (vulkan->physicalDevice == VK_NULL_HANDLE ) {	
	    vulkan->physicalDevice = physicalDevices[0];
	}
	vkGetPhysicalDeviceProperties(vulkan->physicalDevice, &physicalDeviceProperties);
	logMessage(LOG_INFO, "Selected GPU: [%s]\n", physicalDeviceProperties.deviceName);

	QueueTask queueTask = {
	    .vulkan = vulkan,
	    .physicalDevice = physicalDevice,
	};
	pthread_t queueThread;
	bool threadCreated = false;
	bool joined = false;

	if (pthread_create(&queueThread, NULL, findQueueFamily, &queueTask) != 0) {
	    vulkan->result = VK_ERROR_INITIALIZATION_FAILED;
	    logMessage(LOG_ERROR, "Failed to create queue thread\n");
	    goto clean;
	}
	threadCreated = true;
	
	const VkSampleCountFlagBits sampleCounts[] = {
	    VK_SAMPLE_COUNT_64_BIT,
	    VK_SAMPLE_COUNT_32_BIT,
	    VK_SAMPLE_COUNT_16_BIT,
	    VK_SAMPLE_COUNT_8_BIT,
	    VK_SAMPLE_COUNT_4_BIT,
	    VK_SAMPLE_COUNT_2_BIT
	};
	VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts &
		physicalDeviceProperties.limits.framebufferDepthSampleCounts;

	for (size_t i = 0; i < sizeof(sampleCounts) / sizeof(*sampleCounts); ++i) {
	    if (counts & sampleCounts[i]) {
		physicalDevice->msaaSamples = sampleCounts[i];
		break;
	    }
	}
	logMessage(LOG_INFO, "Selected MSAA [x%u] samples\n", physicalDevice->msaaSamples);

	vulkan->result = vkEnumerateDeviceExtensionProperties(vulkan->physicalDevice, NULL, &extensionPropertyCount, NULL);
	if (vulkan->result != VK_SUCCESS) {
	    logMessage(LOG_ERROR, "Failed to enumerate device extension properties [%d]\n", vulkan->result);
	    goto clean;
	}
	extensionProperties = malloc(sizeof(*extensionProperties) * extensionPropertyCount);
	if (!extensionProperties) {
	    vulkan->result = VK_ERROR_OUT_OF_HOST_MEMORY;
	    logMessage(LOG_ERROR, "Failed to malloc memory\n");
	    goto clean;
	}
	vulkan->result = vkEnumerateDeviceExtensionProperties(vulkan->physicalDevice, NULL, &extensionPropertyCount, extensionProperties);
	if (vulkan->result != VK_SUCCESS) {
	    logMessage(LOG_ERROR, "Failed to enumerate device extension properties [%d]\n", vulkan->result);
	    goto clean;
	}
	
	physicalDevice->deviceExtensions = malloc(sizeof(*physicalDevice->deviceExtensions) * physicalDevice->deviceExtensionCount);
	physicalDevice->deviceExtensionCount = deviceExtensionCount;
	if (!physicalDevice->deviceExtensions) {
	    vulkan->result = VK_ERROR_OUT_OF_HOST_MEMORY;
	    logMessage(LOG_ERROR, "Failed to malloc memory\n");
	    goto clean;
	}
	for (size_t i = 0; i < physicalDevice->deviceExtensionCount; ++i) {
	    physicalDevice->deviceExtensions[i] = strdup(requiredDeviceExtensions[i]);
	    if (!physicalDevice->deviceExtensions[i]) {
		vulkan->result = VK_ERROR_OUT_OF_HOST_MEMORY;
		logMessage(LOG_ERROR, "Failed to malloc memory\n");
		goto clean;
	    }
	}
	for (size_t i = 0; i < physicalDevice->deviceExtensionCount; ++i) {
	    bool found = false;
	    for (uint32_t j = 0; j < extensionPropertyCount; ++j) {
		if (strcmp(extensionProperties[j].extensionName, physicalDevice->deviceExtensions[i]) == 0) {
		    logMessage(LOG_INFO, "Enabled Device extension: [%s]\n", physicalDevice->deviceExtensions[i]);
		    found = true;
		    break;
		}
	    }
	    if (!found) {
		vulkan->result = VK_ERROR_INITIALIZATION_FAILED;
		logMessage(LOG_ERROR, "Required physical device extension not supported: [%s]\n", requiredDeviceExtensions[i]);
		goto clean;
	    }
	}
	if ((pthread_join(queueThread, NULL)) != 0) {
	    logMessage(LOG_ERROR, "Failed to join queue thread\n");
	    vulkan->result = VK_ERROR_INITIALIZATION_FAILED; 
	    goto clean;
	}
	joined = true;
	if (queueTask.taskRes != VK_SUCCESS) {
	    vulkan->result = queueTask.taskRes;
	    logMessage(LOG_ERROR, "Graphics queue not supported [%d]\n", vulkan->result);
	    goto clean;
	}
	vulkan->result = VK_SUCCESS;
 clean:
	if (threadCreated && !joined) pthread_join(queueThread, NULL);
	free(extensionProperties);
	free(physicalDevices);
	return vulkan->result;
    }
#endif
    
#endif
