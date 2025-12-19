#pragma once
#include <cstdint>

// インスペクターウィンドウに表示するコンポーネント
void RenderTransformInspector(int64_t entityId);
void RenderMoveInspector(int64_t entityId);
void RenderIsPlayerInspector(int64_t entityId);
void RenderRigidbodyInspector(int64_t entityId);
void RenderStartInspector(int64_t entityId);
void RenderFinishInspector(int64_t entityId);
void RenderLightInspector(int64_t entityId);