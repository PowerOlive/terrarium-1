/*
 * game.cpp
 *
 *  Created on: 11/05/2013
 *      Author: felipe
 */

#include "game.hpp"

#include <iostream>
using std::cout; using std::endl;

#include "fgeal.hpp"
#include "block.hpp"
#include "entity.hpp"
#include "map.hpp"
#include "menu.hpp"

using Physics::World;
using Physics::Vector;

using GameEngine::EventQueue;
using GameEngine::Font;
using GameEngine::Color;

struct GameStuff
{
	vector<Entity*> entities;
	vector<Image*> images;

	Rect visibleArea;

	Entity* player;
	Map* game_map;
	Image* tileset_dirt;
	Font* font;
	Menu* inGameMenu;
	EventQueue* eventQueue;
	World* world;

	bool running, jumping, inGameMenuShowing;
	bool isKeyUpPressed, isKeyDownPressed, isKeyRightPressed, isKeyLeftPressed;

	GameStuff(const string& map_path)
	{
		visibleArea = Rect(0,0,640,480);

		//loading font
		font = new GameEngine::Font("resources/liberation.ttf", 14);

		//loading ingame menu
		inGameMenu = new Menu(Rect(200, 200, 200, 64), font, Color::ORANGE);
		inGameMenu->addEntry("Resume");
		inGameMenu->addEntry("Save and exit");
		inGameMenu->addEntry("Exit without saving");

		//loading event queue
		eventQueue = new GameEngine::EventQueue();

		//setting flags
		running = true;
		jumping = false;
		inGameMenuShowing = false;
		isKeyUpPressed = false;
		isKeyDownPressed = false;
		isKeyRightPressed = false;
		isKeyLeftPressed = false;

		//loading world
		Vector gravity(0.0f, 10.0f);
		world = new World(gravity);

		//loading map
		game_map = Map::loadMapFromFile(map_path, world, images);
		game_map->visibleArea = &visibleArea;

		//loading player graphics
		Image* player_img = new Image("resources/pijamaman-1.png");
		AnimationSet* anim = new AnimationSet(player_img);
		anim->add("still-left", 56, 84, 1, 1);
		anim->add("still-right", 56, 84, 1, 1);
		anim->add("walk-left", 56, 84, 0.1, 4);
		anim->add("walk-right", 56, 84, 0.1, 4);
		anim->setCurrent("still-right");

		//loading player physics
		Body* b = new Body(1,1,Math::convertToMeters(16), Math::convertToMeters(80));
		player = new Entity(anim, b, &visibleArea);
		world->addBody(player->body);
		player->body->setFixedRotation();

		//loading dirt tileset
		tileset_dirt = new Image("resources/tileset-dirt.png");
		cout << "map size (in pixels): " << game_map->computeDimensions().w << "x" << game_map->computeDimensions().h << endl;
	}

	~GameStuff()
	{
		cout << "game stuff destructor..." << endl;
		foreach(Entity*, e, vector<Entity*>, entities)
		{
			delete e;
		}

		foreach(Image*, img, vector<Image*>, images)
		{
			delete img;
		}

		delete player;
		delete game_map;
		delete tileset_dirt;
		delete font;
		delete inGameMenu;
		delete eventQueue;
		delete world;
	}

	void start()
	{
		while(running)
		{
			//visible area quick n' dirt fix
			{
				visibleArea.x = Math::convertToPixels(player->body->getX()) - visibleArea.w/2.0;
				visibleArea.y = Math::convertToPixels(player->body->getY()) - visibleArea.h/2.0;
				if(visibleArea.x + visibleArea.w > game_map->computeDimensions().w)
					visibleArea.x = game_map->computeDimensions().w - visibleArea.w;
				if(visibleArea.x < 0)
					visibleArea.x = 0;
				if(visibleArea.y + visibleArea.h > game_map->computeDimensions().h)
					visibleArea.y = game_map->computeDimensions().h - visibleArea.h;
				if(visibleArea.y < 0)
					visibleArea.y = 0;
			}

			handleInput();
			drawScene();
		}
	}

	void handleInput()
	{

		if(not inGameMenuShowing and isKeyUpPressed and !jumping) {
			Vector v = player->body->getVelocity();
			if(v.y >= -2.0f) {
				player->body->applyImpulse(Vector(0.0f, -0.05f), Vector(player->body->getX(), player->body->getY()));
				jumping = true;
			}
		}
		if(not inGameMenuShowing and isKeyDownPressed) {
			Vector v = player->body->getVelocity();
			if(v.y <= 2.0f)
				player->body->applyForceToCenter(Vector(0.0f, 0.2f));
		}
		if(not inGameMenuShowing and isKeyRightPressed) {
			player->animation->setCurrent("walk-right");
			Vector v = player->body->getVelocity();
			if(v.x <= 2.0f)
				player->body->applyForceToCenter(Vector(0.2f, 0.0f));
		}
		if(not inGameMenuShowing and isKeyLeftPressed) {
			player->animation->setCurrent("walk-left");
			Vector v = player->body->getVelocity();
			if(v.x >= -2.0f)
				player->body->applyForceToCenter(Vector(-0.2f, 0.0f));
		}

		while(! eventQueue->isEmpty() )
		{
			GameEngine::Event* ev = eventQueue->waitForEvent();

			if(ev->getEventType() == GameEngine::Event::Type::DISPLAY_CLOSURE)
			{
				running=false;
			}
			else if(ev->getEventType() == GameEngine::Event::Type::KEY_RELEASE)
			{
				switch(ev->getEventKeyCode())
				{
				case GameEngine::Event::Key::ARROW_UP:
					isKeyUpPressed = false;
					jumping = false;
					break;
				case GameEngine::Event::Key::ARROW_DOWN:
					isKeyDownPressed = false;
					break;
				case GameEngine::Event::Key::ARROW_RIGHT:
					player->animation->setCurrent("still-right");
					isKeyRightPressed = false;
					break;
				case GameEngine::Event::Key::ARROW_LEFT:
					player->animation->setCurrent("still-left");
					isKeyLeftPressed = false;
					break;
				default:
					break;
				}
			}
			else if(ev->getEventType() == GameEngine::Event::Type::KEY_PRESS)
			{
				switch(ev->getEventKeyCode())
				{
				case GameEngine::Event::Key::ARROW_UP:
					isKeyUpPressed = true;
					if(inGameMenuShowing)
						--*inGameMenu;
					break;
				case GameEngine::Event::Key::ARROW_DOWN:
					isKeyDownPressed = true;
					if(inGameMenuShowing)
						++*inGameMenu;
					break;
				case GameEngine::Event::Key::ARROW_RIGHT:
					isKeyRightPressed = true;
					break;
				case GameEngine::Event::Key::ARROW_LEFT:
					isKeyLeftPressed = true;
					break;
				case GameEngine::Event::Key::ESCAPE:
					if(inGameMenuShowing)
						inGameMenuShowing=false;
					else
						inGameMenuShowing=true;
					break;
				case GameEngine::Event::Key::ENTER:
					if(inGameMenuShowing)
					{
						switch(inGameMenu->selectedIndex)
						{
							case 0:
								inGameMenuShowing=false;
								break;
							case 1:
								Map::saveRawMapToFile(string("resources/maps/saved_map.txt"), game_map);
								running=false;
								break;
							case 2:
								running=false;
								break;
						}
					}
					break;
				default:
					break;
				}
			}
			else if(ev->getEventType() == GameEngine::Event::Type::MOUSE_BUTTON_PRESS)
			{

				if(ev->getEventMouseButton() == GameEngine::Event::MouseButton::RIGHT)
				{
					unsigned int mx = (visibleArea.x + ev->getEventMouseX())/BLOCK_SIZE;
					unsigned int my = (visibleArea.y + ev->getEventMouseY())/BLOCK_SIZE;

					if(mx < game_map->grid.capacity() && my < game_map->grid[0].capacity()) // in case you click outside the map
						if (game_map->grid[mx][my] == NULL)
						{
							game_map->grid[mx][my] = new Block(tileset_dirt, mx, my, 1);
							world->addBody(game_map->grid[mx][my]->body);
							game_map->retile(game_map->grid[mx][my]);
						}

				}
				else if (ev->getEventMouseButton() == GameEngine::Event::MouseButton::LEFT)
				{
					unsigned int mx = (visibleArea.x + ev->getEventMouseX())/BLOCK_SIZE;
					unsigned int my = (visibleArea.y + ev->getEventMouseY())/BLOCK_SIZE;

					if(mx < 0) mx = 0; // safety
					if(my < 0) my = 0; // safety
					if(mx < game_map->grid.capacity() && my < game_map->grid[0].capacity()) // in case you click outside the map
						if (game_map->grid[mx][my] != NULL)
						{
							world->destroyBody(game_map->grid[mx][my]->body);
							delete game_map->grid[mx][my];
							game_map->grid[mx][my] = NULL;
							game_map->retileNeighbourhood(mx, my);
						}
				}
			}
		}

	}

	void drawScene()
	{
		GameEngine::display->clear();
		world->step((1.0f / 60.0f), 6, 2);
		/* needs to draw HUD */

		game_map->draw_bg_player();

		player->draw(); //draw player

		game_map->draw_fg_player();

		/* drawing others entities */
		for(vector<Entity*>::iterator it = entities.begin() ; it != entities.end(); ++it){
			if (*it != NULL) (*it)->draw();
		}

		/* later should be a character class */

		//DEBUG!!!
		drawDebug();

		if(inGameMenuShowing)
			inGameMenu->draw();


		GameEngine::display->refresh();
	}


	void drawDebug()
	{
		font->draw_text("## DEBUG BUILD ##", 245, 0, GameEngine::Color::RED);

		font->draw_text("POSITION", 0, 14, GameEngine::Color::WHITE);

		font->draw_text(string("x: ")+player->body->getX()+" y:"+player->body->getY(), 0, 28, GameEngine::Color::WHITE);

		font->draw_text("SPEED", 0, 42, GameEngine::Color::WHITE);

		font->draw_text(string("x: ")+player->body->getVelocity().x+" y: "+player->body->getVelocity().y, 0, 56, GameEngine::Color::WHITE);

	}
};

//===========================================================================================
GameStuff* stuff;
Game::Game(const string& map_path)
{
	stuff = new GameStuff(map_path);
}

Game::~Game()
{
	delete stuff;
}

void Game::start()
{
	stuff->start();
}

