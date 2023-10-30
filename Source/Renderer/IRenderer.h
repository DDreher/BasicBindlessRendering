#pragma once

class IRenderer
{
public:
    virtual ~IRenderer() = default;
    virtual void Render() = 0;
    virtual void Present() = 0;
};

// Define in the client!
class IRenderer* CreateRenderer();
