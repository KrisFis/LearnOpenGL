
#pragma once

#include "ModuleMinimal.h"

// Forward declaration
class IRenderTarget;
enum class ERenderTargetAttachmentType : uint8;

struct FFramebufferCopyArgs
{
	struct FSubData
	{
		glm::vec<2, uint16, glm::defaultp> Pos;
		glm::vec<2, uint16, glm::defaultp> Size;
	};
	
	FSubData Source;
	FSubData Destination;
	
	enum { DT_Color, DT_Depth, DT_Stencil } DataType;
	enum { FT_Nearest, FT_Linear } FilterType;
};

class FFramebuffer
{
	
private: // Constructors

	explicit FFramebuffer();
	
public: // Destructor
	
	virtual ~FFramebuffer();

public: // Static constructions

	FORCEINLINE static TSharedPtr<FFramebuffer> Create()
	{ return MakeShareable(new FFramebuffer()); }

public: // Getters

	FORCEINLINE bool IsEnabled() const { return bIsEnabled; }
	FORCEINLINE bool HasChanges() const { return bHasChanges; }
	FORCEINLINE FFramebufferId GetId() const { return Id; }
	
public: // Getters -> Attachment

	TArray<FRenderTargetPtr> GetAttachments(const ERenderTargetAttachmentType Type) const;

public: // Use methods

	void Enable();
	void Disable();

public: // Copy methods

	bool CopyTo(const TSharedPtr<FFramebuffer>& Destination, const FFramebufferCopyArgs& CopyArgs) const;

public: // Attach methods

	int16 Attach(const FRenderTargetPtr& Target);
	
	void DetachAll();
	void DetachAll(ERenderTargetAttachmentType Type);
	bool Detach(const FRenderTargetPtr& Target);

private: // Fields

	FFramebufferId Id;

private: // Targets

	TFastMap<ERenderTargetAttachmentType, TArray<FRenderTargetPtr>> Targets;

private: // Primitive fields

	uint8 bIsEnabled : 1;
	uint8 bHasChanges : 1;
};

typedef TSharedPtr<FFramebuffer> FFramebufferPtr;