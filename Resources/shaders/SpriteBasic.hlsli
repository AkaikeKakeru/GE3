//マテリアル
cbuffer ConstBufferDataMaterial:register(b0) {
	//色(RGBA)
	float4 color;
}

//頂点シェーダ―の出力構造体
// (頂点シェーダーからピクセルシェーダーへのやり取りに使用する)
struct VSOutput {
	//システム用の出力構造体
	float4 svPos : SV_POSITION;
	// uv値
	float2 uv : TEXCOORD;
};