#pragma once
#include <string>

//this is copied and refactored from libigl
namespace DRAWER {
// Returns a string with a path to an existing file
// The string is returned empty if no file is selected
// (on Linux machines, it assumes that Zenity is installed)
extern std::string fileDialogOpen();
extern std::string fileDialogSave();
}

