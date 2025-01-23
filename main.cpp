#include <iostream>
#include <string>
#include <ctime>
#include <windows.h>
#include "auth.hpp"

using namespace KeyAuth;

// KeyAuth Configuration
std::string name = "Spoofer";
std::string ownerid = "yz7OIEzaOg";
std::string version = "1.0";
std::string url = "https://keyauth.win/api/1.3/";
std::string path = "";
api KeyAuthApp(name, ownerid, version, url, path);

std::string GenerateRandomSerial(int length) {
    static const char hex_chars[] = "0123456789ABCDEF";
    std::string serial;
    for (int i = 0; i < length; ++i) {
        serial += hex_chars[rand() % (sizeof(hex_chars) - 1)];
    }
    return serial;
}

std::string GenerateCPUSerial() {
    return GenerateRandomSerial(4) + "-" + GenerateRandomSerial(4);
}

std::string GenerateBaseboardSerial() {
    return GenerateRandomSerial(10);
}

std::string GenerateVolumeSerial() {
    const char hexDigits[] = "0123456789ABCDEF";
    std::string serial = "";

    for (int i = 0; i < 8; ++i) {
        if (i == 4) {
            serial += '-';
        }
        serial += hexDigits[rand() % 16];
    }
    return serial;
}

std::string selectManufacturer() {
    int choice;
    std::cout << "\nSelect your motherboard brand:\n"
        << "1) ASUS\n"
        << "2) MSI\n"
        << "3) ASRock\n"
        << "4) Gigabyte\n"
        << "Choice: ";
    std::cin >> choice;

    switch (choice) {
    case 1: return "ASUSTeK COMPUTER INC.";
    case 2: return "Micro-Star International Co., Ltd.";
    case 3: return "ASRock Inc.";
    case 4: return "GIGA-BYTE Technology Co., Ltd.";
    default:
        std::cerr << "Invalid choice. Exiting.\n";
        exit(EXIT_FAILURE);
    }
}

void permspoof() {
    srand(time(NULL));

    wchar_t tempPath[MAX_PATH];
    GetTempPathW(MAX_PATH, tempPath);
    std::wstring tempFolder = tempPath;

    if (tempFolder.back() != '\\') tempFolder += '\\'; // Ensure trailing backslash

    // Command strings with proper formatting
    std::string commands[] = {
        "cd \"" + std::string(tempFolder.begin(), tempFolder.end()) + "\" && AMIDEWINx64.EXE /BS \"" + GenerateRandomSerial(16) + "\" > NUL 2>&1",
        "cd \"" + std::string(tempFolder.begin(), tempFolder.end()) + "\" && AMIDEWINx64.EXE /SS \"" + GenerateBaseboardSerial() + "\" > NUL 2>&1",
        "cd \"" + std::string(tempFolder.begin(), tempFolder.end()) + "\" && AMIDEWINx64.EXE /SU auto > NUL 2>&1",
        "cd \"" + std::string(tempFolder.begin(), tempFolder.end()) + "\" && AMIDEWINx64.EXE /SK \"" + GenerateRandomSerial(10) + "\" > NUL 2>&1",
        "cd \"" + std::string(tempFolder.begin(), tempFolder.end()) + "\" && AMIDEWINx64.EXE /PSN \"Unknown\" > NUL 2>&1",
        "cd \"" + std::string(tempFolder.begin(), tempFolder.end()) + "\" && AMIDEWINx64.EXE /CS \"" + GenerateRandomSerial(10) + "\" > NUL 2>&1",
        "cd \"" + std::string(tempFolder.begin(), tempFolder.end()) + "\" && AMIDEWINx64.EXE /CA \"" + GenerateRandomSerial(10) + "\" > NUL 2>&1"
    };

    // Run each command
    for (const auto& cmd : commands) {
        std::cout << "Executing: " << cmd << std::endl; // Debug output
        system(cmd.c_str());
    }

    // Change Volume Serial Numbers
    char driveLetters[] = { 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'X', 'M', 'N', 'R', 'Q' };
    for (char drive : driveLetters) {
        std::string volumeidCommand = "cd \"" + std::string(tempFolder.begin(), tempFolder.end()) + "\" && Volumeid.exe "
            + std::string(1, drive) + ": " + GenerateVolumeSerial() + " > NUL 2>&1";
        std::cout << "Executing: " << volumeidCommand << std::endl; // Debug output
        system(volumeidCommand.c_str());
    }

    // Restart WMI service
    system("net stop winmgmt /y > NUL");
    system("timeout /t 2 /nobreak > NUL");
    system("net start winmgmt /y > NUL");

    std::cout << "Spoofing complete!" << std::endl;
}


int main() {
    KeyAuthApp.init();

    std::cout << R"(
XX   XX  EEEEEEE  NN   NN  OOOOO  NN   NN
 XX XX   EE       NNN  NN OO   OO NNN  NN
  XXX    EEEEE    NN N NN OO   OO NN N NN
 XX XX   EE       NN  NNN OO   OO NN  NNN
XX   XX  EEEEEEE  NN   NN  OOOOO  NN   NN
)" << std::endl;

    std::string license;
    std::cout << "Licence key: ";
    std::cin >> license;

    KeyAuthApp.license(license);
    if (!KeyAuthApp.response.success) {
        std::cerr << "Authentication failed: " << KeyAuthApp.response.message << std::endl;
        return EXIT_FAILURE;
    }

    std::string manufacturer = selectManufacturer();
    std::cout << "Selected manufacturer: " << manufacturer << std::endl;

    permspoof();
    std::cout << "Spoofing completed successfully." << std::endl;

    return 0;
}
