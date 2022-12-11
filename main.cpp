#include <pthread.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <unistd.h>

// time of Ivan "sleeping" it means that he is working at that time
#define sleepIvan 5
// the same
#define sleepOfPetrov 6
//the same
#define sleepOfNecheporuk 8

//method of Ivan work
void *workIvan(void *param);

// the same
void *workPetrov(void *param);

// the same
void *workNecheporuk(void *param);

// input items from command line
std::vector<std::string> takeItemsFromCommand(int argc, char **argv);

// input from other sources
std::vector<std::string> takeItemsFromConsoleOrFileOrRand();

// chosing type of output
void choseTypeOfOutput();

// number of items in store
int number_of_items;

// mutex for items_near_car
pthread_mutex_t mutex_near_car;

// mutex for items in car
pthread_mutex_t mutex_in_car;

// mutex for output
pthread_mutex_t mutex_output;

// ofstream for output data to file
std::ofstream out;

// flag if out exist
bool out_exist;

int main(int argc, char **argv) {
    srand(time(NULL));
    std::vector<std::string> items_at_store;
    if (argc > 1) {
        items_at_store = takeItemsFromCommand(argc, argv);
    } else {
        items_at_store = takeItemsFromConsoleOrFileOrRand();
    }
    choseTypeOfOutput();
    std::vector<std::string> items_near_car;
    std::vector<std::string> items_in_car;
    number_of_items = items_at_store.size();
    pthread_t first;                // thread for Ivan
    pthread_t second;               // thread for Pavel
    pthread_t third;                // thread for Necheporuk

    // making pair of two data for put in param method
    std::pair<std::vector<std::string> *, std::vector<std::string> *> item_near_and_in_car =
        std::make_pair(&items_near_car, &items_in_car);
    // making pair of two data for put in param method
    std::pair<std::vector<std::string> *, std::vector<std::string> *> item_in_store_and_new_car =
        std::make_pair(&items_at_store, &items_near_car);
    // create thread for each person
    pthread_create(&first, nullptr, workIvan, &item_in_store_and_new_car);
    pthread_create(&second, nullptr, workPetrov, &item_near_and_in_car);
    pthread_create(&third, nullptr, workNecheporuk, &items_in_car);
    // waiting each thread
    pthread_join(first, nullptr);
    pthread_join(second, nullptr);
    pthread_join(third, nullptr);
}

void choseTypeOfOutput() {
    int typeOfOutput;
    do {
        std::cout << "chose type of output:\n '1' - output to console \n '2' - output to console and file\n";
        std::cin >> typeOfOutput;
    } while (typeOfOutput != 1 && typeOfOutput != 2);
    out_exist = false;
    std::string name;
    if (typeOfOutput == 2) {
        out_exist = true;
        do {
            std::cout << "input name of file:\n";
            std::cin >> name;
            out = std::ofstream(name);
        } while (!out);
    }
}

// get rand number from [0,1)
double getRand() {
    return (rand() % 3253) / 3253.0;
}

void *workIvan(void *param) {
    auto par = reinterpret_cast<std::pair<std::vector<std::string> *, std::vector<std::string> *> *>(param);
    std::vector<std::string> *items_at_store = par->first;
    std::vector<std::string> *items_near_car = par->second;
    srand(time(NULL));
    while (items_at_store->size() != 0) {
        sleep(sleepIvan * getRand());
        size_t i = rand() % items_at_store->size();
        std::string item = items_at_store->operator[](i);
        pthread_mutex_lock(&mutex_near_car);
        items_near_car->push_back(item);
        pthread_mutex_unlock(&mutex_near_car);
        items_at_store->erase(items_at_store->begin() + i);
        pthread_mutex_lock(&mutex_output);
        std::cout << "Ivan took out " + item << "\n";
        if (out_exist) {
            out << "Ivan took out " + item << "\n";
        }
        pthread_mutex_unlock(&mutex_output);
    }
}

// below just obvious realization of functions

void *workPetrov(void *param) {
    auto par = reinterpret_cast<std::pair<std::vector<std::string> *, std::vector<std::string> *> *>(param);
    std::vector<std::string> *items_near_car = par->first;
    std::vector<std::string> *items_in_car = par->second;
    srand(time(NULL));
    int j = 0;
    while (1) {
        if (j == number_of_items) {
            break;
        }
        if (items_near_car->empty()) {
            //       sleep(rand() % sleepOfPetrov);
        } else {
            sleep(sleepOfPetrov * getRand());
            size_t i = rand() % items_near_car->size();
            pthread_mutex_lock(&mutex_near_car);
            std::string item = items_near_car->operator[](i);
            items_near_car->erase(items_near_car->begin() + i);
            pthread_mutex_unlock(&mutex_near_car);
            pthread_mutex_lock(&mutex_in_car);
            items_in_car->push_back(item);
            pthread_mutex_unlock(&mutex_in_car);
            pthread_mutex_lock(&mutex_output);
            if (out_exist) {
                out << "Petrov put on car " + item << "\n";
            }
            std::cout << "Petrov put on car " + item << "\n";
            pthread_mutex_unlock(&mutex_output);
            ++j;
        }
    }
}

void *workNecheporuk(void *param) {
    std::vector<std::string> *items_in_car = reinterpret_cast<std::vector<std::string> *>(param);
    srand(time(NULL));
    int j = 0;
    while (1) {
        if (j == number_of_items) {
            break;
        }
        if (items_in_car->size() == j) {
            //       sleep(rand()%sleepOfNecheporuk);
        } else if (items_in_car->size() > j) {
            sleep(sleepOfNecheporuk * getRand());
            pthread_mutex_lock(&mutex_in_car);
            std::string item = items_in_car->operator[](j);
            std::string cost = std::to_string(rand() % 100);
            pthread_mutex_unlock(&mutex_in_car);
            pthread_mutex_lock(&mutex_output);
            std::cout << "Necheporuk value new item " + item + " cost: " + cost + "$\n";
            if (out_exist) {
                out << "Necheporuk value new item " + item + " cost: " + cost + "$\n";
            }
            pthread_mutex_unlock(&mutex_output);
            ++j;
        }
    }
}

std::vector<std::string> takeItemsFromCommand(int argc, char **argv) {
    std::vector<std::string> ans(argc - 1);
    for (int i = 0; i < argc - 1; ++i) {
        ans[i] = argv[i + 1];
    }
    return ans;
}

std::string getRandString() {
    int len = rand() % 10 + 1;
    std::string ans = "";
    for (size_t i = 0; i < len; ++i) {
        ans += char(rand() % ('z' - 'a') + 'a');
    }
    return ans;
}

std::vector<std::string> takeItemsFromConsoleOrFileOrRand() {
    std::vector<std::string> ans{};
    int command;
    do {
        std::cout << "chose type of inpute: \n '1' - from console \n '2' - random\n '3' - file\n";
        std::cin >> command;
    } while (command != 1 && command != 2 && command != 3);

    if (command == 1) {
        int num;
        do {
            std::cout << "write number of items ( < 10 ): \n";
            std::cin >> num;
        } while (num < 0 || num > 10);
        for (int i = 0; i < num; ++i) {
            std::cout << "write " + std::to_string(i) + "-ый object:\n";
            std::string item;
            std::cin >> item;
            ans.push_back(item);
        }
        return ans;
    }

    if (command == 2) {
        int num = rand() % 10 + 2;
        for (size_t i = 0; i < num; ++i) {
            std::string item = getRandString();
            ans.push_back(item);
        }
        return ans;
    }

    if (command == 3) {
        std::ifstream in;
        std::string name;
        do {
            std::cout << "input name of file:\n";
            std::cin >> name;
            in = std::ifstream(name);
        } while (!in);
        std::string line;
        while (in >> line) {
            ans.push_back(line);
        }
        if (ans.size() > 15) {
            throw new std::invalid_argument("should be less arguments");
        }
        return ans;
    }
    return ans;
}

