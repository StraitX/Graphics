#include "platform/display.hpp"

namespace StraitX{


Error Display::Open(){
    return m_Impl.Open();
}

Error Display::Close(){
    return m_Impl.Close();
}

bool Display::IsOpen(){
    return m_Impl.IsOpened();
}

Screen Display::MainScreen(){
    return Screen(m_Impl.MainScreen());
}

DisplayImpl &Display::Impl(){
    return m_Impl;
}


}; // namespace StraitX::