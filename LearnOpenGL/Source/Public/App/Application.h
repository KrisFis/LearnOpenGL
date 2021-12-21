
#pragma once

#include "ASTD.h"

// TODO(kristian.fisera): This class should handle whole app, not only window size
class FApplication
{
private: // Constructors

	FApplication();
	~FApplication();
	
public: // Static getters

	static FApplication& Get();

public: // Getters

	FORCEINLINE uint16 GetWindowWidth() const { return WindowWidth; }
	FORCEINLINE uint16 GetWindowHeight() const { return WindowHeight; }
	FORCEINLINE void GetWindowSize(uint16& OutWidth, uint16& OutHeight) const { OutWidth = WindowWidth; OutHeight = WindowHeight; }

public: // Setters

	void SetWindowSize(uint16 Width, uint16 Height);

private: // Fields
	
	uint16 WindowWidth, WindowHeight;
	
};