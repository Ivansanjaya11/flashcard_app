#include <iostream>
#include <CLI11.hpp>
#include <tabulate.hpp>
#include <json.hpp>
using namespace std;
using nlohmann::json;

void main_menu(int &option, vector<string> &decks, json data);
void save_data_to_file();

void deck_menu(int &option, vector<string> &decks, json data);
void open_deck_folder(int &option, vector<string> &decks, int deck_folder_index);
void create_new_deck(int &option, vector<string> &decks, json data);
void rename_a_deck(int &option, vector<string> &decks, json data);
void delete_a_deck(int &option, vector<string> &decks, json data);

void card_menu(int &option, vector<string> &decks, json data);
void create_card(int &option, vector<string> &decks, json data);
void update_card(int &option, vector<string> &decks, json data);
void delete_card(int &option, vector<string> &decks, json data);

void review_menu();
void linear_review();
void random_review();

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

    main_menu(*option, decks, data);

    return 0;
}

void main_menu(int &option, vector<string> &decks, json data) {
    do{
        cout << "Choose what you want to do: " << endl;
        cout << "1. Go to cards" << endl;
        cout << "2. Progres tracking" << endl;
        cout << "3. Exit" << endl;
        cin >> option;
        if(option<1 || option>3) {
            cout << "Input not recognized as a valid option." << endl;
        }
    }while(option<1 || option>3);
    if(option==1) {
        card_menu(option, decks, data);
    }else if(option==2) {
        //progress_tracking_page();
    }else if(option==3) {
        //save data to json file
        exit(0);
    }
}

void deck_menu(int &option, vector<string> &decks, json data){
    do{
        cout << "Choose what you want to do: " << endl;
        cout << "1. Create new deck" << endl;
        cout << "2. Rename a deck" << endl;
        cout << "3. Delete a deck" << endl;
        cout << "4. Review a deck" << endl;
        cout << "5. Quiz a deck" << endl;
        cout << "6. Export/import a deck" << endl;
        cout << "7. Back" << endl;
        for(int i=0;i<decks.size();i++) {
            cout << i+8 << ". " << decks[i] << endl;
        }
        cin >> option;
        if(option<1 || option>decks.size()+7) {
            cout << "Input not recognized as a valid option." << endl;
        }
    }while(option<1 || option>decks.size()+7);
    if(option==1) {
        create_new_deck(option, decks, data);
    }else if(option==2) {
        rename_a_deck(option, decks, data);
    }else if(option==3) {
        delete_a_deck(option, decks, data);
    }else if(option==4) {

    }else if(option==5) {

    }else if(option==6) {

    }else if(option==7) {
        main_menu(option, decks, data);
    }else {
        option -= 8;
        card_menu(option, decks, data);
    }
}

void create_new_deck(int &option, vector<string> &decks, json data) {
    string str;
    cout << "Enter the name of the deck:";
    getline(cin >> ws, str);
    decks.push_back(str);
    data[str] = {};
    cout << "Successfully added a deck." << endl;
    cout << "Returning to previous page...." << endl;
    card_menu(option, decks, data);
}

void rename_a_deck(int &option, vector<string> &decks, json data) {
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
    card_menu(option, decks, data);
}

void delete_a_deck(int &option, vector<string> &decks, json data) {
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
    card_menu(option, decks, data);
}

void card_menu(int &option, vector<string> &decks, json data) {
    do {
        cout << "Choose between 1-4" << endl;
        cout << "1. Create a new card" << endl;
        cout << "2. Update a card" << endl;
        cout << "3. Delete a card" << endl;
        cout << "4. Back" << endl;
        int numbering=5;
        for(auto& [key, value] : data[option].items()) {
            cout << numbering << ". " << key << endl;
            cout << "\t" << value << endl;
            numbering++;
        }
        cin >> option;
        if(option<1 || option>4) {
            cout << "Input not recognized as a valid option." << endl;
        }
    }while(option<1 || option>4);
    if(option==1) {

    }else if(option==2) {

    }else if(option==3) {

    }else if(option==4) {
        card_menu(option, decks, data);
    }
}