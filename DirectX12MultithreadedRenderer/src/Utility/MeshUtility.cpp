#include "Utility/MeshUtility.h"

using namespace DirectX;
using namespace Microsoft::WRL;

namespace mtd3d
{
	std::shared_ptr<Mesh> MeshUtility::CreateBox(std::weak_ptr<RenderDevice> device, float width, float height, float depth, uint32_t numSubdivisions, const XMFLOAT4& color)
	{
		MeshData meshData;
		Vertex v[24];

		float halfWidth = 0.5f * width;
		float halfHeight = 0.5f * height;
		float halfDepth = 0.5f * depth;

		// Fill in the front face vertex data.
		v[0] = Vertex(-halfWidth, -halfHeight, -halfDepth, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, color.x, color.y, color.z, color.w);
		v[1] = Vertex(-halfWidth, +halfHeight, -halfDepth, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, color.x, color.y, color.z, color.w);
		v[2] = Vertex(+halfWidth, +halfHeight, -halfDepth, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, color.x, color.y, color.z, color.w);
		v[3] = Vertex(+halfWidth, -halfHeight, -halfDepth, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, color.x, color.y, color.z, color.w);

		// Fill in the back face vertex data.
		v[4] = Vertex(-halfWidth, -halfHeight, +halfDepth, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, color.x, color.y, color.z, color.w);
		v[5] = Vertex(+halfWidth, -halfHeight, +halfDepth, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, color.x, color.y, color.z, color.w);
		v[6] = Vertex(+halfWidth, +halfHeight, +halfDepth, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, color.x, color.y, color.z, color.w);
		v[7] = Vertex(-halfWidth, +halfHeight, +halfDepth, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, color.x, color.y, color.z, color.w);

		// Fill in the top face vertex data.
		v[8] = Vertex(-halfWidth, +halfHeight, -halfDepth, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  color.x, color.y, color.z, color.w);
		v[9] = Vertex(-halfWidth, +halfHeight, +halfDepth, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  color.x, color.y, color.z, color.w);
		v[10] = Vertex(+halfWidth, +halfHeight, +halfDepth, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, color.x, color.y, color.z, color.w);
		v[11] = Vertex(+halfWidth, +halfHeight, -halfDepth, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, color.x, color.y, color.z, color.w);

		// Fill in the bottom face vertex data.
		v[12] = Vertex(-halfWidth, -halfHeight, -halfDepth, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, color.x, color.y, color.z, color.w);
		v[13] = Vertex(+halfWidth, -halfHeight, -halfDepth, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, color.x, color.y, color.z, color.w);
		v[14] = Vertex(+halfWidth, -halfHeight, +halfDepth, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, color.x, color.y, color.z, color.w);
		v[15] = Vertex(-halfWidth, -halfHeight, +halfDepth, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, color.x, color.y, color.z, color.w);

		// Fill in the left face vertex data.
		v[16] = Vertex(-halfWidth, -halfHeight, +halfDepth, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f, color.x, color.y, color.z, color.w);
		v[17] = Vertex(-halfWidth, +halfHeight, +halfDepth, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, color.x, color.y, color.z, color.w);
		v[18] = Vertex(-halfWidth, +halfHeight, -halfDepth, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, 0.0f, color.x, color.y, color.z, color.w);
		v[19] = Vertex(-halfWidth, -halfHeight, -halfDepth, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, 1.0f, color.x, color.y, color.z, color.w);

		// Fill in the right face vertex data.
		v[20] = Vertex(+halfWidth, -halfHeight, -halfDepth, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, color.x, color.y, color.z, color.w);
		v[21] = Vertex(+halfWidth, +halfHeight, -halfDepth, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, color.x, color.y, color.z, color.w);
		v[22] = Vertex(+halfWidth, +halfHeight, +halfDepth, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, color.x, color.y, color.z, color.w);
		v[23] = Vertex(+halfWidth, -halfHeight, +halfDepth, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, color.x, color.y, color.z, color.w);

		meshData.Vertices.assign(&v[0], &v[24]);

		uint32_t i[36];

		// Fill in the front face index data
		i[0] = 0; i[1] = 1; i[2] = 2;
		i[3] = 0; i[4] = 2; i[5] = 3;

		// Fill in the back face index data
		i[6] = 4; i[7] = 5; i[8] = 6;
		i[9] = 4; i[10] = 6; i[11] = 7;

		// Fill in the top face index data
		i[12] = 8; i[13] = 9; i[14] = 10;
		i[15] = 8; i[16] = 10; i[17] = 11;

		// Fill in the bottom face index data
		i[18] = 12; i[19] = 13; i[20] = 14;
		i[21] = 12; i[22] = 14; i[23] = 15;

		// Fill in the left face index data
		i[24] = 16; i[25] = 17; i[26] = 18;
		i[27] = 16; i[28] = 18; i[29] = 19;

		// Fill in the right face index data
		i[30] = 20; i[31] = 21; i[32] = 22;
		i[33] = 20; i[34] = 22; i[35] = 23;

		meshData.Indices32.assign(&i[0], &i[36]);

		// Put a cap on the number of subdivisions.
		numSubdivisions = std::min<uint32_t>(numSubdivisions, 6u);

		for (uint32_t i = 0; i < numSubdivisions; ++i)
			Subdivide(meshData);

		auto commandQueue = device.lock()->GetCommandQueue(D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT);
		auto commandList = commandQueue.lock()->GetCommandList();

		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
		mesh->Initialize(device.lock()->Get(), commandList, meshData.Vertices, meshData.Indices32);

		commandQueue.lock()->ExecuteCommandList(commandList);
		commandQueue.lock()->Flush();

		return mesh;
	}

	std::shared_ptr<Mesh> MeshUtility::CreateSphere(std::weak_ptr<RenderDevice> device, float radius, uint32_t sliceCount, uint32_t stackCount, const XMFLOAT4& color)
	{
		MeshData meshData;

		Vertex topVertex(0.0f, +radius, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0, 0.0f, 0.0f, color.x, color.y, color.z, color.w);
		Vertex bottomVertex(0.0f, -radius, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, color.x, color.y, color.z, color.w);

		meshData.Vertices.push_back(topVertex);

		float phiStep = XM_PI / stackCount;
		float thetaStep = 2.0f * XM_PI / sliceCount;

		// Compute vertices for each stack ring (do not count the poles as rings).
		for (uint32_t i = 1; i <= stackCount - 1; ++i)
		{
			float phi = i * phiStep;
			for (uint32_t j = 0; j <= sliceCount; ++j)
			{
				float theta = j * thetaStep;

				Vertex v;

				// spherical to cartesian
				v.Position.x = radius * sinf(phi) * cosf(theta);
				v.Position.y = radius * cosf(phi);
				v.Position.z = radius * sinf(phi) * sinf(theta);

				// Partial derivative of P with respect to theta
				v.Tangent.x = -radius * sinf(phi) * sinf(theta);
				v.Tangent.y = 0.0f;
				v.Tangent.z = +radius * sinf(phi) * cosf(theta);

				XMVECTOR T = XMLoadFloat4(&v.Tangent);
				XMStoreFloat4(&v.Tangent, XMVector4Normalize(T));

				XMVECTOR p = XMLoadFloat3(&v.Position);
				XMStoreFloat3(&v.Normal, XMVector3Normalize(p));

				v.UV.x = theta / XM_2PI;
				v.UV.y = phi / XM_PI;

				v.Color = color;

				meshData.Vertices.push_back(v);
			}
		}

		meshData.Vertices.push_back(bottomVertex);

		for (uint32_t i = 1; i <= sliceCount; ++i)
		{
			meshData.Indices32.push_back(0);
			meshData.Indices32.push_back(i + 1);
			meshData.Indices32.push_back(i);
		}

		uint32_t baseIndex = 1;
		uint32_t ringVertexCount = sliceCount + 1;
		for (uint32_t i = 0; i < stackCount - 2; ++i)
		{
			for (uint32_t j = 0; j < sliceCount; ++j)
			{
				meshData.Indices32.push_back(baseIndex + i * ringVertexCount + j);
				meshData.Indices32.push_back(baseIndex + i * ringVertexCount + j + 1);
				meshData.Indices32.push_back(baseIndex + (i + 1) * ringVertexCount + j);

				meshData.Indices32.push_back(baseIndex + (i + 1) * ringVertexCount + j);
				meshData.Indices32.push_back(baseIndex + i * ringVertexCount + j + 1);
				meshData.Indices32.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
			}
		}
		uint32_t southPoleIndex = static_cast<uint32_t>(meshData.Vertices.size()) - 1;
		baseIndex = southPoleIndex - ringVertexCount;

		for (uint32_t i = 0; i < sliceCount; ++i)
		{
			meshData.Indices32.push_back(southPoleIndex);
			meshData.Indices32.push_back(baseIndex + i);
			meshData.Indices32.push_back(baseIndex + i + 1);
		}

		auto commandQueue = device.lock()->GetCommandQueue(D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT);
		auto commandList = commandQueue.lock()->GetCommandList();

		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
		mesh->Initialize(device.lock()->Get(), commandList, meshData.Vertices, meshData.Indices32);

		commandQueue.lock()->ExecuteCommandList(commandList);
		commandQueue.lock()->Flush();

		return mesh;
	}

	std::shared_ptr<Mesh> MeshUtility::CreateCylinder(std::weak_ptr<RenderDevice> device, float bottomRadius, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount, const XMFLOAT4& color)
	{
		MeshData meshData;

		//----------------------------------------------------------------------------------------------------
		//BUILDING THE STACKS.
		float stackHeight = height / stackCount;
		float radiusStep = (topRadius - bottomRadius) / stackCount;
		uint32_t ringCount = stackCount + 1;

		// Compute vertices for each stack ring starting at the bottom and moving up.
		for (uint32_t i = 0; i < ringCount; ++i)
		{
			float y = -0.5f * height + i * stackHeight;
			float r = bottomRadius + i * radiusStep;

			// vertices of ring
			float dTheta = 2.0f * XM_PI / sliceCount;
			for (uint32_t j = 0; j <= sliceCount; ++j)
			{
				Vertex vertex;

				float c = cosf(j * dTheta);
				float s = sinf(j * dTheta);

				vertex.Position = XMFLOAT3(r * c, y, r * s);

				vertex.UV.x = (float)j / sliceCount;
				vertex.UV.y = 1.0f - (float)i / stackCount;

				vertex.Color = color;

				// Cylinder can be parameterized as follows, where we introduce v
				// parameter that goes in the same direction as the v tex-coord
				// so that the bitangent goes in the same direction as the v tex-coord.
				//   Let r0 be the bottom radius and let r1 be the top radius.
				//   y(v) = h - hv for v in [0,1].
				//   r(v) = r1 + (r0-r1)v
				//
				//   x(t, v) = r(v)*cos(t)
				//   y(t, v) = h - hv
				//   z(t, v) = r(v)*sin(t)
				// 
				//  dx/dt = -r(v)*sin(t)
				//  dy/dt = 0
				//  dz/dt = +r(v)*cos(t)
				//
				//  dx/dv = (r0-r1)*cos(t)
				//  dy/dv = -h
				//  dz/dv = (r0-r1)*sin(t)

				// This is unit length.
				vertex.Tangent = XMFLOAT4(-s, 0.0f, c, 1.0f);

				float dr = bottomRadius - topRadius;
				XMFLOAT3 bitangent(dr * c, -height, dr * s);

				XMVECTOR T = XMLoadFloat4(&vertex.Tangent);
				XMVECTOR B = XMLoadFloat3(&bitangent);
				XMVECTOR N = XMVector3Normalize(XMVector3Cross(T, B));
				XMStoreFloat3(&vertex.Normal, N);

				meshData.Vertices.push_back(vertex);
			}
		}

		// Add one because we duplicate the first and last vertex per ring
		// since the texture coordinates are different.
		uint32_t ringVertexCount = sliceCount + 1;

		// Compute indices for each stack.
		for (uint32_t i = 0; i < stackCount; ++i)
		{
			for (uint32_t j = 0; j < sliceCount; ++j)
			{
				meshData.Indices32.push_back(i * ringVertexCount + j);
				meshData.Indices32.push_back((i + 1) * ringVertexCount + j);
				meshData.Indices32.push_back((i + 1) * ringVertexCount + j + 1);

				meshData.Indices32.push_back(i * ringVertexCount + j);
				meshData.Indices32.push_back((i + 1) * ringVertexCount + j + 1);
				meshData.Indices32.push_back(i * ringVertexCount + j + 1);
			}
		}

		//----------------------------------------------------------------------------------------------------
		//BUILDING THE TOP CAP.
		uint32_t baseIndex = static_cast<uint32_t>(meshData.Vertices.size());

		float y = 0.5f * height;
		float dTheta = 2.0f * XM_PI / sliceCount;

		// Duplicate cap ring vertices because the texture coordinates and normals differ.
		for (uint32_t i = 0; i <= sliceCount; ++i)
		{
			float x = topRadius * cosf(i * dTheta);
			float z = topRadius * sinf(i * dTheta);

			// Scale down by the height to try and make top cap texture coord area
			// proportional to base.
			float u = x / height + 0.5f;
			float v = z / height + 0.5f;

			meshData.Vertices.push_back(Vertex(
				x, y, z, 
				0.0f, 1.0f, 0.0f, 1.0f, 
				0.0f, 0.0f, 1.0f, 
				u, v, 
				color.x, color.y, color.z, color.w
			));
		}

		// Cap center vertex.
		meshData.Vertices.push_back(Vertex(
			0.0f, y, 0.0f, 
			0.0f, 1.0f, 0.0f, 1.0f, 
			0.0f, 0.0f, 1.0f, 
			0.5f, 0.5f, 
			color.x, color.y, color.z, color.w
		));

		// Index of center vertex.
		uint32_t centerIndex = static_cast<uint32_t>(meshData.Vertices.size()) - 1;

		for (uint32_t i = 0; i < sliceCount; ++i)
		{
			meshData.Indices32.push_back(centerIndex);
			meshData.Indices32.push_back(baseIndex + i + 1);
			meshData.Indices32.push_back(baseIndex + i);
		}

		//----------------------------------------------------------------------------------------------------
		//BUILDING THE BOTTOM CAP.
		baseIndex = static_cast<uint32_t>(meshData.Vertices.size());
		y = -0.5f * height;

		// vertices of ring
		for (uint32_t i = 0; i <= sliceCount; ++i)
		{
			float x = bottomRadius * cosf(i * dTheta);
			float z = bottomRadius * sinf(i * dTheta);

			// Scale down by the height to try and make top cap texture coord area
			// proportional to base.
			float u = x / height + 0.5f;
			float v = z / height + 0.5f;

			meshData.Vertices.push_back(Vertex(
				x, y, z, 
				0.0f, -1.0f, 0.0f, 1.0f, 
				0.0f, 0.0f, 1.0f, 
				u, v, 
				color.x, color.y, color.z, color.w
			));
		}

		// Cap center vertex.
		meshData.Vertices.push_back(Vertex(
			0.0f, y, 0.0f, 
			0.0f, -1.0f, 0.0f, 1.0f, 
			0.0f, 0.0f, 1.0f, 
			0.5f, 0.5f, 
			color.x, color.y, color.z, color.w
		));

		// Cache the index of center vertex.
		centerIndex = static_cast<uint32_t>(meshData.Vertices.size()) - 1;

		for (uint32_t i = 0; i < sliceCount; ++i)
		{
			meshData.Indices32.push_back(centerIndex);
			meshData.Indices32.push_back(baseIndex + i);
			meshData.Indices32.push_back(baseIndex + i + 1);
		}
		//----------------------------------------------------------------------------------------------------

		auto commandQueue = device.lock()->GetCommandQueue(D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT);
		auto commandList = commandQueue.lock()->GetCommandList();

		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
		mesh->Initialize(device.lock()->Get(), commandList, meshData.Vertices, meshData.Indices32);

		commandQueue.lock()->ExecuteCommandList(commandList);
		commandQueue.lock()->Flush();

		return mesh;
	}

	std::shared_ptr<Mesh> MeshUtility::CreateGeoSphere(std::weak_ptr<RenderDevice> device, float radius, uint32_t numSubdivisions, const XMFLOAT4& color)
	{
		MeshData meshData;

		// Put a cap on the number of subdivisions.
		numSubdivisions = std::min<uint32_t>(numSubdivisions, 8u);

		// Approximate a sphere by tessellating an icosahedron.
		const float X = 0.525731f;
		const float Z = 0.850651f;

		XMFLOAT3 pos[12] =
		{
			XMFLOAT3(-X, 0.0f, Z),  XMFLOAT3(X, 0.0f, Z),
			XMFLOAT3(-X, 0.0f, -Z), XMFLOAT3(X, 0.0f, -Z),
			XMFLOAT3(0.0f, Z, X),   XMFLOAT3(0.0f, Z, -X),
			XMFLOAT3(0.0f, -Z, X),  XMFLOAT3(0.0f, -Z, -X),
			XMFLOAT3(Z, X, 0.0f),   XMFLOAT3(-Z, X, 0.0f),
			XMFLOAT3(Z, -X, 0.0f),  XMFLOAT3(-Z, -X, 0.0f)
		};

		uint32_t k[60] =
		{
			1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,
			1,10,8, 10,3,8, 8,3,5,  3,2,5,  3,7,2,
			3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
			10,1,6, 11,0,9, 2,11,9, 5,2,9,  11,2,7
		};

		meshData.Vertices.resize(12);
		meshData.Indices32.assign(&k[0], &k[60]);

		for (uint32_t i = 0; i < 12; ++i)
			meshData.Vertices[i].Position = pos[i];

		for (uint32_t i = 0; i < numSubdivisions; ++i)
			Subdivide(meshData);

		// Project vertices onto sphere and scale.
		for (uint32_t i = 0; i < meshData.Vertices.size(); ++i)
		{
			// Project onto unit sphere.
			XMVECTOR n = XMVector3Normalize(XMLoadFloat3(&meshData.Vertices[i].Position));

			// Project onto sphere.
			XMVECTOR p = radius * n;

			XMStoreFloat3(&meshData.Vertices[i].Position, p);
			XMStoreFloat3(&meshData.Vertices[i].Normal, n);

			// Derive texture coordinates from spherical coordinates.
			float theta = atan2f(meshData.Vertices[i].Position.z, meshData.Vertices[i].Position.x);

			// Put in [0, 2pi].
			if (theta < 0.0f)
				theta += XM_2PI;

			float phi = acosf(meshData.Vertices[i].Position.y / radius);

			meshData.Vertices[i].UV.x = theta / XM_2PI;
			meshData.Vertices[i].UV.y = phi / XM_PI;

			// Partial derivative of P with respect to theta
			meshData.Vertices[i].Tangent.x = -radius * sinf(phi) * sinf(theta);
			meshData.Vertices[i].Tangent.y = 0.0f;
			meshData.Vertices[i].Tangent.z = +radius * sinf(phi) * cosf(theta);
			meshData.Vertices[i].Tangent.w = 1.0f;

			meshData.Vertices[i].Color = color;

			XMVECTOR T = XMLoadFloat4(&meshData.Vertices[i].Tangent);
			XMStoreFloat4(&meshData.Vertices[i].Tangent, XMVector3Normalize(T));
		}

		auto commandQueue = device.lock()->GetCommandQueue(D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT);
		auto commandList = commandQueue.lock()->GetCommandList();

		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
		mesh->Initialize(device.lock()->Get(), commandList, meshData.Vertices, meshData.Indices32);

		commandQueue.lock()->ExecuteCommandList(commandList);
		commandQueue.lock()->Flush();

		return mesh;
	}

	std::shared_ptr<Mesh> MeshUtility::CreatePlane(std::weak_ptr<RenderDevice> device, float cellWidth, float cellLength, uint32_t columnCount, uint32_t rowCount, const XMFLOAT4& color)
	{
		MeshData meshData;

		uint32_t vertexCount = columnCount * rowCount;
		uint32_t faceCount = (columnCount - 1) * (rowCount - 1) * 2;

		float halfWidth = 0.5f * cellWidth;
		float halfDepth = 0.5f * cellLength;

		float dx = cellWidth / (rowCount - 1);
		float dz = cellLength / (columnCount - 1);

		float du = 1.0f / (rowCount - 1);
		float dv = 1.0f / (columnCount - 1);

		meshData.Vertices.resize(vertexCount);
		for (uint32_t i = 0; i < columnCount; ++i)
		{
			float z = halfDepth - i * dz;
			for (uint32_t j = 0; j < rowCount; ++j)
			{
				float x = -halfWidth + j * dx;

				meshData.Vertices[i * rowCount + j].Position = XMFLOAT3(x, 0.0f, z);
				meshData.Vertices[i * rowCount + j].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
				meshData.Vertices[i * rowCount + j].Tangent = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

				// Stretch texture over grid.
				meshData.Vertices[i * rowCount + j].UV.x = j * du;
				meshData.Vertices[i * rowCount + j].UV.y = i * dv;

				meshData.Vertices[i * rowCount + j].Color = color;
			}
		}

		meshData.Indices32.resize(faceCount * 3); // 3 indices per face

		// Iterate over each quad and compute indices.
		uint32_t k = 0;
		for (uint32_t i = 0; i < columnCount - 1; ++i)
		{
			for (uint32_t j = 0; j < rowCount - 1; ++j)
			{
				meshData.Indices32[k] = i * rowCount + j;
				meshData.Indices32[k + 1] = i * rowCount + j + 1;
				meshData.Indices32[k + 2] = (i + 1) * rowCount + j;

				meshData.Indices32[k + 3] = (i + 1) * rowCount + j;
				meshData.Indices32[k + 4] = i * rowCount + j + 1;
				meshData.Indices32[k + 5] = (i + 1) * rowCount + j + 1;

				k += 6; // next quad
			}
		}

		auto commandQueue = device.lock()->GetCommandQueue(D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT);
		auto commandList = commandQueue.lock()->GetCommandList();

		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
		mesh->Initialize(device.lock()->Get(), commandList, meshData.Vertices, meshData.Indices32);

		commandQueue.lock()->ExecuteCommandList(commandList);
		commandQueue.lock()->Flush();

		return mesh;
	}

	std::shared_ptr<Mesh> MeshUtility::CreateQuad(std::weak_ptr<RenderDevice> device, float x, float y, float w, float h, float depth, const XMFLOAT4& color)
	{
		MeshData meshData;

		meshData.Vertices.resize(4);
		meshData.Indices32.resize(6);

		// Position coordinates specified in NDC space.
		meshData.Vertices[0] = Vertex(
			x, y - h, depth,
			0.0f, 0.0f, -1.0f,
			1.0f, 0.0f, 0.0f, 1.0f,
			0.0f, 1.0f,
			color.x, color.y, color.z, color.w
		);

		meshData.Vertices[1] = Vertex(
			x, y, depth,
			0.0f, 0.0f, -1.0f,
			1.0f, 0.0f, 0.0f, 1.0f,
			0.0f, 0.0f,
			color.x, color.y, color.z, color.w
		);

		meshData.Vertices[2] = Vertex(
			x + w, y, depth,
			0.0f, 0.0f, -1.0f,
			1.0f, 0.0f, 0.0f, 1.0f,
			1.0f, 0.0f,
			color.x, color.y, color.z, color.w
		);

		meshData.Vertices[3] = Vertex(
			x + w, y - h, depth,
			0.0f, 0.0f, -1.0f,
			1.0f, 0.0f, 0.0f, 1.0f,
			1.0f, 1.0f,
			color.x, color.y, color.z, color.w
		);

		meshData.Indices32[0] = 0;
		meshData.Indices32[1] = 1;
		meshData.Indices32[2] = 2;

		meshData.Indices32[3] = 0;
		meshData.Indices32[4] = 2;
		meshData.Indices32[5] = 3;

		auto commandQueue = device.lock()->GetCommandQueue(D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT);
		auto commandList = commandQueue.lock()->GetCommandList();

		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
		mesh->Initialize(device.lock()->Get(), commandList, meshData.Vertices, meshData.Indices32);

		commandQueue.lock()->ExecuteCommandList(commandList);
		commandQueue.lock()->Flush();

		return mesh;
	}

	void MeshUtility::Subdivide(MeshData& meshData)
	{
		MeshData inputCopy = meshData;

		meshData.Vertices.resize(0);
		meshData.Indices32.resize(0);

		//Calculates m0/m1/m2 as new subdivision points - works recursively.
		//       v1
		//       *
		//      / \
		//     /   \
		//  m0*-----*m1
		//   / \   / \
		//  /   \ /   \
		// *-----*-----*
		// v0    m2     v2

		uint32_t numTris = static_cast<uint32_t>(inputCopy.Indices32.size()) / 3;
		for (uint32_t i = 0; i < numTris; ++i)
		{
			Vertex v0 = inputCopy.Vertices[inputCopy.Indices32[i * 3 + 0]];
			Vertex v1 = inputCopy.Vertices[inputCopy.Indices32[i * 3 + 1]];
			Vertex v2 = inputCopy.Vertices[inputCopy.Indices32[i * 3 + 2]];

			Vertex m0 = MidPoint(v0, v1);
			Vertex m1 = MidPoint(v1, v2);
			Vertex m2 = MidPoint(v0, v2);

			meshData.Vertices.push_back(v0); // 0
			meshData.Vertices.push_back(v1); // 1
			meshData.Vertices.push_back(v2); // 2
			meshData.Vertices.push_back(m0); // 3
			meshData.Vertices.push_back(m1); // 4
			meshData.Vertices.push_back(m2); // 5

			meshData.Indices32.push_back(i * 6 + 0);
			meshData.Indices32.push_back(i * 6 + 3);
			meshData.Indices32.push_back(i * 6 + 5);

			meshData.Indices32.push_back(i * 6 + 3);
			meshData.Indices32.push_back(i * 6 + 4);
			meshData.Indices32.push_back(i * 6 + 5);

			meshData.Indices32.push_back(i * 6 + 5);
			meshData.Indices32.push_back(i * 6 + 4);
			meshData.Indices32.push_back(i * 6 + 2);

			meshData.Indices32.push_back(i * 6 + 3);
			meshData.Indices32.push_back(i * 6 + 1);
			meshData.Indices32.push_back(i * 6 + 4);
		}
	}

	Vertex MeshUtility::MidPoint(const Vertex& v0, const Vertex& v1)
	{
		XMVECTOR p0 = XMLoadFloat3(&v0.Position);
		XMVECTOR p1 = XMLoadFloat3(&v1.Position);

		XMVECTOR n0 = XMLoadFloat3(&v0.Normal);
		XMVECTOR n1 = XMLoadFloat3(&v1.Normal);

		XMVECTOR tan0 = XMLoadFloat4(&v0.Tangent);
		XMVECTOR tan1 = XMLoadFloat4(&v1.Tangent);

		XMVECTOR tex0 = XMLoadFloat2(&v0.UV);
		XMVECTOR tex1 = XMLoadFloat2(&v1.UV);

		XMVECTOR col0 = XMLoadFloat4(&v0.Color);
		XMVECTOR col1 = XMLoadFloat4(&v1.Color);

		XMVECTOR pos = 0.5f * (p0 + p1);
		XMVECTOR normal = XMVector3Normalize(0.5f * (n0 + n1));
		XMVECTOR tangent = XMVector4Normalize(0.5f * (tan0 + tan1));
		XMVECTOR tex = 0.5f * (tex0 + tex1);
		XMVECTOR color = 0.5f * (col0 + col1);

		Vertex v;
		XMStoreFloat3(&v.Position, pos);
		XMStoreFloat3(&v.Normal, normal);
		XMStoreFloat4(&v.Tangent, tangent);
		XMStoreFloat4(&v.Color, color);
		XMStoreFloat2(&v.UV, tex);

		return v;
	}
}