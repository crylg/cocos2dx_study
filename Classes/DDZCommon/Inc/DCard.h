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
	DCN_BLACK_JOKER,		// 小王(黑小丑)
	DCN_RED_JOKER,			// 大王(红小丑)
};

#define DCN_SMALL_KING DCN_BLACK_JOKER
#define DCN_BIG_KING DCN_RED_JOKER

enum DCardColor : unsigned char
{
	DCC_NULL = 0,
	DCC_DIAMONDS,	// 红方
	DCC_CLUBS,		// 梅花
	DCC_HEARTS,		// 红心
	DCC_SPADES,		// 黑桃
	DCC_LAIZI,      //癞子
};


/**
 * 手牌类型 
 */
enum DHandType : unsigned char
{
	DHT_UNKNOWN = 0,		// 无效的牌型.
	DHT_1,					// 个子
	DHT_2,					// 对子
	DHT_3,                  //三不带
	DHT_3_1,                //三不带
	DHT_3_2,                //三代二
	DHT_4_2,		//      //四带二
	DHT_straight,	//  顺子
	DHT_straight_2,	//  双顺	
	DHT_straight_3,	//  3顺	plane
	//DHT_plane,		//  飞机
	DHT_4,			//  炸弹
	DHT_2K,			//  对王

};


enum
{
	DDECK_NUM_CARDS = 54,
	DDECK_NUM_POCKETS = 3,	// 底牌数量
	DHAND_NUM_CARDS_NORMAL = 17,
	DHAND_NUM_CARDS_MAX = 20,	// 最多的一手牌数量(地主 17 + 3)	
};

enum DDeckCharacterType
{
	DDCT_LORD = 0,//地主
	DDCT_PRE_LORD,//地主上家
	DDCT_POST_LORD,//地主下家
	DDCT_MAX,
};

#pragma pack(push)
#pragma pack(1)

/*
* 描述一张扑克牌
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
	 * 对于斗地主而言, 只需要数字相等即可 
	 */
	inline bool operator ==(const DCard& Other) const	{	return Number == Other.Number;}
	inline bool operator !=(const DCard& Other) const	{	return Number != Other.Number;}
	inline bool operator <(const DCard& Other) const	{	return Number < Other.Number;}
	inline bool operator <=(const DCard& Other) const	{	return Number <= Other.Number;}
	inline bool operator >(const DCard& Other) const	{	return Number > Other.Number;}
	inline bool operator >=(const DCard& Other) const	{	return Number >= Other.Number;}

};

#pragma pack(pop)
