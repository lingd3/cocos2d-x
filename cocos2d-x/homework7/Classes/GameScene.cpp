#include "GameScene.h"
#include "json/rapidjson.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"
#include <regex>
using std::regex;
using std::match_results;
using std::regex_match;
using std::cmatch;
using namespace rapidjson;

USING_NS_CC;

cocos2d::Scene* GameScene::createScene() {
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = GameScene::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

bool GameScene::init() {
	if (!Layer::init())
	{
		return false;
	}

	srand(time(NULL));

	Size size = Director::getInstance()->getVisibleSize();
	visibleHeight = size.height;
	visibleWidth = size.width;

	score_field = TextField::create("Score", "Arial", 30);
	score_field->setPosition(Size(visibleWidth / 4, visibleHeight / 4 * 3));
	this->addChild(score_field, 2);

	submit_button = Button::create();
	submit_button->setTitleText("Submit");
	submit_button->setTitleFontSize(30);
	submit_button->setPosition(Size(visibleWidth / 4, visibleHeight / 4));
	this->addChild(submit_button, 2);

	rank_field = TextField::create("10", "Arial", 30);
	rank_field->setPosition(Size(visibleWidth / 4 * 3, visibleHeight / 4 * 3));
	this->addChild(rank_field, 2);

	rank_button = Button::create();
	rank_button->setTitleText("Rank");
	rank_button->setTitleFontSize(30);
	rank_button->setPosition(Size(visibleWidth / 4 * 3, visibleHeight / 4));
	this->addChild(rank_button, 2);

	submit_button->addClickEventListener(CC_CALLBACK_1(GameScene::buttonClick, this));
	rank_button->addClickEventListener(CC_CALLBACK_1(GameScene::buttonClick1, this));

	return true;
}

void GameScene::buttonClick(Ref *pSender) {
	HttpRequest* request = new HttpRequest();
	request->setUrl("http://localhost:8080/submit");
	request->setRequestType(HttpRequest::Type::POST);
	request->setResponseCallback(CC_CALLBACK_2(GameScene::onHttpRequestCompleted, this));

	string score = "score=" + score_field->getStringValue();
	const char* postData = score.c_str();
	request->setRequestData(postData, strlen(postData));
	request->setTag("POST");

	std::vector<std::string> headers;
	headers.push_back("Content-Type: application/x-www-form-urlencoded; charset=UTF-8");
	headers.push_back("Cookies: GAMESESSIONID=" + Global::gameSessionId);
	request->setHeaders(headers);


	cocos2d::network::HttpClient::getInstance()->send(request);
	request->release();
}

void GameScene::onHttpRequestCompleted(HttpClient *sender, HttpResponse *response) {
	if (!response) return;
	if (!response->isSucceed()) {
		log("response failed");
		log("error buffer: %s", response->getErrorBuffer());
		return;
	}
	std::vector<char> *buffer = response->getResponseData();
	log("Http Test, dump data: ");
	for (int i = 0; i < buffer->size(); i++) {
		CCLog("%c", (*buffer)[i]);
	}

}

void GameScene::buttonClick1(Ref *pSender) {
	string a = "";
	char str[10] = {'0'};
	sprintf(str, "%d", rand());
	a = str;

	HttpRequest* request = new HttpRequest();
	request->setUrl(("http://localhost:8080/rank?top=10&rand=" + a).c_str());
	request->setRequestType(HttpRequest::Type::GET);
	request->setResponseCallback(CC_CALLBACK_2(GameScene::onHttpRequestCompleted1, this));

	request->setTag("GET");
	std::vector<std::string> headers;
	headers.push_back("Content-Type: application/x-www-form-urlencoded; charset=UTF-8");
	headers.push_back("Cookies: GAMESESSIONID=" + Global::gameSessionId);
	request->setHeaders(headers);

	cocos2d::network::HttpClient::getInstance()->send(request);
	request->release();
}

void GameScene::onHttpRequestCompleted1(HttpClient *sender, HttpResponse *response) {
	if (!response) return;
	if (!response->isSucceed()) {
		log("response failed");
		log("error buffer: %s", response->getErrorBuffer());
		return;
	}

	std::vector<char> *buffer = response->getResponseData();

	string rank = Global::toString(buffer);
	cocos2d::log(rank.c_str());
	rapidjson::Document d;
	d.Parse<0>(rank.c_str());
	if (d.HasParseError()) {
		log("GetParseError %s\n", d.GetParseError());
	}

	if (d.IsObject() && d.HasMember("info")) {
		if (rank_field != NULL) this->removeChild(rank_field);

		TTFConfig ttfConfig;
		ttfConfig.fontFilePath = "fonts/arial.ttf";
		ttfConfig.fontSize = 30;

		string a = "", b = "";
		int count = 0;
		for (int i = 1; d["info"].GetString()[i] !=  '\0'; i++) {
			/*log("%c\nhehe ", d["info"].GetString()[i]);*/
			if (d["info"].GetString()[i] == '|') {
				b = b + "\n" + a;
				a = "";
			}
			else {
				a = a + d["info"].GetString()[i];
			}
		}
		rank_field = TextField::create(b, "Arial", 30);
		rank_field->setPosition(Size(visibleWidth / 4 * 3+70, visibleHeight / 4 * 3-40));
		this->addChild(rank_field, 2);
		
	}

}
