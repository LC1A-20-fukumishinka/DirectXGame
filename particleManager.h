#pragma once

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include <forward_list>
#include "../Camera.h"
/// <summary>
/// 3Dオブジェクト
/// </summary>
class ParticleManager
{
private: // エイリアス
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public: // サブクラス
	struct VertexPos
	{
		XMFLOAT3 pos; // xyz座標
		float scale; //スケール

	};
	// 定数バッファ用データ構造体
	struct ConstBufferData
	{
		XMMATRIX mat;	// ３Ｄ変換行列
		XMMATRIX matBillboard;	// ３Ｄ変換行列
		XMFLOAT4 color;	// 色 (RGBA)
	};

	struct Particle
	{
		//DirectX::を省略
		using XMFLOAT3 = DirectX::XMFLOAT3;

		//座標
		XMFLOAT3 position = {};
		//速度
		XMFLOAT3 velocity = {};
		//加速度
		XMFLOAT3 accel = {};
		//現在のフレーム
		int frame = 0;
		//終了フレーム
		int num_frame = 0;
		//スケール
		float scale = 1.0f;
		//初期値
		float s_scale = 1.0f;
		//最終値
		float e_scale = 0.0f;

		XMFLOAT4 s_color = { 1, 1, 1, 1 };

		XMFLOAT4 e_color = { 1, 1, 1, 1 };

	};
private: // 定数
	static const int division = 50;					// 分割数
	static const float radius;				// 底面の半径
	static const float prizmHeight;			// 柱の高さ
	static const int planeCount = division * 2 + division * 2;		// 面の数
	//static const int vertexCount = planeCount * 3;		// 頂点数
	static const int vertexCount = 256;		// 頂点数
	//static const int indexCount = 3 * 2;
public: // 静的メンバ関数

	/// <summary>
	/// 静的初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="window_width">画面幅</param>
	/// <param name="window_height">画面高さ</param>
	/// <returns>成否</returns>
	static bool StaticInitialize(Camera *camera);

	/// <summary>
	/// 3Dオブジェクト生成
	/// </summary>
	/// <returns></returns>
	static ParticleManager *Create();


	static void SetCamera(Camera camera);



private: // 静的メンバ変数
	// デバイス
	static ID3D12Device *device;
	// ルートシグネチャ
	static ComPtr<ID3D12RootSignature> rootsignature;
	// パイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState> pipelinestate;

	static Camera *camera;

private:// 静的メンバ関数

	/// <summary>
	/// グラフィックパイプライン生成
	/// </summary>
	/// <returns>成否</returns>
	static bool InitializeGraphicsPipeline();

	///// <summary>
	///// テクスチャ読み込み
	///// </summary>
	///// <returns>成否</returns>
	//static bool LoadTexture();


public: // メンバ関数
	ParticleManager();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();


	void UpdateConstantBuffer();
	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="textureNumber">描画するテクスチャのインデックス</param>
	void Draw(int textureNumber);

	/// <summary>
	/// パーティクル生成
	/// </summary>
	/// <param name="life">描画の時間</param>
	/// <param name="position">場所</param>
	/// <param name="velocity">移動ベクトル</param>
	/// <param name="accel">加速ベクトル</param>
	/// <param name="start_scale">初期サイズ</param>
	/// <param name="end_scale">終了時サイズ</param>
	void Add(int life, XMFLOAT3 position, XMFLOAT3 velocity, XMFLOAT3 accel,
		float start_scale, float end_scale, XMFLOAT4 start_color = { 1, 1, 1, 1 }, XMFLOAT4 end_color = { 1, 1, 1,1 });
private: // メンバ変数
	ComPtr<ID3D12Resource> constBuff; // 定数バッファ

	// ローカルスケール
	XMFLOAT3 scale = { 1,1,1 };

	std::forward_list<Particle> particles;

	// 頂点バッファ
	ComPtr<ID3D12Resource> vertBuff;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView;

	VertexPos vertices[vertexCount];

	XMFLOAT4 setColor = { 1, 1, 1, 1 };

private:
	/// <summary>
/// モデル作成
/// </summary>
	void CreateModel();

};

const DirectX::XMFLOAT3 operator+(const DirectX::XMFLOAT3 &lhs, const DirectX::XMFLOAT3 &rhs);
