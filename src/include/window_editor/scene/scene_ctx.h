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

	bool mouseInSprite;

	bool leftDown;
	bool rightDown;

	bool leftClicked;
	bool rightClicked;

	bool leftReleased;
	bool rightReleased;

	bool leftDragging;
	bool rightDragging;

	bool sceneHovered;
	bool sceneActive;
};

struct SceneViewSelection
{
	EntityId draggingEntity = -1;
	bool dragging = false;
	ImVec2 dragOffset;
};

struct SceneViewContext
{
	SceneViewCamera	camera;
	SceneViewContent content;
	SceneViewInput	input;
	SceneViewSelection selection;
};

void BuildSceneViewContext(SceneViewContext& ctx);

#endif // SCENE_CTX_H