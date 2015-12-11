#pragma once

/**
 * 用于显示数字的文本框 
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
	 *  创建字体, 
	 * fontfile为字体所在的文件名, 不需要包含扩展名. 比如 common/number_font_small, 将自动使用 number_font_small.plist number_font_small.png作为资源.
	* @ForceShowSign : 强制显示符号.  如果不强制显示符号,  正数前面将不会显示+号. 
	*/
	static VNumberLabel* create(const char* fontname, bool ForceShowSign = false);

	/**
	* 创建倍数. 将显示为 X Number (X2 X3)这样的样式的数字框
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

