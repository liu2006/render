#ifndef M_PIPELINE_H_
#define M_PIPELINE_H_

#if defined(PIPELINE_IMPLEMENTATION)
    int createPipeline(Renderer *renderer) 
    {	
	Vulkan *vulkan = renderer->vulkan;
	PhysicalDevice *physicalDevice = renderer->physicalDevice;
	SwapChain *swapChain = renderer->swapChain;
	Pipeline *pipeline = renderer->pipeline;
	
	uint32_t *code = NULL;	
	size_t codeSize = 0;
	if (!readFile(pipeline->fname, &code, &codeSize)) goto clean;

	const VkShaderModuleCreateInfo shaderModuleInfo = {
	    .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
	    .pNext = NULL,
	    .flags = 0,
	    .codeSize = codeSize,
	    .pCode = code
	};
	vulkan->result = vkCreateShaderModule(vulkan->device,&shaderModuleInfo, NULL, &vulkan->shaderModule);
	if (vulkan->result != VK_SUCCESS) {
	    logMessage(LOG_ERROR, "Failed to create shader module\n");
	    goto clean;
	}
	const VkPipelineShaderStageCreateInfo vertShaderStageInfo = {
	    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
	    .pNext = NULL,
	    .flags = 0,
	    .stage = VK_SHADER_STAGE_VERTEX_BIT,
	    .module = vulkan->shaderModule,
	    .pName = "vertMain",
	    .pSpecializationInfo = NULL
	};
	const VkPipelineShaderStageCreateInfo fragShaderStageInfo = {
	    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
	    .pNext = NULL,
	    .flags = 0,
	    .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
	    .module = vulkan->shaderModule,
	    .pName = "fragMain",
	    .pSpecializationInfo = NULL
	};
	const VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	const VkVertexInputBindingDescription vertexInputBindingDescription = {
	    .binding = 0,
	    .stride = sizeof(Vertex),
	    .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
	};
	const VkVertexInputAttributeDescription vertexInputAttributeDescriptions[2] = {
	    {
		.location = 0,
		.binding = 0,
		.format = VK_FORMAT_R32G32_SFLOAT,
		.offset = offsetof(Vertex, position),
	    }, 
	    {
		.location = 1,
		.binding = 0,
		.format = VK_FORMAT_R32G32B32_SFLOAT,
		.offset = offsetof(Vertex, color),
	    }
	};
	const VkPipelineVertexInputStateCreateInfo vertexInputInfo = {
	    .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
	    .pNext = NULL,
	    .flags = 0,
	    .vertexBindingDescriptionCount = 1,
	    .pVertexBindingDescriptions = &vertexInputBindingDescription,
	    .vertexAttributeDescriptionCount = 2,
	    .pVertexAttributeDescriptions = vertexInputAttributeDescriptions
	};
	const VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {
	    .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
	    .pNext = NULL,
	    .flags = 0,
	    .topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
	    .primitiveRestartEnable = VK_FALSE,
	};
	const VkPipelineViewportStateCreateInfo viewportInfo = {
	    .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
	    .pNext = NULL,
	    .flags = 0,
	    .viewportCount = 1,
	    .pViewports = &(const VkViewport){
		.x = 0.0f,
		.y = 0.0f,
		.width = (float)swapChain->surfaceExtent.width,
		.height = (float)swapChain->surfaceExtent.height,
		.minDepth = 0.0f,
		.maxDepth = 1.0f
	    },
	    .scissorCount = 1,
	    .pScissors = &(const VkRect2D){
		.offset = {
		    .x = 0,
		    .y = 0
		},
		.extent = {
		    .width = swapChain->surfaceExtent.width,
		    .height = swapChain->surfaceExtent.height
		}
	    }
	};
	const VkPipelineRasterizationStateCreateInfo rasterizationInfo = {
	    .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
	    .pNext = NULL,
	    .flags = 0,
	    .depthClampEnable = VK_FALSE,
	    .rasterizerDiscardEnable = VK_FALSE,
	    .polygonMode = VK_POLYGON_MODE_FILL,
	    .cullMode = VK_CULL_MODE_BACK_BIT,
	    .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
	    .depthBiasEnable = VK_FALSE,
	    .depthBiasConstantFactor = 0.0f,
	    .depthBiasClamp = 0.0f,
	    .depthBiasSlopeFactor = 0.0f,
	    .lineWidth = 1.0f
	};
	const VkPipelineMultisampleStateCreateInfo multisampleInfo = {
	    .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
	    .pNext = NULL,
	    .flags = 0,
	    .rasterizationSamples = physicalDevice->msaaSamples,
	    .sampleShadingEnable = VK_TRUE,
	    .minSampleShading = 1.0f,
	    .pSampleMask = NULL,
	    .alphaToCoverageEnable = VK_FALSE,
	    .alphaToOneEnable = VK_FALSE
	};
	const VkPipelineColorBlendAttachmentState colorBlendAttachment = {
	    .blendEnable = VK_FALSE,
	    .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
	    .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
	    .colorBlendOp = VK_BLEND_OP_ADD,
	    .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
	    .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
	    .alphaBlendOp = VK_BLEND_OP_ADD,
	    .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
		VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
	};
	const VkPipelineColorBlendStateCreateInfo colorBlendInfo = {
	    .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
	    .pNext = NULL,
	    .flags = 0,
	    .logicOpEnable = VK_FALSE,
	    .logicOp = VK_LOGIC_OP_COPY,
	    .attachmentCount = 1,
	    .pAttachments = &colorBlendAttachment,
	    .blendConstants = { 1.0f, 1.0f, 1.0f, 1.0f }
	};
	const VkPipelineRenderingCreateInfo renderingInfo = {
	    .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
	    .pNext = NULL,
	    .viewMask = 0,
	    .colorAttachmentCount = 1,
	    .pColorAttachmentFormats = &swapChain->formats[swapChain->formatIndex].format,
	    .depthAttachmentFormat = VK_FORMAT_UNDEFINED,
	    .stencilAttachmentFormat = VK_FORMAT_UNDEFINED,
	};
	const VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
	const VkPipelineDynamicStateCreateInfo dynamicStateInfo = {
	    .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
	    .pNext = NULL,
	    .flags = 0,
	    .dynamicStateCount = sizeof(dynamicStates) / sizeof(*dynamicStates),
	    .pDynamicStates = dynamicStates
	};
	const VkPipelineLayoutCreateInfo layoutInfo = {
	    .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
	    .pNext = NULL,
	    .flags = 0,
	    .setLayoutCount = 0,
	    .pSetLayouts = NULL,
	    .pushConstantRangeCount = 0,
	    .pPushConstantRanges = NULL,
	};
	
	vulkan->result = vkCreatePipelineLayout(vulkan->device, &layoutInfo, NULL, &vulkan->pipelineLayout);
	if (vulkan->result != VK_SUCCESS) {
	    logMessage(LOG_ERROR, "Failed to create pipeline layout\n");
	    goto clean;
	}
	const VkGraphicsPipelineCreateInfo graphicsPipelineInfo = {
	    .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
	    .pNext = &renderingInfo,
	    .flags = 0,
	    .stageCount = sizeof(shaderStages) / sizeof(shaderStages[0]),
	    .pStages = shaderStages,
	    .pVertexInputState = &vertexInputInfo,
	    .pInputAssemblyState = &inputAssemblyInfo,
	    .pTessellationState = NULL,
	    .pViewportState = &viewportInfo,
	    .pRasterizationState = &rasterizationInfo,
	    .pMultisampleState = &multisampleInfo,
	    .pDepthStencilState = NULL,
	    .pColorBlendState = &colorBlendInfo,
	    .pDynamicState = &dynamicStateInfo,
	    .layout = vulkan->pipelineLayout,
	    .renderPass = VK_NULL_HANDLE,
	    .subpass = 0,
	    .basePipelineHandle = VK_NULL_HANDLE,
	    .basePipelineIndex = -1
	};
	vulkan->result = vkCreateGraphicsPipelines(vulkan->device,
						   VK_NULL_HANDLE, 1, &graphicsPipelineInfo, NULL, &vulkan->graphicsPipeline);
	if (vulkan->result != VK_SUCCESS) {
	    logMessage(LOG_ERROR, "Failed to create graphics pipeline\n");
	    goto clean;
	}
	vulkan->result = VK_SUCCESS;
	logMessage(LOG_INFO, "Graphics pipeline created\n ");
 clean:
	free(code);
	return vulkan->result;
    }

    bool readFile(const char *fname, uint32_t **code, size_t *codeSize) {
	FILE *shaderFile = NULL;
	shaderFile = fopen(fname, "rb");
	if (!shaderFile) {
	    logMessage(LOG_ERROR, "Failed to open shader file\n");
	    return false;
	}
	if (fseek(shaderFile, 0, SEEK_END) != 0) {
	    logMessage(LOG_ERROR, "Failed to seek shader file\n");
	    fclose(shaderFile);
	    return false;
	}
	long int pos = ftell(shaderFile);
	if (pos == -1L) {
	    logMessage(LOG_ERROR, "Failed to get shader file size\n");
	    fclose(shaderFile);
	    return false;
	}
	*codeSize = (size_t)pos;
	if (*codeSize % sizeof(**code) != 0 || *codeSize == 0) {
	    logMessage(LOG_ERROR, "Invalid SPIR-V file size\n");
	    fclose(shaderFile);
	    return false;
	}
	*code = malloc(*codeSize);
	if (!*code) {
	    logMessage(LOG_ERROR, "Failed to malloc memory\n");
	    fclose(shaderFile);
	    return false;
	}
	if (fseek(shaderFile, 0, SEEK_SET) != 0) {
	    logMessage(LOG_ERROR, "Failed to seek shader file\n");
	    fclose(shaderFile);
	    return false;
	}
	size_t count = *codeSize / sizeof(**code);
	size_t readCount = fread(*code, sizeof(**code), count, shaderFile);
	if (readCount != count) {
	    logMessage(LOG_ERROR, "Failed to read shader file\n");
	    fclose(shaderFile);
	    return false;
	}
	logMessage(LOG_INFO, "Shader file loaded\n");
	fclose(shaderFile);
	return true;
    }
#endif

#endif
