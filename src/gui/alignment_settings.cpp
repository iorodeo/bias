#include "alignment_settings.hpp"

namespace bias
{

    const bool DEFAULT_GRID_VISIBLE = true;
    const unsigned int DEFAULT_GRID_NUM_COL = 5;
    const unsigned int DEFAULT_GRID_NUM_ROW = 5;
    const unsigned int DEFAULT_GRID_PEN_WIDTH = 1;
    const QColor DEFAULT_GRID_QCOLOR = QColor(0,0,255);

    const bool DEFAULT_CIRCLE_VISIBLE = true;
    const unsigned int DEFAULT_CIRCLE_RADIUS = 50;
    const unsigned int DEFAULT_CIRCLE_POS_X = 100;
    const unsigned int DEFAULT_CIRCLE_POS_Y = 100;
    const unsigned int DEFAULT_CIRCLE_PEN_WIDTH = 1;
    const QColor DEFAULT_CIRCLE_QCOLOR = QColor(255,255,0);

    AlignmentSettings::AlignmentSettings()
    {
        gridVisible = DEFAULT_GRID_VISIBLE;
        gridNumCol = DEFAULT_GRID_NUM_COL;
        gridNumRow = DEFAULT_GRID_NUM_ROW;
        gridPenWidth = DEFAULT_GRID_PEN_WIDTH;
        gridQColor = DEFAULT_GRID_QCOLOR;

        circleVisible = DEFAULT_CIRCLE_VISIBLE;
        circleRadius = DEFAULT_CIRCLE_RADIUS;
        circlePosX = DEFAULT_CIRCLE_POS_X;
        circlePosY = DEFAULT_CIRCLE_POS_Y;
        circlePenWidth = DEFAULT_CIRCLE_PEN_WIDTH;
        circleQColor = DEFAULT_CIRCLE_QCOLOR;

    };

} // namespace bias
