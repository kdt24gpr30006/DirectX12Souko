#include "FbxMesh.h"
#include <d3dcompiler.h>
#include <string>
#include <array>
#include "Graphics/DirectX/DirectX.h"
#include "Graphics/ConstantBuffer/ConstantBuffer.h"
#include "Graphics/Texture/Texture.h"
#include "Graphics/Renderer/Renderer.h"
#include "Graphics/Graphics.h"
#include "Math/Math.h"
#include "System/Camera/Camera.h"


FbxMesh::FbxMesh()
	: Vertices()
	, Indices()
	, Materials()
	, MaterialsSrc()
	, MeshBufferInfo()
	, Bones()
	, AnimationList()
	, RootSignature(nullptr)
	, PipelineStateDesc()
	, PipelineState(nullptr)
	, VertexShaderBlob()
	, InputLayouts()
	, PixelShaderBlob(nullptr)
	, TransformBuffer(nullptr)
	, Position(Math::Vector3::Zero)
	, Rotation(Math::Quaternion::Identity)
	, Scale(Math::Vector3::One)
	, MeshColor(Color::White)
	, Frame(0.0f)
	, CurrentAnimationName()
{
}
FbxMesh::~FbxMesh()
{
	Release();
}

/// <summary>
/// コンバート済みFbxモデルファイルの読み込み
/// </summary>
/// <param name="FilePath">ファイルパス XXX.fbx.bin</param>
/// <returns>true:成功</returns>
bool FbxMesh::Create(const std::filesystem::path& FilePath)
{
	//	ファイルが存在するかチェック
	if (std::filesystem::exists(FilePath) == false)
	{
		return false;
	}
	//	コンバートしたfbx.binファイルの読み込み
	bool ret = LoadModelData(FilePath.string());
	if (ret == false)
	{
		return false;
	}

	//	3D用シェーダーの作成
	ret = CreateShader();
	if (ret == false)
	{
		return false;
	}

	//	ルートシグネチャの作成
	ret = CreateRootSignature();
	if (ret == false)
	{
		return false;
	}

	//	パイプラインの作成
	ret = CreatePipeline();
	if (ret == false)
	{
		return false;
	}
	
	//	姿勢、カメラ、プロジェクション用の定数バッファ作成
	MeshBufferInfo.Transform = Math::Matrix::identity;
	MeshBufferInfo.View = Math::Matrix::identity;
	MeshBufferInfo.Projection = Math::Matrix::identity;
	MeshBufferInfo.MeshColor = Color::White;

	TransformBuffer = new ConstantBuffer();
	ret = TransformBuffer->Create(MeshConstantBufferSize);
	if (ret == false)
	{
		return false;
	}
	TransformBuffer->Update(&MeshBufferInfo);

	return true;
}

/// <summary>
/// コンバート済みアニメーション(モーション)ファイルの読み込み
/// </summary>
/// <param name="AnimationName">アニメーション名</param>
/// <param name="AnimationPath">ファイルパス XXX.fbx.anm</param>
/// <returns>true:成功</returns>
bool FbxMesh::LoadAnimation(const std::string& AnimationName, const std::filesystem::path& AnimationPath)
{
	//	ファイルが存在するかチェック
	if (std::filesystem::exists(AnimationPath) == false)
	{
		return false;
	}

	bool ret = LoadAnimationData(AnimationName, AnimationPath.string());
	if (ret == false)
	{
		return false;
	}
	return true;
}


void FbxMesh::Release()
{
	Vertices.clear();
	Indices.clear();
	SafeRelease(VertexShaderBlob);
	SafeRelease(PixelShaderBlob);
	SafeRelease(RootSignature);
	SafeRelease(PipelineState);
	
	if (TransformBuffer != nullptr)
	{
		TransformBuffer->Release();
		delete TransformBuffer;
		TransformBuffer = nullptr;
	}

	for (auto& itr : Materials)
	{
		if (itr.Tex != nullptr)
		{
			itr.Tex->Release();
			delete itr.Tex;
			itr.Tex = nullptr;
		}
	}
}

void FbxMesh::Render()
{
	MeshBufferInfo.Transform.Update(Position, Rotation, Scale);

	//	DirectXのシェーダーの仕様で行と列を反転しないといけない
	MeshBufferInfo.Transform = Math::Matrix::Transpose(MeshBufferInfo.Transform);
	MeshBufferInfo.View = Math::Matrix::Transpose(Camera::Main->GetView());
	MeshBufferInfo.Projection = Math::Matrix::Transpose(Camera::Main->GetProjection());
	MeshBufferInfo.MeshColor = MeshColor;

	TransformBuffer->Update(&MeshBufferInfo);

	Graphics::DirectX* DirextXInstance = Graphics::DirectX::GetInstance();
	ID3D12GraphicsCommandList* CommandList = DirextXInstance->GetCommandList();

	CommandList->SetGraphicsRootSignature(RootSignature);
	CommandList->SetPipelineState(PipelineState);
	CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	TransformBuffer->Set(0);

#if true
	Graphics::Renderer* Instance = Graphics::Renderer::GetInstance();
	for (auto& itr : Materials)
	{
		itr.Tex->Set(1);
		Instance->DrawMesh(
			itr.Vertices.data(), itr.Vertices.size(),
			itr.Indices.data(), itr.Indices.size()
		);
		const UINT IndexCount = static_cast<UINT>(itr.PolygonCount) * 3;
		//CommandList->DrawIndexedInstanced(IndexCount, 1, IndexLocation, 0, 0);
		//IndexLocation += IndexCount;
	}
#else
	//const UINT IndexCount = static_cast<UINT>(Indices.size());
	//CommandList->DrawIndexedInstanced(IndexCount, 1, 0, 0, 0);

	UINT IndexLocation = 0;
	for (auto& itr : Materials)
	{
		itr.Tex->Set(1);
		const UINT IndexCount = static_cast<UINT>(itr.PolygonCount) * 3;
		CommandList->DrawIndexedInstanced(IndexCount, 1, IndexLocation, 0, 0);
		IndexLocation += IndexCount;
	}

#endif
}


/// <summary>
	/// アニメーション用更新処理(フレームを進める)
	/// </summary>
	/// <param name="AnimationName">Load時に登録したアニメーション名</param>
	/// <param name="deltaTime"></param>
	/// <param name="bIsLoop">アニメーションをループさせるか</param>
void FbxMesh::Animate(
	const std::string& AnimationName,
	const float deltaTime,
	const bool bIsLoop)
{
	if (AnimationList.count(AnimationName) == 0)
	{
		//読み込まれていないアニメーション
		return;
	}

	if (CurrentAnimationName != AnimationName)
	{
		//再生するアニメーションの変更
		Frame = 0.0f;
		CurrentAnimationName = AnimationName;
	}

	const Animation& anim = AnimationList[CurrentAnimationName];
	const float Time = static_cast<float>(anim.NumFrame) * 0.016666666f;

	Frame += deltaTime;
	if (Frame >= Time)
	{
		if (bIsLoop == true)
		{
			//	ループ用処理
			Frame -= Time;
		}
		else
		{
			Frame = Time;
		}
		bIsAnimationFinish = true;

	}
	else
	{
		bIsAnimationFinish = false;
	}

	Skinning(CurrentAnimationName);
}
/// <summary>
/// アニメーションの現在のフレームを0~1.0fの割合で取得する
/// </summary>
/// <returns>0~1.0f</returns>
float FbxMesh::GetCurrentAnimationFrameRate()const
{
#if true
	return bIsAnimationFinish;
#else
	if (AnimationList.count(CurrentAnimationName) == 0)
	{
		//TODO:警告ログを出す
		return 0.0f;
	}

	const Animation& anim = AnimationList.at(CurrentAnimationName);
	const float Time = static_cast<float>(anim.NumFrame) * 0.016666666f;

	const float rate = Math::Clamp(Frame / Time, 0.0f, 1.0f);
	return rate;
#endif
}


const Math::Vector3& FbxMesh::GetPosition()const
{
	return Position;
}
 Math::Vector3& FbxMesh::GetPosition()
{
	return Position;
}
void FbxMesh::SetPosition(const Math::Vector3& position)
{
	Position = position;
}

const Math::Quaternion& FbxMesh::GetRotation()const
{
	return Rotation;
}
void FbxMesh::SetRotation(const Math::Quaternion& rotaion)
{
	Rotation = rotaion;
}

const Math::Vector3& FbxMesh::GetScale()const
{
	return Scale;
}
void FbxMesh::SetScale(const Math::Vector3& scale)
{
	Scale = scale;
}

void FbxMesh::SetColor(const Color& color)
{
	MeshColor = color;
}

bool FbxMesh::LoadModelData(const std::string& FilePath)
{
	FILE* fp = nullptr;
	const errno_t error = fopen_s(&fp, FilePath.c_str(), "rb");
	if (error != 0)
	{
		return false;
	}

	std::filesystem::path Directory = FilePath;
	Directory.remove_filename();

	int MeshCount = 0;
	fread(&MeshCount, sizeof(int), 1, fp);
	int PolygonCount = 0;
	fread(&PolygonCount, sizeof(int), 1, fp);
	int VertexCount = 0;
	fread(&VertexCount, sizeof(int), 1, fp);

	Vertices.reserve(VertexCount);
	for (int i = 0; i < VertexCount; i++)
	{
		Graphics::SkeletalMeshVertex v = {};
		fread(&v.Position, sizeof(Math::Vector3), 1, fp);
		fread(&v.UV, sizeof(Math::Vector2), 1, fp);
		fread(&v.Normal, sizeof(Math::Vector3), 1, fp);
		fread(v.Bone.data(), sizeof(int) * v.Bone.size(), 1, fp);
		fread(v.Weight.data(), sizeof(int) * v.Weight.size(), 1, fp);
		Vertices.push_back(v);
	}

	int IndexCount = 0;
	fread(&IndexCount, sizeof(int), 1, fp);
	Indices.reserve(IndexCount);
	for (int i = 0; i < IndexCount; i++)
	{
		int index = 0;
		fread(&index, sizeof(int), 1, fp);
		Indices.push_back(index);
	}

	int MaterialCount = 0;
	fread(&MaterialCount, sizeof(int), 1, fp);
	Materials.reserve(MaterialCount);
	for (int i = 0; i < MaterialCount; i++)
	{
		Material readMaterial = {};

		//マテリアル名
		int NameLen = 0;
		fread(&NameLen, sizeof(int), 1, fp);
		readMaterial.Name.resize(NameLen);
		fread(readMaterial.Name.data(), NameLen, 1, fp);

		//テクスチャパス(読み込み)
		int TexturePathLen = 0;
		fread(&TexturePathLen, sizeof(int), 1, fp);
		std::string TexturePath = {};
		TexturePath.resize(TexturePathLen);
		fread(TexturePath.data(), TexturePathLen, 1, fp);
		
		//モデルの階層のTexture用フォルダを参照するようにする
		readMaterial.Tex = new Texture();
		readMaterial.Tex->Create(Directory/"Texture"/TexturePath);
		
		//ポリゴン数
		fread(&readMaterial.PolygonCount, sizeof(int), 1, fp);
		Materials.push_back((Material&&)readMaterial);
	}

	/*
	* ボーン情報読み込み
	*/
	int BoneCount = 0;
	fread(&BoneCount, sizeof(int), 1, fp);
	Bones.reserve(BoneCount);
	for (int i = 0; i < BoneCount; i++)
	{
		BoneInfo info = {};
		
		//ボーン名
		int NameLen = 0;
		fread(&NameLen, sizeof(int), 1, fp);
		info.Name.resize(NameLen);
		fread(info.Name.data(), NameLen, 1, fp);

		//親のボーンインデックス
		fread(&info.ParentIndex, sizeof(int), 1, fp);

		info.Transform = Math::Matrix::identity;
		//初期ポーズ用姿勢行列
		fread(&info.BindMatrix, sizeof(Math::Matrix), 1, fp);
		
		Bones.push_back((BoneInfo&&)info);
	}

	fclose(fp);

	int offset = 0;
	for (auto& m : Materials)
	{
		for (int i = 0; i < m.PolygonCount * 3; i++)
		{
			const uint32_t index = Indices[offset + i];
			const Graphics::SkeletalMeshVertex& vertex = Vertices[index];
			m.Vertices.push_back(vertex);
			m.Indices.push_back((uint32_t)i);
		}
		offset += m.PolygonCount * 3;
	}

	//	意図したコピー
	MaterialsSrc = Materials;

	return true;
}

/// <summary>
/// コンバート済みfbxアニメーションデータの読み込み
/// </summary>
/// <param name="FilePath"></param>
/// <returns></returns>
bool FbxMesh::LoadAnimationData(const std::string& AnimationName, const std::string& FilePath)
{
	if (AnimationList.count(AnimationName) > 0)
	{
		//すでに同一の名前でアニメーションデータが存在する
		return false;
	}

	FILE* fp = nullptr;
	const errno_t error = fopen_s(&fp, FilePath.c_str(), "rb");
	if (error != 0)
	{
		return false;
	}

	Animation Anim = {};
	//アニメーションのフレーム数
	fread(&Anim.NumFrame, sizeof(int), 1, fp);

	/*
	* KeyFrame[ボーン数][アニメーションフレーム数]
	* という行列データ
	*/
	int NumBone = 0;
	fread(&NumBone, sizeof(int), 1, fp);
	Anim.KeyFrames.resize(NumBone);
	for (int bone = 0; bone < NumBone; bone++)
	{
		int frame = 0;
		fread(&frame, sizeof(int), 1, fp);
		Anim.KeyFrames[bone].resize(frame);

		for (int f = 0; f < frame; f++)
		{
			fread(&Anim.KeyFrames[bone][f], sizeof(Math::Matrix), 1, fp);
		}
	}
	fclose(fp);

	AnimationList[AnimationName] = (Animation&&)Anim;
	return true;
}

/// <summary>
/// スキニング処理
/// </summary>
/// <param name="AnimationName"></param>
void FbxMesh::Skinning(const std::string& AnimationName)
{
	Math::Matrix Keyframe[256];

	const Animation& Anim = AnimationList[AnimationName];

	//	配列用変数
	const float rate = Frame - (int)(Frame);
	int frame = (int)(Frame * 60.0f);
	frame = Math::Clamp<int>(frame, 0, Anim.NumFrame - 1);
	int nextFrame = frame + 1;
	nextFrame = Math::Clamp<int>(nextFrame, 0, Anim.NumFrame - 1);

#if true
	const int NumBone = static_cast<int>(Bones.size());
	for (int b = 0; b < NumBone; b++)
	{
		BoneInfo& bone = Bones[b];
		//行列補間
		Math::Matrix m = Math::Matrix::identity;
		if (frame != nextFrame)
		{
			m = Math::Matrix::Lerp(
				Anim.KeyFrames[b][frame],
				Anim.KeyFrames[b][nextFrame],
				rate);
		}
		else
		{
			m = Anim.KeyFrames[b][frame];
		}

		if (bone.ParentIndex >= 0)
		{
			const Math::Matrix tmpM = m;
			const Math::Matrix trans = Bones[bone.ParentIndex].Transform;

			m._11 = tmpM._11 * trans._11 + tmpM._12 * trans._21 + tmpM._13 * trans._31 + tmpM._14 * trans._41;
			m._12 = tmpM._11 * trans._12 + tmpM._12 * trans._22 + tmpM._13 * trans._32 + tmpM._14 * trans._42;
			m._13 = tmpM._11 * trans._13 + tmpM._12 * trans._23 + tmpM._13 * trans._33 + tmpM._14 * trans._43;
			m._14 = tmpM._11 * trans._14 + tmpM._12 * trans._24 + tmpM._13 * trans._34 + tmpM._14 * trans._44;

			m._21 = tmpM._21 * trans._11 + tmpM._22 * trans._21 + tmpM._23 * trans._31 + tmpM._24 * trans._41;
			m._22 = tmpM._21 * trans._12 + tmpM._22 * trans._22 + tmpM._23 * trans._32 + tmpM._24 * trans._42;
			m._23 = tmpM._21 * trans._13 + tmpM._22 * trans._23 + tmpM._23 * trans._33 + tmpM._24 * trans._43;
			m._24 = tmpM._21 * trans._14 + tmpM._22 * trans._24 + tmpM._23 * trans._34 + tmpM._24 * trans._44;

			m._31 = tmpM._31 * trans._11 + tmpM._32 * trans._21 + tmpM._33 * trans._31 + tmpM._34 * trans._41;
			m._32 = tmpM._31 * trans._12 + tmpM._32 * trans._22 + tmpM._33 * trans._32 + tmpM._34 * trans._42;
			m._33 = tmpM._31 * trans._13 + tmpM._32 * trans._23 + tmpM._33 * trans._33 + tmpM._34 * trans._43;
			m._34 = tmpM._31 * trans._14 + tmpM._32 * trans._24 + tmpM._33 * trans._34 + tmpM._34 * trans._44;

			m._41 = tmpM._41 * trans._11 + tmpM._42 * trans._21 + tmpM._43 * trans._31 + tmpM._44 * trans._41;
			m._42 = tmpM._41 * trans._12 + tmpM._42 * trans._22 + tmpM._43 * trans._32 + tmpM._44 * trans._42;
			m._43 = tmpM._41 * trans._13 + tmpM._42 * trans._23 + tmpM._43 * trans._33 + tmpM._44 * trans._43;
			m._44 = tmpM._41 * trans._14 + tmpM._42 * trans._24 + tmpM._43 * trans._34 + tmpM._44 * trans._44;

		}

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				bone.Transform.Mat[i][j] = m.Mat[i][j];
			}
		}

		//	キーフレーム
		Keyframe[b] = bone.BindMatrix * m;
	}
#else
	const int NumBone = static_cast<int>(Bones.size());
	for (int b = 0; b < NumBone; b++)
	{
		const BoneInfo& bone = Bones[b];

		Keyframe[b] = Math::Matrix::identity;
		//Keyframe[b] = bone.BindMatrix;
		//Keyframe[b] = Math::Matrix::Inverse(bone.BindMatrix);
		//Keyframe[b] = Math::Matrix::RotationY(90 * Math::RAD);
		//Keyframe[b] = Math::Matrix::Scaling(2,2,2);
	}
#endif

	//	頂点変形
	const int NumMaterial = static_cast<int>(Materials.size());
	for (int m = 0; m < NumMaterial; m++)
	{
		Material& material = Materials[m];
		const Material& materialSrc = MaterialsSrc[m];
		const int NumVertices = static_cast<int>(materialSrc.Vertices.size());
		for (int v = 0; v < NumVertices; v++)
		{
			Graphics::SkeletalMeshVertex& vertex = material.Vertices[v];
			const Graphics::SkeletalMeshVertex& Src = materialSrc.Vertices[v];
			int count = 0;
			for (const int b : vertex.Bone)
			{
				if (b < 0)
				{
					break;
				}
				count++;
			}

			if (count <= 0)
			{
				continue;
			}

			vertex.Position.x = 0;
			vertex.Position.y = 0;
			vertex.Position.z = 0;

			//	影響個数分ループ
			for (int i = 0; i < count; i++)
			{
				const int boneIndex = vertex.Bone[i];

				const Math::Matrix& key = Keyframe[boneIndex];
				const float weight = Src.Weight[i];

				const float x = Src.Position.x;
				const float y = Src.Position.y;
				const float z = Src.Position.z;
				//	座標を影響力分移動
				vertex.Position.x += (x * key._11 + y * key._21 + z * key._31 + key._41) * weight;
				vertex.Position.y += (x * key._12 + y * key._22 + z * key._32 + key._42) * weight;
				vertex.Position.z += (x * key._13 + y * key._23 + z * key._33 + key._43) * weight;

				const float nx = Src.Normal.x;
				const float ny = Src.Normal.y;
				const float nz = Src.Normal.z;

				//	法線を影響力分変換
				vertex.Normal.x = (nx * key._11 + ny * key._21 + nz * key._31) * weight;
				vertex.Normal.y = (nx * key._12 + ny * key._22 + nz * key._32) * weight;
				vertex.Normal.z = (nx * key._13 + ny * key._23 + nz * key._33) * weight;
			}
		}

		//ModelVertexBuffer->Update(Vertices.data());
	}

#if false
	//	モーション用行列更新
	ModelBuffer* pData = nullptr;
	TransformBuffer->Map(0, nullptr, (void**)&pData);
	memcpy(pData->Bone, Keyframe, sizeof(Matrix) * NumBone);
	TransformBuffer->Unmap(0, nullptr);
#endif
}


bool FbxMesh::CreateShader()
{
	//	シェーダーコンパイル用処理
	auto CompileShader = [](const char* FileName, const char* FunctionName, const char* ShaderModel)
		{
			size_t wLen = 0;
			wchar_t wideFileName[256] = {};
			//	ファイル名変換
			mbstowcs_s(&wLen, wideFileName, ARRAYSIZE(wideFileName), FileName, _TRUNCATE);

			//	シェーダー作成
			ID3DBlob* ShaderBlob = nullptr;
			ID3DBlob* ErrorBlob = nullptr;

			const HRESULT hr = D3DCompileFromFile(wideFileName,
				nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
				FunctionName, ShaderModel,
				D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
				0, &ShaderBlob, &ErrorBlob);

			if (FAILED(hr))
			{
				OutputDebugString((char*)ErrorBlob->GetBufferPointer());
				SafeRelease(ErrorBlob);
			}

			return ShaderBlob;
		};

	VertexShaderBlob = CompileShader("Assets/Shader/VS_3D.hlsl", "main", "vs_5_0");
	if (VertexShaderBlob == nullptr)
	{
		return false;
	}
	D3D12_INPUT_ELEMENT_DESC element = {};
	element.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	element.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	element.InstanceDataStepRate = 0;
	element.InputSlot = 0;

	element.SemanticName = "POSITION";
	element.SemanticIndex = 0;
	element.Format = DXGI_FORMAT_R32G32B32_FLOAT;//float3
	InputLayouts.push_back(element);

	element.SemanticName = "TEXCOORD";
	element.SemanticIndex = 0;
	element.Format = DXGI_FORMAT_R32G32_FLOAT;//float2
	InputLayouts.push_back(element);

	element.SemanticName = "NORMAL";
	element.SemanticIndex = 0;
	element.Format = DXGI_FORMAT_R32G32B32_FLOAT;//float3
	InputLayouts.push_back(element);

	element.SemanticName = "BONE_INDEX";
	element.SemanticIndex = 0;
	element.Format = DXGI_FORMAT_R32G32B32A32_SINT;//int[4]
	InputLayouts.push_back(element);

	element.SemanticName = "WEIGHT";
	element.SemanticIndex = 0;
	element.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;//float[4]
	InputLayouts.push_back(element);

	PixelShaderBlob = CompileShader("Assets/Shader/PS_3D.hlsl", "main", "ps_5_0");
	if (PixelShaderBlob == nullptr)
	{
		return false;
	}

	return true;
}

bool FbxMesh::CreateRootSignature()
{
	std::array<D3D12_DESCRIPTOR_RANGE,2> Range = {};
	Range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	Range[0].NumDescriptors = 1;
	Range[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	Range[0].BaseShaderRegister = 0;
	Range[0].RegisterSpace = 0;

	Range[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	Range[1].NumDescriptors = 1;
	Range[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	Range[1].BaseShaderRegister = 0;
	Range[1].RegisterSpace = 0;

	//	ルートパラメーター設定
	std::array<D3D12_ROOT_PARAMETER, 2> RootParam = {};
	RootParam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	RootParam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	RootParam[0].DescriptorTable.pDescriptorRanges = &Range[0];
	RootParam[0].DescriptorTable.NumDescriptorRanges = 1;

	RootParam[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	RootParam[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	RootParam[1].DescriptorTable.pDescriptorRanges = &Range[1];
	RootParam[1].DescriptorTable.NumDescriptorRanges = 1;


	//	サンプラー設定
	D3D12_STATIC_SAMPLER_DESC SamplerDesc = {};
	SamplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	SamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	SamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	SamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	SamplerDesc.MipLODBias = 0;
	SamplerDesc.MaxAnisotropy = 16;
	SamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_EQUAL;
	SamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
	SamplerDesc.MinLOD = 0;
	SamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	SamplerDesc.ShaderRegister = 0;
	SamplerDesc.RegisterSpace = 0;
	SamplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_DESC RootSignatureDesc = {};
	RootSignatureDesc.pParameters = RootParam.data();
	RootSignatureDesc.NumParameters = static_cast<UINT>(RootParam.size());
	RootSignatureDesc.pStaticSamplers = &SamplerDesc;
	RootSignatureDesc.NumStaticSamplers = 1;
	RootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//	ルートシグネチャ設定
	ID3DBlob* RootSignatureBlob = nullptr;
	ID3DBlob* ErrorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&RootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &RootSignatureBlob, &ErrorBlob);
	if (FAILED(hr))
	{
		OutputDebugString((char*)ErrorBlob->GetBufferPointer());
		SafeRelease(ErrorBlob);
		return false;
	}

	ID3D12Device* Device = Graphics::DirectX::GetInstance()->GetDevice();
	hr = Device->CreateRootSignature(0, RootSignatureBlob->GetBufferPointer(), RootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&RootSignature));
	SafeRelease(RootSignatureBlob);
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

bool FbxMesh::CreatePipeline()
{
	PipelineStateDesc.pRootSignature = RootSignature;

	//コンパイルした頂点シェーダーの情報を格納
	PipelineStateDesc.VS.pShaderBytecode = VertexShaderBlob->GetBufferPointer();
	PipelineStateDesc.VS.BytecodeLength = VertexShaderBlob->GetBufferSize();
	PipelineStateDesc.InputLayout.pInputElementDescs = InputLayouts.data();
	PipelineStateDesc.InputLayout.NumElements = static_cast<UINT>(InputLayouts.size());

	//コンパイルしたピクセルシェーダーの情報を格納
	PipelineStateDesc.PS.pShaderBytecode = PixelShaderBlob->GetBufferPointer();
	PipelineStateDesc.PS.BytecodeLength = PixelShaderBlob->GetBufferSize();


	//	ブレンドステート設定
	PipelineStateDesc.BlendState = GetBlendStateDesc();
	//	ラスタライザー設定
	PipelineStateDesc.RasterizerState = GetRasterizerDesc();
	//	深度ステンシル設定
	PipelineStateDesc.DepthStencilState = GetDepthStencilDesc();

	PipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	PipelineStateDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//	描画先のレンダーターゲットの設定
	for (auto& rtv : PipelineStateDesc.RTVFormats)
	{
		rtv = DXGI_FORMAT_UNKNOWN;
	}
	PipelineStateDesc.NumRenderTargets = 1;
	for (UINT i = 0; i < PipelineStateDesc.NumRenderTargets; i++)
	{
		PipelineStateDesc.RTVFormats[i] = DXGI_FORMAT_R8G8B8A8_UNORM;
	}
	
	PipelineStateDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	PipelineStateDesc.SampleDesc.Count = 1;
	PipelineStateDesc.SampleDesc.Quality = 0;

	//	パイプライン作成
	ID3D12Device* Device = Graphics::DirectX::GetInstance()->GetDevice();
	const HRESULT hr = Device->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&PipelineState));

	if (FAILED(hr))
	{
		return false;
	}
	return true;
}

/// <summary>
/// 深度ステンシルの初期設定の取得
/// </summary>
/// <returns></returns>
D3D12_DEPTH_STENCIL_DESC FbxMesh::GetDepthStencilDesc()
{
	D3D12_DEPTH_STENCIL_DESC Desc = {};
	Desc.DepthEnable = TRUE;
	Desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	Desc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	Desc.StencilEnable = FALSE;
	Desc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
	Desc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;

	const D3D12_DEPTH_STENCILOP_DESC defaultStencilOp =
	{
		D3D12_STENCIL_OP_KEEP,
		D3D12_STENCIL_OP_KEEP,
		D3D12_STENCIL_OP_KEEP,
		D3D12_COMPARISON_FUNC_NEVER
	};
	Desc.BackFace = defaultStencilOp;
	Desc.FrontFace = defaultStencilOp;

	return Desc;
}

/// <summary>
/// ブレンドステートの初期設定の取得
/// </summary>
/// <returns></returns>
D3D12_BLEND_DESC FbxMesh::GetBlendStateDesc()
{
	D3D12_BLEND_DESC Desc = {};
	Desc.AlphaToCoverageEnable = FALSE;
	Desc.IndependentBlendEnable = FALSE;
	D3D12_RENDER_TARGET_BLEND_DESC RenderTargetBlendDesc = {};
	RenderTargetBlendDesc.BlendEnable = FALSE;
	RenderTargetBlendDesc.LogicOpEnable = FALSE;
	RenderTargetBlendDesc.SrcBlend = D3D12_BLEND_ONE;
	RenderTargetBlendDesc.DestBlend = D3D12_BLEND_ZERO;
	RenderTargetBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
	RenderTargetBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	RenderTargetBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	RenderTargetBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	RenderTargetBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
	RenderTargetBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	for (auto& rt : Desc.RenderTarget)
	{
		rt = RenderTargetBlendDesc;
	}
	return Desc;
}

/// <summary>
/// ラスタライザーの初期設定の取得
/// </summary>
/// <returns></returns>
D3D12_RASTERIZER_DESC FbxMesh::GetRasterizerDesc()
{
	D3D12_RASTERIZER_DESC Desc = {};
	Desc.FillMode = D3D12_FILL_MODE_SOLID;
	Desc.CullMode = D3D12_CULL_MODE_NONE;
	Desc.FrontCounterClockwise = FALSE;
	Desc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	Desc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	Desc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	Desc.DepthClipEnable = TRUE;
	Desc.MultisampleEnable = FALSE;
	Desc.AntialiasedLineEnable = FALSE;
	Desc.ForcedSampleCount = 0;
	Desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return Desc;
}