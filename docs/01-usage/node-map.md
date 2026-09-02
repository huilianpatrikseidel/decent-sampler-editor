# Node Map

The **Node Map** tab provides a graphical signal-routing view of your instrument using a node-based flow graph (`NodeMapView` and `NodeGraphView`).

## Overview

The signal network is represented as an interactive canvas containing nodes (SampleGroups, Buses, FX processors, and Modulators) connected by pins and cables.

## Managing Nodes

- **Add Node:** Right-click anywhere on the empty canvas to open the context menu and choose a node type:
  - **SampleGroup** — Sound generation / sample playing group.
  - **Bus** — Submix channel node for routing multiple groups.
  - **FX / Delay / Reverb / Filter** — Standalone or insert processing nodes.
- **Move Node:** Click and drag any node title bar to reposition it on the canvas.
- **Remove Node:** Select a node and press `Delete` (or right-click -> Remove).

## Signal Connections & Routing

- **Audio Flow:** Connect output pins of SampleGroup nodes into Bus nodes or directly into the Master output pin.
- **Modulation Flow:** Route modulation output pins into target parameter input pins.
- **Add Connection:** Click and drag from an output pin to an input pin.
- **Remove Connection:** Click an existing connection line or right-click to delete it.

## Property Inspection

Selecting any node in the Node Map populates the **Properties Panel** on the right sidebar, allowing fine-grained editing of node parameters (volume, pan, effect parameters, IDs).

*Last verified: 2026-08-18*
