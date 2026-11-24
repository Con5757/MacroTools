#pragma once
#include "coordinateToolsCommon.h"

typedef struct {
    char name[CT_PROFILE_NAME_CAPACITY];
    CTRect sourceBounds;
    CTRect targetBounds;
    CTPoint offset;
} CTCoordinateProfile;

// save and load the text profile (versionized) -> name cant be empty and restricted to one line
CTStatus ctValidateCoordinateProfile(const CTCoordinateProfile *profile);
CTStatus ctSaveCoordinateProfile(const char *path,
                                 const CTCoordinateProfile *profile);
CTStatus ctLoadCoordinateProfile(const char *path,
                                 CTCoordinateProfile *profile);