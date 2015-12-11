#pragma once

typedef UINT64 VAccountGuid;

#define VNICKNAME_MAX 32

class VAccount
{
public:
	VAccountGuid AccountGuid;			/// �˻�ΨһID. 0 ��ʶ��Ч. 
	char  NickName[VNICKNAME_MAX];		/// �ǳ�
	//char  Avatar[];
};


class IVAccountView
{

};

class VAccountSystem
{
public:
	VAccountSystem();
	~VAccountSystem();

private:
	VAccount mAccount;
};

