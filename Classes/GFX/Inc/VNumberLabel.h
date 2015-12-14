#pragma once

/**
 * ������ʾ���ֵ��ı��� 
 */
class VCoreApi VNumberLabel : public cocos2d::SpriteBatchNode, public VAllocator
{
	enum Number
	{
		N0 = 0,
		N1,
		N2,
		N3,
		N4,
		N5,
		N6,
		N7,
		N8,
		N9,
		N_ADD,
		N_SUB,
		N_MUL,
		N_MAX,
	};
public:
	VNumberLabel();
	~VNumberLabel();
	virtual void DeleteThis() { VDelete this; }

	/**
	 *  ��������, 
	 * fontfileΪ�������ڵ��ļ���, ����Ҫ������չ��. ���� common/number_font_small, ���Զ�ʹ�� number_font_small.plist number_font_small.png��Ϊ��Դ.
	* @ForceShowSign : ǿ����ʾ����.  �����ǿ����ʾ����,  ����ǰ�潫������ʾ+��. 
	*/
	static VNumberLabel* create(const char* fontname, bool ForceShowSign = false);

	/**
	* ��������. ����ʾΪ X Number (X2 X3)��������ʽ�����ֿ�
	*/
	static VNumberLabel* createFactor(const char* fontname);

	bool initWithTexture(cocos2d::Texture2D* Texture, const char* font_name);

	inline int GetNumber() const { return mNumber; }

	void SetNumber(int Number);

protected:
	virtual void UpdateNumber();
	cocos2d::Sprite* GetSprite(Number n);
protected:
	cocos2d::Texture2D* mSpriteSheet;
	cocos2d::SpriteFrame* mFrames[N_MAX];
	int mNumber;
	bool mAsFactor;
	bool mAwaysShowSign;
};

