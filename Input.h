#pragma once
#include <windows.h>
#include <wrl.h>
//using namespace Microsoft::WRL;

#define DIRECT_VERSION 0x0800	//DirectInput�̃o�[�W�����w��
#include <dinput.h>

//����
class Input
{
	//namespace�̏ȗ�
public:
	template <class Type>
	using ComPtr = Microsoft::WRL::ComPtr<Type>;

	//��{�I�ȃ����o�֐�
public:
	//������
	void Initialize(HINSTANCE hInstance,
		HWND hwnd);
	//�X�V
	void Update();

	//Input�ŗL�̃����o�֐�
public:
	/// <summary>
	/// �L�[���������܂�Ă��邩���m�F
	/// </summary>
	/// <param name="keyNum">�L�[�̔ԍ�</param>
	/// <returns>�������܂�Ă��邩</returns>
	bool ifKeyPress(BYTE keyNum);

	/// <summary>
	/// �L�[���������ꂽ�����m�F
	/// </summary>
	/// <param name="keyNum">�L�[�̔ԍ�</param>
	/// <param name="keyPreNum">1F�O�̃L�[�̔ԍ�</param>
	/// <returns>�������ꂽ��</returns>
	bool ifKeyTrigger(BYTE keyNum, BYTE keyPreNum);

	//�悭�g�������o�ϐ�
private:
	//�L�[�{�[�h�f�o�C�X
	ComPtr<IDirectInputDevice8> keyboard = nullptr;

	BYTE key[256] = {};
	BYTE keyPre[256] = {};
};
