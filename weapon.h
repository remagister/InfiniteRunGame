
#ifndef WEAPONS_H
#define WEAPONS_H

#include "private.h"
#include <ctime>

namespace ItemStates{
	// list of different elemental types
	enum ElementType {USUAL, ACID, ELEC, FIRE, EXPL};
	// weapon types
	enum WeaponType {PIST, SMG, AR, SNIPE};
	// rarity of any item
	enum Rarity {WHITE, GREEN, BLUE, VIOLET, PURPLE, GOLDEN, CYAN};
	// weapon bonuses
	enum Buff {NONE, ACC, CRIT, DMG, ELEMCHANCE};
	// item type
	enum ItemType{ANY, WEAPON, CLOTH};
	// list of clothing types
	enum ClothingType {BOOTS, LEGS, JACK, HELM};
}

float RandomFloat(float min, float max){
	return min + static_cast <float> (std::rand()) /( static_cast <float> (RAND_MAX/(max-min)));
}

class Bullet: public sf::Drawable{
	Bullet(){
	}
	Bullet(const sf::Sprite& sprite, const sf::Vector2f &origin, const sf::Vector2f &dir) : _movement(dir){
		_sprite = new sf::Sprite(sprite);
		_sprite->setPosition(origin);
	}
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states){
		target.draw(*_sprite,states);
	}
	void Update(float dt){
		_sprite->move(_movement*dt);
		_movement += _accel*dt;
	}
	
	float _damage;
	ItemStates::ElementType _element;
	float _additional_damage;
private:

	sf::Sprite* _sprite;
	sf::Vector2f _movement;
	sf::Vector2f _accel;
};

class BackpackItem: public sf::Drawable{
public:
	BackpackItem(){}
	ItemStates::Rarity ItemRarity;
	sf::Sprite *sprite;
	static BackpackItem* GenerateItem(ItemStates::Rarity rarity, ResourceManager& manager, ItemStates::ItemType itype);

	virtual std::string GetName() const = 0;
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states){
		target.draw(*sprite, states);
	}
};

class Weapon : public BackpackItem{
public:
	Weapon()
	{
			
	}
	static Weapon* Generate(ItemStates::WeaponType type, ItemStates::Rarity rar, ResourceManager& manager){

	}
	virtual std::string GetName(){
		return prefix + name;
	}
	virtual void draw(sf::RenderTarget& t, sf::RenderStates s){
		t.draw(worldSprite, s);
	}
	float Damage;
	float Critical_chance;
	float Additional;
	std::string name;
	std::string prefix;
	float Accuracy;
	sf::Sprite worldSprite;
	ItemStates::ElementType DamageType;
	ItemStates::WeaponType Type;
	ItemStates::Buff BuffType;
};


class Clothing : public BackpackItem{
public:
	Clothing(){}
	
};



class RandomizeItemValues{
public:
	RandomizeItemValues(){}
	RandomizeItemValues(ItemStates::Rarity rarity, ItemStates::ItemType) : elemented(false){
		std::srand(std::time(0));
		rar = rarity;
		switch(rar){
		case ItemStates::WHITE:
			koeff1 = RandomFloat(1.0f, 1.5f);
			break;
		case ItemStates::GREEN:
			koeff1 = RandomFloat(1.2f, 2.0f);
			if(std::rand()%100 > 75) elemented = true;
			break;
		case ItemStates::BLUE:
			koeff1 = RandomFloat(1.5f, 2.5f);
			elemented = std::rand()%2;
			break;
		case ItemStates::VIOLET:
			koeff1 = RandomFloat(2.0f, 3.5f);
			if(std::rand()%100 > 25) elemented = true;
			break;
		case ItemStates::PURPLE:
			koeff1 = RandomFloat(2.5f, 4.5f);
			elemented = true;
			break;
		}
	}
	float getDamage()const{
		
	}
	float getCritical()const{

	}
	ItemStates::Rarity rar;
	ItemStates::ItemType type;
private:
	float koeff1;	// first koefficient, based on rarity
	float prop_min;	// minimal val
	float prop_range;
	bool elemented;
};

// Generetes randomly an item
inline BackpackItem* BackpackItem::GenerateItem(ItemStates::Rarity rarity, ResourceManager& res, ItemStates::ItemType itype = ItemStates::ANY){
	std::srand(std::time(0));
	switch(itype){
	case ItemStates::ANY:{
			itype = static_cast<ItemStates::ItemType>(std::rand()%2+1);
			if(itype == 1) goto WEAPON;
			if(itype == 2) goto CLOTH;
			break;
		}
	case ItemStates::WEAPON:{
WEAPON:
		BackpackItem *ret;
		
							}
	case ItemStates::CLOTH:{
		CLOTH:;
						   }
	}
}


class Backpack : public sf::Drawable{
public:
	typedef std::vector<BackpackItem>::const_iterator iter; 
	Backpack(){}
	
	virtual void draw(sf::RenderTarget &target, sf::RenderStates state)const{
		for(iter i=items.begin(); i!= items.end(); i++){
			target.draw(*i, state);
		}
	}
private:
	std::vector<BackpackItem> items;
};

#endif // WEAPONS_H