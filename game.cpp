// game.cpp: определяет точку входа для консольного приложения.
//

#include "misc.h"
#include "private.h"
#include "gamehud.h"
#include "animation.h"

using namespace std;
string TITLE = "Infinite Run Game v1.0";							// window title



int main()
{
	srand(time(0));
	// windows and resources
	// main resource manager
	ResourceManager *res = new ResourceManager("Resource/script.res");
	// obstacles storage
	ResourceManager *obstacles = new ResourceManager("Resource/obstacles.res");
	// buttons storage
	ResourceManager *btns = new ResourceManager("Resource/buttons.res");
	// main sfml window
	sf::RenderWindow window(sf::VideoMode(1280,720), TITLE);
	// drawable textlines
	sf::Text* fnt = res->getText("score");
	sf::Text* over = res->getText("gameover");
	sf::Text* pak = res->getText("pak");
	// all other stuff
	Animation anim(res->getSprite("actor"),sf::IntRect(0,0,32,32),false,3, 5.0f, true);
	anim.scale(3,3);
	AnimationExplorer expl;
	expl.AddAnimation("run", anim, true);
	Background back(window, res->getSprite("background1"), -RUN_SPEED ,0.1f,true, false);
	Background back2(window,res->getSprite("background_menu"), 1, 1, true, true, true);		// menu background img
	back.AddBackgroungImage(res->getSprite("background2"),2);
	Game game(&window);
	game.AddActor(Creature(expl,sf::Vector2f(50,610)), window.getSize().y - 30.0f);
	game.AddBg(back);
	game.AddObstacleResPtr(obstacles);
	game.ConstructHud(btns);
	game.SetFont(fnt);
	sf::Event e;
	sf::Mouse mouse;
	sf::Clock clk;
	//game.LaunchRendering();
	bool f = false;
	float t=0;
	while(window.isOpen()){
		f = window.pollEvent(e);
		if(f){
			if(e.type == sf::Event::Closed){
				window.close();
			}
		}
		game.Update(clk.getElapsedTime().asSeconds()*UPDATE_SPEED,e, mouse);
		window.clear();
		if(game.GetState() == Game::PLAY ) game.__render();
		if(game.GetState() == Game::MENU ) {
			window.draw(back2);
			game.__render_menu();
		}
		if(game.IsStopped() && game.GetState() != Game::MENU){
			window.draw(*over);
			window.draw(*pak);
		}
		window.display();
		clk.restart();
	}
	FILE* hs;
	if((hs = std::fopen("Resource//Hs//score.sc","wb")) != NULL){
		fwrite(&game.Highscore, sizeof(int), 1, hs);
		fclose(hs);
	}
	//game.StopRendering();
	return 0;
}

