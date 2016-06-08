#include "GameOver.h"
#include "HelloWorldScene.h"
#include "Global.h"
USING_NS_CC;

Scene* GameOver::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = GameOver::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool GameOver::init()
{

	if (!Layer::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	//Ìí¼Ó±³¾°
	auto background = Sprite::create("map/end_bg.jpg");
	background->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	addChild(background, 0);

	string a = Global::getWinner();
	a = a + " Win";
	auto field = Label::create(a, "fonts/pan.ttf", 70);
	field->setColor(ccc3(51, 63, 80));
	field->setPosition(Size(visibleSize.width / 2, visibleSize.height / 4 * 3 - 20));
	this->addChild(field, 2);

	//restart
	auto start_button = Button::create();
	start_button->setTitleText("Restart");
	start_button->setTitleFontSize(60);
	start_button->setTitleFontName("fonts/pan.ttf");
	start_button->setColor(ccc3(132, 60, 12));
	start_button->setPosition(Size(visibleSize.width / 2, visibleSize.height / 4 - 20));
	start_button->addClickEventListener(CC_CALLBACK_1(GameOver::menuChangeCallback, this));
	this->addChild(start_button, 2);
	
	return true;
}

void GameOver::menuChangeCallback(Ref* pSender)
{
	CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5, HelloWorld::createScene()));

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}