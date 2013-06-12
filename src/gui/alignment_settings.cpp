#include "alignment_settings.hpp"

namespace bias
{
    const unsigned int AlignmentSettings::GRID_MIN_NUM_COL = 1;
    const unsigned int AlignmentSettings::GRID_MAX_NUM_COL = 20; 
    const unsigned int AlignmentSettings::GRID_MIN_NUM_ROW = 1;
    const unsigned int AlignmentSettings::GRID_MAX_NUM_ROW = 20; 

    const bool AlignmentSettings::DEFAULT_GRID_VISIBLE = false;
    const unsigned int AlignmentSettings::DEFAULT_GRID_NUM_COL = 5;
    const unsigned int AlignmentSettings::DEFAULT_GRID_NUM_ROW = 5;
    const unsigned int AlignmentSettings::DEFAULT_GRID_PEN_WIDTH = 1;
    const QColor AlignmentSettings::DEFAULT_GRID_QCOLOR = QColor(0,0,255);

    const bool AlignmentSettings::DEFAULT_ELLIPSE_VISIBLE = false;
    const unsigned int AlignmentSettings::DEFAULT_ELLIPSE_POS_X = 100;
    const unsigned int AlignmentSettings::DEFAULT_ELLIPSE_POS_Y = 100;
    const unsigned int AlignmentSettings::DEFAULT_ELLIPSE_RADIUS_X = 50;
    const unsigned int AlignmentSettings::DEFAULT_ELLIPSE_RADIUS_Y = 25;
    const unsigned int AlignmentSettings::DEFAULT_ELLIPSE_PEN_WIDTH = 1;
    const QColor AlignmentSettings::DEFAULT_ELLIPSE_QCOLOR = QColor(255,255,0);

    AlignmentSettings::AlignmentSettings()
    {
        gridVisible = DEFAULT_GRID_VISIBLE;
        gridNumCol = DEFAULT_GRID_NUM_COL;
        gridNumRow = DEFAULT_GRID_NUM_ROW;
        gridPenWidth = DEFAULT_GRID_PEN_WIDTH;
        gridQColor = DEFAULT_GRID_QCOLOR;

        ellipseVisible = DEFAULT_ELLIPSE_VISIBLE;
        ellipsePosX = DEFAULT_ELLIPSE_POS_X;
        ellipsePosY = DEFAULT_ELLIPSE_POS_Y;
        ellipseRadiusX = DEFAULT_ELLIPSE_RADIUS_X;
        ellipseRadiusY = DEFAULT_ELLIPSE_RADIUS_Y;
        ellipsePenWidth = DEFAULT_ELLIPSE_PEN_WIDTH;
        ellipseQColor = DEFAULT_ELLIPSE_QCOLOR;
    };

} // namespace bias
