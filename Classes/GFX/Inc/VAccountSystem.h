#pragma once

typedef UINT64 VAccountGuid;

#define VNICKNAME_MAX 32

class VAccount
{
public:
	VAccountGuid AccountGuid;			/// 账户唯一ID. 0 标识无效. 
	char  NickName[VNICKNAME_MAX];		/// 昵称
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

