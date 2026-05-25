#include "SDLHelper.h"

SDL_GPUGraphicsPipeline* SDLHelper::MakeSimplePipeline(SDL_GPUDevice* aDevice, SDL_Window* aWindow,
													   std::string aVertexShader, std::string aFragmentShader,
													   const std::vector<BufferSpec>& aVertexBufferFormats,
													   const std::vector<BufferSpec>& aUniformVertexBufferFormats,
													   const std::vector<BufferSpec>& aUniformFragmentBufferFormats,
													   SDL_GPUPrimitiveType aType)
{
	size_t vertexSize;
	void* vertexData = SDL_LoadFile(aVertexShader.c_str(), &vertexSize);

	if (!vertexData)
		return nullptr;

	SDL_GPUShaderCreateInfo vertexShaderInfo{.code_size = vertexSize,
											 .code = reinterpret_cast<const Uint8*>(vertexData),
											 .format = SDL_GPU_SHADERFORMAT_SPIRV,
											 .stage = SDL_GPU_SHADERSTAGE_VERTEX,
											 .num_samplers = 0,
											 .num_storage_textures = 0,
											 .num_storage_buffers = 0,
											 .num_uniform_buffers =
												 static_cast<uint32_t>(aUniformVertexBufferFormats.size()),
											 .props = 0};

	SDL_GPUShader* vertexShader = SDL_CreateGPUShader(aDevice, &vertexShaderInfo);
	SDL_free(vertexData);

	if (!vertexShader)
		return nullptr;

	size_t fragmentSize;
	void* fragmentData = SDL_LoadFile(aFragmentShader.c_str(), &fragmentSize);

	if (!fragmentData)
		return nullptr;

	SDL_GPUShaderCreateInfo fragmentShaderInfo{.code_size = fragmentSize,
											   .code = reinterpret_cast<const Uint8*>(fragmentData),
											   .format = SDL_GPU_SHADERFORMAT_SPIRV,
											   .stage = SDL_GPU_SHADERSTAGE_FRAGMENT,
											   .num_samplers = 0,
											   .num_storage_textures = 0,
											   .num_storage_buffers = 0,
											   .num_uniform_buffers =
												   static_cast<uint32_t>(aUniformFragmentBufferFormats.size()),
											   .props = 0};

	SDL_GPUShader* fragmentShader = SDL_CreateGPUShader(aDevice, &fragmentShaderInfo);
	SDL_free(fragmentData);

	if (!fragmentShader)
		return nullptr;

	std::vector<SDL_GPUVertexBufferDescription> vertexBufferDescriptions;
	std::vector<SDL_GPUVertexAttribute> vertexAttributes;

	vertexBufferDescriptions.resize(aVertexBufferFormats.size());

	for (size_t i = 0; i < aVertexBufferFormats.size(); i++)
	{
		size_t totalVertexSize = 0;

		for (size_t specIndex = 0; specIndex < aVertexBufferFormats[i].size(); specIndex++)
		{
			const TypeSpec& spec = aVertexBufferFormats[i][specIndex];

			vertexAttributes.push_back(SDL_GPUVertexAttribute{.location = static_cast<uint32_t>(specIndex),
															  .buffer_slot = static_cast<uint32_t>(i),
															  .format = spec.myFormat,
															  .offset = static_cast<uint32_t>(totalVertexSize)});
			totalVertexSize += spec.mySize;
		}

		vertexBufferDescriptions[i].input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
		vertexBufferDescriptions[i].slot = i;
		vertexBufferDescriptions[i].instance_step_rate = 0;
		vertexBufferDescriptions[i].pitch = totalVertexSize;
	}

	SDL_GPUColorTargetDescription colorTargets{.format = SDL_GetGPUSwapchainTextureFormat(aDevice, aWindow),
											   .blend_state{}};

	SDL_GPUGraphicsPipelineCreateInfo pipelineInfo{
		.vertex_shader = vertexShader,
		.fragment_shader = fragmentShader,
		.vertex_input_state{.vertex_buffer_descriptions = vertexBufferDescriptions.data(),
							.num_vertex_buffers = static_cast<uint32_t>(aVertexBufferFormats.size()),
							.vertex_attributes = vertexAttributes.data(),
							.num_vertex_attributes = static_cast<uint32_t>(vertexAttributes.size())},
		.primitive_type = aType,
		.target_info{

		},
		.props = 0};

	SDL_GPUGraphicsPipeline* pipeline = SDL_CreateGPUGraphicsPipeline(aDevice, &pipelineInfo);

	SDL_ReleaseGPUShader(aDevice, vertexShader);
	SDL_ReleaseGPUShader(aDevice, fragmentShader);

	return pipeline;
}