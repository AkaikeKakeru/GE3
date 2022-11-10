#include "Matrix4.h"
#include "Vector3.h"
#include <cmath> //sin cos

// �P�ʍs������߂�
Matrix4 Mat4Identity() {
	static const Matrix4 result{
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f,
	};

	return result;
}

//�g��k���s��̐ݒ�
Matrix4 Mat4Scale(const Vector3& s) {
	Matrix4 result{
		s.x,0.0f,0.0f,0.0f,
		0.0f, s.y,0.0f,0.0f,
		0.0f,0.0f, s.z,0.0f,
		0.0f,0.0f,0.0f,1.0f,
	};

	return result;
}

// ��]�s��̐ݒ�
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

//���s�ړ��s��̐ݒ�
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
	//�s��
	const int LineNum = 4;

	//���ݍs
	int i = 0;
	//���ݗ�
	int j = 0;
	//���ڒ��̑Ίp�������A���݂����
	int focus = 0;

	//�ŏI�o��
	Matrix4 result = {};
	//�|���o���@�p�s��
	float sweep[LineNum][LineNum * 2] = {};

	//sweep�ɁA�Ώۍs��ƒP�ʍs����Z�b�g
	for (i = 0; i < LineNum; i++) {
		for (j = 0; j < LineNum; j++) {
			//�������ɑΏۍs���
			sweep[i][j] = m.m[i][j];

			//�E�����ɒP�ʍs���
			if (i == j) {
				sweep[i][LineNum + j] = 1;
			}
			else {
				sweep[i][LineNum + j] = 0;
			}
		}
	}

	//�|���o���@�ɂ�����A[���ڒ��̑Ίp�������A���݂����]�� focus �Ƃ���B
	//�S�Ă̗�̑Ίp�������I���܂ŌJ��Ԃ��B
	for (focus = 0; focus < LineNum; focus++) {
		/*0���Z�����Ȃ��悤�A�΍􏈗�*/
		//�ő�̐�Βl���A���ڒ��̑Ίp�����̐�Βl�ł���ƁA���ɒ�߂�B
		float max = fabs(sweep[focus][focus]);
		int max_i = focus;

		//focus��ڂ��ő�̐�Βl�ɂȂ�s��T���B
		for (i = focus + 1; i < LineNum; i++) {
			//�b��ő��Βl�����A��Βl���傫��������������A�b�����ւ�
			if (fabs(sweep[i][focus]) > max) {
				max = fabs(sweep[i][focus]);
				max_i = i;
			}
		}

		//�t�s��v�Z���s�\���Ɣ��f������E�_
		const double MAX_ERR = 1e-10;

		//�t�s��̌v�Z���s�\�Ɣ��f�����ꍇ�A�����I��
		if (static_cast<double>(fabs(sweep[max_i][focus])) <= MAX_ERR) {
			return m;
		}

		//focus�s�ڂ�max_i�s�ڂ����ւ���
		if (focus != max_i) {
			for (j = 0; j < LineNum * 2; j++) {
				float tmp = sweep[max_i][j];
				sweep[max_i][j] = sweep[focus][j];
				sweep[focus][j] = tmp;
			}
		}

		/*�Ίp���㐬���𐳋K������*/
		//sweep[focus][focus]�Ɋ|����ƁA1�ɂȂ�l
		float normalize = 1 / sweep[focus][focus];

		//focus�s�ڂ�normalize�{�ɂ���
		for (j = 0; j < LineNum * 2; j++) {
			//�Ίp����̐�����1�ɂȂ�B
			sweep[focus][j] *= normalize;
		}

		/*forcus�s�ڈȊO�́Afocus��ڂ�0�ɂ���*/
		for (i = 0; i < LineNum; i++) {
			//focus�s�ڂ̓X���[(�Ίp���㐬��������)
			if (i == focus) {
				continue;
			}

			//sweep[i][focus]�̕��������ւ����l
			//���{���̈Ӗ��́A�s���ȉ�͂��₂����o�����Ƃ��A�@���������������肵�ĘR�k���Ȃ��悤�ɂ��邱��
			float zeroization = -sweep[i][focus];

			//�Ίp����ȊO��0�ɂ���B
			for (j = 0; j < LineNum * 2; j++) {
				sweep[i][j] += sweep[focus][j] * zeroization;
			}
		}
	}

	/*sweep�̉E�������o�͂���*/
	for (i = 0; i < LineNum; i++) {
		for (j = 0; j < LineNum; j++) {
			result.m[i][j] = sweep[i][LineNum + j];
		}
	}

	return result;
}

// ���W�ϊ� (�x�N�g���ƍs��̊|���Z������)
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

// ������Z�q�I�[�o�[���[�h
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