/************************************************************************
* Minetest-c55
* Copyright (C) 2010-2011 celeron55, Perttu Ahola <celeron55@gmail.com>
*
* main.cpp
* voxelands - 3d voxel world sandbox game
* Copyright (C) Lisa 'darkrose' Milne 2013-2014 <lisa@ltmnet.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
* See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>
*
* License updated from GPLv2 or later to GPLv3 or later by Lisa Milne
* for Voxelands.
************************************************************************/

#ifdef NDEBUG
	// Disable unit tests
	#define ENABLE_TESTS 0
#else
	// Enable unit tests
	#define ENABLE_TESTS 1
#endif

#ifdef _MSC_VER
	#pragma comment(lib, "Irrlicht.lib")
	//#pragma comment(lib, "jthread.lib")
	#pragma comment(lib, "zlibwapi.lib")
	#pragma comment(lib, "Shell32.lib")
	// This would get rid of the console window
	//#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

#include "irrlicht.h" // createDevice

#include "main.h"
#include "mainmenumanager.h"
#include <iostream>
#include <fstream>
#include <locale.h>
#include "common_irrlicht.h"
#include "debug.h"
#include "test.h"
#include "server.h"
#include "constants.h"
#include "porting.h"
#include "gettime.h"
#include "guiMessageMenu.h"
#include "filesys.h"
#include "config.h"
#include "guiMainMenu.h"
#include "mineral.h"
#include "game.h"
#include "keycode.h"
#include "tile.h"
#include "defaultsettings.h"
#include "gettext.h"
#include "settings.h"
#include "profiler.h"
#include "log.h"
// for the init functions
#include "content_craft.h"
#include "content_clothesitem.h"
#include "content_craftitem.h"
#include "content_toolitem.h"
#include "content_mob.h"
#include "path.h"
#include "gui_colours.h"
#include "character_creator.h"
#if USE_FREETYPE
#include "xCGUITTFont.h"
#endif

// This makes textures
ITextureSource *g_texturesource = NULL;

/*
	Settings.
	These are loaded from the config file.
*/
GameSettings main_settings;
GameSettings *g_settings = &main_settings;

// Global profiler
Profiler main_profiler;
Profiler *g_profiler = &main_profiler;

/*
	Random stuff
*/

/*
	mainmenumanager.h
*/

gui::IGUIEnvironment* guienv = NULL;
gui::IGUIStaticText *guiroot = NULL;
MainMenuManager g_menumgr;

bool noMenuActive()
{
	return (g_menumgr.menuCount() == 0);
}

// Passed to menus to allow disconnecting and exiting
MainGameCallback *g_gamecallback = NULL;

/*
	Debug streams
*/

// Connection
std::ostream *dout_con_ptr = &dummyout;
std::ostream *derr_con_ptr = &verbosestream;
//std::ostream *dout_con_ptr = &infostream;
//std::ostream *derr_con_ptr = &errorstream;

// Server
std::ostream *dout_server_ptr = &infostream;
std::ostream *derr_server_ptr = &errorstream;

// Client
std::ostream *dout_client_ptr = &infostream;
std::ostream *derr_client_ptr = &errorstream;

/*
	gettime.h implementation
*/

// A small helper class
class TimeGetter
{
public:
	virtual u32 getTime() = 0;
};

// A precise irrlicht one
class IrrlichtTimeGetter: public TimeGetter
{
public:
	IrrlichtTimeGetter(IrrlichtDevice *device):
		m_device(device)
	{}
	u32 getTime()
	{
		if(m_device == NULL)
			return 0;
		return m_device->getTimer()->getRealTime();
	}
private:
	IrrlichtDevice *m_device;
};
// Not so precise one which works without irrlicht
class SimpleTimeGetter: public TimeGetter
{
public:
	u32 getTime()
	{
		return porting::getTimeMs();
	}
};

// A pointer to a global instance of the time getter
// TODO: why?
TimeGetter *g_timegetter = NULL;

u32 getTimeMs()
{
	if(g_timegetter == NULL)
		return 0;
	return g_timegetter->getTime();
}

/*
	Event handler for Irrlicht

	NOTE: Everything possible should be moved out from here,
	      probably to InputHandler and the_game
*/

class MyEventReceiver : public IEventReceiver
{
public:
	// This is the one method that we have to implement
	virtual bool OnEvent(const SEvent& event)
	{
		/*
			React to nothing here if a menu is active
		*/
		if(noMenuActive() == false)
		{
			return false;
		}

		// Remember whether each key is down or up
		if(event.EventType == irr::EET_KEY_INPUT_EVENT)
		{
			if(event.KeyInput.PressedDown) {
				keyIsDown.set(event.KeyInput);
				keyWasDown.set(event.KeyInput);
			} else {
				keyIsDown.unset(event.KeyInput);
			}
		}

		if(event.EventType == irr::EET_MOUSE_INPUT_EVENT)
		{
			if(noMenuActive() == false)
			{
				left_active = false;
				middle_active = false;
				right_active = false;
			}
			else
			{
				left_active = event.MouseInput.isLeftPressed();
				middle_active = event.MouseInput.isMiddlePressed();
				right_active = event.MouseInput.isRightPressed();

				if(event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
				{
					leftclicked = true;
				}
				if(event.MouseInput.Event == EMIE_RMOUSE_PRESSED_DOWN)
				{
					rightclicked = true;
				}
				if(event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP)
				{
					leftreleased = true;
				}
				if(event.MouseInput.Event == EMIE_RMOUSE_LEFT_UP)
				{
					rightreleased = true;
				}
				if(event.MouseInput.Event == EMIE_MOUSE_WHEEL)
				{
					mouse_wheel += event.MouseInput.Wheel;
				}
			}
		}

		return false;
	}

	bool IsKeyDown(const KeyPress &keyCode) const
	{
		return keyIsDown[keyCode];
	}

	// Checks whether a key was down and resets the state
	bool WasKeyDown(const KeyPress &keyCode)
	{
		bool b = keyWasDown[keyCode];
		if (b)
			keyWasDown.unset(keyCode);
		return b;
	}

	s32 getMouseWheel()
	{
		s32 a = mouse_wheel;
		mouse_wheel = 0;
		return a;
	}

	void clearInput()
	{
		keyIsDown.clear();
		keyWasDown.clear();

		leftclicked = false;
		rightclicked = false;
		leftreleased = false;
		rightreleased = false;

		left_active = false;
		middle_active = false;
		right_active = false;

		mouse_wheel = 0;
	}

	MyEventReceiver()
	{
		clearInput();
	}

	bool leftclicked;
	bool rightclicked;
	bool leftreleased;
	bool rightreleased;

	bool left_active;
	bool middle_active;
	bool right_active;

	s32 mouse_wheel;

private:
	// The current state of keys
	KeyList keyIsDown;
	// Whether a key has been pressed or not
	KeyList keyWasDown;
};

/*
	Separated input handler
*/

class RealInputHandler : public InputHandler
{
public:
	RealInputHandler(IrrlichtDevice *device, MyEventReceiver *receiver):
		m_device(device),
		m_receiver(receiver)
	{
	}
	virtual bool isKeyDown(const KeyPress &keyCode)
	{
		return m_receiver->IsKeyDown(keyCode);
	}
	virtual bool wasKeyDown(const KeyPress &keyCode)
	{
		return m_receiver->WasKeyDown(keyCode);
	}
	virtual v2s32 getMousePos()
	{
		return m_device->getCursorControl()->getPosition();
	}
	virtual void setMousePos(s32 x, s32 y)
	{
		m_device->getCursorControl()->setPosition(x, y);
	}

	virtual bool getLeftState()
	{
		return m_receiver->left_active;
	}
	virtual bool getRightState()
	{
		return m_receiver->right_active;
	}

	virtual bool getLeftClicked()
	{
		return m_receiver->leftclicked;
	}
	virtual bool getRightClicked()
	{
		return m_receiver->rightclicked;
	}
	virtual void resetLeftClicked()
	{
		m_receiver->leftclicked = false;
	}
	virtual void resetRightClicked()
	{
		m_receiver->rightclicked = false;
	}

	virtual bool getLeftReleased()
	{
		return m_receiver->leftreleased;
	}
	virtual bool getRightReleased()
	{
		return m_receiver->rightreleased;
	}
	virtual void resetLeftReleased()
	{
		m_receiver->leftreleased = false;
	}
	virtual void resetRightReleased()
	{
		m_receiver->rightreleased = false;
	}

	virtual s32 getMouseWheel()
	{
		return m_receiver->getMouseWheel();
	}

	void clear()
	{
		m_receiver->clearInput();
	}
private:
	IrrlichtDevice *m_device;
	MyEventReceiver *m_receiver;
};

class RandomInputHandler : public InputHandler
{
public:
	RandomInputHandler()
	{
		leftdown = false;
		rightdown = false;
		leftclicked = false;
		rightclicked = false;
		leftreleased = false;
		rightreleased = false;
		keydown.clear();
	}
	virtual bool isKeyDown(const KeyPress &keyCode)
	{
		return keydown[keyCode];
	}
	virtual bool wasKeyDown(const KeyPress &keyCode)
	{
		return false;
	}
	virtual v2s32 getMousePos()
	{
		return mousepos;
	}
	virtual void setMousePos(s32 x, s32 y)
	{
		mousepos = v2s32(x,y);
	}

	virtual bool getLeftState()
	{
		return leftdown;
	}
	virtual bool getRightState()
	{
		return rightdown;
	}

	virtual bool getLeftClicked()
	{
		return leftclicked;
	}
	virtual bool getRightClicked()
	{
		return rightclicked;
	}
	virtual void resetLeftClicked()
	{
		leftclicked = false;
	}
	virtual void resetRightClicked()
	{
		rightclicked = false;
	}

	virtual bool getLeftReleased()
	{
		return leftreleased;
	}
	virtual bool getRightReleased()
	{
		return rightreleased;
	}
	virtual void resetLeftReleased()
	{
		leftreleased = false;
	}
	virtual void resetRightReleased()
	{
		rightreleased = false;
	}

	virtual s32 getMouseWheel()
	{
		return 0;
	}

	virtual void step(float dtime)
	{
		{
			static float counter1 = 0;
			counter1 -= dtime;
			if(counter1 < 0.0)
			{
				counter1 = 0.1*Rand(1, 40);
				keydown.toggle(getKeySetting(VLKC_JUMP));
			}
		}
		{
			static float counter1 = 0;
			counter1 -= dtime;
			if(counter1 < 0.0)
			{
				counter1 = 0.1*Rand(1, 40);
				keydown.toggle(getKeySetting(VLKC_USE));
			}
		}
		{
			static float counter1 = 0;
			counter1 -= dtime;
			if(counter1 < 0.0)
			{
				counter1 = 0.1*Rand(1, 40);
				keydown.toggle(getKeySetting(VLKC_FORWARD));
			}
		}
		{
			static float counter1 = 0;
			counter1 -= dtime;
			if(counter1 < 0.0)
			{
				counter1 = 0.1*Rand(1, 40);
				keydown.toggle(getKeySetting(VLKC_LEFT));
			}
		}
		{
			static float counter1 = 0;
			counter1 -= dtime;
			if(counter1 < 0.0)
			{
				counter1 = 0.1*Rand(1, 20);
				mousespeed = v2s32(Rand(-20,20), Rand(-15,20));
			}
		}
		{
			static float counter1 = 0;
			counter1 -= dtime;
			if(counter1 < 0.0)
			{
				counter1 = 0.1*Rand(1, 30);
				leftdown = !leftdown;
				if(leftdown)
					leftclicked = true;
				if(!leftdown)
					leftreleased = true;
			}
		}
		{
			static float counter1 = 0;
			counter1 -= dtime;
			if(counter1 < 0.0)
			{
				counter1 = 0.1*Rand(1, 15);
				rightdown = !rightdown;
				if(rightdown)
					rightclicked = true;
				if(!rightdown)
					rightreleased = true;
			}
		}
		mousepos += mousespeed;
	}

	s32 Rand(s32 min, s32 max)
	{
		return (myrand()%(max-min+1))+min;
	}
private:
	KeyList keydown;
	v2s32 mousepos;
	v2s32 mousespeed;
	bool leftdown;
	bool rightdown;
	bool leftclicked;
	bool rightclicked;
	bool leftreleased;
	bool rightreleased;
};

// These are defined global so that they're not optimized too much.
// Can't change them to volatile.
s16 temp16;
f32 tempf;
v3f tempv3f1;
v3f tempv3f2;
std::string tempstring;
std::string tempstring2;

void SpeedTests()
{
	{
		dstream<<"The following test should take around 20ms."<<std::endl;
		TimeTaker timer("Testing std::string speed");
		const u32 jj = 10000;
		for(u32 j=0; j<jj; j++)
		{
			tempstring = "";
			tempstring2 = "";
			const u32 ii = 10;
			for(u32 i=0; i<ii; i++){
				tempstring2 += "asd";
			}
			for(u32 i=0; i<ii+1; i++){
				tempstring += "asd";
				if(tempstring == tempstring2)
					break;
			}
		}
	}

	dstream<<"All of the following tests should take around 100ms each."
			<<std::endl;

	{
		TimeTaker timer("Testing floating-point conversion speed");
		tempf = 0.001;
		for(u32 i=0; i<4000000; i++){
			temp16 += tempf;
			tempf += 0.001;
		}
	}

	{
		TimeTaker timer("Testing floating-point vector speed");

		tempv3f1 = v3f(1,2,3);
		tempv3f2 = v3f(4,5,6);
		for(u32 i=0; i<10000000; i++){
			tempf += tempv3f1.dotProduct(tempv3f2);
			tempv3f2 += v3f(7,8,9);
		}
	}

	{
		TimeTaker timer("Testing core::map speed");

		core::map<v2s16, f32> map1;
		tempf = -324;
		const s16 ii=300;
		for(s16 y=0; y<ii; y++){
			for(s16 x=0; x<ii; x++){
				map1.insert(v2s16(x,y), tempf);
				tempf += 1;
			}
		}
		for(s16 y=ii-1; y>=0; y--){
			for(s16 x=0; x<ii; x++){
				tempf = map1[v2s16(x,y)];
			}
		}
	}

	{
		dstream<<"Around 5000/ms should do well here."<<std::endl;
		TimeTaker timer("Testing mutex speed");

		JMutex m;
		m.Init();
		u32 n = 0;
		u32 i = 0;
		do{
			n += 10000;
			for(; i<n; i++){
				m.Lock();
				m.Unlock();
			}
		}
		// Do at least 10ms
		while(timer.getTime() < 10);

		u32 dtime = timer.stop();
		u32 per_ms = n / dtime;
		dstream<<"Done. "<<dtime<<"ms, "
				<<per_ms<<"/ms"<<std::endl;
	}
}

void drawMenuBackground(video::IVideoDriver* driver)
{
	core::dimension2d<u32> screensize = driver->getScreenSize();

	video::ITexture *mud = driver->getTexture(getTexturePath("mud.png").c_str());
	video::ITexture *stone = driver->getTexture(getTexturePath("stone.png").c_str());
	video::ITexture *grass = driver->getTexture(getTexturePath("grass_side.png").c_str());
	if (mud && stone && grass) {
		video::ITexture *texture;
		s32 texturesize = 128;
		s32 tiled_y = screensize.Height / texturesize + 1;
		s32 tiled_x = screensize.Width / texturesize + 1;
		s32 grassline = (tiled_y/4)*3;

		texture = stone;

		for (s32 y=0; y<tiled_y; y++)
		for (s32 x=0; x<tiled_x; x++) {
			if (y == grassline) {
				core::rect<s32> rect(0,0,texturesize,texturesize);
				rect += v2s32(x*texturesize, y*texturesize);
				driver->draw2DImage(
					mud,
					rect,
					core::rect<s32>(
						core::position2d<s32>(0,0),
						core::dimension2di(mud->getSize())
					),
					NULL,
					NULL,
					true
				);
				driver->draw2DImage(
					grass,
					rect,
					core::rect<s32>(
						core::position2d<s32>(0,0),
						core::dimension2di(grass->getSize())
					),
					NULL,
					NULL,
					true
				);
				texture = mud;
			}else{
				core::rect<s32> rect(0,0,texturesize,texturesize);
				rect += v2s32(x*texturesize, y*texturesize);
				driver->draw2DImage(
					texture,
					rect,
					core::rect<s32>(
						core::position2d<s32>(0,0),
						core::dimension2di(texture->getSize())
					),
					NULL,
					NULL,
					true
				);
			}
		}
	}
}

class StderrLogOutput: public ILogOutput
{
public:
	/* line: Full line with timestamp, level and thread */
	void printLog(const std::string &line)
	{
		std::cerr<<line<<std::endl;
	}
} main_stderr_log_out;

class DstreamNoStderrLogOutput: public ILogOutput
{
public:
	/* line: Full line with timestamp, level and thread */
	void printLog(const std::string &line)
	{
		dstream_no_stderr<<line<<std::endl;
	}
} main_dstream_no_stderr_log_out;

int main(int argc, char *argv[])
{
	/*
		Initialization
	*/

	log_add_output_maxlev(&main_stderr_log_out, LMT_ACTION);
	log_add_output_all_levs(&main_dstream_no_stderr_log_out);

	log_register_thread("main");

	/*
		Parse command line
	*/

	// List all allowed options
	core::map<std::string, ValueSpec> allowed_options;
	allowed_options.insert("help", ValueSpec(VALUETYPE_FLAG));
	allowed_options.insert("server", ValueSpec(VALUETYPE_FLAG,
			"Run server directly"));
	allowed_options.insert("config", ValueSpec(VALUETYPE_STRING,
			"Load configuration from specified file"));
	allowed_options.insert("port", ValueSpec(VALUETYPE_STRING));
	allowed_options.insert("address", ValueSpec(VALUETYPE_STRING));
	allowed_options.insert("random-input", ValueSpec(VALUETYPE_FLAG));
	allowed_options.insert("disable-unittests", ValueSpec(VALUETYPE_FLAG));
	allowed_options.insert("enable-unittests", ValueSpec(VALUETYPE_FLAG));
	allowed_options.insert("map-dir", ValueSpec(VALUETYPE_STRING));
#ifdef _WIN32
	allowed_options.insert("dstream-on-stderr", ValueSpec(VALUETYPE_FLAG));
#endif
	allowed_options.insert("speedtests", ValueSpec(VALUETYPE_FLAG));
	allowed_options.insert("info-on-stderr", ValueSpec(VALUETYPE_FLAG));

	Settings cmd_args;

	bool ret = cmd_args.parseCommandLine(argc, argv, allowed_options);

	if (ret == false || cmd_args.getFlag("help")) {
		dstream<<"Allowed options:"<<std::endl;
		for (core::map<std::string, ValueSpec>::Iterator i = allowed_options.getIterator(); i.atEnd() == false; i++) {
			dstream<<"  --"<<i.getNode()->getKey();
			if(i.getNode()->getValue().type != VALUETYPE_FLAG)
				dstream<<" <value>";
			dstream<<std::endl;

			if (i.getNode()->getValue().help != NULL)
				dstream<<"      "<<i.getNode()->getValue().help<<std::endl;
		}

		return cmd_args.getFlag("help") ? 0 : 1;
	}

	/*
		Low-level initialization
	*/

	bool disable_stderr = false;
#ifdef _WIN32
	if (cmd_args.getFlag("dstream-on-stderr") == false)
		disable_stderr = true;
#endif

	if (cmd_args.getFlag("info-on-stderr"))
		log_add_output(&main_stderr_log_out, LMT_INFO);

	porting::signal_handler_init();
	bool &kill = *porting::signal_handler_killstatus();

	// Initialize porting::path_data and porting::path_userdata
	porting::initializePaths();

	// Create user data directory
	fs::CreateDir(porting::path_userdata);

	init_gettext((porting::path_data+DIR_DELIM+".."+DIR_DELIM+"locale").c_str());

	// Initialize debug streams
#ifdef RUN_IN_PLACE
	std::string debugfile = DEBUGFILE;
#else
	std::string debugfile = porting::path_userdata+DIR_DELIM+DEBUGFILE;
#endif
	debugstreams_init(disable_stderr, debugfile.c_str());
	// Initialize debug stacks
	debug_stacks_init();

	DSTACK(__FUNCTION_NAME);

	// Init material properties table
	//initializeMaterialProperties();

	// Debug handler
	BEGIN_DEBUG_EXCEPTION_HANDLER

	// Print startup message
	actionstream<<PROJECT_NAME<<
			" with SER_FMT_VER_HIGHEST="<<(int)SER_FMT_VER_HIGHEST
			<<", "<<BUILD_INFO
			<<std::endl;

	/*
		Basic initialization
	*/

	// Initialize default settings
	set_default_settings(g_settings);

	// Initialize sockets
	sockets_init();
	atexit(sockets_cleanup);

	/*
		Read config file
	*/

	// Path of configuration file in use
	std::string configpath = "";

	if (cmd_args.exists("config")) {
		bool r = g_settings->readConfigFile(cmd_args.get("config").c_str());
		if (r == false) {
			errorstream<<"Could not read configuration from \""
					<<cmd_args.get("config")<<"\""<<std::endl;
			return 1;
		}
		configpath = cmd_args.get("config");
	}else{
		core::array<std::string> filenames;
		filenames.push_back(porting::path_userdata +
				DIR_DELIM + "voxelands.conf");
#ifdef RUN_IN_PLACE
		filenames.push_back(porting::path_userdata +
				DIR_DELIM + ".." + DIR_DELIM + "voxelands.conf");
#endif

		for (u32 i=0; i<filenames.size(); i++) {
			bool r = g_settings->readConfigFile(filenames[i].c_str());
			if (r) {
				configpath = filenames[i];
				break;
			}
		}

		// If no path found, use the first one (menu creates the file)
		if (configpath == "")
			configpath = filenames[0];
	}

	// Initialize random seed
	srand(time(0));
	mysrand(time(0));

	/*
		Pre-initialize some stuff with a dummy irrlicht wrapper.

		These are needed for unit tests at least.
	*/

	crafting::initCrafting();
	content_clothesitem_init();
	content_craftitem_init();
	content_toolitem_init();
	content_mob_init();
	// Initial call with g_texturesource not set.
	init_mapnode(NULL);
	// Must be called before g_texturesource is created
	// (for texture atlas making)
	init_mineral();

#if USE_AUDIO == 1
	ISoundManager *sound = createSoundManager();
#else
	ISoundManager *sound = NULL;
#endif

	/*
		Run unit tests
	*/

	if (
		(ENABLE_TESTS && cmd_args.getFlag("disable-unittests") == false)
		|| cmd_args.getFlag("enable-unittests") == true
	)
		run_tests();

	/*for(s16 y=-100; y<100; y++)
	for(s16 x=-100; x<100; x++)
	{
		std::cout<<noise2d_gradient((double)x/10,(double)y/10, 32415)<<std::endl;
	}
	return 0;*/

	/*
		Game parameters
	*/

	// Port
	u16 port = 30000;
	if (cmd_args.exists("port")) {
		port = cmd_args.getU16("port");
	}else if (g_settings->exists("port")) {
		port = g_settings->getU16("port");
	}
	if (port == 0)
		port = 30000;

	// Map directory
	std::string map_dir = porting::path_userdata+DIR_DELIM+"world";
	if (cmd_args.exists("map-dir")) {
		map_dir = cmd_args.get("map-dir");
	}else if (g_settings->exists("map-dir")) {
		map_dir = g_settings->get("map-dir");
	}

	// Run dedicated server if asked to
	if (cmd_args.getFlag("server")) {
		DSTACK("Dedicated server branch");

		// Create time getter
		g_timegetter = new SimpleTimeGetter();

		// Create server
		Server server(map_dir.c_str(), configpath);
		server.start(port);

		// Run server
		dedicated_server_loop(server, kill);

		return 0;
	}


	/*
		More parameters
	*/

	// Address to connect to
	std::string address = "";

	if (cmd_args.exists("address")) {
		address = cmd_args.get("address");
	}else{
		address = g_settings->get("address");
	}

	std::string playername = g_settings->get("name");

	/*
		Device initialization
	*/

	// Resolution selection

	bool fullscreen = g_settings->getBool("fullscreen");
	u16 screenW = g_settings->getU16("screenW");
	u16 screenH = g_settings->getU16("screenH");

	// bpp, fsaa, vsync

	bool vsync = g_settings->getBool("vsync");
	u16 bits = g_settings->getU16("fullscreen_bpp");
	u16 fsaa = g_settings->getU16("fsaa");

	// Determine driver

	video::E_DRIVER_TYPE driverType;

	std::string driverstring = g_settings->get("video_driver");

	if (driverstring == "null") {
		driverType = video::EDT_NULL;
	}else if (driverstring == "software") {
		driverType = video::EDT_SOFTWARE;
	}else if (driverstring == "burningsvideo") {
		driverType = video::EDT_BURNINGSVIDEO;
	}else if (driverstring == "direct3d8") {
		driverType = video::EDT_DIRECT3D8;
	}else if (driverstring == "direct3d9") {
		driverType = video::EDT_DIRECT3D9;
	}else if (driverstring == "opengl") {
		driverType = video::EDT_OPENGL;
	}else{
		errorstream<<"WARNING: Invalid video_driver specified; defaulting "
				"to opengl"<<std::endl;
		driverType = video::EDT_OPENGL;
	}

	/*
		Create device and exit if creation failed
	*/

	MyEventReceiver receiver;

	SIrrlichtCreationParameters params = SIrrlichtCreationParameters();
	params.DriverType    = driverType;
	params.WindowSize    = core::dimension2d<u32>(screenW, screenH);
	params.Bits          = bits;
	params.AntiAlias     = fsaa;
	params.Fullscreen    = fullscreen;
	params.Stencilbuffer = false;
	params.Vsync         = vsync;
	params.EventReceiver = &receiver;
	params.HighPrecisionFPU = g_settings->getBool("high_precision_fpu");

	IrrlichtDevice * device = createDeviceEx(params);

	//IrrlichtDevice *device;
	//device = createDevice(driverType,
			//core::dimension2d<u32>(screenW, screenH),
			//16, fullscreen, false, false, &receiver);

	if (device == 0)
		return 1; // could not create selected driver.

	/*
		Continue initialization
	*/

	video::IVideoDriver* driver = device->getVideoDriver();

	// Disable mipmaps (because some of them look ugly)
	driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, false);

	/*
		This changes the minimum allowed number of vertices in a VBO.
		Default is 500.
	*/
	//driver->setMinHardwareBufferVertexCount(50);

	// Set the window caption
	device->setWindowCaption(L"Voxelands [Main Menu]");

	// Create time getter
	g_timegetter = new IrrlichtTimeGetter(device);

	// Create game callback for menus
	g_gamecallback = new MainGameCallback(device);

	// Create texture source
	g_texturesource = new TextureSource(device);

	drawLoadingScreen(driver,L"");

	/*
		Speed tests (done after irrlicht is loaded to get timer)
	*/
	if (cmd_args.getFlag("speedtests")) {
		dstream<<"Running speed tests"<<std::endl;
		SpeedTests();
		return 0;
	}

	device->setResizable(true);

	bool random_input = g_settings->getBool("random_input")
			|| cmd_args.getFlag("random-input");
	InputHandler *input = NULL;
	if (random_input) {
		input = new RandomInputHandler();
	}else{
		input = new RealInputHandler(device, &receiver);
	}

	scene::ISceneManager* smgr = device->getSceneManager();

	guienv = device->getGUIEnvironment();
	gui::IGUISkin* skin = guienv->getSkin();
#if USE_FREETYPE
	u16 font_size = g_settings->getU16("font_size");
	gui::IGUIFont* font = gui::CGUITTFont::createTTFont(guienv, getPath("font","liberationsans.ttf",false).c_str(), font_size, true, true, 1, 128);
#else
	gui::IGUIFont* font = guienv->getFont(getTexturePath("fontlucida.png").c_str());
#endif
	if (font) {
		skin->setFont(font);
	}else{
		errorstream<<"WARNING: Font file was not found. Using default font."<<std::endl;
	}
	// If font was not found, this will get us one
	font = skin->getFont();
	assert(font);
	drawLoadingScreen(driver,wgettext("Setting Up UI"));

	u32 text_height = font->getDimension(L"Hello, world!").Height;
	infostream<<"text_height="<<text_height<<std::endl;

	skin->setColor(gui::EGDC_3D_HIGH_LIGHT, GUI_3D_HIGH_LIGHT);
	skin->setColor(gui::EGDC_3D_SHADOW, GUI_3D_SHADOW);
	skin->setColor(gui::EGDC_HIGH_LIGHT, GUI_HIGH_LIGHT);
	skin->setColor(gui::EGDC_HIGH_LIGHT_TEXT, GUI_HIGH_LIGHT_TEXT);
	skin->setColor(gui::EGDC_BUTTON_TEXT, GUI_BUTTON_TEXT);

#if (IRRLICHT_VERSION_MAJOR >= 1 && IRRLICHT_VERSION_MINOR >= 8) || IRRLICHT_VERSION_MAJOR >= 2
	// Irrlicht 1.8 input colours
	skin->setColor(gui::EGDC_3D_FACE, GUI_3D_FACE);
	skin->setColor(gui::EGDC_3D_DARK_SHADOW, GUI_3D_DARK_SHADOW);
	skin->setColor(gui::EGDC_EDITABLE, GUI_EDITABLE);
	skin->setColor(gui::EGDC_FOCUSED_EDITABLE, GUI_FOCUSED_EDITABLE);
#endif

	/*
		Preload some textures and stuff
	*/

	drawLoadingScreen(driver,wgettext("Loading MapNodes"));
	init_mapnode(driver); // Second call with g_texturesource set
	drawLoadingScreen(driver,wgettext("Loading Creatures"));
	content_mob_init();

	/*
		GUI stuff
	*/

	/*
		If an error occurs, this is set to something and the
		menu-game loop is restarted. It is then displayed before
		the menu.
	*/
	std::wstring error_message = L"";

	// The password entered during the menu screen,
	std::string password;

	// if there's no chardef then put the player directly into the character creator
	bool character_creator = !g_settings->exists("character_definition");

	/*
		Menu-game loop
	*/
	while (device->run() && kill == false) {

		// This is used for catching disconnects
		try{

			/*
				Clear everything from the GUIEnvironment
			*/
			guienv->clear();

			/*
				We need some kind of a root node to be able to add
				custom gui elements directly on the screen.
				Otherwise they won't be automatically drawn.
			*/
			guiroot = guienv->addStaticText(L"",core::rect<s32>(0, 0, 10000, 10000));

			/*
				Out-of-game menu loop.
				Loop quits when menu returns proper parameters.
				break out immediately if we should be in the character creator
			*/
			while (kill == false && character_creator == false) {
				// Cursor can be non-visible when coming from the game
				device->getCursorControl()->setVisible(true);
				// Some stuff are left to scene manager when coming from the game
				// (map at least?)
				smgr->clear();

				// Initialize menu data
				MainMenuData menudata;
				menudata.address = narrow_to_wide(address);
				menudata.name = narrow_to_wide(playername);
				menudata.port = narrow_to_wide(itos(port));
				menudata.fancy_trees = g_settings->getBool("new_style_leaves");
				menudata.smooth_lighting = g_settings->getBool("smooth_lighting");
				menudata.clouds_3d = g_settings->getBool("enable_3d_clouds");
				menudata.opaque_water = g_settings->getBool("opaque_water");
				menudata.fullscreen = g_settings->getBool("fullscreen");
				menudata.particles = g_settings->getBool("enable_particles");
				menudata.mip_map = g_settings->getBool("mip_map");
				menudata.anisotropic_filter = g_settings->getBool("anisotropic_filter");
				menudata.bilinear_filter = g_settings->getBool("bilinear_filter");
				menudata.trilinear_filter = g_settings->getBool("trilinear_filter");
				menudata.hotbar = g_settings->getBool("old_hotbar");
				menudata.volume = g_settings->getFloat("sound_volume");
				driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, menudata.mip_map);
				menudata.game_mode = narrow_to_wide(g_settings->get("game_mode"));
				menudata.max_mob_level = narrow_to_wide(g_settings->get("max_mob_level"));
				menudata.initial_inventory = g_settings->getBool("initial_inventory");
				menudata.infinite_inventory = g_settings->getBool("infinite_inventory");
				menudata.droppable_inventory = g_settings->getBool("droppable_inventory");
				menudata.enable_damage = g_settings->getBool("enable_damage");
				menudata.suffocation = g_settings->getBool("enable_suffocation");
				menudata.hunger = g_settings->getBool("enable_hunger");
				menudata.tool_wear = g_settings->getBool("tool_wear");
				menudata.delete_map = false;
				menudata.clear_map = false;
				menudata.use_fixed_seed = false;
				if (g_settings->exists("fixed_map_seed")) {
					menudata.fixed_seed = narrow_to_wide(g_settings->get("fixed_map_seed"));
					if (menudata.fixed_seed != L"")
						menudata.use_fixed_seed = true;
				}
				if (g_settings->exists("mapgen_type"))
					menudata.map_type = g_settings->get("mapgen_type");

				GUIMainMenu *menu =
						new GUIMainMenu(guienv, guiroot, -1,
							&g_menumgr, &menudata, g_gamecallback);
				menu->allowFocusRemoval(true);

				if (error_message != L"") {
					errorstream<<"error_message = "
							<<wide_to_narrow(error_message)<<std::endl;

					GUIMessageMenu *menu2 =
							new GUIMessageMenu(guienv, guiroot, -1,
								&g_menumgr, error_message.c_str());
					menu2->drop();
					error_message = L"";
				}

				video::IVideoDriver* driver = device->getVideoDriver();

				infostream<<"Created main menu"<<std::endl;

				while (device->run() && kill == false) {
					if (menu->getStatus() == true)
						break;

					//driver->beginScene(true, true, video::SColor(255,0,0,0));
					driver->beginScene(true, true, video::SColor(255,170,230,255));

					drawMenuBackground(driver);

					guienv->drawAll();

					driver->endScene();

					// On some computers framerate doesn't seem to be
					// automatically limited
					sleep_ms(25);
				}

				// Break out of menu-game loop to shut down cleanly
				if (device->run() == false || kill == true)
					break;

				infostream<<"Dropping main menu"<<std::endl;

				menu->drop();

				character_creator = menudata.character_creator;

				if (character_creator)
					break;

				// Delete map if requested
				if (menudata.delete_map) {
					bool r = fs::RecursiveDeleteContent(map_dir);
					if(r == false) {
						error_message = L"Map deletion failed";
						continue;
					}
					if (menudata.use_fixed_seed)
						g_settings->set("fixed_map_seed",wide_to_narrow(menudata.fixed_seed));
					g_settings->set("mapgen_type",menudata.map_type);
				}else if (menudata.clear_map) {
					std::string map_file = map_dir+DIR_DELIM+"map.sqlite";
					bool r = fs::RecursiveDelete(map_file);
					if(r == false) {
						error_message = L"Map clearing failed";
						continue;
					}
				}

				playername = wide_to_narrow(menudata.name);

				password = translatePassword(playername, menudata.password);

				//infostream<<"Main: password hash: '"<<password<<"'"<<std::endl;

				address = wide_to_narrow(menudata.address);
				int newport = mywstoi(menudata.port);
				if (newport != 0)
					port = newport;
				g_settings->set("new_style_leaves", itos(menudata.fancy_trees));
				g_settings->set("smooth_lighting", itos(menudata.smooth_lighting));
				g_settings->set("enable_3d_clouds", itos(menudata.clouds_3d));
				g_settings->set("opaque_water", itos(menudata.opaque_water));
				g_settings->set("mip_map", itos(menudata.mip_map));
				g_settings->set("anisotropic_filter", itos(menudata.anisotropic_filter));
				g_settings->set("bilinear_filter", itos(menudata.bilinear_filter));
				g_settings->set("trilinear_filter", itos(menudata.trilinear_filter));
				g_settings->set("fullscreen", itos(menudata.fullscreen));
				g_settings->set("enable_particles", itos(menudata.particles));
				g_settings->set("old_hotbar", itos(menudata.hotbar));
				g_settings->set("game_mode", wide_to_narrow(menudata.game_mode));
				g_settings->set("max_mob_level", wide_to_narrow(menudata.max_mob_level));
				g_settings->set("initial_inventory", itos(menudata.initial_inventory));
				g_settings->set("infinite_inventory", itos(menudata.infinite_inventory));
				g_settings->set("droppable_inventory", itos(menudata.droppable_inventory));
				g_settings->set("enable_damage", itos(menudata.enable_damage));
				g_settings->set("enable_suffocation", itos(menudata.suffocation));
				g_settings->set("enable_hunger", itos(menudata.hunger));
				g_settings->set("tool_wear", itos(menudata.tool_wear));

				// Save settings
				g_settings->set("name", playername);
				g_settings->set("address", address);
				g_settings->set("port", itos(port));
				// Update configuration file
				if (configpath != "")
					g_settings->updateConfigFile(configpath.c_str());

				// Continue to game
				break;
			}

			// Break out of menu-game loop to shut down cleanly
			if (device->run() == false)
				break;

			if (character_creator) {
				character_creator = false;
				video::IVideoDriver* driver = device->getVideoDriver();

				GUICharDefMenu *menu = new GUICharDefMenu(device, guienv, guiroot, -1, &g_menumgr);
				menu->allowFocusRemoval(true);

				while (device->run() && kill == false) {
					if (menu->getStatus() == true)
						break;

					driver->beginScene(true, true, video::SColor(255,0,0,0));

					smgr->drawAll();
					guienv->drawAll();

					driver->endScene();

					// On some computers framerate doesn't seem to be
					// automatically limited
					sleep_ms(25);
				}

				menu->drop();

				continue;
			}
			// Initialize mapnode again to enable changed graphics settings
			init_mapnode(driver);

			/*
				Run game
			*/
			the_game(
				kill,
				random_input,
				input,
				device,
				font,
				map_dir,
				playername,
				password,
				address,
				port,
				error_message,
				configpath,
				sound
			);

		} //try
		catch(con::PeerNotFoundException &e)
		{
			errorstream<<"Connection error (timed out?)"<<std::endl;
			error_message = L"Connection error (timed out?)";
		}
		catch(SocketException &e)
		{
			errorstream<<"Socket error (port already in use?)"<<std::endl;
			error_message = L"Socket error (port already in use?)";
		}
#ifdef NDEBUG
		catch(std::exception &e)
		{
			std::string narrow_message = "Some exception, what()=\"";
			narrow_message += e.what();
			narrow_message += "\"";
			errorstream<<narrow_message<<std::endl;
			error_message = narrow_to_wide(narrow_message);
		}
#endif

	} // Menu-game loop

	delete input;

	/*
		In the end, delete the Irrlicht device.
	*/
	device->drop();

	END_DEBUG_EXCEPTION_HANDLER(errorstream)

	debugstreams_deinit();

	return 0;
}

//END
