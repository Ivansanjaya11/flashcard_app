#include <iostream>
#include <CLI11.hpp>
#include <tabulate.hpp>
#include <json.hpp>
#include <filesystem>
#include <windows.h>
#include <random>
#include <nfd.h>

using namespace std;
using namespace tabulate;
using nlohmann::json;

enum MenuLevel {MAIN_MENU, DECK_MENU, CARD_MENU, QUIZ_MENU, IMPORT_EXPORT_MENU};

void navigate_menu(int &option, vector<string> &decks, json &progress_data, json &data, string &chosen_deck, MenuLevel &current_menu);

void main_menu(int &option, vector<string> &decks, json &progress_data, json &data, MenuLevel &current_menu);
void save_data_to_file(json &progress_data, json &data);

void import_export_menu(int &option, json &progress_data, json &data, MenuLevel &current_menu);

void deck_menu(int &option, vector<string> &decks, json &data, string &chosen_deck, MenuLevel &current_menu);
void create_new_deck(int &option, vector<string> &decks, json &data);
void rename_a_deck(int &option, vector<string> &decks, json &data);
void delete_a_deck(int &option, vector<string> &decks, json &data);

void card_menu(int &option, vector<string> &decks, json &progress_data, json &data, string &chosen_deck, MenuLevel &current_menu);
void create_card(int &option, vector<string> &decks, json &data, string &chosen_deck);
void update_card(int &option, vector<string> &decks, json &data, string &chosen_deck);
void delete_card(int &option, vector<string> &decks, json &data, string &chosen_deck);

void quiz_menu(int &option, vector<string> &decks, json &progress_data, json &data, MenuLevel &current_menu);
void quiz(int &option, vector<string> &decks, json &progress_data, json data, bool random);

void progress_tracking_page(json &progress_data, json &data);
void update_is_review(json &progress_data, string name);
void update_after_quiz(json &progress_data, string name);

void update_is_after_question(json &progress_data, string name, string question, bool is_correct);

void import_page(json &progress_data, json &data);
void export_page(int &option, json &data);

int max_of_2_values(int a, int b);
int min_of_2_values(int a, int b);

int main(int argc, char **argv) {

    cout << "Welcome to CS flashcard game!" << endl;
    int ptr_option;
    int* option;
    option = &ptr_option;

    // Opens existing json file, otherwise, create new json file for storing decks and cards
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

    // create a vector of deck names to make accessing the names easier
    vector<string> decks;
    for(auto& deck : data.items()) {
        decks.push_back(deck.key());
    }

    // Opens existing progress json file, otherwise create the new json file
    json progress_data = {};
    string progress_json_path = filesystem::current_path().string();
    string progress_filename = "progress";
    ifstream progress_input_file(progress_json_path + "/" + progress_filename + ".json");
    cout << "Opening the progress json file at: " << progress_json_path << endl;
    if (progress_input_file.is_open()) {
        progress_input_file >> progress_data;
        progress_input_file.close();
        // for loop body will make sure the progress data is up to date with the content of main json data
        for(int i=0;i<decks.size();i++) {
            string chosen_deck = decks[i];
            if(!progress_data.contains(chosen_deck)) {
                json progress_one_deck;
                progress_one_deck["number_reviewed"] = 0;
                progress_one_deck["is_mastered"] = 0;
                for(auto& [question, answer] : data[chosen_deck].items()) {
                    progress_one_deck[question] = {0, 0};
                }
                progress_data[chosen_deck] = progress_one_deck;
            }else {
                if(!progress_data[chosen_deck].contains("number_reviewed")) {
                    progress_data[chosen_deck]["number_reviewed"] = 0;
                }
                if(!progress_data[chosen_deck].contains("is_mastered")) {
                    progress_data[chosen_deck]["is_mastered"] = 0;
                }
                for(auto& [question, answer] : data[chosen_deck].items()) {
                    if(!progress_data[chosen_deck].contains(question)) {
                        progress_data[chosen_deck][question] = {0, 0};
                    }
                }
            }
        }
        cout << "Successfully loaded progress data" << endl;
    }else {
        cout << "No existing progress data found."<< endl;
        cout << "Creating new file...." << endl;
        ofstream progress_output_file(progress_json_path + "/" + progress_filename + ".json");
        if(progress_output_file.is_open()) {
            for(int i=0;i<decks.size();i++) {
                json progress_one_deck;
                progress_one_deck["number_reviewed"] = 0;
                progress_one_deck["is_mastered"] = 0;
                progress_data[decks[i]] = progress_one_deck;
            }
            progress_output_file << progress_data.dump(4);
            cout << "Successfully created new file." << endl;
        }else {
            cout << "Error creating new file." << endl;
            exit(-1);
        }
    }

    // initialize currentMenu as main menu
    MenuLevel currentMenu = MAIN_MENU;

    // make a chosen_deck pointer variable that points to a string variable
    // The purpose is to keep track of the chosen deck whenever it changes when passed to different pages
    string str = "";
    string* chosen_deck = &str;

    // call the navigate menu to access other pages, starting with the main menu
    while (true) {
        navigate_menu(*option, decks, progress_data, data, *chosen_deck, currentMenu);
    }

    return 0;
}

// keeps track of the page being currently accessed
void navigate_menu(int &option, vector<string> &decks, json &progress_data, json &data, string &chosen_deck, MenuLevel &current_menu) {
    switch(current_menu) {
        case MAIN_MENU:
            main_menu(option, decks, progress_data, data, current_menu);
            break;
        case DECK_MENU:
            deck_menu(option, decks, data, chosen_deck, current_menu);
            break;
        case CARD_MENU:
            card_menu(option, decks, progress_data, data, chosen_deck, current_menu);
            break;
        case QUIZ_MENU:
            quiz_menu(option, decks, progress_data, data, current_menu);
            break;
        case IMPORT_EXPORT_MENU:
            import_export_menu(option, progress_data, data, current_menu);
    }
}

// saves to main json file and progress json file
void save_data_to_file(json &progress_data, json &data) {
    string json_path = filesystem::current_path().string();
    string filename = "decks";
    ofstream file(json_path + "/" + filename + ".json");
    if(!file.is_open()) {
        cerr << "Error opening file '" << json_path + filename << ".json'!" << endl;
        exit(-1);
    }
    file << data.dump(4);
    file.close();

    string progress_json_path = filesystem::current_path().string();
    string progress_filename = "progress";
    ofstream progress_file(progress_json_path + "/" + progress_filename + ".json");
    if(!progress_file.is_open()) {
        cerr << "Error opening file '" << progress_json_path + progress_filename << ".json'!" << endl;
        exit(-1);
    }
    progress_file << progress_data.dump(4);
    progress_file.close();
}

// the first menu displayed
void main_menu(int &option, vector<string> &decks, json &progress_data, json &data, MenuLevel &current_menu) {
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
        progress_tracking_page(progress_data, data);
    }else if(option==3) {
        save_data_to_file(progress_data, data);
        cout << "Successfully saved data to file." << endl;
        cout << "Closing the program...." << endl;
        exit(0);
    }
}

// allows CRUD operations on decks, go back to main menu, import/export decks, and access quiz page
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
    }else if(option==6) {
        current_menu = MAIN_MENU;
    }else {
        option -= 7;
        chosen_deck = decks[option];
        current_menu = CARD_MENU;
    }
}

// creates new deck as a json object
void create_new_deck(int &option, vector<string> &decks, json &data) {
    string str;
    cout << "Enter the name of the deck:";
    getline(cin >> ws, str);
    decks.push_back(str);
    data[str] = json::object();
    cout << "Successfully added a deck." << endl;
    cout << "Returning to previous page...." << endl;
}

// renames a deck by creating a new key value pair in the json object and remove the old one
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

// deletes the chosen deck
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

// allows CRUD operations on card and to go back to deck menu
void card_menu(int &option, vector<string> &decks, json &progress_data, json &data, string &chosen_deck, MenuLevel &current_menu) {
    update_is_review(progress_data, chosen_deck);
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

// creates a new card (by taking 2 inputs for Q and A)
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

// changes the question and answer of a card by creating a new key-value pair in json object and removes the old one
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

// deletes the chosen card (removes the key-value pair)
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

// allows to choose which deck the user wants to do quiz on and to go back to deck menu
// allows the user to choose linear or random mode
void quiz_menu(int &option, vector<string> &decks, json &progress_data, json &data, MenuLevel &current_menu) {
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
        quiz(option, decks, progress_data, data, is_random);
    }
}

// mechanism to allow the user to do quiz on a chosen deck
void quiz(int &option, vector<string> &decks, json &progress_data, json data, bool random) {
    string chosen_deck_name = decks[option-2];
    if(data[chosen_deck_name].size()==0) {
        cout << "This deck has no cards to quiz you on. Returning to previous menu..." << endl;
        return ;
    }
    // make a vector copy of questions. This allows easier access of question names
    // Another reason is if the user choose random, it is easier to shuffle randomly a vector data type
    vector<string> temp_questions;
    for(auto& [question, answer] : data[chosen_deck_name].items()){
        temp_questions.push_back(question);
    }
    // if user choose random,,will shuffle the questions randomly
    if(random) {
        random_device rd;
        mt19937 g(rd());
        shuffle(temp_questions.begin(), temp_questions.end(), g);
    }
    // keep track of correct amount and the number of cards left on deck that hasn't showed up on the quiz
    int card_left = data[chosen_deck_name].size();
    int correct_count = 0;
    for(auto& question : temp_questions){
        cout << "Q: " << question << endl;
        cout << "   A: ";
        string my_answer;
        getline(cin >> ws, my_answer);
        string answer = data[chosen_deck_name][question];
        while(true) {
            // wait until user press return key to display correct answer
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
            update_is_after_question(progress_data, chosen_deck_name, question, true);
        }else {
            cout << "Whoops! Try to learn more next time!" << endl;
            update_is_after_question(progress_data, chosen_deck_name, question, false);
        }
        // decrement the card_left variable until it is 0, in which case, break from the loop
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
        update_after_quiz(progress_data, chosen_deck_name);
        cout << "Excellent! Perfect score!" << endl;
    }
    cout << "Returning to previous page...." << endl;

}

void import_export_menu(int &option, json &progress_data, json &data, MenuLevel &current_menu) {
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
        import_page(progress_data, data);
    }else if(option==2) {
        export_page(option, data);
    }else {
        current_menu = DECK_MENU;
    }
}

void import_page(json &progress_data, json &data) {
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
            save_data_to_file(progress_data, data);
            cout << "Deck imported successfully." << endl;
        }else {
            cerr << "Unable to open file." << endl;
        }
        free(outpath);
    }else if(result == NFD_CANCEL) {
        cout << "Cancelled file selection." << endl;
    }else {
        cerr << "Error: " << NFD_GetError() << endl;
    }
}

void export_page(int &option, json &data) {
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
    string filepath = filesystem::current_path().string();
    filesystem::path path = filepath + "/" + filename;
    if(filesystem::exists(path)) {
        char choice;
        cout << "File already exists in directory. Do you want to overwrite it? (y/n)" << endl;
        cin >> choice;
        if (choice != 'y') {
            cout << "Export canceled." << endl;
            return;
        }
    }
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

void progress_tracking_page(json &progress_data, json &data) {
    string current_max = "None";
    int current_max_reviewed_count = 0;
    // most reviewed
    for(auto &deck : progress_data.items()) {
        if(current_max_reviewed_count < max_of_2_values(deck.value()["number_reviewed"], current_max_reviewed_count)) {
            current_max_reviewed_count = max_of_2_values(deck.value()["number_reviewed"], current_max_reviewed_count);
            current_max = deck.key();
        }
    }
    cout << "Most reviewed deck: " << current_max << endl;

    // number of reviewed for each deck
    Table reviewed_count_table;
    reviewed_count_table.format()
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner(" ");
    reviewed_count_table.add_row({"Deck", "Number of times reviewed"});
    reviewed_count_table[0].format()
        .padding_top(1)
        .padding_bottom(1)
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold})
        .font_color(Color::red)
        .font_background_color(Color::white);
    for(auto deck : progress_data.items()) {
        if(deck.value().contains("number_reviewed")) {
            int number_reviewed = deck.value()["number_reviewed"];
            reviewed_count_table.add_row({deck.key(), to_string(number_reviewed)});
        }
    }
    cout << reviewed_count_table << endl;

    // most mastered
    int current_max_mastered_count = 0;
    for(auto &deck : progress_data.items()) {
        if(current_max_mastered_count < max_of_2_values(deck.value()["number_reviewed"], current_max_mastered_count)) {
            current_max_mastered_count = max_of_2_values(deck.value()["number_reviewed"], current_max_mastered_count);
            current_max = deck.key();
        }
    }
    cout << "Deck with best performance: " << current_max << endl;

    int mastered_count = 0;
    int total_deck = 0;
    //percentage of mastered
    Table mastered_percentage;
    for(auto &deck : progress_data.items()) {
        if(deck.value().contains("is_mastered") && deck.value()["is_mastered"] > 0) {
            mastered_count++;
        }
        total_deck++;
    }
    float percentage_of_mastered = ((1.0*mastered_count)/total_deck)*100;
    float percentage_of_not_mastered = 100 - percentage_of_mastered;
    ostringstream out_percent_mastered;
    out_percent_mastered << fixed << setprecision(2) << percentage_of_mastered;
    ostringstream out_percent_not_mastered;
    out_percent_not_mastered << fixed << setprecision(2) << percentage_of_not_mastered;

    mastered_percentage.add_row({out_percent_mastered.str() + "%", out_percent_not_mastered.str() + "%"});
    mastered_percentage[0].format()
        .padding_top(1)
        .padding_bottom(1)
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold});
    mastered_percentage[0][0].format()
        .font_background_color(Color::green)
        .font_color(Color::red)
        .width(percentage_of_mastered);
    mastered_percentage[0][1].format()
        .font_background_color(Color::red)
        .font_color(Color::green)
        .width(100-percentage_of_mastered);
    cout << "Chart of percentage of mastered decks: " << endl;
    cout << mastered_percentage << endl;

    //how many time every deck is mastered
    Table mastered_count_table;
    mastered_count_table.format()
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner(" ");
    mastered_count_table.add_row({"Deck", "Number of times mastered"});
    mastered_count_table[0].format()
        .padding_top(1)
        .padding_bottom(1)
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold})
        .font_color(Color::red)
        .font_background_color(Color::white);
    for(auto &deck : progress_data.items()) {
        if(deck.value().contains("is_mastered")) {
            mastered_count_table.add_row({deck.key(), to_string(deck.value()["is_mastered"])});
        }
    }
    cout << mastered_count_table << endl;

    // best and least performing card
    Table card_table;
    card_table.format()
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner(" ");
    card_table.add_row({"Deck", "Best performing card", "Worst performing card"});
    card_table[0].format()
        .padding_top(1)
        .padding_bottom(1)
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold})
        .font_color(Color::red)
        .font_background_color(Color::white);
    for(auto& deck : progress_data.items()) {
        string current_max_question;
        int current_max_card = 0;
        string current_min_question;
        int current_min_card = numeric_limits<int>::max();
        for(auto& question : deck.value().items()) {
            if(question.key() != "number_reviewed" && question.key() != "is_mastered") {
                if(current_max_card < max_of_2_values(question.value()[0], current_max_card)) {
                    current_max_card = max_of_2_values(question.value()[0], current_max_card);
                    current_max_question = question.key();
                }
                if(current_min_card > min_of_2_values(question.value()[0], current_min_card)) {
                    current_min_card = min_of_2_values(question.value()[0], current_min_card);
                    current_min_question = question.key();
                }
            }
        }
        card_table.add_row({deck.key(), current_max_question, current_min_question});
    }
    cout << card_table << endl;
}

void update_is_review(json &progress_data, string name) {
    if(progress_data[name].contains("number_reviewed")) {
        progress_data[name]["number_reviewed"] = progress_data[name]["number_reviewed"].get<int>() + 1;
    }else {
        progress_data[name]["number_reviewed"] = 1;
    }

}

void update_after_quiz(json &progress_data, string name) {
    if(progress_data[name].contains("is_mastered")) {
        progress_data[name]["is_mastered"]  = progress_data[name]["is_mastered"].get<int>() + 1;
    }else {
        progress_data[name]["is_mastered"] = 1;
    }
}

void update_is_after_question(json &progress_data, string name, string question, bool is_correct) {
    if(is_correct==true) {
        if(progress_data[name].contains(question)) {
            progress_data[name][question][0] = progress_data[name][question][0].get<int>() + 1;
        }else {
            progress_data[name][question][0] = 1;
        }
    }else {
        if(progress_data[name].contains(question)) {
            progress_data[name][question][1] = progress_data[name][question][0].get<int>() + 1;
        }else {
            progress_data[name][question][1] = 1;
        }
    }
}

int max_of_2_values(int a, int b) {
    if(a>b) {
        return a;
    }else {
        return b;
    }
}

int min_of_2_values(int a, int b) {
    if(a<b) {
        return a;
    }else {
        return b;
    }
}