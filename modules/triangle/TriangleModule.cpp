#include "TriangleModule.h"

#include "GL/gl.h"
#include "SDL3/SDL.h"

TriangleModule::TriangleModule(Engine* aEngine)
{
	myEngine = aEngine;
	myNeedsVertexUpload = true;
	myTimeLeft = std::chrono::milliseconds(300);
	
	SDL_GPUDevice* device = aEngine->GetDevice();
	
	myTri = {.myA{.myPos{-0.5f, -0.5f, 0.0f, 1.0f}, .myColor{1, 0, 0, 1.0f}},
	.myB{.myPos{0.5f, -0.5f, 0.0f, 1.0f}, .myColor{0, 1, 0, 1.0f}},
	.myC{.myPos{0.0f, 0.5f, 0.0f, 1.0f}, .myColor{0, 0, 1, 1.0f}}};
	
	SDL_GPUBufferCreateInfo bufferInfo;
	
	bufferInfo.props = 0;
	bufferInfo.size = Tri::SerializedSize;
	bufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
	
	myVertexBuffer = SDL_CreateGPUBuffer(device, &bufferInfo);
	
	if (!myVertexBuffer)
	{
		SDL_Log("Failed to create vertex buffer: %s", SDL_GetError());
		return;
	}
	
	SDL_GPUTransferBufferCreateInfo transferInfo;
	
	transferInfo.size = Tri::SerializedSize;
	transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
	
	myVertexTransferBuffer = SDL_CreateGPUTransferBuffer(device, &transferInfo);
	if (!myVertexTransferBuffer)
	{
		SDL_Log("Failed to create transferbuffer: %s", SDL_GetError());
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
														.pitch = Vertex::SerializedSize,
														.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
														.instance_step_rate = 0};

		SDL_GPUVertexAttribute vertexBufferAttributes[2] = {
			{.location = 0, .buffer_slot = 0, .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4, .offset = 0},
			{.location = 1, .buffer_slot = 0, .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4, .offset = sizeof(float) * 4}};

		SDL_GPUColorTargetDescription colorTargetDesc{
			.format = SDL_GetGPUSwapchainTextureFormat(device, aEngine->GetWindow()), .blend_state{}};

		SDL_GPUGraphicsPipelineCreateInfo pipelineInfo{
			.vertex_shader = vertexShader,
			.fragment_shader = fragmentShader,
			.vertex_input_state{.vertex_buffer_descriptions = &vertexBufferDesc,
								.num_vertex_buffers = 1,

								.vertex_attributes = vertexBufferAttributes,
								.num_vertex_attributes = 2},
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

	SDL_ReleaseGPUShader(device, vertexShader);
	SDL_ReleaseGPUShader(device, fragmentShader);


	myUpdateHandle = aEngine->OnUpdate.Register(std::bind(&TriangleModule::Update, this, std::placeholders::_1));
	myPaintHandle = aEngine->OnPaint.Register(
		std::bind(&TriangleModule::Paint, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	myImGuiHandle = aEngine->RegisterImgui("Triangle", std::bind(&TriangleModule::ImGui, this));
}

TriangleModule::~TriangleModule()
{
	SDL_ReleaseGPUBuffer(myEngine->GetDevice(), myVertexBuffer);
	SDL_ReleaseGPUGraphicsPipeline(myEngine->GetDevice(), myGraphicsPipeline);
	SDL_ReleaseGPUTransferBuffer(myEngine->GetDevice(), myVertexTransferBuffer);
}

void TriangleModule::Update(Engine::TimeDelta aDelta)
{
	myTimeLeft -= std::chrono::duration_cast<std::chrono::microseconds>(aDelta);
	if (myTimeLeft < std::chrono::microseconds(0))
	{
		myEngine->Shutdown();
	}
	
}

void TriangleModule::Paint(SDL_GPUDevice* aDevice, SDL_GPUCommandBuffer* aCommandBuffer, SDL_GPUTexture* aSwapTexture)
{
	if (myNeedsVertexUpload)
	{
		UploadVertexBuffer(aCommandBuffer);
		myNeedsVertexUpload = false;
	}

	return;

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
	if (myTri.ImGui())
		myNeedsVertexUpload = true;
}

void TriangleModule::UploadVertexBuffer(SDL_GPUCommandBuffer* aCommandBuffer)
{
	void* transferMemory = SDL_MapGPUTransferBuffer(myEngine->GetDevice(), myVertexTransferBuffer, false);
	if (!transferMemory)
	{
		SDL_Log("Failed to map transferbuffer: %s", SDL_GetError());
		return;
	}

	std::byte* writeHead = reinterpret_cast<std::byte*>(transferMemory);
	myTri.Serialize(writeHead);
	
	SDL_UnmapGPUTransferBuffer(myEngine->GetDevice(), myVertexTransferBuffer);
	
	SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(aCommandBuffer);
	if (!copyPass)
	{
		SDL_Log("Failed to create copy pass: %s", SDL_GetError());
		return;
	}

	SDL_GPUTransferBufferLocation transferLocation;

	transferLocation.transfer_buffer = myVertexTransferBuffer;
	transferLocation.offset = 0;

	SDL_GPUBufferRegion bufferRegion;

	bufferRegion.buffer = myVertexBuffer;
	bufferRegion.offset = 0;

	SDL_UploadToGPUBuffer(copyPass, &transferLocation, &bufferRegion, false);
	SDL_EndGPUCopyPass(copyPass);

	SDL_Log("Refreshed vertex buffer");
}

void TriangleModule::Tri::Serialize(std::byte*& aInOutDestination)
{
	myA.Serialize(aInOutDestination);
	myB.Serialize(aInOutDestination);
	myC.Serialize(aInOutDestination);
}

bool TriangleModule::Tri::ImGui()
{
	bool r = false;
	ImGui::PushID(this);
	ImGui::TextUnformatted("A");
	r |= myA.ImGui();
	ImGui::TextUnformatted("B");
	r |= myB.ImGui();
	ImGui::TextUnformatted("C");
	r |= myC.ImGui();
	ImGui::PopID();

	return r;
}

void TriangleModule::Vertex::Serialize(std::byte*& aInOutDestination)
{
	TriangleModule::SerializeVar(aInOutDestination, myPos[0]);
	TriangleModule::SerializeVar(aInOutDestination, myPos[1]);
	TriangleModule::SerializeVar(aInOutDestination, myPos[2]);
	TriangleModule::SerializeVar(aInOutDestination, myPos[3]);
	TriangleModule::SerializeVar(aInOutDestination, myColor[0]);
	TriangleModule::SerializeVar(aInOutDestination, myColor[1]);
	TriangleModule::SerializeVar(aInOutDestination, myColor[2]);
	TriangleModule::SerializeVar(aInOutDestination, myColor[3]);
}

bool TriangleModule::Vertex::ImGui()
{
	bool r = false;
	ImGui::PushID(this);
	r |= ImGui::DragFloat4("Pos", myPos, 0.01f, -1.f, 1.f);
	r |= ImGui::ColorEdit4("Color", myColor);
	ImGui::PopID();
	return r;
}
