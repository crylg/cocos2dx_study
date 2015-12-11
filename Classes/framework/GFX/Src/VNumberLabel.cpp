#include "VStdAfx.h"
#include "VNumberLabel.h"
USING_NS_CC;



VNumberLabel* VNumberLabel::create(const char* fontname, bool ForceShowSign)
{
	SpriteFrameCache* Cache = SpriteFrameCache::getInstance();
	std::string _fontfile = fontname;
	std::string plist = _fontfile + ".plist";
	std::string texture = _fontfile + ".png";
	Cache->addSpriteFramesWithFile(plist, texture);
	Texture2D* pTex = Director::getInstance()->getTextureCache()->addImage(texture);
	if (pTex)
	{
		VNumberLabel* Label = VNew VNumberLabel;
		
		if (Label->initWithTexture(pTex, fontname))
		{
			Label->mAwaysShowSign = ForceShowSign;
			return Label;
		}
		VDelete Label;
	}
	return NULL;
}

VNumberLabel* VNumberLabel::createFactor(const char* fontname)
{
	VNumberLabel* Label = create(fontname, false);
	if (Label)
	{
		Label->mAsFactor = true;
	}
	return Label;
}

VNumberLabel::VNumberLabel()
	:mSpriteSheet(nullptr)
	, mNumber(-1)
	, mAsFactor(false)
	, mAwaysShowSign(false)
{
	memset(mFrames, 0, sizeof(mFrames));
}


VNumberLabel::~VNumberLabel()
{
}

bool VNumberLabel::initWithTexture(cocos2d::Texture2D* Texture, const char* font_name)
{
	SpriteBatchNode::initWithTexture(Texture, 10);
	VASSERT(Texture);
	mSpriteSheet = Texture;
	SpriteFrameCache* Cache = SpriteFrameCache::getInstance();
	const char* NumberName[N_MAX] = {
		"0.png",
		"1.png",
		"2.png",
		"3.png",
		"4.png",
		"5.png",
		"6.png",
		"7.png",
		"8.png",
		"9.png",
		"add.png",
		"sub.png",
		"mul.png"
	};
	char number_name[256];
	strcpy(number_name, font_name);
	int len = strlen(number_name);
	if (number_name[len - 1] != '/')
	{
		number_name[len++] = '/';
		number_name[len] = 0;
	}
	char* pstr = &number_name[len];
	for (int Index = N0; Index < N_MAX; ++Index)
	{
		strcpy(pstr, NumberName[Index]);
		mFrames[Index] = Cache->getSpriteFrameByName(number_name);
	}

	autorelease();
	return true;
}


void VNumberLabel::UpdateNumber()
{
	this->removeAllChildren();

	Sprite* Sprites[65];
	int NumSprites = 0;
	int Number = VAbs(mNumber);
	if (Number == 0)
	{
		Number = 0;
		Sprites[NumSprites++] = GetSprite((VNumberLabel::Number)0);
	}
	else
	{
		do
		{
			int N = Number % 10;
			Number = Number / 10;
			Sprites[NumSprites++] = GetSprite((VNumberLabel::Number)N);
		} while (Number);
	}
	if (mAsFactor)
	{
		Sprites[NumSprites] = GetSprite(N_MUL);
		if (Sprites[NumSprites])
		{
			NumSprites++;
		}
	}
	else if (mAwaysShowSign)
	{
		VNumberLabel::Number Sign = mNumber < 0 ? N_SUB : N_ADD;
		Sprites[NumSprites++] = GetSprite((VNumberLabel::Number)Sign);
	}
	else if (mNumber < 0)
	{
		Sprites[NumSprites++] = GetSprite(N_SUB);
	}
	
	if (NumSprites == 0)
	{
		return;
	}

	// align sprites.
	Size content_size(0.0f,0.0f);
	for (int Index = 0; Index < NumSprites; ++Index)
	{
		const Size& ssize = Sprites[Index]->getContentSize();
		content_size.width += ssize.width;
		if (ssize.height > content_size.height)
		{
			content_size.height = ssize.height;
		}
	}
	Vec2 pos;
	pos.x = 0.0f;
	pos.y = content_size.height * 0.5f;
	float Offset;
	Sprite* spr;
	for (int Index = NumSprites - 1; Index >= 0; --Index)
	{
		spr = Sprites[Index];
		Offset = spr->getContentSize().width * 0.5f;
		pos.x += Offset;
		spr->setPosition(pos);
		addChild(spr);
		pos.x += Offset;
	}
	setContentSize(content_size);
}

cocos2d::Sprite* VNumberLabel::GetSprite(Number n)
{
	if (mFrames[n])
	{
		Sprite* sn = Sprite::createWithSpriteFrame(mFrames[n]);
		return sn;
	}
	return NULL;
}

void VNumberLabel::SetNumber(int Number)
{
	if (mNumber != Number)
	{
		mNumber = Number;
		UpdateNumber();
	}
}

