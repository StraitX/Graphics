#include <assert.h>
#include "platform/types.hpp"
#include "platform/keyboard.hpp"


namespace Keyboard {

static const char* KeyCodeNames[(size_t)Key::KeysCount] = {
    "Unknown",
    "Space",
    "Quote",
    "Comma",  
    "Minus", 
    "Period",  
    "Slash",  
    "Key_0",
    "Key_1",
    "Key_2",
    "Key_3",
    "Key_4",
    "Key_5",
    "Key_6",
    "Key_7",
    "Key_8",
    "Key_9",
    "Semicolon",  
    "Equal", 
    "A",
    "B",
    "C",
    "D",
    "E",
    "F",
    "G",
    "H",
    "I",
    "J",
    "K",
    "L",
    "M",
    "N",
    "O",
    "P",
    "Q",
    "R",
    "S",
    "T",
    "U",
    "V",
    "W",
    "X",
    "Y",
    "Z",
    "LeftBracket",
    "Backslash",
    "RightBracket",
    "GraveAccent",
    "Escape",
    "Enter",
    "Tab",
    "Backspace",
    "Insert",
    "Delete",
    "Right",
    "Left",
    "Down",
    "Up",
    "PageUp",
    "PageDown",
    "Home",
    "End",
    "CapsLock",
    "ScrollLock",
    "NumLock",
    "PrintScreen",
    "Pause",
    "F1",
    "F2",
    "F3",
    "F4",
    "F5",
    "F6",
    "F7",
    "F8",
    "F9",
    "F10",
    "F11",
    "F12",
    "F13",
    "F14",
    "F15",
    "F16",
    "F17",
    "F18",
    "F19",
    "F20",
    "F21",
    "F22",
    "F23",
    "F24",
    "Keypad_0",
    "Keypad_1",
    "Keypad_2",
    "Keypad_3",
    "Keypad_4",
    "Keypad_5",
    "Keypad_6",
    "Keypad_7",
    "Keypad_8",
    "Keypad_9",
    "KeypadDecimal",
    "KeypadDivide",
    "KeypadMultiply",
    "KeypadSubstract",
    "KeypadAdd",
    "KeypadEnter",
    "KeypadEqual",
    "LeftShift",
    "LeftControl",
    "LeftAlt",
    "LeftSuper",
    "RightShift",
    "RightControl",
    "RightAlt",
    "RightSuper",
    "Menu"
};

const char* GetName(Key code) {
    assert((size_t)code < (size_t)Key::KeysCount);
	return KeyCodeNames[(size_t)code];
}

}//namespace Keyboard::
