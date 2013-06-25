#ifndef PARAMETERS_HPP
#define PARAMETERS_HPP

class ImageGrabberParam
{
    public:
        float frameRate;
};

struct FlySorterParam
{
    public:
        ImageGrabberParam imageGrabber;
};

#endif // #ifndef PARAMETERS_HPP
