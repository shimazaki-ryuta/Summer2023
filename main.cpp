
#include "D3DResourceLeakChacker.h"
//static D3DResourceLeakChacker* leacCheck = D3DResourceLeakChacker::GetInstance();
#include <Windows.h>
#define _USE_MATH_DEFINES
#include <math.h>
//#include "WindowProcedure.h"
#include <cstdint>
#include <string>
#include <format>
#include <vector>
#include"ConvertString.h"

//window関係
#include "CommonFiles/WinApp.h"

//directx12関係
#include "CommonFiles/DirectXCommon.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <dxgidebug.h>
#include <wrl.h>

#include "externals/DirectXTex/DirectXTex.h"
#include "externals/DirectXTex/d3dx12.h"

//Shader関係
#include <dxcapi.h>

//自作関数
#include "Matrix.h"
#include "MatrixFunction.h"
#include "VectorFunction.h"

//ImGui
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);


#include "TextureManager.h"

#include "DeltaTime.h"

#include "Vector4.h"

#include "ShaderCompiler.h"

#include "Sprite.h"
#include "Primitive3D.h"
//入力
#include "Input.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")

#include "GameScene.h"

const int32_t kClientWidth = 1280;
const int32_t kClientHeight = 720;
const std::string kTitle = "アイス食べたい4";


struct VertexData
{
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

struct Material
{
	Vector4 color;
	int32_t enableLighting;
};

struct Transform
{
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
} ;

struct TransformationMatrix
{
	Matrix4x4 WVP;
	Matrix4x4 World;
};

struct DirectionalLight
{
	Vector4 color;
	Vector3 direction;
	float intensity;
};
/*
struct D3DResourceLeakChacker
{
	~D3DResourceLeakChacker()
	{
		Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug))))
		{
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
			debug->Release();
		}
	}
};
*/
//Shaderコンパイル用関数
/*
IDxcBlob* CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler)
{
	Log(ConvertString(std::format(L"Begine CompileShader,path:{},profile:{}\n", filePath, profile)));

	//hlslファイルを読む
	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT hr = dxcUtils->LoadFile(filePath.c_str(),nullptr,&shaderSource);
	assert(SUCCEEDED(hr));
	//読み込んだファイルの内容を設定
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;

	LPCWSTR arguments[] = {
		filePath.c_str(),
		L"-E",L"main",
		L"-T",profile,
		L"-Zi",L"-Qembed_debug",
		L"-Od",
		L"-Zpr",
	};

	//Shaderをコンパイルする
	IDxcResult* shaderResult = nullptr;
	hr = dxcCompiler->Compile(&shaderSourceBuffer,arguments,_countof(arguments),includeHandler,IID_PPV_ARGS(&shaderResult));
	//dxcが起動できない等の致命的な状況
	assert(SUCCEEDED(hr));

	//警告・エラーが出たらログに表示し止める
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS,IID_PPV_ARGS(&shaderError),nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0)
	{
		Log(shaderError->GetStringPointer());
		assert(false);
	}

	//コンパイル結果から実行用のバイナリ部分を取得
	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT,IID_PPV_ARGS(&shaderBlob),nullptr);
	assert(SUCCEEDED(hr));
	//成功したログをだす
	Log(ConvertString(std::format(L"Compile Succeeded ,path:{},profile:{}\n",filePath,profile)));
	//使わないリソースを解放
	shaderSource->Release();
	shaderResult->Release();

	//実行用のバイナリを返却
	return shaderBlob;
}
*/
//Resource作成
/*
ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes)
{
	//リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	//リソースの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	//バッファリソース。テクスチャの場合は別の設定をする
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = sizeInBytes;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//リソースを作る
	ID3D12Resource* resourse = nullptr;
	assert(SUCCEEDED(device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resourse))));
	return resourse;
}
*/
/*
ID3D12DescriptorHeap* CreateDescriptorHeap(ID3D12Device* device,D3D12_DESCRIPTOR_HEAP_TYPE heapType,UINT numDescriptors,bool shaderVisible)
{
	ID3D12DescriptorHeap* descreptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc,IID_PPV_ARGS(&descreptorHeap));
	assert(SUCCEEDED(hr));
	return descreptorHeap;
}
*/
/*
DirectX::ScratchImage LoadTexture(const std::string& filePath)
{
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(),DirectX::WIC_FLAGS_FORCE_SRGB,nullptr,image);
	assert(SUCCEEDED(hr));
	
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(),image.GetImageCount(),image.GetMetadata(),DirectX::TEX_FILTER_SRGB,0,mipImages);
	assert(SUCCEEDED(hr));

	return mipImages;
}
*/
/*
ID3D12Resource* CreateTextureResource(ID3D12Device* device,const DirectX::TexMetadata& metadata)
{
	//metadataをもとにのResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);
	resourceDesc.Height = UINT(metadata.height);
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);
	resourceDesc.Format = metadata.format;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);

	//利用するヒープの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	//heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	//heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	//Resourceの生成
	ID3D12Resource* resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr));
	return resource;
}
*/
/*
[[nodiscard]]
ID3D12Resource* UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages,ID3D12Device* device,ID3D12GraphicsCommandList* commandList)
{
	std::vector<D3D12_SUBRESOURCE_DATA> subresource;
	DirectX::PrepareUpload(device,mipImages.GetImages(),mipImages.GetImageCount(),mipImages.GetMetadata(),subresource);
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture,0,UINT(subresource.size()));
	ID3D12Resource* intermediateResource = CreateBufferResource(device,intermediateSize);
	UpdateSubresources(commandList,texture,intermediateResource, 0, 0,UINT(subresource.size()),subresource.data());
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	commandList->ResourceBarrier(1,&barrier);
	return intermediateResource;
}
*/
/*
ID3D12Resource* CreateDepthStencilTextureResource(ID3D12Device* device,int32_t width,int32_t height)
{
	//生成するResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;
	resourceDesc.Height = height;
	resourceDesc.MipLevels = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	//利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapPropaerties{};
	heapPropaerties.Type = D3D12_HEAP_TYPE_DEFAULT;

	//深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//Resourceの生成
	ID3D12Resource* resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(&heapPropaerties,D3D12_HEAP_FLAG_NONE,&resourceDesc,D3D12_RESOURCE_STATE_DEPTH_WRITE,&depthClearValue,IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr));
	return resource;
}

D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap,uint32_t descriptorSize,uint32_t index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize*index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}
*/




// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	//D3DResourceLeakChacker leacCheck;// = D3DResourceLeakChacker::GetInstance();
	assert(SUCCEEDED(CoInitializeEx(0,COINIT_MULTITHREADED)));

	Window* mainWindow=nullptr;
	mainWindow = new Window();
	mainWindow->CreateGameWindow(kTitle,kClientWidth,kClientHeight);
	

#ifdef _DEBUG
	ID3D12Debug1* debugController = nullptr;
	if(SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif 

	DirectXCommon* dxCommon = nullptr;
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(mainWindow);


	Input::GetInstance()->Initialize(mainWindow->GetHwnd());

	//TextureManagerの初期化
	TextureManager* textureManager = TextureManager::GetInstance();
	textureManager->Initialize(dxCommon->GetDevice());
	textureManager->SetDirectXCommon(dxCommon);
	textureManager->SetsrvDescriptorHeap(dxCommon->GetsrvDescriptorHeap());

	//Spriteの初期化
	Sprite::StaticInitialize(dxCommon->GetDevice(),mainWindow->GetClientWidth(), mainWindow->GetClientHeight());

	//
	Primitive3D::StaticInitialize(dxCommon->GetDevice(), mainWindow->GetClientWidth(), mainWindow->GetClientHeight());

	DeltaTime::GetInstance();

	const uint32_t descriptorSizeSRV = dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	dxCommon->SetDescriptorSizeSRV(descriptorSizeSRV);
	const uint32_t descriptorSizeRTV = dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	dxCommon->SetDescriptorSizeRTV(descriptorSizeRTV);
	const uint32_t descriptorSizeDSV = dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	dxCommon->SetDescriptorSizeDSV(descriptorSizeDSV);
	
	GameScene* gameScene = new GameScene();
	gameScene->Initialize(dxCommon);

	//dxCompilerを初期化
	IDxcUtils* dxcUtils = nullptr;
	IDxcCompiler3* dxcCompiler = nullptr;
	HRESULT hr = DxcCreateInstance(CLSID_DxcUtils,IID_PPV_ARGS(&dxcUtils));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler,IID_PPV_ARGS(&dxcCompiler));
	assert(SUCCEEDED(hr));

	//includeに対応するための設定を行う
	IDxcIncludeHandler* includeHandler = nullptr;
	hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
	assert(SUCCEEDED(hr));

	
	//RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	
	//RootParameter作成
	D3D12_ROOT_PARAMETER rootParameters[4] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[0].Descriptor.ShaderRegister = 0;

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].Descriptor.ShaderRegister = 0;
	
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);
	
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[3].Descriptor.ShaderRegister = 1;


	descriptionRootSignature.pParameters = rootParameters;
	descriptionRootSignature.NumParameters = _countof(rootParameters);
	
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplers[0].ShaderRegister = 0;
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);
	
	
	
	//シリアライズしてバイナリ化
	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRootSignature,D3D_ROOT_SIGNATURE_VERSION_1,&signatureBlob,&errorBlob);
	if (FAILED(hr))
	{
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	//バイナリを元に生成
	ID3D12RootSignature* rootSignature = nullptr;
	hr = dxCommon->GetDevice()->CreateRootSignature(0,signatureBlob->GetBufferPointer(),signatureBlob->GetBufferSize(),IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr));

	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	//02_04追加分
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	//法線
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;


	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	//BlendDtateの設定
	D3D12_BLEND_DESC blendDesc{};
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	//Shaderのコンパイル
	IDxcBlob* vertexShaderBlob = CompileShader(L"Object3D.VS.hlsl",L"vs_6_0",dxcUtils,dxcCompiler,includeHandler);
	assert(vertexShaderBlob != nullptr);
	IDxcBlob* pixelShaderBlob = CompileShader(L"Object3D.PS.hlsl", L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	
	//PSOの生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature;
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),vertexShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),pixelShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトポロジのタイプ、三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//どのように画面に色を打ち込むかの設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	
	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	
	
	//実際に生成
	ID3D12PipelineState* graphicsPipelineState = nullptr;
	hr = dxCommon->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,IID_PPV_ARGS(&graphicsPipelineState));
	assert(SUCCEEDED(hr));

	//DirectionalLight用のリソース
	ID3D12Resource* directinalLightResource = DirectXCommon::CreateBufferResource(dxCommon->GetDevice(),sizeof(DirectionalLight));
	DirectionalLight* directinalLightData = nullptr;
	directinalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directinalLightData));
	directinalLightData->color = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
	directinalLightData->direction={0.0f,-1.0f,0.0f};
	directinalLightData->intensity = 1.0f;

	const uint32_t kSubdivision = 16;
	const uint32_t vertexCount = kSubdivision * kSubdivision * 6;

	//頂点リソース
	ID3D12Resource* vertexResourse = DirectXCommon::CreateBufferResource(dxCommon->GetDevice(),sizeof(VertexData)*vertexCount);

	//頂点バッファ
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	vertexBufferView.BufferLocation = vertexResourse->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = sizeof(VertexData)* vertexCount;
	vertexBufferView.StrideInBytes = sizeof(VertexData);



	VertexData* vertexData = nullptr;
	vertexResourse->Map(0,nullptr,reinterpret_cast<void**>(&vertexData));
	
	const float kLonEvery = float(M_PI) * 2.0f / float(kSubdivision);
	const float kLatEvery = float(M_PI) / float(kSubdivision);
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex)
	{
		float lat = -float(M_PI) / 2.0f + kLatEvery * latIndex;
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex)
		{
			float u = float(lonIndex) / float(kSubdivision);
			float v =1.0f - float(latIndex+1.0f) / float(kSubdivision);

			uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
			float lon = lonIndex * kLonEvery;
			Vector4 a, b, c, d;
			a = { -cosf(lat) * cosf(lon)			  ,sinf(lat)            ,cosf(lat) * sin(lon),1.0f };
			b = { -cosf(lat + kLatEvery) * cosf(lon),sinf(lat + kLatEvery),cosf(lat + kLatEvery) * sin(lon),1.0f };
			c = { -cosf(lat) * cosf(lon + kLonEvery),sinf(lat)            , cosf(lat) * sin(lon + kLonEvery),1.0f };
			d = { -cosf(lat + kLatEvery) * cosf(lon + kLonEvery),sinf(lat + kLatEvery),cosf(lat + kLatEvery) * sin(lon + kLonEvery),1.0f };
			a *= 500.0f;
			b *= 500.0f;
			c *= 500.0f;
			d *= 500.0f;
			//1枚目
			vertexData[start].position = a;
			vertexData[start].texcoord = {u,v + 1.0f / float(kSubdivision) };
			vertexData[start].normal = { vertexData[start].position.x,vertexData[start].position.y,vertexData[start].position.z };
			vertexData[start + 1].position = b;
			vertexData[start + 1].texcoord = { u,v };
			vertexData[start + 1].normal = { vertexData[start+1].position.x,vertexData[start+1].position.y,vertexData[start+1].position.z };
			vertexData[start + 2].position = c;
			vertexData[start + 2].texcoord = { u + 1.0f / float(kSubdivision), v + 1.0f / float(kSubdivision) };
			vertexData[start + 2].normal = { vertexData[start+2].position.x,vertexData[start+2].position.y,vertexData[start+2].position.z };
			
			//2枚目
			vertexData[start + 3].position = b;
			vertexData[start + 3].texcoord = { u,v };
			vertexData[start + 3].normal = { vertexData[start+3].position.x,vertexData[start+3].position.y,vertexData[start+3].position.z };
			vertexData[start + 4].position = d;
			vertexData[start + 4].texcoord = { u + 1.0f / float(kSubdivision),v };
			vertexData[start + 4].normal = { vertexData[start+4].position.x,vertexData[start+4].position.y,vertexData[start+4].position.z };
			vertexData[start + 5].position = c;
			vertexData[start + 5].texcoord = { u + 1.0f / float(kSubdivision),v + 1.0f / float(kSubdivision) };
			vertexData[start + 5].normal = { vertexData[start+5].position.x,vertexData[start+5].position.y,vertexData[start+5].position.z };

		}
	}
	//マテリアル用のリソースを作成
	ID3D12Resource* materialResource = DirectXCommon::CreateBufferResource(dxCommon->GetDevice(),sizeof(Material));
	Material* materialData = nullptr;
	materialResource->Map(0,nullptr,reinterpret_cast<void**>(&materialData));
	materialData->color = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
	materialData->enableLighting = 0;
	//WVP用のリソースを作る。Matrix4x4一つ分のサイズを用意
	ID3D12Resource* wvpResource = DirectXCommon::CreateBufferResource(dxCommon->GetDevice(),sizeof(TransformationMatrix));
	TransformationMatrix* wvpData = nullptr;
	wvpResource->Map(0,nullptr,reinterpret_cast<void**>(&wvpData));
	wvpData->WVP = MakeIdentity4x4();
	wvpData->World = MakeIdentity4x4();

	//DepthStencilTextureをウィンドウのサイズで作成
	/*D3D12Resource* depthStencilResource = CreateDepthStencilTextureResource(dxCommon->GetDevice(), kClientWidth, kClientHeight);

	//DSV用のヒープでディスクリプタの数は1,DSVはShader内で触るものではないので.ShaderVisibleはfalse
	ID3D12DescriptorHeap* dsvDescriptorHeap = CreateDescriptorHeap(dxCommon->GetDevice(),D3D12_DESCRIPTOR_HEAP_TYPE_DSV,1,false);
	//DSVの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	//DSVHeapの先頭にDSVを作る
	dxCommon->GetDevice()->CreateDepthStencilView(depthStencilResource,&dsvDesc,dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	*/

	/*

	//Sprite用のリソースを作る
	ID3D12Resource* vertexResourceSprite = DirectXCommon::CreateBufferResource(dxCommon->GetDevice(),sizeof(VertexData)*6);
	//頂点バッファビューを作る
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};
	vertexBufferViewSprite.BufferLocation = vertexResourceSprite->GetGPUVirtualAddress();
	//使用するリソースのサイズ
	vertexBufferViewSprite.SizeInBytes = sizeof(VertexData) * 6;
	//1頂点当たりのサイズ
	vertexBufferViewSprite.StrideInBytes = sizeof(VertexData);
	
	VertexData* vertexDataSprite = nullptr;
	vertexResourceSprite->Map(0,nullptr,reinterpret_cast<void**>(&vertexDataSprite));
	//1枚目
	vertexDataSprite[0].position = {0.0f,360.0f,0.0f,1.0f};
	vertexDataSprite[0].texcoord = {0.0f,1.0f};
	vertexDataSprite[0].normal = {0.0f,0.0f,-1.0f};
	vertexDataSprite[1].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexDataSprite[1].texcoord = { 0.0f,0.0f };
	vertexDataSprite[1].normal = { 0.0f,0.0f,-1.0f };
	vertexDataSprite[2].position = { 640.0f,360.0f,0.0f,1.0f };
	vertexDataSprite[2].texcoord = { 1.0f,1.0f };
	vertexDataSprite[2].normal = { 0.0f,0.0f,-1.0f };
	//2枚目
	vertexDataSprite[3].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexDataSprite[3].texcoord = { 0.0f,0.0f };
	vertexDataSprite[3].normal = { 0.0f,0.0f,-1.0f };
	vertexDataSprite[4].position = { 640.0f,0.0f,0.0f,1.0f };
	vertexDataSprite[4].texcoord = { 1.0f,0.0f };
	vertexDataSprite[4].normal = { 0.0f,0.0f,-1.0f };
	vertexDataSprite[5].position = { 640.0f,360.0f,0.0f,1.0f };
	vertexDataSprite[5].texcoord = { 1.0f,1.0f };
	vertexDataSprite[5].normal = { 0.0f,0.0f,-1.0f };
	
	//Sprite用のTransformMatrix用リソースを作る
	ID3D12Resource* transformationMatrixResourceSprite = DirectXCommon::CreateBufferResource(dxCommon->GetDevice(),sizeof(TransformationMatrix));
	TransformationMatrix* transformationMatrixDataSprite = nullptr;
	transformationMatrixResourceSprite->Map(0,nullptr,reinterpret_cast<void**>(&transformationMatrixDataSprite));
	transformationMatrixDataSprite->WVP = MakeIdentity4x4();
	transformationMatrixDataSprite->World = MakeIdentity4x4();

	//スプライト用のマテリアルリソースを作成
	ID3D12Resource* materialResourceSprite = DirectXCommon::CreateBufferResource(dxCommon->GetDevice(), sizeof(Material));
	Material* materialDataSprite = nullptr;
	materialResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&materialDataSprite));
	materialDataSprite->color = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
	materialDataSprite->enableLighting = false;
	
	*/

	//ビューポート
	D3D12_VIEWPORT viewport{};
	viewport.Width = kClientWidth;
	viewport.Height = kClientHeight;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;


	//シザー矩形
	D3D12_RECT scissorRect{};
	scissorRect.left = 0;
	scissorRect.right = kClientWidth;
	scissorRect.top = 0;
	scissorRect.bottom = kClientHeight;


	//Transform変数を作る
	struct Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	struct Transform cameraTransform { {1.0f, 1.0f, 1.0f}, { 0.0f,0.0f,0.0f }, {0.0f,0.0f,-5.0f} };

	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f,float(kClientWidth)/float(kClientHeight),0.1f,100.0f);

	//ImGuiの初期化
	/*IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(mainWindow->GetHwnd());
	ImGui_ImplDX12_Init(dxCommon->GetDevice(),swapChainDesc.BufferCount,rtvDesc.Format,srvDescriptorHeap,
		srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		*/
	//textureの読み込み
	/*
	DirectX::ScratchImage mipImages = LoadTexture("Resources/uvChecker.png");
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	ID3D12Resource* textureResource = CreateTextureResource(dxCommon->GetDevice(), metadata);
	ID3D12Resource* intermediateResource = UploadTextureData(textureResource, mipImages,dxCommon->GetDevice(),commandList);

	DirectX::ScratchImage mipImages2 = LoadTexture("Resources/monsterBall.png");
	const DirectX::TexMetadata& metadata2 = mipImages2.GetMetadata();
	ID3D12Resource* textureResource2 = CreateTextureResource(dxCommon->GetDevice(), metadata2);
	ID3D12Resource* intermediateResource2 = UploadTextureData(textureResource2, mipImages2, dxCommon->GetDevice(), commandList);

	//コマンドリストの実行
	ID3D12CommandList* commandLists[] = { commandList };
	commandList->Close();
	commandQueue->ExecuteCommandLists(1, commandLists);

	//Fenceの値をこうしん
	fenceValue++;
	commandQueue->Signal(fence, fenceValue);

	//Fenceの値が指定したSignal値にたどり着いているか確認
	if (fence->GetCompletedValue() < fenceValue)
	{
		fence->SetEventOnCompletion(fenceValue, fenceEvent);
		//イベント待つ
		WaitForSingleObject(fenceEvent, INFINITE);
	}

	hr = commandAllocator->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList->Reset(commandAllocator, nullptr);
	assert(SUCCEEDED(hr));
	intermediateResource->Release();
	intermediateResource2->Release();
	*/
	 
	uint32_t uvCheckerTextureHandle	=TextureManager::LoadTexture("uvChecker.png");
	uint32_t monsterTextureHandle = TextureManager::LoadTexture("monsterBall.png");

	uint32_t sTextureHandle = TextureManager::LoadTexture("s.png");
	uint32_t gTextureHandle = TextureManager::LoadTexture("g.png");
	uint32_t nowTextureHandle = TextureManager::LoadTexture("now.png");

	uint32_t skyTextureHandle = TextureManager::LoadTexture("Sky.png");




	/*
	//metadataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	//SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();

	textureSrvHandleCPU.ptr += dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	textureSrvHandleGPU.ptr += dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//SRVの生成
	dxCommon->GetDevice()->CreateShaderResourceView(textureResource,&srvDesc,textureSrvHandleCPU);

	//2枚目
	//metadataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};
	srvDesc2.Format = metadata2.format;
	srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc2.Texture2D.MipLevels = UINT(metadata2.mipLevels);

	//SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU2 = GetCPUDescriptorHandle(srvDescriptorHeap,descriptorSizeSRV,2);
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU2 = GetGPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, 2);
	//SRVの生成
	dxCommon->GetDevice()->CreateShaderResourceView(textureResource2, &srvDesc2, textureSrvHandleCPU2);
	*/


	bool useMonsterBall = false;

	//Spriteクラスを利用したSprite
	struct Transform transformSprite { {1.0f, 1.0f, 1.0f}, { 0.0f,0.0f,0.0f }, { 320.0f,180.0f,0.0f } };
	Vector4 spriteColor = {1.0f,1.0f,1.0f,1.0f};
	Sprite* sprite = Sprite::Create(uvCheckerTextureHandle, Vector2{ 0.0f,0.0f }, Vector2{ 640.0f,360.0f }, spriteColor);

	//Matrix4x4 spriteUVTransform = MakeIdentity4x4();
	Vector3 uvTranslate = {0.0f,0.0f,0.0f};
	Vector3 uvScale = {1.0f,1.0f,1.0f};
	Vector3 uvRotate = {0.0f,0.0f,0.0f};

	struct Transform starttrans { {1.0f, 1.0f, 1.0f}, { 0.0f,0.0f,0.0f }, { 320.0f,300.0f,0.0f } };
	Sprite* start = Sprite::Create(sTextureHandle, Vector2{ 0.0f,0.0f }, Vector2{ 36.0f,36.0f }, spriteColor);

	struct Transform goaltrans { {1.0f, 1.0f, 1.0f}, { 0.0f,0.0f,0.0f }, { 320.0f,180.0f,0.0f } };
	Sprite* goal = Sprite::Create(gTextureHandle, Vector2{ 0.0f,0.0f }, Vector2{ 36.0f,36.0f }, spriteColor);

	struct Transform nowtrans { {1.0f, 1.0f, 1.0f}, { 0.0f,0.0f,0.0f }, { 320.0f,180.0f,0.0f } };
	Sprite* now = Sprite::Create(nowTextureHandle, Vector2{ 0.0f,0.0f }, Vector2{ 36.0f,36.0f }, spriteColor);
	Vector3 s = { 0.0f,0.0f,0.0f };
	Vector3 g = { 0.0f,0.0f,200.0f };
	float t = 0.0f;


	//player
	struct Transform transformPlayer { {1.0f, 1.0f, 1.0f}, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,30.0f } };
	Primitive3D* player = Primitive3D::Create(Vector2{ 0.0f,0.0f }, Vector2{ 640.0f,360.0f }, Vector4{0.0f,0.0f,1.0f,1.0f});
	Vector4 playerColor = {0.0f,0.0f,1.0f,0.0f};

	//Vector3 s = {0.0f,0.0f,0.0f};
	//Vector3 g = { 0.0f,0.0f,200.0f };
	//float t = 0.0f;

	MSG msg{};
	//メインループ
	DeltaTime::GameLoopStart();
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Input::GetInstance()->KeyboardUpdate();

			DeltaTime::FrameStart();
			//ゲームの処理

			ImGui_ImplDX12_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			//更新処理開始
			/*
			ImGui::Begin("fps");
			ImGui::Text("%3.2f", ImGui::GetIO().Framerate);
			ImGui::Text("%3.2f", ImGui::GetIO().Framerate * DeltaTime::GetDeltaTime());
			ImGui::Text("%f", DeltaTime::GetDeltaTime());
			ImGui::End();
			*/
			//ImGui::ShowDemoWindow();

			//ImGui::Begin("color");
			//ImGui::SliderFloat3("RGB",&materialData->x,0.0f,1.0f);
			//ImGui::End();
			/*ImGui::Begin("Window");
			ImGui::DragFloat3("translateSprite", &transformSprite.translate.x, 1.0f);
			ImGui::End();*/
			/*
			ImGui::Begin("Sphere");
			ImGui::SliderFloat3("CameraTranslate", &cameraTransform.translate.x, -10.0f, 10.0f,0 );
			ImGui::Checkbox("useMonsterBall", &useMonsterBall);
			ImGui::SliderFloat3("DirectionalLight", &directinalLightData->direction.x, -1.0f, 1.0f, 0);
			ImGui::SliderFloat("Intensity", &directinalLightData->intensity, 0.0f, 1.0f, 0);
			//ImGui::SliderFloat3("DirectionalLightColor", &directinalLightData->color.x, 0.0f, 1.0f, 0);
			ImGui::ColorEdit4("DirectionalLightColor" ,&directinalLightData->color.x);
			ImGui::RadioButton("LightingMode : NONE ", &materialData->enableLighting, 0);
			ImGui::RadioButton("LightingMode : Lambert ", &materialData->enableLighting, 1);
			ImGui::RadioButton("LightingMode : HalfLambert ", &materialData->enableLighting, 2);
			ImGui::End();
			*/
			directinalLightData->direction = Nomalize(directinalLightData->direction);
			

			//transform.rotate.y += 0.01f;
			cameraTransform.translate = t * g;
			t += 0.001f;
			if (t >= 1.0f)
			{
				t = 1.0f;
			}
			Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale,transform.rotate,transform.translate);
			Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale,cameraTransform.rotate,cameraTransform.translate);
			Matrix4x4 viewMatrix = Inverse(cameraMatrix);
			//Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f,float(kClientWidth)/float(kClientHeight),0.1f,100.0f);
			Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix,Multiply(viewMatrix,projectionMatrix));
			//*wvpData = worldMatrix;
			wvpData->WVP = worldViewProjectionMatrix;
			wvpData->World = worldMatrix;
			
			if (Input::GetKey(DIK_RIGHT))
			{
				transformPlayer.translate.x+=3.0f*DeltaTime::GetDeltaTime();
			}
			if (Input::GetKey(DIK_LEFT))
			{
				transformPlayer.translate.x-= 3.0f * DeltaTime::GetDeltaTime();
			}
			if (Input::GetKey(DIK_UP))
			{
				transformPlayer.translate.y+= 3.0f * DeltaTime::GetDeltaTime();
			}
			if (Input::GetKey(DIK_DOWN))
			{
				transformPlayer.translate.y-= 3.0f * DeltaTime::GetDeltaTime();
			}
			Matrix4x4 worldMatrixPlayer = MakeAffineMatrix(transformPlayer.scale, transformPlayer.rotate, transformPlayer.translate);
			worldMatrixPlayer = Multiply(worldMatrixPlayer, cameraMatrix);
			Matrix4x4 worldViewProjectionMatrixPlayer = Multiply(worldMatrixPlayer, Multiply(viewMatrix, projectionMatrix));



			//Sprite用のworldviewProjection
			Matrix4x4 worldMatrixSprite = MakeAffineMatrix(transformSprite.scale,transformSprite.rotate,transformSprite.translate);
			Matrix4x4 viewMatrixSprite = MakeIdentity4x4();
			Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f,0.0f,float(kClientWidth),float(kClientHeight),0.0f,100.0f);
			Matrix4x4 worldViewProjectionMatrixSprite = Multiply(worldMatrixSprite,Multiply(viewMatrixSprite,projectionMatrixSprite));
			Matrix4x4 uvTransformSprite = MakeAffineMatrix(uvScale,uvRotate,uvTranslate);

			sprite->SetWVP(worldViewProjectionMatrixSprite);
			sprite->SetUVTransform(uvTransformSprite);


			Matrix4x4 worldMatrixs = MakeAffineMatrix(starttrans.scale, starttrans.rotate, starttrans.translate);
			Matrix4x4 worldViewProjectionMatrixs = Multiply(worldMatrixs, Multiply(viewMatrixSprite, projectionMatrixSprite));
			start->SetWVP(worldViewProjectionMatrixs);
			start->SetUVTransform(uvTransformSprite);


			Matrix4x4 worldMatrixg = MakeAffineMatrix(goaltrans.scale, goaltrans.rotate, goaltrans.translate);
			Matrix4x4 worldViewProjectionMatrixg = Multiply(worldMatrixg, Multiply(viewMatrixSprite, projectionMatrixSprite));
			goal->SetWVP(worldViewProjectionMatrixg);
			goal->SetUVTransform(uvTransformSprite);


			nowtrans.translate = (1.0f - t) * starttrans.translate + t * goaltrans.translate;

			Matrix4x4 worldMatrixn = MakeAffineMatrix(nowtrans.scale, nowtrans.rotate, nowtrans.translate);
			Matrix4x4 worldViewProjectionMatrixn = Multiply(worldMatrixn, Multiply(viewMatrixSprite, projectionMatrixSprite));
			now->SetWVP(worldViewProjectionMatrixn);
			now->SetUVTransform(uvTransformSprite);
			
			gameScene->Update();
			//更新処理終了
			 
			//画面の初期化
			/*//コマンドの確定
			UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();
			
			//TransitionBarrierの設定
			D3D12_RESOURCE_BARRIER barrier{};
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource = swapChainResources[backBufferIndex];
			//遷移前
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
			//遷移後
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
			//TransitionBarrierを張る
			commandList->ResourceBarrier(1,&barrier);
			
			//描画先のRTVとDSVを設定
			D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
			commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, &dsvHandle);
			float clearColor[] = {0.1f,0.25f,0.5f,1.0f};//RGBA
			commandList->ClearRenderTargetView(rtvHandles[backBufferIndex],clearColor,0,nullptr);
			*/
			/*
			
			commandList->SetGraphicsRootSignature(rootSignature);
			commandList->SetPipelineState(graphicsPipelineState);
			commandList->IASetVertexBuffers(0,1,&vertexBufferView);
			commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			//マテリアルCBufferの場所を設定
			commandList->SetGraphicsRootConstantBufferView(0,materialResource->GetGPUVirtualAddress());
			//wvp用のCBufferの場所を設定
			commandList->SetGraphicsRootConstantBufferView(1,wvpResource->GetGPUVirtualAddress());

			//Lighting用のリソースの場所を設定
			commandList->SetGraphicsRootConstantBufferView(3, directinalLightResource->GetGPUVirtualAddress());

			ID3D12DescriptorHeap* descriptorHeaps[] = {srvDescriptorHeap};
			commandList->SetDescriptorHeaps(1,descriptorHeaps);

			//SRVのDescriptorTableの先頭を設定。
			commandList->SetGraphicsRootDescriptorTable(2,useMonsterBall ? textureSrvHandleGPU2 : textureSrvHandleGPU);

			*/


			dxCommon->PreDraw();
			
			//描画
			ID3D12GraphicsCommandList* commandList = dxCommon->GetCommandList();


			commandList->SetGraphicsRootSignature(rootSignature);
			commandList->SetPipelineState(graphicsPipelineState);
			commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
			commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			//マテリアルCBufferの場所を設定
			commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
			//wvp用のCBufferの場所を設定
			commandList->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
			//Lighting用のリソースの場所を設定
			commandList->SetGraphicsRootConstantBufferView(3, directinalLightResource->GetGPUVirtualAddress());

			ID3D12DescriptorHeap* descriptorHeaps[] = { dxCommon->GetsrvDescriptorHeap()};
			commandList->SetDescriptorHeaps(1, descriptorHeaps);

			//SRVのDescriptorTableの先頭を設定。
			//commandList->SetGraphicsRootDescriptorTable(2, useMonsterBall ? textureSrvHandleGPU2 : textureSrvHandleGPU);
			TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(2,skyTextureHandle);

			commandList->DrawInstanced(vertexCount, 1, 0, 0);
			gameScene->Draw3D();
			/*
			Primitive3D::PreDraw(dxCommon->GetCommandList());

			player->Draw(worldViewProjectionMatrixPlayer,playerColor);

			Primitive3D::PostDraw();
			*/
			Sprite::PreDraw(dxCommon->GetCommandList());

			gameScene->Draw2D();

			//sprite->Draw();
			//start->Draw();
			//goal->Draw();
			//now->Draw();

			Sprite::PostDraw();

			dxCommon->PostDraw();

			//描画
			//commandList->DrawInstanced(vertexCount,1,0,0);

			/*
			//Spriteの描画
			commandList->SetGraphicsRootConstantBufferView(0, materialResourceSprite->GetGPUVirtualAddress());
			commandList->SetGraphicsRootDescriptorTable(2,textureSrvHandleGPU);
			commandList->IASetVertexBuffers(0,1,&vertexBufferViewSprite);
			commandList->SetGraphicsRootConstantBufferView(1,transformationMatrixResourceSprite->GetGPUVirtualAddress());
			commandList->DrawInstanced(6,1,0,0);
			
			//ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(),commandList);

			//描く処理終了、画面に移すため状態を遷移
			/*barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
			//TransitionBarrierを張る
			commandList->ResourceBarrier(1, &barrier);
			
			//コマンドの確定(最後にやる)
			hr = commandList->Close();
			assert(SUCCEEDED(hr));

			//コマンドリストの実行
			ID3D12CommandList* commandLists[] = {commandList};
			commandQueue->ExecuteCommandLists(1,commandLists);
			swapChain->Present(1,0);

			//Fenceの値をこうしん
			fenceValue++;
			commandQueue->Signal(fence,fenceValue);

			//Fenceの値が指定したSignal値にたどり着いているか確認
			if (fence->GetCompletedValue()<fenceValue)
			{
				fence->SetEventOnCompletion(fenceValue,fenceEvent);
				//イベント待つ
				WaitForSingleObject(fenceEvent,INFINITE);
			}

			hr = commandAllocator->Reset();
			assert(SUCCEEDED(hr));
			hr = commandList->Reset(commandAllocator,nullptr);
			assert(SUCCEEDED(hr));
			*/
			//Sleep(1000 * DWORD(1.0f / 60.0f));
			//Sleep(1000);
		}
	}

	Log(ConvertString(std::format(L"WSTRING:{}\n",msg.message)));

	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();


	//解放処理
	delete dxCommon;
	//CloseHandle(fenceEvent);
	//fence->Release();
	//rtvDescriptorHeap->Release();
	//srvDescriptorHeap->Release();
	//dsvDescriptorHeap->Release();
	//swapChainResources[0]->Release();
	//swapChainResources[1]->Release();
	//swapChain->Release();
	//commandList->Release();
	//commandAllocator->Release();
	//commandQueue->Release();
	//device->Release();
	//useAdapter->Release();
	//dxgiFactory->Release();
#ifdef _DEBUG
	debugController->Release();
#endif // _DEBUG
	CloseWindow(mainWindow->GetHwnd());

	directinalLightResource->Release();

	vertexResourse->Release();
	graphicsPipelineState->Release();
	signatureBlob->Release();
	if (errorBlob)
	{
		errorBlob->Release();
	}
	rootSignature->Release();
	pixelShaderBlob->Release();
	vertexShaderBlob->Release();

	materialResource->Release();
	//materialResourceSprite->Release();

	wvpResource->Release();

	//textureResource->Release();
	//textureResource2->Release();

	//depthStencilResource->Release();

	//vertexResourceSprite->Release();
	//transformationMatrixResourceSprite->Release();

	//リソースリークチェック
	/*IDXGIDebug1* debug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug))))
	{
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		debug->Release();
	}
	*/
	CoUninitialize();

	return 0;
}