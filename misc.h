
#ifndef MISC_H
#define MISC_H

#ifndef INCLUDES_H
#include "includes.h"
#endif // INCLUDES_H
#include "animation.h"
#include "weapon.h"
#include "private.h"
#include <deque>
#include <thr/mutex>

// constants
#define GRAVITY (sf::Vector2f(0,300.0f))
#define PLAYER_JUMP (-550.0f)
#define UPDATE_SPEED (200.0f)
#define RUN_SPEED 200.0f
#define WAIT_RANGE RandomFloat(2.0f,10.0f)

// i dont know why this is here
class Vec{
public:
	typedef const Vec& ref;
	typedef const float& fref;
	float x, y;
	Vec(float x1, float y1, float x2, float y2): x(x2-x1), y(y2-y1){
	}
	Vec(float X, float Y) : x(X), y(Y){}
	template <class T> Vec(const sf::Vector2<T>& t) : x(t.x), y(t.y){
	}

	Vec& operator= (ref t){
		x = t.x; y = t.y;
		return *this;
	}
	Vec& operator+= (ref t){
		x += t.x; y += t.y;
		return *this;
	}
	Vec& operator-= (ref t){
		x -= t.x; y-= t.y;
		return *this;
	}
	Vec& operator*= (fref t){
		x *= t; y*= t;
		return *this;
	}
	Vec& operator/= (fref t){
		x /= t; y/= t;
		return *this;
	}
	Vec operator+ (ref t){
		return Vec(x+t.x, y+t.y);
	}
	Vec operator- (ref t){
		return Vec(x-t.x, y-t.y);
	}
	float operator* (ref t){
		return x*t.x + y*t.y;
	}
	friend Vec operator * (fref t, ref v){
		return Vec(t*v.x, t*v.y);
	}
	Vec& operator* (fref t){
		x *= t;
		y *= t;
		return *this;
	}
	float operator () (){
		return x*x + y*y;
	}
	void Reflect(ref a, ref b){
		Vec n(a.y - b.y, b.x - a.x);
		*this = *this - (n*2)*((*this*n)/ n());
	}
};


// damage/protection types


// game creature (animation based)
class Creature: public sf::Drawable, public sf::Transformable{
public:
	Creature()
	{
	}
	Creature(const AnimationExplorer& anim, const sf::Vector2f& position, sf::Vector2f grav = GRAVITY) : _direction(0,0), _accel(grav), _gravity(GRAVITY), _anim(anim){
		_anim.setPosition(position);
	}
	Creature(const Creature& cr) : _hp(cr._hp), _ap(cr._ap), _anim(cr._anim), _gravity(cr._gravity){
		_direction = cr._direction;
		_accel = cr._accel;
	}
	virtual void setPosition(const sf::Vector2f& pos){
		_anim.setPosition(pos);
	}
	virtual void setPosition(float x, float y){
		_anim.setPosition(x,y);
	}
	virtual sf::Vector2f getPosition()const{
		return _anim.getPosition();
	}
	void SetMovement(const sf::Vector2f& move){
		_direction = move;
	}
	void SetAcceleration(const sf::Vector2f& accel){
		_accel = accel;
	}
	void Update(float dt){
		_anim.move(_direction*dt);
		_direction += _accel*dt;
		_anim.Update(dt);
	}
	virtual bool Intersect(const sf::FloatRect& rect)const{
		return _anim.Intersect(rect);
	}
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states)const{
		target.draw(_anim, states);
	}
	virtual void scale(float dx, float dy){
		_anim.scale(dx, dy);
	}
	virtual void move(float dx, float dy){
		_anim.move(dx, dy);
	}
	virtual void move(const sf::Vector2f &offset){
		_anim.move(offset);
	}
	sf::Vector2f Direction()const{
		return _direction;
	}
	void NullifySpeed(){
		_direction.y = 0;
		_direction.x = 0;
		_accel.y = 0;
	}
	float getDownY()const{
		return _anim.getPosition().y + _anim.getGlobalBounds().height;
	}
	float getRightX()const{
		return _anim.getPosition().x + _anim.getGlobalBounds().width;
	}
protected:
	float _hp;
	float _ap;
	AnimationExplorer _anim;
	sf::Vector2f _direction;
	sf::Vector2f _accel;
	sf::Vector2f _gravity;
};


class Enemy: public Creature{
public:
	int Level;
	Enemy(){}
	Enemy(const Creature& creat, int level, bool resist=false, ItemStates::ElementType resist_elem = ItemStates::ElementType::USUAL):
	_resistant(resist), _resistance(resist_elem), Level(level){

	}

private:
	bool _resistant;
	ItemStates::ElementType _resistance;
};


// class that represents a playing person
class Actor: public Creature{
public:
	
	// in-game existing actor's states
	static enum ActorState{ONFLOOR, JUMP, FALL, DUCK, DEAD};

	Actor(const Creature &creat, float floor): Creature(creat), _dt(0), _floor(floor), _flt(floor){
		hands = 0;
		head = torse = legs = foot = hands = 0;
		_onfloor = true;
		_mystate = ONFLOOR;
		NullifySpeed();
		_space_pressed = false;
	}
	Actor(){}
	
	// process events and update
	ActorState Control(float dt, const sf::Event& eventHappened, const sf::Mouse &aim){
		
		static float dx;
		if( !_onfloor && _direction.y > 0 && _anim.getGlobalBounds().top >= _floor - _anim.getGlobalBounds().height){
			Land();
		}
		if(eventHappened.type == sf::Event::KeyPressed){

			if(eventHappened.key.code == sf::Keyboard::Space && _onfloor){
				_space_pressed = true;
				if(_onfloor){
					// _dt += dt; // непонятно вообще зачем оно тут
					SetMovement(sf::Vector2f(0, PLAYER_JUMP));
					Fall(FALL);
					dx = 0.97f;
				}
			}

			if(eventHappened.key.code == sf::Keyboard::A){

				_anim.move(-dt*10.0f,0);

			}
			if(eventHappened.key.code == sf::Keyboard::A){

				_anim.move(dt*10.0f,0);

			}
		}
		if(eventHappened.type == sf::Event::KeyReleased){
			_space_pressed = false;
		}
		if(_space_pressed){
			if(_direction.y > 0.0f) {
				_direction *= dx;
				dx += dt/100.0f;
			}
		}
		Update(dt);
		return _mystate;
	}
	void Fall(ActorState state){
		if(state == FALL) _anim.ChangeAnimation("jump");	// если будет много разных анимаций
		_mystate = state;
		_accel = _gravity;
		_onfloor = false;
	}
	void Land(ActorState state = ONFLOOR){
		Creature::NullifySpeed();
		_anim.ChangeAnimation("run");
		_mystate = state;
		_onfloor = true;
	}
	ActorState GetState()const{
		return _mystate;
	}
	void SetFloor(const float &f){
		_flt = _floor;
		_floor = f;
	}
	void SetFloorBack(){
		_floor = _flt;
	}
	bool TestFloor()const{
		return _floor == _flt;
	}
private:
	ActorState _mystate;
	float _dt;	// time delta
	bool _onfloor;
	Backpack pack;
	BackpackItem* head, *torse, *legs, *foot, *hands;
	Weapon *weap;
	float _floor, _flt;
	bool _space_pressed;
};


// class representing an movable obstacle, that can be found in the game
class Obstacle : public sf::Drawable{
public:
	Obstacle(){
	}
	// hpos - formal height position (from the bottom)
	Obstacle(const sf::Sprite& sprite, const sf::Vector2u winsize, float hpos, float speed, float stretch): _speed(speed), _myspr(sprite){
		_myspr.scale(stretch, stretch);
		if(speed < 0.0f) _myspr.setPosition(winsize.x + _myspr.getGlobalBounds().width, winsize.y - _myspr.getGlobalBounds().height- hpos);
		else _myspr.setPosition(-_myspr.getGlobalBounds().width, winsize.y - _myspr.getGlobalBounds().height - hpos);
	}
	
	// allocate different obstacels in pack more than 1 item
	// for automatical use only. do not call this by yourself
	/*void MultipleSprite(ObstacleFactor amount){
		switch(amount){
		case ObstacleFactor::MED:{
			sf::Sprite* ns = new sf::Sprite(_mysprites[0]);
			ns->move(0, -ns->getGlobalBounds().height);
			_mysprites.push_back(*ns);
			break;
								 }
		case ObstacleFactor::BIG:{
			sf::Sprite* ns = new sf::Sprite(_mysprites[0]);
			sf::Sprite* ns2 = new sf::Sprite(*ns);
			ns->move(0, -ns->getGlobalBounds().height);
			ns2->move(ns2->getGlobalBounds().width, 0);
			_mysprites.push_back(*ns);
			_mysprites.push_back(*ns2);
			break;
								 }
		}
	}*/

	void Update(float dt){
		_myspr.move(_speed*dt,0);
	}
	// upd speed via offset
	void UpdateSpeed(float dv){
		_speed += dv;
	}
	void SetSpeed(float v){ _speed = v;}

	bool Collide(const sf::FloatRect& rect)const{
		if(rect.intersects(_myspr.getGlobalBounds())) return true;
		return false;
	}
	bool Collide(Actor* p)const{
		if(p->Intersect(_myspr.getGlobalBounds())){
			/*if(p->Direction().y > 0.0f && p->getPosition().x > _myspr.getPosition().x && p->getDownY() < _myspr.getPosition().y ){
				p->SetFloor(_myspr.getPosition().y);
				return false;
			}
			else return true;
		}
		else{
			if(!p->TestFloor() && p->GetState() == Actor::ONFLOOR && p->getPosition().x > _myspr.getGlobalBounds().width + _myspr.getPosition().x){
				p->SetFloorBack();
			}
			return false;*/
			return true;
		}
		return false;
	}
	// returns true if sprites are out of window limitation 
	bool CheckIfOut(const sf::Vector2u& win)const{
		if(_myspr.getPosition().x  < -_myspr.getGlobalBounds().width && _speed < 0.0f) return  true;
		if(_myspr.getPosition().x > win.x && _speed > 0.0f) return true;
		return false;
	}

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states)const{
		target.draw(_myspr, states);
	}

	static Obstacle* Generate(ResourceManager* resmgr, const sf::Vector2u& window, float h, float spd){
		sf::Sprite* mspr;
		float scale = RandomFloat(0.7f, 1.4f);
		switch(std::rand()%3){
		case 0:
			mspr = resmgr->getSprite(Resources::OBSTACLE_CRATE_WOOD.c_str());
			scale /= 3.0f;
			break;
		case 1:
			mspr = resmgr->getSprite(Resources::OBSTACLE_CRATE_METAL.c_str());
			break;
		case 2:
			mspr = resmgr->getSprite(Resources::OBSTACLE_TREE.c_str());
			scale/= 3.0f;
			break;
		}
		Obstacle *ret = new Obstacle(*mspr,window,h, spd, scale);
		return ret;
	}

	//static Obstacle* Generate(ObstacleFactor factor, ResourceManager* resmgr, const Background& bg, float h){
	//	std::srand(std::time(0));	
	//	sf::Sprite* mspr;	// My SPRite
	//	float scale;	// randomized scale multiplier
	//	switch(factor){
	//	case ObstacleFactor::SMALL:{
	//		scale = RandomFloat(1.0f, 1.2f);
	//		switch(std::rand()%3){
	//		case 0:
	//			mspr = resmgr->getSprite(Resources::OBSTACLE_CRATE_WOOD.c_str());
	//			scale /= 3.0f;
	//			break;
	//		case 1:
	//			mspr = resmgr->getSprite(Resources::OBSTACLE_CRATE_METAL.c_str());
	//			break;
	//		case 2:
	//			mspr = resmgr->getSprite(Resources::OBSTACLE_TREE.c_str());
	//			break;
	//		}

	//		break;
	//							   }
	//	case ObstacleFactor::MED:{
	//		scale = RandomFloat(1.0f, 1.3f);
	//		mspr = (std::rand()%2 == 0 ? resmgr->getSprite(Resources::OBSTACLE_CRATE_METAL.c_str()) : resmgr->getSprite(Resources::OBSTACLE_CRATE_WOOD.c_str()));
	//		break;
	//							 }
	//	case ObstacleFactor::BIG:{
	//		scale = RandomFloat(1.1f, 1.4f);
	//		mspr = (std::rand()%2 == 0 ? resmgr->getSprite(Resources::OBSTACLE_CRATE_METAL.c_str()) : resmgr->getSprite(Resources::OBSTACLE_CRATE_WOOD.c_str()));
	//							 }
	//	}
	//	Obstacle *ret = new Obstacle(*mspr,bg.GetWindowSize(),h,bg.GetSpeed());
	//	ret->Scale(scale);
	//	ret->factor = factor;
	//	ret->MultipleSprite(factor);
	//	return ret;
	//}

	void Scale(float stretch){
		//for(_iter i = _mysprites.begin(); i!= _mysprites.end(); i++)
		_myspr.scale(stretch, stretch);
	}

private:

	float _speed;
	sf::Sprite _myspr;
};


class Game{
public:

	static enum GameState {PLAY, MENU, PAUSE, BACK};
	Game() : _dt(0.0f), _state(MENU), _distance(0){
		graphicThread = new sf::Thread(&Game::_render, this);
		menuThread = new sf::Thread(&Game::_render_menu, this);
		_stop = _graphics_working = false;
		_menu_working = true;
		std::FILE *hs;
		if((hs = std::fopen("Resource//Hs//score.sc","rb")) == NULL) Highscore = 0;
		else{
			int h=0;
			std::fread(&h,sizeof(int), 1, hs);
			Highscore = h;
			std::fclose(hs);
		}
	}
	Game(sf::RenderWindow* window) : _dt(0.0f), _rand_obstacle_wait(WAIT_RANGE), _state(MENU), _distance(0){
		graphicThread = new sf::Thread(&Game::_render, this);
		menuThread = new sf::Thread(&Game::_render_menu, this);
		windowPtr = window;
		states = sf::RenderStates::Default;
		_stop = _graphics_working = false;
		_menu_working = true;
		std::FILE* hs;
		if((hs = std::fopen("Resource//Hs//score.sc","rb")) == NULL) Highscore = 0;
		else{
			int h=0;
			std::fread(&h,sizeof(int), 1, hs);
			Highscore = h;
			std::fclose(hs);
		}
	}

	void ConstructHud(ResourceManager* res){
		Event<Game, int> evn(&Game::_menu_buttons, this);
		hud.AddButton(Button<Game, int>(sf::FloatRect(512,200,256,64), res->getSprite("start_btn_face"), res->getSprite("start_btn_ht"), res->getSprite("start_btn_press"), evn, 0));
		hud.AddButton(Button<Game, int>(sf::FloatRect(512,300,256,64), res->getSprite("settings_btn_face"), res->getSprite("settings_btn_ht"), res->getSprite("settings_btn_press"), evn, 1));
		hud.AddButton(Button<Game, int>(sf::FloatRect(512,400,256,64), res->getSprite("about_btn_face"), res->getSprite("about_btn_ht"), res->getSprite("about_btn_press"), evn, 2));
		hud.AddButton(Button<Game, int>(sf::FloatRect(512,500,256,64), res->getSprite("exit_btn_face"), res->getSprite("exit_btn_ht"), res->getSprite("exit_btn_press"), evn, 3));
	}

	void LaunchRendering(){
		windowPtr->setActive(false);
		switch(_state){
		case PLAY: graphicThread->launch();
			break;
		case MENU:
			menuThread->launch();
			break;
		default:
			;
		}
	}
	void StopRendering(){
		graphicThread->wait();
		menuThread->wait();
	}
	
	// temporary
	void AddActor(const Creature& cr, float floor){
		actor = new Actor(cr, floor);
	}
	void AddBg(const Background& back){
		bg = new Background(back);
	}

	void Update(float dt, sf::Event& happened, sf::Mouse& mouse){
		switch(_state){
		case PLAY:
			PlayUpdate(dt, happened, mouse);
			break;
		case MENU:
			MenuUpdate(dt, happened, mouse);
		default:
			break;
		}
	}

	void MenuUpdate(const float &dt, sf::Event& happened, sf::Mouse& mouse){
		hud.Process(mouse, *windowPtr, happened);
	}

	void PlayUpdate(float dt, sf::Event& happened, sf::Mouse& mouse){
		if(happened.type == sf::Event::KeyPressed){
			switch(happened.key.code){
			case sf::Keyboard::Escape: _state = MENU;
				break;
			default:
				if(_stop){
					_stop = false;
					_distance = 0;
					obstacles.clear();
					bg->ResetSpeed();
				}
			}

		}
		if(_stop) return;
		_dt += dt;
		_distance -= bg->GetSpeed()*dt/UPDATE_SPEED;
		bg->AdjustSpeed(-dt);
		char b[10];
		//mtx1.lock();
		score->setString(std::string(_itoa(_distance, b, 10))+ "M");
		if (_distance > Highscore) hscore->setString(std::string("HIGH: ") + std::string(_itoa(_distance, b, 10)) + "M");
		//mtx1.unlock();
		//windowPtr->setActive(false);
	//	mtx2.lock();
		bg->Update(dt);
		
	//	mtx2.unlock();
		if(_dt > _rand_obstacle_wait){
			//obstacles.push_back(Obstacle::Generate(GenerateFactor(),Obstacle_res,*bg,RandomFloat(5.0f,15.0f)));
			obstacles.push_back(Obstacle::Generate(Obstacle_res,windowPtr->getSize(),RandomFloat(10.0f,20.0f),bg->GetSpeed()));
			_dt = 0.0f;
			_rand_obstacle_wait = WAIT_RANGE;
		}
		actor->Control(dt, happened, mouse);
		if(!obstacles.empty()){
			if((*obstacles.begin())->CheckIfOut(windowPtr->getSize())) obstacles.pop_front();
			for(std::deque<Obstacle*>::iterator i = obstacles.begin(); i!= obstacles.end(); i++){
				(*i)->SetSpeed(bg->GetSpeed());
				(*i)->Update(dt);
				if((*i)->Collide(actor)){
					Stop();;
					if (_distance > Highscore) Highscore = _distance;
				}
			}
		}
	}
	void _render(){
		windowPtr->setActive(true);
		while(windowPtr->isOpen() && _graphics_working){
			windowPtr->clear();
			//==========================
			mtx2.lock();
			windowPtr->draw(*bg, states);
			mtx2.unlock();
			for(int i=0; i<obstacles.size(); i++)
				windowPtr->draw(*obstacles[i], states);
			windowPtr->draw(*actor, states);
			mtx1.lock();
			windowPtr->draw(*score, states);
			mtx1.unlock();
			//==========================
			windowPtr->display();
		}
		windowPtr->setActive(false);
	}
	
	void __render(){
		windowPtr->draw(*bg, states);

		for(int i=0; i<obstacles.size(); i++)
			windowPtr->draw(*obstacles[i], states);
		windowPtr->draw(*actor, states);
		windowPtr->draw(*score, states);
		windowPtr->draw(*hscore, states);

	}

	void _menu_buttons(const int &x){
		switch(x){
		case 0: 
			_state = PLAY;
			_menu_working = false;
			_graphics_working = true;
			//menuThread->wait();
			//graphicThread->launch();
			break;
		case 3:
			windowPtr->close();
			break;
		default:
			break;
		}
	}

	// menuThread function
	void _render_menu(){
		windowPtr->setActive(true);
		while(windowPtr->isOpen() && _menu_working){
			windowPtr->clear();
			windowPtr->draw(hud, states);
			windowPtr->display();
		}
		windowPtr->setActive(false);
	}

	void __render_menu(){
		windowPtr->draw(hud, states);
	}
	GameState GetState()const{
		return _state;
	}

	sf::Thread *graphicThread;
	sf::Thread *menuThread;
	/*static Obstacle::ObstacleFactor GenerateFactor(){
		std::srand(std::time(0));
		int r = std::rand()%10;
		return ( r < 2 ? Obstacle::BIG : (r < 5 ? Obstacle::MED : Obstacle::SMALL));
	}*/
	void AddObstacleResPtr(ResourceManager* ptr){
		Obstacle_res = ptr;
	}
	void Stop(){ _stop = true;}
	void Start(){ _stop = false;}
	bool IsStopped()const{return _stop;}
	void SetFont(sf::Text* ptr){
		char b[10];
		score = new sf::Text(*ptr);
		hscore = new sf::Text(*ptr);
		hscore->move(0,score->getGlobalBounds().height+5.0f);
		hscore->setString(std::string("HIGH: ") + std::string(_itoa(Highscore,b,10)) + "M");
	}
	int Highscore;
private:
	
	sf::Mutex mtx1;
	sf::Mutex mtx2;
	GameState _state;
	float _dt;
	float _rand_obstacle_wait;	// time delay before next obstacle appears
	std::deque<Obstacle*> obstacles;
	sf::RenderWindow *windowPtr;
	sf::RenderStates states;
	Actor *actor;
	sf::Text* score;
	sf::Text* hscore;
	Background *bg;
	HUD hud;
	ResourceManager *Obstacle_res;
	bool _stop;
	bool _menu_working;
	bool _graphics_working;
	float _distance;
};

#endif // MISC_H