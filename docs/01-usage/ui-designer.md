# UI Designer

The **UI Designer** (`UIDesignerView` and `InstrumentCanvasView`) provides a WYSIWYG interface for creating the instrument's visual panel in Decent Sampler and mapping controls for SFZ.

---

## UI Target Engines

The top toolbar offers a **UI Target Engine** selector:

1. **Decent Sampler (Full Graphical Design):** The default visual canvas for positioning controls, styling components, and binding UI elements to DSP parameters.
2. **Sforzando / SFZ (Controls Mapping):** Switches to a table view listing all instrument knobs and assigning them to standard MIDI Continuous Controllers (`CC 70` upwards) for `<control>` section generation in `.sfz` exports.

---

## Canvas & Toolbar Controls

The UI Designer toolbar includes tools for precision layout editing:

- **Alignment Actions:** Align selected components to the **Left**, **Center**, **Right**, **Top**, or **Bottom**.
- **Snap to Grid:** Toggleable grid snapping for clean alignment during drags.
- **Zoom & View Navigation:**
  - **Zoom In (`🔍+`)** / **Zoom Out (`🔍-`):** Scale the canvas view incrementally.
  - **Fit:** Fits the entire canvas inside the available editor viewport.
  - **100%:** Resets zoom level to 1:1 pixel scale.
- **Interactive Keyboard:** Embedded at the bottom of the canvas (75px height) for auditioning the instrument while tweaking interface controls.

---

## Component Palette

Components can be placed by clicking items in the **Tools** palette or by dragging images from the **UI Assets** dock:

| Component | Class | Supported Features |
|---|---|---|
| **Knob** | `UiKnob` | Default labeled knob or skinned control using vertical/rotary filmstrip images |
| **Slider** | `UiSlider` | Vertical or horizontal slider with optional filmstrip skin |
| **Button** | `UiButton` | Momentary or toggle button with custom `On` / `Off` state images |
| **Label** | `UiLabel` | Text labels with custom font size, text color, and alignment |
| **Menu** | `UiMenu` | Dropdown menu with configurable option list |
| **Shape** | `UiShape` | Vector rectangles and ellipses with fill and border stroke colors |
| **Image** | `UiImage` | Static image background element or decorative panel |
| **Multi-Frame Image** | `UiMultiFrameImage` | Multi-state image sequenced across frames |
| **Line** | `UiLine` | Clean separator line with custom thickness and color |
| **XY Pad** | `UiXYPad` | Dual-axis 2D controller binding X and Y parameters simultaneously |
| **Oscilloscope** | `UiOscilloscope` | Real-time audio waveform visualization scope |
| **Keyboard** | `UiKeyboard` | Customizable keyboard preview element |

---

## Filmstrip Skinning

Knobs and sliders can be skinned with vertical **filmstrips**:
- A single PNG image containing all animation frames stacked vertically.
- Enter the frame count in the **Properties Panel** (**Frames** field).
- The exporter transpiles skinned controls to `<control style="custom_skin_vertical_drag" customSkinImage="..." customSkinNumFrames="...">`.

---

## Drag-and-Drop with UI Assets

The **UI Assets** dock (`AssetManagerView`) on the right sidebar displays thumbnails of all image files in the project's media folder or any custom directory. Dragging an image from the asset browser directly onto the canvas automatically creates and places a `UiImage` component.

---

## Data Bindings & Macros

Controls come alive when bound to instrument parameters:
- **Binding Targets:** Group volume, pan, tuning, amplifier envelopes (Attack, Decay, Sustain, Release), and insert effect parameters.
- **Positional Addressing:** In the Decent Sampler format, insert effects are addressed positionally (`position="0"`, `position="1"`). Reordering effects in the mixer can repoint bindings; re-verify bindings when modifying the effect chain.
- **Macros:** Multi-parameter global macro knobs in the bottom dock allow a single UI knob to drive multiple target parameters across various groups and effects simultaneously.

### Control Tooltips & Modulation Routing
Knobs on synthesizer cards and macro editors feature an interactive two-section tooltip:
1. **LFO Assignment:** Quickly assign or configure LFO modulation depth directly on the knob.
2. **Macro Parameter:** Add the knob's target parameter directly to a global macro control.

---

## Canvas Background & Sizing

Clicking the empty canvas activates the **Canvas Inspector** in the Properties Panel:
- **Dimensions:** Custom width and height (default: 812 × 375 px).
- **Background Image:** Assign background artwork rendered into bundle exports.
- **Background Mode:** `Stretch`, `Center`, or `Absolute`.

---

*Last verified: 2026-08-19*
