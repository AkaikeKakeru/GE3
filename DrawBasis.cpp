#include "DrawBasis.h"
#include <DirectXMath.h>
#include "Vector3.h"
#include <wrl.h>

using namespace DirectX;

void DrawBasis::Initialize(){
	//頂点データ
	Vector3 vertices[] = {
		//x		 y		z
		{-5.0f, -5.0f, -5.0f},//左下
		{-5.0f,  5.0f, -5.0f},//左上
		{ 5.0f, -5.0f, -5.0f},//右下
	};

	//頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	UINT sizeVB = static_cast<UINT>(sizeof(Vector3) * _countof(vertices));
}