#include "Image.hpp"

namespace gui
{

    void Image::onPaint(Renderer::Renderer& r) 
    {
        Widget::onPaint(r);

        r.drawImage(path, bounds);
    }

}; // namespace gui