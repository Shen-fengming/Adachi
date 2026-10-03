#pragma once

namespace Adachi{

class GraphicsContext{
public:
    virtual ~GraphicsContext() = default;

    virtual void MakeCurrent() = 0;
    virtual void SwapBuffers() = 0;

    virtual void SetVSync(bool enabled) = 0;
    virtual bool IsVSync() const = 0;
};

}