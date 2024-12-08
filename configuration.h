#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <stdexcept>

#define DEFAULT_CONFIG_FILE_NAME "config.txt"
#define sl_config Configuration::instance()
#define GENERATOR_RATE 60
#define TICK_RATE 1

#define MIN_PT 1
#define MAX_PT 15

class Configuration {
public:

    static Configuration& instance() {
        static Configuration instance;
        return instance;
    }

    void load() {
        read(DEFAULT_CONFIG_FILE_NAME);
    }

    void load(const std::string& filename) {
        read(filename);
    }


    int tickrate() const { return _tickrate; }
    int generator_rate() const { return _generator_rate; }
    int min_pt() const { return _min_pt; }
    int max_pt() const { return _max_pt; }


    template <typename T = std::string> 
    T get(const std::string& key) const {
        auto it = settings.find(key);
        if (it != settings.end()) {
            return it->second;
        }

        throw std::runtime_error("Key not found: " + key);
    }

    template <typename T = std::string>
    T get(const std::string& key, const T& defaultValue) const {
        auto it = settings.find(key);
        return it != settings.end() ? it->second : defaultValue;
    }

    int getint(std::string key) const {
        auto val = get(key);
        return std::stoi(val);
    }

    int getint(std::string key, int def_val) {
        auto val = get(key, std::to_string(def_val));
        return std::stoi(val);
    }

    void set(const std::string& key, const std::string& value) {
        settings[key] = value;
    }

    std::string list();

private:

    int _tickrate = TICK_RATE;
    int _generator_rate = GENERATOR_RATE;
    int _min_pt = MIN_PT;
    int _max_pt = MAX_PT;

    std::unordered_map<std::string, std::string> settings;

    void read(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open configuration file: " + filename);
        }

        std::string line;
        while (std::getline(file, line)) {
            // Ignore comments and empty lines
            if (line.empty() || line[0] == '#') continue;

            auto delimiterPos = line.find('=');
            if (delimiterPos == std::string::npos) {
                throw std::runtime_error("Invalid configuration format in line: " + line);
            }

            std::string key = line.substr(0, delimiterPos);
            std::string value = line.substr(delimiterPos + 1);

            // Trim spaces around key and value
            key = trim(key);
            value = trim(value);

            settings[key] = value;
            if (key.compare("tickrate") == 0) {
                _tickrate = std::stoi(value);
            }
            else if (key.compare("generator_rate") == 0) {
                _generator_rate = std::stoi(value);
            }
            else if (key.compare("min_pt") == 0) {
                _min_pt = std::stoi(value);
            }
            else if (key.compare("max_pt") == 0) {
                _max_pt = std::stoi(value);
            }
        }
    }
    static std::string trim(const std::string& str) {
        const char* whitespace = " \t\n\r";
        size_t start = str.find_first_not_of(whitespace);
        size_t end = str.find_last_not_of(whitespace);
        return start == std::string::npos ? "" : str.substr(start, end - start + 1);
    }
};

