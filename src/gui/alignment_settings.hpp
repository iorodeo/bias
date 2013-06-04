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

            bool ellipseVisible;
            unsigned int ellipsePosX;
            unsigned int ellipsePosY;
            unsigned int ellipseRadiusX;
            unsigned int ellipseRadiusY;
            unsigned int ellipsePenWidth;
            QColor ellipseQColor;

            static const bool DEFAULT_GRID_VISIBLE;
            static const unsigned int DEFAULT_GRID_NUM_COL;
            static const unsigned int DEFAULT_GRID_NUM_ROW;
            static const unsigned int DEFAULT_GRID_PEN_WIDTH;
            static const QColor DEFAULT_GRID_QCOLOR;

            static const unsigned int GRID_MIN_NUM_COL;
            static const unsigned int GRID_MAX_NUM_COL; 
            static const unsigned int GRID_MIN_NUM_ROW;
            static const unsigned int GRID_MAX_NUM_ROW; 

            static const bool DEFAULT_ELLIPSE_VISIBLE;
            static const unsigned int DEFAULT_ELLIPSE_POS_X;
            static const unsigned int DEFAULT_ELLIPSE_POS_Y;
            static const unsigned int DEFAULT_ELLIPSE_RADIUS_X;
            static const unsigned int DEFAULT_ELLIPSE_RADIUS_Y;
            static const unsigned int DEFAULT_ELLIPSE_PEN_WIDTH;
            static const QColor DEFAULT_ELLIPSE_QCOLOR;

    }; // AlignmentSettings

} // namespace bias

#endif // #ifndef BIAS_ALIGNMENT_SETTINGS_HPP
