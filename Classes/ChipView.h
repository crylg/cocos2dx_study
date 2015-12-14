#pragma once
//µ×²¿ÏÂ×¢
#include "cocos2d.h"
class ChipView:public VView
{
public:
	enum eTag
	{
		discard=77,
		follow=26,
		one=27,
		five=28,
		all_in=29,
		head=76
	};

	ChipView();
	~ChipView();
	CREATE_FUNC(ChipView);
	bool init();
private:
	void initRootViewAndrootOfRequestNode();
	void requestChildTag();
	cocos2d::Node* _rootView;
	cocos2d::Node* _rootOfRequestNode;
	cocos2d::ui::Button* _discardBtn;
	cocos2d::ui::Button* _followBtn;
	cocos2d::ui::Button* _oneBtn;
	cocos2d::ui::Button* _fiveBtn;
	cocos2d::ui::Button* _allInBtn;
};


