#include <iostream>
#include <CLI11.hpp>
#include <tabulate.hpp>
#include <json.hpp>
#include <filesystem>
#include <windows.h>
#include <random>
#include <nfd.h>
using namespace std;
using nlohmann::json;

enum MenuLevel {MAIN_MENU, DECK_MENU, CARD_MENU, QUIZ_MENU, IMPORT_EXPORT_MENU};

void navigate_menu(int &option, vector<string> &decks, json &data, string &chosen_deck, MenuLevel &current_menu);

void main_menu(int &option, vector<string> &decks, json &data, MenuLevel &current_menu);
void save_data_to_file(json &data);

void import_export_menu(int &option, json &data, MenuLevel &current_menu);

void deck_menu(int &option, vector<string> &decks, json &data, string &chosen_deck, MenuLevel &current_menu);
void create_new_deck(int &option, vector<string> &decks, json &data);
void rename_a_deck(int &option, vector<string> &decks, json &data);
void delete_a_deck(int &option, vector<string> &decks, json &data);

void card_menu(int &option, vector<string> &decks, json &data, string &chosen_deck, MenuLevel &current_menu);
void create_card(int &option, vector<string> &decks, json &data, string &chosen_deck);
void update_card(int &option, vector<string> &decks, json &data, string &chosen_deck);
void delete_card(int &option, vector<string> &decks, json &data, string &chosen_deck);

void quiz_menu(int &option, vector<string> &decks, json &data, MenuLevel &current_menu);
void quiz(int &option, vector<string> &decks, json data, bool random);

void progress_tracking_page(json &progress_data, json &data);
void update_progress(json &progress_data, json &data);

void import_page(json &data);
void export_page(int &option, json &data);

int main(int argc, char **argv) {

    cout << "Welcome to CS flashcard game!" << endl;
    int ptr_option;
    int* option;
    option = &ptr_option;

    json data = {};
    string json_path = filesystem::current_path().string();
    string filename = "decks";
    ifstream input_file(json_path + "/" + filename + ".json");
    cout << "Opening the json file at: " << json_path << endl;
    if (input_file.is_open()) {
        input_file >> data;
        input_file.close();
        cout << "Successfully loaded deck data" << endl;
    }else {
        cout << "No existing deck data found."<< endl;
        cout << "Creating new file...." << endl;
        ofstream output_file(json_path + "/" + filename + ".json");
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
        case QUIZ_MENU:
            quiz_menu(option, decks, data, current_menu);
            break;
        case IMPORT_EXPORT_MENU:
            import_export_menu(option, data, current_menu);
    }
}

void save_data_to_file(json &data) {
    string json_path = filesystem::current_path().string();
    string filename = "decks";
    ofstream file(json_path + "/" + filename + ".json");
    if(!file.is_open()) {
        cerr << "Error opening file '" << json_path + filename << ".json'!" << endl;
        exit(-1);
    }
    file << data.dump(4);
    file.close();
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
        //progress_tracking_page(progress_data, data);
    }else if(option==3) {
        save_data_to_file(data);
        cout << "Successfully saved data to file." << endl;
        cout << "Closing the program...." << endl;
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
        current_menu=QUIZ_MENU;
    }else if(option==5) {
        current_menu=IMPORT_EXPORT_MENU;
        //EXPORT/IMPORT 3
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

void quiz_menu(int &option, vector<string> &decks, json &data, MenuLevel &current_menu) {
    do {
        cout << "Choose which deck you want to quiz on: " << endl;
        cout << "1. Back" << endl;
        for(int i=0;i<decks.size();i++) {
            cout << i+2 << ". " << decks[i] << endl;
        }
        cin >> option;
        if(option<1 || option>decks.size()+1) {
            cout << "Input not recognized as a valid option." << endl;
        }
    }while(option<1 || option>decks.size()+1);
    bool is_random=false;
    int mode_option;
    if(option!=1) {
        do {
            cout << "Choose linear or random mode: " << endl;
            cout << "1. linear mode" << endl;
            cout << "2. random mode" << endl;
            cin >> mode_option;
            if(mode_option<1 || mode_option>2) {
                cout << "Input not recognized as a valid option." << endl;
            }
        }while(mode_option<1 || mode_option>2);
    }
    if(option==1) {
        current_menu = DECK_MENU;
    }else {
        if(mode_option==1) {
            is_random = false;
        }else {
            is_random=true;
        }
        quiz(option, decks, data, is_random);
    }
}

void quiz(int &option, vector<string> &decks, json data, bool random) {
    string chosen_deck_name = decks[option-2];
    if(data[chosen_deck_name].size()==0) {
        cout << "This deck has no cards to quiz you on. Returning to previous menu..." << endl;
        return ;
    }
    vector<string> temp_questions;
    for(auto& [question, answer] : data[chosen_deck_name].items()){
        temp_questions.push_back(question);
    }
    if(random) {
        random_device rd;
        mt19937 g(rd());
        shuffle(temp_questions.begin(), temp_questions.end(), g);
    }
    int card_left = data[chosen_deck_name].size();
    int correct_count = 0;
    for(auto& question : temp_questions){
        cout << "Q: " << question << endl;
        cout << "   A: ";
        string my_answer;
        getline(cin >> ws, my_answer);
        string answer = data[chosen_deck_name][question];
        while(true) {
            if(GetAsyncKeyState(VK_RETURN) & 0x8000 ) {
                cout << "The correct answer is: " << answer << endl;
                break;
            }
            Sleep(50);
        }
        string is_correct;
        do {
            cin.clear();
            cout << "did you get it right? (y/n)"<<endl;
            cin >> is_correct;
            if(is_correct!="y" && is_correct!="n") {
                cout << "Woah. Let's try again. ";
            }
        }while(is_correct!="y" && is_correct!="n");
        if(is_correct=="y") {
            correct_count++;
            cout << "Congratz! You got this one correct." << endl;
        }else {
            cout << "Whoops! Try to learn more next time!" << endl;
        }
        card_left--;
        if(card_left==0) {
            break;
        }
    }
    cout << "You've reached the end of this deck." << endl;
    double score = (1.0*correct_count/data[chosen_deck_name].size())*100.0;
    cout << "You got " << correct_count << " out of " << data[chosen_deck_name].size() << " correct!" << endl;
    cout << "Your score for deck '" << chosen_deck_name << "' is " << score << endl;
    if(score<75) {
        cout << "You're not doing well. Learn more!"<<endl;
    }else if(score <90) {
        cout << "Hmm... Not bad. Still, Learning more can't hurt." << endl;
    }else if(score<100) {
        cout << "You're doing well. Keep it up! And don't forget to learn a bit more." << endl;
    }else {
        cout << "Excellent! Perfect score!" << endl;
    }
    cout << "Returning to previous page...." << endl;

}

void import_export_menu(int &option, json &data, MenuLevel &current_menu) {
    do {
        cout << "Choose what action you want to do: " << endl;
        cout << "1. Import a deck" << endl;
        cout << "2. Export a deck" << endl;
        cout << "3. Back" << endl;
        cin >> option;
        if(option<1 || option>3) {
            cout << "Input not recognized as a valid option." << endl;
        }
    }while(option<1 || option>3);
    if(option==1) {
        import_page(data);
    }else if(option==2) {
        export_page(option, data);
    }else {
        current_menu = DECK_MENU;
    }
}

void import_page(json &data) {
    nfdchar_t *outpath = NULL;
    nfdresult_t result = NFD_OpenDialog("json", NULL, &outpath);
    if(result==NFD_OKAY) {
        cout << "file selected: " << outpath << endl;
        ifstream imported_file(outpath);
        if(imported_file.is_open()) {
            json imported_deck;
            imported_file >> imported_deck;
            imported_file.close();
            for(auto& [deck, QA] : imported_deck.items()) {
                if(data.contains(deck)) {
                    cout << "A deck with the name '" << deck << "' already exists." << endl;
                    cout << "Do you want to overwrite it? (y/n):" << " ";
                    char choice;
                    cin >> choice;
                    if (choice != 'y') {
                        cout << "Import canceled." << endl;
                        return;
                    }
                }
                data[deck] = QA;
            }
            save_data_to_file(data);
            cout << "Deck imported successfully." << endl;
        }else {
            cerr << "Unable to open file." << endl;
        }
        free(outpath);
        // try using this:
        // NFD_FreePath(outpath);
    }else if(result == NFD_CANCEL) {
        cout << "Cancelled file selection." << endl;
    }else {
        cerr << "Error: " << NFD_GetError() << endl;
    }
}

void export_page(int &option, json &data) {
    //handle conflicts/duplicate names
    // for example, when saving to a file, if that file already exist in that directory, prompt the question:
    // wanna overwrite it?
    if(data.size()==0) {
        cout << "You have no deck to export!" << endl;
        return ;
    }
    int numbering = 1;
    vector<string> deck_names;
    do {
        cout << "Choose the deck you want to import: " << endl;
        for(auto& [deck, QA] : data.items()) {
            cout << numbering++ << ". " << deck << endl;
            deck_names.push_back(deck);
        }
        cin >> option;
        if(option<1 || option > numbering) {
            cout << "Input not recognized as a valid input." << endl;
        }
    }while(option<1 || option > numbering);
    string filename = deck_names[option-1]+"_export.json";
    ofstream exported_file(filename);
    if(exported_file.is_open()) {
        json dump_the_data = {{deck_names[option-1], data[deck_names[option-1]]}};
        exported_file << dump_the_data.dump(4);
        exported_file.close();
        cout << "Deck '" << deck_names[option-1] << "' successfully exported." << endl;
    }else {
        cout << "Unable to create exported file." << endl;
    }
}
