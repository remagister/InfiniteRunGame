
#pragma once

#ifndef INCLUDES_H
#include "includes.h"
#endif // includes_h
#ifndef EVENT_H
#include "event.h"
#endif // event.h
#ifndef GAMEHUD_H
#include "gamehud.h"
#endif // gamehud

// all existing game/hud/sfml elements
enum Element {UNDEF, IMAGE=1, TEXTURE, SPRITE, FONT, TEXT, BUTTON, MUSIC, SOUND, COLOR, WINDOW, RESOURCE, INFOBAR};
// list of all Element members in std::string's
std::string elems[] = {"IMAGE", "TEXTURE", "SPRITE", "FONT", "TEXT" , "BUTTON", "MUSIC", "SOUND", "COLOR", "WINDOW", "RESOURCE", "INFOBAR"}; 
const int elsize = 12;	// size of std::string elems[]


// helping class for downloading different objects from special external scripts
class ResourceManager
{
public:
	// empty resource manager
	// don't try to declare with this
	ResourceManager(){}
	// resource manager constructed from script
	ResourceManager(const char* path){
		_path = path;
		std::ifstream file(path);
		if(file.is_open()){
			_read(file);
		}
		else{
			std::cout << "Cannot open file \"" << path << "\"\n";
		}
	}
	
	void ChangeScript(const char* newpath){
		if(!newpath) return;
		_path = newpath;
		std::ifstream file(newpath);
		if(file.is_open()){
			_read(file);
		}
		else{
			std::cout << "Cannot open file \"" << _path << "\"\n";
		}
	}

	sf::Image* getImage(const char* name){
		What f(IMAGE, name);
		int fin = _find_repeat(f);
		if(fin != -1){
			return (sf::Image*)_container[fin];
		}
		_ctype::iterator iter = _pairs.find(f);
		sf::Image* ret=0;
		if(iter == _pairs.end()){
			for(int i=0; i<_res.size(); i++){
				ret = _res[i]->getImage(name);
				if(!ret)continue;
			}
			return ret;
		}
		ret = new sf::Image;
		ret->loadFromFile(_relatedpath(iter->second.getString("filename=").c_str()));
		_container.push_back((void*)ret);
		_rep_container.push_back(f);
		return ret;
	}
	sf::Texture* getTexure(const char* name){
		What f(TEXTURE, name);
		int fin = _find_repeat(f);
		if(fin != -1){
			return (sf::Texture*)_container[fin];
		}
		_ctype::iterator iter = _pairs.find(f);
		sf::Texture* ret=0;
		if(iter == _pairs.end()){
			for(int i=0; i<_res.size(); i++){
				ret = _res[i]->getTexure(name);
				if(!ret)continue;
			}
			return ret;
		}
		sf::Image *im;
		float *pos = iter->second.getValue("rect=", 4);
		std::string file = _relatedpath(iter->second.getString("filename=").c_str());
		if(file.empty()){
			file = iter->second.getString("image=");
			_ctype::iterator img = _pairs.find(What(IMAGE, file));
			if(img == _pairs.end()) return 0;
			else{
				im = getImage(img->first.name.c_str());
				ret = new sf::Texture;
				ret->loadFromImage(*im,(pos? sf::IntRect(pos[0],pos[1],pos[2],pos[3]) : sf::IntRect(0,0,32,32)));
			}
		}
		else{
			ret = new sf::Texture;
			ret->loadFromFile(file, (pos? sf::IntRect(pos[0],pos[1],pos[2],pos[3]) : sf::IntRect(0,0,32,32)));
		}
		std::string Bool = iter->second.getString("smooth=");
		if(!Bool.empty()) ret->setSmooth(stoi(Bool));
		Bool = iter->second.getString("repeat=");
		if(!Bool.empty()) ret->setRepeated(stoi(Bool));
		_container.push_back((void*)ret);
		_rep_container.push_back(f);
		return ret;
	}
	sf::Sprite* getSprite(const char* name){
		What f(SPRITE, name);
		int fin = _find_repeat(f);
		if(fin != -1){
			return (sf::Sprite*)_container[fin];
		}
		_ctype::iterator iter = _pairs.find(f);
		sf::Sprite* ret=0;
		if(iter == _pairs.end()){
			for(int i=0; i<_res.size(); i++){
				ret = _res[i]->getSprite(name);
				if(!ret)continue;
			}
			return ret;
		}
		std::string tex = iter->second.getString("texture=");
		if(!tex.empty()){
			_ctype::iterator texi = _pairs.find(What(TEXTURE, tex));
			if(texi != _pairs.end()){
				float *r = texi->second.getValue("rect=",4);
				if(!r){
					r = iter->second.getValue("rect=", 4);
				}
				sf::Texture* t = getTexure(texi->first.name.c_str());
				if(!t) return 0;
				ret = (r ? new sf::Sprite(*t, sf::IntRect(r[0],r[1],r[2],r[3])) : new sf::Sprite(*t));
				if(r) delete []r;
			}
			else return 0;
		}
		else{
			sf::Texture *t = new sf::Texture;
			float *r = iter->second.getValue("rect=", 4);
			if (!r) return 0;
			t->loadFromFile(_relatedpath(iter->second.getString("filename=").c_str()), sf::IntRect(r[0],r[1],r[2],r[3]));
			ret = new sf::Sprite(*t);
		}
		float *pos = iter->second.getValue("position=", 2);
		if(pos){
			ret->setPosition(pos[0],pos[1]);
			delete []pos;
		}
		pos = iter->second.getValue("scale=",2);
		if(pos){
			ret->setScale(pos[0],pos[1]);
			delete []pos;
		}
		float rot = iter->second.getValue("rotation=");
		if(rot) ret->setRotation(rot);
		_container.push_back((void*)ret);
		_rep_container.push_back(f);
		return ret;
	}
	sf::Color getColor(const char* name){
		What f(COLOR, name);
		_ctype::iterator iter = _pairs.find(f);
		if(iter == _pairs.end()) return sf::Color(255,255,255,255);
		float *cols = iter->second.getValue("color=", 4);
		return (cols? sf::Color(cols[0],cols[1],cols[2],cols[3]) : sf::Color(255,255,255,255));
	}
	Styles::Align _getAlign(const std::string& name){
		if(name == "left") return Styles::LEFT;
		if(name == "right") return Styles::RIGHT;
		if(name == "center") return Styles::CENTER;
		return Styles::NOALIGN;
	}
	Styles::Size _getSize(const std::string& name){
		if(name == "tiny") return Styles::TINY;
		if(name == "small") return Styles::SMALL;
		if(name == "normal") return Styles::NORMAL;
		if(name == "big") return Styles::BIG;
		if(name == "huge") return Styles::HUGE;
		if(name == "enormous") return Styles::ENORMOUS;
		return Styles::NOSIZE;
	}
	sf::Font* getFont(const char* name){
		What f(FONT,name);
		int fin = _find_repeat(f);
		if(fin != -1){
			return (sf::Font*)_container[fin];
		}
		_ctype::iterator iter = _pairs.find(f);
		sf::Font * ret=0;
		if(iter == _pairs.end()){
			for(int i=0; i<_res.size(); i++){
				ret = _res[i]->getFont(name);
				if(!ret)continue;
			}
			return ret;
		}
		ret = new sf::Font;
		std::string filename = iter->second.getString("filename=");
		if(filename.empty()) return 0;
		ret->loadFromFile(_relatedpath(filename.c_str()));
		_container.push_back((void*)ret);
		_rep_container.push_back(f);
		return ret;
	}
	sf::Text* getText(const char* name){
		What f(TEXT, name);
		int fin = _find_repeat(f);
		if(fin != -1){
			return (sf::Text*)_container[fin];
		}
		_ctype::iterator iter = _pairs.find(f);
		sf::Text* ret=0;
		if(iter == _pairs.end()){
			for(int i=0; i<_res.size(); i++){
				ret = _res[i]->getText(name);
				if(!ret)continue;
			}
			return ret;
		}
		std::string str = iter->second.getString("font=");
		sf::Font * font;
		std::string *text;
		if(str.empty()){
			str = iter->second.getString("filename=");
			if(str.empty()) return 0;
			font = new sf::Font;
			font->loadFromFile(_relatedpath(str.c_str()));
		}
		else{
			_ctype::iterator i = _pairs.find(What(FONT, str.c_str()));
			if(i == _pairs.end()) return 0;
			font = getFont(str.c_str());
		}
		text = new std::string (iter->second.getString("text="));
		unsigned size = iter->second.getValue("size=");
		ret = new sf::Text (text->c_str(), *font, (size ? size : 30));
		float *arg = iter->second.getValue("position=",2);
		if(arg) {
			ret->setPosition(arg[0], arg[1]);
			delete []arg;
		}
		std::string col = iter->second.getString("color=");
		_ctype::iterator i;
		if (col.empty()) goto NOCOLOR;
		i = _pairs.find(What(COLOR, col));
		if(i != _pairs.end()){
			ret->setColor(getColor(col.c_str()));
		}
		else{
			arg = iter->second.getValue("color=", 4);
			if(arg){
				ret->setColor(sf::Color(arg[0],arg[1],arg[2],arg[3]));
				delete []arg;
			}
		}
NOCOLOR:
		arg = iter->second.getValue("scale=",2);
		if(arg){
			ret->setScale(arg[0],arg[1]);
			delete []arg;
		}
		_container.push_back((void*)ret);
		_rep_container.push_back(f);
		return ret;
	}
	sf::Music* getMusic(const char* name){
		What t(MUSIC, name);
		int fin = _find_repeat(t);
		if(fin != -1){
			return (sf::Music*)_container[fin];
		}
		_ctype::iterator iter = _pairs.find(t);
		sf::Music* ret=0;
		if(iter == _pairs.end()){
			for(int i=0; i<_res.size(); i++){
				ret = _res[i]->getMusic(name);
				if(!ret)continue;
			}
			return ret;
		}
		ret = new sf::Music;
		std::string file = _relatedpath(iter->second.getString("filename=").c_str());
		if(file.empty()) return 0;
		ret->openFromFile(file);
		float arg = iter->second.getValue("volume=");
		if(arg) ret->setVolume(arg);
		std::string loop = iter->second.getString("loop=");
		if(!loop.empty()){
			ret->setLoop(stoi(loop));
		}
		_container.push_back((void*)ret);
		_rep_container.push_back(t);
		return ret;
	}
	sf::Sound* getSound(const char* name){
		What f(SOUND, name);
		int fin = _find_repeat(f);
		if(fin != -1){
			return (sf::Sound*)_container[fin];
		}
		_ctype::iterator iter = _pairs.find(f);
		sf::Sound *ret=0;
		if(iter == _pairs.end()){
			for(int i=0; i<_res.size(); i++){
				ret = _res[i]->getSound(name);
				if(!ret)continue;
			}
			return ret;
		}
		sf::SoundBuffer b;
		std::string file = _relatedpath(iter->second.getString("filename=").c_str());
		if(file.empty()) return 0;
		b.loadFromFile(file);
		ret = new sf::Sound(b);
		float arg = iter->second.getValue("volume=");
		if(arg) ret->setVolume(arg);
		std::string loop = iter->second.getString("loop=");
		if(!loop.empty())
			ret->setLoop(stoi(loop));
		_container.push_back((void*)ret);
		_rep_container.push_back(f);
		return ret;
	}
	ResourceManager* _getResourceManager(const char* name){
		What t(RESOURCE, name);
		ResourceManager* ret=0;
		_ctype::iterator iter = _pairs.find(t);
		if(iter == _pairs.end()){
			for(int i=0; i<_res.size(); i++){
				ret = _res[i]->_getResourceManager(name);
				if(!ret)continue;
			}
			return ret;
		}
		std::string file = iter->second.getString("filename=");
		if(file.empty()) return 0;
		ret = new ResourceManager(_relatedpath(file.c_str()).c_str());
		return ret;
	}
	/*InfoBar* getInfoBar(const char* name){
		What t(INFOBAR, name);
		_ctype::iterator iter = _pairs.find(t);
		InfoBar* ret=0;
		if(iter == _pairs.end()){
			for(int i=0; i<_res.size(); i++){
				ret = _res[i]->getInfoBar(name);
				if(!ret)continue;
			}
			return ret;
		}
		float *r = iter->second.getValue("rect=", 4);
		if(!r) return 0;
		std::string bg = iter->second.getString("background=");
		_ctype::iterator spr = _pairs.find(What(SPRITE, bg));
		if(spr != _pairs.end()){
			sf::Sprite* bg = getSprite(spr->first.name.c_str());
			if(!bg) return 0;
			ret = new InfoBar(r[0],r[1],r[2],r[3],*bg);
		}
		else return 0;
		std::string *text;
		char b[10];
		for(int i=1;;i++){
			text = iter->second.getString((std::string("text")+itoa(i,b,10)).c_str(), 4);
			if(!text) break;
			sf::Text* t = getText(text[0].c_str());
			if(!t) {
				std::cout << "Cannot find " << text[0] << " in script\n";
				continue;	
			}
			Styles::Align a = _getAlign(text[1]);
			if(a == Styles::NOALIGN) continue;
			Styles::Size s = _getSize(text[2]);
			if(s == Styles::NOSIZE) continue;
			int str = stoi(text[3]);
			ret->AddLabel(t, a, s, str);
		}
		return ret;
	}*/

	// destroy everything
	~ResourceManager(){
		for(unsigned i=0; i<_res.size(); i++)
			delete _res[i];
		for(std::vector<void*>::iterator i= _container.begin(); i != _container.end(); i++)
			delete *i;
	}
	class What{
	public:
		What(Element t, const char* n){
			type = t;
			name = n;
			inherit = "";
		}
		What(const Element &t, const std::string& s){
			type = t;
			name = s;
			inherit = "";
		}
		What(Element t, const char* n, const char *in){
			type = t;
			name = n;
			inherit = in;
		}
		What(int i=0){
			type = UNDEF;
			name = "";
			inherit = "";
		}
		bool operator< (const What& t) const{
			return name < t.name;
		}
		bool Inherited()const{
			return !inherit.empty();
		}
		bool Empty()const{
			return (name.empty() && type == UNDEF);
		}
		void Nullify(){
			type = UNDEF;
			name.clear();
		}
		bool operator== (const What& t)const{
			return type == t.type && name == t.name;
		}
		Element type;
		std::string name;
		std::string inherit;
	};
	class Block{
	public:
		Block(){
		_inherited = 0;
		}
		Block(std::string &what){
			_inherited = 0;
			_s = what;
		}
		Block(const char* what){
			_s = what;
		}
		Block& operator+= (char c){
			_s += c;
			return *this;
		}
		Block& operator+= (const char* str){
			_s += str;
			return *this;
		}
		Block& operator+= (const std::string &t){
			_s += t;
			return *this;
		}
		bool Empty()const{
			return _s.empty() && !_inherited;
		}
		void Set(const char* str){
			_s = str;
		}
		const char* ToChar()const{
			return _s.c_str();
		}
		Block& operator= (const std::string & t){
			_s  = t;
		}
		Block& operator= (const char* str){
			_s = str;
		}
		bool operator== (const char* str)const{
			return _s == str;
		}
		bool operator== (std::string &str)const{
			return _s == str;
		}
		void Inherit(const Block *t){
			_inherited = t;
		}
		void Clear(){
			_s.clear();
			_inherited = 0;
		}
		std::string getString(const char* param)const{
			if(Empty()) return "";
			unsigned pos = _s.find(param);
			if(pos == _s.npos){
				return (_inherited ? _inherited->getString(param) : std::string());
			}
			std::string ret;
			pos = _s.find('\"', pos)+1;
			unsigned end = _s.find('\"',pos);
			ret = _s.substr(pos, end-pos);
			return ret;
		}
		std::string* getString(const char* param, unsigned amount)const{
			if(Empty()) return 0;
			unsigned beg = _s.find(param);
			if(beg == _s.npos){
				return (_inherited ? _inherited->getString(param, amount) : 0);
			}
			std::string * ret = new std::string[amount];
			beg = _s.find('\"', beg)+1;
			unsigned end = _s.find('\"', beg);
			std::string buf = _s.substr(beg, end-beg);
			const char* str = buf.c_str();
			for(int a=0; a<amount; a++){
				while(*str != ',' && *str){
					if(!isspace(*str)) ret[a] += *str;
					*str++;
				}
				if(*str) *str++;
			}
			return ret;
		}
		float getValue(const char* param)const{
			if(Empty()) return 0;
			unsigned beg = _s.find(param);
			if(beg == _s.npos){
				return (_inherited ? _inherited->getValue(param) : 0);
			}
			beg = _s.find('\"', beg)+1;
			unsigned end = _s.find('\"', beg);
			return atof(_s.substr(beg, end-beg).c_str());
		}
		float* getValue(const char* param, unsigned amount)const{
			if(Empty()) return 0;
			unsigned beg = _s.find(param);
			if(beg == _s.npos) {
				return (_inherited ? _inherited->getValue(param,amount) : 0);
			}
			float* ret = new float[amount];
			beg = _s.find('\"', beg)+1;
			unsigned end = _s.find('\"', beg);
			std::string buf = _s.substr(beg, end-beg);
			std::string val;
			const char* str = buf.c_str();
			for(int a=0; a<amount; a++){
				while(*str != ',' && *str){
					if(isdigit(*str)) val += *str;
					*str++;
				}
				if(*str) *str++;
				ret[a] = atof(val.c_str());
				val.clear();
			}
			return ret;
		}
	private:
		std::string _s;
		const Block* _inherited;
	};
private:
	void _read(std::ifstream &file){
		_res.clear();
		What w;
		Block block;
		bool open = false;
		while(!file.eof()){
			getline(file, _line);
			if(!open || w.Empty()){
				w = _findinit();
				if(!w.Empty()){
					open = true;
					getline(file,_line);
				}
			}
			if(open) block += _line;
			if(_line.find('}') != _line.npos){
				open = false;
				_pairs.insert(std::pair<What, Block>(w, block));
				w.Nullify();
				block.Clear();
			}
		}
		_alloc_inheritance();
	}

	void _alloc_inheritance(){
		_ctype::iterator iter = _pairs.begin();
		while(iter != _pairs.end()){
			if(iter->first.Inherited()){
				_ctype::iterator n = _pairs.find(What(iter->first.type,iter->first.inherit));
				if(n!= _pairs.end()) iter->second.Inherit(&n->second);
			}
			if(iter->first.type == RESOURCE){
				ResourceManager* n = _getResourceManager(iter->first.name.c_str());
					if(n) {
						_res.push_back(n);
						_pairs.insert(n->_pairs.begin(), n->_pairs.end());
					}
			}
			iter++;
		}
	}
	What _findinit(){
		std::string fword;
		const char* str = _line.c_str();
		while(isspace(*str)) str++;
		while(!isspace(*str)) fword+= *str++;
		bool flag = false;
		int a;
		for(a=0; a<elsize; a++){
			if(fword == elems[a]) {
				flag = true;
				break;
			}
		}
		if(flag){
			std::string sword;
			while(isspace(*str)) str++;
			while(*str != '{' && *str != '\n'){
				if(!isspace(*str)) sword += *str++;
				else str++;
				if(*str == ':'){
					*str++;
					std::string tword;
					while(isspace(*str)) str++;
					while(!isspace(*str) && *str != '{' && *str != '\n'){
						tword += *str++;
					}
					return  What(static_cast<Element>(a+1), sword.c_str(), tword.c_str());
					break;
				}
			}
			return What(static_cast<Element>(a+1), sword.c_str());
		}
		else return 0;
	}
	std::string _relatedpath(const char* oldpath){
		if(!strcmp(oldpath, "")) return "";
		unsigned p = _path.rfind('/');
		if(p == _path.npos) return oldpath;
		else return _path.substr(0, p+1)+oldpath;
	}

	int _find_repeat(const What& what){
		for(unsigned i=0; i<_rep_container.size(); i++){
			if(what == _rep_container[i]) return i;
		}
		return -1;
	}

	std::vector<What> _rep_container;
	std::vector<void*> _container;
	std::string _path;
	std::string _line;
	std::multimap <What, Block> _pairs;
	std::vector <ResourceManager*> _res;
	typedef std::multimap<What, Block> _ctype;
};
