/*
*	Animation.h
*	graphics components routine
*
*	Contains:
*		- class Animation
*		- class AnimationExplorer
*		- class Background
*			-- class LayerConfig
*		- class MyRect
*		- function Sign
*	Includes:
*		- "includes.h"
*
*	Author: me
*
*/

#ifndef ANIMATION_H
#define ANIMATION_H

#include "includes.h"

int Sign(const float& x){
	return (x >= 0.0f ? 1 : -1);
}
int Sign(const double &x){
	return (x >= 0.0 ? 1 : -1);
}
int Sign(const int &x){
	return (x >= 0 ? 1 : -1);
}

class Animation : public sf::Transformable, public sf::Drawable{
public:
	Animation() { }
	Animation(const Animation& t): Speed(t.Speed), FrameRect(t.FrameRect), Frames(t.Frames),_flipped(t._flipped), _loop(t._loop),
		_frames(t._frames), _flip_frames(t._flip_frames){
			_rev_flag = t._rev_flag;
			_frame = t._frame;
			_dt = t._dt;
			_mysprite = new sf::Sprite(*t._mysprite);
	}


	// fulltex - ���������� ��� ��������� ������� �� �����. ���� �� ����� ����, ���� �� ����� �����������
	Animation(const sf::Sprite* sprite, const sf::IntRect& framerect, bool fulltex, int frames, float speed = 2.0f, bool loop = false, bool flip = false):
		_frame(1), Frames(frames), Speed(1.0f/speed), _loop(loop), _flipped(flip), FrameRect(framerect), _dt(0.0f){
			_mysprite = new sf::Sprite(*sprite);

			sf::IntRect temp(framerect);
			int cnt = 0;
			if(fulltex){
				while(temp.top <= _mysprite->getTexture()->getSize().y - temp.height){
					while(temp.left <= _mysprite->getTexture()->getSize().x - temp.width){
						if(cnt++ == Frames) break;
						_frames.push_back(sf::IntRect(temp.left, temp.top, temp.width, temp.height));
						_flip_frames.push_back(sf::IntRect(temp.left, temp.top, -temp.width, temp.height));
						temp.left += temp.width;
					}
					if(cnt == Frames) break;
					temp.top += temp.height;
					temp.left = 0;
				}
			}
			else{
				while(temp.left <= _mysprite->getTexture()->getSize().x - temp.width){
					if(cnt++ == Frames) break;
					_frames.push_back(sf::IntRect(temp.left, temp.top, temp.width, temp.height));
					_flip_frames.push_back(sf::IntRect(temp.left + temp.width, temp.top, -temp.width, temp.height));
					temp.left += temp.width;
				}
			}
			Frames = cnt;
			_mysprite->setTextureRect(FrameRect);
	}

	/// <summary>
	/// ��������� ����������� �������� � ��������� ��������
	/// </summary>
	/// <param name="dt">������� �� ������� ����� �������� �������.</param>
	void Update(float dt)
	{
		if ((_dt += dt) >= Speed)
		{
			_dt = 0.0f;
			if(!_loop){
				if (++_frame > Frames)
				{
					_mysprite->setTextureRect(_flipped ? _flip_frames[(_frame = 1)-1] : _frames[(_frame = 1)-1]);
					return;
				}
				else _mysprite->setTextureRect(_flipped ? _flip_frames[_frame-1] : _frames[_frame-1]);
			}
			else{
				if(_rev_flag){
					if(--_frame < 1){
						_mysprite->setTextureRect(_flipped ? _flip_frames[(_frame = 1)-1] : _frames[(_frame = 1)-1]);
						_rev_flag = false;
						return;
					}
					else _mysprite->setTextureRect(_flipped ? _flip_frames[_frame-1] : _frames[_frame-1]);
				}
				else{
					if(++_frame > Frames){
						_mysprite->setTextureRect(_flipped ? _flip_frames[(_frame = Frames)-1] : _frames[(_frame = Frames)-1]);
						_rev_flag = true;
						return;
					}
					else _mysprite->setTextureRect(_flipped ? _flip_frames[_frame-1] : _frames[_frame-1]);
				}
			}
		}
	}
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const
	{
		target.draw(*_mysprite,states);
	}

public: virtual void move(const sf::Vector2f& offset){
			_mysprite->move(offset);
		}
		virtual void move(float dx, float dy){
			_mysprite->move(dx, dy);
		}
		virtual void setPosition(const sf::Vector2f& pos){
			_mysprite->setPosition(pos);
		}
		virtual void setPosition(float x, float y){
			_mysprite->setPosition(x, y);
		}
		virtual void rotate(float angle){
			_mysprite->rotate(angle);
		}
		virtual void setOrigin(float x, float y){
			_mysprite->setOrigin(x, y);
		}
		virtual void setOrigin(const sf::Vector2f& or){
			_mysprite->setOrigin(or);
		}
		virtual void scale(float x, float y){
			_mysprite->scale(x, y);
		}
		bool Intersect(const sf::FloatRect& rect)const{
			return _mysprite->getGlobalBounds().intersects(rect);
		}
		virtual sf::FloatRect getGlobalBounds()const{
			return _mysprite->getGlobalBounds();
		}
		void Flip(){
			_flipped ^= true;
		}
		// private fields
private:
	bool _loop;
	bool _rev_flag;
	bool _flipped;
	sf::Sprite* _mysprite;
	// sf::Texture* _tex;
	float _dt;
	int _frame;
	std::vector<sf::IntRect> _frames;	// ������ �������
	std::vector<sf::IntRect> _flip_frames;
	// public fields

	/// <summary>
	/// ������ ��� �������� ���������� ������ � ��������, ������ ����� ������� � ������ ���� (��� ���������� �������� �������)
	/// </summary>
public:
	int Frames;
	/// <summary>
	/// ������ ��� �������� ������ �������������� ����� ��������, ��� �������� ������������ �� �������
	/// </summary>
	sf::IntRect FrameRect;
	/// <summary>
	/// ������ ��� �������� �������� �������� ��������������� �������� (����� � �������)
	/// </summary>
	float Speed;
};


// �����, �������������� �������� ���������� ��������� ����������� ��������� ��������
// ����� ���� ����������� � �������� ���������� ������������ ��������
class AnimationExplorer : public sf::Drawable, public sf::Transformable{
public:
	typedef std::map<std::string, Animation>::iterator iter;
	AnimationExplorer(): _stop(false){}
	AnimationExplorer(const AnimationExplorer& t):_container(t._container), _stop(t._stop){
		iter f = _container.find(*t._cname);
		_current = &f->second;
		_cname = &f->first;
	}
	void AddAnimation(const std::string& name, const Animation& anim, bool setCurrent=false){
		_container.insert(std::pair<std::string, Animation>(name, anim));
		if(setCurrent){
			_current = &_container.begin()->second;
			_cname = &_container.begin()->first;
		}
	}
	void EraseAnimation(const std::string& name){
		iter found = _container.find(name);
		if(found != _container.end()){
			if(_current == &found->second) {
				_current = (_container.size() != 0 ? &_container.begin()->second : 0 );
				_cname = (_container.size() != 0 ? &_container.begin()->first : 0);
			}
			_container.erase(found);
		}
	}

	void SwitchAnimation(){

	}

	void Switch(){
		_stop ^= true;
	}
	void Stop(){
		_stop = true;
	}
	void Start(){
		_stop = false;
	}
	bool IsStopped()const{
		return _stop;
	}

	void Update(float dt){
		if(_stop) return;
		_current->Update(dt);
	}

	virtual sf::FloatRect getGlobalBounds()const{
		return _current->getGlobalBounds();
	}
	virtual void move(const sf::Vector2f& mov){
		_current->move(mov);
	}
	virtual void move(float x, float y){
		_current->move(x,y);
	}
	virtual void setPosition(float x, float y){
		_current->setPosition(x, y);
	}
	virtual void setPosition(const sf::Vector2f& pos){
		_current->setPosition(pos);
	}
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states)const{
		target.draw(*_current,states);
	}
	bool Intersect(const sf::FloatRect& rect)const{
		return _current->Intersect(rect);
	}
	bool ChangeAnimation(const char* state){
		if(*_cname == state) return false;
		iter f = _container.find(state);
		if(f == _container.end()) return false;
		Animation* temp = _current;
		_current = &f->second;
		_current->setPosition(temp->getPosition());
		_cname = &f->first;
		return true;
	}
	virtual sf::Vector2f getPosition()const{
		return _current->getPosition();
	}
private:
	bool _stop;
	std::map<std::string, Animation> _container;
	Animation* _current;
	const std::string *_cname;
};

// transformable rectangle (adjusted SFML Rect functionality)
class MyRect{
public:
	MyRect(): top(0), left(0), width(0), height(0){
	}
	MyRect(int Left, int Top, int Width, int Height) :  top(Top), left(Left), width(Width), height(Height){

	}
	MyRect(const sf::IntRect& r): top(r.top), left(r.left), width(r.width), height(r.height){
	}
	operator sf::IntRect(){
		return sf::IntRect(left, top, width, height);
	}
	void Move(int dx, int dy){
		top += dy;
		left += dx;
	}
	MyRect& operator = (const sf::IntRect& r){
		top = r.top;
		left = r.left;
		width = r.width;
		height = r.height;
		return *this;
	}
	int top, left, width, height;
};


class Background: public sf::Drawable{
public:
	// class representing a configuration of a background level
	class LayerConfig{
	public:
		LayerConfig(){}
		LayerConfig(int lvl, bool stat=false):Level(lvl), Static(stat){}
		bool operator < (const LayerConfig& right)const{
			return Level < right.Level;
		}
		bool operator == (const LayerConfig& right)const{ return Level == right.Level;}
		bool operator > (const LayerConfig& right)const{
			return Level > right.Level;
		}
		int Level;
		bool Static;
	};
	// very very long typenames typedef
	typedef std::map<LayerConfig, sf::Sprite**>::const_reverse_iterator _riter;
	typedef std::map<LayerConfig, sf::Sprite**>::const_iterator _iter;
	typedef std::map<LayerConfig, sf::Sprite**> _map;
	Background(){}

	// 
	Background(const sf::Window& display, const sf::Sprite* mainspr, float speed, float speedFactor = 1.0f, bool stretchx=true, bool stretchy = true, bool isstatic=false) :
		_speed(speed), _speedFactor(speedFactor), _winsize(display.getSize()), _dt(0){
			sf::Sprite **tmps = new sf::Sprite*[2]; if(!isstatic) tmps[1] = new sf::Sprite(*mainspr);
			tmps[0] = new sf::Sprite(*mainspr);
			tmps[0]->setPosition(0,_winsize.y);

			// stretching the image
			if(stretchx) {
				tmps[0]->scale(_winsize.x/tmps[0]->getGlobalBounds().width*1.01f, 1);
				if(!isstatic) tmps[1]->scale(_winsize.x/tmps[1]->getGlobalBounds().width*1.01f, 1);
			}
			if(stretchy){
				tmps[0]->scale(1, _winsize.y/tmps[0]->getGlobalBounds().height);
				if(!isstatic) tmps[1]->scale(1, _winsize.y/tmps[1]->getGlobalBounds().height);
			}

			if(!isstatic) tmps[1]->setPosition(-Sign(speed)*tmps[0]->getGlobalBounds().width,_winsize.y-tmps[1]->getGlobalBounds().height);
			tmps[0]->setPosition(0,_winsize.y-tmps[0]->getGlobalBounds().height);
			_images.insert(std::pair<LayerConfig, sf::Sprite**>(LayerConfig(1, isstatic), tmps));
			_stop = false;
	}
	Background(const Background& t) : _images(t._images), _dt(t._dt), _speed(t._speed), _speedFactor(t._speedFactor), _winsize(t._winsize)
	{
		_adj = 0.0f;
		_stop = t._stop;
	}
	void AdjustSpeed(float dv){
		_speed += dv;
		_adj += dv;
	}
	void ResetSpeed(){
		_speed -= _adj;
		_adj=0.0f;
	}
	void SetSpeed(float spd){
		_speed = spd;
	}
	float GetSpeed()const{ return _speed;}
	// set how speed changes via bg level shift
	void SetSpeedFactor(float spdf){
		_speedFactor = spdf;
	}
	void Start(){
		_stop = false;
	}
	void Stop(){
		_stop = true;
	}
	// add a new bg (if you try to set an existing bg level, it won't work)
	void AddBackgroungImage(const sf::Sprite* sprite, int level, bool stretchx = true, bool stretchy = true, bool isstatic = false){
		sf::Sprite **tmps = new sf::Sprite*[2]; 
		if(!isstatic) tmps[1] = new sf::Sprite(*sprite);
		tmps[0] = new sf::Sprite(*sprite);
		if(stretchx) {
			tmps[0]->scale(_winsize.x/tmps[0]->getGlobalBounds().width*1.01f, 1);
			if(!isstatic) tmps[1]->scale(_winsize.x/tmps[1]->getGlobalBounds().width*1.01f, 1);
		}
		if(stretchy){
			tmps[0]->scale(1, _winsize.y/tmps[0]->getGlobalBounds().height);
			if(!isstatic) tmps[1]->scale(1, _winsize.y/tmps[1]->getGlobalBounds().height);
		}
		if(!isstatic) tmps[1]->setPosition(-Sign(_speed)*tmps[0]->getGlobalBounds().width,_winsize.y-tmps[1]->getGlobalBounds().height);
		tmps[0]->setPosition(0,_winsize.y-tmps[0]->getGlobalBounds().height);
		_images.insert(std::pair<LayerConfig, sf::Sprite**>(LayerConfig(level, isstatic), tmps));
	}

	// update a bg
	void Update(float dt){
		if(_stop) return;
		_dt += dt;
		for(_iter i = _images.begin(); i != _images.end(); i++){
			if(i->first.Static) continue;
			switch(Sign(_speed)){
			case 1:{
				for(int j=0; j<2; j++){
					if(i->second[j]->getPosition().x >= _winsize.x)
						i->second[j]->move(-2*i->second[j]->getGlobalBounds().width, 0);
					i->second[j]->move(_speed*dt*std::pow(_speedFactor,i->first.Level-1), 0);
				}
				break;
				   }
			case -1:{
				for(int j=0; j<2; j++){
					if(i->second[j]->getPosition().x <= -i->second[j]->getGlobalBounds().width)
						i->second[j]->move(2*i->second[j]->getGlobalBounds().width, 0);
					i->second[j]->move(_speed*dt*std::pow(_speedFactor,i->first.Level-1), 0);
				}
				break;
					}
			}
		}
	}
	// draw to the screen
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const{
		for(_riter i = _images.rbegin(); i != _images.rend(); i++){
			target.draw(*i->second[0], states);
			if(!i->first.Static) target.draw(*i->second[1], states);
		}

	}
	// destroy everything
	~Background(){
		for(_iter i = _images.begin(); i != _images.end(); i++)
			delete []i->second;		
	}
	sf::Vector2u GetWindowSize()const{
		return _winsize;
	}
private:
	// time
	float _dt;
	float _adj;	// accumulated additional speed
	sf::Vector2u _winsize;
	_map _images;
	float _speed;
	// how speed changes via level shift
	float _speedFactor;
	bool _stop;
};

#endif /* ANIMATION_H */
