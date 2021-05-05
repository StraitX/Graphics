#ifndef STRAITX_ENGINE_HPP
#define STRAITX_ENGINE_HPP

#include "platform/result.hpp"
#include "servers/display_server.hpp"
#include "main/application.hpp"

namespace StraitX{

class Application;

class Engine{
private:
    Application *m_Application = nullptr;
    ApplicationConfig m_ApplicationConfig = {};
    bool m_Running = true;
    Result m_ErrorWindowSystem = Result::None;
    Result m_Window = Result::None;
    Result m_ErrorApplication = Result::None, m_ErrorMX = Result::None;
public:
    Engine() = default;
    ~Engine() = default;

    int Run();

    void Stop();
    
private:
    Result Initialize();
    Result Finalize();
    void MainLoop();
};

}; // namespace StraitX::


#endif // STRAITX_ENGINE_HPP