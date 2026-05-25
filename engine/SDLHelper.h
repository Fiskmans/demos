
#pragma once

#include "SDL3/SDL.h"

#include <string>
#include <vector>

class SDLHelper
{
public:
	struct TypeSpec
	{
		template <typename T> TypeSpec(T&& aVal = {});

		size_t mySize;
        SDL_GPUVertexElementFormat myFormat;
	};

	using BufferSpec = std::vector<TypeSpec>;

	static SDL_GPUGraphicsPipeline* MakeSimplePipeline(SDL_GPUDevice* aDevice, SDL_Window* aWindow, std::string aVertexShader,
													   std::string aFragmentShader,
													   const std::vector<BufferSpec>& aVertexBufferFormats,
													   const std::vector<BufferSpec>& aUniformVertexBufferFormats,
													   const std::vector<BufferSpec>& aUniformFragmentBufferFormats,
                                                        SDL_GPUPrimitiveType aType);
};