#include <iostream>
#include <fstream>
#include <utility>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <limits>
#include <cstdlib>
#include <thread>

#include "colors.h"
#include "boxes.h"

struct TodoItem {
    std::string description;
    int priority;  // -1 for non-priority items
    bool is_priority;

    TodoItem(std::string  desc, bool is_pri = false, int pri = -1)
        : description(std::move(desc)), priority(pri), is_priority(is_pri) {}
};

class TodoBBS {
private:
    std::vector<TodoItem> priority_list;
    std::vector<TodoItem> regular_list;
    std::string priority_file;
    std::string regular_file;
    bool has_changes;

    static void clear_screen() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }

    void draw_header() const {
        std::cout << boxes::box("", {"░▒▓ TODO-BBS v1.0 ▓▒░", "A Retro styled Todo Manager"}, CYAN BOLD, CYAN BOLD);

        // Show modification status
        if (has_changes) {
            std::cout << YELLOW << "  [*] UNCOMMITTED CHANGES" << RESET << "\n";
        } else {
            std::cout << GREEN << "  [✓] All changes committed" << RESET << "\n";
        }
        std::cout << "\n";
    }

    static void draw_separator(const std::string& c = "-") {
        std::cout << BLUE;
        for (int i = 0; i < 72; i++) std::cout << c;
        std::cout << RESET << "\n";
    }

    static void load_from_file(const std::string& filename, std::vector<TodoItem>& list, bool is_priority) {
        std::ifstream file(filename);
        if (!file.is_open()) return;

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;

            if (is_priority) {
                size_t pos = line.find('|');
                if (pos != std::string::npos) {
                    int pri = std::stoi(line.substr(0, pos));
                    std::string desc = line.substr(pos + 1);
                    list.push_back(TodoItem(desc, true, pri));
                }
            } else {
                list.push_back(TodoItem(line, false));
            }
        }
        file.close();
    }

    static void save_to_file(const std::string& filename, const std::vector<TodoItem>& list, bool is_priority) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cout << RED << "  [ERROR] Could not save to file: " << filename << RESET << "\n";
            return;
        }

        for (const auto& item : list) {
            if (is_priority) {
                file << item.priority << "|" << item.description << "\n";
            } else {
                file << item.description << "\n";
            }
        }
        file.close();
    }

    void display_priority_list() const
    {
        // Sort by priority
        std::vector<TodoItem> sorted = priority_list;
        std::sort(sorted.begin(), sorted.end(),
            [](const TodoItem& a, const TodoItem& b) { return a.priority < b.priority; });

        std::vector<std::string> toDisp;

        if (sorted.empty()) {
            toDisp.push_back("(empty)");
        } else {
            for (const auto& item : sorted) {
                toDisp.push_back("[" + std::to_string(item.priority) + "] " + item.description);
            }
        }

        std::cout << boxes::indent(boxes::box("PRIORITY TODO LIST", toDisp, CYAN, MAGENTA BOLD), "  ");
    }

    void display_regular_list() const
    {
        std::vector<std::string> toDisp;

        if (regular_list.empty()) {
            toDisp.push_back("(empty)");
        } else {
            for (const auto& item : regular_list) {
                toDisp.push_back("• " + item.description);
            }
        }

        std::cout << boxes::indent(boxes::box("REGULAR TODO LIST", toDisp, CYAN, GREEN BOLD), "  ");
    }

    int find_priority_index(int priority) const
    {
        for (size_t i = 0; i < priority_list.size(); i++) {
            if (priority_list[i].priority == priority) {
                return i;
            }
        }
        return -1;
    }

    void bump_priorities_down(int starting_priority) {
        for (auto& item : priority_list) {
            if (item.priority >= starting_priority) {
                item.priority++;
            }
        }
    }

    void handle_priority_conflict(const std::string& new_desc, int new_priority) {
        std::cout << RED << "\n  [!] Priority " << new_priority << " already exists!" << RESET << "\n";
        std::cout << "      Current item: " << CYAN
                  << priority_list[find_priority_index(new_priority)].description
                  << RESET << "\n\n";

        std::cout << "  [1] Bump - Auto-reassign all conflicting priorities down\n";
        std::cout << "  [2] Reassign - Manually reassign priorities\n";
        std::cout << "  [3] Cancel\n\n";
        std::cout << YELLOW << "  > Select option: " << RESET;

        int choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice == 1) {
            bump_priorities_down(new_priority);
            priority_list.push_back(TodoItem(new_desc, true, new_priority));
            has_changes = true;
            std::cout << GREEN << "\n  [✓] Item added, priorities bumped down" << RESET << "\n";
        } else if (choice == 2) {
            manual_reassign(new_desc, new_priority);
        } else {
            std::cout << RED << "\n  [✗] Addition cancelled" << RESET << "\n";
        }
    }

    void manual_reassign(const std::string& new_desc, int new_priority) {
        std::cout << "\n" << YELLOW << "  ═══ MANUAL PRIORITY REASSIGNMENT ═══" << RESET << "\n\n";

        // Show all conflicting items
        std::vector<TodoItem*> conflicting;
        for (auto& item : priority_list) {
            if (item.priority >= new_priority) {
                conflicting.push_back(&item);
            }
        }

        std::sort(conflicting.begin(), conflicting.end(),
            [](const TodoItem* a, const TodoItem* b) { return a->priority < b->priority; });

        std::cout << "  Items that need reassignment:\n\n";
        for (auto* item : conflicting) {
            std::cout << "  [" << item->priority << "] " << item->description << "\n";
        }

        std::cout << "\n  New item:\n";
        std::cout << "  [" << new_priority << "] " << new_desc << "\n\n";

        // Reassign each
        for (auto* item : conflicting) {
            std::cout << CYAN << "  Reassign [" << item->priority << "] "
                     << item->description << RESET << "\n";
            std::cout << "  New priority: ";
            int new_pri;
            std::cin >> new_pri;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (new_pri == item->priority) continue;

            // Check if new priority conflicts
            if (find_priority_index(new_pri) != -1) {
                std::cout << RED << "  [!] Priority " << new_pri
                         << " already assigned. Try again." << RESET << "\n";
                // Re-do this item
                continue;
            }

            item->priority = new_pri;
        }

        priority_list.push_back(TodoItem(new_desc, true, new_priority));
        has_changes = true;
        std::cout << GREEN << "\n  [✓] Items reassigned successfully" << RESET << "\n";
    }

    void add_item() {
        clear_screen();
        draw_header();
        std::cout << YELLOW << "  ═══ ADD TODO ITEM ═══" << RESET << "\n\n";

        std::cout << "  [1] Priority Item\n";
        std::cout << "  [2] Regular Item\n\n";
        std::cout << CYAN << "  > Select type: " << RESET;

        int type;
        std::cin >> type;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (type == 1) {
            std::cout << YELLOW << "\n  Enter priority number: " << RESET;
            int priority;
            std::cin >> priority;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            std::cout << YELLOW << "  Enter description: " << RESET;
            std::string desc;
            std::getline(std::cin, desc);

            if (desc.empty()) {
                std::cout << RED << "\n  [✗] Description cannot be empty" << RESET << "\n";
                pause();
                return;
            }

            // Check for priority conflicts
            if (find_priority_index(priority) != -1) {
                handle_priority_conflict(desc, priority);
            } else {
                priority_list.push_back(TodoItem(desc, true, priority));
                has_changes = true;
                std::cout << GREEN << "\n  [✓] Priority item added" << RESET << "\n";
            }

        } else if (type == 2) {
            std::cout << YELLOW << "\n  Enter description: " << RESET;
            std::string desc;
            std::getline(std::cin, desc);

            if (desc.empty()) {
                std::cout << RED << "\n  [✗] Description cannot be empty" << RESET << "\n";
                pause();
                return;
            }

            regular_list.push_back(TodoItem(desc, false));
            has_changes = true;
            std::cout << GREEN << "\n  [✓] Regular item added" << RESET << "\n";
        }

        pause();
    }

    void remove_item() {
        clear_screen();
        draw_header();
        std::cout << YELLOW << "  ═══ REMOVE TODO ITEM ═══" << RESET << "\n\n";

        std::cout << "  [1] Priority List\n";
        std::cout << "  [2] Regular List\n\n";
        std::cout << CYAN << "  > Select list: " << RESET;

        int list_choice;
        std::cin >> list_choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (list_choice == 1) {
            if (priority_list.empty()) {
                std::cout << RED << "\n  [✗] Priority list is empty" << RESET << "\n";
                pause();
                return;
            }

            clear_screen();
            draw_header();
            std::cout << YELLOW << "  ═══ PRIORITY LIST ═══" << RESET << "\n\n";

            std::vector<TodoItem> sorted = priority_list;
            std::sort(sorted.begin(), sorted.end(),
                [](const TodoItem& a, const TodoItem& b) { return a.priority < b.priority; });

            for (const auto& item : sorted) {
                std::cout << "  [" << item.priority << "] " << item.description << "\n";
            }

            std::cout << YELLOW << "\n  > Enter priority to remove (0 to cancel): " << RESET;
            int priority;
            std::cin >> priority;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            int idx = find_priority_index(priority);
            if (idx == -1) {
                std::cout << RED << "\n  [✗] Priority not found" << RESET << "\n";
                pause();
                return;
            }

            std::cout << RED << "\n  Remove: " << priority_list[idx].description
                     << "? (y/n): " << RESET;
            char confirm;
            std::cin >> confirm;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (confirm == 'y' || confirm == 'Y') {
                priority_list.erase(priority_list.begin() + idx);
                has_changes = true;
                std::cout << GREEN << "\n  [✓] Item removed" << RESET << "\n";
            } else {
                std::cout << RED << "\n  [✗] Removal cancelled" << RESET << "\n";
            }

        } else if (list_choice == 2) {
            if (regular_list.empty()) {
                std::cout << RED << "\n  [✗] Regular list is empty" << RESET << "\n";
                pause();
                return;
            }

            clear_screen();
            draw_header();
            std::cout << YELLOW << "  ═══ REGULAR LIST ═══" << RESET << "\n\n";

            for (size_t i = 0; i < regular_list.size(); i++) {
                std::cout << "  [" << (i + 1) << "] " << regular_list[i].description << "\n";
            }

            std::cout << YELLOW << "\n  > Select item to remove (0 to cancel): " << RESET;
            int choice;
            std::cin >> choice;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (choice < 1 || choice > static_cast<int>(regular_list.size())) {
                std::cout << RED << "\n  [✗] Removal cancelled" << RESET << "\n";
                pause();
                return;
            }

            std::cout << RED << "\n  Remove: " << regular_list[choice - 1].description
                     << "? (y/n): " << RESET;
            char confirm;
            std::cin >> confirm;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (confirm == 'y' || confirm == 'Y') {
                regular_list.erase(regular_list.begin() + (choice - 1));
                has_changes = true;
                std::cout << GREEN << "\n  [✓] Item removed" << RESET << "\n";
            } else {
                std::cout << RED << "\n  [✗] Removal cancelled" << RESET << "\n";
            }
        }

        pause();
    }

    void commit_changes() {
        if (!has_changes) {
            std::cout << CYAN << "\n  [i] No changes to commit" << RESET << "\n";
            pause();
            return;
        }

        clear_screen();
        draw_header();
        std::cout << YELLOW << "  ═══ COMMIT CHANGES ═══" << RESET << "\n\n";

        std::cout << "  The following changes will be saved:\n\n";
        display_priority_list();
        display_regular_list();

        std::cout << RED << "  Confirm commit? (y/n): " << RESET;
        char confirm;
        std::cin >> confirm;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (confirm == 'y' || confirm == 'Y') {
            save_to_file(priority_file, priority_list, true);
            save_to_file(regular_file, regular_list, false);
            has_changes = false;
            std::cout << GREEN << "\n  [✓] Changes committed successfully!" << RESET << "\n";
        } else {
            std::cout << RED << "\n  [✗] Commit cancelled" << RESET << "\n";
        }

        pause();
    }

    void view_list() const
    {
        clear_screen();
        draw_header();
        std::cout << YELLOW << "  ═══ CHOOSE LIST TO VIEW ═══" << RESET << "\n\n";
        std::cout << "  [1] Priority List\n";
        std::cout << "  [2] Regular List\n\n";
        std::cout << CYAN << "  > Select type: " << RESET;

        int choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice == 1) display_priority_list();
        else display_regular_list();

        pause();
        clear_screen();
    }

    static void pause() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        std::cout << "\n" << CYAN << "  Press ENTER to continue..." << RESET;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    void show_menu() const
    {
        draw_separator("=");
        std::cout << CYAN << "  [1] View TODO List\n";
        std::cout << "  [2] Add Item\n";
        std::cout << "  [3] Remove Item\n";
        std::cout << "  [4] " << (has_changes ? YELLOW + std::string("[*] ") + CYAN : "")
                  << "Commit Changes\n" << RESET;
        std::cout << CYAN << "  [5] Exit (discard uncommitted changes)\n";
        draw_separator("=");
        std::cout << YELLOW << "\n  > Enter command: " << RESET;
    }

public:
    TodoBBS(const std::string& pri_file, const std::string& reg_file)
        : priority_file(pri_file), regular_file(reg_file), has_changes(false) {
        load_from_file(priority_file, priority_list, true);
        load_from_file(regular_file, regular_list, false);
    }

    void run() {
        while (true) {
            clear_screen();
            draw_header();
            display_priority_list();
            display_regular_list();
            show_menu();
            
            int choice;
            std::cin >> choice;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            
            switch (choice) {
                case 1:
                    view_list();
                    break;
                case 2:
                    add_item();
                    break;
                case 3:
                    remove_item();
                    break;
                case 4:
                    commit_changes();
                    break;
                case 5:
                    if (has_changes) {
                        std::cout << RED << "\n  [!] You have uncommitted changes. Exit anyway? (y/n): " << RESET;
                        char confirm;
                        std::cin >> confirm;
                        if (confirm == 'y' || confirm == 'Y') {
                            std::cout << "\n" << CYAN << "  ═══ Thanks for using TODO-BBS! ═══" << RESET << "\n\n";
                            return;
                        }
                    } else {
                        std::cout << "\n" << CYAN << "  ═══ Thanks for using TODO-BBS! ═══" << RESET << "\n\n";
                        return;
                    }
                    break;
                default:
                    std::cout << RED << "\n  [✗] Invalid option" << RESET << "\n";
                    pause();
            }
        }
    }
};

std::string get_home_directory() {
    #ifdef _WIN32
        const char* home = std::getenv("USERPROFILE");
        if (!home) home = std::getenv("HOMEDRIVE");
    #else
        const char* home = std::getenv("HOME");
    #endif
    return home ? std::string(home) : ".";
}

std::pair<std::string, std::string> select_file_paths() {
    std::cout << CYAN << BOLD;
    std::cout << "\n╔════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║              ░▒▓ TODO-BBS v1.0 ▓▒░                                 ║\n";
    std::cout << "║              Your Retro Task Manager                               ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════════╝\n";
    std::cout << RESET << "\n";
    
    std::cout << YELLOW << "  ═══ FILE LOCATION SETUP ═══" << RESET << "\n\n";
    std::cout << "  [1] Local - Use TODO lists in current directory\n";
    std::cout << "  [2] Global - Use TODO lists in Documents/todo/\n\n";
    std::cout << CYAN << "  > Select mode: " << RESET;
    
    int choice;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    std::string priority_file, regular_file;
    
    if (choice == 2) {
        std::string home = get_home_directory();
        std::string todo_dir = home + "/Documents/todo";
        
        // Create directory if it doesn't exist
        #ifdef _WIN32
            system(("mkdir \"" + todo_dir + "\" 2>nul").c_str());
        #else
            system(("mkdir -p \"" + todo_dir + "\"").c_str());
        #endif
        
        priority_file = todo_dir + "/priority_todo.txt";
        regular_file = todo_dir + "/regular_todo.txt";
        
        std::cout << GREEN << "\n  [✓] Using global TODO lists: " << todo_dir << RESET << "\n";
    } else {
        priority_file = "priority_todo.txt";
        regular_file = "regular_todo.txt";
        
        std::cout << GREEN << "\n  [✓] Using local TODO lists in current directory" << RESET << "\n";
    }
    
    std::cout << CYAN << "\n  Press ENTER to continue..." << RESET;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    return {priority_file, regular_file};
}

int main() {
    std::pair<std::string, std::string> file_paths = select_file_paths();
    
    TodoBBS app(file_paths.first, file_paths.second);
    app.run();
    
    return 0;
}