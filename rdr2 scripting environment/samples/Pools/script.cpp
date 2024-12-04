#include <algorithm> // For std::transform and std::isspace
#include <iostream>  // Include iostream for console output
#include <fstream>
#include <string>
#include <sys/stat.h>
#include "global.h"

// Function to trim leading and trailing spaces
std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    size_t end = str.find_last_not_of(" \t\n\r");
    return (start == std::string::npos || end == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

// Native declarations
NATIVE_DECL BOOL _IS_IN_FULL_FIRST_PERSON_MODE() { return invoke<BOOL>(0xD1BA66940E94C547); } // 0xD1BA66940E94C547 b1207
NATIVE_DECL BOOL IS_MOVE_BLEND_RATIO_WALKING(float moveBlendRatio) { return invoke<BOOL>(0xF133BBBE91E1691F, moveBlendRatio); } // 0xF133BBBE91E1691F 0xD21639A8 b1207
NATIVE_DECL BOOL IS_PED_STILL(Ped ped) { return invoke<BOOL>(0xAC29253EEF8F0180, ped); } // 0xAC29253EEF8F0180 0x09E3418D b1207
NATIVE_DECL BOOL IS_PED_SPRINTING(Ped ped) { return invoke<BOOL>(0x57E457CD2C0FC168, ped); } // 0x57E457CD2C0FC168 0x4F3E0633 b1207
NATIVE_DECL int GET_GAME_TIMER() { return invoke<int>(0x4F67E8ECA7D3F667); } // 0x4F67E8ECA7D3F667 0xA4EA0691 b1207

bool isModActive = true;
// Global variables for key bindings
//Hash inputCustomBindHold = INPUT_JUMP;     // Default custom hold key (e.g., INPUT_JUMP)
//Hash inputCustomBindRelease = INPUT_OPEN_SATCHEL_MENU;  // Default custom release key (e.g., INPUT_OPEN_SATCHEL_MENU)

// Global variables for movement speed and state
float walkValue = 1.2f;      // Walking speed value
float runValue = 3.f;      // Running speed value

// Forward declarations
void LoadSettings();
bool CheckMovement();

// Variables to track file changes
time_t lastModifiedTime = 0;
bool revertToWalkWhenStill = true;  // Default value
bool wasInFirstPerson = false;  // This should be declared globally as well
int movementState = 0;  // Movement state: 0 = Walk, 1 = Run, 2 = Sprint
// Global variables to track the sprint timer
int sprintStartTime = 0;  // The time the sprint was first pressed

// Function to check and reload settings if the file was modified
void CheckAndReloadSettings()
{
    const std::string iniFilePath = "FPSWalk.ini";
    struct stat fileStat;

    if (stat(iniFilePath.c_str(), &fileStat) == 0)
    {
        if (fileStat.st_mtime != lastModifiedTime)
        {
            lastModifiedTime = fileStat.st_mtime; // Update the last modified time
            LoadSettings(); // Reload settings from the file
        }
    }
}

// Save settings to the INI file
void SaveSettings()
{
    const std::string iniFilePath = "FPSWalk.ini";
    std::ofstream outFile(iniFilePath);
    if (outFile.is_open())
    {
        outFile << "isModActive=" << (isModActive ? "true" : "false") << std::endl;
        //outFile << "inputCustomBindHold=" << inputCustomBindHold << std::endl;    // Save custom bind for hold
        //outFile << "inputCustomBindRelease=" << inputCustomBindRelease << std::endl;  // Save custom bind for release
        outFile << "walkValue=" << walkValue << std::endl;
        outFile.close();
    }
}

// Load settings from the INI file
void LoadSettings() {
    const std::string iniFilePath = "FPSWalk.ini";
    std::ifstream inFile(iniFilePath);

    if (inFile.is_open()) {
        std::string line;
        while (std::getline(inFile, line)) {
            std::cout << "Reading line: " << line << std::endl;  // Debugging line
            size_t equalsPos = line.find("=");
            if (equalsPos != std::string::npos) {
                std::string key = line.substr(0, equalsPos);
                std::string value = trim(line.substr(equalsPos + 1));

                // Check for isModActive and allow variations like "True" or "1"
                if (key == "isModActive") {
                    std::transform(value.begin(), value.end(), value.begin(), ::tolower); // Convert to lowercase
                    if (value == "true" || value == "1") {
                        isModActive = true;
                    }
                    else if (value == "false" || value == "0") {
                        isModActive = false;
                    }
                }
                else if (key == "walkValue") {
                    walkValue = std::stof(value); // Parse as float
                }
            }
        }
        inFile.close();
    }
    else {
        std::cerr << "Error: Unable to open INI file. Creating default settings." << std::endl;
        SaveSettings(); // Save default settings if the file doesn't exist
    }
}

bool CheckMovement()
{
    bool w = PAD::IS_CONTROL_PRESSED(0, MISC::GET_HASH_KEY("INPUT_MOVE_UP_ONLY"));
    bool a = PAD::IS_CONTROL_PRESSED(0, MISC::GET_HASH_KEY("INPUT_MOVE_LEFT_ONLY"));
    bool s = PAD::IS_CONTROL_PRESSED(0, MISC::GET_HASH_KEY("INPUT_MOVE_DOWN_ONLY"));
    bool d = PAD::IS_CONTROL_PRESSED(0, MISC::GET_HASH_KEY("INPUT_MOVE_RIGHT_ONLY"));

    return w || a || s || d;
}

void update()
{
    // Check the file for changes every frame
    CheckAndReloadSettings();
    Player player = PLAYER::PLAYER_ID();
    Ped playerPed = PLAYER::PLAYER_PED_ID();
    bool keySatchel;
    bool keyJump;
    bool keySprint;
    bool keySprintPressed;

    // Check if specific key combination is pressed to toggle the speed
    keySatchel = PAD::IS_CONTROL_JUST_RELEASED(0, INPUT_OPEN_SATCHEL_MENU) || PAD::IS_DISABLED_CONTROL_JUST_RELEASED(0, INPUT_OPEN_SATCHEL_MENU);
    keyJump = PAD::IS_CONTROL_PRESSED(0, INPUT_JUMP) || PAD::IS_DISABLED_CONTROL_PRESSED(0, INPUT_JUMP);
    keySprint = PAD::IS_CONTROL_JUST_PRESSED(0, INPUT_SPRINT);
    keySprintPressed = PAD::IS_CONTROL_PRESSED(0, INPUT_SPRINT);

    if (keyJump && keySatchel)
    {
        isModActive = !isModActive;  // Toggle the mod state
        SaveSettings();
    }

    if (!isModActive)
    {
        return;
    }

    // Check if player ped exists and control is on
    if (!ENTITY::DOES_ENTITY_EXIST(playerPed) || !PLAYER::IS_PLAYER_CONTROL_ON(player) || !_IS_IN_FULL_FIRST_PERSON_MODE() && isModActive)
    {
        wasInFirstPerson = true; // Reset state if control is lost, player doesn't exist, or not in first person.
        return;
    }

    // Transition into first-person mode (only when first entering)
    if ((_IS_IN_FULL_FIRST_PERSON_MODE() && wasInFirstPerson && isModActive) ||
        (_IS_IN_FULL_FIRST_PERSON_MODE() && IS_PED_STILL(playerPed) && revertToWalkWhenStill) && isModActive)
    {
        wasInFirstPerson = false;
        movementState = 0; // Reset state to walk
    }

    // Toggle between walking, running, and sprinting on sprint key release
    if (keySprint && isModActive)
    {
        // Toggle movement states (walking, running, sprinting)
        if (movementState == 0) // Walking
        {
            movementState = 1; // Switch to Running
        }
        else if (movementState == 1) // Running
        {
            movementState = 2; // Switch to Sprinting
        }
        else if (movementState == 2) // Sprinting
        {
            movementState = 1; // Switch to Walking
        }
    }

    // Check if the player is actively moving and apply the speed adjustment
    if (CheckMovement() && isModActive)
    {
        if (movementState == 0) // Walk
        {
            //IS_PED_RUNNING(playerPed) == false;
            //IS_PED_SPRINTING(playerPed) == false;
            //IS_PED_WALKING(playerPed) == true;
            PED::SET_PED_MIN_MOVE_BLEND_RATIO(playerPed, walkValue);
            PED::SET_PED_MAX_MOVE_BLEND_RATIO(playerPed, walkValue);
        }
        else if (movementState == 1) // Run
        {
            //IS_PED_SPRINTING(playerPed) == false;
            //IS_PED_RUNNING(playerPed) == false;
            //IS_PED_WALKING(playerPed) == true;
            TASK::SET_PED_DESIRED_MOVE_BLEND_RATIO(playerPed, runValue); // Apply smooth sprinting transition
        }
        else if (keySprintPressed) {
            // If sprint has just started (button pressed), record the start time
            if (sprintStartTime == 0) {
                sprintStartTime = GET_GAME_TIMER(); // Record when sprint key is pressed
            }

            // Hold the sprint key while the player is not sprinting
            if (!IS_PED_SPRINTING(playerPed)) {
                // If the player isn't sprinting yet, simulate pressing the sprint key
                PAD::SET_CONTROL_VALUE_NEXT_FRAME(0, MISC::GET_HASH_KEY("INPUT_SPRINT"), 1.0f);
            }
            else {
                // Once the player starts sprinting, release the sprint key
                PAD::SET_CONTROL_VALUE_NEXT_FRAME(0, MISC::GET_HASH_KEY("INPUT_SPRINT"), 0.0f);
                movementState = 2;
            }
        }
        else {
            // When the sprint button is released, stop holding it
            if (IS_PED_SPRINTING(playerPed)) {
                // If the player is sprinting, disable sprinting on key release
                PAD::SET_CONTROL_VALUE_NEXT_FRAME(0, MISC::GET_HASH_KEY("INPUT_SPRINT"), 0.0f); // Disable sprint
                movementState = 2;
            }

            // Check and revert from sprint to run if the player has stopped sprinting
            if (movementState == 2 && !IS_PED_SPRINTING(playerPed))
            {
                movementState = 1; // Revert to Running
            }

            // Reset the sprint start time when the button is released
            sprintStartTime = 0;
        }
    }
}

// Main function
int main()
{
    WAIT(3000);
    LoadSettings();
    while (true)
    {
        update();
        WAIT(0);
    }
}

void ScriptMain()
{
    srand(GetTickCount());
    main();
}