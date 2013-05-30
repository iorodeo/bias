#ifndef BIAS_ALIGNMENT_SETTINGS_HPP
#define BIAS_ALIGNMENT_SETTINGS_HPP

#include <QColor>

namespace bias
{

    class AlignmentSettings
    {
        public:

            AlignmentSettings();

            bool gridVisible;
            unsigned int gridNumCol;
            unsigned int gridNumRow;
            unsigned int gridPenWidth;
            QColor gridQColor;

            bool circleVisible;
            unsigned int circleRadius;
            unsigned int circlePosX;
            unsigned int circlePosY;
            unsigned int circlePenWidth;
            QColor circleQColor;

    };

} // namespace bias

#endif // #ifndef BIAS_ALIGNMENT_SETTINGS_HPP
