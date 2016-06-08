#pragma once
#ifndef __MENU_SEBCE_H__
#define __MENU_SEBCE_H__

#include "cocos2d.h"
#include "network/HttpClient.h"
#include "ui/CocosGUI.h"
#include <string>
#include "Global.h"
using namespace cocos2d::ui;
using namespace cocos2d::network;

class MenuSence : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	void menuChangeCallback(Ref * pSender);

	// a selector callback
	//void startMenuCallback(cocos2d::Ref* pSender);
	void getNameListener(Ref *pSender);
	void onHttpComplete(HttpClient *sender, HttpResponse *response);

	TextField * name_field;
	// implement the "static create()" method manually
	CREATE_FUNC(MenuSence);
};

#endif // __MENU_SEBCE_H__