#pragma once

enum DCardNumber : unsigned char
{
	DCN_NULL = 0,
	DCN_3,
	DCN_4,
	DCN_5,
	DCN_6,
	DCN_7,
	DCN_8,
	DCN_9,
	DCN_10,
	DCN_J,
	DCN_Q,
	DCN_K,
	DCN_A,
	DCN_2,
	DCN_BLACK_JOKER,		// С��(��С��)
	DCN_RED_JOKER,			// ����(��С��)
};

#define DCN_SMALL_KING DCN_BLACK_JOKER
#define DCN_BIG_KING DCN_RED_JOKER

enum DCardColor : unsigned char
{
	DCC_NULL = 0,
	DCC_DIAMONDS,	// �췽
	DCC_CLUBS,		// ÷��
	DCC_HEARTS,		// ����
	DCC_SPADES,		// ����
	DCC_LAIZI,      //���
};


/**
 * �������� 
 */
enum DHandType : unsigned char
{
	DHT_UNKNOWN = 0,		// ��Ч������.
	DHT_1,					// ����
	DHT_2,					// ����
	DHT_3,                  //������
	DHT_3_1,                //������
	DHT_3_2,                //������
	DHT_4_2,		//      //�Ĵ���
	DHT_straight,	//  ˳��
	DHT_straight_2,	//  ˫˳	
	DHT_straight_3,	//  3˳	plane
	//DHT_plane,		//  �ɻ�
	DHT_4,			//  ը��
	DHT_2K,			//  ����

};


enum
{
	DDECK_NUM_CARDS = 54,
	DDECK_NUM_POCKETS = 3,	// ��������
	DHAND_NUM_CARDS_NORMAL = 17,
	DHAND_NUM_CARDS_MAX = 20,	// ����һ��������(���� 17 + 3)	
};

enum DDeckCharacterType
{
	DDCT_LORD = 0,//����
	DDCT_PRE_LORD,//�����ϼ�
	DDCT_POST_LORD,//�����¼�
	DDCT_MAX,
};

#pragma pack(push)
#pragma pack(1)

/*
* ����һ���˿���
*
*/
class DCard
{
public:
	static const DCard SMALL_KING;
	static const DCard BIG_KING;
	union 
	{
		unsigned short Code;
		struct  
		{
			DCardColor Color;
			DCardNumber Number;
		};
	};

	inline DCard():Code(0){}
	inline DCard(DCardColor _Color, DCardNumber _Number) 
		: Color(_Color),Number(_Number){}

	inline DCard(const DCard& Card) :Code(Card.Code){}
	inline ~DCard(){}

	inline bool IsValid() const { return Code != 0; }
	inline void Clear() { Code = 0; }
	inline DCard& operator= (const DCard& Card)
	{
		Code = Card.Code;
		return *this;
	}

	inline bool IsKing() const { return Number > DCN_2; }
	inline bool IsKinOr2() const { return Number >= DCN_2; }
	inline bool Is2() const { return Number == DCN_2; }
	
	/**
	 * ���ڶ���������, ֻ��Ҫ������ȼ��� 
	 */
	inline bool operator ==(const DCard& Other) const	{	return Number == Other.Number;}
	inline bool operator !=(const DCard& Other) const	{	return Number != Other.Number;}
	inline bool operator <(const DCard& Other) const	{	return Number < Other.Number;}
	inline bool operator <=(const DCard& Other) const	{	return Number <= Other.Number;}
	inline bool operator >(const DCard& Other) const	{	return Number > Other.Number;}
	inline bool operator >=(const DCard& Other) const	{	return Number >= Other.Number;}

};

#pragma pack(pop)
