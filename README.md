# 3D Printing Placer Assistant

## Overview
3D Printing Placer Assistant is (more accurately, will be) a user-friendly tool designed to ease the 3D printing process by automating the tedious task of manual support placement. By accurately identifying overhanging parts in STL files (the task often failed by slicers such as Wanhao and Chitubox), this application not only saves time but also improves the printing success rate. It utilizes C++ for high-efficiency calculations and Python for a seamless graphical user interface (WIP), making it accessible to everyone.

## Features
- **Automatic Overhang Detection**: Quickly scans STL files to highlight all overhanging sections that require supports.
- **Visualization**: Offers a visual representation of the STL file, with overhangs prominently marked, facilitating easy inspection and planning.
- **Customizable Slicing Parameters**: Allows users to adjust slicing parameters for more precise control over the detection process.
- **Cross-Platform Compatibility**: Works on multiple operating systems, ensuring wide accessibility.

## Getting Started

### What is Considered to Be an Overhang?

In the context of 3D printing, an overhang refers to any part of the model that extends beyond a certain angle relative to the vertical axis and lacks direct support from the layers below. The 3D Printing Placer Assistant specifically identifies two types of overhangs:

1. Isolated Regions: These are portions of a model that, when viewed in a sliced layer, do not connect directly to the layer below. This effectively means they are "floating" in mid-air, creating a practical overhang angle of more than 90 degrees. These regions require support to be printed successfully.

2. Angled Overhangs: This type involves parts of the model that are connected to the layer below but at an angle greater than 45 degrees. By analyzing consecutive slices, if the normal line drawn through the edges of contours forms an angle exceeding 45 degrees with the vertical, that portion is flagged as an overhang. These areas often need support structures to maintain print quality and integrity.
