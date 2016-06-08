#include "GameSence.h"
USING_NS_CC;

Scene* GameSence::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = GameSence::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}


// on "init" you need to initialize your instance
bool GameSence::init()
{

	if (!Layer::init())
	{
		return false;
	}

	//add touch listener
	EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(GameSence::onTouchBegan, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	GameSence::stoneLayer = LayerColor::create();
	stoneLayer->setPosition(Point(0, 0));
	addChild(stoneLayer, 0);

	auto background = Sprite::create("level-background-0.jpg");
	background->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y ));
	stoneLayer->addChild(background, 0);

	GameSence::stone = Sprite::create("stone.png");
	stone->setPosition(Point(560, 480));
	stoneLayer->addChild(stone, 1);

	auto label = MenuItemImage::create(
		"shoot.png",
		"shoot.png",
		CC_CALLBACK_1(GameSence::menu, this));
	label->setPosition(Point(760, 480));

	auto item = Menu::create(label, NULL);
	item->setPosition(Vec2::ZERO);
	stoneLayer->addChild(item, 1);

	GameSence::mouseLayer = LayerColor::create();
	mouseLayer->setPosition(Point(0, visibleSize.height / 2));
	addChild(mouseLayer, 1);

	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("level-sheet.plist");
	GameSence::mouse = Sprite::createWithSpriteFrameName("gem-mouse-4.png");
	mouse->setPosition(Point(visibleSize.width / 2, 0));
	mouseLayer->addChild(mouse, 1);

	return true;
}

bool GameSence::onTouchBegan(Touch *touch, Event *unused_event) {
	Size visibleSize = Director::getInstance()->getVisibleSize();
	auto location = touch->getLocation();

	auto cheese = Sprite::create("cheese.png");
	cheese->setPosition(location);
	this->addChild(cheese, 1);
	cheese->runAction(FadeOut::create(4));

	auto moveTo = MoveTo::create(1, Point(location.x, location.y - visibleSize.height / 2));
	auto actions = Sequence::create(moveTo, NULL);
	mouse->runAction(actions);

	return true;
}

void GameSence::menu(Ref* pSender)
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	auto stone_ = Sprite::create("stone.png");
	stone_->setPosition(Point(560, 480));
	stoneLayer->addChild(stone_, 1);
	stone_->runAction(FadeOut::create(3));

	auto moveTo = MoveTo::create(1, Point(mouse->getPosition().x, mouse->getPosition().y + visibleSize.height / 2));
	auto actions = Sequence::create(moveTo, NULL);
	stone_->runAction(actions);

	auto diamond = Sprite::create("diamond.png");
	diamond->setPosition(Point(mouse->getPosition().x, mouse->getPosition().y + visibleSize.height / 2));
	stoneLayer->addChild(diamond, 1);

	auto moveTo1 = MoveTo::create(1, Point(rand()%200, rand()%200));
	auto actions1 = Sequence::create(moveTo1, NULL);
	mouse->runAction(actions1);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}