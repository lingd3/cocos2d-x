#include "HelloWorldScene.h"
#include "GameOver.h"
#include"Monster.h"
#include "cocostudio/CocoStudio.h"
#include "SimpleAudioEngine.h"
using namespace CocosDenshion;
#define database UserDefault::getInstance()

#pragma execution_character_set("utf-8")
USING_NS_CC;

void HelloWorld::setPhysicsWorld(PhysicsWorld* world) { m_world = world; }
Scene* HelloWorld::createScene()
{

	auto scene = Scene::createWithPhysics();
	//scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	scene->getPhysicsWorld()->setGravity(Point(0, 0));

	auto layer = HelloWorld::create(scene->getPhysicsWorld());

	scene->addChild(layer);
	return scene;
}
HelloWorld * HelloWorld::create(PhysicsWorld * world) {
	HelloWorld* pRet = new(std::nothrow) HelloWorld();
	if (pRet && pRet->init(world)) {
		pRet->autorelease();
		return pRet;
	}
	delete pRet;
	pRet = NULL;
	return NULL;
}
// on "init" you need to initialize your instance
bool HelloWorld::init(PhysicsWorld* world)
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}
	//
	this->setPhysicsWorld(world);

	//设置龟派气功数量
	fire_count = 2;
	fire_count2 = 2;

	visibleSize = Director::getInstance()->getVisibleSize();
	origin = Director::getInstance()->getVisibleOrigin();

	//导入地图
	loadMap();

	//导入音乐和音效
	preloadMusic();
	playBGM();

	TTFConfig ttfConfig;
	ttfConfig.fontFilePath = "fonts/arial.ttf";
	ttfConfig.fontSize = 36;

	//倒计时
	time = Label::createWithTTF(ttfConfig, "60");

	int attackNumber = 0;

	//添加精灵
	addPlayers();

	//倒计时周期性调用调度器
	schedule(schedule_selector(HelloWorld::updateTime), 1.0f, kRepeatForever, 0);
	//倒计时的数字
	dtime = 200;
	time->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - time->getContentSize().height));
	addChild(time);

	/*设置每隔个3秒产生一个怪物*/
	//倒计时周期性调用调度器
	//schedule(schedule_selector(HelloWorld::addMonster), 3.0f, kRepeatForever, 0);

	/*设置每隔1秒检测一下*/
	//倒计时周期性调用调度器
	schedule(schedule_selector(HelloWorld::hitByMonster), 0.05f, kRepeatForever, 0);

	//周期性更新人物移动
	schedule(schedule_selector(HelloWorld::continueKeyUpdate), 0.05f, kRepeatForever, 0);


	//hp条,红色
	Sprite* sp0 = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 320, 420, 47)));
	Sprite* sp = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(610, 362, 4, 16)));

	//使用hp条设置progressBar
	pT = ProgressTimer::create(sp);
	pT->setScaleX(90);
	pT->setAnchorPoint(Vec2(0, 0));
	pT->setType(ProgressTimerType::BAR);
	pT->setBarChangeRate(Point(1, 0));
	pT->setMidpoint(Point(0, 1));
	pT->setPercentage(100);
	pT->setPosition(Vec2(origin.x + 14 * pT->getContentSize().width, origin.y + visibleSize.height - 2 * pT->getContentSize().height));
	addChild(pT, 1);
	sp0->setAnchorPoint(Vec2(0, 0));
	sp0->setPosition(Vec2(origin.x + pT->getContentSize().width, origin.y + visibleSize.height - sp0->getContentSize().height));
	addChild(sp0, 0);

	/*--------------------------------------------*/
		//player2的 hp条
		//hp条
	Sprite* sp2 = Sprite::create("hp2.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 320, 420, 47)));
	Sprite* sp1 = Sprite::create("hp2.png", CC_RECT_PIXELS_TO_POINTS(Rect(610, 362, 4, 16)));

	//使用hp条设置progressBar
	pT2 = ProgressTimer::create(sp1);
	pT2->setScaleX(90);
	pT2->setAnchorPoint(Vec2(0, 0));
	pT2->setType(ProgressTimerType::BAR);
	pT2->setBarChangeRate(Point(1, 0));
	pT2->setMidpoint(Point(0, 1));
	pT2->setPercentage(100);
	pT2->setPosition(Vec2(origin.x + visibleSize.width - sp2->getContentSize().width + 35, origin.y + visibleSize.height - 2 * pT2->getContentSize().height));
	addChild(pT2, 1);
	sp2->setAnchorPoint(Vec2(0, 0));
	sp2->setPosition(Vec2(origin.x + visibleSize.width - sp2->getContentSize().width, origin.y + visibleSize.height - sp2->getContentSize().height));
	addChild(sp2, 0);
	/*---------------------------------------------------------------*/



	//添加事件监听器
	addKeyboardListener();
	//碰撞监听器
	addContactListener();
	return true;
}

//刚体边界
void HelloWorld::addEdge() {
	auto edgeSp = Sprite::create();
	auto boundBody = PhysicsBody::createEdgeBox(visibleSize);
	boundBody->setDynamic(false);
	boundBody->setTag(0);
	edgeSp->setPosition(Point(visibleSize.width / 2, visibleSize.height / 2));
	edgeSp->setPhysicsBody(boundBody);
	this->addChild(edgeSp);
}
void HelloWorld::newEnemys() {
	if (enemys.size() > 50) return;
	int newNum = 2;
	while (newNum--) {

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
		addEnemy(location);
	}
}
//添加随机炸弹
void HelloWorld::addEnemy(Point p) {
	//////////
	SpriteFrameCache* frameCache = SpriteFrameCache::sharedSpriteFrameCache();
	frameCache->addSpriteFramesWithFile("ex_stone.plist");

	Vector<SpriteFrame*> spVector;
	for (int i = 0; i < 4; i++) {
		SpriteFrame *m_frame = frameCache->spriteFrameByName(StringUtils::format("stone%d.png", i));
		spVector.pushBack(m_frame);
	}
	auto m_Animation = Animation::createWithSpriteFrames(spVector);
	m_Animation->setDelayPerUnit(0.5);
	auto ani = Animate::create(m_Animation);
	auto re = Sprite::create();
	re->setTag(2);
	re->runAction(RepeatForever::create(ani));
	addChild(re, 3);
	
	//////////
	re = Sprite::create("stone2.png");
	re->setPhysicsBody(PhysicsBody::createCircle(re->getContentSize().height / 2));
	re->setAnchorPoint(Vec2(0.5, 0.5));
	re->setScale(0.5, 0.5);
	re->setPosition(p);

	//设置初速度
	re->getPhysicsBody()->setVelocity((Point(rand() % (int)(visibleSize.width - 100) + 50, rand() % (int)(visibleSize.height - 100) + 50) - p));

	//设置撞击掩码
	//0110
	re->getPhysicsBody()->setCategoryBitmask(0x06);
	re->getPhysicsBody()->setContactTestBitmask(0xffffff);

	re->getPhysicsBody()->setAngularVelocity(CCRANDOM_0_1() * 10);
	enemys.pushBack(re->getPhysicsBody());
	addChild(re);
}


void HelloWorld::preloadMusic() {
	auto audio = SimpleAudioEngine::getInstance();

	audio->preloadBackgroundMusic("music/playing_bgm.mp3");
	audio->preloadEffect("music/shoot.wav");
	audio->preloadEffect("music/boom.wav");
	audio->preloadEffect("music/beat.wav");
}

void HelloWorld::playBGM() {
	SimpleAudioEngine::getInstance()->playBackgroundMusic("music/playing_bgm.mp3", true);
}

void HelloWorld::playBoomM() {
	SimpleAudioEngine::getInstance()->playEffect("music/boom.wav", false, 1.0f, 1.0f, 1.0f);
}

void HelloWorld::playBeatM() {
	SimpleAudioEngine::getInstance()->playEffect("music/beat.wav", false, 1.0f, 1.0f, 1.0f);
}

void HelloWorld::playShoot()
{
	SimpleAudioEngine::getInstance()->playEffect("music/shoot.wav", false, 1.0f, 1.0f, 1.0f);
}

void HelloWorld::addPlayers()
{
	//////////////////////////////////////
	//设置第一个精灵的动画
	//创建一张贴图
	auto texture = Director::getInstance()->getTextureCache()->addImage("$kanaIriya_walk.png");
	auto frame0 = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 46, 72)));

	//使用第一帧创建精灵
	player = Sprite::createWithSpriteFrame(frame0);
	player->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height / 2));
	player->setScale(1.5);
	//刚体设置  撞击掩码
	player->setAnchorPoint(Vec2(0.5, 0.5));
	player->setPhysicsBody(PhysicsBody::createBox(Size(40, 40)));
	player->getPhysicsBody()->setCategoryBitmask(0x01);
	player->getPhysicsBody()->setCollisionBitmask(0x02);
	//设置不被随机炸弹打动
	player->getPhysicsBody()->setAngularVelocityLimit(0);
	player->getPhysicsBody()->setVelocityLimit(0);
	//0100
	player->getPhysicsBody()->setContactTestBitmask(0x04);

	//静态动画
	idle.reserve(1);
	idle.pushBack(frame0);

	//攻击动画
	//$kanaIriya_beat
	auto texture0_1 = Director::getInstance()->getTextureCache()->addImage("$kanaIriya_beat.png");
	attack.reserve(9);
	for (int i = 0; i < 9; i++) {
		auto frame = SpriteFrame::createWithTexture(texture0_1, CC_RECT_PIXELS_TO_POINTS(Rect(72 * i, 0, 72, 102)));
		attack.pushBack(frame);
	}
	attack.pushBack(frame0);

	//死亡动画
	auto texture0_2 = Director::getInstance()->getTextureCache()->addImage("$kanaIriya_dead.png");
	dead.reserve(3);
	for (int i = 0; i < 3; i++) {
		auto frame = SpriteFrame::createWithTexture(texture0_2, CC_RECT_PIXELS_TO_POINTS(Rect(82 * i, 0, 82, 62)));
		dead.pushBack(frame);
	}

	//运动动画
	//$kanaIriya_forward
	auto texture0_3 = Director::getInstance()->getTextureCache()->addImage("$kanaIriya_forward.png");
	run.reserve(8);
	for (int i = 0; i < 8; i++) {
		auto frame = SpriteFrame::createWithTexture(texture0_3, CC_RECT_PIXELS_TO_POINTS(Rect(79 * i, 0, 79, 79)));
		run.pushBack(frame);
	}
	run.pushBack(frame0);
	/////////////////////////////////////



	////////////////////////////////////
	//设置第二个精灵动画
	//创建一张贴图
	auto texture2 = Director::getInstance()->getTextureCache()->addImage("$ailes_1.png");
	//从贴图中以像素单位切割，创建关键帧
	auto frame2 = SpriteFrame::createWithTexture(texture2, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 64, 66)));

	//创建第二个精灵
	player2 = Sprite::createWithSpriteFrame(frame2);
	player2->setPosition(Vec2(origin.x + visibleSize.width / 4 * 3,
		origin.y + visibleSize.height / 4 * 3));
	player2->setScale(1.5);


	//刚体设置  撞击掩码
	player2->setAnchorPoint(Vec2(0.5, 0.5));
	player2->setPhysicsBody(PhysicsBody::createBox(Size(40, 40)));
	player2->getPhysicsBody()->setCategoryBitmask(0x01);
	player->getPhysicsBody()->setCollisionBitmask(0x02);
	//设置不被随机炸弹打动
	player2->getPhysicsBody()->setAngularVelocityLimit(0);
	player2->getPhysicsBody()->setVelocityLimit(0);
	//0011
	player2->getPhysicsBody()->setContactTestBitmask(0x03);
	//攻击动画
	attack2.reserve(7);
	for (int i = 0; i < 7; i++) {
		auto texture2_temp = Director::getInstance()->getTextureCache()->addImage("$ailes_2.png");
		auto frame = SpriteFrame::createWithTexture(texture2_temp, CC_RECT_PIXELS_TO_POINTS(Rect(84* i, 0, 84, 68)));
		attack2.pushBack(frame);
	}

	//运动动画
	auto texture3 = Director::getInstance()->getTextureCache()->addImage("$ailes_1.png");
	run2.reserve(7);
	for (int i = 0; i < 4; i++) {
		auto frame = SpriteFrame::createWithTexture(texture3, CC_RECT_PIXELS_TO_POINTS(Rect(64 * i, 0, 64, 66)));
		run2.pushBack(frame);
	}

	//静止
	auto standbyAnimation = Animation::createWithSpriteFrames(run2, 0.1f);
	auto standbyAnimate = Animate::create(standbyAnimation);
	player2->runAction(RepeatForever::create(standbyAnimate));
	///////////////////////////////////

	//设置精灵的状态
	status = 4;
	status2 = 4;

	addChild(player, 3);
	addChild(player2, 3);


}


void HelloWorld::actionEvent(Ref*, char cid) {


	//攻击
	auto attackAnimation = Animation::createWithSpriteFrames(attack, 0.1f);
	auto attackAnimate = Animate::create(attackAnimation);

	//死亡
	auto deadAnimation = Animation::createWithSpriteFrames(dead, 0.1f);
	auto deadAnimate = Animate::create(deadAnimation);

	switch (cid) {
	case 'X': {
		//pT->setPercentage(pT->getPercentage() - 10);
		pT->runAction(ProgressTo::create(0.5f, pT->getPercentage() - 10));
		player->runAction(Sequence::create(deadAnimate, NULL));
		break;
	}
	case 'Y': {
		//pT->setPercentage(pT->getPercentage() + 10);

		//每当人物的攻击时候 怪物也移动
		Factory::getInstance()->moveMonster(player->getPosition(), 0.5f);
		pT->runAction(ProgressTo::create(0.5f, pT->getPercentage() + 10));
		player->runAction(Sequence::create(attackAnimate, NULL));

		if (attackMonster()) {

			//把数据添加到数据库中
			int tempN = database->getIntegerForKey("attackNumber");
			database->setIntegerForKey("attackNumber", tempN + 1);
			std::stringstream temp;
			temp << (tempN + 1);
			std::string record;
			temp >> record;
			time->setString(record);

		}
		break;
	}
	}
}

void HelloWorld::stopAc() {
}

void HelloWorld::updateTime(float dt) {

	//大于0时候， 每次调用就减去1
	if (dtime > 0) {
		dtime--;
		std::stringstream temp;
		temp << dtime;
		std::string dataString;
		temp >> dataString;
		time->setString(dataString);
	}
	if (dtime <= 0) {
		if (pT->getPercentage() > pT2->getPercentage()) Global::setWinner(2);
		else if (pT->getPercentage() == pT2->getPercentage()) Global::setWinner(1);
		else Global::setWinner(0);
		CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5, GameOver::createScene()));
	}

	//随机炸弹
	if (dtime % 10 == 0) {
		newEnemys();
	}

}

void HelloWorld::addMonster(float dt) {

	auto fac = Factory::getInstance();
	auto m = fac->createMonster();
	float x = random(origin.x, visibleSize.width);
	float y = random(origin.y, visibleSize.height);
	m->setPosition(x, y);

}

void HelloWorld::hitByMonster(float dt) {

	//auto fac = Factory::getInstance();
	//Sprite* collision = fac->collider(player->getBoundingBox());
	//if (collision != NULL) {
	//	fac->removeMonster(collision);
	//	actionEvent(this, 'X');
	//}

	//发动龟派气功
	if (bullet != NULL) {
		bullet->setPosition(bullet->getPosition() + (player2->getPosition() - player->getPosition())*0.1);
		//击中目标
		if (bullet->getPosition().getDistance(player2->getPosition()) < 20) {
			//爆炸特效
			auto exp = ParticleSystemQuad::create("explode.plist");
			exp->setPosition(bullet->getPosition());
			addChild(exp);
			//音效
			playBoomM();
			removeChild(bullet);
			bullet = NULL;
			pT2->runAction(ProgressTo::create(0.5f, pT2->getPercentage() - 30));
		}
		if (bullet != NULL && (bullet->getPositionX() > visibleSize.width || bullet->getPositionX() < 0 || bullet->getPositionY() > visibleSize.height || bullet->getPositionY() < 0)) {
			removeChild(bullet);
			bullet = NULL;
		}
	}

	if (bullet2 != NULL) {
		bullet2->setPosition(bullet2->getPosition() + (player->getPosition() - player2->getPosition())*0.1);
		//击中目标
		if (bullet2->getPosition().getDistance(player->getPosition()) < 20) {
			//爆炸特效
			auto exp = ParticleSystemQuad::create("explode.plist");
			exp->setPosition(bullet2->getPosition());
			addChild(exp);
			//音效
			playBoomM();
			removeChild(bullet2);
			bullet2 = NULL;
			pT->runAction(ProgressTo::create(0.5f, pT->getPercentage() - 30));
		}
		if (bullet2 != NULL && (bullet2->getPositionX() > visibleSize.width || bullet2->getPositionX() < 0 || bullet2->getPositionY() > visibleSize.height || bullet2->getPositionY() < 0)) {
			removeChild(bullet2);
			bullet2 = NULL;
		}
	}

	//游戏结束，判断获胜者
	if (pT->getPercentage() > pT2->getPercentage()) Global::setWinner(2);
	else if (pT->getPercentage() == pT2->getPercentage()) Global::setWinner(1);
	else Global::setWinner(0);
	
	if (pT->getPercentage() <= 0 || pT2->getPercentage() <= 0) {
		CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5, GameOver::createScene()));
	}

	removeChild(count_field);
	removeChild(count_field2);

	//更新龟派气功数量
	string a = "Rest of Bang: ";
	if (fire_count == 2) a = a + "2";
	else if (fire_count == 1) a = a + "1";
	else a = a + "0";
	count_field = Label::create(a, "Arial", 15);
	count_field->setPosition(Size(visibleSize.width / 10 - 10, visibleSize.height - 40));
	this->addChild(count_field, 1);

	string a2 = "Rest of Bang: ";
	if (fire_count2 == 2) a2 = a2 + "2";
	else if (fire_count2 == 1) a2 = a2 + "1";
	else a2 = a2 + "0";
	count_field2 = Label::create(a2, "Arial", 15);
	count_field2->setPosition(Size(visibleSize.width / 2 + 120, visibleSize.height - 40));
	this->addChild(count_field2, 1);
}

void HelloWorld::loadMap() {
	//根据文件路径快速导入瓦片地图 
	TMXTiledMap* tmx = TMXTiledMap::create("map/main_map.tmx");
	//设置位置 
	tmx->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	//设置锚点 
	tmx->setAnchorPoint(Vec2(0.5, 0.5));
	//适应大小
	tmx->setScale(Director::getInstance()->getContentScaleFactor());
	//添加到游戏图层中，其中0代表Z轴（Z轴低的会被高的遮挡） 
	this->addChild(tmx, 0);
}
bool HelloWorld::attackMonster() {
	//定义攻击范围
	Rect playRect = player->getBoundingBox();
	Rect attackReck = Rect(playRect.getMinX() - 40, playRect.getMinY(), \
		playRect.getMaxX() - playRect.getMinX() + 80, playRect.getMaxY() - playRect.getMinY());

	auto fac = Factory::getInstance();

	Sprite* collision = fac->collider(attackReck);
	if (collision != NULL) {
		fac->removeMonster(collision);
		return true;
	}
	return false;
}
void HelloWorld::addContactListener() {
	auto touchListener = EventListenerPhysicsContact::create();
	touchListener->onContactBegin = CC_CALLBACK_1(HelloWorld::onConcactBegan, this);
	_eventDispatcher->addEventListenerWithFixedPriority(touchListener, 1);
}

bool HelloWorld::onConcactBegan(PhysicsContact& contact) {
		auto body1 = contact.getShapeA()->getBody();
		auto body2 = contact.getShapeB()->getBody();
		auto sp1 = (Sprite*)body1->getNode();
		auto sp2 = (Sprite*)body2->getNode();
		//爆炸效果
		auto exp = ParticleSystemQuad::create("explode.plist");
		if (sp1 && sp2) {
			if (sp1 == player || sp1 == player2) {
				//播放爆炸声效
				playBoomM();
				//移除爆炸的陨石，并且播放爆炸动效
				exp->setPosition(sp2->getPosition());
				//游戏者受伤
				if (sp1 == player) {
					pT->runAction(ProgressTo::create(1, pT->getPercentage() - 20.0));
				}
				if (sp1 == player2) {
					pT2->runAction(ProgressTo::create(1, pT2->getPercentage() - 20.0));
				}
				//随机炸弹爆炸
				if (sp2 != player && sp2 != player2) {
					sp2->removeFromParentAndCleanup(true);
				}
				addChild(exp);
			}
			else if (sp2 == player || sp2 == player2) {
				//播放爆炸声效
				playBoomM();
				//移除爆炸的陨石，并且播放爆炸动效
				exp->setPosition(sp1->getPosition());
				//游戏者受伤
				if (sp2 == player) {
					pT->runAction(ProgressTo::create(1, pT->getPercentage() - 20.0));
				}
				if (sp2 == player2) {
					pT2->runAction(ProgressTo::create(1, pT2->getPercentage() - 20.0));
				}
				//随机炸弹爆炸
				if (sp1 != player && sp1 != player2) {
					sp1->removeFromParentAndCleanup(true);
				}
				addChild(exp);
			}
			else {
				//播放爆炸声效
				playBoomM();
				//移除爆炸的陨石，并且播放爆炸动效
				exp->setPosition(sp1->getPosition());
				sp1->removeFromParentAndCleanup(true);
				sp2->removeFromParentAndCleanup(true);
				addChild(exp);
			}
		}
		return true;
}
void HelloWorld::addKeyboardListener() {

	auto listenKB = EventListenerKeyboard::create();
	listenKB->onKeyPressed = CC_CALLBACK_2(HelloWorld::onKeyPressed, this);
	listenKB->onKeyReleased = CC_CALLBACK_2(HelloWorld::onKeyReleased, this);
	auto dispatcher = Director::getInstance()->getEventDispatcher();
	dispatcher->addEventListenerWithSceneGraphPriority(listenKB, player);
}


void HelloWorld::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
	//player1
	lastKey = 'D';
	lastKey2 = 'D';
	switch (code) {
	case cocos2d::EventKeyboard::KeyCode::KEY_A:
		if (lastKey != 'A') {
			player->setFlipX(true);
		}
		lastKey = 'A';
		player->setPosition(player->getPositionX() - 1, player->getPositionY());
		status = 0;
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_D:
		player->setFlipX(false);
		lastKey = 'D';
		player->setPosition(player->getPositionX() + 1, player->getPositionY());
		status = 2;
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_W:
		player->setPosition(player->getPositionX(), player->getPositionY() + 1);
		status = 1;
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_S:
		player->setPosition(player->getPositionX(), player->getPositionY() - 1);
		status = 3;
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_J:
		playerAttack();
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_K:
		fire1();
		break;

	//player2
	case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		//反转
		if (lastKey2 != 'A') {
			player2->setFlipX(true);
		}
		lastKey2 = 'A';
		player2->setPosition(player2->getPositionX() - 1, player2->getPositionY());
		status2 = 0;
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		//反转
		player2->setFlipX(false);
		lastKey2 = 'D';
		player2->setPosition(player2->getPositionX() + 1, player2->getPositionY());
		status2 = 2;
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
		player2->setPosition(player2->getPositionX(), player2->getPositionY() + 1);
		status2 = 1;
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW:
		player2->setPosition(player2->getPositionX(), player2->getPositionY() - 1);
		status2 = 3;
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_SLASH:
		player2Attack();
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_SHIFT:
		fire2();
		break;
	default:
		break;
	}
}

void HelloWorld::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
	status = 4;
	status2 = 4;
}


//为保证按键持续有效 周期调度
void HelloWorld::continueKeyUpdate(float f) {

	if (player == NULL || player2 == NULL) return;

	//player1移动的动画
	auto moveAnimation = Animation::createWithSpriteFrames(run, 0.1f);
	auto moveAnimate = Animate::create(moveAnimation);

	//player2移动的动画
	auto moveAnimation2 = Animation::createWithSpriteFrames(run2, 0.08f);
	auto moveAnimate2 = Animate::create(moveAnimation2);

	switch (status) {

		//左
	case 0:
		if (player->getPosition().x - 25 > origin.x) {
			player->setPosition(player->getPositionX() - 10, player->getPositionY());
			player->runAction(Sequence::create(moveAnimate, NULL));
		}
		break;
		//上
	case 1:
		if (player->getPosition().y + 25 < origin.y + visibleSize.height) {
			player->setPosition(player->getPositionX(), player->getPositionY() + 10);
			player->runAction(Sequence::create(moveAnimate, NULL));
		}
		break;
		//右
	case 2:
		if (player->getPosition().x + 25 < origin.x + visibleSize.width) {
			player->setPosition(player->getPositionX() + 10, player->getPositionY());
			player->runAction(Sequence::create(moveAnimate, NULL));
		}
		break;
		//下
	case 3:
		if (player->getPosition().y - 25 > origin.y) {
			player->setPosition(player->getPositionX(), player->getPositionY() - 10);
			player->runAction(Sequence::create(moveAnimate, NULL));
		}
		break;
	default:
		break;
	}

	switch (status2) {

		//左
	case 0:
		if (player2->getPosition().x - 25 > origin.x) {
			player2->setPosition(player2->getPositionX() - 10, player2->getPositionY());
			player2->runAction(Sequence::create(moveAnimate2, NULL));
		}
		break;
		//上
	case 1:
		if (player2->getPosition().y + 25 < origin.y + visibleSize.height) {
			player2->setPosition(player2->getPositionX(), player2->getPositionY() + 10);
			player2->runAction(Sequence::create(moveAnimate2, NULL));
		}
		break;
		//右
	case 2:
		if (player2->getPosition().x + 25 < origin.x + visibleSize.width) {
			player2->setPosition(player2->getPositionX() + 10, player2->getPositionY());
			player2->runAction(Sequence::create(moveAnimate2, NULL));
		}
		break;
		//下
	case 3:
		if (player2->getPosition().y - 25 > origin.y) {
			player2->setPosition(player2->getPositionX(), player2->getPositionY() - 10);
			player2->runAction(Sequence::create(moveAnimate2, NULL));
		}
		break;
	default:
		break;
	}
}

//判断两个人物是否在攻击范围
bool HelloWorld::canAttack() {

	//定义攻击范围
	Rect playRect = player->getBoundingBox();
	Rect attackReck = Rect(playRect.getMinX() - 40, playRect.getMinY(), \
		playRect.getMaxX() - playRect.getMinX() + 80, playRect.getMaxY() - playRect.getMinY());

	if (attackReck.containsPoint(player2->getPosition())) {

		return true;
	}
	return false;
}

//player攻击
void HelloWorld::playerAttack() {
	//音效
	playBeatM();

	auto attackAnimation = Animation::createWithSpriteFrames(attack, 0.1f);
	auto attackAnimate = Animate::create(attackAnimation);
	player->runAction(Sequence::create(attackAnimate, NULL));
	if (canAttack()) {
		pT2->runAction(ProgressTo::create(0.5f, pT2->getPercentage() - 10));
	}
}

//player2攻击
void HelloWorld::player2Attack() {
	//音效
	playBeatM();

	auto attackAnimation = Animation::createWithSpriteFrames(attack2, 0.08f);
	auto attackAnimate = Animate::create(attackAnimation);
	player2->runAction(Sequence::create(attackAnimate, NULL));
	if (canAttack()) {
		pT->runAction(ProgressTo::create(0.5f, pT->getPercentage() - 10));
	}
}

void HelloWorld::fire1()
{
	if (fire_count <= 0 || bullet != NULL) return;
	//音效
	playShoot();

	bullet = Sprite::create("bullet.png");
	bullet->setPosition(player->getPosition());
	addChild(bullet, 1);
	fire_count--;
}

void HelloWorld::fire2()
{
	if (fire_count2 <= 0 || bullet2 != NULL) return;
	//音效
	playShoot();

	bullet2 = Sprite::create("bullet.png");
	bullet2->setPosition(player2->getPosition());
	addChild(bullet2, 1);
	fire_count2--;
}
