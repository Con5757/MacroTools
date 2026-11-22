#include "coordinateInspector.h"

CTStatus ctInspectCoordinate(CTPoint position, CTRect bounds,
                             CTCoordinateInspection *inspection) {
    if (inspection == NULL || !ctPointIsFinite(position) ||
        !ctRectIsValid(bounds)) {
        return CT_STATUS_INVALID_ARGUMENT;
    }

    const double width = ctRectWidth(bounds);
    const double height = ctRectHeight(bounds);
    const double xSpan = width > 1.0 ? width - 1.0 : 0.0;
    const double ySpan = height > 1.0 ? height - 1.0 : 0.0;

    inspection->position = position;
    inspection->relativePosition.x = position.x - bounds.left;
    inspection->relativePosition.y = position.y - bounds.top;
    inspection->normalizedPosition.x =
        xSpan > 0.0 ? inspection->relativePosition.x / xSpan : 0.0;
    inspection->normalizedPosition.y =
        ySpan > 0.0 ? inspection->relativePosition.y / ySpan : 0.0;
    inspection->withinBounds = ctRectContainsPoint(bounds, position);

    if (!ctPointIsFinite(inspection->relativePosition) ||
        !ctPointIsFinite(inspection->normalizedPosition)) {
        return CT_STATUS_OUT_OF_RANGE;
    }

    return CT_STATUS_OK;
}