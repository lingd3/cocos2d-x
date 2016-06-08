#include "MenuScene.h"
#include "HelloWorldScene.h"
#include "GameOver.h"

USING_NS_CC;

Scene* MenuSence::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = MenuSence::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool MenuSence::init()
{

	if (!Layer::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	//添加背景
	auto background = Sprite::create("map/start_map.jpg");
	background->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	addChild(background, 0);

	auto GameName = Label::createWithTTF("星幻格斗", "fonts/pan.ttf", 50);
	GameName->setColor(ccc3(197, 90, 17));
	GameName->setPosition(visibleSize.width / 2, visibleSize.height * 3 / 4);
	addChild(GameName, 1);

	//开始游戏
	auto start_button = Button::create();
	start_button->setTitleText("Start Game");
	start_button->setTitleFontSize(30);
	start_button->setTitleFontName("fonts/pan.ttf");
	start_button->setColor(ccc3(64, 64, 64));
	start_button->setPosition(Size(visibleSize.width / 2, visibleSize.height / 4));
	start_button->addClickEventListener(CC_CALLBACK_1(MenuSence::menuChangeCallback, this));
	this->addChild(start_button, 2);

	//getName	
	auto button = Button::create();
	button->setTitleText("Get Name");
	button->setTitleFontSize(30);
	button->setColor(ccc3(64, 64, 64));
	button->setTitleFontName("fonts/pan.ttf");
	button->setPosition(Size(visibleSize.width / 2, visibleSize.height / 2-50));

	//添加点击监听
	button->addClickEventListener(CC_CALLBACK_1(MenuSence::getNameListener, this));
	this->addChild(button, 2);
	
	return true;
}

void MenuSence::menuChangeCallback(Ref* pSender)
{
	CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5, HelloWorld::createScene()));

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}

void MenuSence::getNameListener(Ref *pSender) {

	HttpRequest* request = new HttpRequest();
	request->setRequestType(HttpRequest::Type::GET);

	int number = rand();
	std::stringstream ss;
	ss << number;
	std::string numberToString;
	ss >> numberToString;
	std::string url = "http://119.29.222.187:8001/getName?rand=" + numberToString;
	request->setUrl(url.c_str());

	//request->setUrl("http://localhost:8000/getName");
	request->setResponseCallback(CC_CALLBACK_2(MenuSence::onHttpComplete, this));

	std::vector<std::string> headers;
	headers.push_back("Content-Type: application/x-www-form-urlencoded; charset=UTF-8");
	request->setHeaders(headers);

	cocos2d::network::HttpClient::getInstance()->send(request);
	request->release();
}

void MenuSence::onHttpComplete(HttpClient *sender, HttpResponse *response) {

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	//rank_field->setText("");
	//先把运行的field删除
	if (name_field != NULL) {
		name_field->removeFromParent();
		name_field = NULL;
	}
	std::vector<char> *buffer = response->getResponseData();
	string name = Global::toString(buffer);
	name_field = TextField::create(name, "fonts/pan.ttf", 30);
	name_field->setPosition(Size(visibleSize.width / 2, visibleSize.height / 2+10));
	this->addChild(name_field, 2);

}
