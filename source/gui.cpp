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
    // constructor
    Gui::Gui();
    Gui::Gui(Gui*);
    
    // change gui state
    void set_current_menu(int);
    bool get_exit_status();
    void set_exit_status(bool);
    
    // getters and setters for gui data
    void set_header(std::vector<std::string>);
    
    void add_menu(std::shared_ptr<Menu>);
    
    void get_current_menu();

    void load_audio_file();
    void save_audio_file();
    
    void set_file_display_name(std::string);
    void get_file_display_name();

    // iteract with gui
    void display();
    void execute();
    
    int get_int_from_user(int, int);
    std::string get_str_from_user();

protected:
    // gui state
    bool exit_status;
    int current_menu;

    // gui audio file data
    AudioFile<float> file;
    std::string loaded_file_name;
    std::string loaded_file_path;

    // gui menu data
    std::vector<std::string> header;
    std::vector<std::shared_ptr<Menu>> menus;
    int num_menus;
    
    // gui functions
    void print_header();
    void print_menu(std::string);
};

// default constructor
Gui::Gui()
{
    current_menu = 0;
    loaded_file_name = "No file loaded";
    file.shouldLogErrorsToConsole(false);
    header.push_back("");
    num_menus = 0;
    exit_status = false;
};

// default constructor
Gui::Gui(Gui*)
{
    current_menu = 0;
    loaded_file_name = "No file loaded";
    file.shouldLogErrorsToConsole(false);
    header.push_back("");
    num_menus = 0;
    exit_status = false;
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

// print gui header
void Gui::print_header()
{
    std::cout << header[0] << std::endl;
    std::cout << header[1] << loaded_file_name << std::endl;
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
        if(file.getNumChannels())
        {
            successful_load = true;
            loaded_file_name = path;       
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
    if(path.compare(loaded_file_path) == 0)
    {
        std::cout << "WARNING: This will overwrite your file. Continue?" << std::endl;
        std::cout << "[0] Cancel" << std::endl;
        std::cout << "[1] Overwrite file" << std::endl;
        int choice = get_int_from_user(0, 1);
        if(choice == 0)
        {
            return;
        }
        if(choice == 1)
        {
            file.save(path);
            // reload file
            file.load(path);
        }
    }
    else{
        file.save(path);
    }
    return;
}

//=================================================================================================
// Main Menu Functions
//=================================================================================================
void main_menu_exit_program(Gui* gui)
{
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
    std::cout << "file options option 2" << std::endl; 
}

void audio_effects_stretch(Gui* gui)
{
    std::cout << "file options option 3" << std::endl; 
}

void audio_effects_repitch(Gui* gui)
{
    std::cout << "file options option 4" << std::endl; 
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