#include <iostream>
#include <CLI11.hpp>
#include <tabulate.hpp>
#include <json.hpp>
#include <filesystem>
using namespace std;
using nlohmann::json;

enum MenuLevel {MAIN_MENU, DECK_MENU, CARD_MENU};

void navigate_menu(int &option, vector<string> &decks, json &data, string &chosen_deck, MenuLevel &current_menu);

void main_menu(int &option, vector<string> &decks, json &data, MenuLevel &current_menu);
void save_data_to_file();

void deck_menu(int &option, vector<string> &decks, json &data, string &chosen_deck, MenuLevel &current_menu);
void create_new_deck(int &option, vector<string> &decks, json &data);
void rename_a_deck(int &option, vector<string> &decks, json &data);
void delete_a_deck(int &option, vector<string> &decks, json &data);

void card_menu(int &option, vector<string> &decks, json &data, string &chosen_deck, MenuLevel &current_menu);
void create_card(int &option, vector<string> &decks, json &data, string &chosen_deck);
void update_card(int &option, vector<string> &decks, json &data, string &chosen_deck);
void delete_card(int &option, vector<string> &decks, json &data, string &chosen_deck);

void quiz_menu();
void linear_quiz();
void random_quiz();

void progress_tracking_page();

int main(int argc, char **argv) {

    cout << "Welcome to CS flashcard game!" << endl;
    int ptr_option;
    int* option;
    option = &ptr_option;

    json data = {};
    string json_path = filesystem::current_path().string();
    string filename = "decks";
    ifstream input_file(json_path + "/" + filename + ".json");
    cout << "here: ";
    cout << json_path << endl;
    if (input_file.is_open()) {
        input_file >> data;
        input_file.close();
        cout << "Successfully loaded deck data" << endl;
    }else {
        cout << "No existing deck data found."<< endl;
        cout << "Creating new file...." << endl;
        ofstream output_file(json_path + filename + ".json");
        if(output_file.is_open()) {
            output_file << data.dump(4);
            cout << "Successfully created new file." << endl;
        }else {
            cout << "Error creating new file." << endl;
            exit(-1);
        }
    }

    vector<string> decks;
    for(auto& deck : data.items()) {
        decks.push_back(deck.key());
    }

    MenuLevel currentMenu = MAIN_MENU;
    string str = "";
    string* chosen_deck = &str;

    while (true) {
        navigate_menu(*option, decks, data, *chosen_deck, currentMenu);
    }

    return 0;
}

void navigate_menu(int &option, vector<string> &decks, json &data, string &chosen_deck, MenuLevel &current_menu) {
    switch(current_menu) {
        case MAIN_MENU:
            main_menu(option, decks, data, current_menu);
            break;
        case DECK_MENU:
            deck_menu(option, decks, data, chosen_deck, current_menu);
            break;
        case CARD_MENU:
            card_menu(option, decks, data, chosen_deck, current_menu);
            break;
    }
}

void main_menu(int &option, vector<string> &decks, json &data, MenuLevel &current_menu) {
    do{
        cout << "Choose what you want to do: " << endl;
        cout << "1. Go to decks" << endl;
        cout << "2. Progres tracking" << endl;
        cout << "3. Exit" << endl;
        cin >> option;
        if(option<1 || option>3) {
            cout << "Input not recognized as a valid option." << endl;
        }
    }while(option<1 || option>3);
    if(option==1) {
        current_menu = DECK_MENU;
    }else if(option==2) {
        //progress_tracking_page();
    }else if(option==3) {
        //save data to json file
        exit(0);
    }
}

void deck_menu(int &option, vector<string> &decks, json &data, string &chosen_deck, MenuLevel &current_menu){
    do{
        cout << "Choose what you want to do: " << endl;
        cout << "1. Create new deck" << endl;
        cout << "2. Rename a deck" << endl;
        cout << "3. Delete a deck" << endl;
        cout << "4. Quiz a deck" << endl;
        cout << "5. Export/import a deck" << endl;
        cout << "6. Back" << endl;
        for(int i=0;i<decks.size();i++) {
            cout << i+7 << ". " << decks[i] << endl;
        }
        cin >> option;
        if(option<1 || option>decks.size()+6) {
            cout << "Input not recognized as a valid option." << endl;
        }
    }while(option<1 || option>decks.size()+6);
    if(option==1) {
        create_new_deck(option, decks, data);
    }else if(option==2) {
        rename_a_deck(option, decks, data);
    }else if(option==3) {
        delete_a_deck(option, decks, data);
    }else if(option==4) {

    }else if(option==5) {

    }else if(option==6) {
        current_menu = MAIN_MENU;
    }else {
        option -= 7;
        chosen_deck = decks[option];
        current_menu = CARD_MENU;
    }
}

void create_new_deck(int &option, vector<string> &decks, json &data) {
    string str;
    cout << "Enter the name of the deck:";
    getline(cin >> ws, str);
    decks.push_back(str);
    data[str] = json::object();
    cout << "Successfully added a deck." << endl;
    cout << "Returning to previous page...." << endl;
}

void rename_a_deck(int &option, vector<string> &decks, json &data) {
    do {
        cout << "Choose a deck you want to rename: " << endl;
        for(int i=0;i<decks.size();i++) {
            cout << i+1 << ". " << decks[i] << endl;
        }
        cin >> option;
        if(option<1 || option>decks.size()+1) {
            cout << "Input not recognized as a valid option." << endl;
        }
    }while(option<1 || option>decks.size()+1);
    string str;
    cout << "Enter the new name for the deck: ";
    getline(cin>>ws, str);
    string old_name=decks[option-1];

    decks[option-1] = str;
    data[str] = data[old_name];
    data.erase(old_name);
    cout << "Successfully renamed deck '" << old_name << "' to '" << str << "'." << endl;
    cout << "Returning to previous page...." << endl;
}

void delete_a_deck(int &option, vector<string> &decks, json &data) {
    do {
        cout << "Choose a deck you want to delete: " << endl;
        for(int i=0;i<decks.size();i++) {
            cout << i+1 << ". " << decks[i] << endl;
        }
        cin >> option;
        if(option<1 || option>decks.size()+1) {
            cout << "Input not recognized as a valid option." << endl;
        }
    }while(option<1 || option>decks.size()+1);
    string name=decks[option-1];
    decks.erase(decks.begin()+option-1);
    data.erase(name);
    cout << "Successfully deleted a deck." << endl;
    cout << "Returning to previous page...." << endl;
}

void card_menu(int &option, vector<string> &decks, json &data, string &chosen_deck, MenuLevel &current_menu) {
    do {
        cout << "Choose between 1-4" << endl;
        cout << "1. Create a new card" << endl;
        cout << "2. Update a card" << endl;
        cout << "3. Delete a card" << endl;
        cout << "4. Back" << endl;
        int numbering=5;
        for(auto& [question, answer] : data[chosen_deck].items()) {
            cout << numbering++ << ". Q: " << question << endl;
            cout << "   A: " << answer << endl;
        }
        cin >> option;
        if(option<1 || option>4) {
            cout << "Input not recognized as a valid option." << endl;
        }
    }while(option<1 || option>4);
    if(option==1) {
        create_card(option, decks, data, chosen_deck);
    }else if(option==2) {
        update_card(option, decks, data, chosen_deck);
    }else if(option==3) {
        delete_card(option, decks, data, chosen_deck);
    }else if(option==4) {
        current_menu = DECK_MENU;
    }
}

void create_card(int &option, vector<string> &decks, json &data, string &chosen_deck) {
    string question, answer;
    cout << "Enter the question of the card: ";
    getline(cin >> ws, question);
    cout << "Enter the answer of the card: ";
    getline(cin >> ws, answer);
    data[chosen_deck][question] = answer;
    cout << "Successfully added a deck." << endl;
    cout << "Returning to previous page...." << endl;
}

void update_card(int &option, vector<string> &decks, json &data, string &chosen_deck) {
    if(data[chosen_deck].size()==0) {
        cout << "This deck has no cards to update. Returning to previous menu..." << endl;
        return ;
    }
    vector<string> questions;
    do {
        questions.clear();
        cout << "Choose a card you want to rename: " << endl;
        int numbering = 1;
        for(auto& [question, answer] : data[chosen_deck].items()) {
            cout << numbering++ << ". Q: " << question << endl;
            cout << "\tA: " << answer << endl;
            questions.push_back(question);  // Store question in the vector
        }
        cin >> option;
        if(option<1 || option>data[chosen_deck].size()) {
            cout << "Input not recognized as a valid option." << endl;
        }
    }while(option<1 || option>data[chosen_deck].size());
    string question, answer;
    cout << "Enter the new question of the card: ";
    getline(cin>>ws, question);
    cout << "Enter the new answer of the card: ";
    getline(cin >> ws, answer);

    string old_name = questions[option-1];

    data[chosen_deck][question] = answer;
    data[chosen_deck].erase(old_name);

    cout << "Successfully updated card '" << old_name << "' to '" << question << "'." << endl;
    cout << "Returning to previous page...." << endl;
}

void delete_card(int &option, vector<string> &decks, json &data, string &chosen_deck) {
    if(data[chosen_deck].size()==0) {
        cout << "This deck has no cards to delete. Returning to previous menu..." << endl;
        return ;
    }
    vector<string> questions;
    do {
        questions.clear();
        cout << "Choose a deck you want to delete: " << endl;
        int numbering = 1;
        for(auto& [question, answer] : data[chosen_deck].items()) {
            cout << numbering++ << ". Q: " << question << endl;
            cout << "\tA: " << answer << endl;
            questions.push_back(question);  // Store question in the vector
        }
        cin >> option;
        if(option<1 || option>data[chosen_deck].size()) {
            cout << "Input not recognized as a valid option." << endl;
        }
    }while(option<1 || option>data[chosen_deck].size());

    string name = questions[option-1];

    data[chosen_deck].erase(name);

    cout << "Successfully deleted a card." << endl;
    cout << "Returning to previous page...." << endl;
}