#include "BreakoutScene.h"
#include <string>
using namespace std;
#include "SimpleAudioEngine.h"

using namespace CocosDenshion;

void Breakout::setPhysicsWorld(PhysicsWorld* world) { m_world = world; }

Scene* Breakout::createScene() {
	auto scene = Scene::createWithPhysics();
	//scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	scene->getPhysicsWorld()->setGravity(Point(0, 0));

	auto layer = Breakout::create(scene->getPhysicsWorld());

	scene->addChild(layer);
	return scene;
}

bool Breakout::init(PhysicsWorld* world) {
	if (!Layer::init()) {
		return false;
	}
	
	t = 0; //用于判断游戏是否结束

	this->setPhysicsWorld(world);
	visibleSize = Director::getInstance()->getVisibleSize();

	preloadMusic();
	playBgm();

	addBackground();
	addEdge();
	addPlayer();

	addContactListener();
	addTouchListener();
	addKeyboardListener();

	this->schedule(schedule_selector(Breakout::update), 1);


	TTFConfig ttfConfig;
	ttfConfig.fontFilePath = "./fonts/arial.ttf";
	ttfConfig.fontSize = 36;

	time = Label::createWithTTF(ttfConfig, "30");
	dtime = 30;
	schedule(schedule_selector(Breakout::updateTime), 1.0f, kRepeatForever, 0);
	time->setPosition(Vec2(origin.x + visibleSize.width*3/4,
		origin.y + visibleSize.height-20));
	addChild(time, 1);


	Sprite* sp0 = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 320, 420, 47)));
	Sprite* sp = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(610, 362, 4, 16)));

	pT = ProgressTimer::create(sp);
	pT->setScaleX(90);
	pT->setAnchorPoint(Vec2(0, 0));
	pT->setType(ProgressTimerType::BAR);
	pT->setBarChangeRate(Point(1, 0));
	pT->setMidpoint(Point(0, 1));
	pT->setPercentage(100);
	pT->setPosition(Vec2(origin.x + 14 * pT->getContentSize().width+80, origin.y + visibleSize.height - 2 * pT->getContentSize().height));
	addChild(pT, 2);
	sp0->setAnchorPoint(Vec2(0, 0));
	sp0->setPosition(Vec2(origin.x + pT->getContentSize().width+80, origin.y + visibleSize.height - sp0->getContentSize().height));
	addChild(sp0, 1);

	return true;
}

Breakout * Breakout::create(PhysicsWorld * world) {
	Breakout* pRet = new(std::nothrow) Breakout();
	if (pRet && pRet->init(world)) {
		pRet->autorelease();
		return pRet;
	}
	delete pRet;
	pRet = NULL;
	return NULL;
}

void Breakout::preloadMusic() {
	SimpleAudioEngine::getInstance()->preloadBackgroundMusic("./music/bgm.mp3");
	SimpleAudioEngine::getInstance()->preloadEffect("./music/meet_stone.wav");
}

void Breakout::playBgm() {
	SimpleAudioEngine::getInstance()->playBackgroundMusic("./music/bgm.mp3", true);
}

void Breakout::addBackground() {
	auto bgsprite = Sprite::create("black_hole_bg0.jpg");
	bgsprite->setPosition(visibleSize / 2);
	bgsprite->setScale(visibleSize.width / bgsprite->getContentSize().width, visibleSize.height / bgsprite->getContentSize().height);
	this->addChild(bgsprite, 0);

	auto ps = ParticleSystemQuad::create("black_hole.plist");
	ps->setPosition(visibleSize / 2);
	this->addChild(ps);
}

void Breakout::addEdge() {
	auto edgeSp = Sprite::create();
	auto boundBody = PhysicsBody::createEdgeBox(visibleSize);
	boundBody->setDynamic(false);
	boundBody->setTag(0);
	edgeSp->setPosition(Point(visibleSize.width / 2, visibleSize.height / 2));
	edgeSp->setPhysicsBody(boundBody);
	this->addChild(edgeSp);
}

void Breakout::addPlayer() {
	player = Sprite::create("player.png");
	player->setAnchorPoint(Vec2(0.5, 0.5));
	player->setPhysicsBody(PhysicsBody::createCircle(25));
	
	player->getPhysicsBody()->setCategoryBitmask(0x01);
	player->getPhysicsBody()->setCollisionBitmask(0x02);
	player->getPhysicsBody()->setContactTestBitmask(0x02);

	player->setPosition(visibleSize / 2);
	player->getPhysicsBody()->setTag(1);
	player->getPhysicsBody()->setAngularVelocityLimit(0);
	addChild(player);
}

void Breakout::addContactListener() {
	auto touchListener = EventListenerPhysicsContact::create();
	touchListener->onContactBegin = CC_CALLBACK_1(Breakout::onConcactBegan, this);
	_eventDispatcher->addEventListenerWithFixedPriority(touchListener, 1);
}

void Breakout::addTouchListener() {
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(Breakout::onTouchBegan, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(Breakout::onTouchMoved, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(Breakout::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
}

void Breakout::addKeyboardListener() {
	auto keboardListener = EventListenerKeyboard::create();
	keboardListener->onKeyPressed = CC_CALLBACK_2(Breakout::onKeyPressed, this);
	keboardListener->onKeyReleased = CC_CALLBACK_2(Breakout::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keboardListener, this);
}

bool Breakout::onTouchBegan(Touch *touch, Event *unused_event) {
	Vec2 postion = touch->getLocation();
	// do something
	//
	return true;
}

void Breakout::onTouchMoved(Touch *touch, Event *unused_event) {
	// TODO
	//
}

void Breakout::onTouchEnded(Touch *touch, Event *unused_event) {
	// TODO
	//
}

bool Breakout::onConcactBegan(PhysicsContact& contact) {
	if (t == 1) return false;
	auto body1 = contact.getShapeA()->getBody();
	auto body2 = contact.getShapeB()->getBody();
	auto sp1 = (Sprite*)body1->getNode();
	auto sp2 = (Sprite*)body2->getNode();

	if (sp1 == player) {
		auto ps = ParticleSystemQuad::create("explode.plist");
		ps->setPosition(sp2->getPosition());
		this->addChild(ps);
		auto audio = SimpleAudioEngine::getInstance();
		audio->playEffect("./music/meet_stone.wav", false, 1.0f, 1.0f, 1.0f);
		CCProgressFromTo *to = CCProgressFromTo::create(1, pT->getPercentage(), pT->getPercentage() - 20);
		pT->runAction(to);
		sp2->removeFromParentAndCleanup(true);
	}
	else if (sp2 == player) {
		auto ps = ParticleSystemQuad::create("explode.plist");
		ps->setPosition(sp1->getPosition());
		this->addChild(ps);
		auto audio = SimpleAudioEngine::getInstance();
		audio->playEffect("./music/meet_stone.wav", false, 1.0f, 1.0f, 1.0f);
		CCProgressFromTo *to = CCProgressFromTo::create(1, pT->getPercentage(), pT->getPercentage() - 20);
		pT->runAction(to);
		sp1->removeFromParentAndCleanup(true);
	}
	if (pT->getPercentage() <= 0) {
		int sum = enemys.size();
		for (int i = 0; i < sum; i++) {
			removeChild(enemys.back()->getOwner());
			enemys.popBack();
		}

		TTFConfig ttfConfig;
		ttfConfig.fontFilePath = "./fonts/arial.ttf";
		ttfConfig.fontSize = 36;
		auto label = Label::createWithTTF(ttfConfig, "GAME OVER!");
		label->setPosition(Vec2(origin.x + visibleSize.width /2,
			origin.y + visibleSize.height/2));
		addChild(label, 1);

		t = 1;

	}
	return true;
}

void Breakout::update(float f) {
	if (t == 1) return;
	newEnemys();
}

void Breakout::newEnemys() {
	if (enemys.size() > 50) return;
	int newNum = 2;
	while (newNum--) {
		int type = 0;
		if (CCRANDOM_0_1() > 0.85) { type = 2; }
		else if (CCRANDOM_0_1() > 0.6) { type = 1; }

		Point location = Vec2(0, 0);
		switch (rand() % 4)
		{
		case 0:
			location.y = visibleSize.height;
			location.x = rand() % (int)(visibleSize.width);
			break;
		case 1:
			location.x = visibleSize.width;
			location.y = rand() % (int)(visibleSize.height);
			break;
		case 2:
			location.y = 0;
			location.x = rand() % (int)(visibleSize.width);
			break;
		case 3:
			location.x = 0;
			location.y = rand() % (int)(visibleSize.height);
			break;
		default:
			break;
		}
		addEnemy(type, location);
	}
}

void Breakout::addEnemy(int type, Point p) {
	char path[100];
	int tag;
	switch (type)
	{
	case 0:
		sprintf(path, "stone1.png");
		tag = 10;
		break;
	case 1:
		sprintf(path, "stone2.png");
		tag = 20;
		break;
	case 2:
		sprintf(path, "stone3.png");
		tag = 30;
		break;
	default:
		sprintf(path, "stone1.png");
		tag = 10;
		break;
	}
	auto re = Sprite::create(path);
	re->setPhysicsBody(PhysicsBody::createCircle(re->getContentSize().height / 2));
	re->setAnchorPoint(Vec2(0.5, 0.5));
	re->setScale(0.5, 0.5);
	re->setPosition(p);
	// TODO set bitmask
	re->getPhysicsBody()->setCategoryBitmask(0x02); 
	re->getPhysicsBody()->setCollisionBitmask(0x01); 
	re->getPhysicsBody()->setContactTestBitmask(0x01);

	re->getPhysicsBody()->setTag(tag);
	if (rand() % 100 < 50) {
		re->getPhysicsBody()->setVelocity((player->getPosition() - p) * (0.25));
	}
	else {
		re->getPhysicsBody()->setVelocity((Point(rand() % (int)(visibleSize.width - 100) + 50, rand() % (int)(visibleSize.height - 100) + 50) - p) * (0.25));
	}
	re->getPhysicsBody()->setAngularVelocity(CCRANDOM_0_1() * 10);
	enemys.pushBack(re->getPhysicsBody());
	addChild(re);
}

void Breakout::updateTime(float dt)
{
	if (t == 1) {
		removeChild(time);
		return;
	}
	dtime--;
	char str[5] = { '0' };
	sprintf(str, "%d", dtime);
	time->setString(str);

	if (dtime <= 0) {
		int sum = enemys.size();
		for (int i = 0; i < sum; i++) {
			removeChild(enemys.back()->getOwner());
			enemys.popBack();
		}
		TTFConfig ttfConfig;
		ttfConfig.fontFilePath = "./fonts/arial.ttf";
		ttfConfig.fontSize = 36;
		auto label = Label::createWithTTF(ttfConfig, "YOU MIN!");
		label->setPosition(Vec2(origin.x + visibleSize.width / 2,
			origin.y + visibleSize.height / 2));
		addChild(label, 1);

		t = 1;
	}
}

void Breakout::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
	switch (code)
	{
	case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	case cocos2d::EventKeyboard::KeyCode::KEY_A:
		player->getPhysicsBody()->setVelocity(Point(-200, player->getPhysicsBody()->getVelocity().y));
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
	case cocos2d::EventKeyboard::KeyCode::KEY_D:
		player->getPhysicsBody()->setVelocity(Point(200, player->getPhysicsBody()->getVelocity().y));
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
	case cocos2d::EventKeyboard::KeyCode::KEY_W:
		player->getPhysicsBody()->setVelocity(Point(player->getPhysicsBody()->getVelocity().x, 200));
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW:
	case cocos2d::EventKeyboard::KeyCode::KEY_S:
		player->getPhysicsBody()->setVelocity(Point(player->getPhysicsBody()->getVelocity().x, -200));
		break;
	default:
		break;
	}
}

void Breakout::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
	switch (code)
	{
	case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	case cocos2d::EventKeyboard::KeyCode::KEY_A:
		player->getPhysicsBody()->setVelocity(player->getPhysicsBody()->getVelocity() - Point(-200, 0));
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
	case cocos2d::EventKeyboard::KeyCode::KEY_D:
		player->getPhysicsBody()->setVelocity(player->getPhysicsBody()->getVelocity() - Point(200, 0));
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
	case cocos2d::EventKeyboard::KeyCode::KEY_W:
		player->getPhysicsBody()->setVelocity(player->getPhysicsBody()->getVelocity() - Point(0, 200));
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW:
	case cocos2d::EventKeyboard::KeyCode::KEY_S:
		player->getPhysicsBody()->setVelocity(player->getPhysicsBody()->getVelocity() - Point(0, -200));
		break;
	default:
		break;
	}
}
