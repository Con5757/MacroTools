# Coordinate & Geometry Tools

A collection of reusable C11 utilities for coordinate inspection, coordinate
mapping, rectangle geometry, path generation, calibration and profile storage.
The modules are platform-independent and contain no `main()` functions or
console input loops, so they can be called by a Raylib application.

## Rectangle and coordinate convention

`CTRect` uses half-open bounds: `[left, right) x [top, bottom)`. The left and
top edges are included; right and bottom are excluded. Thus a rectangle with
`left=0`, `top=0`, `right=1920`, `bottom=1080` describes a 1920x1080 pixel area
with valid integer pixel coordinates from `(0, 0)` through `(1919, 1079)`.

Normalization and resolution mapping preserve the first and last pixel
coordinates. For an axis containing one pixel, its normalized coordinate is
always zero and denormalization returns that axis's only coordinate. Values
outside the source rectangle are not silently clamped; callers can validate
range separately or intentionally extrapolate.

## Tools and primary functions

| Tool | Primary API |
|---|---|
| Coordinate Inspector | `ctInspectCoordinate` |
| Coordinate Normalizer | `ctNormalizeCoordinate`, `ctDenormalizeCoordinate` |
| Coordinate Mapper | `ctMapCoordinate` |
| Coordinate Offset Calculator | `ctCalculateCoordinateOffset`, `ctApplyCoordinateOffset` |
| Coordinate Range Validator | `ctValidateCoordinateRange` |
| Rectangle Calculator | `ctCreateRectangle`, `ctCalculateRectangle` |
| Rectangle Intersection Tester | `ctTestRectangleIntersection` |
| Distance Calculator | `ctCalculateDistance` |
| Path Interpolation Tool | `ctInterpolatePath` |
| Bezier Path Generator | `ctGenerateBezierPath` |
| Grid Generator | `ctGenerateCoordinateGrid` |
| Coordinate Calibration Tool | `ctInitializeCoordinateCalibration`, `ctApplyCoordinateCalibration` |
| Coordinate Profile Manager | `ctSaveCoordinateProfile`, `ctLoadCoordinateProfile` |

## Buffer sizing

Path interpolation, Bezier generation and grid generation write into caller-owned
arrays. To determine the required element count, call the function with
`points=NULL`, `capacity=0`, and a valid `requiredCount` pointer. The function
returns `CT_STATUS_BUFFER_TOO_SMALL` and writes the required count. Allocate the
array, then call again with sufficient capacity.

## Status codes

All functions returning `CTStatus` use these shared values:

- `CT_STATUS_OK`: operation succeeded.
- `CT_STATUS_INVALID_ARGUMENT`: null pointer, non-finite input or invalid dimensions.
- `CT_STATUS_BUFFER_TOO_SMALL`: output array is missing or too small; required count is reported where available.
- `CT_STATUS_OUT_OF_RANGE`: calculation overflowed or produced a non-finite result.
- `CT_STATUS_IO_ERROR`: profile file could not be read or written.
- `CT_STATUS_INVALID_FORMAT`: profile file is malformed or uses an unsupported version.
- `CT_STATUS_DEGENERATE_CALIBRATION`: calibration source points are collinear or numerically indistinguishable from collinear.

## Example: normalize and map a screen coordinate

```c
#include "coordinateNormalizer.h"
#include "coordinateMapper.h"

CTRect source = {0.0, 0.0, 1920.0, 1080.0};
CTRect target = {-1920.0, 0.0, 0.0, 1080.0};
CTPoint position = {960.0, 540.0};
CTPoint normalized;
CTPoint mapped;

if (ctNormalizeCoordinate(position, source, &normalized) == CT_STATUS_OK &&
    ctMapCoordinate(position, source, target, &mapped) == CT_STATUS_OK) {
    // normalized and mapped are ready for the application to use.
}
```

## Compilation

The modules use standard C11 and the C math library. Example with GCC or
MinGW-w64 from this directory:

```powershell
gcc -Wall -Wextra -Werror -std=c11 -c coordinateInspector.c coordinateNormalizer.c coordinateMapper.c coordinateOffsetCalculator.c coordinateRangeValidator.c rectangleCalculator.c rectangleIntersectionTester.c distanceCalculator.c pathInterpolationTool.c bezierPathGenerator.c gridGenerator.c coordinateCalibrationTool.c coordinateProfileManager.c
```

When linking an executable with GCC/MinGW, include `-lm` if your toolchain
requires it for math functions such as `hypot`, `floorl` and `fabs`.

## Integration notes

- No Windows API calls or Raylib calls are made by these modules.
- No `main()` functions are defined.
- Call the functions directly from the GUI or application layer.
- The profile manager writes a versioned UTF-8-compatible text format; profile names must fit in 63 bytes and cannot contain line breaks. Numeric serialization follows the process C locale.
- The affine calibration tool maps three non-collinear reference points exactly (subject to floating-point precision). It is an affine transform, not a perspective/homography transform.