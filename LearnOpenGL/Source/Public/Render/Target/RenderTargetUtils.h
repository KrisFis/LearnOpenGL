
#pragma once

#include "ModuleMinimal.h"

// Forward declarations
class FTexture;
class FFramebuffer;
class IRenderTarget;
enum class ERenderTargetAttachmentType : uint8;
enum class ETextureType : uint8;

namespace NRenderTargetUtils
{
	TSharedPtr<FTexture> TryGetAsTexture(const TSharedPtr<IRenderTarget>& Target, ETextureType TextureType);
	TArray<TSharedPtr<FTexture>> TryGetAsTextures(const TArray<TSharedPtr<IRenderTarget>>& Targets, ETextureType TextureType);
	
	TSharedPtr<FTexture> TryGetAsTexture(const TSharedPtr<FFramebuffer>& FBTarget, ERenderTargetAttachmentType AttachType, ETextureType TextureType, uint8 Idx);
	TArray<TSharedPtr<FTexture>> TryGetAsTextures(const TSharedPtr<FFramebuffer>& FBTarget, ERenderTargetAttachmentType AttachType, ETextureType TextureType);
}