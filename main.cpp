#include <iostream>
#include <string>
#include <cstdlib>
#include <sstream>
#include <curl/curl.h>
#include "geometry.hpp"
#include "generator.cpp"

void generateMapFromAIInstructions(std::string name, const std::string& aiInstructions);

// Функция для приёма сырого HTTP-ответа от cURL
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
    size_t newLength = size * nmemb;
    try {
        s->append((char*)contents, newLength);
        return newLength;
    } catch (std::bad_alloc& e) {
        return 0;
    }
}

std::string askOllama(const std::string& prompt) {
    CURL* curl = curl_easy_init();
    if (!curl) return "ERROR";

    std::string response_string;
    std::string url = "http://localhost:11434/api/generate";

    std::string systemPrompt =
        "You are the DeltaBox FGD-Compliant Level Designer AI. You must analyze the user prompt and output ONLY a raw layout script using these strict commands:\\n"
        "- ROOM x y z width length height texture mat_type (mat_type: -1=unbreakable, 0=glass, 1=wood, 4=cinder block)\\n"
        "- WALL_ITEM x y z width length height texture mat_type spawn_object (spawn_object: 1=battery, 2=healthkit, 8=shotgun)\\n"
        "- BOX x y z width length height texture (Solid pillar)\\n"
        "- ROTATING_BOX x y z width length height speed texture\\n"
        "- LIGHT x y z R G B intensity style (style: 0=normal, 10=fluorescent flicker, 2=slow pulse, 4=fast strobe)\\n"
        "- MONSTER type x y z (types: headcrab, alien_slave)\\n"
        "- EQUIP item1 item2 ...\\n\\n"
        "Available textures: CONCRETE, LAB_FLR1, LAB_WALL01, OUT_W3, SKY.\\n"
        "CRITICAL RULES:\\n"
        "1. Do NOT include markdown, backticks (```), quotes, conversational text, or introductions. Output ONLY raw commands.\\n"
        "2. If the user inputs gibberish, spam, empty text, or repetitive symbols (like ',,,,,,,', 'aaaaa'), you MUST activate a REALITY GLITCH: generate a chaotic, massive room with blinking lights, rotating pillars, and multiple monsters.";

    std::string jsonPayload = "{\"model\": \"llama3\", \"prompt\": \"" + prompt + "\", \"system\": \"" + systemPrompt + "\", \"stream\": false}";

    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonPayload.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

    std::cout << "[Ollama] Transmitting instructions to LLaMA 3 core..." << std::endl;
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) return "ERROR";

    size_t pos = response_string.find("\"response\":\"");
    if (pos != std::string::npos) {
        std::string clean = response_string.substr(pos + 12);
        size_t end_pos = clean.find("\",\"");
        if (end_pos != std::string::npos) return clean.substr(0, end_pos);
    }
    return "ERROR";
}

int main() {
    std::string mapName = "procedural_map";
    int mode = 1;

    std::cout << "\033[1;33m";
    std::cout << "      /\\ \n";
    std::cout << "     /  \\       DeltaBox Engine v1.25\n";
    std::cout << "    /_@@_\\\n";
    std::cout << "\033[0m" << std::endl;

    std::cout << "Select Generation Mode:\n";
    std::cout << "1. Classic Manual Mode (Procedural Seed Array)\n";
    std::cout << "2. AI Dream Mode (High-Level LLaMA 3 Prompt)\n";
    std::cout << "Choice: "; std::cin >> mode;

    if (mode == 2) {
        std::string aiPrompt;
        std::cout << "\nEnter your map concept prompt: ";
        std::cin.ignore();
        std::getline(std::cin, aiPrompt);

        std::string aiOutput = askOllama(aiPrompt);
        std::cout << "\n[DeltaBox AI Raw Output]:\n" << aiOutput << "\n" << std::endl;

        if (aiOutput == "ERROR" || aiOutput.find("ROOM") == std::string::npos) {
            std::cout << "[!] EMERGENCY RESET: Reality glitch detected in AI matrix! Hardcoding maximum instability..." << std::endl;
            std::string glitchScript =
                "ROOM 0 0 0 1024 1024 256 CONCRETE 4\n"
                "LIGHT 0 0 150 255 0 0 120 10\n"
                "ROTATING_BOX 100 100 0 64 64 200 45 OUT_W3\n"
                "ROTATING_BOX -100 -100 0 64 64 200 45 OUT_W3\n"
                "MONSTER alien_slave 200 200 16\n"
                "MONSTER headcrab -200 -200 16\n"
                "EQUIP weapon_shotgun ammo_buckshot item_suit item_battery\n";

            std::cout << "[1/3] Triggering emergency glitch architecture..." << std::endl;
            generateMapFromAIInstructions(mapName, glitchScript);
        } else {
            std::cout << "[1/3] Translating high-level AI script to geometry..." << std::endl;
            generateMapFromAIInstructions(mapName, aiOutput);
        }
    } else {
        std::cout << "\n[1/3] Building standard 220 manual layout..." << std::endl;
        std::string manualScript =
            "ROOM 0 0 0 512 512 160 LAB_FLR1 -1\n"
            "BOX 128 128 0 32 32 160 CONCRETE\n"
            "BOX -128 -128 0 32 32 160 CONCRETE\n"
            "LIGHT 0 0 120 255 255 255 80 0\n"
            "MONSTER headcrab 64 64 16\n"
            "EQUIP weapon_shotgun ammo_buckshot item_suit item_battery\n";

        generateMapFromAIInstructions(mapName, manualScript);
    }

    std::cout << "[2/3] Executing ZHLT compilers..." << std::endl;
    std::string compileCmd = "wine hlcsg.exe -nowadtextures " + mapName + " && " +
                             "wine hlbsp.exe " + mapName + " && " +
                             "wine hlvis.exe " + mapName + " && " +
                             "wine hlrad.exe " + mapName;

    int res = system(compileCmd.c_str());

    if (res != -1 && (res >> 8) == 0) {
        std::cout << "[3/3] Deployment successful! Syncing map..." << std::endl;
        std::cout << "\n--- DELTABOX GENERATION COMPLETE ---\n";
        std::cout << "Put " << mapName << ".bsp in half-life map folder manually.\n";
    } else {
        std::cerr << "\n[!] Critical: ZHLT compilation failed!" << std::endl;
    }

    return 0;
}
