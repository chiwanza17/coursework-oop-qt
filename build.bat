@echo off
setlocal
cd /d "%~dp0"
g++ -std=c++17 -Iinclude src\main.cpp src\ConsoleUI.cpp src\PhoneNumber.cpp src\Contact.cpp src\ContactManager.cpp src\ContactValidator.cpp src\FileStorage.cpp -o phonebook.exe
if errorlevel 1 (
    echo Compilation failed.
    exit /b 1
)
echo Build successful. Run: .\phonebook.exe
