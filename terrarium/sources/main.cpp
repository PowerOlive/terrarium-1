/*
 * main.cpp
 *
 *  Created on: Apr 10, 2013
 *      Author: felipe
 */

#include <cstdlib>
#include "../headers/engine.hpp"
#include "../headers/main_menu.hpp"
#include "../headers/file_parser.hpp"

using std::cout;
using std::endl;

int main(int argc, char** argv)
{
	try
	{
		Engine::initialize();
		Engine::display = new Engine::Display(640, 480, "Project Terrarium");

//		cout << "Lets test the directory listing method" << endl;
//		list<String> dirs = Engine::getFilenamesWithinDirectory("./resources/maps");
//		for(list<String>::iterator it = dirs.begin(); it != dirs.end() ; ++it)
//			cout << *it << endl;

		Image loading_image("./resources/loading.png");
		loading_image.draw();
		Engine::display->refresh();

		MainMenu menu;
		menu.show();

		delete Engine::display;
		Engine::finalize();
	}
	catch(Exception& e)
	{
		cout << e.message() << endl;
	}
	return EXIT_SUCCESS;
}