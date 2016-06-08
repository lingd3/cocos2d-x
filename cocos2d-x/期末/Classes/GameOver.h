#pragma once

#include "cocos2d.h"
#include "network/HttpClient.h"
#include "ui/CocosGUI.h"
#include <string>
#include "Global.h"
using namespace cocos2d::ui;
using namespace cocos2d::network;

class GameOver : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	void menuChangeCallback(Ref* pSender);

	// implement the "static create()" method manually
	CREATE_FUNC(GameOver);
};
