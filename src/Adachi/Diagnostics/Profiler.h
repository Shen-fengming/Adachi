#pragma once

#include "Adachi/Diagnostics/Log.h"
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <functional>
#include <utility>

namespace Adachi{
struct ProfileContent{
    std::string Name;

    std::chrono::duration<double, std::micro> StartTimeNumeric;
    std::chrono::duration<double, std::micro> ElapsedTimeNumeric;

    std::thread::id ThreadID;
};
// Upload the output from ProfilerTimer to designated json file
// Lazily constructed the first time Get() is called.
// The application should initialize logging before beginning profiling.
class Profiler {
public:
    // The name & relative location of target json file
    struct Target{
        std::string Name;
        std::ofstream OutputStream;
        //Constructor that open the stream
        Target(const std::string& name, const std::string& filepath):Name(name),OutputStream(filepath){
            if(!IsOpen()){
                if(Log::GetCoreLogger())
                    ADACHI_CORE_ERROR("Profiler could not open file '{}'", filepath);
                else
                    std::cout << "Profiler could not open file & logger not exist: " << filepath << std::endl;
            }
        };
        //Destructor that closes the stream: std::ofstream automatically closes itself when destroyed
        ~Target() = default;

        bool IsOpen() const{return OutputStream.is_open();}
    };
public:
    void BeginProfiling(const std::string& name, const std::string& filepath = "unnamed_results.json"){
        std::lock_guard<std::mutex> lock(m_Mutex);

        // Check: there is no current target
        if(m_Target){
            if(Log::GetCoreLogger())
                ADACHI_CORE_ERROR("Profiler Target already exist '{}', Construction invalid", m_Target->Name);
            else
                std::cout << "Profiler Target '"  << m_Target->Name << "'already exist: Construction invalid & Logger not exist" << std::endl;
            return;
        }

        auto target = std::make_unique<Target>(name, filepath);
        if(!target->IsOpen()){
            return;
        }

        m_Target = std::move(target);

        WriteProfileHead();
    }
    void EndProfiling(){
        std::lock_guard<std::mutex> lock(m_Mutex);

        InternalEndProfiling();
    }

    static Profiler& Get(){
        static Profiler s_Profiler;
        return s_Profiler;
    }

        void WriteProfile(const ProfileContent& content){
        std::stringstream json;

        const auto threadIDPrinted = std::hash<std::thread::id>{}(content.ThreadID);
        const std::string namePrinted = EscapeJsonString(content.Name);

        json << std::setprecision(3) << std::fixed;
        json << ",{";
        json << "\"cat\":\"function\",";                                    //category
        json << "\"dur\":" << (content.ElapsedTimeNumeric.count()) << ',';  //duration
        json << "\"name\":\"" << namePrinted << "\",";                     //name
        json << "\"ph\":\"X\",";                                            //event phase
        json << "\"pid\":0,";                                               //process id
        json << "\"tid\":" << threadIDPrinted << ",";                      //thread id
        json << "\"ts\":" << content.StartTimeNumeric.count();              // timestamp start
        json << "}";

        std::lock_guard<std::mutex> lock(m_Mutex);
        if(m_Target){
            m_Target->OutputStream << json.str();
            m_Target->OutputStream.flush();
        }
    }

    static std::string EscapeJsonString(const std::string& input)
    {
        std::string output;
        output.reserve(input.size());

        for (char character : input)
        {
            switch (character)
            {
                case '"':
                    output += "\\\"";
                    break;

                case '\\':
                    output += "\\\\";
                    break;

                case '\n':
                    output += "\\n";
                    break;

                case '\r':
                    output += "\\r";
                    break;

                case '\t':
                    output += "\\t";
                    break;

                case '\b':
                    output += "\\b";
                    break;

                case '\f':
                    output += "\\f";
                    break;

                default:
                    output += character;
                    break;
            }
        }

        return output;
    }

    Profiler(const Profiler&) = delete;
    Profiler& operator=(const Profiler&) = delete;
    Profiler(Profiler&&) = delete;
    Profiler& operator=(Profiler&&) = delete;
private:
    Profiler(){};
    ~Profiler(){
        if(m_Target){InternalEndProfiling();}
    }

    void WriteProfileHead(){
        m_Target->OutputStream << "{\"otherData\": {},\"traceEvents\":[{}";
        m_Target->OutputStream.flush();
    }

    void WriteProfileFoot(){
        m_Target->OutputStream << "]}";
		m_Target->OutputStream.flush();
    }

    void InternalEndProfiling(){
        if(!m_Target){
            if(Log::GetCoreLogger())
                ADACHI_CORE_ERROR("Cannot end profiling: no profiling session is active");
            else
                std::cout << "Cannot end profiling: no profiling session is active & Logger not exist" << std::endl;

            return;
        }

        WriteProfileFoot();
        m_Target.reset();
    }
private:
    // Make sure the json file has only 1 writer working, necessary for WriteProfile, not sure for Begin&EndProfiling
    std::mutex m_Mutex;
    std::unique_ptr<Target> m_Target;
};

// Created in the scope of current function, send profile using Profiler on destruction
class ScopedProfileTimer {
public:
    explicit ScopedProfileTimer(std::string name)
      : m_Name(std::move(name)), 
        m_StartTimeMachine(std::chrono::steady_clock::now()),
        m_Stopped(false) {};
    ~ScopedProfileTimer(){
        Stop();
    };

    void Stop(){
        if(m_Stopped)return;

        m_Stopped = true;

        auto endTimeMachine = std::chrono::steady_clock::now();
        auto startTimeNumeric = std::chrono::duration<double, std::micro>{m_StartTimeMachine.time_since_epoch()};
        auto elapsedTimeNumeric = std::chrono::duration<double, std::micro>{endTimeMachine-m_StartTimeMachine};

        Profiler::Get().WriteProfile({m_Name,startTimeNumeric,elapsedTimeNumeric, std::this_thread::get_id()});
    };

    ScopedProfileTimer(const ScopedProfileTimer&) = delete;
    ScopedProfileTimer& operator=(const ScopedProfileTimer&) = delete;
    ScopedProfileTimer(ScopedProfileTimer&&) = delete;
    ScopedProfileTimer& operator=(ScopedProfileTimer&&) = delete;
private:
    std::string m_Name;
    //Machine means that this data type is for computer&calculation rather than output
    std::chrono::time_point<std::chrono::steady_clock> m_StartTimeMachine;
    bool m_Stopped;
};

}

#ifndef ADACHI_ENABLE_PROFILING
    #define ADACHI_ENABLE_PROFILING 0
#endif

#if ADACHI_ENABLE_PROFILING

    

    // Helpers for creating a unique timer variable using its line number.
    
    

    #define ADACHI_PROFILE_BEGIN_SESSION(name, filepath)                 \
        do                                                               \
        {                                                                \
            ::Adachi::Profiler::Get().BeginProfiling(name, filepath);    \
        } while (false)

    #define ADACHI_PROFILE_END_SESSION()                  \
        do                                                \
        {                                                 \
            ::Adachi::Profiler::Get().EndProfiling();     \
        } while (false)

    // If I do ADACHI_PROFILE_SCOPE in function Adachi::Renderer::Draw(int), line 35
    // 0. ADACHI_PROFILE_FUNCTION()

    #define ADACHI_PROFILE_FUNCTION() \
        ADACHI_PROFILE_SCOPE(ADACHI_FUNCTION_SIGNATURE)
    // 1. ->ADACHI_PROFILE_SCOPE(ADACHI_FUNCTION_SIGNATURE)
    
    #if defined(__clang__) || defined(__GNUC__)
        #define ADACHI_FUNCTION_SIGNATURE __PRETTY_FUNCTION__
    #else
        #define ADACHI_FUNCTION_SIGNATURE __func__
    #endif
    // 2. ->ADACHI_PROFILE_SCOPE(__PRETTY_FUNCTION__)

    #define ADACHI_PROFILE_SCOPE(name)                                  \
        [[maybe_unused]] ::Adachi::ScopedProfileTimer                   \
            ADACHI_INTERNAL_CONCAT(profileTimer_, __LINE__){name}
    // 3. ->::Adachi::ScopedProfileTimer ADACHI_INTERNAL_CONCAT(profileTimer_, __LINE__){__PRETTY_FUNCTION__}
    
    #define ADACHI_INTERNAL_CONCAT(first, second) \
        ADACHI_INTERNAL_CONCAT_IMPL(first, second)
    // 4. ->::Adachi::ScopedProfileTimer ADACHI_INTERNAL_CONCAT_IMPL(profileTimer_, __LINE__){__PRETTY_FUNCTION__}
    // 5. ->::Adachi::ScopedProfileTimer ADACHI_INTERNAL_CONCAT_IMPL(profileTimer_, 35){__PRETTY_FUNCTION__}
    
    #define ADACHI_INTERNAL_CONCAT_IMPL(first, second) first##second
    // 6. ->::Adachi::ScopedProfileTimer profileTimer_35{__PRETTY_FUNCTION__}
    // 7. ->::Adachi::ScopedProfileTimer profileTimer_35{"Adachi::Renderer::Draw(int)""}

#else

    #define ADACHI_PROFILE_BEGIN_SESSION(name, filepath) ((void)0)
    #define ADACHI_PROFILE_END_SESSION() ((void)0)
    #define ADACHI_PROFILE_SCOPE(name) ((void)0)
    #define ADACHI_PROFILE_FUNCTION() ((void)0)

#endif