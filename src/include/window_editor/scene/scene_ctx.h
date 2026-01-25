#ifndef SCENE_CTX_H
#define SCENE_CTX_H

#include "include/component/componentDefaults.h"

#include <imgui.h>

struct SceneViewCamera
{
	ImVec2 viewOffset = { 0,0 };
	float zoom = 1.0f;
};

struct SceneViewContent
{
	ImVec2 screenPos;
	ImVec2 size;
	ImVec2 fbScale;
};

struct SceneViewInput
{
	ImVec2 mouseScreen;

	bool mouseInSprite		= false;
	bool isSceneCaptured	= false;

	bool leftDown			= false;
	bool rightDown			= false;

	bool leftClicked		= false;
	bool rightClicked		= false;

	bool leftReleased		= false;
	bool rightReleased		= false;

	bool leftDragging		= false;
	bool rightDragging		= false;

	bool sceneHovered		= false;
	bool sceneActive		= false;
};

struct SceneViewContext
{
	SceneViewCamera*	camera = nullptr;
	SceneViewContent	content;
	SceneViewInput		input;
};

void BuildSceneViewContext(SceneViewContext& ctx);

#endif // SCENE_CTX_H