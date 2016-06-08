#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
using namespace cocos2d;
class HelloWorld : public cocos2d::Layer
{
public:
	void setPhysicsWorld(PhysicsWorld * world);
	static cocos2d::Scene* createScene();

	static HelloWorld * create(PhysicsWorld * world);

    virtual bool init(PhysicsWorld* world);

	//初始化地图
	void loadMap();
	//bool init(PhysicsWorld * world);
	void addEdge();
	void newEnemys();
	void addEnemy(Point p);
	//音乐与音效
	void preloadMusic();
	void playBGM();
	void playBoomM();
	void playBeatM();
	void playShoot();

	//添加精灵
	void addPlayers();

	//
	void moveEvent(Ref*, char);
	void actionEvent(Ref*, char);
	void stopAc();
	void updateTime(float dt);
    // implement the "static create()" method manually
    //CREATE_FUNC(HelloWorld);
	//void update(float dt) override;

	//add function
	//每个一秒产出一个怪物
	void addMonster(float dt);

	//碰撞检测
	void hitByMonster(float dt);

	//攻击怪物
	bool attackMonster();
	void addContactListener();
	bool onConcactBegan(PhysicsContact & contact);
	//添加键盘监听
	void addKeyboardListener();
	void onKeyPressed(EventKeyboard::KeyCode code, Event* event);

	void onKeyReleased(EventKeyboard::KeyCode code, Event* event);
	//持续静听键盘移动
	void continueKeyUpdate(float f);
	//移动的状态 0 左 1 上 2 右 3 下  4无效
	int status;
	int status2;

	//攻击
	//判断攻击
	bool canAttack();
	void playerAttack();
	void player2Attack();
	void fire1();
	void fire2();


private:
	//first player
	cocos2d::Sprite* player;
	cocos2d::Sprite* bullet;
	cocos2d::Vector<SpriteFrame*> attack;
	cocos2d::Vector<SpriteFrame*> dead;
	cocos2d::Vector<SpriteFrame*> run;
	cocos2d::Vector<SpriteFrame*> idle;

	//second player
	cocos2d::Sprite* player2;
	cocos2d::Sprite* bullet2;
	cocos2d::Vector<SpriteFrame*> attack2;
	cocos2d::Vector<SpriteFrame*> dead2;
	cocos2d::Vector<SpriteFrame*> run2;
	cocos2d::Vector<SpriteFrame*> idle2;


	cocos2d::Size visibleSize;
	cocos2d::Vec2 origin;
	cocos2d::Label* time;
	cocos2d::Label* count_field;
	cocos2d::Label* count_field2;
	PhysicsWorld* m_world;
	Vector<PhysicsBody*> enemys;

	int dtime;
	int fire_count;
	int fire_count2;
	cocos2d::ProgressTimer* pT;
	cocos2d::ProgressTimer* pT2;

	//记录上一次的方向
	char lastKey;
	char lastKey2;

};

#endif // __HELLOWORLD_SCENE_H__
