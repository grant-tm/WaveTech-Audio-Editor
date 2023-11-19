//=================================================================================================
// Include Dependencies
//=================================================================================================
#include "..\dependencies\AudioFile\AudioFile.h"

// objects for sending/receiving messages and audio data
#include "Pipe.hpp"

// objects for creating, displaying, running the GUI
#include "Gui.hpp"

//=================================================================================================
// Standard Libraries
//=================================================================================================
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
//=================================================================================================
// Main Menu Functions
//=================================================================================================
void main_menu_exit_program(Gui* gui)
{
    gui->send_message("exit");
    gui->set_exit_status(true);
}

void main_menu_file_options(Gui* gui)
{
    gui->set_current_menu(1);
}

void main_menu_audio_effects(Gui* gui)
{
    gui->set_current_menu(2);
}
//=================================================================================================
// File Options Functions
//=================================================================================================
void file_options_main_menu(Gui* gui)
{
    gui->set_current_menu(0);
}

void file_options_load_file(Gui* gui)
{
    gui->load_audio_file();
}

void file_options_save_file(Gui* gui)
{
    gui->save_audio_file();
}
//=================================================================================================
// Audio Effect Functions
//=================================================================================================
void audio_effects_main_menu(Gui* gui)
{
    gui->set_current_menu(0);
}

void audio_effects_reverse(Gui* gui)
{
    if(gui->get_file_loaded_status() == false)
    {
        std::cout << "Load a file to add effects." << std::endl;
        return;
    }

    gui->send_message("reverse");

    std::string alias = gui->get_filename_display_alias();
    if(alias.compare(alias.length()-1, 1, "*") != 0)
    {
        alias += "*";
    }
    gui->set_filename_display_alias(alias);
}

void audio_effects_stretch(Gui* gui)
{
    gui->send_message("stretch");
}

void audio_effects_repitch(Gui* gui)
{
    gui->send_message("shift");
}
//=================================================================================================
// Functions
//=================================================================================================
std::unique_ptr<Gui> create_wavetech_gui()
{
    std::unique_ptr<Gui> gui = std::make_unique<Gui>();

    // build header
    std::vector<std::string> wavetech_header(3);
    wavetech_header[0] = "------------------------------------------------------------";
    wavetech_header[1] = "File: ";
    wavetech_header[2] = "------------------------------------------------------------";
    gui->set_header(wavetech_header);

    // reusable vectors to build menus
    std::string name;
    std::vector<std::string> options;
    std::vector<void(*)(Gui*)> functions;
    
    // build main menu
    name = "Main Menu";
    options =
    {
        "Exit Program",
        "File Options",
        "Audio Effects"
    };
    functions = 
    {
        main_menu_exit_program, 
        main_menu_file_options, 
        main_menu_audio_effects
    };
    std::shared_ptr<Menu> main_menu = std::move(create_menu(name, options, functions));
    gui->add_menu(main_menu);

    // build file options menu
    name = "File Options";
    options = 
    {
        "Main Menu", 
        "Load File", 
        "Save File"
    };
    functions = 
    {
        file_options_main_menu,
        file_options_load_file,
        file_options_save_file
    };
    std::shared_ptr<Menu> file_options = std::move(create_menu(name, options, functions));
    gui->add_menu(file_options);

    // build audio effects menu
    name = "Audio Effects";
    options =
    {
        "Main Menu",
        "Reverse Audio",
        "Stretch Audio",
        "Repitch Audio"
    };
    functions = 
    {
        audio_effects_main_menu,
        audio_effects_reverse,
        audio_effects_stretch,
        audio_effects_repitch
    };
    std::shared_ptr<Menu> audio_effects = std::move(create_menu(name, options, functions));
    gui->add_menu(audio_effects);
    
    return gui;
}
//=================================================================================================
// Main
//=================================================================================================
int main(int argc, char** argv)
{
    std::unique_ptr<Gui> gui = create_wavetech_gui();
    gui->set_current_menu(0);
    while(!gui->get_exit_status())
    {
        gui->display();
        gui->execute();
    }
    return EXIT_SUCCESS;
}