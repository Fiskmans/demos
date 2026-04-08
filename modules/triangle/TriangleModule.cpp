#include "TriangleModule.h"

#include "GL/gl.h"
#include "SDL3/SDL.h"

TriangleModule::TriangleModule(Engine* aEngine)
{
	myEngine = aEngine;

	myUpdateHandle = aEngine->OnUpdate.Register(std::bind(&TriangleModule::Update, this, std::placeholders::_1));
	//myPaintHandle = aEngine->OnPaint.Register(
	//	std::bind(&TriangleModule::Paint, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	myImGuiHandle = aEngine->RegisterImgui("Triangle", std::bind(&TriangleModule::ImGui, this));
	float vertices[]{-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f};
	return;
	SDL_GPUDevice* device = aEngine->GetDevice();
	
	SDL_GPUBufferCreateInfo bufferInfo;
	
	bufferInfo.props = 0;
	bufferInfo.size = sizeof(vertices);
	bufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
	
	myVertexBuffer = SDL_CreateGPUBuffer(device, &bufferInfo);
	
	if (!myVertexBuffer)
	{
		SDL_Log("Failed to create vertex buffer: %s", SDL_GetError());
		return;
	}
	
	SDL_GPUCommandBuffer* commands = SDL_AcquireGPUCommandBuffer(device);
	
	if (!commands)
	{
		SDL_Log("Failed to acquire command buffer: %s", SDL_GetError());
		return;
	}
	
	SDL_GPUTransferBufferCreateInfo transferInfo;
	
	transferInfo.size = sizeof(vertices);
	transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
	
	SDL_GPUTransferBuffer* transferbuffer = SDL_CreateGPUTransferBuffer(device, &transferInfo);
	if (!transferbuffer)
	{
		SDL_Log("Failed to create transferbuffer: %s", SDL_GetError());
		return;
	}
	
	void* transferMemory = SDL_MapGPUTransferBuffer(device, transferbuffer, false);
	if (!transferMemory)
	{
		SDL_Log("Failed to map transferbuffer: %s", SDL_GetError());
		return;
	}
	
	memcpy(transferMemory, vertices, sizeof(vertices));
	SDL_UnmapGPUTransferBuffer(device, transferbuffer);
	
	SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(commands);
	
	SDL_GPUTransferBufferLocation transferLocation;
	
	transferLocation.transfer_buffer = transferbuffer;
	transferLocation.offset = 0;
	
	SDL_GPUBufferRegion bufferRegion;
	
	bufferRegion.buffer = myVertexBuffer;
	bufferRegion.offset = 0;
	
	SDL_UploadToGPUBuffer(copyPass, &transferLocation, &bufferRegion, false);
	SDL_EndGPUCopyPass(copyPass);
	
	if (!SDL_SubmitGPUCommandBuffer(commands))
	{
		SDL_Log("Failed to submit triangle setup commands: %s", SDL_GetError());
		return;
	}
	
	SDL_GPUShader* vertexShader;
	SDL_GPUShader* fragmentShader;
	{
		// vertex shader
		size_t vertexShaderFileSize;
		void* vertexShaderFileData = SDL_LoadFile("shaders/triangle/Vertex.spv", &vertexShaderFileSize);
		if (!vertexShaderFileData)
		{
			SDL_Log("Failed to load vertex shader file: %s", SDL_GetError());
			return;
		}
		
		SDL_GPUShaderCreateInfo vertexShaderInfo{.code_size = vertexShaderFileSize,
												 .code = reinterpret_cast<const Uint8*>(vertexShaderFileData),
												 .entrypoint = "main",
												 .format = SDL_GPU_SHADERFORMAT_SPIRV,
												 .stage = SDL_GPU_SHADERSTAGE_VERTEX,
												 .num_samplers = 0,
												 .num_storage_textures = 0,
												 .num_storage_buffers = 0,
												 .num_uniform_buffers = 0};

		vertexShader = SDL_CreateGPUShader(device, &vertexShaderInfo);

		SDL_free(vertexShaderFileData);

		if (!vertexShader)
		{
			SDL_Log("Failed to create vertex shader: %s", SDL_GetError());
			return;
		}
	}

	{
		// fragment shader
		size_t fragmentShaderFileSize;
		void* fragmentShaderFileData = SDL_LoadFile("shaders/triangle/Fragment.spv", &fragmentShaderFileSize);
		if (!fragmentShaderFileData)
		{
			SDL_Log("Failed to load fragment shader file: %s", SDL_GetError());
			return;
		}

		SDL_GPUShaderCreateInfo fragmentShaderInfo{.code_size = fragmentShaderFileSize,
												   .code = reinterpret_cast<const Uint8*>(fragmentShaderFileData),
												   .entrypoint = "main",
												   .format = SDL_GPU_SHADERFORMAT_SPIRV,
												   .stage = SDL_GPU_SHADERSTAGE_FRAGMENT,
												   .num_samplers = 0,
												   .num_storage_textures = 0,
												   .num_storage_buffers = 0,
												   .num_uniform_buffers = 0};

		fragmentShader = SDL_CreateGPUShader(device, &fragmentShaderInfo);

		SDL_free(fragmentShaderFileData);

		if (!fragmentShader)
		{
			SDL_Log("Failed to create fragment shader: %s", SDL_GetError());
			return;
		}
	}

	{
		SDL_GPUVertexBufferDescription vertexBufferDesc{.slot = 0,
														.pitch = sizeof(float) * 3,
														.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
														.instance_step_rate = 0};

		SDL_GPUVertexAttribute vertexBufferAttributes{
			.location = 0, .buffer_slot = 0, .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, .offset = 0};

		SDL_GPUColorTargetDescription colorTargetDesc{
			.format = SDL_GetGPUSwapchainTextureFormat(device, aEngine->GetWindow()), .blend_state{}};

		SDL_GPUGraphicsPipelineCreateInfo pipelineInfo{
			.vertex_shader = vertexShader,
			.fragment_shader = fragmentShader,
			.vertex_input_state{.vertex_buffer_descriptions = &vertexBufferDesc,
								.num_vertex_buffers = 1,

								.vertex_attributes = &vertexBufferAttributes,
								.num_vertex_attributes = 1},
			.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
			.target_info{.color_target_descriptions = &colorTargetDesc,
						 .num_color_targets = 1,
						 .has_depth_stencil_target = false}};

		myGraphicsPipeline = SDL_CreateGPUGraphicsPipeline(device, &pipelineInfo);
		if (!myGraphicsPipeline)
		{
			SDL_Log("Failed to create graphics pipeline: %s", SDL_GetError());
			return;
		}
	}

	SDL_ReleaseGPUTransferBuffer(device, transferbuffer);
}

TriangleModule::~TriangleModule()
{
	SDL_ReleaseGPUBuffer(myEngine->GetDevice(), myVertexBuffer);
	SDL_ReleaseGPUGraphicsPipeline(myEngine->GetDevice(), myGraphicsPipeline);
}

void TriangleModule::Update(Engine::TimeDelta aDelta)
{
}

void TriangleModule::Paint(SDL_GPUDevice* aDevice, SDL_GPUCommandBuffer* aCommandBuffer, SDL_GPUTexture* aSwapTexture)
{
	SDL_GPUColorTargetInfo colorInfo{
		.texture = aSwapTexture, .load_op = SDL_GPU_LOADOP_LOAD, .store_op = SDL_GPU_STOREOP_STORE};

	SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(aCommandBuffer, &colorInfo, 1, nullptr);

	SDL_GPUBufferBinding binding{.buffer = myVertexBuffer, .offset = 0};
	SDL_BindGPUVertexBuffers(renderPass, 0, &binding, 1);
	SDL_BindGPUGraphicsPipeline(renderPass, myGraphicsPipeline);
	SDL_DrawGPUPrimitives(renderPass, 3, 1, 0, 0);

	SDL_EndGPURenderPass(renderPass);
}

void TriangleModule::ImGui()
{
	ImGui::TextUnformatted("Triangle :()");
}
