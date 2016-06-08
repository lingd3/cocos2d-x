#include "HelloWorldScene.h"
#include "Monster.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

#define database UserDefault::getInstance()
using namespace cocostudio::timeline;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
	if (!database->getIntegerForKey("value")) {
		database->setIntegerForKey("value", 0);
	}

	int value = database->getIntegerForKey("value");

    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	//����һ����ͼ
	auto texture = Director::getInstance()->getTextureCache()->addImage("$lucia_2.png");
	//����ͼ�������ص�λ�и�����ؼ�֡
	auto frame0 = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 113, 113)));
	//ʹ�õ�һ֡��������
	player = Sprite::createWithSpriteFrame(frame0);
	player->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height / 2));
	addChild(player, 4);


	TTFConfig ttfConfig;
	ttfConfig.fontFilePath = "fonts/arial.ttf";
	ttfConfig.fontSize = 36;

	char str[5] = {'0'};
	sprintf(str, "%d", value);

	//����ʱ
	time = Label::createWithTTF(ttfConfig, str);
	//����ʱ�����Ե��õ�����
	schedule(schedule_selector(HelloWorld::updateTime), 1.0f, kRepeatForever, 0);
	//����ʱ������
	dtime = 1800;
	time->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - time->getContentSize().height));
	addChild(time, 1);

	//hp��
	Sprite* sp0 = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 320, 420, 47)));
	Sprite* sp = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(610, 362, 4, 16)));

	//ʹ��hp������progressBar
	pT = ProgressTimer::create(sp);
	pT->setScaleX(90);
	pT->setAnchorPoint(Vec2(0, 0));
	pT->setType(ProgressTimerType::BAR);
	pT->setBarChangeRate(Point(1, 0));
	pT->setMidpoint(Point(0, 1));
	pT->setPercentage(100);
	pT->setPosition(Vec2(origin.x + 14 * pT->getContentSize().width, origin.y + visibleSize.height - 2 * pT->getContentSize().height));
	addChild(pT, 2);
	sp0->setAnchorPoint(Vec2(0, 0));
	sp0->setPosition(Vec2(origin.x + pT->getContentSize().width, origin.y + visibleSize.height - sp0->getContentSize().height));
	addChild(sp0, 1);

	//��̬����
	idle.reserve(1);
	idle.pushBack(frame0);

	//��������
	attack.reserve(17);
	for (int i = 0; i < 17; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(113 * i, 0, 113, 113)));
		attack.pushBack(frame);
	}
	attack.pushBack(frame0);

	//��������
	auto texture2 = Director::getInstance()->getTextureCache()->addImage("$lucia_dead.png");
	dead.reserve(22);
	for (int i = 0; i < 22; i++) {
		auto frame = SpriteFrame::createWithTexture(texture2, CC_RECT_PIXELS_TO_POINTS(Rect(79 * i, 0, 79, 90)));
		dead.pushBack(frame);
	}
	dead.pushBack(frame0);

	//�˶�����
	auto texture3 = Director::getInstance()->getTextureCache()->addImage("$lucia_forward.png");
	for (int i = 0; i < 8; i++) {
		auto frame = SpriteFrame::createWithTexture(texture3, CC_RECT_PIXELS_TO_POINTS(Rect(68 * i, 0, 68, 101)));
		run.pushBack(frame);
	}

	//Label
	auto menuLabel1 = Label::createWithTTF(ttfConfig, "W");
	auto menuLabel2 = Label::createWithTTF(ttfConfig, "S");
	auto menuLabel3 = Label::createWithTTF(ttfConfig, "A");
	auto menuLabel4 = Label::createWithTTF(ttfConfig, "D");
	auto menuLabel6 = Label::createWithTTF(ttfConfig, "Y");
	//menuItem
	auto item1 = MenuItemLabel::create(menuLabel1, CC_CALLBACK_1(HelloWorld::moveEvent, this, 'W'));
	auto item2 = MenuItemLabel::create(menuLabel2, CC_CALLBACK_1(HelloWorld::moveEvent, this, 'S'));
	auto item3 = MenuItemLabel::create(menuLabel3, CC_CALLBACK_1(HelloWorld::moveEvent, this, 'A'));
	auto item4 = MenuItemLabel::create(menuLabel4, CC_CALLBACK_1(HelloWorld::moveEvent, this, 'D'));
	auto item6 = MenuItemLabel::create(menuLabel6, CC_CALLBACK_1(HelloWorld::actionEvent, this, 'Y'));

	item3->setPosition(Vec2(origin.x + item3->getContentSize().width, origin.y + item3->getContentSize().height));
	item4->setPosition(Vec2(item3->getPosition().x + 3 * item4->getContentSize().width, item3->getPosition().y));
	item2->setPosition(Vec2(item3->getPosition().x + 1.5*item2->getContentSize().width, item3->getPosition().y));
	item1->setPosition(Vec2(item2->getPosition().x, item2->getPosition().y + item1->getContentSize().height));
	item6->setPosition(Vec2(origin.x + visibleSize.width - item6->getContentSize().width, item3->getPosition().y));

	auto menu = Menu::create(item1, item2, item3, item4, item6, NULL);
	menu->setPosition(Vec2(0, 0));
	addChild(menu, 1);

	TMXTiledMap* tmx = TMXTiledMap::create("bg.tmx");
	tmx->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	tmx->setAnchorPoint(Vec2(0.5, 0.5));
	tmx->setScale(Director::getInstance()->getContentScaleFactor());
	addChild(tmx, 0);

	return true;
}

char lastCid;
void HelloWorld::moveEvent(Ref*, char cid) {
	Size visibleSize = Director::getInstance()->getVisibleSize();
	switch (cid) {
		case 'W': {
			float a = player->getPosition().y+70 >= visibleSize.height ? visibleSize.height - 30: player->getPosition().y + 30;
			auto moveTo = MoveTo::create(0.1f, Point(player->getPosition().x, a));
			auto actions = Sequence::create(moveTo, NULL);
			player->runAction(actions);
			auto animation = Animation::createWithSpriteFrames(run, 0.03f);
			auto animate = Animate::create(animation);
			player->runAction(animate);
			break;
		}
		case 'A': {
			float b = player->getPosition().x -20 <= 0 ? 10 : player->getPosition().x - 30;
			if (lastCid != 'A') player->setFlipX(true);
			lastCid = 'A';
			auto moveTo1 = MoveTo::create(0.1f, Point(b, player->getPosition().y));
			auto actions1 = Sequence::create(moveTo1, NULL);
			player->runAction(actions1);
			auto animation1 = Animation::createWithSpriteFrames(run, 0.03f);
			auto animate1 = Animate::create(animation1);
			player->runAction(animate1);
			break;
		}
		case 'S': {
			float c = player->getPosition().y-40 <= 0 ? 30 : player->getPosition().y - 30;
			auto moveTo2 = MoveTo::create(0.1f, Point(player->getPosition().x, c));
			auto actions2 = Sequence::create(moveTo2, NULL);
			player->runAction(actions2);
			auto animation2 = Animation::createWithSpriteFrames(run, 0.03f);
			auto animate2 = Animate::create(animation2);
			player->runAction(animate2);
			break;
		}
		case 'D': {
			float d = player->getPosition().x + 30 >= visibleSize.width ? visibleSize.width-10 : player->getPosition().x + 30;
			if (lastCid != 'D') player->setFlipX(false);
			lastCid = 'D';
			auto moveTo3 = MoveTo::create(0.1f, Point(d, player->getPosition().y));
			auto actions3 = Sequence::create(moveTo3, NULL);
			player->runAction(actions3);
			auto animation3 = Animation::createWithSpriteFrames(run, 0.03f);
			auto animate3 = Animate::create(animation3);
			player->runAction(animate3);
			break; 
		}
	}
}


void HelloWorld::actionEvent(Ref*, char cid) {
	switch (cid) {
		case 'X': {
			auto animation = Animation::createWithSpriteFrames(dead, 0.1f);
			auto animate = Animate::create(animation);
			player->runAction(animate);
			CCProgressFromTo *to = CCProgressFromTo::create(1, pT->getPercentage(), pT->getPercentage() - 20);
			pT->runAction(to);
			break; 
		}
		case 'Y': {
			auto animation1 = Animation::createWithSpriteFrames(attack, 0.1f);
			auto animate1 = Animate::create(animation1);
			player->runAction(animate1);
			if (attackMonster()) {
				CCProgressFromTo *to1 = CCProgressFromTo::create(1, pT->getPercentage(), pT->getPercentage() + 20);
				pT->runAction(to1);
				database->setIntegerForKey("value", database->getIntegerForKey("value")+1);
				break; 
			}
		}
	}
}

void HelloWorld::stopAc() {
}

void HelloWorld::updateTime(float dt) {
	dtime = dtime - 1 >= 0 ? dtime-1 : 0;
	char str[5] = { '0' };
	sprintf(str, "%d", database->getIntegerForKey("value"));
	time->setString(str);
	Factory::getInstance()->moveMonster(player->getPosition(), 10);
	hitByMonster(0.1f);
	if (dtime % 8 == 0) {
		monster();
	}
}

void HelloWorld::monster() {
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	auto fac = Factory::getInstance();
	auto m = fac->createMonster();
	float x = random(origin.x, visibleSize.width);
	float y = random(origin.y, visibleSize.height);
	m->setPosition(x, y);
	addChild(m, 3);
}

void HelloWorld::hitByMonster(float dt) {
	auto fac = Factory::getInstance();
	Sprite* collision = fac->collider(player->getBoundingBox());
	if (collision != NULL) {
		fac->removeMonster(collision);
		actionEvent(this, 'X');
	}
}

bool HelloWorld::attackMonster() {
	auto fac = Factory::getInstance();
	Sprite* collision = fac->collider1(player->getBoundingBox());
	if (collision != NULL) {
		fac->removeMonster(collision);
		return true;
	}
	return false;
}