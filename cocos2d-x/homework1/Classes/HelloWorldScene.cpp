#include "HelloWorldScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "SimpleAudioEngine.h"
#include <algorithm>

USING_NS_CC;

using namespace cocostudio::timeline;
using namespace CocosDenshion;

Scene* HelloWorldScene::createScene() {
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = HelloWorldScene::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

bool HelloWorldScene::init() {
	if (!Layer::init()) {
		return false;
	}

	visibleSize = Director::getInstance()->getVisibleSize();

	auto bgsprite = Sprite::create("background.jpg");
	bgsprite->setPosition(visibleSize / 2);
	bgsprite->setScale(visibleSize.width / bgsprite->getContentSize().width, \
		visibleSize.height / bgsprite->getContentSize().height);
	this->addChild(bgsprite, 0);

	player = Sprite::create("player.png");
	player->setPosition(visibleSize.width / 2, player->getContentSize().height + 5);
	player->setName("player");
	this->addChild(player, 1);

	addEnemy(5);

	preloadMusic();
	playBgm();

	addKeyboardListener();
	addCustomListener();

	schedule(schedule_selector(Thunder::update), 0.01, kRepeatForever, 0);

	return true;
}

void HelloWorldScene::preloadMusic() {
	auto audio = SimpleAudioEngine::getInstance();
	audio->preloadBackgroundMusic("music/bgm.mp3");
	audio->preloadEffect("music/fire.wav");
}

void HelloWorldScene::playBgm() {
	auto audio = SimpleAudioEngine::getInstance();
	audio->playBackgroundMusic("music/bgm.mp3", true);
}

void HelloWorldScene::addCustomListener() {
	auto meetListener = EventListenerCustom::create("meet", CC_CALLBACK_1(Thunder::meet, this));
	_eventDispatcher->addEventListenerWithFixedPriority(meetListener, 1);
}

void HelloWorldScene::meet(EventCustom* event) {
	int i = *(int*)event->getUserData();

	if (bullet != NULL) removeChild(bullet);
	bullet = NULL;

	if (enemys[i] != NULL) removeChild(enemys[i]);
	enemys[i] = NULL;
}

void HelloWorldScene::addEnemy(int n) {
	enemys.resize(n * 3);
	for (unsigned i = 0; i < 3; ++i) {
		char enemyPath[20];
		sprintf(enemyPath, "stone%d.png", 3 - i);
		double width = (visibleSize.width - 20) / (n + 1.0),
			height = visibleSize.height - (50 * (i + 1));
		for (int j = 0; j < n; ++j) {
			auto enemy = Sprite::create(enemyPath);
			enemy->setAnchorPoint(Vec2(0.5, 0.5));
			enemy->setScale(0.5, 0.5);
			enemy->setPosition(width * (j + 1), height);
			enemys[i * n + j] = enemy;
			addChild(enemy);
		}
	}
}

void HelloWorldScene::addKeyboardListener() {
	auto listener = EventListenerKeyboard::create();
	listener->onKeyPressed = CC_CALLBACK_2(Thunder::onKeyPressed, this);
	listener->onKeyReleased = CC_CALLBACK_2(Thunder::onKeyReleased, this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void HelloWorldScene::update(float f) {
	static double count = 0;
	static int dir = 1;
	count += f;
	if (count > 1) { count = 0.0; dir = -dir; }

	if (bullet != NULL) {
		bullet->setPosition(bullet->getPositionX(), bullet->getPositionY() + 5);
		if (bullet->getPositionY() > visibleSize.height - 10) {
			bullet->removeFromParentAndCleanup(true);
			bullet = NULL;
		}
	}

	for (unsigned i = 0; i < enemys.size(); i++) {
		if (enemys[i] != NULL) {
			enemys[i]->setPosition(enemys[i]->getPosition() + Vec2(dir, 0));
			if (bullet != NULL && bullet->getPosition().getDistance(enemys[i]->getPosition()) < 30) {
				auto audio = SimpleAudioEngine::getInstance();
				audio->playEffect("music/explore.wav", false, 1.0f, 1.0f, 1.0f);
				EventCustom e("meet");
				e.setUserData(&i);
				_eventDispatcher->dispatchEvent(&e);
			}
		}
	}

	if (player->getPositionX() + move > visibleSize.width - 10 || player->getPositionX() + move <= 10) return;
	player->setPosition(player->getPosition() + Vec2(move, 0));
}

void HelloWorldScene::fire() {
	if (bullet != NULL) return;
	bullet = Sprite::create("bullet.png");
	bullet->setPosition(player->getPosition());
	addChild(bullet);
	auto audio = SimpleAudioEngine::getInstance();
	audio->playEffect("fire.wav", false, 1.0f, 1.0f, 1.0f);
}

float move = 0.0;
void HelloWorldScene::onKeyPressed(EventKeyboard::KeyCode code, Event * event)
{
	switch (code)
	{
	case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	case cocos2d::EventKeyboard::KeyCode::KEY_A:
		if (player->getPositionX() >= 15) player->setPosition(player->getPositionX() - 1, player->getPositionY());
		move -= 5;
		break;

	case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
	case cocos2d::EventKeyboard::KeyCode::KEY_D:
		if (player->getPositionX() < visibleSize.width - 20) player->setPosition(player->getPositionX() + 1, player->getPositionY());
		move += 5;
		break;

	case cocos2d::EventKeyboard::KeyCode::KEY_SPACE:
		fire();
		break;

	default:
		break;
	}
}

void HelloWorldScene::onKeyReleased(EventKeyboard::KeyCode code, Event * event)
{
	switch (code)
	{
	case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	case cocos2d::EventKeyboard::KeyCode::KEY_A:
		if (player->getPositionX() >= 15) player->setPosition(player->getPositionX() - 1, player->getPositionY());
		move += 5;
		break;

	case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
	case cocos2d::EventKeyboard::KeyCode::KEY_D:
		if (player->getPositionX() < visibleSize.width - 20) player->setPosition(player->getPositionX() + 1, player->getPositionY());
		move -= 5;
		break;

	case cocos2d::EventKeyboard::KeyCode::KEY_SPACE:
		fire();
		break;

	default:
		break;
	}
}