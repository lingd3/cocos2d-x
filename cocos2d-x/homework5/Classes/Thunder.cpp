#include "Thunder.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "SimpleAudioEngine.h"
#include <algorithm>

USING_NS_CC;

using namespace cocostudio::timeline;
using namespace CocosDenshion;

Scene* Thunder::createScene() {
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = Thunder::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

bool Thunder::init() {
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
	addTouchListener();

	schedule(schedule_selector(Thunder::update), 0.01, kRepeatForever, 0);

	return true;
}

void Thunder::preloadMusic() {
	auto audio = SimpleAudioEngine::getInstance();
	audio->preloadBackgroundMusic("./music/bgm.mp3");
	audio->preloadEffect("./music/fire.wav");
}

void Thunder::playBgm() {
	auto audio = SimpleAudioEngine::getInstance();
	audio->playBackgroundMusic("./music/bgm.mp3", true);
}

void Thunder::addCustomListener() {
	auto meetListener = EventListenerCustom::create("meet", CC_CALLBACK_1(Thunder::meet, this));
	_eventDispatcher->addEventListenerWithFixedPriority(meetListener, 1);
}

void Thunder::addTouchListener() {
	EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(Thunder::onTouchBegan, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	EventListenerTouchOneByOne* listener1 = EventListenerTouchOneByOne::create();
	listener1->setSwallowTouches(true);
	listener->onTouchMoved = CC_CALLBACK_2(Thunder::onTouchMoved, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener1, this);
}

void Thunder::meet(EventCustom* event) {
	int i = *(int*)event->getUserData();
	if (enemys[i] != NULL) removeChild(enemys[i]);
	enemys.erase(enemys.begin() + i);
}

void Thunder::addEnemy(int n) {
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

void Thunder::addKeyboardListener() {
	auto listener = EventListenerKeyboard::create();
	listener->onKeyPressed = CC_CALLBACK_2(Thunder::onKeyPressed, this);
	listener->onKeyReleased = CC_CALLBACK_2(Thunder::onKeyReleased, this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void Thunder::update(float f) {
	static double count = 0;
	static int dir = 1;
	static int down = 0;
	static int stop = 0;
	down++;
	count += f;
	if (count > 1 && stop == 0) {
		count = 0.0;
		dir = -dir;
		for (unsigned i = 0; i < enemys.size(); i++) {
			if (enemys[i] != NULL && down % 2 == 0) {
				enemys[i]->setPosition(enemys[i]->getPositionX(), enemys[i]->getPositionY() - 40);
				if (enemys[i]->getPositionY() - 40 < player->getPosition().y) {
					auto label = Label::createWithSystemFont("You lost!!!", "Arial", 80);
					Vec2 origin = Director::getInstance()->getVisibleOrigin();
					label->setPosition(Vec2(origin.x + visibleSize.width / 2,
						origin.y + visibleSize.height / 2));
					addChild(label);
					player->removeFromParentAndCleanup(true);
					player = NULL;
					for (unsigned k = 0; k < enemys.size(); k++) {
						enemys[k]->removeFromParentAndCleanup(true);
					}
					stop = 1;
					return;
				}
			}
		}
	}

	if (stop != 1) {

		if (enemys.size() == 0) {
			auto label = Label::createWithSystemFont("You win!!!", "Arial", 80);
			Vec2 origin = Director::getInstance()->getVisibleOrigin();
			label->setPosition(Vec2(origin.x + visibleSize.width / 2,
				origin.y + visibleSize.height / 2));
			addChild(label);
		}

		for (int i = 0; i < bullet.size(); i++) {
			if (bullet[i] != NULL) {
				bullet[i]->setPosition(bullet[i]->getPositionX(), bullet[i]->getPositionY() + 5);
				if (bullet[i]->getPositionY() > visibleSize.height - 10) {
					bullet[i]->removeFromParentAndCleanup(true);
					bullet[i] = NULL;
				}
			}
		}


		for (unsigned i = 0; i < enemys.size(); i++) {
			if (enemys[i] != NULL) {
				enemys[i]->setPosition(enemys[i]->getPosition() + Vec2(dir, 0));
				for (int j = 0; j < bullet.size(); j++) {
					if (bullet[j] != NULL && bullet[j]->getPosition().getDistance(enemys[i]->getPosition()) < 30) {
						bullet[j]->removeFromParentAndCleanup(true);
						bullet.erase(bullet.begin() + j);
						auto audio = SimpleAudioEngine::getInstance();
						audio->playEffect("music/explore.wav", false, 1.0f, 1.0f, 1.0f);
						EventCustom e("meet");
						e.setUserData(&i);
						_eventDispatcher->dispatchEvent(&e);
						break;
					}
				}
			}
		}

		if (player == NULL || player->getPositionX() + move > visibleSize.width - 10 || player->getPositionX() + move <= 10) return;
		player->setPosition(player->getPosition() + Vec2(move, 0));
	}
}

void Thunder::fire() {
	bullet.push_back(Sprite::create("bullet.png"));
	bullet[bullet.size() - 1]->setPosition(player->getPosition());
	addChild(bullet[bullet.size() - 1]);
	auto audio = SimpleAudioEngine::getInstance();
	audio->playEffect("fire.wav", false, 1.0f, 1.0f, 1.0f);
}

float move = 0.0;
void Thunder::onKeyPressed(EventKeyboard::KeyCode code, Event * event)
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

void Thunder::onKeyReleased(EventKeyboard::KeyCode code, Event * event)
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
		break;

	default:
		break;
	}
}

bool Thunder::onTouchBegan(Touch *touch, Event *unused_event) {
	fire();
	return true;
}

void Thunder::onTouchMoved(Touch * touch, Event * unused_event)
{
	player->setPosition(touch->getLocation().x, player->getPositionY());
}

void Thunder::onTouchEnded(Touch * touch, Event * unused_event)
{
}
