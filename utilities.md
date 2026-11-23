## 1. Visual Geometry & Annotation Addons

- **Bounding Box Toolkit**: Create, resize, move, scale, clip, merge, and compare bounding boxes; calculate corners, center points, dimensions, area, and overlap.
- **Rectangle Utilities**: Create and manipulate rectangles; test containment, intersection, adjacency, alignment, and distance.
- **Circle & Ellipse Utilities**: Create and manipulate circles and ellipses; calculate bounds, centers, radii, intersections, and point containment.
- **Line & Segment Utilities**: Create and manipulate lines and segments; calculate length, direction, midpoint, intersections, and nearest points.
- **Polygon & Polyline Utilities**: Create, edit, simplify, transform, and compare polygon and polyline shapes.
- **Shape Conversion Tools**: Convert between rectangles, rotated boxes, polygons, circles, ellipses, and point-based representations where supported.
- **Region-of-Interest Manager**: Define named screen or image regions, with padding, clipping, grouping, and coordinate-space metadata.
- **Bounding Box Comparison**: Calculate intersection-over-union (IoU), overlap ratios, containment, and similarity between detected boxes.
- **Shape Grouping & Filtering**: Group nearby or overlapping shapes and filter results by size, aspect ratio, area, position, or other properties.
- **Contour & Outline Adapter**: Convert image contours or detected outlines into reusable geometric shapes.
- **Annotation Data Format**: Store and exchange boxes, points, labels, polygons, confidence values, and timestamps in a consistent format.
- **Visual Overlay Renderer**: Draw boxes, outlines, points, labels, guides, paths, and measurement markers over screenshots or images.
- **Coordinate Space Adapter**: Convert annotations between image, window, client-area, monitor, desktop, and normalized coordinate spaces.
- **Anchor Point Utilities**: Define center, corner, edge, and custom anchor points for positioning and aligning visual elements.
- **Hit Testing Utilities**: Test whether points, lines, or shapes touch or fall inside other geometric objects.

## 2. Scripting & Extensibility

- **Scripting Bridge**: Expose selected MacroTools functions to scripting languages.
- **Lua Integration**: Run Lua scripts that call supported MacroTools APIs.
- **Python Integration**: Expose selected tools to Python.
- **JavaScript Integration**: Expose selected tools to JavaScript runtimes.
- **Plugin Loader**: Discover and load compatible external extensions.
- **Plugin Registry**: Maintain metadata about installed plugins and their capabilities.
- **Plugin Compatibility Checker**: Check plugin API and version compatibility.
- **Custom Action Interface**: Allow external modules to register reusable automation actions.
- **Event Subscription Bridge**: Allow extensions to subscribe to supported MacroTools events.

## 3. Data, Storage & Interchange

- **Database Adapter Pack**: Optional adapters for supported database systems.
- **Structured Data Exporter**: Export tool output to JSON, CSV, XML, or other supported formats.
- **Configuration Profiles**: Store and switch between named tool configurations.
- **Configuration Importer/Exporter**: Move settings between installations.
- **Workflow Importer/Exporter**: Share workflow definitions between users or systems.
- **Automation History Store**: Persist selected execution history and results.
- **Screenshot and Trace Archive**: Organize captured images, input traces, and related metadata.
- **Data Redaction Adapter**: Remove configured sensitive information before data is exported or shared.

## 4. Developer & Integration Addons

- **Command-Line Interface Pack**: Provide command-line wrappers for selected tools.
- **Example Project Pack**: Include small integration examples for common development environments.
- **API Binding Generator**: Generate bindings for supported languages from documented interfaces.
- **Test Fixture Pack**: Provide reusable screenshots, input traces, and sample data for testing.
- **Mock Input Provider**: Supply synthetic input events for development and tests.
- **Mock Screen Provider**: Supply test images or virtual screen data without capturing the real desktop.
- **Debug Overlay Adapter**: Visualize coordinates, regions, detections, and paths in a host application.
- **Integration Test Harness**: Test interactions between independently built MacroTools modules.