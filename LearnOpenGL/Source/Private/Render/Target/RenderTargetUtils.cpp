
#include "RenderTargetUtils.h"

#include "Framebuffer.h"
#include "Texture.h"
#include "RenderTexture.h"

namespace NRenderTargetUtils
{
	TSharedPtr<FTexture> TryGetAsTexture(const TSharedPtr<IRenderTarget>& Target, ETextureType TextureType)
	{
		if(!Target.IsValid() || Target->GetType() != ERenderTargetType::RenderTexture) return nullptr;
	
		return FTexture::Create(static_cast<FRenderTexture*>(Target.Get()), TextureType);
	}

	TArray<TSharedPtr<FTexture>> TryGetAsTextures(const TArray<TSharedPtr<IRenderTarget>>& Targets, ETextureType TextureType)
	{
		TArray<FTexturePtr> resultArray;
		for(const FRenderTargetPtr& target : Targets)
		{
			FTexturePtr result = TryGetAsTexture(target, TextureType);
			if(result.IsValid())
			{
				resultArray.push_back(result);
			}
		}
		
		return resultArray;
	}
	
	TSharedPtr<FTexture> TryGetAsTexture(const TSharedPtr<FFramebuffer>& FBTarget, ERenderTargetAttachmentType AttachType, ETextureType TextureType, uint8 Idx)
	{
		if(!FBTarget.IsValid() || AttachType == ERenderTargetAttachmentType::Invalid) return nullptr;
	
		TArray<FRenderTargetPtr> attachments = FBTarget->GetAttachments(AttachType);
	
		if(Idx >= attachments.size()) return nullptr;
		
		return TryGetAsTexture(attachments[Idx], TextureType);
	}

	TArray<TSharedPtr<FTexture>> TryGetAsTextures(const TSharedPtr<FFramebuffer>& FBTarget, ERenderTargetAttachmentType AttachType, ETextureType TextureType)
	{
		if(!FBTarget.IsValid() || AttachType == ERenderTargetAttachmentType::Invalid) return {};
	
		return TryGetAsTextures(FBTarget->GetAttachments(AttachType), TextureType);
	}
}