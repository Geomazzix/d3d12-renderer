#pragma once
#include <DirectXMath.h>
#include "EventSystem/EventArgs.h"

namespace mtd3d
{
	/// <summary>
	/// The camera class manages the viewport aspectratio and project matrix.
	/// </summary>
	class Camera
	{
	public:
		Camera() = default;
		~Camera() = default;

		void Initialize(float yFov, unsigned int clientWidth, unsigned int clientHeight, float zNear = 0.1f, float zFar = 1000.0f);
		void Destroy();

		DirectX::XMFLOAT4X4 GetProjection();

		void SetYFov(float value);
		void SetZNear(float value);
		void SetZFar(float value);

		unsigned int GetClientWidth() const;
		unsigned int GetClientHeight() const;
		
		float GetYFov() const;
		float GetZNear() const;
		float GetZFar() const;
		float GetAspectRatio() const;

		void OnClientResize(ResizeEventArgs& e);

	private:
		DirectX::XMFLOAT4X4 m_Projection;
		unsigned int m_ClientWidth;
		unsigned int m_ClientHeight;
		float m_YFov;
		float m_ZNear;
		float m_ZFar;
		bool m_IsDirty;

		void RecalculateProjectionMatrix();
	};
}