#include "Application.h"
#include "Adachi/Diagnostics/Log.h"
#include "Adachi/Diagnostics/Profiler.h"

#include <filesystem>

int main(int argc, char** argv){
    // Currently the working directory is every examples dir
    const auto config = Adachi::CreateApplicationConfig({ argc, argv });

    if (!config.WorkingDirectory.empty())
        std::filesystem::current_path(std::filesystem::path(ADACHI_PROJECT_ROOT)/config.WorkingDirectory);

    Adachi::Log::Init();

    ADACHI_APP_INFO(
        "Working directory: {}",
        config.WorkingDirectory
    );

    ADACHI_APP_INFO(
        "ADACHI_ENABLE_PROFILING: {}",
        ADACHI_ENABLE_PROFILING
    );

    ADACHI_PROFILE_BEGIN_SESSION("Startup", "AdachiProfile-Startup.json");
    auto app = Adachi::CreateApplication(config);
    ADACHI_PROFILE_END_SESSION();

    ADACHI_PROFILE_BEGIN_SESSION("Runtime", "AdachiProfile-Runtime.json");
    app->Run();
    ADACHI_PROFILE_END_SESSION();

    ADACHI_PROFILE_BEGIN_SESSION("Shutdown", "AdachiProfile-Shutdown.json");
    delete app;
    ADACHI_PROFILE_END_SESSION();
}