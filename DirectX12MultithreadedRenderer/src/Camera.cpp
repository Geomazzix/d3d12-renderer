#include "Camera.h"
#include "EventSystem/EventArgs.h"
#include "EventSystem/EventMessenger.h"

using namespace DirectX;

namespace mtd3d
{
	void Camera::Initialize(float yFovInRad, unsigned int clientWidth, unsigned int clientHeight, float zNear /*= 0.1f*/, float zFar /*= 1000.0f*/)
	{
		EventMessenger::GetInstance().ConnectListener<ResizeEventArgs&>("OnWindowResize", &Camera::OnClientResize, this);

		m_YFov = yFovInRad;
		m_ClientWidth = clientWidth;
		m_ClientHeight = clientHeight;
		m_ZNear = zNear;
		m_ZFar = zFar;

		m_IsDirty = true;
	}

	void Camera::Destroy()
	{
		EventMessenger::GetInstance().DisconnectListener<ResizeEventArgs&>("OnWindowResize", &Camera::OnClientResize, this);
	}

	XMFLOAT4X4 Camera::GetProjection()
	{
		if (m_IsDirty)
			RecalculateProjectionMatrix();
		return m_Projection;
	}

	void Camera::SetYFov(float value)
	{
		m_YFov = value;
		m_IsDirty = true;
	}

	void Camera::SetZNear(float value)
	{
		m_ZNear = value;
		m_IsDirty = true;
	}

	void Camera::SetZFar(float value)
	{
		m_ZFar;
		m_IsDirty = true;
	}

	unsigned int Camera::GetClientWidth() const
	{
		return m_ClientWidth;
	}

	unsigned int Camera::GetClientHeight() const
	{
		return m_ClientHeight;
	}

	float Camera::GetYFov() const
	{
		return m_YFov;
	}

	float Camera::GetZNear() const
	{
		return m_ZNear;
	}

	float Camera::GetZFar() const
	{
		return m_ZFar;
	}

	float Camera::GetAspectRatio() const
	{
		return static_cast<float>(m_ClientWidth) / static_cast<float>(m_ClientHeight);
	}

	void Camera::OnClientResize(ResizeEventArgs& e)
	{
		m_ClientWidth = static_cast<unsigned int>(e.Width);
		m_ClientHeight = static_cast<unsigned int>(e.Height);
		m_IsDirty = true;
	}

	void Camera::RecalculateProjectionMatrix()
	{
		XMStoreFloat4x4(&m_Projection, XMMatrixPerspectiveFovLH(m_YFov, GetAspectRatio(), m_ZNear, m_ZFar));
		m_IsDirty = false;
	}
}