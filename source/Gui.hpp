//=================================================================================================
// Include Dependencies
//=================================================================================================
#include "..\dependencies\AudioFile\AudioFile.h"

// objects for sending/receiving messages and audio data
#include "Pipe.hpp"

//=================================================================================================
// Standard Libraries
//=================================================================================================
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
//=================================================================================================
// Menu Object
//=================================================================================================
class Gui;
class Menu
{
public:
    Menu();
    Menu(std::string, std::vector<std::string>, std::vector<void(*)(Gui*)>);

    void print();

    int num_options;
    std::string menu_name;
    std::vector<std::string> menu_options;
    std::vector<void(*)(Gui*)> menu_functions;
};

Menu::Menu()
{

}

std::unique_ptr<Menu> create_menu(std::string name, 
    std::vector<std::string> options, std::vector<void(*)(Gui*)> functions)
{
    return std::make_unique<Menu>(name, options, functions);
}

Menu::Menu(std::string name, std::vector<std::string> options, std::vector<void(*)(Gui*)> functions)
{
    menu_name = name;
    menu_options = options;
    num_options = static_cast<int>(options.size());
    menu_functions = functions;
}

void Menu::print()
{
    std::cout << menu_name << std::endl;
    int option_number = 0;
    for(auto& option : menu_options)
    {
        std::cout << "[" << option_number++ << "] " << option << std::endl;
    }
}
//=================================================================================================
// GUI Object
//=================================================================================================
class Gui
{
public:
    // constructors
    Gui::Gui();
    Gui::Gui(Gui*);

    // gui setup
    void set_header(std::vector<std::string>);
    void add_menu(std::shared_ptr<Menu>);
    
    // determine/update gui state
    int get_current_menu();
    void set_current_menu(int);
    bool get_exit_status();
    void set_exit_status(bool);
    bool get_file_loaded_status();

    // audio file interactions
    void load_audio_file();
    void save_audio_file();
    void set_filename_display_alias(std::string);
    std::string get_filename_display_alias();

    // iteractions
    void display();
    void execute();
    int get_int_from_user(int, int);
    std::string get_str_from_user();

    // communications
    void send_message(std::string);
    std::string recv_message();
    
    void send_audio();
    void recv_audio();

protected:
    // gui state
    bool exit_status;
    int current_menu;
    bool file_loaded_status;

    // gui audio file data
    AudioFile<float> file;
    std::string filename_display_alias;
    std::string loaded_file_path;

    // gui menu data
    std::vector<std::string> header;
    std::vector<std::shared_ptr<Menu>> menus;
    int num_menus;

    // gui communication pipe
    std::unique_ptr<UpstreamPipe> pipe;

    // gui functions
    void print_header();
    void print_menu(std::string);
};

// default constructor
Gui::Gui()
{
    // init state
    current_menu = 0;
    exit_status = false;
    file_loaded_status = false;
        
    // init configuration
    header.push_back("");
    num_menus = 0;
    
    // init audio data
    filename_display_alias = "No file loaded";
    file.shouldLogErrorsToConsole(false);

    // init pipe
    pipe = std::make_unique<UpstreamPipe>(5500);
    pipe->connect();
};

// default constructor
Gui::Gui(Gui*)
{
    // init state
    current_menu = 0;
    exit_status = false;
    file_loaded_status = false;
        
    // init configuration
    header.push_back("");
    num_menus = 0;
    
    // init audio data
    filename_display_alias = "No file loaded";
    file.shouldLogErrorsToConsole(false);

    // init pipe
    pipe = std::make_unique<UpstreamPipe>(5500);
    pipe->connect();
};

// set the gui header
void Gui::set_header(std::vector<std::string> new_header)
{
    header = new_header;
}

// add a menu
void Gui::add_menu(std::shared_ptr<Menu> new_menu)
{
    menus.push_back(new_menu);
    num_menus++;
}
// display gui
void Gui::display()
{
    system("CLS");
    print_header();
    menus[current_menu]->print();
}

// get menu option choice and execute associated function
void Gui::execute()
{
    int choice = get_int_from_user(0, menus[current_menu]->num_options);
    menus[current_menu]->menu_functions[choice](this);
}

// return exit status
bool Gui::get_exit_status()
{
    return exit_status;
}

// set exit status
void Gui::set_exit_status(bool new_exit_status)
{
    exit_status = new_exit_status;
}

// get current menu
int Gui::get_current_menu()
{
    return current_menu;
}

// set the current menu
void Gui::set_current_menu(int menu_number)
{
    if(menu_number > num_menus)
    {
        menu_number = num_menus;
    }
    if(menu_number < 0)
    {
        menu_number = 0;
    }
    current_menu = menu_number;
}

bool Gui::get_file_loaded_status()
{
    return file_loaded_status;
}

// print gui header
void Gui::print_header()
{
    std::cout << header[0] << std::endl;
    std::cout << header[1] << filename_display_alias << std::endl;
    std::cout << header[2] << std::endl;
}

// print menu
void Gui::print_menu(std::string menu_to_print)
{
    print_header();
    for(auto& menu : menus)
    {
        if(menu->menu_name.compare(menu_to_print) == 0)
        {
            menu->print();
            return;
        }
    }
}

// get input from the user
int Gui::get_int_from_user(int lowerbound, int upperbound)
{
    int choice;
    std::cout << "Enter (" << lowerbound << "-" << upperbound << "): ";
    std::cin >> choice;
    if(choice < lowerbound || choice > upperbound)
    {
        std::cout << std::endl << "Error: choice out of bounds!" << std::endl;
        return get_int_from_user(lowerbound, upperbound);
    }
    return choice;
}

// get input from the user
std::string Gui::get_str_from_user()
{
    std::string user_input;
    std::cin >> user_input;
    return user_input;
}

// load an audio file
void Gui::load_audio_file()
{
    bool successful_load = false;
    while(!successful_load)
    {
        std::cout << "Enter Load Filepath: ";
        std::string path = get_str_from_user();
        file.load(path);
        if(file.getNumChannels() == 2)
        {
            successful_load = true;
            filename_display_alias = path;       
            file_loaded_status = true;
            return;
        }
    }
    return;
}

// save an audio file
void Gui::save_audio_file()
{
    std::cout << "Enter Save Filepath: ";
    std::string path = get_str_from_user();
    
    // give warning and require confirmation is user attempts to overwrite file
    std::ifstream file_check(path);
    if(file_check.good())
    {
        std::cout << "WARNING: This file already exists. Overwrite?" << std::endl;
        std::cout << "[0] Cancel" << std::endl;
        std::cout << "[1] Overwrite" << std::endl;
        int choice = get_int_from_user(0, 1);
        if(choice == 0)
        {
            return;
        }
        if(choice == 1)
        {
            file.save(path);
        }
    }
    else{
        file.save(path);
    }
    return;
}

void Gui::set_filename_display_alias(std::string alias)
{
    filename_display_alias = alias;
}

std::string Gui::get_filename_display_alias()
{
    return filename_display_alias;
}

void Gui::send_message(std::string msg)
{
    pipe->send_message(msg);
}

std::string Gui::recv_message()
{
    return pipe->recv_message();
}

void Gui::send_audio()
{
    pipe->send_audio(&(file.samples));
}

void Gui::recv_audio()
{
    std::shared_ptr<AudioFile<float>::AudioBuffer> manipulated_audio = pipe->recv_audio();
    file.setAudioBuffer(*manipulated_audio);
}

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