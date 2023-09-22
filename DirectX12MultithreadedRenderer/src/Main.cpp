#if defined (DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <cassert>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <vector>
#include <memory>
#include <chrono>
#include <DirectXColors.h>

#include "Window.h"
#include "RenderDevice.h"
#include "RenderContext.h"
#include "CommandQueue.h"
#include "CommandList.h"
#include "Camera.h"
#include "Input.h"
#include "RootSignature.h"
#include "PipelineStateObject.h"
#include "Resources/FrameResource.h"
#include "JobSystem.h"
#include "Light.h"
#include "RenderItem.h"

#include "ResourceHandler.h"
#include "Resources/UploadBuffer.h"

#include "Utility/D3DErrorHandler.h"
#include "Utility/D3dx12.h"
#include "Utility/Time.h"
#include "Utility/MeshUtility.h"
#include "Utility/MathUtility.h"

#include "EventSystem/EventArgs.h"
#include "EventSystem/EventMessenger.h"

using namespace mtd3d;			//#TODO: Remove later on, macro magic doing it's job here --'
using namespace Microsoft::WRL;
using namespace DirectX;

const static int s_WindowWidthInPx = 800;
const static int s_WindowHeightInPx = 600;
const static int s_SwapChainBufferCount = 2;

const static int s_SceneCountX = 45;
const static int s_SceneCountZ = 45;

const static float s_scrollSpeed = 0.02f;
static float s_RelScrollSpeed = 0.0f;

const static int s_FrameResourceCount = 3;

XMFLOAT3 cameraPosition;
POINT lastMousePos = { 0, 0 };
float theta = 1.5f * XM_PI;
float phi = XM_PIDIV4;
float radius = 45.0f;

enum class ERenderMode
{
	DIFFUSE = 0,
	WIREFRAME
};

UINT cbvSrvUavDescriptorSize = 0;
ERenderMode renderMode = ERenderMode::DIFFUSE;

std::shared_ptr<PipelineStateObject> diffusePSO;
std::shared_ptr<PipelineStateObject> wireFramePSO;
XMMATRIX view;

struct ShaderConstants
{
	XMFLOAT4X4 ViewProjectionMatrix = Identity();
	XMFLOAT4X4 ModelMatrix = Identity();
	XMFLOAT4X4 NormalMatrix = Identity();
	XMFLOAT4 CameraPosition = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
};

void OnScrollWheelChanged(mtd3d::MouseWheelEventArgs& e)
{
	s_RelScrollSpeed = e.WheelDelta * s_scrollSpeed;
};

std::vector<RenderItem> renderItems;
std::vector<DirectionalLight> directionalLightItems;
std::vector<PointLight> pointLightItems;

JobSystem jobSystem;
Window window;
std::shared_ptr<Input> input = std::make_shared<Input>();
Camera camera;
ResourceHandler resourceHandler;

std::shared_ptr<mtd3d::RenderDevice> renderDevice = std::make_shared<mtd3d::RenderDevice>();
std::weak_ptr<mtd3d::CommandQueue> gfxQueue;

//Frame resources.
std::vector<std::unique_ptr<FrameResource<ShaderConstants>>> frameResources;
FrameResource<ShaderConstants>* currentFrameResource = nullptr;
int currentFrameResourceIndex = 0;

mtd3d::RootSignature rootSignature;


//--- Function Definitions ---

void HandleInput(std::shared_ptr<Input> input);
void CreateMaterials();
void CreateLightItems();
void CreateRenderItems();
void UpdateObjectCBs();
void UpdateMaterialCBs();
void UpdateDirectionalLightSRVs();
void UpdatePointLightSRVs();
void CreateFrameResources();
void CreateRootSignature();
void CreatePSOs(std::shared_ptr<RenderDevice> renderDevice, RenderContext& renderContext, RootSignature& rootSignature);
void Tick();
void Draw(RenderContext& renderContext);


//--- Main functionality/loops ---

int WINAPI main(HINSTANCE appModule, HINSTANCE prevModule, PSTR cmdline, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	EventMessenger::GetInstance().AddMessenger("OnAppPause");
	EventMessenger::GetInstance().AddMessenger("OnAppResume");
	EventMessenger::GetInstance().AddMessenger<mtd3d::ResizeEventArgs&>("OnWindowResize");
	EventMessenger::GetInstance().AddMessenger<MouseWheelEventArgs&>("OnMouseScrollWheelChanged");

	Time s_Time;

	window.Initialize(appModule, input, L"Direct3D 12 multithreaded rendering", s_WindowWidthInPx, s_WindowHeightInPx);
	EventMessenger::GetInstance().ConnectListener<MouseWheelEventArgs&>("OnMouseScrollWheelChanged", &OnScrollWheelChanged);

	renderDevice->Initialize();
	cbvSrvUavDescriptorSize = renderDevice->Get()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	RenderContext renderContext;
	renderContext.Initialize(renderDevice, renderDevice->GetCommandQueue(D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT), window, s_SwapChainBufferCount);
	camera.Initialize(DirectX::XMConvertToRadians(40), window.GetWindowWidth(), window.GetWindowHeight());

	CreateRootSignature();
	CreateMaterials();
	CreateLightItems();
	CreateRenderItems();
	CreateFrameResources();
	CreatePSOs(renderDevice, renderContext, rootSignature);

	s_Time.Reset();

	gfxQueue = renderDevice->GetCommandQueue(D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT);
	while(!window.DoesWinAPIWindowClose())
	{	
		s_Time.Tick();

		currentFrameResourceIndex = (currentFrameResourceIndex + 1) % static_cast<int>(frameResources.size());
		currentFrameResource = frameResources[currentFrameResourceIndex].get();

		gfxQueue.lock()->WaitOnFenceValue(currentFrameResource->FenceValue); //Wait for the current frame resource.

		Tick();
		Draw(renderContext);

		renderContext.Present();
		gfxQueue.lock()->Signal();
		currentFrameResource->FenceValue = gfxQueue.lock()->GetFenceValue();

		window.PollEvents();
		window.UpdateWindowText();
	}

	renderDevice->Flush();

	EventMessenger::GetInstance().DisconnectListener<MouseWheelEventArgs&>("OnMouseScrollWheelChanged", &OnScrollWheelChanged);
	window.Destroy();

	return 0;
}

void Tick()
{
	HandleInput(input);

	UpdateMaterialCBs();
	UpdateObjectCBs();

	UpdateDirectionalLightSRVs();
	UpdatePointLightSRVs();
}

void Draw(RenderContext& renderContext)
{
	auto cmdListAllocator = currentFrameResource->CommandAllocator;

	D3D_CHECK(cmdListAllocator->Reset());

	auto commandList = gfxQueue.lock()->GetCommandList(cmdListAllocator);
	commandList.lock()->TransitionResource(
		renderContext.GetCurrentBackBuffer().Get(),
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET
	);

	//PRE RENDER. ---------------------------------------------------------------------------------------------------------------
	switch (renderMode)
	{
	case ERenderMode::DIFFUSE:
		commandList.lock()->SetPipelineStateObject(diffusePSO->Get());
		break;
	case ERenderMode::WIREFRAME:
		commandList.lock()->SetPipelineStateObject(wireFramePSO->Get());
		break;
	}
	renderContext.SetRSProperties(commandList.lock());

	const float clearScreenColor[4] = { 0.3f, 0.5f, 0.2f, 1.0f };

	commandList.lock()->GetCmdList()->ClearRenderTargetView(renderContext.GetCurrentBackBufferView(), clearScreenColor, 0, nullptr);
	commandList.lock()->GetCmdList()->ClearDepthStencilView(
		renderContext.GetDepthStencilView(),
		D3D12_CLEAR_FLAGS::D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAGS::D3D12_CLEAR_FLAG_STENCIL,
		1.0f, 0, 0, nullptr
	);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = renderContext.GetCurrentBackBufferView();
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = renderContext.GetDepthStencilView();
	commandList.lock()->GetCmdList()->OMSetRenderTargets(1, &rtvHandle, TRUE, &dsvHandle);

	//DRAWING ITEMS. ------------------------------------------------------------------------------------------------------------
	commandList.lock()->GetCmdList()->SetGraphicsRootSignature(rootSignature.Get().Get());

	//Lights.
	LightConstants lightConstants;
	lightConstants.DirectionalLightCount = directionalLightItems.size();
	lightConstants.PointLightCount = pointLightItems.size();
	commandList.lock()->GetCmdList()->SetGraphicsRoot32BitConstants(2, sizeof(LightConstants) / sizeof(uint32_t), &lightConstants, 0);

	if (directionalLightItems.size() > 0)
	{
		UINT dirLightSVSizeInBytes = sizeof(DirectionalLight);
		auto dirLightSv = currentFrameResource->DirectionalLightSR->GetResource();
		commandList.lock()->GetCmdList()->SetGraphicsRootShaderResourceView(3, dirLightSv->GetGPUVirtualAddress());
	}

	if (pointLightItems.size() > 0)
	{
		UINT pointLightSVSizeInBytes = sizeof(PointLight);
		auto pointLightSv = currentFrameResource->PointLightSR->GetResource();
		commandList.lock()->GetCmdList()->SetGraphicsRootShaderResourceView(4, pointLightSv->GetGPUVirtualAddress());
	}

	//Meshes
	UINT objCBSizeInBytes = CalcConstantBufferByteSize(sizeof(ShaderConstants));
	UINT matCBSizeInBytes = CalcConstantBufferByteSize(sizeof(MaterialConstants));

	auto objectCb = currentFrameResource->ObjectCB->GetResource();
	auto materialCb = currentFrameResource->MaterialCB->GetResource();

	for (int i = 0; i < renderItems.size(); i++)
	{
		D3D12_GPU_VIRTUAL_ADDRESS cbObject = objectCb->GetGPUVirtualAddress() + renderItems[i].ObjectCBIndex * objCBSizeInBytes;
		D3D12_GPU_VIRTUAL_ADDRESS cbMat = materialCb->GetGPUVirtualAddress() + renderItems[i].MaterialInstance.lock()->CbIndex * matCBSizeInBytes;

		commandList.lock()->GetCmdList()->SetGraphicsRootConstantBufferView(0, cbObject);
		commandList.lock()->GetCmdList()->SetGraphicsRootConstantBufferView(1, cbMat);

		commandList.lock()->SetPrimitiveTopology(renderItems[i].PrimitiveType);
		renderItems[i].MeshInstance.lock()->RecordDrawCommand(commandList, 1, 0);
	}

	//END RENDER ----------------------------------------------------------------------------------------------------------------
	commandList.lock()->TransitionResource(
		renderContext.GetCurrentBackBuffer().Get(),
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT
	);

	gfxQueue.lock()->ExecuteCommandList(commandList);
}


//--- Creation/Initialization ---

void CreateMaterials()
{
	int cbIndex = 0;

	std::shared_ptr<Material> diffuseMaterial = std::make_shared<Material>(s_FrameResourceCount, cbIndex++);
	diffuseMaterial->Constants.AlbedoDiffuse = XMFLOAT4(0.5, 0.5, 0.5, 1.0);
	diffuseMaterial->Constants.FresnelR0 = XMFLOAT3(0.04, 0.04, 0.04);
	diffuseMaterial->Constants.RoughnessFactor = 64;

	resourceHandler.CacheMaterial("diffuseMaterial", diffuseMaterial);
}

void CreateLightItems()
{
	DirectionalLight sunLight;
	sunLight.Direction = XMFLOAT3(-1.0f, -1.0, -1.0);
	sunLight.Intensity = 1.0f;
	sunLight.Color = XMFLOAT3(1.0f, 1.0f, 1.0f);
	directionalLightItems.push_back(sunLight);

	PointLight pointLight;
	pointLight.Position = XMFLOAT3(-20.0f, 20.0f, -20.0f);
	pointLight.Intensity = 1.5f;
	pointLight.Color = XMFLOAT3(1.0f, 1.0f, 1.0f);
	pointLight.Radius = 35.0f;
	pointLightItems.push_back(pointLight);

	PointLight pointLight1;
	pointLight1.Position = XMFLOAT3(20.0f, 20.0f, 20.0f);
	pointLight1.Intensity = 1.5f;
	pointLight1.Color = XMFLOAT3(1.0f, 1.0f, 0.0f);
	pointLight1.Radius = 35.0f;
	pointLightItems.push_back(pointLight1);

	PointLight pointLight2;
	pointLight2.Position = XMFLOAT3(-20.0f, 20.0f, 20.0f);
	pointLight2.Intensity = 1.5f;
	pointLight2.Color = XMFLOAT3(1.0f, 0.0f, 1.0f);
	pointLight2.Radius = 35.0f;
	pointLightItems.push_back(pointLight2);

	PointLight pointLight3;
	pointLight3.Position = XMFLOAT3(20.0f, 20.0f, -20.0f);
	pointLight3.Intensity = 1.5f;
	pointLight3.Color = XMFLOAT3(0.0f, 1.0f, 1.0f);
	pointLight3.Radius = 35.0f;
	pointLightItems.push_back(pointLight3);
}

void CreateRenderItems()
{
	const float sceneWidth = 20.0f;
	const float sceneLength = 20.0f;

	resourceHandler.CacheMesh("Ground", MeshUtility::CreatePlane(renderDevice, sceneWidth, sceneLength, 100, 100, XMFLOAT4(Colors::Green)));
	std::weak_ptr<Mesh> groundMesh = resourceHandler.GetMesh("Ground");

	resourceHandler.CacheMesh("Cube", MeshUtility::CreateBox(renderDevice, 6.0f, 0.1f, 6.0f, 7, XMFLOAT4(Colors::LimeGreen)));
	std::weak_ptr<Mesh> cubeMesh = resourceHandler.GetMesh("Cube");

	resourceHandler.CacheMesh("Pillar", MeshUtility::CreateCylinder(renderDevice, 0.25f, 0.75f, 2, 300, 2, XMFLOAT4(Colors::PaleVioletRed)));
	std::weak_ptr<Mesh> pillarMesh = resourceHandler.GetMesh("Pillar");

	resourceHandler.CacheMesh("CenterPillar", MeshUtility::CreateCylinder(renderDevice, 1, 2, 2, 400, 2, XMFLOAT4(Colors::Beige)));
	std::weak_ptr<Mesh> centerPillarMesh = resourceHandler.GetMesh("CenterPillar");

	resourceHandler.CacheMesh("Sphere", MeshUtility::CreateSphere(renderDevice, 2, 500, 500, XMFLOAT4(Colors::Red)));
	std::weak_ptr<Mesh> sphereMesh = resourceHandler.GetMesh("Sphere");

	int cbIndex = -1;
	for(int x = 0; x < s_SceneCountX; ++x)
	{
		for(int z = 0; z < s_SceneCountZ; ++z)
		{
			float offsetX = (x - s_SceneCountX / 2) * sceneWidth;
			float offsetZ = (z - s_SceneCountZ / 2) * sceneLength;

			//Ground plane.
			RenderItem ri(s_FrameResourceCount, ++cbIndex, groundMesh, resourceHandler.GetMaterial("diffuseMaterial"));
			XMStoreFloat4x4(&ri.ModelMatrix, XMMatrixTranslationFromVector(XMVectorSet(offsetX, 0.0f, offsetZ, 1)));
			renderItems.push_back(ri);

			//Center sphere.
			ri.ObjectCBIndex = ++cbIndex;
			ri.MeshInstance = sphereMesh;
			XMStoreFloat4x4(&ri.ModelMatrix, XMMatrixTranslationFromVector(XMVectorSet(offsetX, 6.1f, offsetZ, 1)));
			renderItems.push_back(ri);

			//Top left pillar.
			ri.ObjectCBIndex = ++cbIndex;
			ri.MeshInstance = pillarMesh;
			XMStoreFloat4x4(&ri.ModelMatrix, XMMatrixTranslationFromVector(XMVectorSet(-2 + offsetX, 1, 2 + offsetZ, 1)));
			renderItems.push_back(ri);

			//Top right pillar.
			ri.ObjectCBIndex = ++cbIndex;
			ri.MeshInstance = pillarMesh;
			XMStoreFloat4x4(&ri.ModelMatrix, XMMatrixTranslationFromVector(XMVectorSet(2 + offsetX, 1, 2 + offsetZ, 1)));
			renderItems.push_back(ri);

			//Bottom left pillar.
			ri.ObjectCBIndex = ++cbIndex;
			ri.MeshInstance = pillarMesh;
			XMStoreFloat4x4(&ri.ModelMatrix, XMMatrixTranslationFromVector(XMVectorSet(-2 + offsetX, 1, -2 + offsetZ, 1)));
			renderItems.push_back(ri);

			//Bottom right pillar.
			ri.ObjectCBIndex = ++cbIndex;
			ri.MeshInstance = pillarMesh;
			XMStoreFloat4x4(&ri.ModelMatrix, XMMatrixTranslationFromVector(XMVectorSet(2 + offsetX, 1, -2 + offsetZ, 1)));
			renderItems.push_back(ri);

			//Cube plate pillar.
			ri.ObjectCBIndex = ++cbIndex;
			ri.MeshInstance = cubeMesh;
			XMStoreFloat4x4(&ri.ModelMatrix, XMMatrixTranslationFromVector(XMVectorSet(offsetX, 2.05f, offsetZ, 1)));
			renderItems.push_back(ri);

			//Center pillar.
			ri.ObjectCBIndex = ++cbIndex;
			ri.MeshInstance = centerPillarMesh;
			XMStoreFloat4x4(&ri.ModelMatrix, XMMatrixTranslationFromVector(XMVectorSet(offsetX, 3.1f, offsetZ, 1)));
			renderItems.push_back(ri);
		}
	}
}

void CreateFrameResources()
{
	for (int i = 0; i < s_FrameResourceCount; i++)
	{
		frameResources.push_back(std::make_unique<FrameResource<ShaderConstants>>(
			renderDevice->Get(), 
			static_cast<UINT>(renderItems.size()), 
			static_cast<UINT>(resourceHandler.GetMaterialCount()),
			static_cast<UINT>(directionalLightItems.size()),
			static_cast<UINT>(pointLightItems.size()))
		);
	}
}

void CreateRootSignature()
{
	CD3DX12_ROOT_PARAMETER1 slotRootParameter[5]{};
	slotRootParameter[0].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAGS::D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC, D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_VERTEX);
	slotRootParameter[1].InitAsConstantBufferView(1, 0, D3D12_ROOT_DESCRIPTOR_FLAGS::D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC, D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_PIXEL);

	slotRootParameter[2].InitAsConstants(sizeof(LightConstants) / 4, 2, 0, D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_PIXEL);
	slotRootParameter[3].InitAsShaderResourceView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAGS::D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_PIXEL);
	slotRootParameter[4].InitAsShaderResourceView(1, 0, D3D12_ROOT_DESCRIPTOR_FLAGS::D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_PIXEL);

	D3D12_ROOT_SIGNATURE_DESC1 rootSignDesc =
	{
		5,
		slotRootParameter,
		0,
		nullptr,
		D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
	};

	rootSignature.Initialize(renderDevice, rootSignDesc);
}

void CreatePSOs(std::shared_ptr<RenderDevice> renderDevice, RenderContext& renderContext, RootSignature& rootSignature)
{
	//Load the shaders.
	Microsoft::WRL::ComPtr<ID3DBlob> vsByteCode;
	Microsoft::WRL::ComPtr<ID3DBlob> psByteCode;

	D3D_CHECK(D3DReadFileToBlob(L"res/shaders/vs/Diffuse.vs.cso", &vsByteCode));
	D3D_CHECK(D3DReadFileToBlob(L"res/shaders/ps/Diffuse.ps.cso", &psByteCode));

	//The pipeline stream setup.
	struct DiffusePipelineStream
	{
		CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE        pRootSignature;
		CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT          InputLayout;
		CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY    PrimitiveTopologyType;
		CD3DX12_PIPELINE_STATE_STREAM_VS                    VS;
		CD3DX12_PIPELINE_STATE_STREAM_PS                    PS;
		CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER			RasterizerDesc;
		CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT  DSVFormat;
		CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
	} DiffuseStream;

	//The pipeline stream setup.
	struct WireFramePipelineStream
	{
		CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE        pRootSignature;
		CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT          InputLayout;
		CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY    PrimitiveTopologyType;
		CD3DX12_PIPELINE_STATE_STREAM_VS                    VS;
		CD3DX12_PIPELINE_STATE_STREAM_PS                    PS;
		CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER			RasterizerDesc;
		CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT  DSVFormat;
		CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
	} WireframeStream;


	D3D12_RT_FORMAT_ARRAY rtvFormats =
	{
		{ renderContext.GetBackBufferFormat() },
		1
	};

	CD3DX12_RASTERIZER_DESC rasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

	DiffuseStream =
	{
		rootSignature.Get().Get(),
		D3D12_INPUT_LAYOUT_DESC{ Vertex::s_InputElements, Vertex::s_InputElementCount },
		D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
		CD3DX12_SHADER_BYTECODE(vsByteCode.Get()),
		CD3DX12_SHADER_BYTECODE(psByteCode.Get()),
		CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER(rasterizerDesc),
		renderContext.GetDepthStencilFormat(),
		rtvFormats
	};

	rasterizerDesc.AntialiasedLineEnable = true;
	rasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;

	WireframeStream =
	{
		rootSignature.Get().Get(),
		D3D12_INPUT_LAYOUT_DESC{ Vertex::s_InputElements, Vertex::s_InputElementCount },
		D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
		CD3DX12_SHADER_BYTECODE(vsByteCode.Get()),
		CD3DX12_SHADER_BYTECODE(psByteCode.Get()),
		CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER(rasterizerDesc),
		renderContext.GetDepthStencilFormat(),
		rtvFormats
	};

	diffusePSO = renderDevice->CreatePipelineStateObject<DiffusePipelineStream>(DiffuseStream);
	wireFramePSO = renderDevice->CreatePipelineStateObject<WireFramePipelineStream>(WireframeStream);
}


//--- Tick/Update functionality ---

void UpdateObjectCBs()
{
	auto currentObjectCB = currentFrameResource->ObjectCB.get();
	for(auto& ri : renderItems)
	{
		if (ri.NumFramesDirty <= 0)
			continue;

		//Calculate the worldViewProj.
		XMMATRIX world = XMLoadFloat4x4(&ri.ModelMatrix);
		XMFLOAT4X4 camProj = camera.GetProjection();
		XMMATRIX proj = XMLoadFloat4x4(&camProj);
		XMFLOAT4 cameraPos(cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0f);


		XMMATRIX inverseWorld = XMMatrixInverse(nullptr, world);
		XMMATRIX normalMatrix = XMMatrixTranspose(inverseWorld);

		// Update the constant buffer with the latest worldViewProj matrix.
		ShaderConstants objConstants;
		XMStoreFloat4x4(&objConstants.ViewProjectionMatrix, XMMatrixTranspose(view * proj));
		XMStoreFloat4x4(&objConstants.ModelMatrix, XMMatrixTranspose(world));
		XMStoreFloat4x4(&objConstants.NormalMatrix, normalMatrix);
		XMStoreFloat4(&objConstants.CameraPosition, XMLoadFloat4(&cameraPos));
		currentObjectCB->CopyData(ri.ObjectCBIndex, objConstants);

		ri.NumFramesDirty--;
	}
}

void UpdateMaterialCBs()
{
	std::shared_ptr<Material> material = resourceHandler.GetMaterial("diffuseMaterial").lock();

	if (material->NumFramesDirty <= 0)
		return;

	auto currentMaterialCb = currentFrameResource->MaterialCB.get();
	MaterialConstants materialConstants;
	materialConstants.AlbedoDiffuse = material->Constants.AlbedoDiffuse;
	materialConstants.FresnelR0 = material->Constants.FresnelR0;
	materialConstants.RoughnessFactor = material->Constants.RoughnessFactor;

	currentMaterialCb->CopyData(material->CbIndex, materialConstants);
	material->NumFramesDirty--;
}

void UpdateDirectionalLightSRVs()
{
	auto directionalLightSrv = currentFrameResource->DirectionalLightSR.get();
	for(int i = 0; i < directionalLightItems.size(); i++)
	{
		directionalLightSrv->CopyData(i, directionalLightItems[i]);
	}
}

void UpdatePointLightSRVs()
{
	auto pointLightSrv = currentFrameResource->PointLightSR.get();
	for (int i = 0; i < pointLightItems.size(); i++)
	{
		pointLightSrv->CopyData(i, pointLightItems[i]);
	}
}

void HandleInput(std::shared_ptr<Input> input)
{
	if (input->IsPressed(KeyCode::LButton) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f * static_cast<float>(input->GetMouseX() - lastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(input->GetMouseY() - lastMousePos.y));

		// Update angles based on input to orbit camera around box.
		theta -= dx;
		phi -= dy;

		// Restrict the angle mPhi.
		phi = Clamp(phi, 0.1f, XM_PI - 0.1f);

		//Make sure to update the dirty frames whenever the camera changed.
		for (auto& ri : renderItems)
		{
			ri.NumFramesDirty = static_cast<int>(frameResources.size());
		}
	}
	else if (abs(s_RelScrollSpeed) > 0.0f)
	{
		// Update the camera radius based on input.
		radius -= s_RelScrollSpeed;

		// Restrict the radius.
		radius = Clamp(radius, 5.0f, 200.0f);
		s_RelScrollSpeed = 0.0f;

		//Make sure to update the dirty frames whenever the camera changed.
		for (auto& ri : renderItems)
		{
			ri.NumFramesDirty = static_cast<int>(frameResources.size());
		}
	}

	if (input->IsPressed(KeyCode::F1))
	{
		renderMode = ERenderMode::DIFFUSE;
	}
	else if (input->IsPressed(KeyCode::F2))
	{
		renderMode = ERenderMode::WIREFRAME;
	}

	lastMousePos.x = static_cast<LONG>(input->GetMouseX());
	lastMousePos.y = static_cast<LONG>(input->GetMouseY());

	// Convert Spherical to Cartesian coordinates.
	float x = radius * sinf(phi) * cosf(theta);
	float z = radius * sinf(phi) * sinf(theta);
	float y = radius * cosf(phi);

	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMStoreFloat3(&cameraPosition, pos);
	view = XMMatrixLookAtLH(pos, target, up);
}
