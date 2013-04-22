#include "ar_graphics_view.hpp"
#include <iostream>

namespace bias
{

    ARGraphicsView::ARGraphicsView(QWidget *parent) : QGraphicsView(parent)
    {
        //QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed); 
        //sizePolicy.setHeightForWidth(true);
        //setSizePolicy(sizePolicy);
    }

    int ARGraphicsView::heightForWidth(int w) const
    {
        int h = int(double(w)*(1024.0/1280.0)); 
        std::cout << "h = " << h << std::endl;
        return h;
    }

} // namespace bias
