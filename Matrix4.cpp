#include "Matrix4.h"
#include "Vector3.h"
#include <cmath> //sin cos

// 単位行列を求める
Matrix4 Mat4Identity() {
	static const Matrix4 result{
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f,
	};

	return result;
}

//拡大縮小行列の設定
Matrix4 Mat4Scale(const Vector3& s) {
	Matrix4 result{
		s.x,0.0f,0.0f,0.0f,
		0.0f, s.y,0.0f,0.0f,
		0.0f,0.0f, s.z,0.0f,
		0.0f,0.0f,0.0f,1.0f,
	};

	return result;
}

// 回転行列の設定
Matrix4 Mat4RotationX(float angle) {
	float sin = std::sin(angle);
	float cos = std::cos(angle);

	Matrix4 result{
		1.0f,0.0f,0.0f,0.0f,
		0.0f, cos, sin,0.0f,
		0.0f,-sin, cos,0.0f,
		0.0f,0.0f,0.0f,1.0f,
	};

	return result;
}

Matrix4 Mat4RotationY(float angle) {
	float sin = std::sin(angle);
	float cos = std::cos(angle);

	Matrix4 result{
		cos,0.0f,-sin,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		sin,0.0f, cos,0.0f,
		0.0f,0.0f,0.0f,1.0f,
	};

	return result;
}

Matrix4 Mat4RotationZ(float angle) {
	float sin = std::sin(angle);
	float cos = std::cos(angle);

	Matrix4 result{
		cos, sin,0.0f,0.0f,
		-sin, cos,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f,
	};

	return result;
}

//平行移動行列の設定
Matrix4 Mat4Translation(const Vector3& t) {
	Matrix4 result{
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		t.x, t.y, t.z,1.0f,
	};

	return result;
}

Matrix4 Mat4Transposed(const Matrix4& m) {
	Matrix4 result{
		m.m[0][0],m.m[1][0],m.m[2][0],m.m[3][0],
		m.m[0][1],m.m[1][1],m.m[2][1],m.m[3][1],
		m.m[0][2],m.m[1][2],m.m[2][2],m.m[3][2],
		m.m[0][3],m.m[1][3],m.m[2][3],m.m[3][3],
	};

	return result;
}

Matrix4 Mat4Inverse(const Matrix4& m) {
	//行数
	const int LineNum = 4;

	Matrix4 result = {};
	float sweep[LineNum][LineNum * 2] = {};

	//sweepに、対象行列と単位行列をセット
	for (int i = 0; i < LineNum; i++) {
		for (int j = 0; j < LineNum; j++) {

			sweep[i][j] = m.m[i][j];

			if (i == j) {
				sweep[i][LineNum + j] = 1;
			}
			else {
				sweep[i][LineNum + j] = 0;
			}
		}
	}

	//掃き出し法における、[注目中の対角成分が、存在する列]を focus とする。
	//全ての列の対角成分が終わるまで繰り返す。
	for (int focus = 0; focus < LineNum; focus++) {
		/*対角線上成分を正規化する*/
		//sweep[focus][focus]に掛けると、1になる値
		float normalize = 1 / sweep[focus][focus];

		//focus行目をnormalize倍にする
		for (int j = 0; j < LineNum * 2; j++) {
			//対角線上の成分が1になる。
			sweep[focus][j] *= normalize;
		}

		/*forcus行目以外の、focus列目を0にする*/
		for (int i = 0; i < LineNum; i++) {
			//focus行目はスルー(対角線上成分を除く)
			if (i == focus) {
				continue;
			}

			//sweep[i][focus]の符号を入れ替えた値
			//※本来の意味は、不正な解析や改竄が発覚したとき、機密情報を消去したりして漏洩しないようにすること
			float zeroization = -sweep[i][focus];

			//対角線上以外を0にする。
			for (int j = 0; j < LineNum * 2; j++) {
				sweep[i][j] += sweep[focus][j] * zeroization;
			}
		}
	}

	/*sweepの右半分を出力する*/
	for (int i = 0; i < LineNum; i++) {
		for (int j = 0; j < LineNum; j++) {
			result.m[i][j] = sweep[i][LineNum + j];
		}
	}

	return result;
}

// 座標変換 (ベクトルと行列の掛け算をする)
Vector3 Vec3Transform(const Vector3& v, const Matrix4& m) {
	float v_[4] = { v.x,v.y,v.z,1 };

	//v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + (1 * ) m.m[3][3];
	float w = 0;

	for (int i = 0; i < 4; i++) {
		w += (v_[i] * m.m[i][3]);
	}

	Vector3 result = {};
	/*{
	(v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + m.m[3][0]) / w,
	(v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + m.m[3][1]) / w,
	(v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + m.m[3][2]) / w,
};*/

	float ans[3] = { 0,0,0 };

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 4; j++) {
			ans[i] += v_[i] * m.m[j][i] / w;
		}
	}

	result = {
		ans[0],
		ans[1],
		ans[2]
	};

	return result;
}

// 代入演算子オーバーロード
Matrix4& operator*=(Matrix4& m1, const Matrix4& m2) {
	Matrix4 result = { 0 };

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++) {
				result.m[i][j] += m1.m[i][k] * m2.m[k][j];
			}
		}
	}

	return m1 = result;
}

const Matrix4 operator*(const Matrix4& m1, const Matrix4& m2) {
	Matrix4 result = m1;
	return result *= m2;
}

const Vector3 operator*(const Vector3& v, const Matrix4& m) {
	return Vec3Transform(v, m);
}