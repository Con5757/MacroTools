#include "rectangleIntersectionTester.h"

CTStatus ctTestRectangleIntersection(CTRect first, CTRect second,
                                     bool *intersects, CTRect *intersection) {
    if (intersects == NULL || !ctRectIsValid(first) || !ctRectIsValid(second)) {
        return CT_STATUS_INVALID_ARGUMENT;
    }

    const double left = first.left > second.left ? first.left : second.left;
    const double top = first.top > second.top ? first.top : second.top;
    const double right = first.right < second.right ? first.right : second.right;
    const double bottom = first.bottom < second.bottom ? first.bottom : second.bottom;

    *intersects = right > left && bottom > top;

    if (intersection != NULL) {
        if (*intersects) {
            intersection->left = left;
            intersection->top = top;
            intersection->right = right;
            intersection->bottom = bottom;
        } else {
            *intersection = (CTRect){0.0, 0.0, 0.0, 0.0};
        }
    }

    return CT_STATUS_OK;
}
