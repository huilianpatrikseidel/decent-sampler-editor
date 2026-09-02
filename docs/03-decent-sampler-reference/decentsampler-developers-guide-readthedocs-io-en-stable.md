DecentSampler

Release 1.23.5

David Hilowitz

Jul 10, 2026

## CONTENTS

1 Contents 3   
1.1 File Format Overview 3   
1.2 The <ui> element . 4   
1.3 The <groups> element (required) 18   
1.4 The <effects> element 36   
1.5 The <midi> element 48   
1.6 The <noteSequences> element 50   
1.7 The <modulators> element 51   
1.8 The <tags> element 61   
1.9 The <buses> element . 62   
1.10 Appendix A: The Color Format 64   
1.11 Appendix B: The <binding> element 64   
1.12 Appendix C: Boilerplate .dspreset File 77   
1.13 Useful Tutorials and Resources 79

DecentSampler is a free sampling plug-in that allows music composers to use multi-samples in the DecentSampler format. This document is a guide to creating samples in that format.

If you have no familiarity with the DecentSamples format, this video is a great place to start. After that, you'll want to start at the File Format Overview to learn a bit more about how to create sample libraries.

## CONTENTS

## 1.1 File Format Overview

At its core each DecentSampler sample library consists of two things: a folder containing a bunch of assets like audio files and pictures, and a single text file (called a dspreset file) which describes how the engine should use all of those files. This reference document is a guide to creating dspreset files.

dspreset files are just XML files. As such, each one begins with an XML declaration:

```xml
<?xml version="1.0" encoding="UTF-8"?>
```

## 1.1.1 The top-level <DecentSampler> element (required)

At the top level of every dspreset file is a <DecentSampler> element. Every file must have one. Here is a list of attributes:

• minVersion (optional): This is the minimum version on which this preset is known to run. If a user is running an old version of DS, and a developer has specified a min Version for their instrument, a dialog box will show up telling users that their version is outdated and that they should upgrade in order to get the full effect. They can than choose to ignore this warning or hit download. The dialog box does not show up for iOS users as most of them have auto-updates turned on.

Example:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<DecentSampler minVersion="1.0.0">
<!-- More tags go here. :) -->
</DecentSampler>
```

Underneath the top-level <DecentSampler> element you can put any number of other elements, all of which are described in the sections that follow.

## 1.2 The <ui> element

The <ui> element is how you specify a user interface for your instrument. Each dspreset should have at most one <ui> element. There are several important attributes:

• coverArt (optional): A relative or absolutely path to a cover art image to use. After the first time this library is opened, this will get displayed on the "My Libraries"' tab.

• bgImage (optional): A relative or absolutely path to a background image to use.

• bgColor (required): An eight digit hex value indicating the background color to be used for the background of the UI. This color will be drawn underneath any background image specified by bgimage.

• width (required): The width of your user interface. Recommended value: 812.

• height (required): The height of your user interface. Recommended value: 375.

## Example:

```xml
<DecentSampler>
<ui width="812" height="375">
<tab name="main">
<labeled-knob x="560" y="0" label="Tone" type="float" minValue="60" maxValue="22000

textColor="FF000000" value="22000.0" uid="y8AA4uuURh3">
<binding type="effect" level="instrument" position="0" parameter="FX_FILTER_
FREQUENCY"/>
</labeled-knob>
<label x="360" y="0" width="50" height="30" text="Reverb"/>
<control x="360" y="30" parameterName="Reverb" type="float" minValue="0" maxValue=
"1" textColor="FF000000" value="0.5">
<!-- Your <binding /> elements should go here -->
</control>
</tab>
</ui>
</DecentSampler>
```

## 1.2.1 The <tab> element

The <tab> element lives underneath the <ui> element. Every <ui> must have at most one <tab> element.

## Attributes:

• name (optional): An optional name to be associated with this tab. This is currently not displayed anywhere.

## 1.2.2 The <button> element

The <button> element allows you to create a button within your UI. It lives underneath the <tab> element. There are two styles of buttons: text buttons and image buttons.

Attributes:


| Attribute | Required | Description | Default |
| --- | --- | --- | --- |
| x | (required) | The x position of the menu | None |
| y | (required) | The y position of the menu | None |
| width | (required) | The width of the menu | None |
| height | (required) | The height of the menu | None |
| value | (optional) | The is the 0-based index of the button state that is currently selected. A value of 0 means that the first state is active. | 0 |
| style | (optional) | The type of button we want. There are two valid values: text, image. | text |
| mainImage (required | for image | For image buttons only. The path of the main image to display for this button. This can also be set at the state level so that it only applies to a specific state. | None |
| hoverImage: (optional) | buttons) | For image buttons only. The path of the main image to display when the user hovers their mouse over this button. This can also be set at the state level so that it | None |
| clickImage: (optional) |  | only applies to a specific state. For image buttons only. The path of the main image to display when the user clicks down on this button. This can also be set at the state level so that it only | None |
| disabled (optional) |  | applies to a specific state. The opacity of the button when it is disabled. This is a floating point value between 0.5 |  |
| visible (optional) |  | 0 and 1. Default: 0.5 This controls whether or not this button is visible. There are two valid values: true |  |
| enabled (optional) |  | true, false. This controls whether or not this button is enabled. There are two valid values: true |  |
| tags | (optional) | true, false. A comma-separated list of tags to be associated with this button. These can be None |  |
| tooltip (optional) |  | used in conjunction with any binding that takes a controlIndex (instead of the controlIndex) A tool tip to display when the user hovers over this control. |  |



Example:

<button x="10" y="40" width="120" height="30" style="image" value="0" mainImage= "samples/ButtonMainImage.png" hoverImage="samples/ButtonHoverImage.png" clickImage=   
"samples/ButtonSelectedImage.png"> <!-- Your button states go here -->   
</button>

## The <state> element

In order for your button to work, it must contain at least one <state> elements.

Attributes:


| Attribute | Required | Description |
| --- | --- | --- |
| name | (required for text buttons) | The text to display on a text button when this state is active |
| mainImage | (required for image buttons) | For image buttons only. The path of the main image to display for this button when the button is in the current state. |
| hoverImage (optional) |  | For image buttons only. The path of the image to display when the user hovers their mouse over this button when the button is in the current state. |
| clickImage (optional) |  | For image buttons only. The path of the image to display when the user clicks down on this button when the button is in the current state. |



In order to have your <button> elements actually do something useful, you need to attach bindings to them. Here's an example:

<button x="10" y="30" width="70" height="50" style="image" value="0" >   
<state name="English" mainImage="samples/EFlag_MainImage.png" hoverImage="samples/   
EFlag_HoverImage.png" clickImage="samples/EFlag_SelectedImage.png">   
<!-- Turn on this group -->   
<binding type="general" level="group" position="0" parameter="ENABLED"   
translation="fixed_value" translationValue="true" />   
<!-- Turn off this group −->   
<binding type="general" level="group" position="1" parameter="ENABLED"   
translation="fixed_value" translationValue="false" />   
</state>   
<state name="French" mainImage="Samples/FFlag_MainImage.png" hoverImage="Samples/   
FFlag_HoverImage.png" clickImage="Samples/FFlag_SelectedImage.png">   
<!-- Turn off this group -->   
<binding type="general" level="group" position="0" parameter="ENABLED"   
translation="fixed_value" translationValue="false" />   
<!-- Turn on this group -->   
<binding type="general" level="group" position="1" parameter="ENABLED"   
translation="fixed_value" translationValue="true" />   
</state>   
</button>

As you can see, this example uses a button to switch between two groups. You'll note the liberal use of the fixed_value translation mode above. This means that when any of these options are selected, a fixed predetermined value is used for the value of that binding.

## 1.2.3 The <image> element

The <image> element allows you to place a static image into your user interface. It lives underneath the <tab> element. Attributes:

• x (required): The x position of your image where (0,0) is the top-left corner

• y (required): The y position of your image where (0,0) is the top-left corner

• width (required): The width in pixels of the image component

• height (required): The height in pixels of the image component

• path (required): The relative path of the image file to show in this component

•  aspectRatioMode (required): Whether or not the engine should preserve the aspect ratio of the image. Note: regardless of these settings, you still need to specify a width and height for your image element. Valid values: preserve, stretch. Default value is preserve.

• opacity (optional): The opacity of the image. This is a floating point value between 0 and 1. Default: 1

• visible (optional): This controls whether or not this image is visible. There are two valid values: true (default), false.

•  tooltip (optional): A tool tip to display when the user hovers over this image.

## 1.2.4 The <multiFrameImage> element

The <multiFrameImage> element allows you to play a sequence of images as an animation. The expectation is that all the frames of the animation will be loaded in a single image, arranged in a strip — either horizontal or vertical. This is the same format as is used by the custom knobs above. It lives underneath the <tab> element. Attributes:

• x (required): The x position of your image where (0,0) is the top-left corner

• y (required): The y position of your image where (0,0) is the top-left corner

• width (required): The width in pixels of the image component

• height (required): The height in pixels of the image component

• path (required): The relative path of the image file to show in this component

• numFrames (required): The number of frames in the animation

• frameRate (required): The frame rate of the animation in frames per second. The maximum supported frame rate is 24 frames per second.

• opacity (optional): The opacity of the animation. This is a floating point value between 0 and 1. Default: 1

• sourceFormat (required):  The orientation of the frames within the image strip.  Valid values: horizontal_image_strip, vertical_image_strip.

• playbackMode (optional): The direction in which the animation should play. Valid values: forward_loop, forward_once, reverse_loop, reverse_once, ping_pong_loop (forth and back), and stopped. Default value is forward_loop.

• visible (optional): This controls whether or not this image is visible. There are two valid values: true (default), false.

• tags (optional): A comma-separated list of tags to be associated with this image. These can be used in conjunction with any binding that takes a controlIndex (instead of the controlIndex)

•  tooltip (optional): A tool tip to display when the user hovers over this image.

Example:

<multiFrameImage x="10" y="10" width="64" height="64" path="Images/Animation.png"   
numFrames="31" imageStripOrientation="vertical" frameRate="24" playbackMode="forward"/>

## 1.2.5 The <label> element

The <label> element allows you to place a static block of text into your user interface. It lives underneath the <tab> element. Attributes:

• x (required): The x position of your control where (0,0) is the top-left corner

• y (required): The y position of your control where (0,0) is the top-left corner

• text (required): The actual text that should be displayed as part of the label.

• textColor (optional): An 8 digit hex value indicating the text color to be used for the label. See Appendix A for an explanation on these hex values.

• textSize (optional): A font size for the text label. Default: 12

• width (required): The width in pixels of the label.

• height (required): The height in pixels of the label.

• vAlign (optional): The vertical alignment of the text within the box described by the width and height attributes. Valid values: top,bottom, center. Default is center.

• halign (optional): The horizontal alignment of the text within the box described by the width and height attributes. Valid values: left,right, center. Default is center.

• orientation (optional): The orientation of the text within the box described by the width and height attributes. Valid values: horizontal, vertical_up, vertical_down. Default is horizontal.

•  tags (optional): A comma-separated list of tags to be associated with this label. These can be used in conjunction with any binding that takes a controlIndex (instead of the controlIndex)

• visible (optional): This controls whether or not this text label is visible. There are two valid values: true (default), false.

• tooltip (optional): A tool tip to display when the user hovers over this label.

A label's text can also be set dynamically using bindings using the TExT binding parameter name.

## 1.2.6 The <rectangle> element

The <rectangle> element allows you to draw filled rectangles with optional borders in your UI. It lives underneath the <tab> element. This is useful for creating backgrounds, panels, dividers, or decorative elements.

Attributes:


| Attribute | Re- quired | Description | Default |
| --- | --- | --- | --- |
| x | (required) | The x position of the rectangle's top-left corner | None |
| y | (required) | The y position of the rectangle's top-left corner | None |
| width | (required) | The width of the rectangle in pixels | None |
| height | (required) | The height of the rectangle in pixels | None |
| fillcolor | (optional) | An 8-character hex value for the fill color. See Appendix A for format details. Use #00000000 for transparent fill. | #FF80808 |
| borderColor | (optional) | An 8-character hex value for the border color. See Appendix A for format details. | #0000000 |
| borderThickness | (optional) | The thickness of the border in pixels. Set to 0 for no border. | 0 |
| visible | (optional) | Controls whether the rectangle is visible. Valid values: true, false. | true |



Example:

<tab name="main'">   
<!-- Background panel -->   
<rectangle x="0" y="0" width="812" height="375" fillColor="#FF2a2a2a"/>   
<!-- Panel with border -->   
<rectangle x="30" y="70" width="240" height="135" fillColor="#FF2d2d2d" borderColor="   
−#FFe74c3c" borderThickness="1"/>   
<!-- Decorative stripe -->   
<rectangle x="0" y="0" width="8" height="255" fillColor="#FF4a90e2"/>   
</tab>

## 1.2.7 The <line> element

The <line> element allows you to draw lines in your UI. It lives underneath the <tab> element. This is useful for creating dividers, borders, or decorative elements.

Attributes:


| Attribute | Re- quired | Description | Default |
| --- | --- | --- | --- |
| x1 | (required) | The x coordinate of the line's start point | None |
| y1 | (required) | The y coordinate of the line's start point | None |
| x2 | (required) | The x coordinate of the line's end point | None |
| y2 | (required) | The y coordinate of the line's end point | None |
| lineColor | (optional) | An 8-character hex value for the line color. See Appendix A for format details. | #FFFFFFFF |
| lineThicknesss | (optional) | The thickness of the line in pixels | 1.0 |
| visible | (optional) | Controls whether the line is visible. Valid values: true, false. | true |



Example:

<tab name="main"'>   
<!-- Horizontal divider line -->   
<line x1="30" y1="95" x2="270" y2="95" lineColor="#FFe74c3c" lineThickness="2"/>   
<!-- Vertical divider line -->   
<line x1="280" y1="70" x2="280" y2="205" lineColor="#FF555555" lineThicknesss="1"/>   
<!-- Diagonal decorative line -->   
<line x1="790" y1="155" x2="804" y2="215" lineColor="#804a90e2" lineThickness="2"/>   
</tab>

## 1.2.8 The <oscilloscope> element

The <osci lloscope> element displays a live waveform of the plugin's audio output inside your UI. It lives underneath the <tab> element. The waveform is a mono mix of the left and right output channels and refreshes at approximately 20 fps.

Attributes:


| Attribute | Re- quired | Description | Default |
| --- | --- | --- | --- |
| x | (required) | The x position of the element's top-left corner | None |
| y | (required) | The y position of the element's top-left corner | None |
| width | (required) | The width of the element in pixels | None |
| height | (required) | The height of the element in pixels | None |
| backgroundColor (op- | tional) | An 8-character hex value for the background fill color. See Appendix A for #FF0000 format details. |  |
| waveColor | (optional) | An 8-character hex value for the waveform line color. See Appendix A for #FF00FF0 format details. |  |
| lineThickness | (optional) | The stroke width of the waveform line in pixels | 1.5 |
| showCenterLine | (optional) | When set to true, draws a faint horizontal center line at the zero-crossing level. Valid values: true, false. | false |
| visible | (optional) | Controls whether the oscilloscope is visible. Valid values: true, false. | true |



Example:

```xml
<tab name="main">
<oscilloscope x="10" y="10" width="300" height="80"
backgroundColor="#FF101010"
waveColor="#FFooFF88"
lineThicknesss="1.5"
showCenterLinene="true"/>
</tab>
```

## 1.2.9 The <labeled-knob> and <control> elements

The <labeled-knob> and <control> elements live underneath the <tab> element. These tags correspond to user controls (usually round radial dials) that can be used as part of a UI. These two element types are the same except that <labeled-knob> elements contain built-in labels, where as <control> elements do not. Every tab can have many <control> or <labeled-knob> elements underneath it.

For precise UI creation, it may be advisable to use a combination of <control> & <label> elements rather than <labeled-knob>.

## Attributes:

• x (required): The x position of your control where (0,0) is the top-left corner

• y (required): The y position of your control where (0,0) is the top-left corner

• width (required): The width in pixels of the knob + label.

• height (required): The height in pixels of the knob + label.

• parameterName (required): In a situation where the sampler does not have enough room to display the full UI, a shrunken down version of the UI will be used. In such situations, this control will be labeled using the parameterName. It is good practice to always include a parameterName. If not parameterName is specified and a value label is specified, then that will be used instead.

style (optional): The specific kind of control that is created. The following values are supported: linear_bar, linear_bar_vertical, linear_horizontal, linear_vertical, rotary, rotary_horizontal_drag, rotary_horizontal_vertical_drag, rotary_vertical_drag, custom_skin_vertical_drag, custom_skin_horizontal_drag. Default: rotary_vertical_drag.

• showLabel (optional): A true/false value dictating whether or not a built-in label should be displayed. Default: true for <labeled-knob> and false for <control> elements

• label (optional): If showLabel is true, the actual text that should be displayed above the control.

parameterName (required): In a situation where the sampler does not have enough room to display the full UI, a shrunken down version of the UI will be used. In such situations, this control will be labeled using the parameterName. It is good practice to always include a parameterName.

• minValue (optional): The minimum value of your control. Default: 0

• maxValue (optional): The maximum value of your control. Default: 1

• value (optional): The initial value of your control. Default: 0

• defaultValue (optional): If a user double-clicks on the control, the control's value will be set to this default value. If no default value is specified, then nothing will happen on double-click.

• valueType (optional): There are several possible values for this: float which yields numbers with two decimal points, integer which yields whole numbers, mul ti_state, which allows the user to choose between multiple states, and musical_time which yields musical time increments in beats and measures. In order to use the multi_state option, you must also specify several <state name="something"> type elements. Default: float

• textColor (optional): An 8 digit hex value indicating the text color to be used for the label. See Appendix A for an explanation on these hex values.

• textSize (optional): A font size for the text label. Default: 12

• trackForegroundColor (optional): An 8 digit hex value indicating the foreground color to use for the knob track. See Appendix A for an explanation on these hex values.

• trackBackgroundColor (optional): An 8 digit hex value indicating the background color to use for the knob track. See Appendix A for an explanation on these hex values.

• tags (optional): A comma-separated list of tags to be associated with this control. These can be used in conjunction with any binding that takes a control Index (instead of the control Index)

• visible (optional): This controls whether or not this control is visible. There are two valid values: true (default), false.

• enabled (optional): This controls whether or not this control is enabled. There are two valid values: true (default), false.

• tooltip (optional): A tool tip to display when the user hovers over this control.

• snapMode (optional): This attribute controls how the control should snap to values. Valid values: none, whole_numbers, tenths, hundredths, thousandths, and stop_points. Default: none.

• snapStopPoints (optional): A comma-separated list of values that the control should snap to when snapMode is set to stop_points. Default: none.

• defeatSnapwithShift (optional): A true/false value indicating whether or not the user can defeat the snap-tovalue behavior by holding down the shift key. Default: false.

It is also possible to use custom control graphics using the following attributes:

•  customskinImage (optional): This is path to an image to use for the control. This is expected to be a JPEG or PNG in KnobMan format. A huge gallery of compatible knobs can be found here.

• customSkinNumFrames (optional): The number of animation frames contained in the KnobMan image pointed to by customSkinImage.

• customSkinImageOrientation (optional): The orientation of the frames within the KnobMan image pointed to by customSkinImage. Valid values: horizontal, vertical. Default: vertical.

• mouseDragSensitivity (optional): An integer number describing how sensitive the control should be to mouse drags. The higher the number, the less sensitive the control will be to mouse movements.

If you are using custom knobs, it’s important that you specify a style= value of either custom_skin_vertical_drag or custom_skin_horizontal_drag.

Example:

```xml
<DecentSampler>
<ui>
<tab>
<labeled-knob x="560" y="0" label="Tone" type="float" minValue="60" maxValue="22000
 textColor="FF000000" value="22000.0">
<!-- Your <binding /> elements should go here -->
</labeled-knob>
<label x="360" y="0" width="50" height="30" text="Reverb"/>
<control x="360" y="30" parameterName="Reverb" type="float" minValue="0" maxValue=
"1" textColor="FF000000" value="0.5" style="custom_skin_vertical_drag" customSkinImage=
"Samples/ENIGMA-nolight.png" customSkinNumFrames="31" customSkinImageOrientation=
"horizontal" mouseDragSensitivity="100">
<!-- Your <binding /> elements should go here -->
</control>
</tab>
</ui>
</DecentSampler>
```

To learn how to make knobs actually control parameters of your instrument, see “Appendix B: Bindings” section below.

## Multi-State Controls

As of 1.13.6, it is now possible to use the multi_state value type for controls. This allows you to create controls that can switch between multiple states, rather than just a continuous range of values. Here is an example of how you might use one of these:

```html
<ui>
<tab>
<control x="80" y="10" style="rotary" width="84" height="84" trackForegroundColor=
"FFF06C55" trackBackgroundColor="66999999" parameterName="Language" valueType="multi_
state" value="10.0">
<state name="English" mainImage="samples/EFlag_MainImage.png" hoverImage=
"samples/EFlag_HoverImage.png" clickImage="samples/EFlag_SelectedImage.png">
<!-- Various <binding />s that get triggered when the first state is selected
->
</state>
<state name="French" mainImage="Samples/FFlag_MainImage.png" hoverImage="Samples/
FFlag_HoverImage.png" clickImage="Samples/FFlag_SelectedImage.png">
<!-- Various <binding />s that get triggered when the second state is selected␣
-->
</state>
```

(continues on next page)


| (continued from previous page) |
| --- |
| </control> |
|  |
| </tab> |
| </ui> |



## 1.2.10 The <menu> element

The <menu> element allows you to create a drop-down menu within your UI.

Attributes:


| Attribute | Re- quirec | Description |
| --- | --- | --- |
| x | (re- quired | The x position of the menu |
| y | (re- quired | The y position of the menu |
| width | (re- quired | The width of the menu |
| height | (re- quired | The height of the menu |
| value | (op- | The is the 1-based index of the menu option that is currently selected. NOTE: tional Index numbers for menu items start at 1. A value of 0 means that no item is selected. |
| tags | (op- | A comma-separated list of tags to be associated with this menu. These can be tional used in conjunction with any binding that takes a controlIndex (instead of the controlIndex) |
| visible | (op- tional | This controls whether or not this menu is visible. There are two valid values: true, true false. |
| enabled | (op- tional true, false. | This controls whether or not this menu is enabled. There are two valid values: true |
| textColor | (optional) | A hex ARGB color value for the menu text (e.g., "FFFFFFF" for white text). |
| background- Color | (optional) | A hex ARGB color value for the menu background (e.g., "FF333333" for dark gray). |
| highlighted- TextColor | (op- tional for black text). | A hex ARGB color value for the highlighted (selected) menu text (e.g.,"F000000" |
| highlighted- Background- t Color | (op- tional "FCCCCCC" for light gray). | A hex ARGB color value for the highlighted (selected) menu background (e.g., |
| vAlign | tional) | (op- The vertical alignment of the menu text. Valid values are "top", "center", "bottom". "cen- ter' |
| hAlign | (optional) | The horizontal alignment of the menu text. Valid values are "left", "center","right". "left" |
| tooltip | (optional) | A tool tip to display when the user hovers over this control. |



Example:

<menu x="10" y="40" width="120" height="30" value="2"> <!-- Your menu options go here

## The <option> element

In order for your drop-down menu to have options, it must contain <option> elements.

Attributes:


| Attribute | Required | Description |
| --- | --- | --- |
| name | (required) | The name of this element |



That’s right. The <option> element has only one attribute. In order to have your <option> elements actually do something useful, you need to attach bindings to them. Here’s an example:

```xml
<menu x="10" y="40" width="120" height="30" requireSelection="true" placeholderText=
"Choose.. ." value="2">
<option name="Menu Option 1">
<!-- Set the text of a label element -->
<binding type="control" level="ui" position="2" parameter="TEXT" translation=
"fixed_value" translationValue="You chose the first option." />
<!-- Turn on this group -->
<binding type="general" level="group" position="0" parameter="ENABLED"␣
translation="fixed_value" translationValue="true" />
<!-- Turn off this group -->
<binding type="general" level="group" position="1" parameter="ENABLED"
translation="fixed_value" translationValue="false" />
</option>
<option name="Menu Option 2">
<!-- Set the text of a label element -->
<binding type="control" level="ui" position="2" parameter="TEXT" translation=
"fixed_value" translationValue="You chose the second option." />
<!-- Turn off this group -->
<binding type="general" level="group" position="0" parameter="ENABLED" L
translation="fixed_value" translationValue="false" />
<!-- Turn on this group -->
<binding type="general" level="group" position="1" parameter="ENABLED" ␣
translation="fixed_value" translationValue="true" />
</option>
</menu>
```

As you can see, this example uses a menu to switch between two groups. It also sets the text of a text label. You’ll note the liberal use of the new fixed_value translation mode above. This means that when any of these options are selected, a fixed predetermined value is used for the value of that binding.

## 1.2.11 The <xyPad> element

The <xyPad> element allows you to create a two-dimensional pad control within your UI. This control functions almost like two sliders, one for the x-axis and one for the y-axis. Each axis has a range from 0.0 to 1.0. Each axis can be bound to a different parameter. The <xyPad> element lives underneath the <tab> element.

## Attributes:

• x (required): The x position of your control where (0,0) is the top-left corner

• y (required): The y position of your control where (0,0) is the top-left corner

• width (required): The width in pixels of the control.

• height (required): The height in pixels of the control.

• markerDiameter (optional): The diameter of the marker that moves around the control. Default: 10

• markerOutlineColor (optional): An 8 digit hex value indicating the color of the marker’s outline. See Appendix A for an explanation on these hex values.

• markerFillColor (optional): An 8 digit hex value indicating the color of the marker’s fill. See Appendix A for an explanation on these hex values.

• outlineColor (optional): An 8 digit hex value indicating the color of the control’s outline. See Appendix A for an explanation on these hex values.

• bgColor (optional): An 8 digit hex value indicating the color of the control’s background. See Appendix A for an explanation on these hex values.

• tooltip (optional): A tool tip to display when the user hovers over this control.

• xValue (optional): The initial value of the x-axis. Default: 0

• yValue (optional): The initial value of the y-axis. Default: 0

• tags (optional): A comma-separated list of tags to be associated with this control. These can be used in conjunction with any binding that takes a controlIndex (instead of the controlIndex)

• visible (optional): This controls whether or not this control is visible. There are two valid values: true (default), false.

• enabled (optional): This controls whether or not this control is enabled. There are two valid values: true (default), false.

## The <x> and <y> elements

Below the <xyPad> element, you can specify <x> and <y> elements. These elements are used to specify the bindings for the x and y axes of the control.

Example:

```xml
<xyPad x="10" y="10" width="300" height="100" parameterName="Pad" xValue="0.5" yValue="0.
5" bgColor="77FFCC00" markerFillColor="FFFFFFFF" outlineColor="77FFFFFFF">
<x>
<binding type="amp" level="group" groupIndex="0" parameter="AMP_VOLUME" translation=
"linear" translationOutputMin="0" translationOutputMax="1" />
<binding type="amp" level="group" groupIndex="1" parameter="AMP_VOLUME" translation=
"linear" translationOutputMin="1" translationOutputMax="0" />
</x>
<y>
```

(continues on next page)

<binding type="effect" level="instrument" effectIndex="0" parameter="FX_FILTER_   
FREQUENCY"   
translation="table"   
translationTable="0,33;0.3,150;0.4,450;0.5,1100;0.7,4100;0.9,11000;1.0001,22000"/   
>   
</y>   
</xyPad>

## 1.2.12 The <keyboard> element

The <keyboard> element lives underneath the <ui> element. This is where you specify settings relating to the onscreen keyboard. There should be only one <keyboard> element in your preset file. At this point, the only settings are color ranges which are specified using <color> sub-elements.

## The <color> element

You can use <color> elements to change the color of portions of the on-screen keyboard. You can have as many <color> elements as you like. Only white keys are affected. It’s worth noting that colors specified in the <color> elements are overlayed on top of the white keys using a 75% transparency, so choose your colors accordingly. This is done to preserve the readability of the key labels.

```xml
<DecentSampler>
<ui>
<!-- Other stuff here -->
<keyboard>
<color loNote="36" hiNote="50" color="FF2C365E" />
<color loNote="51" hiNote="57" color="FF6D9DC5" />
<color loNote="58" hiNote="67" color="FFCCF3F5" />
<color loNote="68" hiNote="73" color="FFE8DA9B" />
<color loNote="74" hiNote="84" color="FFD19D61" />
</keyboard>
</ui>
<!-- Other stuff here -->
</DecentSampler>
```

At- Description   
tribute   
loNote (re- The bottom of the range for which this color should be displayed. Format: MIDI Note number.   
quired)   
hiNote (re- The top of the range for which this color should be displayed. Format: MIDI Note number.   
quired)   
color (re- A text representation of the color to be used for this key range. See Appendix A for an expla  
quired) nation on these hex values.

## 1.3 The <groups> element (required)

In this section, we’ll find elements that pertain to samples and sample-mapping.

Every dspreset file should have one and only one <groups> element. This is where you specify the samples tha make up your sample library. This element lives right underneath the top-level <DecentSampler> element. The basic structure is this:

<DecentSampler>   
<groups>   
<group>   
<sample /> <!-- This is where -->   
<sample /> <!-- the samples   
<sample /> <!-- get defined   
</group>   
</groups>   
</DecentSampler>


| Attribute | Description |
| --- | --- |
| tional | volu (op- The volume of the instrument as a whole. This will be reflected in the UI in the top-right corner. Value can be in linear 0.0-1.0 or in decibels. If it's in decibels you must append dB after the value (example: "3dB"). Default: 1.0 (no volume change) |
| globa (op- tional | Global pitch adjustment for changing note pitch. In semitones. For example 1.0 would be a half-step up. Default: 0 |
| glide (op- tional | The glide/portamento time in seconds. A value of 0.0 would mean no portamento. This value can also be set at the <group> and <sample> levels, although most people will want to set it globally at the <groups> level. Default: 0.0 |
| glide (op- tional | Controls the glide/portamento behavior. Possible values are: always (glide is always performed), legato (glide is performed only when transitioning from one note to another), and off. This value can also be set at the <group> and <sample> levels, although most people will want to set it globally at the <groups> level. Default: legato |



## 1.3.1 The <group> element

Samples and oscillators live in groups. There can be many group elements under the <groups> element. It can be useful to sort your samples into groups in order to apply similar settings to them or to control them with a knob. Each group can contain <sample> elements, an <oscillator> element, or both together. The order of groups in a file matters insofar as bindings will often reference groups by using an index. The first group in a file is group 0, the second is group 1, etc.


| Attribute | Description |  |
| --- | --- | --- |
|  | enabl Whether or not this group is enabled. Possible values: true, false. Default: true | (optional) |
|  | volum The volume of the group. Value can be in linear 0.0-1.0 or in decibels. If it's in decibels you must append dB after the value (example: "3dB'"). Default: 1.0 | (optional) |
|  | ampVe The degree to which the velocity of the incoming notes affects the volume of the samples in this group. 0 = not at all. 1 = volume is completely determined by incoming velocity. When the value is 1, a velocity of 127 (max velocity) yields a gain 1.0 (full volume), a velocity of 63 (half velocity) | (optional) |
| group' | yields a gain of 0.5 (half volume), etc. Group-level pitch adjustment for changing note pitch. In semitones. For example 1.0 would be a half-step up and -1 would a half-step down. Default: 0 | (optional) |
|  | pitch A number from 0.0 to 1.0. 0 means that the pitch will stay the same regardless of what note is played. 1 means that the pitch will increase by one semitone when the note increases by one semitone (normal | (optional) |
|  | key pitch tracking). Applies to all samples and oscillators in the group. Default: 1 glide The glide/portamento time in seconds for samples and oscillators in this group. A value of 0.0 means no portamento. Can be overridden at the " level. Inherits from " level if not specified. Default: 0.0 | (optional) |
| glide | : Controls the glide/portamento behavior for samples and oscillators in this group. Possible values: "always" (glide is always performed), 'legato° (glide only when transitioning from one note to an- other), *off (no glide). Inherits from " level if not specified. Default: "legato° | (optional) |



## The <sample> element

Underneath the <group> elements are <sample> elements. Each sample corresponds to a playable “zone” of your instrument. Attributes:


| At- tribui | Description |
| --- | --- |
| path (re- | The relative path of the sample file to play for this zone. Supported audio formats are WAV (.wav), quire AIFF (.aif, . aiff), and FLAC (.flac). |
| root (re- quire | The MIDI note number (from 1 to 127) of the note. |
| loNo (op- tiona | The MIDI note number (from 1 to 127) of the lowest note for which the zone should be triggered. Default: 0. |
| hiNo (op- tiona Default: 127. | The MIDI note number (from 1 to 127) of the highest note for which the zone should be triggered. |
| loVe (op- tiona | The lowest velocity for which this zone should be triggered. Default: 0 |
| hiVe (op- tiona | The highest velocity for which this zone should be triggered. Default: 127 |
| locc (op- | Using these parameter, you can use MIDI continuous controllers to filter whether or not a note should hiCC tiona be played. This lets you, for example, have one set of samples that get played when the piano sustain pedal is down and another set that get played when it is up. Each time a MIDI CC value comes for a specific CC#, the engine stores that value. When a "note on'" signal is received, the engine makes a decision (based on the last received value and the range defined by these attributes) about whether or not this sample should be played. If you use loCCN, you must also use a corresponding hiCCN for the same MIDI CC number so that you are defining a range of values. Example: 1oCC64="90" and hiCC64=" 127" would mean that a "note on" message will only trigger this sample if the last received value for CC64 (Sustain Pedal) is between 90 and 127. This can also be set at the <group> level. |
| star (op- end (op- | The frame/sample position of the start of the sample audio. This is useful if the sample starts midway tiona through the audio file. Default: 0 |
| tuni (op- | The frame/sample position of the end of the sample audio. The is useful is the zone ends before the tiona end of the audio file. Default: the file's length in samples minus 1. |
| tiona Default: 0 volu (op- | A fine-tuning number (in semitones) for changing the note pitch. e.g 1.0 would be a half-step up. |
|  | The volume of the sample. Value can be in linear 0.0-1.0 or in decibels. If it's in decibels you must tiona append dB after the value (example: "3dB'"). Default: 1.0 |
| pan (op- tiona | A number of -100 to 100. -100 in panned all the way to the left, 100 is panned all the way to the right. This can also be set at the <group> or <groups> levels. Default: 0 |
| pitc (op- | A number from 0.0 to 1.0. 0 means that the pitch will stay the same regardless of what note is played. tiona 1 means that the pitch will increase by one semitone when the note increases by one semitone (i.e. normal key pitch tracking). This can also be set at the <group> level. Default: 1 |
| glid (op- tiona | The glide/portamento time in seconds for this sample. A value of 0.0 means no portamento. Inherits from " or ** level if not specified. Default: 0.0 |
| glid (op- tiona | Controls the glide/portamento behavior for this sample. Possible values: "always", 'legato", or *off . Inherits from " or " level if not specified. Default: "1legato |
| trig (op- | Valid values: at tack means a sample is played when the note on message is received. release means tiona the sample is played when the note off message is received (aka a release trigger). first means that the sample will only be played if no other notes are playing. legato means that the sample will only |
| rele (op- | be played if some other notes are already playing. continuous means that the sample will always play. This can also be set at the <group> level. Default: attack. 'Controls the volume decay rate for release-triggered samples based on how long the note was held. tiona When a release trigger sample plays, its volume decreases based on the time elapsed since the note was initially pressed. The value can be specified in two formats: Decibel format (recommended): Append "dB"to specify decay in decibels per second. For example, " 3dB" means the volume decreases by 3dB |
|  | for each second the note was held. If you specify a positive dB value, it will be automatically converted to negative (decay direction). Linear format: A decimal value between 0.0 and 1.0 specifying the linear gain decay factor per second. For example, 0.3 means the volume decreases by a factor of 0.3 each second. This is useful for piano pedal-up samples and other release triggers where shorter notes |
| 20 | should have louder release samples. This can also be set at the <group> or <grehaptefyels Cofehts |
|  | 0.0 (no decay). tags (op- A command-separated list of tags. Example: tags="rt,mic1". These are useful when controlling vol- tiona umes using tags. See Appendix D. |



## Looping


| Attribute | Description |
| --- | --- |
| tional | loops (op- The frame/sample position of the start of the sample's loop. If this is not specified, but the sample is a wave file with embedded loop markers, those will be used instead. Default: 0 |
| loopE (op- tional | The frame/sample position of the end of the sample's loop. If this is not specified, but the sample is a wave file with embedded loop markers, those will be used instead. Default: the file's length in samples minus 1. |
| loopC (op- tional | When loop crossfades are used, instead of simply looping at a specific end point, a portion of the audio from before the loop point is faded in just as the audio from the end of the loop is faded out. In this way, smooth audio loops can be achieved on samples that weren't specifically prepared as looping. This parameter is used for specifying the length of the crossade region in frames/samples. |
| loopC (op- tiona | This can also be set at the <group> level. Default: 0 (crossfades off). This parameter is used to specify the curve used for crossfading when loop crossfades are turned on. This can also be set at the <group> level. Value values: linear, equal_power. Default: |
| loopE (op- tional | equal_power. A boolean value indicating whether or not the loop should be used. Valid values: true, false |



## Amplitude Envelope

Each sample has its own ADSR amplitude envelope.


| Attribute | Description |  |
| --- | --- | --- |
| ampEnvEnabl (op- | tional) | This turns the amplitude envelope on and off. Valid values are: false and true (default). |
| attack | (optional) | The attack time in seconds of the amplitude envelope of this zone. This can also be set at the <group> or <groups> levels. |
| decay | (optional) | The decay time in seconds of the amplitude envelope of this zone. This can also be set at the <group> or <groups> levels. |
| sustain | (optional) | The sustain level (0.0 - 1.0) of the amplitude envelope of this zone. This can also be set at the <group> or <groups> levels. |
| release | (optional) | The release time in seconds of the amplitude envelope of this zone. This can also be set at the <group> or <groups> levels. |



The curve shapes of the attack, decay, and release zones can be changed as well. All three of the of the following parameters use the same system: a value from -100 to 100 that determines the shape of the curve. -100 is a logarithmic curve, 0 is a linear curve, and 100 is an exponential curve.


| Attribute | Description | Default Value |
| --- | --- | --- |
| attackCu (optional) | A value from -100 to 100 that determines the shape of the attack curve. This can also be set at the <group> or <groups> levels. | -100 (loga- rithmic) |
| decayCur (optional) | A value from -100 to 100 that determines the shape of the decay curve. This can also be set at the <group> or <groups> levels. | 100 (expo- nential) |
| releaseC1 (optional) | A value from -100 to 100 that determines the shape of the release curves. This can also be set at the <group> or <groups> levels. | 100 (expo- nential) |



## Round Robins

Round robins allow different samples to be played each time a zone is triggered. This is especially useful with sounds that have short attacks (such as drums), and is a great way to keep your sample libraries from sounding fake. In order for round robins to work, you must specify both a seqMode and a seqPosition for all samples. If you have several different sets of round robins with different lengths, you’ll want to set the seqLength value as well. There are several round-robin modes:

• round_robin: This causes samples to be triggered sequentially according to their seqPosition values.

• random: This causes random samples to be chosen from within the group of samples. If there are more than two round robins, then the algorithm makes sure not to hit the same one twice in a row.

• true_random: This causes random samples to be chosen from within the group of samples.

• always: This just turns round robins off.


| Attribute | Description |
| --- | --- |
|  | seqMod (op- Valid values are random, true_random, round_robin, and always. A value indicating the de- tional sired round robin behavior for this sample or group of samples. This can also be set at the <group> and <groups> levels. Default: always |
|  | seqLer (op- The length of the round robin queue. This can also be set at the <group> or <groups> levels. If this tional is left out, then the engine willtry to auto-detect the length of the roudn robin sequence. Default: 0 |
|  | seqPos (op- A number indicating this zone's position in the round robin queue. This can also be set at the tional <group> level. Default: 1 |



## Voice-Muting / Legato

## Looping


| At- tribut | Description |
| --- | --- |
| sile( (op- | A command-separated list of tags. Example: tags="'rt,mic1". If a sample containing one of these tags tiona gets triggered, then this sample will be stopped. This is useful when setting up drums as it will allow you mute one hi-hat when another hi-hat plays. See Appendix D. |
| sile (op- tiona | Controls how quickly voices get silenced. fast = immediately; normal = triggers the sample's release phase. This second option, when used in conjunction with the release attribute, allows you to specify a longer release time. Values: fast, normal. Default: fast |
| sile (op- tiona | Specifi es a custom fade-out time in seconds when a voice is silenced by the silencedByTags mech- anism or tag polyphony limits. When set to a value greater than 0, this overrides the silencingMode behavior and uses the specified decay time for the fade-out. This provides precise control over silenc- ing transitions. For example, silencingDecay="0.05" creates a 50ms fade-out. Default: 0 (uses |
| prev: (op- tiona | silencingMode instead) Only play this sample if the previously triggered note equals one of these notes. Format: a comma- separated list of MIDI note numbers (from 1 to 127) of the note. |
| lega : (op- tiona | This is similar to the previousNote attribute. This causes the engine to only play the sample if the previously triggered note is exactly this semitone distance from the previous note. For example, if the note for which this sample is being triggered is a C3 and the legatoInterval is set to -2, then the sample will only play if the previous note was a D3 because D3 minus 2 semitones equals C3. Format: This can be a positive or negative whole number. |



## Routing Audio

My default all audio is routed to the main output. However, using the attributes below you can route audio to any of the 16 buses or directly to any one of the 16 auxiliary outputs. You can also specify the volume of the audio being sent

to each output.


| At- Descriptiontribui |
| --- |
| outp(op-The first audio output for this sample. This is a string that specifies the audio output that the sampletionai should be routed to. The available options are MAIN_OUTPUT (the main audio output), NO_OUTPUT (au-dio is not routed anywhere), BUS_1 (the first bus defined in the <buses> element), BUS_2 (the secondbus), .., BUS_16 (bus 16), AUX_STERE0_OUTPUT_1 (auxiliary output 1), AUX_STERE0_0UTPUT_2(auxiliary output 2), ..., AUX_STERE0_OUTPUT_16 (auxiliary output 16). Default: MAIN_OUTPUToutp(op- The second audio output for this sample. This is a string that specifies the audio output that the sampletionashould be routed to. The available options are the same as for output 1Target. Default: No_OUTPUT |
|  |
| outp(op- The third audio output for this sample. This is a string that specifies the audio output that the sampletionaishould be routed to. The available options are the same as for output 1Target. Default: No_oUTPUT |
| outp(op- The fourth audio output for this sample. This is a string that specifies the audio output that the sampleshould be routed to. The available options are the same as for output 1Target. Default: No_OUTPUT |
|  |
| outp(op- |
|  |
| outp |
|  |
| outp |
|  |
| outp |
|  |
| outp |
|  |
| outp |
|  |
| outp |
|  |
| outp |
|  |
| outp |
|  |
| outp |
|  |
| outp |
|  |
| outp(op- |
| tionaibetween 0.0 and 1.0, where 0.0 is silent and 1.0 is full volume. Default: 1.0 |



## The <oscillator> element

The <oscillator> element allows you to add synthesized waveforms to your instrument, which can be used alongside or instead of traditional audio samples. Oscillators are useful for creating sub-bass layers, adding harmonic content, or building entirely synthesized instruments within DecentSampler.

Oscillators live inside <group> elements, just like <sample> elements. Each group can contain samples, an oscillator, or both, allowing you to layer oscillators with samples within the same group or create multi-oscillator patches by using multiple groups.

## Basic Usage:

The simplest oscillator configuration requires only a waveform type:

```xml
<group name="Sub Bass" volume="0.6" groupTuning="-12">
<oscillator waveform="sine" />
</group>
```

This creates a sine wave oscillator tuned one octave below the played note.

## Attributes:

The <oscillator> element itself has only one attribute:


| Attribute | Re- quire | Description | Default |
| --- | --- | --- | --- |
| wavef | (op- tionai | The waveform shape. Valid values: sine, saw, square, triangle, noise (or white_noise), pluck1, wavetable, harmonic, fm6op. Only for pluck1 waveform. Controls the decay time of the plucked string. Range: 0.0 to | sine |
| dampi | (op- tiona | 1.0. Lower values (closer to 0.0) create heavily damped, shorter sounds. Higher values (closer to 1.0) create minimal damping with longer, more resonant decay. This simulates the natural damping characteristics of string materials and playing techniques. | 0. 5 |
|  | pluck (op- tiona | Only for pluck1 waveform. Blends between different excitation signals to control the timbral character. Range: 0.0 to 1.0. At 0.0, the oscillator uses a smooth triangle wave excitation producing a softer, mellower tone. At 1.0, it uses a noise burst excitation producing a brighter, more aggressive attack with richer harmonics. Intermediate values blend between the two | 0. 5 |
| wavet | (op- tiona | extremes. Only for wavetable waveform. Path to the multi-frame wavetable .wav file, relative to the .dspreset file. The file should contain all wavetable frames concatenated in a single audio file. If the file contains a clm RIFF chunk (Serum-compatible format), the frame size is | (none |
| wavet | (op- tiona | detected automatically. Only for wavetable waveform. Number of audio samples per wavetable frame. When the wavetable file contains a clm RIFF chunk, this value is read automatically and any explicit setting is overridden. | 2048 |
| wavet | (op- tiona | Only for wavetable waveform. The initial playback position within the wavetable, ex- pressed as a normalized value from 0.0 (first frame) to 1.0 (last frame). Intermediate values produce a linear crossfade between adjacent frames. Can be animated using the | о. о |
| rando: (op- | tionai | OSCILLATOR_WAVETABLE_POSITION binding parameter. Only for wavetable waveform. When true, each note-on randomizes the oscillator's start phase instead of always resetting to zero. Strongly recommended when layering multiple wavetable groups (e.g. detuned unison voices) to prevent phase cancellation between voices. | fals |
| wavet (op- | tionai | Has no eff ect on other waveform types. Only for wavetable waveform. Controls whether adjacent wavetable frames are linearly crossfaded as the position moves. When true (default), the position knob smoothly interpo- lates between adjacent frames — ideal for morphing wavetables where intermediate timbres are musically meaningful. When false, the oscillator snaps to the nearest integer frame with no crossfading — useful for wavetables that contain discrete, unrelated shapes (e.g. sine, tri- angle, saw, square) where blended intermediates are unwanted. Can be changed at runtime | true |



When using waveform="harmonic", you can define additive harmonic content using these optional attributes (typically set on the parent <group> element):


| Attribute | Description | Default |
| --- | --- | --- |
| numPartials | Number of active harmonic partials. Integer range: 1–64. | 8 |
| harmonicTilt | Spectral tilt control for harmonic partials. Range: -1.0 to 1.0. Positive values reduce higher partials (darker tone), negative values emphasize higher partials | 0. о |
| harmonicOddEvenBalance | (brighter tone). Crossfades between odd and even partial emphasis. Range: 0.0 to 1.0. 0.0 emphasizes odd partials, 1.0 emphasizes even partials, 0.5 is balanced. | о. 5 |
| harmonicNormalization | Loudness compensation amount for summed harmonic energy. Range: 0.0 to 1.0. 0.0 disables compensation, 1.0 applies full compensation. | 0. 0 |
| harmonicPartial1Level harmonicPartial64Level | Per-partial amplitude controls for partials 1–64. Each level is a floating-point value from 0.0 to 1.0. | о. 0 |



harmonicPartial1Level controls the fundamental, harmonicPartial2Level controls the first overtone, and so on.

When using waveform="fm6op", set the FM parameters on the parent <group> element (they are inherited by the oscillator). All FM attributes are optional:


| Attribute | Description | Default |
| --- | --- | --- |
| fmAlgorithm | DX7-compatible algorithm number (1–32). Selects the operator routing topol- ogy—which operators are carriers (produce audio) and which are modulators. | 1 |
| fmOp1Ratio | Frequency ratio for operator 1 (primary carrier in most algorithms). Multiplies the played note frequency. 2.0 = one octave up, 0.5 = one octave down. | 1. 0 |
| fmOp1Detune | DX7-compatible pitch detune for operator 1. Range -7 to +7. Uses the classic DX7 detune algorithm: higher ptch of sets at lower notes, smaller off sets at higher notes. 0 = no detune, positive values sharpen, negative values flatten. | о |
| fmOp1Mode | Frequency mode for operator 1. ratio (default) = frequency is a ratio of the played note. fixed = absolute frequency in Hz specified by fmOp1FixedFreq. | rati |
| fmOp1FixedFreq | Fixed frequency in Hz for operator 1 when fmOp1Mode=" fixed". Ignored when mode is ratio. Useful for metallic/bell-like timbres. | 440. 0 |
| fmOp1Level | Output/modulation level for operator 1. Range 0.0–1.0. | 1. 0 |
|  | fm0p1VelocitySensit: Velocity sensitivity for operator 1. Range 0–7 (DX7 convention). 0 = no velocity response (full level regardless of velocity). 7 = maximum velocity scaling (level | о |
| fmOp1Feedback | varies from 0 at vel=1 to full at vel=127). Self-feedback amount for operator 1. Range 0.0–1.0. Note: only the physically correct feedback operator for each algorithm produces sound (usually Op6). | 0. 0 |
| fmOp1Attack | ADSR attack time (seconds) for operator 1's internal envelope. A value of -1 means the operator has no independent envelope and is gated only by the outer group ADSR. | 0. 0 |
| fmOp1Decay | ADSR decay time (seconds) for operator 1. | 0. 0 |
| fmOp1Sustain | ADSR sustain level (0.0–1.0) for operator 1. | 1. 0 |
| fmOp1Release | ADSR release time (seconds) for operator 1. A value of –1 (sentinel) lets the outer group ADSR control the release. | -1. 0 |
| fmOp2Ratio fm0p6Ratio | Frequency ratios for operators 2–6. Same semantics as fmOp1Ratio. | 1. 0 |
| fmOp2Detune fmOp6Detune | Pitch detune for operators 2–6. Range -7 to +7. Same semantics as fmOp1Detune. | 0 |
| fmOp2Mode fmOp6Mode | Frequency modes for operators 2–6. ratio or fixed. Same semantics as fmOp1Mode. | rati |
| fmOp2FixedFreq fmOp6FixedFreq | Fixed frequencies in Hz for operators 2–6 when respective mode is fixed. | 440. 0 |
| fmOp2Level fmOp6Level | Output/modulation levels for operators 2–6. Range 0.0–1.0. | 1. 0 |
| fmOp2VelocitySensit: Velocity sensitivity for operators 2-6. R | Range 0-7. Same semantics as fmOp1VelocitySensitivity. |  |
| fm0p6VelocitySensit: | Self-feedback amounts for operators 2–6. fmOp6Feedback is the primary feed-6 |  |
| fm0Op2Feedback fm0p6Feedback | back aand afects all algorithms. | о. 0 |
| fmOp2Attack fmOp6Attack | ADSR attack times for operators 2–6. | 0. 0 |
| fmOp2Decay fmOp6Decay | ADSR decay times for operators 2–6. | 0. 0 |
| fmOp2Sustain fm0p6Sustain | ADSR sustain levels for operators 2–6. Range 0.0–1.0. | 1. 0 |
| fmOp2Release fmOp6Release | ADSR release times for operators 2–6. -1 = sentinel (outer ADSR controls). | -1. 0 |



DX7-compatible operator envelope (optional). Each operator can alternatively use a 4-stage DX7-style envelope instead of the ADSR above. Set fmOpNEgType="dx7" on the group to activate it, then provide the eight rate/level parameters:


| Attribute | Description | Default |
| --- | --- | --- |
| fmOp1EgType | Envelope type for operator 1. adsr = standard ADSR (default). dx7 = 4-stage DX7- compatible envelope using integer rate/level parameters below. | adsr |
| fmOp1EgRate1 fm0p1EgRate4 | Envelope rates R1–R4 for operator 1 (DX7 mode only). Range 0–99. Higher values are faster. R1 = attack rate, R2 = decay rate, R3 = sustain rate (holds at L3 while key is held), R4 = release rate. | 99, 99, 0,99 |
| fmOp1EgLevel1 | Envelope target levels L1-L4 for operator 1 (DX7 mode only). Range 0-99. L1 = peak after attack, L2 = level after first decay, L3 = sustain level (held while key is | 99, 99, |
| fmOp1EgLevel4 fmOp2EgType | down), L4 = level after release. Envelope types for operators 2–6. Same as fmOp1EgType. | 99,0 adsr |
| fmOp6EgType fmOp2EgRate1 *. | Envelope rates R1–R4 for operators 2–6 (DX7 mode only). Same semantics as fm0p1EgRate1-fmOp1EgRate4. | 99, 99, |
| fmOp6EgRate4 fmOp2EgLevel1 | Envelope levels L1–L4 for operators 2–6 (DX7 mode only). Same semantics as | 0,99 99, |
| fmOp6EgLevel4 | fm0p1EgLevel1–fmOp1EgLevel4. | 99, 99,0 |



The DX7 rate/level envelope uses the same integer parameter conventions as classic 6-operator DX7 synthesizers: rates and levels are both in the range 0–99, and the values are directly compatible with patch data from vintage hardware and standard DX7 patch banks. See How to Use FM Synthesis for worked examples.

DX7-compatible operator envelope (optional). Each operator can alternatively use a 4-stage DX7-style envelope instead of the ADSR above. Set fmOpNEgType="dx7" on the group to activate it, then provide the eight rate/level parameters:


| Attribute | Description | Default |
| --- | --- | --- |
| fmOp1EgType | Envelope type for operator 1. adsr = standard ADSR (default). dx7 = 4-stage DX7- compatible envelope using integer rate/level parameters below. | adsr |
| fmOp1EgRate1 | Envelope rates R1–R4 for operator 1 (DX7 mode only). Range 0-99. Higher values are faster. R1 = attack rate, R2 = decay rate, R3 = sustain rate (holds at L3 while key | 99, 99, |
| fmOp1EgRate4 fmOp1EgLevel1 | is held), R4 = release rate. Envelope target levels L1–L4 for operator 1 (DX7 mode only). Range 0-99. L1 = peak after atack, L2 = level after first decay, L3 = sustain level (held while key is | 0,99 99, 99, |
| fmOp1EgLevel4 fmOp2EgType | down), L4 = level after release. Envelope types for operators 2–6. Same as fm0p1EgType. | 99,0 adsr |
| fmOp6EgType fm0p2EgRate1 | Envelope rates R1–R4 for operators 2–6 (DX7 mode only). Same semantics as | 99, |
| fm0p6EgRate4 | fm0p1EgRate1-fmOp1EgRate4. | 99, 0,99 |
| fmOp2EgLevel1 fmOp6EgLevel4 | Envelope levels L1–L4 for operators 2–6 (DX7 mode only). Same semantics as fm0p1EgLevel1–fmOp1EgLevel4. | 99, 99, 99,0 |



The DX7 rate/level envelope uses the same integer parameter conventions as classic 6-operator DX7 synthesizers: rates and levels are both in the range 0–99, and the values are directly compatible with patch data from vintage hardware and standard DX7 patch banks. See How to Use FM Synthesis for worked examples.

All FM operator parameters can be modulated in real time using bindings with the parameter names OSCILLATOR_FM_ALGORITHM, OSCILLATOR_FM_OP1_LEVEL OSCILLATOR_FM_OP6_LEVEL, OSCILLATOR_FM_OP1_RATIO . . . OSCILLATOR_FM_OP6_RATIO, and OSCILLATOR_FM_OP1_FEEDBACK OSCILLATOR_FM_OP6_FEEDBACK. See Appendix B fo independent per-operator ADSR envelope, and an optional DX7-compatible 4-stage rate/level

All other oscillator parameters are inherited from the parent <group> element.

## Waveform Types:

• \`sine\`: A pure sine wave with no harmonics. Ideal for sub-bass and clean tones.

• \`saw\`: A sawtooth wave with rich harmonics. Great for bright, cutting sounds.

• \`square\`: A square wave with odd harmonics. Useful for hollow, reed-like tones.

• \`triangle\`: A triangle wave with fewer harmonics than saw. Produces a mellower tone.

• \`noise\` (or \`white_noise\`): White noise generator. Perfect for percussion textures, hi-hats, snares, wind sounds, and adding grit to other sounds.

• pluck1: A physical modeling oscillator based on digital waveguide synthesis that simulates plucked string behavior. Unlike traditional waveforms that cycle continuously, pluck1 generates a single excitation (like plucking a string) that decays naturally over time. Ideal for synthesizing plucked string instruments like guitar, bass, harp, koto, gayageum, and other stringed instruments. This oscillator includes two parameters for controlling the sound character: damping and pluckType.

• wavetable: A user-defined wavetable oscillator that reads frames from a multi-frame .wav file. The playback position (which frame is played) is controlled by the wavetablePosition attribute and can be scanned in real time using the OSCILLATOR_WAVETABLE_POSITION binding parameter—allowing LFOs, envelopes, MIDI CCs, or UI knobs to sweep through timbres. Supports Serum-compatible wavetable files with automatic frame size detection via the clm RIFF chunk.

• harmonic: An additive oscillator that sums harmonic partials. Use numPartials to control how many partials are active (1–64), and harmonicPartial1Level . . . harmonicPartial64Level (0.0–1.0) to set each partial’s level. Use harmonicTilt for brightness shaping, harmonicOddEvenBalance to bias odd/even harmonics, and harmonicNormalization for loudness compensation. These parameters are bindable and modulatable via oscillator harmonic binding parameters in Appendix B.

• fm6op: A 6-operator FM synthesizer implementing the 32 classic Yamaha DX7 algorithm topologies. Each algorithm defines which operators are carriers (they produce the final audio output) and which are modulators (they phase-modulate another operator to add harmonics). Every operator has its own frequency ratio, output level, feedback amount, an independent per-operator ADSR envelope, and an optional DX7-compatible 4-stage rate/level envelope. Set fmAlgorithm plus the fmOp1–fmOp6 family of attributes on the parent <group> element. See How to Use FM Synthesis for a full guide. Group-Level Parameters:

Oscillators inherit and respect most of the same parameters as samples when set at the <group> level, including:

• \`groupTuning\` - Pitch adjustment in semitones

• \`loNote\` / \`hiNote\` - Note range triggering

• \`attack\`, \`decay\`, \`sustain\`, \`release\` - ADSR envelope parameters

• \`attackCurve\`, \`decayCurve\`, \`releaseCurve\` - Envelope curve shapes

• \`volume\` - Oscillator group volume

• \`pan\` - Stereo positioning

• \`ampVelTrack\` - Velocity sensitivity

• \`pitchKeyTrack\` - Keyboard pitch tracking (0.0 to 1.0). A value of 0 means the oscillator stays at a fixed pitch regardless of which key is pressed. A value of 1 means normal keyboard tracking (default)

• \`glideTime\` - Portamento/glide time in seconds (inherited from \`\` level if not specified)

• \`glideMode\` - Portamento behavior: \`always\`, \`legato\`, or \`off\` (inherited from \`\` level if not specified)

## Modulation Support:

Oscillators fully support modulation through bindings and modulators, just like samples:

• GROUP_TUNING - Modulate oscillator pitch (e.g., for vibrato LFOs)

• GLOBAL_TUNING - Global pitch modulation affects all oscillators

• GROUP_VOLUME - Modulate oscillator amplitude

• AMP_VOLUME - Control oscillator volume with UI controls or MIDI

Example with LFO vibrato:

```xml
<modulators>
<lfo shape="sine" frequency="5.0" modAmount="0.15" scope="global">
<binding type="amp" level="group" position="0" parameter="GROUP_TUNING"
modBehavior="add" translation="linear"
translationOutputMin="-0.5" translationOutputMax="0.5"/>
</lfo>
</modulators>
```

## Multi-Oscillator Example:

Here’s a practical example showing how to layer multiple oscillators to create a rich synthesized sound:

```xml
<DecentSampler minVersion="1.15.0">
<groups>
<!-- Fundamental saw wave -->
<group name="Fundamental" volume="0.5" attack="0.01" decay="0.3" sustain="0.7"␣
release="0.5">
<oscillator waveform="saw" />
</group>
<!-- Sine wave octave up for brightness -->
<group name="Octave Up" volume="0.3" groupTuning="12" attack="0.02" decay="0.3"␣
sustain="0.7" release="0.5">
<oscillator waveform="sine" />
</group>
<!-- Sub bass (limited to lower notes) -->
<group name="Sub Bass" volume="0.6" groupTuning="-12" loNote="0" hiNote="60"
attack="0.01" decay="0.3" sustain="0.7" release="0.8">
<oscillator waveform="sine" />
</group>
<!-- Detuned oscillators for stereo width -->
<group name="Detuned Left" volume="0.25" pan="-50" groupTuning="-0.1"
attack="0.01" decay="0.3" sustain="0.7" release="0.5">
<oscillator waveform="saw" />
```

(continues on next page)

```html
(continued from previous page) (continued from previous page)
</group>
<group name="Detuned Right" volume="0.25" pan="50" groupTuning="0.1"
attack="0.01" decay="0.3" sustain="0.7" release="0.5">
<oscillator waveform="saw" />
</group>
</groups>
<effects>
<effect type="lowpass" frequency="8000.0" resonance="1.0"/>
<effect type="reverb" wetLevel="0.15" roomSize="0.5"/>
</effects>
</DecentSampler>
```

## Monophonic Lead Synth with Portamento:

Here’s a complete example of a Moog-style monophonic lead synthesizer with portamento:

```xml
<DecentSampler minVersion="1.15.0">
<ui width="812" height="375">
<tab name="main">
<labeled-knob x="10" y="20" width="90" label="Attack" type="float"
minValue="0.0" maxValue="2.0" value="0.01">
<binding type="amp" level="instrument" position="0" parameter="ENV_ATTACK"/>
</labeled-knob>
<labeled-knob x="110" y="20" width="90" label="Glide Time" type="float"
minValue="0.0" maxValue="2.0" value="0.3">
<binding type="general" level="instrument" position="0" parameter="GLIDE_TIME"/>
</labeled-knob>
<labeled-knob x="210" y="20" width="90" label="Cutoff" type="float"
minValue="0" maxValue="22000" value="2000.0">
<binding type="effect" level="instrument" position="0" parameter="FX_FILTER_
FREQUENCY"/>
</labeled-knob>
<labeled-knob x="10" y="130" width="90" label="Detune" type="float"
minValue="-1" maxValue="1" value="0.1">
<binding type="amp" level="group" position="1" parameter="GROUP_TUNING"/>
</labeled-knob>
</tab>
</ui>
<!-- glideTime and glideMode set at groups level apply to all oscillators -->
<groups glideTime="0.3" glideMode="legato" attack="0.01" decay="0.3"
sustain="0.7" release="0.5" volume="0.8">
<!-- Two detuned saw oscillators for thickness
<group name="Osc1" tags="osc1">
<oscillator waveform="saw" />
</group>
<group name="Osc2" tags="osc2">
<oscillator waveform="saw" />
</group>
</groups>
<effects>
<effect type="lowpass" frequency="2000.0" resonance="0.3"
envelope_amount="3000.0" envelope_attack="0.01"
envelope_decay="0.3" envelope_sustain="0.0" envelope_release="0.5"/>
</effects>
<modulators>
<!-- LFO vibrato -->
<lfo shape="sine" frequency="5.0" modAmount="0.15" scope="global">
<binding type="amp" level="group" tags="osc1,osc2" parameter="GROUP_TUNING"
modBehavior="add" translation="linear"
translationOutputMin="-0.5" translationOutputMax="0.5"/>
</lfo>
</modulators>
<!-- Use tag polyphony to enforce monophonic behavior -->
<tags>
<tag name="osc1" volume="1" pan="0" polyphony="1" />
<tag name="osc2" volume="1" pan="0" polyphony="1" />
</tags>
</DecentSampler>
```

This preset demonstrates:

• Portamento/Glide: Set via \`glideTime\` (0.3 seconds) and \`glideMode\` (legato - only glides between connected notes)

• Monophonic behavior: Achieved using tag polyphony limits (\`polyphony=”1”\`)

• Detuned oscillators: Two saw waves with slightly different tuning for a fatter sound

• Modulation: LFO vibrato applied to both oscillators via GROUP_TUNING binding

• Filter envelope: Adds movement to the timbre

## Combining Oscillators with Samples:

You can combine oscillators and samples within the same group or across separate groups. Here’s an example mixing them in a single group to add a sub-bass layer to piano samples:

```xml
<groups>
<!-- Piano samples with built-in sine wave sub-bass -->
<group name="Piano with Sub" volume="1.0">
<sample path="Samples/Piano/C3.wav" rootNote="60" loNote="59" hiNote="61" />
<sample path="Samples/Piano/D3.wav" rootNote="62" loNote="61" hiNote="63" />
<!-- more samples... ->
<!-- Oscillator mixed in the same group -->
<oscillator waveform="sine" />
</group>
</groups>
```

Or use separate groups for independent control:

<groups>   
<!-- Sample-based piano sound >   
<group name="Piano Samples" volume="1.0">

(continues on next page)

## 1.3. The <groups> element (required)

```xml
pag
<sample path="Samples/Piano/C3.wav" rootNote="60" loNote="59" hiNote="61" />
<!-- more samples... ->
</group>
<!-- Sine wave sub-bass layer >
<group name="Sub Layer" volume="0.3" groupTuning="-12" hiNote="60">
<oscillator waveform="sine" />
</group>
</groups>
```

## Physical Modeling with pluck1:

The \`pluck1\` waveform uses digital waveguide synthesis to simulate plucked string instruments. Unlike traditional oscillator waveforms that cycle continuously, pluck1 generates a transient excitation that decays naturally, making it ideal for realistic plucked string sounds:

```xml
<DecentSampler minVersion="1.15.0">
<ui width="812" height="375">
<tab name="main">
<labeled-knob x="10" y="20" width="90" label="Damping" type="float"
minValue="0.0" maxValue="1.0" value="0.5">
<binding type="general" level="group" position="0"
parameter="OSCILLATOR_DAMPING"/>
</labeled-knob>
<labeled-knob x="110" y="20" width="90" label="Pluck Type" type="float"
minValue="0.0" maxValue="1.0" value="0.5">
<binding type="general" level="group" position="0"
parameter="OSCILLATOR_PLUCK_TYPE"/>
</labeled-knob>
</tab>
</ui>
<groups attack="0.001" decay="0.1" sustain="0.8" release="0.2">
<group name="Plucked String" volume="0.8">
<oscillator waveform="pluck1" damping="0.5" pluckType="0.5" />
</group>
</groups>
</DecentSampler>
```

## In this example:

• \`damping=”0.5”\`: Moderate decay time - the string sustains for a reasonable duration

• \`pluckType=”0.5”\`: Balanced between smooth (triangle wave) and bright (noise burst) excitation

• OSCILLATOR_DAMPING and OSCILLATOR_PLUCK_TYPE binding parameters allow real-time control from the UI

The pluck1 oscillator is particularly effective for:

• Bass guitars: Use lower \`damping\` values (0.3-0.5) with higher \`pluckType\` (0.6-0.8) for percussive attack

• Acoustic guitars: Medium \`damping\` (0.5-0.7) with balanced \`pluckType\` (0.4-0.6)

• Harps and lyres: Higher \`damping\` (0.7-0.9) with lower \`pluckType\` (0.2-0.4) for softer, sustained tones

• Ethnic stringed instruments: Experiment with \`pluckType\` to match the excitation character of traditional instruments like koto, sitar, gayageum, etc. User-Defined Wavetable Oscillator:

The wavetable waveform lets you load any multi-frame .wav file as a wavetable. Each frame is a fixed-size window of samples that the oscillator cycles through at the correct pitch. Moving the position between frames transitions between different timbres, enabling classic wavetable scanning effects.

## Wavetable File Format:

• The file must be a standard .wav containing all frames concatenated in sequence (e.g., 128 frames × 2048 samples = 262144 samples total).

• If the file contains a clm RIFF chunk (Serum-compatible format), the frame size is detected automatically. Files exported from Serum or other compatible tools include this chunk.

• If no clm chunk is present, the frame size defaults to 2048 samples. You can override this by setting wavetableFrameSize on the <oscillator> element.

## Wavetable Scanning with a Knob:

```xml
<DecentSampler minVersion="1.15.0">
<ui width="812" height="375">
<tab name="main">
<labeled-knob x="10" y="20" width="90" label="Position" type="float"
minValue="0.0" maxValue="1.0" value="0.0">
<binding type="general" level="group" position="0"
parameter="OSCILLATOR_WAVETABLE_POSITION"
translation="linear"
translationOutputMin="0.0" translationOutputMax="1.0"/>
</labeled-knob>
</tab>
</ui>
<groups attack="0.01" decay="0.5" sustain="1.0" release="0.5">
<group name="Wavetable Oscillator">
<oscillator waveform="wavetable"
wavetableFile="Wavetables/MyWavetable.wav"
wavetablePosition="0.0"/>
</group>
</groups>
</DecentSampler>
```

## Wavetable Scanning with an LFO:

```xml
<DecentSampler minVersion="1.15.0">
<ui width="812" height="375">
<tab name="main">
<labeled-knob x="10" y="20" width="90" label="LFO Rate" type="float"
minValue="0.1" maxValue="10.0" value="1.0">
<binding type="modulator" level="instrument" modulatorIndex="0"
parameter="FREQUENCY"/>
</labeled-knob>
<labeled-knob x="110" y="20" width="90" label="LFO Depth" type="float"
minValue="0.0" maxValue="1.0" value="0.5">
<binding type="modulator" level="instrument" modulatorIndex="0"
parameter="MOD_AMOUNT"/>
</labeled-knob>
```

(continued fror

```xml
</tab>
</ui>
<groups attack="0.01" decay="0.5" sustain="1.0" release="0.5">
<group name="Wavetable Oscillator" wavetablePosition="0.5">
<oscillator waveform="wavetable"
wavetableFile="Wavetables/MyWavetable.wav"/>
</group>
</groups>
<modulators>
<lfo shape="sine" frequency="1.0" modAmount="0.5" scope="voice">
<binding type="general" level="group" position="0"
parameter="OSCILLATOR_WAVETABLE_POSITION"
modBehavior="add"
translationOutputMin="0.0" translationOutputMax="0.5"/>
</lfo>
</modulators>
</DecentSampler>
```

In this example, the LFO continuously sweeps the wavetable position around a center value of 0.5, scanning through timbres at the LFO rate. The modBehavior="add" means the LFO value is added on top of the static wavetablePosition set on the group.

## Selecting the active wavetable from a menu:

Because wavetableFile cannot be changed at runtime via a binding, the way to offer a wavetable selector is to define one <group> per wavetable (each with a unique tag) and use a <menu> with TAG_ENABLED bindings to switch between them:

```xml
<ui width="812" height="375">
<tab name="main">
<menu x="20" y="68" width="220" height="26" value="1">
<option name="Growl 01">
<binding type="amp" level="tag" identifier="growl" parameter="TAG_ENABLED


translation="fixed_value" translationValue="true"/>
<binding type="amp" level="tag" identifier="fm" parameter="TAG_ENABLED
translation="fixed_value" translationValue="false"/>
</option>
<option name="FM 01">
<binding type="amp" level="tag" identifier="growl" parameter="TAG_ENABLED

translation="fixed_value" translationValue="false"/>
<binding type="amp" level="tag" identifier="fm" parameter="TAG_ENABLED

translation="fixed_value" translationValue="true"/>
</option>
</menu>
</tab>
</ui>
```

(continues on next page)

(continued from previous page (continued from previous page)

<groups>   
<group tags="growl">   
<oscillator waveform="wavetable" wavetableFile="Wavetables/Growl/Growl 01.wav"␣   
randomPhase="true"/>   
</group>   
<group tags="fm" enabled="false">   
<oscillator waveform="wavetable" wavetableFile="Wavetables/FM/FM 01.wav"␣   
randomPhase="true"/>   
</group>   
</groups>

Each menu option enables exactly one tag and disables all others. The initial enabled="false" on non-default groups matches the menu’s default selection (value=”1” = first option).

## Tips for Wavetable Sounds:

• Use Serum-format wavetables (exported from Serum or compatible tools) for automatic frame size detection.

• A knob scanning the position gives manual timbral control, while an LFO creates evolving, animated textures.

• Combine with envelope modulation on OSCILLATOR_WAVETABLE_POSITION for timbral sweeps triggered by note velocity or the amplitude envelope.

• Pair with groupTuning detuning and panning for unison-style / supersaw-like layering.

• Always set randomPhase="true" when layering multiple wavetable groups. Without it, all voices start at phase zero and cancel each other. UI Control for Oscillators:

You can create UI controls to switch waveforms dynamically:

```xml
<ui width="812" height="375">
<tab name="main">
<menu x="10" y="70" width="150" height="30" value="1">
<option name="Sine">
<binding type="general" level="group" position="0"
parameter="OSCILLATOR_WAVEFORM"
translation="fixed_value" translationValue="sine"/>
</option>
<option name="Saw">
<binding type="general" level="group" position="0"
parameter="OSCILLATOR_WAVEFORM"
translation="fixed_value" translationValue="saw"/>
</option>
<option name="Square">
<binding type="general" level="group" position="0"
parameter="OSCILLATOR_WAVEFORM"
translation="fixed_value" translationValue="square"/>
</option>
</menu>
</tab>
</ui>
```

## Tips and Best Practices:

1. Layer multiple waveforms: Combine different waveform types at different octaves for complexity.

2. Use detuning: Create stereo width by layering slightly detuned oscillators panned left and right.

3. Limit note ranges: Use \`loNote\` and \`hiNote\` to restrict certain oscillators to specific registers (e.g., sub-bass only on low notes).

4. Add envelope variation: Use different ADSR settings on each oscillator group for evolving textures.

5. Monophonic leads: Use tag polyphony (\`polyphony=”1”\`) combined with \`glideMode=”legato”\` for classic synthesizer lead sounds.

6. Modulation: Apply LFOs and envelopes to \`GROUP_TUNING\` for vibrato, tremolo, and evolving textures.

## Feature Parity with Samples:

As of DecentSampler 1.15.0, oscillators support most sample features including:

• Pitch key tracking (pitchKeyTrack) - Control how oscillators respond to keyboard pitch

• Portamento/Glide (glideTime, glideMode) - Smooth pitch transitions between notes

• Modulation bindings - Full support for GROUP_TUNING, GLOBAL_TUNING, and GROUP_VOLUME modulation

• Tag-based polyphony - Use tags with polyphony limits for monophonic/polyphonic control

• ADSR envelopes - Complete amplitude envelope control with curve shaping

• Effects chains - Group-level and global effects apply to oscillators

• Velocity tracking - ampVelTrack for velocity-sensitive volume

• Note range filtering - loNote/hiNote for keyboard zones

## Limitations:

• Oscillators do not support looping parameters (\`loopStart\`, \`loopEnd\`, etc.) since they generate continuous waveforms.

• Round-robin sequencing (\`seqMode\`, \`seqPosition\`) is not applicable to oscillators.

• Sample-specific features like \`start\`/\`end\` positions and embedded loop markers don’t apply.

## Version Requirements:

Oscillators were introduced in DecentSampler version 1.15.0. All oscillator features including portamento and modulation support require version 1.15.0 or higher. Set the appropriate \`minVersion\` attribute:

<DecentSampler minVersion="1.15.0">

## 1.4 The <effects> element

Adding global, instrument-wide effects is easy: just add an <effects> element right below your top-level <DecentSampler> element.

It is also possible to have effects that only get added to a specific group. To adding effects that only apply to a specific group, all you need to do is create an <effects> group that lives underneath the <group> element for the group you want to affect.

Group level effects are initialized every time a note is started and destroyed every time a note is stopped. If you play two notes simultaneously, two instances of this effect will be created and these will be independent of eachother. As a result, they use more CPU than global effects.

NOTE: Only certain effects will work as group-level effects: lowpass filter, hipass filter, bandpass filter, gain, and chorus. Delay and reverb cannot work properly as they will be deleted before their tail peters out.

## 1.4.1 The <effect> element

Within the <effects> element, you can have any number of <effect> sub-elements. These specify parameters for each individual effect that you would like to have in your global effects chain. There are currently only a handful effects available although more could definitely be added on request:

All <effect> elements support a tags attribute — a comma-separated list of tag names. Tags allow bindings to target effects by name instead of by numeric index. For example:

```xml
<effects>
<effect type="lowpass" frequency="22000" tags="main-filter" />
</effects>
```

A binding can then reference this effect using tags="main-filter" instead of effectIndex="0":

<binding type="effect" level="instrument" tags="main-filter" parameter="FX_FILTER_   
FREQUENCY"   
translation="linear" translationOutputMin="100" translationOutputMax="8000" />

Low-pass, Band pass, and Hi-pass filter

A 2-pole resonant filter that can be either a lowpass, bandpass, or highpass filter

Example:

```xml
<DecentSampler>
<effects>
<effect type="lowpass" resonance="0.7" frequency="22000" />
</effects>
</DecentSampler>
```

There is also a single pole version of the lowpass filter that can be accessed using the lowpass_1pl effect type. This version does not have a resonance parameter.

```xml
<DecentSampler>
<effects>
<effect type="lowpass_1pl" frequency="22000" />
</effects>
</DecentSampler>
```

Attributes:


| Attribute | Type |  | Valid Range | Default |
| --- | --- | --- | --- | --- |
| type | Re- quired | The type of filter | Must be either lowpass (legacy: lowpass_4pl), lowpass_1pl, bandpass, or highpass |  |
| resonanc | Op- tional | The filter reso- nance (Q) | 0.001 - 5.0, where 5 is big, 0 is small. | 0.7 |
| frequenc | Op- tional | The filter fre- quency | 0 - 22000 | 22000 |



## Notch EQ Filter

A simple notch filter.

Example:

```xml
<DecentSampler>
<effects>
<effect type="notch" q="0.7" frequency="22000" />
</effects>
</DecentSampler>
```

## Attributes:


| Attribute | Type |  | Valid Range | Default |
| --- | --- | --- | --- | --- |
| type | Required The type of filter |  | Must be notch |  |
| frequency Required The filter frequency |  |  | 60 - 22000.0 | 10000 |
| Q | Optional | Q is the ratio of center frequency to bandwidth 0.01 - 18.0 |  | 0.7 |



## Peak EQ Filter

A peak filter centred around a given frequency, with a variable Q and gain.

Example:

```xml
<DecentSampler>
<effects>
<effect type="peak" q="0.7" frequency="22000" gain="2.0" />
</effects>
</DecentSampler>
```

Attributes:


| Attribute | Type | Valid Range | Default |
| --- | --- | --- | --- |
| type | Re- quired | The type of fi lter | Must be peak |
| frequenc | Re- quired | The filter frequency | 60 22000.0 |
| Q | Op- tional | Q is the ratio of center frequency to bandwidth | 0.01 18.0 |
| gain | Re- quired | Values greater than 1.0 will boost the high frequencies, values less than 1.0 will attenuate them. | 0 - 1.0 |



## Gain effect

Applies a volume boost or cut to the output signal.

Example:

```xml
<DecentSampler>
<effects>
<effect type="gain" level="-6" />
</effects>
</DecentSampler>
```

Attributes:


| At- tribui | Type | Default | Valid Range |
| --- | --- | --- | --- |
| type Re- quire | Must be gain |  | gain dB: -99 |
| leve Re- quire | The amount of gain to apply. By default (and for backwards compatibility) this is ex- pressed in decibels (e.g. -6 reduces the signal by ~50%). If levelUnit="1inear" is also set, this value is treated as a linear multiplier (e.g. 0.5 reduces the signal by 50%, 1.0 is unity gain, 2.0 doubles it). | о | 24; linear: 0.0 8.0 |
| leve Op- tional | Specifies the unit for the level attribute. Valid values are decibels (default, for backwards compatibility) and linear. When set to linear, the level attribute is treated as a linear multiplier rather than a dB value. Note: this atribute must appear before level in the XML element. |  | deci decibels linear |



## Reverb effect

Example:

<DecentSampler>   
<effects>   
<effect type="reverb" roomSize="" damping="" wetLevel="" />   
</effects>   
</DecentSampler>

Attributes:


| Attribute | Type |  | Valid Range | Default |
| --- | --- | --- | --- | --- |
| type | Re- quired | Must be reverb | reverb |  |
| roomSize | Op- tional | The reverb "room size" | 0 - 1.0, where 1.0 is big, 0 is small. | 0.7 |
| damping | Op- tional | The reverb damping level | 0 - 1.0, where 0 is not damped, 1.0 is fully damped. | 0.3 |
| wetLevel | Op- tional | The volume of reverb sig- nal | 0 - 1.0 | 0 |



## Delay effect

A simple delay effect that can be controlled either in seconds or using musical time increments based on the host tempo.   
For a complete explanation of how to use tempo-syncing, see here.

## Attributes:


| At- tribui | Type |  | Valid Range | Default |
| --- | --- | --- | --- | --- |
| type | Re- quire Op- | Must be delay | delay | seconds seco |
| dela | tiona | Determines whether the delay will be synced to DAW tempo or not, as well as what format you will be using for the delayTime parameter. There are two possible values: 1) seconds, which is the default, means that delayTime will be specified in seconds and will not change when the DAW tempo changes; 2) musical_time means that delay time will be specified using an integer value generated by a control which is setup to use the musical_time valueType parameter. In order for this to work, you | musical |  |
|  | dela Op- tiona | will need to be using the plug-in within a DAW that provides a tempo to the plug-in. The delay time in seconds | 0- 20.0 | 0.7 |
| feed | Op- tiona | The feedback level. | 0 - 1.0, where 0 is no feed- back, 1.0 is max | 0.2 |
| ster Op- ' | tiona | The parameter allows you to introduce delay variations between the left and right channels. Half of this amount is subtracted from the left channel's delay time and half of this amount is added to the right channel's delay time. For example, if the delayTime is 0.5 seconds and the stereo0ffset is 0.02 s, then the actual left chan- | feed- back. -10 10 | 0 |
| wetL tiona | Op- | nel delay time will be 0.49s and the actual right channel delay time will be 0.51s so that the two channels are of set by 0.02 seconds. The volume of the delay signal | 0 - 1.0 | 0.5 |



Example of using the delay effect when specifying time in seconds:

```xml
<DecentSampler>
<effects>
<effect type="delay" delayTime="0.5" stereoOffset="0.01" feedback="0.2" wetLevel="0.5
/>
</effects>
</DecentSampler>
```

Example of using the delay effect when specifying time in musical time:

<DecentSampler>   
<ui>   
<tab>   
<labeled-knob x="180" y="40" label="Delay Time" valueType="musical_time"   
(continues on next page)

```xml
minValue="0" maxValue="20" value="10" defaultValue="10">
<binding type="effect" level="instrument" position="0" parameter="FX_DELAY_TIME"␣
/>
</labeled-knob>
</tab>
</ui>
<effects>
<effect type="delay" delayTimeFormat="musical_time" delayTime="10" stereoOffset="0.01
feedback="0.3" wetLevel="0.5" />
</effects>
</DecentSampler>
```

## Chorus effect

Example:

<DecentSampler>   
<effects>   
<effect type="chorus" mix="0.5" modDepth="0.2" modRate="0.2"/>   
</effects>   
</DecentSampler>

## Attributes:


| Attribute | Type | Valid Range | Default |
| --- | --- | --- | --- |
| type | Re- quired | Must be chorus | chorus |
| mix | Op- tional | The wet/dry mix which controls how much of the chorus signal we hear | 0 - 1.0, where 1.0 is just chorus, 0 is just dry signal. |
| modDep1 | Op- tional | The modulation depth of the eff ect | 0 - 1.0, where 0 is no modulation, 1.0 is max modulation. |
| modRat | Op- tional | The modulation speed in Hz. | 0 - 10.0 |



## Phaser effect

Example:

```xml
<DecentSampler>
<effects>
<effect type="phaser" mix="0.5" modDepth="0.2" modRate="0.2" centerFrequency="400"␣
feedback="0.7" />
</effects>
</DecentSampler>
```

Attributes:


| Attribute | Type | Valid Range | Default |
| --- | --- | --- | --- |
| type | Re- quired | Must be phaser | phaser |
| mix | Op- tional | The wet/dry mix which controls how much of the phaser signal we hear | 0 - 1.0, where 1.0 is just phaser, 0 is just dry signal. |
| modDepth | Op- tional | The modulation depth of the eff ect | 0 - 1.0, where 0 is no modulation, 1.0 is max modulation. |
| modRate | Op- tional | The modulation speed in Hz. | 0 - 10.0 |
| centerFreqi | Op- tional | The center frequency (in Hz) of the phaser all-pass filters modulation | 0- 22000 |
| feedback | Op- tional | Sets the feedback volume of the phaser. | -1 - 1.0 |



## Convolution effect

This effect allows you to use a convolution reverb or amp simulation to your sample library. Depending on the length of the impulse response, the convolution effect can use substantial CPU, so you’ll definitely want to do some testing both with and without the convolution effect turned on.

Example:

<DecentSampler>   
<effects>   
<effect type="convolution" mix="0.5" irFile="Samples/Hall 3.wav" />   
</effects>   
</DecentSampler>

Attributes:


| Attribute | Type | Valid Range | Default |
| --- | --- | --- | --- |
| type | Re- quired | Must be convolution | convolution |
| mix | Op- tional | The wet/dry mix controls how much of the con- volution signal we hear | 0 - 1.0, where 1.0 is just convolution, 0 is just dry signal. |
| irFile | Re- quired | The path of the WAV or AIFF to use as an Im- pulse Response (IR) file | String |



## Pitch Shifter effect

As of version 1.13.3, Decent Sampler contains an old-school pitch shifter effect, which allows you to shift the pitch of the audio signal up or down by a specified number of semitones. This can be useful for creating harmonies or for adding subtle chorus effects.

Attributes:


| Attribute | Type | Valid Range | Default |
| --- | --- | --- | --- |
| type | Re- quired | Must be pitch_shift | pitch_shift |
| pitchS] | Op- tional | The number of semitones to shift the pitch of the audio signal | -24 - 24, where -24 means two octaves down, 0 means no pitch shift, and 24 means two octaves up |
| mix | Op- | The wet/dry mix controls how much tional of the pitch-shifted signal we hear | 0 - 1.0, where 1.0 is just pitch-shifted signal, 0 is just dry signal. |



Example:

```xml
<DecentSampler>
<effects>
<effect type="pitch_shift" pitchShift="2" mix="0.5" />
</effects>
</DecentSampler>
```

## Wave Folder effect

Introduced in version 1.7.2. This effect allows you to fold a waveform back on itself. This is very useful for generating additional harmonic content.

Attributes:


| Attribute | Type | Valid Range | Default |
| --- | --- | --- | --- |
| type | Re- quired | Must be wave_folder | wave_folder |
| drive | Op- tional | The volume of the input signal | 1 - 100, where 100 means the signal is amplified by a factor of 100 and 1 means no amplification is applied |
| thresh | Op- tional | The amplitude above which wave folding should take place | 0 - 10.0 |



Because wave folding tends to sound better when applied on a per-voice basis, it usually makes sense to set up the wave folder at the group level (separate group effects get created for each keypress). Example:

```xml
<DecentSampler pluginVersion="1">
<ui>
<tab>
<labeled-knob x="180" y="40" label="Drive" type="float" minValue="1" maxValue="100
textColor="FF000000" value="1">
<binding type="effect" level="group" groupIndex="0" effectIndex="0" parameter=
"FX_DRIVE" translation="linear" />
</labeled-knob>
<labeled-knob x="280" y="40" label="Threshold" type="float" minValue="0" maxValue=
1  value="1" textColor="FF000000">
<binding type="effect" level="group" groupIndex="0" effectIndex="0" parameter=
"FX_THRESHOLD" translation="linear" />
</labeled-knob>
</tab>
```

</ui>   
<groups>   
<group>   
<!- Samples go here. >   
<effects>   
<effect type="wave_folder" drive="1" threshold="1" />   
</effects>   
</group>   
</groups>

## Wave Shaper effect

Introduced in version 1.7.2. This effect allows you to distort an audio signal. This is very useful for generating additional harmonic content.

## Attributes:


| Attribute | Type | Valid Range |  | Default |
| --- | --- | --- | --- | --- |
| type | Re- quire | Must be wave_shaper | wave_shaper 1 - 1000, where 1000 means the signal | 1 |
| drive | Op- | The amount of distortion. This really just controls tiona the volume of the input signal. The volume of the is amplified by a factor of 1000 and 1 input signal | means no amplification is applied |  |
| drive | Op- tiona | Introduces an extra gain boost to the drive | 0 - 1.0 | 1 |
| outpu | Op- tiona | The linear output level of the signal | 0 - 1.0 | 0.1 |
| highQ Op- | tionai s | Whether or not oversampling is performed. Over- sampling sounds better, but it's CPU intensive. If you want to save CPU, set this to false. | true, false | false |



Because wave shaping tends to sound better when applied on a per-voice basis, it usually makes sense to set up the wave shaper at the group level (separate group effects get created for each keypress). Example:

```xml
<DecentSampler pluginVersion="1">
<ui>
<tab>
<labeled-knob x="180" y="40" label="Drive" type="float" minValue="1" maxValue="40"
textColor="FF000000" value="0.5473124980926514">
<binding type="effect" level="group" groupIndex="0" effectIndex="0" parameter=
"FX_DRIVE" translation="linear"/>
</labeled-knob>
<labeled-knob x="280" y="40" label="Drive Boost" type="float" minValue="0"␣
maxValue="1" value="0.328312486410141" textColor="FF000000">
<binding type="effect" level="group" groupIndex="0" effectIndex="0" parameter=
"FX_DRIVE_BOOST" translation="linear"/>
</labeled-knob>
<labeled-knob x="380" y="40" label="Output Lvl" type="float" minValue="0" maxValue=
"1" value="0.328312486410141" textColor="FF000000">
```

(continues on next page)

```xml
<binding type="effect" level="group" groupIndex="0" effectIndex="0" parameter=
"FX_OUTPUT_LEVEL" translation="linear"/>
</labeled-knob>
</tab>
</ui>
<groups>
<group>
<!-- Samples go here. -->
<effects>
<effect type="wave_shaper" drive="0.5473124980926514" shape="0.328312486410141"
outputLevel="0.1"/>
</effects>
</group>
</groups>
```

## Stereo Simulator effect

Introduced in version 1.17.0. This effect converts a mono input signal into a pseudo-stereo signal using one of three classic algorithms.

```xml
<effect type="stereo_simulator"
algorithm="adt"
width="0.5"
delayTime="0.005"
modRate="0.5"
modDepth="0.3" />
```

## Attributes:


| Attribute | Type |  | Valid Range | Default |
| --- | --- | --- | --- | --- |
| type | Re- quired | Must be stereo_simulator | stereo_simulator |  |
| algorith | Op- tional | Pseudo-stereo algorithm to use | lauridsen, schroeder, adt | adt |
| width | Op- tional | Stereo spread and dry/wet amount; 0 = mono/dry, 1 = full stereo eff ect | 0-1 | 0.5 |
| delayTim | Op- tional | Delay time in seconds | 0.001–0.030 | 0.005 |
| modRate | Op- tional | LFO modulation rate in Hz (ADT algorithm only) | 0.1-10.0 | 0.5 |
| modDepth | Op- tional | LFO modulation depth (ADT algorithm only) | 0-1 | 0.3 |



The three algorithms are:

• lauridsen: Complementary comb filters — simple and CPU-efficient.

• schroeder: Double-delay comb filters — slightly richer stereo image.

• adt (default): Artificial Double Tracking with LFO-modulated delay — the most convincing stereo widening effect. The modRate and modDepth parameters only affect this algorithm.

The width parameter serves as both the stereo spread amount and the dry/wet control. Setting width to 0 produces a mono dry signal; setting it to 1 applies the full stereo effect.

All parameters except algorithm are bindable. Example with a width knob:

```xml
<DecentSampler pluginVersion="1">
<ui>
<tab>
<labeled-knob x="100" y="40" label="Width" type="float" minValue="0" maxValue="1"␣
value="0.5" textColor="FF000000">
<binding type="effect" level="instrument" effectIndex="0" parameter="FX_WIDTH"␣
translation="linear" translationOutputMin="0" translationOutputMax="1"/>
</labeled-knob>
</tab>
</ui>
<groups>
<group>
<!-- Samples go here. >
</group>
</groups>
<effects>
<effect type="stereo_simulator" algorithm="adt" width="0.5" delayTime="0.005"␣
modRate="0.5" modDepth="0.3" />
</effects>
</DecentSampler>
```

## Bit Crusher effect

The bit crusher reduces the bit depth and sample rate of the audio signal, producing a characteristic lo-fi, digital, crunchy sound. Bit depth reduction introduces quantisation noise, while sample rate reduction creates aliasing artifacts.

```html
<effect type="bit_crusher" bitDepth="8" sampleRateReduction="4" mix="1.0"/>
```

Attributes:


| Attribute | Type |  | Valid Range | Default |
| --- | --- | --- | --- | --- |
| type | Re- quirec | Must be bit_crusher | bit_crusher |  |
| bitDepth | Op- | The number of bits used to quantise the audio signal. tional Lower values = more aggressive crushing. | 1–24, where 24 is clean and 1 is maximum crushing. | 24 |
| sampleRate | e Op- | Sample-rate reduction factor. A value of 4 means the tional effective sample rate is reduced to one quarter of the original. | 1–32, where 1 is no reduction and 32 is maximum downsam- pling. | 1 |
| mix | Op- | The wet/dry mix which controls how much of the bit- tional crushed signal we hear. | 0−1.0, where 1.0 is fully crushed and 0.0 is the original dry signal. | 1.0 |



Both bitDepth and sampleRateReduction are bindable, so you can attach knobs to control them in real time. Binding parameters for the bit crusher effect:


| Binding parameter value | Description |
| --- | --- |
| FX_BIT_DEPTH | Controls the bit depth (1–24) |
| FX_SAMPLE_RATE_REDUCTION | Controls the sample-rate reduction factor (1–32) |
| FX_MIX | Controls the wet/dry mix (0–1) |



Example with knobs for all three parameters:

```xml
<DecentSampler pluginVersion="1">
<ui>
<tab>
<labeled-knob x="80" y="40" label="Bit Depth" type="float"
minValue="1" maxValue="24" value="24" textColor="FF000000">
<binding type="effect" level="instrument" effectIndex="0"
parameter="FX_BIT_DEPTH" translation="linear"
translationOutputMin="1" translationOutputMax="24"/>
</labeled-knob>
<labeled-knob x="185" y="40" label="Rate Reduction" type="float"
minValue="1" maxValue="32" value="1" textColor="FF000000">
<binding type="effect" level="instrument" effectIndex="0"
parameter="FX_SAMPLE_RATE_REDUCTION" translation="linear"
translationOutputMin="1" translationOutputMax="32"/>
</labeled-knob>
<labeled-knob x="290" y="40" label="Mix" type="float"
minValue="0" maxValue="1" value="1" textColor="FF000000">
<binding type="effect" level="instrument" effectIndex="0"
parameter="FX_MIX" translation="linear"
translationOutputMin="0" translationOutputMax="1"/>
</labeled-knob>
</tab>
</ui>
<groups>
<group>
<!-- Samples go here. -->
</group>
</groups>
<effects>
<effect type="bit_crusher" bitDepth="24" sampleRateReduction="1" mix="1.0"/>
</effects>
</DecentSampler>
```

## 1.5 The <midi> element

MIDI mappings can be added to your instrument by adding a <midi> element right below your top-level <DecentSampler> element.

## 1.5.1 The <cc> element

Within the <midi> element, you can have any number of <cc> elements. These allow you to map changes in incoming continuous controller messages to specific parameters of your instrument. To use this functionality, you’ll want to add a separate <cc> element for each CC number you would like to respond to. The <cc> element has a single required attribute number="" which specifies the number (from 0 to 127) of the continuous controller you would like to listen on. Beneath the <cc> element, you can have any number of bindings.

```html
<midi>
<cc number="11">
<binding level="ui" type="control" position="0" parameter="VALUE" translation="linear

translationOutputMin="0" translationOutputMax="1"/>
</cc>
<cc number="1">
<binding level="ui" type="control" position="1" parameter="VALUE" translation="linear

translationOutputMin="0" translationOutputMax="1"/>
</cc>
</midi>
```

## 1.5.2 The <note> element

Within the <midi> element, you can have any number of <note> elements. These allow you to map specific notes to specific parameters of your instrument. To use this functionality, you’ll want to add a separate <note> element for each MIDI note or range of notes you would like to respond to.

Here are the attributes of the <note /> element:

• note (required): This attribute specifies the MIDI note number (from 0 to 127) you would like to listen on. You can also specify ranges of notes by using a dash. For example note="24-35" would be used to specify bindings for the range of notes 24 thorugh 35.

• eventType (optional): This attribute specifies the type of event to listen for. The default is note_on, but you can also specify note_off or any. The default is any if this attribute is not specified.

• enabled (optional): A true/false value that specifies whether this note listener is turned on.

swallowNotes (optional): The bindings that live below this note listener are called before any notes are played. By default, swallowNotes is false, which means that the keypress will then be received by the sampler. If swallowNotes is true, the sampler will not receive the note. This is useful if you wish to prevent certain keys from triggers notes.

It is possible to enable and disable a note listener by targeting the enabled attribute.

Beneath the <note> element, you can have any number of bindings. Here is an example of how keyswitches might be set up:

```xml
<midi>
<note note="11" enabled="true" eventType="note_on">
<binding enabled="true" type="general" level="group" groupIndex="0" parameter=
"ENABLED" translation="fixed_value" translationValue="true" />
<binding enabled="true" type="general" level="group" groupIndex="1" parameter=
"ENABLED" translation="fixed_value" translationValue="false" />
</note>
<note note="12" enabled="true" eventType="note_on">
<binding enabled="true" type="general" level="group" groupIndex="0" parameter=
"ENABLED" translation="fixed_value" translationValue="false" />
<binding enabled="true" type="general" level="group" groupIndex="1" parameter=
"ENABLED" translation="fixed_value" translationValue="true" />
</note>
</midi>
```

In the above keyswitch example, MIDI note 11 turns on group 0 and turns off group 1, whereas MIDI note 12 does the opposite. Note the use of the fixed_value translation type.

## 1.5.3 The <velocity> element

Within the <midi> element, you can also have a <velocity> element. This element allows you to control an instrument in response to MIDI velocity messages. This is useful for creating dynamic responses based on how hard a note is played.

Example usage:

```xml
<midi>
<velocity>
<binding modAmount="0.3" level="group" parameter="FX_FILTER_FREQUENCY"
groupIndex="0" effectIndex="0" type="effect"/>
</velocity>
</midi>
```

In this example, the <velocity> element contains a single <binding> that modifies the FX_FILTER_FREQUENCY parameter of the first effect (effectIndex: 0) in the first group (groupIndex: 0) based on the velocity of incoming MIDI notes. The modAmount attribute specifies how much the velocity will affect the parameter.

## Bindings within the <midi> section

The bindings that the <cc>, <note>, and <velocity> element listens on are the same as those used by the UI controls.   
See Appendix B for a complete description of these.

If you have a UI control mapped to the same internal parameter as a MIDI mapping, you’ll want to have your MIDI mapping control the UI control instead of the parameter directly. The benefit of doing this is that, as the MIDI CC input is received, the UI control will be updated as well as the desired internal parameter.

The way to accomplish this is to make use of the labeled_knob or control binding types (control was introduced in version 1.1.7) as follows:

```xml
<binding level="ui" type="control" position="0" parameter="VALUE" translation="linear"
translationOutputMin="0" translationOutputMax="1"/>
```

You’ll notice that the control type has a level value of ui and a parameter value of VALUE. Another thing to notice is the position="" parameter. This contains the 0-based index of the control to be modified. NOTE: The indexes of

the parameter list includes all UI controls, including <label> and menu controls, so you’ll want to account for that when calculating your positions.

An example of changing a menu option based on a MIDI note (keyswitch) would look like this:

```html
<midi>
<note note="11" eventType="note_on">
<binding type="control" level="ui" position="1" parameter="VALUE" translation="fixed
value" translationValue="1" />
</note>
<note note="12" eventType="note_on">
<binding type="control" level="ui" position="1" parameter="VALUE" translation="fixed
value" translationValue="2" />
</note>
</midi>
```

## 1.6 The <noteSequences> element

As of 1.11.1, DecentSampler has a built-in note sequencer. It can be mapped to keys so that clusters of notes are played every time certain keys are pressed. It can also be mapped to UI controls such as buttons.

The <noteSequences> element is how you specify note sequences that can be used by this playback engine. There should be exactly one <noteSequences> element in each <DecentSampler> file.

The <noteSequences> element can contain one or more <sequence> elements:

## 1.6.1 The <sequence> element

The <sequence> element has the following attributes:

• name (optional): An optional descriptive name for the sequence. This is only used in the sample editor UI to help you identify the sequence.

• length (required): The length of the sequence in beats. This is a floating point number.

• rate (optional): The rate at which the sequence is played. This is a floating point number. The default is 1.0.

The <sequence> element can contain one or more <note> elements:

## The <note> element

The <note> element has the following attributes:

• position (required): The position of the note in the sequence, in beats. This is a whole number.

• velocity (required): The velocity of the note. This is a floating point number between 0 and 1.

• note (required): The MIDI note number of the note.

• length (required): The length of the note in beats. This is a whole number.

This is an an example showing the strum from an Omnichord:

```xml
<noteSequences>
<sequence name="Maj1Slow" length="768.0" rate="96">
<note position="0" velocity="1" note="48" length="768"/>
<note position="11" velocity="1" note="52" length="757"/>
<note position="29" velocity="1" note="55" length="739"/>
<note position="46" velocity="1" note="60" length="722"/>
<note position="63" velocity="1" note="64" length="705"/>
<note position="81" velocity="1" note="67" length="687"/>
<note position="99" velocity="1" note="72" length="669"/>
<note position="117" velocity="1" note="76" length="651"/>
<note position="134" velocity="1" note="79" length="634"/>
<note position="153" velocity="1" note="84" length="615"/>
<note position="171" velocity="1" note="88" length="597"/>
<note position="192" velocity="1" note="91" length="576"/>
</sequence>
</noteSequences>
```

For a full discussion of how to use note sequences, see the tutorial on how to use note sequences.

## 1.7 The <modulators> element

Version 1.6.0 of Decent Sampler officially introduces the new <modulators> section into the .dspreset format. This section lives below the top-level <DecentSampler> element and it is where all modulators for the entire sample library live.

<DecentSampler>   
<modulators>   
<!-- Your modulators go here. -->   
</modulators>   
</DecentSampler>

## 1.7.1 The <lfo> element

Underneath the <modulators> section, you can have any number of different LFOs, which are defined using an <lfo> element, for example:

```xml
<modulators>
<lfo shape="sine" frequency="2" modAmount="1.0"></lfo>
</modulators>
```

For an LFO with a 500ms delay before starting:

```xml
<modulators>
<lfo shape="sine" frequency="2" modAmount="1.0" delayTime="0.500"></lfo>
</modulators>
```

For a tempo-synced LFO using musical subdivisions:

```xml
<modulators>
<lfo shape="sine" frequencyFormat="musical_time" frequency="10" modAmount="1.0"></lfo>
</modulators>
```

## 1.7. The <modulators> element

In this mode, frequency is not Hz. It is an integer index into the same musical subdivision list used by valueType="musical_time" controls (for example, 10 = 1/8, 13 = 1/4, 19 = 1, 24 = 5/1).

This element has the following attributes:

• shape: controls the oscillator shape. Possible values are sine, square, saw.

• frequency: The speed of the LFO. If frequencyFormat is hz, this is in cycles per second. If frequencyFormat is musical_time, this is a musical subdivision index.

• frequencyFormat: How frequency is interpreted. Valid values are hz (default) and musical_time.

• modAmount: This value between 0 and 1 controls how much the modulation affects the things it is targeting. In conventional terms, this is like the modulation depth. Default value: 1.0.

• delayTime: The time in seconds to wait before the LFO starts outputting signal. During this delay period, the LFO outputs zero. Default value: 0.0 (no delay).

• scope: Whether or not this LFO exists for all notes or whether each keypress gets its own LFO. Possible values are global (default for LFOs) and voice. If voice is chosen, a new LFO is started each time a new note is pressed.

• modBehavior: This attribute controls how the LFO affects the parameter it is targeting. Possible values are add, modulate, multiply, and set. If add is chosen, the LFO will add its translated value directly to the target parameter (legacy behavior). If modulate is chosen, the LFO adds a zero-centered modulation delta around the target parameter’s current/base value, so a neutral LFO value contributes no offset. If multiply is chosen, the LFO will multiply its value by the parameter it is targeting. If set is chosen, the LFO will set the parameter it is targeting to its value. Default value: set.

## 1.7.2 The <envelope> element

In addition to LFOs, you can also have additional ADSR envelopes. These can be useful for controlling group-level effects, such as low-pass filters. If this is what you wish to achieve, make sure you check out the section on group-level effects below.

To create an envelope, use an <envelope> element:

This element has the following attributes:

• attack: The length in seconds of the attack portion of the ADSR envelope

• decay: The length in seconds of the decay portion of the ADSR envelope

• sustain: The height of the sustain portion of the ADSR envelope. This is expressed as a value between 0 and 1.

• release: The length in seconds of the release portion of the ADSR envelope

• modAmount: This value between 0 and 1 controls how much the modulation affects the things it is targeting. In conventional terms, this is like the modulation depth. Default value: 1.0.

• scope: Whether or not this LFO exists for all notes or whether each keypress gets its own LFO. Possible values are global and voice (default for envelopes). If voice is chosen, a new LFO is started each time a new note is pressed.

• modBehavior: This attribute controls how the envelope affects the parameter it is targeting. Possible values are add, modulate, multiply, and set. If add is chosen, the envelope will add its translated value directly to the target parameter (legacy behavior). If modulate is chosen, the envelope adds a zero-centered modulation delta around the target parameter’s current/base value, so a neutral envelope value contributes no offset. If multiply is chosen, the envelope will multiply its value by the parameter it is targeting. If set is chosen, the envelope will set the parameter it is targeting to its value. Default value: set.

• attackCurve: A numeric value from -100 to 100 that determines the shape of the attack portion of the ADSR envelope. Common values are -100 (logarithmic), 0 (linear), and 100 (exponential). Default value: -100 (logarithmic).

• decayCurve: A numeric value from -100 to 100 that determines the shape of the decay portion of the ADSR envelope. Common values are -100 (logarithmic), 0 (linear), and 100 (exponential). Default value: 100 (exponential).

• releaseCurve: A numeric value from -100 to 100 that determines the shape of the release portion of the ADSR envelope. Common values are -100 (logarithmic), 0 (linear), and 100 (exponential). Default value: 100 (exponential).

## 1.7.3 The <midiCC> element

The <midiCC> element allows you to use MIDI Continuous Controller (CC) messages as modulation sources. Thi makes it possible to control various parameters of your instrument in response to incoming MIDI CC data, such as the mod wheel, expression pedal, or any other MIDI controller.

```xml
<modulators>
<midiCC number="1 " modAmount="1.0" channel="voice">
<!-- Bindings go here -->
</midiCC>
</modulators>
```

This element has the following attributes:

• number: The MIDI CC number to respond to (0-127). Common CC numbers include 1 (mod wheel), 7 (volume), 10 (pan), 11 (expression), and 74 (filter cutoff). This attribute is required.

• modAmount: This value between 0 and 1 controls how much the modulation affects the things it is targeting. In conventional terms, this is like the modulation depth. Default value: 1.0.

• channel: Determines which MIDI channel’s CC values to read. Options are:

– "voice" (default): Uses the MIDI channel of the note that triggered the voice. This is essential for MPE and multi-channel setups where each voice may be on a different channel.

– 1-16: A specific channel number. The modulator will always read CC values from this channel, regardless of which channel triggered the voice. Useful for global control in non-MPE setups.

For voice-scope modulators, the default is "voice". For global-scope modulators, the default is "1".

Here’s a practical example that uses the mod wheel (CC#1) to control filter cutoff with per-voice channel tracking:

```xml
<modulators>
<midiCC number="1" modAmount="1.0" channel="voice" scope="voice">
<binding type="effect" level="group" groupIndex="0" effectIndex="0" parameter=
"FX_FILTER_FREQUENCY"
modBehavior="add"
translation="table"
translationTable="0,33;0.3,150;0.4,450;0.5,1100;0.7,4100;0.9,11000;1.0001,
22000" />
</midiCC>
</modulators>
```

Example of a global CC modulator that always reads from channel 1:

```xml
<modulators>
<midiCC number="11" modAmount="1.0" channel="1" scope="global">
<binding type="amp" level="instrument" parameter="AMP_VOLUME"
modBehavior="set"
translation="linear"
translationOutputMin="0"
translationOutputMax="1" />
</midiCC>
</modulators>
```

Important distinction: The <midiCC> modulator creates temporary modulation that only lasts for the life of each note. This means the modulation values are computed per-note and do not permanently change the underlying parameter values. In contrast, the <midi><cc> element (documented in the MIDI section) allows MIDI continuous controllers to make permanent value changes to parameters, similar to how UI knobs work. Use <midiCC> modulators when you want per-note modulation behavior (like using the mod wheel to add vibrato to individual notes), and use <midi><cc> bindings when you want global parameter changes (like using a CC to control the overall cutoff frequency of a filter).

MPE and Multi-Channel Considerations: In MPE mode or multi-channel setups, CC messages are sent per-channel. The channel attribute determines how the modulator handles this:

• With channel="voice", each note reads CC values from its own MIDI channel, allowing independent control per note (essential for MPE)

• With channel="1" (or any specific channel), all voices read from that channel’s CC values, providing global control

## 1.7.4 The <midiVelocity> element

The <midiVelocity> element allows you to use note-on velocity as a modulation source. This makes it possible to control various parameters based on how hard a key is pressed, providing dynamic expression in your instrument.

```xml
<modulators>
<midiVelocity modAmount="1.0" scope="voice">
<!-- Bindings go here -->
</midiVelocity>
</modulators>
```

This element has the following attributes:

• modAmount: This value between 0 and 1 controls how much the modulation affects the things it is targeting. In conventional terms, this is like the modulation depth. Default value: 1.0.

• scope: Whether or not this modulator exists for all notes or whether each keypress gets its own modulator. Possible values are global and voice (default for midiVelocity). If voice is chosen, each note retains its own velocity value for modulation.

Here’s a practical example that uses velocity to control the brightness of a sound:

<modulators>   
<midiVelocity scope="voice" modAmount="1.0">   
<binding type="effect" level="group" groupIndex="0" effectIndex="0" parameter=   
"FX_FILTER_FREQUENCY"   
modBehavior="add"   
translation="linear"   
translationOutputMin="0"

(continues on next page)

```xml
translationOutputMax="5000" />
</midiVelocity>
</modulators>
```

(continued from previous page)

This example uses velocity to modulate the cutoff frequency of a low-pass filter, making softer notes sound darker and harder notes sound brighter. This is a common technique for adding expressiveness to sampled instruments.

Another example showing velocity controlling reverb amount:

```xml
<modulators>
<midiVelocity scope="voice" modAmount="1.0">
<binding type="effect" level="group" groupIndex="0" effectIndex="1" parameter=
"FX_REVERB_WET_LEVEL"
modBehavior="set"
translation="linear"
translationOutputMin="0.0"
translationOutputMax="0.5" />
</midiVelocity>
</modulators>
```

## 1.7.5 The <mpeTimbre> element

The <mpeTimbre> element allows users to control the timbre of an instrument in response to MPE messages. NOTE: In order for this to work, the plugin must be in MPE mode. This can be turned on by going into the File > MIDI Input Settings.. dialog box. The <mpeTimbre> element has the following attributes:

• scope: Whether or not this MPE timbre exists for all notes or whether each keypress gets its own MPE timbre. Possible values are global and voice (default for MPE timbre). If voice is chosen, a new MPE timbre is started each time a new note is pressed.

• risingSmoothingTime: The time in milliseconds it takes for the MPE timbre to rise to its target value. Default value: 0 milliseconds.

• fallingSmoothingTime: The time in milliseconds it takes for the MPE timbre to fall to its target value. Default value: 0 milliseconds.

Here’s a practical example of how to use the <mpeTimbre> element:

<modulators>   
<mpeTimbre scope="voice" fallingSmoothingTime="20" risingSmoothingTime="20">   
<!-- This binding modifies the frequency of a low-pass filter -->   
<binding type="effect" level="group" groupIndex="0" effectIndex="0" parameter=   
"FX_FILTER_FREQUENCY"   
modBehavior="add"   
translation="table"   
translationTable="0,33;0.3,150;0.4,450;0.5,1100;0.7,4100;0.9,11000;1.0001,   
22000" />   
</mpeTimbre>   
</modulators>

This example shows an MPE timbre modulator that modifies the frequency of a low-pass filter based on the MPE timbre value. This example uses a translation table to create a non-linear response curve, making the filter frequency changes more musical and intuitive.

## 1.7.6 The <mpePressure> element

The <mpePressure> element allows users to control an instrument in response to MPE Pressure messages. NOTE: In order for this to work, the plugin must be in MPE mode. This can be turned on by going into the File > MIDI Input Settings.. dialog box. The <mpePressure> element has the following attributes:

• scope: Whether or not this MPE pressure exists for all notes or whether each keypress gets its own MPE pressure. Possible values are global and voice (default for MPE pressure). If voice is chosen, a new MPE pressure is started each time a new note is pressed.

• risingSmoothingTime: The time in milliseconds it takes for the MPE pressure to rise to its target value. Default value: 0 milliseconds.

• fallingSmoothingTime: The time in milliseconds it takes for the MPE pressure to fall to its target value. Default value: 0 milliseconds.

Here’s a practical example of how to use the <mpePressure> element:

```xml
<modulators>
<mpePressure scope="voice" fallingSmoothingTime="20" risingSmoothingTime="20">
<binding type="amp" level="group" groupIndex="0" effectIndex="0" parameter="AMP_
VOLUME"
modBehavior="set"
translation="linear"
translationOutputMin="0.5"
translationOutputMax="1" />
</mpePressure>
</modulators>
```

This example shows an MPE pressure modulator that modifies the volume of a group based on the MPE pressure value.   
The translation is linear, meaning that the pressure value directly scales the volume between 0.5 and 1.

## 1.7.7 The <random> element

The <random> element allows you to generate random values for modulation. This can be useful for adding unpredictability and variation to your sounds. The <random> element always produces a value between -1 and 1. It has the following attributes:

• mode: The event that triggers the random value generation. This can be either note_on or periodic.

• frequency: How often the random value is generated. This is only relevant if the mode is set to periodic.

• trigger: Two possible values: attack means that the random value generator is reset on note-on events, none means that it is not. This is really only help for periodic random value generators.

• seed: An integer seed value for the random number generator. This can be useful for creating reproducible results.

• scope: Whether or not this modulator exists for all notes or whether each keypress gets its own modulator. Possible values are global and voice. If voice is chosen, a new random modulator is started each time a new note is pressed.

Here’s an example of how to use the <random> element:

```xml
<modulators>
<random mode="note_on" seed="12345" scope="voice">
<binding type="amp" level="group" groupIndex="0" effectIndex="0" parameter="AMP
VOLUME"
```

modBehavior="set"   
translation="linear"   
translationOutputMin="0.5"   
translationOutputMax="1" />   
</random>   
</modulators>

## 1.7.8 How to use <binding>s in conjunction with modulators

In order to actually have your LFOs and envelopes do anything, you need to have bindings under them. If you are not familiar with the concept of bindings, you may want to read this section then return here. Bindings tell the engine which parameters the LFO should be affecting and how. Here is an example:

```xml
<modulators>
<lfo shape="sine" frequency="2" modAmount="1.0">
<!-- This binding modifies the frequency of a low-pass filter
<binding type="effect" level="instrument" effectIndex="0" parameter="FX_FILTER
FREQUENCY" modBehavior="add" translation="linear" translationOutputMin="0"␣
translationOutputMax="2000.0" />
</lfo>
</modulators>
```

## Controlling Modulator Parameters with Bindings

You can bind to modulator parameters themselves to control them in real-time from UI controls, MIDI CC, or other sources. The binding uses type="modulator" and level="instrument", with a modulatorIndex attribute pointing to the modulator (0-based) and a parameter attribute specifying which property to change.

## Bindable parameters by modulator type

## <lfo>

parameter Description   
value   
MOD_AMOUNT Modulation depth (0.0–1.0)   
FREQUENCY LFO rate (Hz when frequencyFormat="hz", subdivision index when   
frequencyFormat="musical_time")   
SHAPE Waveform shape ("sine", "saw", "square", "triangle")   
MOD_DELAY_TIME Delay before the LFO begins (seconds)   
TRIGGER Reset behavior ("attack" or "none")

## <envelope>


| parameter value | Description |
| --- | --- |
| MOD_AMOUNT | Modulation depth (0.0–1.0) |
| ENV_ATTACK | Attack time (seconds) |
| ENV_ATTACK_CURVE | Attack curve shape |
| ENV_DECAY | Decay time (seconds) |
| ENV_DECAY_CURVE | Decay curve shape |
| ENV_SUSTAIN | Sustain level (0.0–1.0) |
| ENV_RELEASE | Release time (seconds) |
| ENV_RELEASE_CURVE | Release curve shape |
| MOD_DELAY_TIME | Delay before the envelope begins (seconds) |



## <random>


| parameter value Description |  |
| --- | --- |
| MOD_AMOUNT | Modulation depth (0.0–1.0) |
| FREQUENCY | Generation rate in Hz (only relevant when mode="periodic") |
| TRIGGER | Reset behavior ("attack" or "none") |



## <midiCC>


| parameter value Description |  |
| --- | --- |
| MOD_AMOUNT | Modulation depth (0.0–1.0) |




| parameter value Description |  |
| --- | --- |
| MOD_AMOUNT | Modulation depth (0.0–1.0) |




| parameter value Description |  |
| --- | --- |
| MOD_AMOUNT | Modulation depth (0.0–1.0) |




| parameter value Description |  |
| --- | --- |
| MOD_AMOUNT | Modulation depth (0.0–1.0) |



## Example: controlling LFO parameters from UI knobs

```xml
<DecentSampler>
<ui>
<tab>
<!-- Controls LFO depth -->
<labeled_knob x="10" y="30" width="90" textSize="16" textColor="AA000000"
parameterName="Depth" minValue="0" maxValue="1" value="1">
<binding type="modulator" level="instrument" parameter="MOD_AMOUNT"
modulatorIndex="0" translation="linear" />
</labeled_knob>
<!-- Controls LFO rate -->
<labeled_knob x="110" y="30" width="90" textSize="16" textColor="AA000000"
parameterName="Rate" minValue="0.1" maxValue="20" value="2">
<binding type="modulator" level="instrument" parameter="FREQUENCY"
modulatorIndex="0" translation="linear" />
</labeled_knob>
<!-- Controls LFO delay time -->
<labeled_knob x="210" y="30" width="90" textSize="16" textColor="AA000000"
parameterName="Delay" minValue="0" maxValue="2" value="0.5">
<binding type="modulator" level="instrument" parameter="MOD_DELAY_TIME"
modulatorIndex="0" translation="linear" />
</labeled_knob>
</tab>
</ui>
<modulators>
<lfo shape="sine" frequency="2" modAmount="1.0" delayTime="0.500">
<binding type="effect" level="instrument" effectIndex="0" parameter="FX_FILTER_
FREQUENCY"
modBehavior="add" translation="linear" translationOutputMin="0"␣
translationOutputMax="2000.0" />
</lfo>
</modulators>
</DecentSampler>
```

You can also control modulator parameters via MIDI CC:

```xml
<DecentSampler>
<midi>
<cc number="74">
<binding type="modulator" level="instrument" parameter="MOD_AMOUNT"
modulatorIndex="0" translation="linear" translationOutputMin="0"␣
translationOutputMax="1" />
</cc>
</midi>
<modulators>
<lfo shape="sine" frequency="2" modAmount="1.0" delayTime="0.500">
<binding type="effect" level="instrument" effectIndex="0" parameter="FX_FILTER
FREQUENCY"
modBehavior="add" translation="linear" translationOutputMin="0"␣
translationOutputMax="2000.0" />
</lfo>
</modulators>
```

## </DecentSampler>

There are a few differences between bindings as they are used by knobs and the ones used by modulators. Specifically, when you move a UI control that has a binding attached, the engine actually goes out and changes the value of the parameter that is targeted by that binding. For example, if you have a knob that controls a lowpass filter’s cutoff frequency, moving that knob will cause that actual frequency of that filter to change. In other words, the changes that the knob is making on the underlying sample library are permanent. The same is also true for bindings associated with MIDI continuous controllers.

Modulators, on the other hand, are temporary. If a modulator (such as an LFO) changes its value, the engine looks at the bindings associated with that LFO and then makes a list of temporary changes to the underlying data. When it comes time to render out the effect, it consults both the permanent value and the temporary modulation values. As a result of this difference in the way bindings are handled, only some parameters are “modulatable.” At time of press, the following parameters are modulatable:

• Almost all effect parameters

• Group Volume

• Global Volume

• Group Pan

• Global Pan

• Group Tuning

• Global Tuning

## 1.7.9 Modulator scope: global or voice-level

By default, all modulators will be created at the global level. This means that there will be exactly one modulator that is shared by all voices. In many situations, such as an LFO modulating a single low-pass filter which is shared by all of voices, this is often what we want.

But there are other situations where we don’t want our modulator to be global. For example, what if we want to have a unique envelope for each key-press? Well, for that we use the scope attribute of the <lfo> or <envelope> element. This attribute can be set to voice, which means that each time a new note is pressed, a new modulator will be created for that note. This is particularly useful for envelopes, which are often used to control parameters that are unique to each note, such as the volume or filter cutoff of a note:

<envelope attack="2" decay="0" sustain="1" release="0.5" modAmount="1.0" scope="voice">

For certain parameters, such as Group Tuning or Group Pan, it may always make sense to have scope="voice" set.   
For others, such as Global Volume or Global Pan, it may make more sense to have scope="global" set.

For a full discussion of how to use modulators, check out this article on the Decent Samples blog.

## 1.8 The <tags> element

The <tags> element lives right below your top-level <DecentSampler> element. It allows you to specify details about the tags you use throughout your instrument. It is however not actually necessary to include a <tags> element for every tag you use. You only need to create this if you want to specify additional details about your tags.

## 1.8.1 The <tag> element

Underneath the <tags> element, you can have any number of <tag> elements. These specify details for each individual tag that you use throughout your sample mapping. Attributes:


| Attribute | Description |
| --- | --- |
| enabled ( (optional) | Whether or not this tag is enabled. Possible values: true, false. Default: true |
| volume (optional) | A number for 0.0 to 1.0 that specifies the initial volume for a tag. Default: 1.0 |
| polyphor (op- | A whole number that specifies the number of voices allowed for this tag. Default: -1 (unlim- tional) ited). Set to 1 for monophonic behavior. |



## 1.8.2 Example

This example shows a tag configuration with polyphony control:

```xml
<tags>
<tag name="voice1" volume="1" pan="0" polyphony="12"/>
</tags>
```

The polyphony attribute can be controlled dynamically using bindings with the TAG_POLYPHONY parameter. This is useful for creating mono/poly switches:

<button x="740" y="130" width="12" height="12" style="image" value="0">   
<state name="Poly" mainImage="Images/Checkbox On.png">   
<binding type="general" level="tag" identifier="voice1"   
parameter="TAG_POLYPHONY" translation="fixed_value" translationValue="12"/>   
</state>   
<state name="Mono" mainImage="Images/Checkbox Off.png">   
<binding type="general" level="tag" identifier="voice1"   
parameter="TAG_POLYPHONY" translation="fixed_value" translationValue="1"/>   
</state>   
</button>

## 1.9 The <buses> element

As of version 1.12.0, DecentSampler has support for audio buses, which can be used to create a more complex mix of the samples in the sample library as well as route audio to various audio outputs. Sample library designers can specify up to 16 buses in the <buses> element using the <bus> tag. Each bus can have its own volume and audio output settings.

## 1.9.1 The <bus> element

Within the <buses> element, you can have up to 16 <bus> sub-elements. These specify parameters for each individual bus that you would like to have in your sample library. The <bus> element has the following attributes:

• busVolume: The volume of the bus. This is a floating-point number between 0.0 and 1.0, where 0.0 is silent and 1.0 is full volume.

• output1Target: The first audio output for this bus. This is a string that specifies the audio output that the bus should be routed to. The available options are MAIN_OUTPUT (the main audio output), AUX_STEREO_OUTPUT_1 (auxiliary output 1), AUX_STEREO_OUTPUT_2 (auxiliary output 2), AUX_STEREO_OUTPUT_3 (auxiliary output 3), AUX_STEREO_OUTPUT_4 (auxiliary output 4), AUX_STEREO_OUTPUT_5 (auxiliary output 5), AUX_STEREO_OUTPUT_6 (auxiliary output 6), AUX_STEREO_OUTPUT_7 (auxiliary output 7), AUX_STEREO_OUTPUT_8 (auxiliary output 8), AUX_STEREO_OUTPUT_9 (auxiliary output 9), AUX_STEREO_OUTPUT_10 (auxiliary output 10), AUX_STEREO_OUTPUT_11 (auxiliary output 11), AUX_STEREO_OUTPUT_12 (auxiliary output 12), AUX_STEREO_OUTPUT_13 (auxiliary output 13), AUX_STEREO_OUTPUT_14 (auxiliary output 14), AUX_STEREO_OUTPUT_15 (auxiliary output 15), and AUX_STEREO_OUTPUT_16 (auxiliary output 16).

• output2Target: The second audio output for this bus. This is a string that specifies the audio output that the bus should be routed to. The available options are the same as for output1Target.

• output3Target: The third audio output for this bus. This is a string that specifies the audio output that the bus should be routed to. The available options are the same as for output1Target.

• output4Target: The fourth audio output for this bus. This is a string that specifies the audio output that the bus should be routed to. The available options are the same as for output1Target.

• output5Target: The fifth audio output for this bus. This is a string that specifies the audio output that the bus should be routed to. The available options are the same as for output1Target.

• output6Target: The sixth audio output for this bus. This is a string that specifies the audio output that the bus should be routed to. The available options are the same as for output1Target.

• output7Target: The seventh audio output for this bus. This is a string that specifies the audio output that the bus should be routed to. The available options are the same as for output1Target.

• output8Target: The eighth audio output for this bus. This is a string that specifies the audio output that the bus should be routed to. The available options are the same as for output1Target.

• output1Volume: The volume of the audio being sent to the bus’ first output. This is a floating-point number between 0.0 and 1.0, where 0.0 is silent and 1.0 is full volume.

• output2Volume: The volume of the audio being sent to the bus’ second output. This is a floating-point number between 0.0 and 1.0, where 0.0 is silent and 1.0 is full volume.

• output3Volume: The volume of the audio being sent to the bus’ third output. This is a floating-point number between 0.0 and 1.0, where 0.0 is silent and 1.0 is full volume.

• output4Volume: The volume of the audio being sent to the bus’ fourth output. This is a floating-point number between 0.0 and 1.0, where 0.0 is silent and 1.0 is full volume.

• output5Volume: The volume of the audio being sent to the bus’ fifth output. This is a floating-point number between 0.0 and 1.0, where 0.0 is silent and 1.0 is full volume.

• output6Volume: The volume of the audio being sent to the bus’ sixth output. This is a floating-point number between 0.0 and 1.0, where 0.0 is silent and 1.0 is full volume.

• output7Volume: The volume of the audio being sent to the bus’ seventh output. This is a floating-point number between 0.0 and 1.0, where 0.0 is silent and 1.0 is full volume.

• output8Volume: The volume of the audio being sent to the bus’ eighth output. This is a floating-point number between 0.0 and 1.0, where 0.0 is silent and 1.0 is full volume.

Here is an example of a <buses> element with two buses:

```xml
<buses>
<bus busVolume="0.5" output1Target="MAIN_OUTPUT" output2Target="AUX_STEREO_OUTPUT_1"
output1Volume="0.8" output2Volume="0.5"/>
<bus busVolume="0.8" output1Target="AUX_STEREO_OUTPUT_2" output2Target="AUX_STEREO_
OUTPUT_3" output1Volume="0.6" output2Volume="0.7"/>
</buses>
```

In this example, there are two buses defined. The first bus has a volume of 0.5 and is routed to the main output with a volume of 0.8 and auxiliary output 1 with a volume of 0.5. The second bus has a volume of 0.8 and is routed to auxiliary outputs 2 and 3 with volumes of 0.6 and 0.7, respectively.

## 1.9.2 Effects and buses

Buses can also have effects applied to them. To apply an effect to a bus, you can use the <effects> element within the <bus> element. The <effects> element can contain one or more <effect> sub-elements, each of which specifies an effect to apply to the bus. Details on the attributes of the <effect> element can be found in the Effects section.

Here is an example of a <bus> element with an effect applied to it:

<bus busVolume="0.5" output1Target="MAIN_OUTPUT" output2Target="AUX_STEREO_OUTPUT_1"␣   
output1Volume="0.8" output2Volume="0.5">   
<effects>   
<effect type="Reverb" wetDryMix="0.5" roomSize="0.5" damping="0.5"/>   
</effects>   
</bus>

In this example, a reverb effect is applied to the bus with a wet/dry mix of 0.5, room size of 0.5, and damping of 0.5.

## 1.9.3 Using buses in the sample library

Once you have defined buses in your sample library, you can use them in the <sample> elements to route the audio of the samples to specific buses. To do this, you can use the outputXTarget attributes in the <sample>, <group>, or <groups> elements. The outputXTarget attributes specifies the audio output that the sample should be routed to. The available options are the same as for the output1Target attribute above, but, with additional options for the buses defined in the <buses> element using the format: BUS_1, BUS_2, . . . , BUS_16.

Here is an example of a <group> element with samples routed to a bus:

<group output1Target="MAIN_OUTPUT" output1Volume="1.0" output2Target="BUS_1"␣   
output2Volume="0.2">   
<sample path="Samples/Volca Keys Poly-V127-29-F0.wav" loNote="17" hiNote="17"

(continues on next page)

```twig
rootNote="17"/>
<!-- More samples here
</group>
```

(continued from previous page)

In this example, the audio of the samples is routed to both the main output and to the first bus defined in the <buses> element. The volume of the audio sent to the bus is 0.2 (20% of full volume).

## 1.10 Appendix A: The Color Format

Colors are represented throughout the dspreset files using an 8-digit ARGB color format. These are identical to web color hex codes except with an additional 2-digit hex number in front of them. The first two digits are a hexadecimal representation of alpha level with 00 being fully transparent, 80 being 50% transparent, and FF being fully opaque.

Examples:

• Black (solid): FF000000

• Black (90% transparency): E6000000

• Red (solid): FFFF0000

• Red (50% transparency): 80FF0000

• Blue (solid): FF0000FF

## 1.11 Appendix B: The <binding> element

Adding a binding to a UI control, MIDI handler, or modulator tells the DecentSampler engine that it should take input from a source and use it to change values in another part of the engine. An example of this would be a knob which controls the volume of a group, a CC controller that changes an effect parameter, or an LFO that modulates an effect parameter.

In order to set up a binding for a specific source, create a <binding> element within the source element.

In this example, a labeled knob is controlling the volume of the first group of samples (group 0):

```xml
<DecentSampler>
<ui>
<tab>
<labeled-knob x="420" y="100" label="RT" type="float" minValue="0" maxValue="1" ␣
value="0.3" textSize="20">
<binding type="amp" level="group" position="0" parameter="AMP_VOLUME"␣
translation="linear" translationOutputMin="0" translationOutputMax="1.0" />
</labeled-knob>
</tab>
</ui>
</DecentSampler>
```

Here’s a full list of parameters for the <binding> element:


| Attribute | Description |  |
| --- | --- | --- |
| type | This tells the engine what type of parameter this is. Valid values are: amp, general, effect, control, note, note_binding, velocity_binding, button_state_binding, keyboard_color, modulator, note_sequence. | Re- quired |
| level | Valid values are ui, instrument, group, tag, midi | Re- quired |
|  | positi The specific 0-based index of the element to be modified by this binding. If you are targeting a group, for example, the first group would be 0, the second group would be 1, etc. | Re- quired |
|  | contro. When a binding is targeting a control, this is the same thing as the position attribute. It is a specific 0-based index of the control to be modified by this binding. If you are targeting an group- level eff ect, this would specified the group under which the eff ect l ives. | Op- tional |
|  | groupI When a binding is targeting a group, this is the same thing as the position attribute. It is a specific 0-based index of the group to be modified by this binding. If you are targeting an group-level effect, this would specified the group under which the effect lives. | Op- tional |
|  | effect: When a binding is targeting an effect, this is the same thing as the posi ti on attribute. It is a specific 0-based index of the effect to be modified by this binding. As an alternative to using an index, you can use the effectTags attribute (see below) to target effects by their tags name. | Op- tional |
|  | modula When a binding is targeting a modulator, this is the same thing as the position attribute. It is a specific 0-based index of the modulator to be modified by this binding. As an alternative to using an index, you can use the modulatorTags attibute (see below) to target modulators by their tags name. | Op- tional |
| tags | A comma-separated list of tags. The meaning depends on context: at level="group", this selects the target group(s) by tag. For backward compatibility, tags is also promoted into the appropriate typed tag attribute (groupTags, effectTags, modulatorTags, or controlTags) when the cor- responding typed attribute is not explicitly set. Prefer the typed attributes for new presets. | tional |
|  | groupT: A comma-separated list of group tags. At level="group", targets all <group> elements whose tags attribute contains any of these tags. This is the explicit, typed form of the group-selection behavior that tags has always provided. | Op- tional |
|  | effect' A comma-separated list of effect tags. When type="effect", targets all <effect> elements (at instrument or group level) whose tags atribute contains any of these tags. Use this instead of effect Index when you want to adress eff ects by name rather than position. | Op- tional |
|  | modula A comma-separated list of modulator tags. When type="modulator" at level="instrument", targets all modulator elements (<lfo>, <envelope>, etc.) whose tags attribute contains any of these tags. Use this instead of modulatorIndex when you want to address modulators by name rather than position. | Op- tional |
|  | contro. A comma-separated list of UI control tags. When type="control" at level="ui", targets all UI controls whose tags attribute contains any of these tags. Use this instead of controlIndex when you want to address controls by name rather than position. | Op- tional |
|  | enable A value that turns the binding on and off. Valid values are: true, false. identi: A string identifying the specific parameter that you wish to change. If you are modulating based on | Op- tional Re- |
|  | tags, you would put the tag you are targeting here. See Appendix D for example. parame A token describing the specific kind of parameter that you wish to change. A list of controller | quired Re- |
|  | parameters are below. transl Valid values are fixed_value, linear and table. Explanation of both translation modes is in a | quired Op- |
|  | separate section below. Default: linear transl: This is the min value this binding should send to the target parameter. This is only looked at if | tional Op- |
|  | translation is set to linear. | tional |
|  | transl This is the max value this binding should send to the target parameter. This is only looked at if translation is set to linear. | Op- tional |
|  | transl: Valid values are true and false. Default: false. This is only looked at if translation is set to linear. | Op- tional |
|  | transl A list of input-output pairs that make up the translation table. The input and output are separated by | Op- |
|  | 1.11. Appendihernangrienemselves are separated by semi-colons. Default: 0, 0;1,1. You must have at east two coordinates in your list. This is only looked at if translation is set to |  |
| table. | transl: The value that should be passed along when translation is set to fixed_value. | Op- |



## 1.11.1 Binding Parameters for Targeting Note Sequences

A special set of binding attributes exist for targeting note sequences:


| Attribute | Description | Default | Required |
| --- | --- | --- | --- |
|  | seqIndex A 0-based index of a sequence underneath the <noteSequences> section seqFollc Whether or not the sequence should follow the global tempo. Valid values | None true | Required Optional |
|  | are true and false. If this is set to false, then playback will be hardcoded at 120BPM. This can be useful if you want to assure that sequences will always play back at the same rate regardless of the DAW clock. |  |  |
| seqTrigg | What the binding should do with the sequence in question. Valid values are on (start playing the sequence), off (stop playing the sequence), midi_key (special value that will cause the binding to follow a specific MIDI key note binding) | midi_key Optional |  |
|  | seqPlaye An identifier used for tracking the state of a sequence. This value can be None any sequence of numbers or letters. |  | Required when seqTriggerBe is on or off |
|  | seqTrack Whether or not the sequence should respect the velocity of the incoming MIDI note. This can only be used when the sequence is being triggered by a MIDI note binding. Value should be a floating point number from 0.0 to 1. |  | Optional |
|  | seqTrans Transpose the notes in the sequence by an arbitrary number of half steps. Any Value should be a floating point number from -36 to 36. | sequence of num- bers of letters | Optional |
|  | seqTrans Transpose the notes in the sequence relative to the pitch of the incoming MIDI note. This can only be used when the sequence is being triggered by a MIDI note binding. Value should be a floating point number from 0 to 127. | Any sequence of num- bers of letters | Optional |
|  | seqPlayt The speed of playback. Value should be a floating point number from 0.001 to 10000. | 1.0 | Optional |
| no_loop | seqLoopl Valid values are: forward, reverse, random, random_no_repeat, forward |  | Optional |



## 1.11.2 Controllable Parameters

This is a list of parameters that can be used in conjunction with the <binding> element above. NOTE: The table below scrolls to the right.


| Description | type | level | parameter |
| --- | --- | --- | --- |
| Global Volume | amp | instrument | AMP_VOLUME |
| Global Tuning | amp | instrument | GLOBAL_TUNING |
| Global Pan | amp | instrument | PAN |
| Sample Start (see note 2 below) | general | instrument or group | SAMPLE_START |
| Sample End (see note 2 below) | general | instrument or group | SAMPLE_END |
| Loop Start (see note 2 below) | general | instrument or group | LOOP_START |
| Loop End (see note 2 below) | general | instrument or group | LOOP_END |
| Low Note | general | instrument or group | LO_NOTE |
| High Note | general | instrument or group | HI_NOTE |
| Low Velocity | general | group | LO_VEL |
| High Velocity | general | group | HI_VEL |
| Root Note | general | instrument or group | ROOT_NOTE |
| Silencing Decay | general | instrument or group | SILENCING_DECAY |
| Silencing Mode | general | instrument or group | SILENCING_MODE |
| Pitch Key Track | general | instrument or group | PITCH_KEY_TRACK |
| Amplitude Velocity Tracking | amp | instrument | AMP_VEL_TRACK |
| Global Amp Envelope Attack | amp | instrument | ENV_ATTACK |
| Global Amp Envelope Attack Curve Shape | amp | instrument | ENV_ATTACK_CURV |
| Global Amp Envelope Decay | amp | instrument | ENV_DECAY |
| Global Amp Envelope Decay Curve Shape Global Amp Envelope Sustain | amp | instrument | ENV_DECAY_CURVE |
| Global Amp Envelope Release | amp | instrument | ENV_SUSTAIN |
|  | amp | instrument | ENV_RELEASE |
| Global Amp Envelope Release Curve Shape | amp | instrument | ENV_RELEASE_CUR |
| Glide/Portamento Time | amp | instrument | GLIDE_TIME |
| Group Enabled / Disabled | amp | group | ENABLED |
| Group Amplitude Envelope Enabled / Disabled | amp | group | AMP_ENV_ENABLED |
| Group Volume Group Tuning | amp | group | AMP_VOLUME |
| Pan | amp | group | GROUP_TUNING |
| Amplitude Velocity Tracking | amp | group | PAN |
| Group Amp Envelope Attak | amp | group | AMP_VEL_TRACK |
| Group Amp Envelope Decay | amp | group | ENV_ATTACK |
| Group Amp Envelope Sustain | amp | group | ENV_DECAY |
| Group Amp Envelope Release | amp | group | ENV_SUSTAIN |
| Group Output 1 Volume | amp | group | ENV_RELEASE |
| Group Output 2 Volume | amp | group | OUTPUT_1_VOLUME |
| Group Output 3 Volume | amp | group | OUTPUT_2_VOLUME |
| Group Output 4 Volume | amp | group | OUTPUT_3_VOLUME |
| Group Output 5 Volume | amp | group | OUTPUT_4_VOLUME |
| Group Output 6 Volume | amp | group | OUTPUT_5_VOLUME |
|  | amp | group | OUTPUT_6_VOLUME |
| Group Output 7 Volume | amp | group | OUTPUT_7_VOLUME |
| Group Output 8 Volume | amp | group | OUTPUT_8_VOLUME |
| Group Output 1 Target Group Output 2 Target | amp | group | OUTPUT_1_TARGET |
| Group Output 3 Target | amp | group | OUTPUT_2_TARGET |
| Group Output 4 Target | amp | group | OUTPUT_3_TARGET |
| Group Output 5 Target | amp | group | OUTPUT_4_TARGET |
| Group Output 6 Target | amp | group | OUTPUT_5_TARGET |
| Group Output 7 Target | amp | group | OUTPUT_6_TARGET |
| Group Output 8 Target | amp | group | OUTPUT_7_TARGET |
| Group Glide/Portamento Time | amp | group | OUTPUT_8_TARGET |
| Oscillator Waveform (group level) | amp | group | GLIDE_TIME |
|  | general | group | OSCILLATOR_WAVE |
| Oscillator Damping (pluck1 only) | general | group | OSCILLATOR_DAMP |
| Oscillator Pluck Type (pluck1 only) Oscillator Wavetable Position (wavetable only) | general general | group | OSCILLATOR_PLUC OSCILLATOR_WAVE |
| Oscillator Wavetable Frame Interpolation (wavetable only) | general | group group | OSCILLATOR_WAVE |
| Harmonic Oscillator Partial Count (harmonic only) | general | group | OSCILLATOR_HARM |
| Harmonic Oscillator Tilt (harmonic only) | general | group | OSCILLATOR_HARM |
| Harmonic Oscillator Odd/Even Balance (harmonic only) | general | group | OSCILLATOR_HARM |
| Harmonic Oscillator Normalization (harmonic only) | general | group | OSCILLATOR_HARM |
| Harmonic Oscillator Partial Levels (harmonic only) | general | group | OSCILLATOR_HARM |
| FM6 Algorithm (fm6op only) | general | group | OSCILLATOR_FM_A |
| FM6 Operator 1 Level (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 2 Level (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 3 Level (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 4 Level (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 5 Level (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 6 Level (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 1 Ratio (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 2 Ratio (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 3 Ratio (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 4 Ratio (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 5 Ratio (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 6 Ratio (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 1 Feedback (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 2 Feedback (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 3 Feedback (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 4 Feedback (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 5 Feedback (fm6op only) FM6 Operator 6 Feedback (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 1 Detune (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 2 Detune (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 3 Detune (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 4 Detune (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 5 Detune (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 6 Detune (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 1 Mode (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 2 Mode (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 3 Mode (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 4 Mode (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 5 Mode (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 6 Mode (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 1 Fixed Frequency (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 2 Fixed Frequency (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 3 Fixed Frequency (fm6op only) | general general | group | OSCILLATOR_FM_0 |
| FM6 Operator 4 Fixed Frequency (fm6op only) | general | group | OSCILLATOR_FM_0 OSCILLATOR_FM_0 |
| FM6 Operator 5 Fixed Frequency (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 6 Fixed Frequency (fm6op only) | general | group group | OSCILLATOR_FM_0 |
| FM6 Operator 1 Velocity Sensitivity (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 2 Velocity Sensitivity (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 3 Velocity Sensitivity (fmbop only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 4 Velocity Sensitivity (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 5 Velocity Sensitivity (fm6op only) | general |  | OSCILLATOR_FM_0 |
| FM6 Operator 6 Velocity Sensitivity (fm6op only) | general | group group | OSCILLATOR_FM_0 |
| FM6 Operator 1 Attack (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 2 Attack (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 3 Attack (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 4 Attack (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 5 Attack (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 6 Attack (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 1 Decay (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 2 Decay (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 3 Decay (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 4 Decay (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 5 Decay (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 6 Decay (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 1 Sustain (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 2 Sustain (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 3 Sustain (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 4 Sustain (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 5 Sustain (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 6 Sustain (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 1 Release (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 2 Release (fm6op only) FM6 Operator 3 Release (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 4 Release (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 5 Release (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 6 Release (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 1 EG Type (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 2 EG Type (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 3 EG Type (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 4 EG Type (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 5 EG Type (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 6 EG Type (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 1 EG Rate 1 (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 1 EG Rate 2 (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 1 EG Rate 3 (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 1 EG Rate 4 (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 2 EG Rate 1 (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 2 EG Rate 2 (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 2 EG Rate 3 (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 2 EG Rate 4 (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 3 EG Rate 1 (fm6op only) | general general | group | OSCILLATOR_FM_0 |
| FM6 Operator 3 EG Rate 2 (fm6op only) | general | group | OSCILLATOR_FM_0 OSCILLATOR_FM_0 |
| FM6 Operator 3 EG Rate 3 (fmbop only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 3 EG Rate 4 (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 4 EG Rate 1 (fm6op only) | general | group group | OSCILLATOR_FM_0 |
| FM6 Operator 4 EG Rate 2 (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 4 EG Rate 3 (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 4 EG Rate 4 (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 5 EG Rate 1 (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 5 EG Rate 2 (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 5 EG Rate 3 (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 5 EG Rate 4 (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 6 EG Rate 1 (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 6 EG Rate 2 (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 6 EG Rate 3 (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 6 EG Rate 4 (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 1 EG Level 1 (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 1 EG Level 2 (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 1 EG Level 3 (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 1 EG Level 4 (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 2 EG Level 1 (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 2 EG Level 2 (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 2 EG Level 3 (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 2 EG Level 4 (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 3 EG Level 1 (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 3 EG Level 2 (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 3 EG Level 3 (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 3 EG Level 4 (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 4 EG Level 1 (fm6op only) FM6 Operator 4 EG Level 2 (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 4 EG Level 3 (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 4 EG Level 4 (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 5 EG Level 1 (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 5 EG Level 2 (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 5 EG Level 3 (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 5 EG Level 4 (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 6 EG Level 1 (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 6 EG Level 2 (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 6 EG Level 3 (fm6op only) | general | group | OSCILLATOR_FM_0 |
| FM6 Operator 6 EG Level 4 (fm6op only) | general | group | OSCILLATOR_FM_0 |
| Tag Enabled | general | group | OSCILLATOR_FM_0 |
| Tag Volume | amp | tag | TAG_ENABLED |
| Tag Polyphony | amp | tag | TAG_VOLUME |
| MIDI CC Binding Enabled | general | tag | TAG_POLYPHONY |
| MIDI Note Mapping Enabled | cc_binding | midi | ENABLED |
| MIDI Note Binding Enabled | note | midi | ENABLED |
| MIDI Note Binding Change seqIndex | note_binding | midi | ENABLED |
| MIDI Note Binding Change seqLoopMode | note_binding | midi | SEQ_INDEX |
| MIDI Note Binding Change seqTransposeWithRootNote | note_binding | midi | SEQ_LOOP_MODE |
| MIDI Note Binding Change seqPlaybackRate | note_binding | midi | SEQ_TRANSPOSE_W |
|  | note_binding | midi | SEQ_PLAYBACK_RA |
| MIDI Note Binding Change seqTrackMidiInputVelocity MIDI Note Binding Change seqTranspose | note_binding | midi | SEQ_TRACK_MIDI_ |
| MIDI Velocity Binding Enabled | note_binding | midi | SEQ_TRANSPOSE |
| Modulator Amount (Depth) | velocity_binding | midi | ENABLED |
| LFO Modulator Rate (or Frequency) | modulator | instrument | MOD_AMOUNT |
| LFO Modulator Delay Time | modulator | instrument | FREQUENCY |
| Envelope Modulator Attack | modulator | instrument | MOD_DELAY_TIME |
| Envelope Modulator Attack Curve Shape | modulator | instrument | ENV_ATTACK |
| Envelope Modulator Decay | modulator | instrument | ENV_ATTACK_CURV |
|  | modulator | instrument | ENV_DECAY |
| Envelope Modulator Decay Curve Shape | modulator | instrument | ENV_DECAY_CURVE |
| Envelope Modulator Sustain | modulator | instrument | ENV_SUSTAIN |
| Envelope Modulator Release | modulator | instrument | ENV_RELEASE |
| Envelope Modulator Release Curve Shape Sequence Rate | modulator note_sequence | instrument instrument | ENV_RELEASE_CUR RATE |
| All Notes Off | general | instrument | ALL_NOTES_OFF |



## UI Parameters

NOTE: The table below scrolls to the right.


| De- scrip- tion |  | type leve para Valid Range |  | Mod Additional required parameters и- lat- able |  |
| --- | --- | --- | --- | --- | --- |
| UI Back- ground Image Path UI | gener ui | BG_I Text |  |  | tags or controlIndex contains the 0- |
| Button State Bind- ing En- abled | buttc ui | ENAB true, false |  | the binding being referenced. | based index of the control in question (see note 1 below), stateIndex contains the 0- based index of the state in question, and bindingIndex that contains the index of |
| UI Con- trol En- abled | contr ui | ENAB true, false |  | question (see note 1 below) | tags or controlIndex/position con- tains the 0-based index of the control in |
| UI Con- trol Visible | contr ui | VISI true, false |  | question (see note 1 below) | tags or controlIndex or position con- tains the 0-based index of the control in |
| UI Con- trol Value | contr ui |  | VALU Any number | question (see note 1 below) | tags or controlIndex or position con- tains the 0-based index of the control in |
| UI Con- trol Text UI | contr ui contr ui | TEXT Text |  | question (see note 1 below) | tags or controlIndex or position con- tains the 0-based index of the control in tags or controlIndex or position con- |
| Con- trol Mini- mum Value |  | MIN_ Any number |  | question (see note 1 below) | tains the 0-based index of the control in |
| UI Con- trol Max- imum Value | contr ui | MAX_ Any number |  | question (see note 1 below) | tags or controlIndex or position con- tains the 0-based index of the control in |
| UI Con- trol Value Type | contr ui |  | VALU float, integer, musical_time | question (see note 1 below) | tags or controlIndex or position con- tains the 0-based index of the control in |
| Image or Ani- mation File | contr ui | PATH Text |  | question (see note 1 below) | tags or controlIndex or position con- tains the 0-based index of the control in |
| 7Bath fmage or Ani- mation | contr ui | OPAC 0.0 - 1.0 |  | question (see note 1 below) | Chapter 1, +Contents tags or controlIndexor position con- tains the 0-based index of the control in |



1. NOTE: The indexes of the controls within the UI also include UI controls that are not editable, such as <label> elements, so you’ll want to account for that when calculating your positions.

Here’s a quick example:

If your UI’s <tab> section has the following elements under it: <label>, <control>,<label>,<control>. The position indexes of the four elements will be 0, 1, 2, 3. Therefore, the indexes of the two <control> elements will be 1 and 3, respectively.

It’s also possible to use tags to address several controls at once. For example, if you have several <control> elements with the tag some-controls, you can use the following binding to address all of those controls in a single binding:

```xml
<binding type="control" level="ui" tags="some-controls" parameter="VISIBLE"␣
translation="fixed_value" translationValue="true"></binding>
```

## Effects Parameters

NOTE: The table below scrolls to the right.


|  | Description type level parameter Valid Mod- Additional required parame-Range ulat- tersable |
| --- | --- |
|  | Effect Enabled (all ef- effe instru ENABLED false, true Yes effectIndex, position, orfects) tags identifies the target ef-fect(s) |
|  | targ |
|  | Convolution Mix Level effe instru FX_MIX 0.0 - 1.0 Yes effectIndex, position, ortags identifies the target ef-fect(s) |
|  | Convolution IR File effe instru FX_IR_FILEText No effectIndex, position, ortags identifes the target ef-fect(s) |
|  | Filter Frequency(foeffe instru FX_FILTER_F 0.0 Yesseveral filters) 22000.0 |
|  | tags identifies |
|  | fect(s) |
|  | Peak or Notch Filter Q effe instru FX_FILTER_C 0.01 -18.0 Yes |
|  | tags identifies |
|  | fect(s) |
|  | Peak or Notch Filter effe instru FX_FILTER_c 0.0- 10.0 YesGain |
|  |  |
|  | fect(s) |
|  | Low-pass or High-pass effe instru FX_FILTER_R 0.0-5.0Filter Resonance |
|  | tags identifies |
|  | fect(s) |
|  | Reverb Wet Level effe i |
|  | tags |
|  | ft(s) |
|  | Reverb Room Size |
|  |  |
|  | fect(s) |
|  | Reverb Damping effe i |
|  |  |
|  | fect(s) |
|  | Chorus/Phaser/Pitch- effe instru FShift/Convolution MixLevel |
|  |  |
|  |  |
|  | Chorus/Phaser Modeffe iDepth |
|  |  |
|  |  |
|  | Chorus/Phaser Mod effe instru FX_MOD_RATE 0.0-10.0Rate |
|  | tags identifies |
|  | fect(s) |
|  | Phaser CenterFre-effe instru FX_CENTER_F 0.0 Yesquency 22000.0 |
| tags id | entifies |
| fect(s) |  |
| effectIndex, position, ortags identifies the target ef-Delay Time effe instru FX_DELAY_TI 0.0-1.0 Yes effectIndex, position, ortags identifies the target ef-Delay Time Format effe instru FX_DELAY_TI seconds, Yes effectIndex, position, ormusi- tags identifies the target ef-cal_time fect(s) |  |
|  | the target ef- |
|  |  |
| effectIndex, | pos |
| tects identiies | the target ef- |
| effectIndex, |  |
| 7Pelay Stereo Olffset effe instru FX_STERE0_C 0.0 -1.0 Yes effect1fpterosit8htentstags identifes the target effct(s)Delay Wet Level effe instru FX_WET_LEVE 0.0-1.0 Yes effectIndex, position, ortags identifies the target ef- |  |



## Bus-related Parameters

NOTE: The table below scrolls to the right.


| De- scrip- tion |  | type leve parameter |  | Valid Range | и- lat- able | Mod- Additional required parame- ters |
| --- | --- | --- | --- | --- | --- | --- |
| Bus Vol- ume | amp | bus bus | BUS_VOLUME OUTPUT_1_VOLUME | 0.0 - 16.0 0.0 - 1 | No No | busIndex or position con- tains the 0-based index of the bus busIndex or position con- |
| Bus Out- put 1 Vo1- ume | amp | bus |  |  |  | tains the 0-based index of the bus |
| Bus Out- put 2 Vo1- ume | amp |  | OUTPUT_2_VOLUME | 0.0 - 1 | No | busIndex or position con- tains the 0-based index of the bus |
| Bus Out- put 3 Vo1- | amp | bus | OUTPUT_3_VOLUME | 0.0 - 1 | No | busIndex or position con- tains the 0-based index of the bus |
| ume Bus Out- put 4 | amp | bus | OUTPUT_4_VOLUME | 0.0 - 1 | No | busIndex or position con- tains the 0-based index of the bus |
| Vol- ume Bus Out- put5 | amp | bus OUTPUT_5_VOLUME |  | 0.0 - 1 | No | busIndex or position con- tains the 0-based index of the bus |
| Vol- ume Bus Out- | amp | bus OUTPUT_6_VOLUME |  | 0.0 - 1 | No | busIndex or position con- tains the 0-based index of the bus |
| put 6 Vo1- ume Bus Out- put 7 | amp | bus OUTPUT_7_VOLUME |  | 0.0 - 1 | No | busIndex or position con- tains the 0-based index of the bus |
| Vo1- ume Bus Out- put 8 | amp | bus OUTPUT_8_VOLUME |  | 0.0 - 1 | No | busIndex or position con- tains the 0-based index of the bus |
| Vol- ume Bus Out- | amp | bus OUTPUT_1_TARGET |  | Any of the No |  | busIndex or position con- |
| put 1 Target Bus Out- | amp | bus OUTPUT_2_TARGET |  | valid values for output1Target Any of | the No | tains the 0-based index of the bus busIndex or position con- |
| put 2 7argett Bus | amp | bus OUTPUT_3_TARGET |  | valid values for output2Target Any of |  | tains the 0-based index of the bus |
| Out- put 3 Target |  |  |  | valid values for output3Target | the No | busIndexhosition on tains the 0-based index of the bus |



2. NOTE: If your sample library manipulates start, end, loopStart, or loopEnd after a sample library’s initial load, the sample playback engine must be in RAM/Memory mode (not disk streaming) or you will get very unpredictable results. In order to enforce this, sample creators should use the playbackMode attribute.

## 1.11.3 Translation Modes

There are currently three binding translation modes: linear, table, fixed_value

## Mode #1: linear

linear mode allows values that come in to be scaled up or down before they get passed along to the binding’s target. If you set your translation mode to linear you should also translationOutputMin and translationOutputMax.

Example usage:

<binding level="ui" type="control" position="0" parameter="value" translation="linear"   
translationOutputMin="0" translationOutputMax="1"/>

## Mode #2: table

table mode allows you to transform the binding’s input in a more complex fashion before it gets passed along to the binding’s target. If you set your translation mode to table you must define the translationTable parameter as well. This consists of a series of input-output pairs, separated by semi-colons.

## Mode #3: fixed_value

fixed_value mode allows you to completely disregard the input of a binding and instead always use a supplied value. In order to use this translation mode, you must also specify a translationValue. This can be very useful when trying to have menu options enable and disable groups. An example usage:

<binding type="general" level="group" position="1" parameter="ENABLED" translation=   
"fixed_value" translationValue="true" />

## 1.12 Appendix C: Boilerplate .dspreset File

```xml
<?xml version="1.0" encoding="UTF-8"?>
<DecentSampler minVersion="1.0.0">
<ui width="812" height="375" layoutMode="relative"
bgMode="top_left">
<tab name="main">
<labeled-knob x="445" y="75" width="90" textSize="16" textColor="AA000000"
trackForegroundColor="CC000000" trackBackgroundColor="66999999"
label="Attack" type="float" minValue="0.0" maxValue="4.0" value="0.01
 >
<binding type="amp" level="instrument" position="0" parameter="ENV_ATTACK" />
</labeled-knob>
<labeled-knob x="515" y="75" width="90" textSize="16" textColor="AA000000"
trackForegroundColor="CC000000" trackBackgroundColor="66999999"
```

(continues on next page)

```xml
label="Release" type="float" minValue="0.0" maxValue="20.0" value="1
" >
<binding type="amp" level="instrument" position="0" parameter="ENV_RELEASE" />
</labeled-knob>
<labeled-knob x="585" y="75" width="90" textSize="16" textColor="AA000000"
trackForegroundColor="CC000000" trackBackgroundColor="66999999"
label="Chorus" type="float" minValue="0.0" maxValue="1" value="0" >
<binding type="effect" level="instrument" position="1" parameter="FX_MIX" />
</labeled-knob>
<labeled-knob x="655" y="75" width="90" textSize="16" textColor="FF000000"
trackForegroundColor="CC000000" trackBackgroundColor="66999999"
label="Tone" type="float" minValue="0" maxValue="1" value="1">
<binding type="effect" level="instrument" position="0" parameter="FX_FILTER_
FREQUENCY"
translation="table"
translationTable="0,33;0.3,150;0.4,450;0.5,1100;0.7,4100;0.9,11000;1.
0001,22000"/>
</labeled-knob>
<labeled-knob x="725" y="75" width="90" textSize="16" textColor="AA000000"
trackForegroundColor="CC000000" trackBackgroundColor="66999999"
label="Reverb" type="percent" minValue="0" maxValue="100"
textColor="FF000000" value="50">
<binding type="effect" level="instrument" position="2"
parameter="FX_REVERB_WET_LEVEL" translation="linear"
translationOutputMin="0" translationOutputMax="1" />
</labeled-knob>
</tab>
</ui>
<groups attack="0.000" decay="25" sustain="1.0" release="0.430" volume="-3dB">
<group>
<sample loNote="21" hiNote="21" rootNote="21" path="DefaultPiano-21.aif"
length="805888"/>
<sample loNote="22" hiNote="33" rootNote="33" path="DefaultPiano-33.aif"
length="807552"/>
<sample loNote="34" hiNote="45" rootNote="45" path="DefaultPiano-45.aif"
length="759168"/>
<sample loNote="46" hiNote="57" rootNote="57" path="DefaultPiano-57.aif"
length="756480"/>
<sample loNote="58" hiNote="69" rootNote="69" path="DefaultPiano-69.aif"
length="758656"/>
<sample loNote="70" hiNote="77" rootNote="77" path="DefaultPiano-77.aif"
length="595328"/>
<sample loNote="78" hiNote="89" rootNote="89" path="DefaultPiano-89.aif"
length="457600"/>
<sample loNote="90" hiNote="96" rootNote="96" path="DefaultPiano-96.aif"
length="469888"/>
<sample loNote="94" hiNote="108" rootNote="108" path="DefaultPiano-108.aif"
length="75264"/>
</group>
</groups>
<effects>
<effect type="lowpass" frequency="22000.0"/>
```

(continues on next page)

```xml
<effect type="chorus" mix="0.0" modDepth="0.2" modRate="0.2" />
<effect type="reverb" wetLevel="0.5"/>
</effects>
<midi>
<!-- This causes MIDI CC 1 to control the 4th knob (cutoff) -->
<cc number="1">
<binding level="ui" type="control" parameter="VALUE" position="3"
translation="linear" translationOutputMin="0"
translationOutputMax="1" />
</cc>
</midi>
</DecentSampler>
```

## 1.13 Useful Tutorials and Resources

## 1.13.1 UI Elements

How to Color the Keys of the On-Screen Keyboard

![](data:image/jpeg;base64,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)

As of version 1.4.0, it is now possible to color the keys of the on-screen keyboard.

This can be useful for showing different ranges of notes that serve different purposes or highlighting notes used as keyswitches. In order to implement colored keys, make use of the new <keyboard> and <color> elements as follows:

<DecentSampler>   
<ui>   
<!-- Other stuffhere -->   
<keyboard>   
<color loNote="36" hiNote="50" color="FF2C365E" />

<color loNote="51" hiNote="57" color="FF6D9DC5" />   
<color loNote="58" hiNote="67" color="FFCCF3F5" />   
<color loNote="68" hiNote="73" color="FFE8DA9B" />   
<color loNote="74" hiNote="84" color="FFD19D61" />   
</keyboard>   
</ui>   
<!-- Other stuff here -->   
</DecentSampler>

By default, Decent Sampler highlights all of the notes that are mapped for a given sample. If you use the color keys feature, this default highlighting will be turned off and it will be up to you to color whatever keys you want.

Full documentation for the color element is here.

## How to turn keyboard note coloring on and off using bindings

It also possible to programmically turn keyboard coloring on and off using bindings. Here’s an example of how to do that:

```xml
<DecentSampler>
<ui>
<button name="Strum Enabled" x="464" y="82" width="14" height="14" style="image"␣
value="1" visible="false">
<state name="Off" mainImage="Images/StrumsCheckboxUnchecked.png" hoverImage=
"Images/StrumsCheckboxUnchecked.png" clickImage="Images/StrumsCheckboxUnchecked.png">
<binding level="ui" type="keyboard_color" colorIndex="0" parameter="ENABLED"
translation="fixed_value" translationValue="false"/>
<binding level="ui" type="keyboard_color" colorIndex="1" parameter="ENABLED"
translation="fixed_value" translationValue="false"/>
<binding level="ui" type="keyboard_color" colorIndex="2" parameter="ENABLED"␣
translation="fixed_value" translationValue="false"/>
<binding level="ui" type="keyboard_color" colorIndex="3" parameter="ENABLED"
translation="fixed_value" translationValue="false"/>
<binding level="ui" type="keyboard_color" colorIndex="4" parameter="ENABLED"␣
translation="fixed_value" translationValue="false"/>
</state>
<state name="On" mainImage="Images/StrumsCheckboxChecked.png" hoverImage="Images/
StrumsCheckboxChecked.png" clickImage="Images/StrumsCheckboxChecked.png">
<binding level="ui" type="keyboard_color" colorIndex="0" parameter="ENABLED"
translation="fixed_value" translationValue="true"/>
<binding level="ui" type="keyboard_color" colorIndex="1" parameter="ENABLED"␣
translation="fixed_value" translationValue="true"/>
<binding level="ui" type="keyboard_color" colorIndex="2" parameter="ENABLED"
translation="fixed_value" translationValue="true"/>
<binding level="ui" type="keyboard_color" colorIndex="3" parameter="ENABLED"
translation="fixed_value" translationValue="true"/>
<binding level="ui" type="keyboard_color" colorIndex="4" parameter="ENABLED"␣
translation="fixed_value" translationValue="true"/>
</state>
</button>
<keyboard>
<color loNote="36" hiNote="50" color="FF2C365E" />
```

(continues on next page)

```html
(continued from previous page)
<color loNote="51" hiNote="57" color="FF6D9DC5" />
<color loNote="58" hiNote="67" color="FFCCF3F5" />
<color loNote="68" hiNote="73" color="FFE8DA9B" />
<color loNote="74" hiNote="84" color="FFD19D61" />
</keyboard>
</ui>
<!- Other stuff here -->
</DecentSampler>
```

## How to add Dropdown Menus

## Dropdown Menus

In order to implemented dropdown menus, use the new <menu> and <option> elements. The <menu> element defines where the dropdown menu will show up in the ui, whereas the XML elements determine what menu options it has and what if anything those options actually do:

```xml
<menu x="10" y="40" width="120" height="30" value="2">
<option name="Menu Option 1">
<!-- Turn on this group -->
<binding type="general" level="group" position="0" parameter="ENABLED"
translation="fixed_value" translationValue="true" />
<!-- Turn off this group -->
<binding type="general" level="group" position="1" parameter="ENABLED"
translation="fixed_value" translationValue="false" />
</option>
<option name="Menu Option 2">
<!-- Turn off this group -->
<binding type="general" level="group" position="0" parameter="ENABLED"
translation="fixed_value" translationValue="false" />
<!-- Turn on this group -->
<binding type="general" level="group" position="1" parameter="ENABLED"
translation="fixed_value" translationValue="true" />
</option>
</menu>
```

In this example, a menu is being used to switch between two groups (the first menu option turns group 0 on and group 1 off; the section option turns group 0 off and group 1 on). Full documentation for the new <menu> and <option> elements is here.

## The new fixed_value translation type

You’ll note, in the example above, there’s something new in the bindings: the four bindings elements have a translation parameter of type fixed_value. This is a new translation type. Up until now, binding translation has strictly been about taking an input parameter (such as a knob value or continuous controller amount) and translating it so that it is useful for some other purpose (it’s our way of being able to do a little bit of math without having a full-blown scripting language). This new fixed_value binding is different. It ignores the input value completely and instead provides whatever is specified in the translationValue parameter. In this way, each menu option can have hardcoded values that it provides its bindings when it is selected.

## Customizing Menu Colors

You can customize the appearance of dropdown menus using the textColor, backgroundColor, highlightedTextColor, and highlightedBackgroundColor attributes:

```xml
<menu x="10" y="40" width="120" height="30" value="1"
textColor="FFFFFFFF" backgroundColor="FF333333"
highlightedTextColor="FF000000" highlightedBackgroundColor="FFCCCCCC">
<option name="Option 1">
<!-- binding code here
</option>
<option name="Option 2">
<!-- binding code here -->
</option>
</menu>
```

Attribute Description   
textColor A hex ARGB color value for the menu text (e.g., “FFFFFFFF” for white text)   
backgroundColor A hex ARGB color value for the menu background (e.g., “FF333333” for dark gray)   
highlightedTextColor A hex ARGB color value for the highlighted menu text (e.g., “FF000000” for black)   
highlightedBackgroundColorA hex ARGB color value for the highlighted menu background (e.g., “FFCCCCCC”   
for light gray)

## Text Alignment

You can control the text alignment within the menu using the vAlign and hAlign attributes, similar to labels:

<menu x="10" y="40" width="120" height="30" value="1"   
vAlign="center" hAlign="center">   
<option name="Centered Option">   
<!-- binding code here -->   
</option>   
</menu>


| Attribute | Valid Values | Description | Default |
| --- | --- | --- | --- |
| vAlign | "top","center", "bottom" | Vertical alignment of the menu text | "center" |
| hAlign | "left", "center", "right" | Horizontal alignment of the menu text | "left" |



## Dynamic Control via Bindings

All color attributes can also be controlled dynamically using bindings. The color values should be specified as 8- character hex strings in ARGB format (Alpha, Red, Green, Blue).

<!-- Example: Dynamic color control   
<control x="200" y="40" width="100" height="30">   
<binding type="control" level="ui" position="0" parameter="HIGHLIGHTED_TEXT_COLOR"   
translation="linear" translationOutputMin="FF000000"

(continues on next page)

translationOutputMax="FFFFFFFF" /   
</control>

(continued from previous page)

Available binding parameters for menu color control:

• TEXT_COLOR: Controls the menu text color

• BACKGROUND_COLOR: Controls the menu background color

• HIGHLIGHTED_TEXT_COLOR: Controls the highlighted menu text color

• HIGHLIGHTED_BACKGROUND_COLOR: Controls the highlighted menu background color

## How to Use Animations

As of version 1.12.9, DecentSampler has support for simple animations. You can use animations to create more compelling user interfaces. You can also use animations to create visual feedback for your controls. For example, a knob might control an animated visual element that rotates as the knob is turned.

Animations are defined using the <multiFrameImage> tag. This tag allows you to specify a series of images that will be displayed in sequence. It uses the same sprite sheet format that is used elsewhere in DecentSampler for defining knob skins. The <multiFrameImage> tag has the following attributes:

• x (required): The x position of your image where (0,0) is the top-left corner

• y (required): The y position of your image where (0,0) is the top-left corner

• width (required): The width in pixels of the image component

• height (required): The height in pixels of the image component

• path (required): The relative path of the image file to show in this component

• numFrames (required): The number of frames in the animation

• frameRate (required): The frame rate of the animation in frames per second. The maximum supported frame rate is 24 frames per second.

• sourceFormat (required): The orientation of the frames within the image strip. Valid values: horizontal_image_strip, vertical_image_strip.

• playbackMode (optional): The direction in which the animation should play. Valid values: forward_loop, forward_once, reverse_loop, reverse_once, ping_pong_loop (forth and back), and stopped. Default value is forward_loop.

• visible (optional): This controls whether or not this image is visible. There are two valid values: true (default), false.

• tooltip (optional): A tool tip to display when the user hovers over this image.

Here is an example of a simple animation that loops continuously:

```html
<DecentSampler pluginVersion="1">
<ui bgColor="FFADD8E6">
<tab>
<multiFrameImage x="350" y="80" width="64" height="64" path="Images/
AnimationDemo128.png" numFrames="31" sourceFormat="vertical_image_strip"frameRate="24"␣
playbackMode="forward_loop"/>
</tab>
```

</ui>   
</DecentSampler>

In this example, the image Images/AnimationDemo128.png is a vertical image strip with 31 frames. The animation will play at 24 frames per second in a forward loop. The image will be displayed at position (10,10) with a width of 64 pixels and a height of 64 pixels.

## Controlling Animations

## Current Frame

You can control the current frame of an animation using the currentFrame attribute. This attribute is a zero-based index that specifies the current frame of the animation. You can use this attribute to create animations that are controlled by other components. For example, you could use a knob to control the current frame of an animation.

Here is an example of an animation that is controlled by a knob:

```xml
<DecentSampler>
<ui bgColor="FFADD8E6">
<tab>
<multiFrameImage x="450" y="80" width="64" height="64" path="Images/
AnimationDemo128.png" numFrames="31" sourceFormat="vertical_image_strip"frameRate="24"␣
playbackMode="stopped"/>
<labeled-knob x="280" y="50" label="Frame" type="integer" minValue="0" maxValue="31
value="0" textColor="FF000000" value="0">
<binding type="control" level="ui" position="0" parameter="CURRENT_FRAME"␣
translation="linear" translationOutputMin="0" translationOutputMax="31" />
</labeled-knob>
</tab>
</ui>
</DecentSampler>
```

In this example, the knob controls the current frame of the animation. The binding tag is used to bind the knob to the CURRENT_FRAME parameter of the animation. The translation attribute specifies how the knob value should be translated to the animation frame. In this case, the knob value is linearly translated to the range 0-31, which corresponds to the 31 frames of the animation.

You can download a working example of this code here. This is Example 2 in the example-011-how-to-use-animations folder.

## Frame Rate

You can control the frame rate of an animation using the frameRate attribute. This attribute specifies the number of frames per second that the animation should play at. You can use this attribute to create animations that play at different speeds. For example, you could use a slider to control the frame rate of an animation.

Here is an example of an animation that is controlled by a slider:

```typescript
<DecentSampler>
<ui bgColor="FFADD8E6">
<tab>
<multiFrameImage x="450" y="80" width="64" height="64" path="Images/
```

(continues on next page)

```xml
(continued from previous page)
AnimationDemo128.png" numFrames="31" sourceFormat="vertical_image_strip"frameRate="24"␣
playbackMode="forward_loop"/>
<labeled-knob x="280" y="50" label="Frame Rate" type="integer" minValue="1"␣
maxValue="60" value="24" textColor="FF000000" value="24">
<binding type="control" level="ui" position="0" parameter="FRAME_RATE"␣
translation="linear" translationOutputMin="1" translationOutputMax="24" />
</labeled-knob>
</tab>
</ui>
</DecentSampler>
```

In this example, the slider controls the frame rate of the animation. The binding tag is used to bind the slider to the FRAME_RATE parameter of the animation. The translation attribute specifies how the slider value should be translated to the animation frame rate. In this case, the slider value is linearly translated to the range 1-24, which corresponds to the frame rates supported by DecentSampler.

You can download a working example of this code here. This is Example 3 in the example-011-how-to-use-animations folder.

## Playback Mode

You can control the playback mode of an animation using the playbackMode attribute. This attribute specifies how the animation should play. You can use this attribute to create animations that play in different ways. For example, you could use a drop-down menu to select the playback mode of an animation.

Here is an example of an animation that is controlled by a drop-down menu:

```xml
<DecentSampler>
<ui bgColor="FFADD8E6">
<tab>
<multiFrameImage x="450" y="80" width="64" height="64" path="Images/
AnimationDemo128.png" numFrames="31" sourceFormat="vertical_image_strip"frameRate="24"␣
playbackMode="forward_loop"/>
<label x="275" y="50" height="30" text="Playback Mode" textColor="FF000000"␣
textSize="16" />
<menu x="280" y="80" width="100" height="30">
<option name="Forward Loop">
<binding type="control" level="ui" position="0" parameter="PLAYBACK_MODE"
translation="fixed_value" translationValue="forward_loop"/>
</option>
<option name="Forward Once">
<binding type="control" level="ui" position="0" parameter="PLAYBACK_MODE"
translation="fixed_value" translationValue="forward_once"/>
</option>
<option name="Reverse Loop">
<binding type="control" level="ui" position="0" parameter="PLAYBACK_MODE"
translation="fixed_value" translationValue="reverse_loop"/>
</option>
<option name="Reverse Once">
<binding type="control" level="ui" position="0" parameter="PLAYBACK_MODE"
translation="fixed_value" translationValue="reverse_once"/>
</option>
```

(continues on next page)

```html
(continued from previous page)
<option name="Ping Pong Loop">
<binding type="control" level="ui" position="0" parameter="PLAYBACK_MODE"␣
translation="fixed_value" translationValue="ping_pong_loop"/>
</option>
<option name="Stopped">
<binding type="control" level="ui" position="0" parameter="PLAYBACK_MODE"␣
translation="fixed_value" translationValue="stopped"/>
</option>
</menu>
</tab>
</ui>
</DecentSampler>
```

In this example, the drop-down menu controls the playback mode of the animation. The binding tag is used to bind the menu to the PLAYBACK_MODE parameter of the animation. The translation attribute specifies how the menu value should be translated to the animation playback mode. In this case, the menu value is translated to one of the valid playback modes: forward_loop, forward_once, reverse_loop, reverse_once, ping_pong_loop, and stopped.

You can download a working example of this code here. This is Example 4 in the example-011-how-to-use-animations folder.

## Conclusion

In this guide, we have covered how to use animations in DecentSampler. You can use animations to create more compelling user interfaces and provide visual feedback for your controls. You can control animations using the currentFrame, frameRate, and playbackMode attributes. You can also bind animations to other components to create interactive animations.

## 1.13.2 Sample Mapping and Effects

## How to Use Buses and Auxiliary Outputs

As of version 1.12.0, Decent Sampler has support for both audio buses and auxiliary outputs. Buses are a powerful feature that allow you to route audio to different auxiliary outputs and apply effects to the audio. This can be useful for creating complex audio routing setups and adding effects to groups of samples. This tutorial will show you how to use buses and auxiliary outputs in Decent Sampler.

## Auxilary Outputs

By default, all audio in Decent Sampler is routed to the main audio output. However, you can also route audio to secondary,auxiliary outputs. Auxiliary outputs are additional audio outputs that can be used to route audio to external effects processors or other audio devices within your DAW. Decent Sampler supports up to 16 auxiliary outputs, which can be used to create complex audio routing setups.

There are two ways that you can route audio to auxiliary outputs in Decent Sampler:

1. You can use the outputXTarget attributes in the <sample>, <group>, or <groups> elements to directly specify the audio output that the sample should be routed to. The available options are MAIN_OUTPUT (the main audio output, which is the default) and AUX_STEREO_OUTPUT_1 through AUX_STEREO_OUTPUT_16 (the auxiliary outputs).

2. You can route audio from the samples to user-defined buses, and then route audio from the buses to the auxiliary outputs. This allows you to apply effects to the audio before sending it to the auxiliary outputs.

## Buses

As mentioned above, buses can be used to create a more complex mix of the samples in the sample library as well as route audio to various audio outputs. Sample library designers can specify up to 16 buses in the <buses> elemen using the <bus> tag. Each bus can have its own volume and audio output settings. For a full list of attributes that can be used in the <bus> element, see the Buses Element documentation.

## Examples

Here are a few examples of how you can use buses and auxiliary outputs in Decent Sampler:

## Example 1: Routing audio to an auxiliary output

In this example, we will route audio from a sample to an auxiliary output. To do this, we will use the outputXTarget attributes in the <sample> element to specify the audio output that the sample should be routed to. Here is an example of how you can route audio to AUX_STEREO_OUTPUT_1:

```xml
<group output1Target="MAIN_OUTPUT" output1Volume="1.0" output2Target="AUX_STEREO_OUTPUT_1
 output2Volume="0.5" />
<!-- Samples go here >
</group>
```

In this example, the audio from the samples in the <group> element will be routed to the main audio output with a volume of 1.0 and to auxiliary output 1 with a volume of 0.5.

## Example 2: Applying effects to audio using buses

In this example, we will apply an effect to audio using a bus. To do this, we will define a bus with an effect applied to it, and then route audio from the samples to the bus. Here is an example of how you can apply a reverb effect to audio using a bus:

```xml
<buses>
<bus busVolume="0.5" output1Target="MAIN_OUTPUT" output2Target="AUX_STEREO_OUTPUT_1"
output1Volume="0.8" output2Volume="0.5">
<effects>
<effect type="reverb" wetLevel="1"/>
</effects>
</bus>
</buses>
<group output1Target="MAIN_OUTPUT" output2Target="BUS_1" output2Volume="1.0">
<!-- Samples go here -->
</group>
```

In this example, a bus is defined with a reverb effect applied to it. The audio from the samples in the <group> element will be routed to the main audio output with a volume of 1.0, as well as to the bus with the reverb effect applied to it.

## Conclusion

Buses and auxiliary outputs are powerful features in Decent Sampler that allow you to create complex audio routing setups and apply effects to groups of samples. By using buses and auxiliary outputs, you can create more dynamic and interesting sample libraries that take full advantage of Decent Sampler’s capabilities.

## How to Use Note Sequences within your Sample Libraries

As of version 1.11.1, DecentSampler now supports embedding note sequences with your sample libraries. This means that you can create patters or musical motifs that can be triggered either using MIDI or via the UI. This guide will show you how to create a sample library with note sequences and how to trigger them using MIDI.

## Creating a sequence by hand

In this part of the guide, we will go over how to create a sequence by hand. This is useful if you want to create a sequence that is simple, and if you want the XML for your sequence to continue to be easily edited by hand. The first thing you need to do is add a <noteSequences> element to your <DecentSampler> file:

## The <noteSequences> element

The <noteSequences> element is how you specify note sequences that can be used by this playback engine. There should be exactly one <noteSequences> element in each <DecentSampler> file.

The <noteSequences> element can contain one or more <sequence> elements:

## The <sequence> element

The <sequence> element has the following attributes:

• name (optional): An optional descriptive name for the sequence. This is only used in the sample editor UI to help you identify the sequence.

• length (required): The length of the sequence in beats. This is a floating point number.

• rate (optional): The rate at which the sequence is played. This is a floating point number. The default is 1.0. The <sequence> element can contain one or more <note> elements:

## The <note> element

The <note> element has the following attributes:

• position (required): The position of the note in the sequence, in beats. This is a whole number.

• velocity (required): The velocity of the note. This is a floating point number between 0 and 1.

• note (required): The MIDI note number of the note.

• length (required): The length of the note in beats. This is a whole number.

An example sequence might looks like this:

```xml
<noteSequences>
<sequence name="Maj1Slow" length="16" rate="1">
<note position="0" velocity="1" note="48" length="16"/>
<note position="1" velocity="1" note="52" length="15"/>
<note position="2" velocity="1" note="55" length="14"/>
<note position="3" velocity="1" note="60" length="13"/>
<note position="4" velocity="1" note="64" length="12"/>
<note position="5" velocity="1" note="67" length="11"/>
<note position="6" velocity="1" note="72" length="10"/>
<note position="7" velocity="1" note="76" length="9"/>
<note position="8" velocity="1" note="79" length="8"/>
<note position="9" velocity="1" note="84" length="7"/>
<note position="10" velocity="1" note="88" length="6"/>
<note position="11" velocity="1" note="91" length="5"/>
</sequence>
</noteSequences>
```

## Creating a sequence using the sample editor

Another way to create sequences is via MIDI. While the Decent Sampler preset editor does not have a sequence editor built in, it does allow you to import MIDI files to create sequences. This means that you can use your favorite DAW to create sequences and then import them into Decent Sampler. This list of steps assumes that already have a preset created and that you are just looking to add sequences to it.

1. Create a MIDI sequence in your favorite DAW. Here is a sequence I created in Ableton Live:

| 1.1.3 1.2 1.2.3 1.3 1.3.3 1.4 1.4.3 |
| --- |
|  |
| Fold Scale |
| 0 |
|  |
|  |
|  |
|  |
| G#3 |
| C3 |
| − |
|  |
|  |
|  |
|  |
|  |
|  |
|  |



2. Export the sequence as a MIDI file. The command for doing this will be different in every DAW. In Ableton Live, you can do this by right-clicking on the MIDI clip and selecting Export MIDI Clip. . . .

3. Load your preset in Decent Sampler

4. Open the preset editor by going to File > Developer Tools > Preset Editor. . . . You can also get there by hitting F12.

5. Once you’re in the preset editor, choose the File > Sequence Manager. . . menu item. A little box will pop up. This is the sequence manager. It will be empty at first.

6. Click the Edit > Import MIDI File menu option, and select the MIDI file you created in step 2.

![](data:image/jpeg;base64,/9j/4AAQSkZJRgABAQAAAQABAAD/2wBDAAgGBgcGBQgHBwcJCQgKDBQNDAsLDBkSEw8UHRofHh0aHBwgJC4nICIsIxwcKDcpLDAxNDQ0Hyc5PTgyPC4zNDL/2wBDAQkJCQwLDBgNDRgyIRwhMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjL/wAARCAJ6BJADASIAAhEBAxEB/8QAHwAAAQUBAQEBAQEAAAAAAAAAAAECAwQFBgcICQoL/8QAtRAAAgEDAwIEAwUFBAQAAAF9AQIDAAQRBRIhMUEGE1FhByJxFDKBkaEII0KxwRVS0fAkM2JyggkKFhcYGRolJicoKSo0NTY3ODk6Q0RFRkdISUpTVFVWV1hZWmNkZWZnaGlqc3R1dnd4eXqDhIWGh4iJipKTlJWWl5iZmqKjpKWmp6ipqrKztLW2t7i5usLDxMXGx8jJytLT1NXW19jZ2uHi4+Tl5ufo6erx8vP09fb3+Pn6/8QAHwEAAwEBAQEBAQEBAQAAAAAAAAECAwQFBgcICQoL/8QAtREAAgECBAQDBAcFBAQAAQJ3AAECAxEEBSExBhJBUQdhcRMiMoEIFEKRobHBCSMzUvAVYnLRChYkNOEl8RcYGRomJygpKjU2Nzg5OkNERUZHSElKU1RVVldYWVpjZGVmZ2hpanN0dXZ3eHl6goOEhYaHiImKkpOUlZaXmJmaoqOkpaanqKmqsrO0tba3uLm6wsPExcbHyMnK0tPU1dbX2Nna4uPk5ebn6Onq8vP09fb3+Pn6/9oADAMBAAIRAxEAPwD27XdatvD2jXGqXaSvBBt3LEAWO5goxkgdSO9cX/wubw7/AM+Wqf8AfqP/AOLrX+J3/JO9V/7Y/wDo1K+dqiTaZEm0z3P/AIXL4dxn7Fqn/fqP/wCLpP8Ahc3h3/ny1T/v1H/8XXhx+4PrSUuZi5me5f8AC5vDv/Plqn/fqP8A+LpR8ZfDpGfsWqf9+o//AIuvDKVfumjmYczPcf8Ahc3h3/ny1T/v1H/8XR/wubw7/wA+Wqf9+o//AIuvDaKOZhzM9zHxm8Ok/wDHlqn/AH6j/wDi6P8Ahc3h3/ny1T/v1H/8XXhqfeFIetHMw5me5f8AC5vDv/Plqn/fqP8A+Lo/4XN4d/58tU/79R//ABdeG0UczDmZ7mfjL4dB/wCPLVP+/Uf/AMXSf8Lm8O/8+Wqf9+o//i68Ob71JRzMOZnuX/C5vDv/AD5ap/36j/8Ai6X/AIXL4dxn7Fqn/fqP/wCLrwynH7oo5mHMz3D/AIXN4d/58tU/79R//F0f8Ln8O/8APlqn/fqP/wCLrw2kNHMw5me6D4y+HSCfsWqcf9Mo/wD4uk/4XN4d/wCfLVP+/Uf/AMXXh6/dam0czDmZ7l/wubw7/wA+Wqf9+o//AIumyfGnw5Ghc2Wq4HpFH/8AF14fUNz/AMez/Sk5uwuZnu6/Gjw4yhhZargjP+qj/wDi6X/hc3h3/ny1T/v1H/8AF14TF/qk+gqShTdg5me5f8Lm8O/8+Wqf9+o//i6U/GXw6P8Aly1T/v1H/wDF14YOopW60+Zj5me4/wDC5vDv/Plqn/fqP/4uj/hc3h3/AJ8tU/79R/8AxdeG0UczDmZ7p/wuTw7t3fYtUx/1yj/+Lpv/AAubw7/z5ap/36j/APi68QH+pNMo5mHMz3L/AIXN4d/58tU/79R//F0v/C5fDuM/YtU/79R//F14ZSj7po5mHMz3H/hc3h3/AJ8tU/79R/8AxdH/AAubw7/z5ap/36j/APi68Noo5mHMz3MfGXw6f+XLVP8Av1H/APF0f8Lm8O/8+Wqf9+o//i68NXrSHrRzMOZnuX/C5vDv/Plqn/fqP/4uj/hc3h3/AJ8tU/79R/8AxdeG0DrRzMOZnuZ+Mvh0HH2LVP8Av1H/APF0n/C5vDv/AD5ap/36j/8Ai68Pf7xptHMw5me5f8Lm8O/8+Wqf9+o//i6U/GXw6B/x5ap/36j/APi68MpW6CjmYczPcf8Ahc3h3/ny1T/v1H/8XR/wubw7/wA+Wqf9+o//AIuvDaKOZhzM9z/4XL4dxn7Fqn/fqP8A+LpP+FzeHf8Any1T/v1H/wDF14ePumm0czDmZ7l/wubw7/z5ap/36j/+LpR8ZfDp/wCXLVP+/Uf/AMXXhlKnWjmYczPcf+FzeHf+fLVP+/Uf/wAXR/wubw7/AM+Wqf8AfqP/AOLrw2ijmYczPc/+FzeHf+fLVP8Av1H/APF0H4zeHQcfYtU/79R//F14YOtK33qOZhzM9x/4XN4d/wCfLVP+/Uf/AMXR/wALm8O/8+Wqf9+o/wD4uvDaKOZhzM9zPxl8Oj/ly1T/AL9R/wDxdJ/wubw7/wA+Wqf9+o//AIuvD26Cm0czDmZ7l/wufw7/AM+Wqf8AfqP/AOLpf+Fy+HcZ+xap/wB+o/8A4uvCzTx9w/WjmYczPcP+FzeHf+fLVP8Av1H/APF0f8Lm8O/8+Wqf9+o//i68Noo5mHMz2+X41+G4k3NZaqR7RR//ABynj4z+HCAfsWq/9+o//i68FvP+Pc/UVMv3B9Knndxc7ue6f8Lm8O/8+Wqf9+o//i6UfGbw6Tj7Fqn/AH6j/wDi68MoH3hVczHzM9zPxm8Oj/ly1T/v1H/8XSf8Lm8O/wDPlqn/AH6j/wDi68ObrSUczDmZ7l/wubw7/wA+Wqf9+o//AIunf8Lk8O7d32LVMf8AXKP/AOLrwunn/Uj60czDmZ7f/wALm8O/8+Wqf9+o/wD4uj/hc3h3/ny1T/v1H/8AF14bRRzMOZnuf/C5fDuM/YtU/wC/Uf8A8XSf8Lm8O/8APlqn/fqP/wCLrw7+A0lHMw5me5f8Lm8O/wDPlqn/AH6j/wDi6UfGXw6f+XLVP+/Uf/xdeGUq96OZhzM9x/4XN4d/58tU/wC/Uf8A8XR/wubw7/z5ap/36j/+Lrw2ijmYczPcx8ZvDpOPsWqf9+o//i6D8ZvDoOPsWqf9+o//AIuvDF+9+NK33jRzMOZnuP8Awubw7/z5ap/36j/+Lo/4XN4d/wCfLVP+/Uf/AMXXhtFHMw5me5n4y+HRj/QtU/79R/8AxdJ/wubw7/z5ap/36j/+Lrw5u1JRzMOZnuX/AAubw7/z5ap/36j/APi6X/hcvh3GfsWqf9+o/wD4uvDKd/BRzMOZnuH/AAubw7/z5ap/36j/APi6P+FzeHf+fLVP+/Uf/wAXXhtIaOZhzM90Hxl8On/ly1T/AL9R/wDxdJ/wubw7/wA+Wqf9+o//AIuvD07/AEptHMw5me5f8Lm8O/8APlqn/fqP/wCLpR8ZvDpP/Hlqn/fqP/4uvDKVfvCjmYczPcv+FzeHc4+xap/36j/+LpP+FzeHf+fLVP8Av1H/APF14afvGijmYczPcv8Ahc3h3/ny1T/v1H/8XSn4y+HR/wAuWqf9+o//AIuvDKc3ajmYczPcP+FzeHf+fLVP+/Uf/wAXR/wufw7/AM+Wqf8AfqP/AOLrw2ko5mHMz3T/AIXL4dxn7Fqn/fqP/wCLpP8Ahc3h3/ny1T/v1H/8XXh//LP8abRzMOZnuX/C5vDv/Plqn/fqP/4umSfGvw3EAWstV5OOIo//AI5XiFVbz7if71JzaQnNnvn/AAubw7/z5ap/36j/APi6P+FzeHf+fLVP+/Uf/wAXXhnelo5mPmZ7mPjN4dJx9i1T/v1H/wDF0H4zeHR/y5ap/wB+o/8A4uvDV+9Qepp8zDmZ7j/wubw7/wA+Wqf9+o//AIuj/hc3h3/ny1T/AL9R/wDxdeG0UczDmZ7ofjJ4eCg/Y9U5/wCmUf8A8XTf+FzeHf8Any1T/v1H/wDF14i3+qWo6OZhzM9y/wCFzeHf+fLVP+/Uf/xdL/wuXw7jP2LVP+/Uf/xdeGUp+5RzMOZnuP8Awubw7/z5ap/36j/+Lo/4XN4d/wCfLVP+/Uf/AMXXhtFHMw5me5j4y+HT/wAuWqf9+o//AIuk/wCFzeHf+fLVP+/Uf/xdeHr0NNo5mHMz3L/hc3h3/ny1T/v1H/8AF0o+M3h0nH2LVP8Av1H/APF14ZSp94UczDmZ7l/wubw7/wA+Wqf9+o//AIuk/wCFzeHf+fLVP+/Uf/xdeGnqaKOZhzM9y/4XN4d/58tU/wC/Uf8A8XSn4y+HR/y5ap/36j/+LrwylbqPpRzMOZnuP/C5vDv/AD5ap/36j/8Ai6P+FzeHf+fLVP8Av1H/APF14bRRzMOZnuf/AAuXw7jP2LVP+/Uf/wAXSf8AC5vDv/Plqn/fqP8A+Lrw8/dFNo5mHMz3L/hc3h3/AJ8tU/79R/8AxdKPjL4dIJ+xapx/0yj/APi68LNPX7rfSjmYczPcP+FzeHf+fLVP+/Uf/wAXR/wubw7/AM+Wqf8AfqP/AOLrw2ijmYczPcx8ZfDpOPsWqf8AfqP/AOLpP+FzeHf+fLVP+/Uf/wAXXhy/epD1NHMw5me5f8Lm8O/8+Wqf9+o//i6P+FzeHf8Any1T/v1H/wDF14bR3FHMw5me5n4y+HR/y5ap/wB+o/8A4uk/4XN4d/58tU/79R//ABdeHN1pKOZhzM9y/wCFz+Hf+fLVP+/Uf/xdL/wuXw7jP2LVP+/Uf/xdeF96efuCjmYczPcP+FzeHf8Any1T/v1H/wDF0f8AC5vDv/Plqn/fqP8A+Lrw2ijmYczPb3+NfhuMqDZar8xwMRR//HKf/wALn8O/8+Wqf9+o/wD4uvBLn/WQf71WKSm7iU2e5/8AC5vDv/Plqn/fqP8A+LpR8ZvDpP8Ax5ap/wB+o/8A4uvDKVfvU+Zj5me5f8Lm8O/8+Wqf9+o//i6T/hc3h3/ny1T/AL9R/wDxdeGnrRRzMOZnuX/C5vDv/Plqn/fqP/4unH4yeHVAJs9U5/6ZR/8AxdeF0+T7qUczDmZ9DfE7/kneq/8AbH/0alfO1fRPxO/5J3qv/bH/ANGpXztRPcJ7in7g+tJSn7g+tJUkhSr900lKv3TQAlFFJQA5PvCkPWlXhhSE8mgAopM0ZoAc33qSlfrTc0ALTj90UzNOJ+UUAJSGjNFAD1+61NpVPymm5oAWobn/AI9n+lS1Fc/8ez/Sk9hMdF/qk+gqSo4v9Sn0FPzQtgQo6ilbrSDqKVjzTGJRSZozQBIP9SaZTx/qTUeaAFpR9003NOH3TQAlFJmjNADl60h60q9TSE80AFA60maUdaAHP9402lc/NTc0ALSt0FNzTm+6KAEopM0ZoAePumm0oPyU3NAC0qdabmnJ96gBKKTNGaAFHWlb71IDzSt96gBKKTNGaAHt0FNpWPApuaAA08fcP1plOB+Q0AJRSZooAgvP+Pc/UVMv3B9KhvP+Pc/UVMv3V+lStxLcdQPvCkzSr96qGK3WkoY80maAFp5/1I+tR5qQ/wCpH1oAZRSZozQA7+A0lL/AabmgBaVe9NzTl70AJRSZozQAq/e/Glb7xpF6ilY/MaAEopM0ZoAc3akpW6Cm5oAWnfwUzNOz8lACUhozRQA9O/0ptKh6/Sm5oAWlX7wpuaVTyKAA/eNFB+8aTNAC05u1MpzHpQAlJRmigB//ACz/ABptLn5PxpuaAFqrefcT/eqzVa8+4n+9Uy2Eyz3paTvRmqWwxy/eoPU0i/eoJ5NABRSZooAlb/VLUdPb/VLUeaAFpT9ym5px+5QAlFJmjNAD16Gm0q9DTc0ALSp94U3NKvDUAB6mignk0maAFpW6j6U3NObqPpQAlFJmjNADz90U2lJ+UU3NAAaev3W+lMpyng/SgBKKTNGaAHL96kPU0qn5qQ9TQAUdxSZpR1oAVutJSseabmgA708/cFMpxPyCgBKKTNFAFe5/1kH+9Viq91/rIP8Aeqx3qVuJC0q/epuacnWqGIetFB60UAFPk+6lMp8n3UoA+hvid/yTvVf+2P8A6NSvnavon4nf8k71X/tj/wCjUr52qp7lT3FP3B9aSlP3B9aQ/WpJCtTRdA1HXZ/JsYGf1bHC/U1N4X8O3HiTV0tYsiNfmlf0WvoTSNHs9DsEtLKFY0UfMQOWPqapK5SVzz7Svg/bqivqV6zuRykY4/OtyP4V+GFXD20zH185h/Wu1oq7IuyONHwu8LD/AJdJv+/7f40n/CrfC3/PpN/3/b/GuzoosgsjjP8AhVvhb/n0m/7/ALf40f8ACrfC3/PpN/3/AG/xrs6KLILI40/C7wsf+XSb/v8At/jSf8Kt8Lf8+k3/AH/b/GuzoosgsjjP+FW+Fv8An0m/7/t/jS/8Ku8Lf8+k3/f9v8a7KiiyCyOM/wCFW+Fv+fSb/v8At/jR/wAKt8Lf8+k3/f8Ab/Guzoosgsjjf+FXeFsf8ek3/f8Ab/Gk/wCFW+Fv+fSb/v8At/jXZ0UWQWRxn/CrfC3/AD6Tf9/2/wAaa/wr8KOhU2c2D/08N/jXa0h6UcqCyOMX4WeFVUAWc3Ax/wAfD/40v/CrfC3/AD6Tf9/2/wAa7LtS0JILI4z/AIVd4W/59Jv+/wC3+NL/AMKu8LE/8ek3/f8Ab/GuyoosgsjjP+FW+Fv+fSb/AL/t/jR/wq3wr/z6Tf8Af9v8a7OiiyCyON/4Vf4W24+xzf8Af9v8aT/hVvhb/n0m/wC/7f412dFFkFkcZ/wq3wt/z6Tf9/2/xpf+FXeFgP8Aj0m/7/t/jXZUUWQWRxn/AAq3wtn/AI9Jv+/7f40f8Kt8Lf8APpN/3/b/ABrs6KLILI4wfC7wsP8Al0m/7/t/jR/wq7wt/wA+k3/f9v8AGuzoosgsjjP+FW+Fv+fSb/v+3+NH/CrfC3/PpN/3/b/GuzoosgsjjT8LvCx/5dJv+/7f40n/AAq3wt/z6Tf9/wBv8a7OiiyCyOM/4Vb4W/59Jv8Av+3+NL/wq7wt/wA+k3/f9v8AGuyoosgsjjP+FW+Fv+fSb/v+3+NH/CrfC3/PpN/3/b/GuzoosgsjjP8AhV3hbGPsk3/f9v8AGj/hVvhb/n0m/wC/7f412dFFkFkcZ/wq3wt/z6Tf9/2/xpR8LvCw/wCXSb/v+3+NdlRRZBZHGf8ACrfC3/PpN/3/AG/xo/4Vb4W/59Jv+/7f412dFFkFkcZ/wq7wt/z6Tf8Af9v8aD8LvCx/5dJv+/7f412dFFkFkcZ/wq3wt/z6Tf8Af9v8aP8AhVvhb/n0m/7/ALf412dFFkFkcafhd4WP/LpN/wB/2/xpP+FW+Fv+fSb/AL/t/jXZ0UWQWRxn/CrfC3/PpN/3/b/Gl/4Vd4Wxj7JN/wB/2/xrsqKLILI4z/hVvhb/AJ9Jv+/7f40f8Kt8Lf8APpN/3/b/ABrs6KLILI4mX4VeFJE2tZzY/wCvh/8AGnD4W+FQP+POb/wIf/GuzPSlpcquFlc4z/hVvhb/AJ9Jv+/7f40f8Ku8LA/8ek3/AH/b/Guzop2QWRxv/CrvCx/5dJv+/wC3+NJ/wq3wt/z6Tf8Af9v8a7OiiyCyOM/4Vb4V/wCfSb/v+3+NL/wq/wALbcfY5v8Av+3+NdlRRZBZHGf8Kt8K/wDPpN/3/b/Gj/hVvhb/AJ9Jv+/7f412dFFkFkcb/wAKu8LYx9km/wC/7f40n/CrfC3/AD6Tf9/2/wAa7OiiyCyOM/4Vb4W/59Jv+/7f40D4XeFh/wAuk3/f9v8AGuzoosgsjjP+FW+Fv+fSb/v+3+NH/CrfC3/PpN/3/b/GuzoosgsjjR8LvCw/5dJv+/7f40f8Ku8Lf8+k3/f9v8a7KiiyCyOM/wCFW+Fv+fSb/v8At/jR/wAKt8Lf8+k3/f8Ab/GuzoosgsjjT8LvCx/5dJv+/wC3+NJ/wq3wt/z6Tf8Af9v8a7OiiyCyOM/4Vb4W/wCfSb/v+3+NL/wq7wtjH2Sb/v8At/jXZUUWQWRxn/CrfC3/AD6Tf9/2/wAaP+FW+Fv+fSb/AL/t/jXZ0UWQWRxo+F3hYf8ALpN/3/b/ABpP+FW+Fv8An0m/7/t/jXZ0UWQWRxn/AAq3wt/z6Tf9/wBv8aX/AIVd4WH/AC6Tf9/2/wAa7KiiyCyOMPwu8LH/AJdJv+/7f40f8Kt8Lf8APpN/3/b/ABrs6KLILI4z/hVvhb/n0m/7/t/jS/8ACrvCx/5dJv8Av+3+NdlRRZBZHGf8Kt8Lf8+k3/f9v8aP+FW+Fv8An0m/7/t/jXZ0UWQWRxv/AAq7wtjH2Sb/AL/t/jSf8Kt8Lf8APpN/3/b/ABrs6KLILI4z/hV3hb/n0m/7/t/jTJPhV4UkABs5uDn/AI+H/wAa7Wg0OKBpHG/8Kt8K/wDPnN/4EP8A40f8Kt8Lf8+k3/f9v8a7OihJBZHGD4XeFgf+PSb/AL/t/jR/wq7wt/z6Tf8Af9v8a7OiiyCyOM/4Vb4W/wCfSb/v+3+NH/CrfCv/AD6Tf9/2/wAa7OiiyCyONPwv8LEAfY5v+/7f40n/AAq3wt/z6Tf9/wBv8a7OiiyCyOM/4Vb4W/59Jv8Av+3+NL/wq7wtjH2Sb/v+3+NdlRRZBZHGf8Kt8Lf8+k3/AH/b/Gj/AIVb4W/59Jv+/wC3+NdnRRZBZHGj4XeFsH/RJv8Av+3+NJ/wq3wt/wA+k3/f9v8AGuzoosgsjjP+FW+Fv+fSb/v+3+NKPhd4WH/LpN/3/b/GuyoosgsjjP8AhV3hb/n0m/7/ALf40f8ACrfC3/PpN/3/AG/xrs6KLILI4z/hVvhb/n0m/wC/7f40p+F3hY/8uk3/AH/b/GuyoosgsjjP+FW+Fv8An0m/7/t/jR/wq3wt/wA+k3/f9v8AGuzoosgsjjf+FXeFsD/RJv8Av+3+NJ/wq3wt/wA+k3/f9v8AGuzoosgsjjP+FW+Fv+fSb/v+3+NL/wAKu8LY/wCPSb/v+3+NdlRRZBZHGf8ACrfC3/PpN/3/AG/xo/4Vb4W/59Jv+/7f412dFFkFkcaPhd4WB/49Jv8Av+3+NJ/wq7wt/wA+k3/f9v8AGuzoosgsjjP+FW+Fv+fSb/v+3+NH/CrvC3/PpN/3/b/Guzoosgsjjf8AhV3hYn/j0m/7/t/jSf8ACrfC3/PpN/3/AG/xrs6KLILI4z/hVvhb/n0m/wC/7f40v/CrvC2P+PSb/v8At/jXZUUWQWRxn/CrfC3/AD6Tf9/2/wAaP+FXeFv+fSb/AMCG/wAa7OiiyCyOJf4VeFHKE2c3ynI/0h/8aV/hZ4XK4W2mU+vnsf612hpaXKgsjzPUvhBYyIW0+7kicDhX5H515xrfhbVPD1xsvID5eTtlX7rV9J1Vv9PttTs5LW7iWWJxyrChxE4ny3nmiul8aeFZvDOqlQGazlJMLn+VczWdrGdrC0+T7qUynyfdSgD6G+J3/JO9V/7Y/wDo1K+dq+ifid/yTvVf+2P/AKNSvnaqnuVPcU/cH1pppx+4PrSGpJPdvhjoaab4aS8ZP39385J6gdMfpXcVQ0WEQaJZRjoIV6fTNX61Rqtgoo/CsrWvEeleHkhfVLoW6zEhCVJzjr0HvTGatFcn/wALL8Jf9BdP+/b/AOFH/Cy/CX/QXT/v2/8AhQB1lFcn/wALL8Jf9BdP+/b/AOFH/Cy/CX/QXT/v2/8AhQB1lFcn/wALL8Jf9BdP+/b/AOFH/Cy/CX/QXT/v2/8AhQB1lFcn/wALL8Jf9BdP+/b/AOFH/Cy/CX/QXT/v2/8AhQB1lFcn/wALL8Jf9BdP+/b/AOFH/Cy/CX/QXT/v2/8AhQB1lFcn/wALL8Jf9BdP+/b/AOFH/Cy/CX/QXT/v2/8AhQB1lIelcp/wsvwl/wBBdP8Av2/+FIfiX4S/6C6f9+3/AMKAOsHSlrkv+Fl+Ev8AoLp/37f/AApf+Fl+Ev8AoLp/37f/AAoA6yiuT/4WX4S/6C6f9+3/AMKP+Fl+Ev8AoLp/37f/AAoA6yiuT/4WX4S/6C6f9+3/AMKP+Fl+Ev8AoLp/37f/AAoA6yiuT/4WX4S/6C6f9+3/AMKP+Fl+Ev8AoLp/37f/AAoA6yiuT/4WX4S/6C6f9+3/AMKP+Fl+Ev8AoLp/37f/AAoA6yiuT/4WX4S/6C6f9+3/AMKP+Fl+Ev8AoLp/37f/AAoA6yiuT/4WX4S/6C6f9+3/AMKP+Fl+Ev8AoLp/37f/AAoA6yiuT/4WX4S/6C6f9+3/AMKP+Fl+Ev8AoLp/37f/AAoA6yiuT/4WX4S/6C6f9+3/AMKP+Fl+Ev8AoLp/37f/AAoA6yiuT/4WX4S/6C6f9+3/AMKP+Fl+Ev8AoLp/37f/AAoA6yiuT/4WX4S/6C6f9+3/AMKP+Fl+Ev8AoLp/37f/AAoA6yiuT/4WX4S/6C6f9+3/AMKP+Fl+Ev8AoLp/37f/AAoA6yiuT/4WX4S/6C6f9+3/AMKP+Fl+Ev8AoLp/37f/AAoA6yiuT/4WX4S/6C6f9+3/AMKP+Fl+Ev8AoLp/37f/AAoA6yiuT/4WX4S/6C6f9+3/AMKP+Fl+Ev8AoLp/37f/AAoA6yiuT/4WX4S/6C6f9+3/AMKP+Fl+Ev8AoLp/37f/AAoA6yiuT/4WX4S/6C6f9+3/AMKP+Fl+Ev8AoLp/37f/AAoA6yiuT/4WX4S/6C6f9+3/AMKP+Fl+Ev8AoLp/37f/AAoA6yiuT/4WX4S/6C6f9+3/AMKP+Fl+Ev8AoLp/37f/AAoA6s9KWuSPxL8Jf9BdP+/b/wCFL/wsvwl/0F0/79v/AIUAdZRXJ/8ACy/CX/QXT/v2/wDhR/wsvwl/0F0/79v/AIUAdZRXJ/8ACy/CX/QXT/v2/wDhR/wsvwl/0F0/79v/AIUAdZRXJ/8ACy/CX/QXT/v2/wDhR/wsvwl/0F0/79v/AIUAdZRXJ/8ACy/CX/QXT/v2/wDhR/wsvwl/0F0/79v/AIUAdZRXJ/8ACy/CX/QXT/v2/wDhR/wsvwl/0F0/79v/AIUAdZRXJ/8ACy/CX/QXT/v2/wDhR/wsvwl/0F0/79v/AIUAdZRXJ/8ACy/CX/QXT/v2/wDhR/wsvwl/0F0/79v/AIUAdZRXJ/8ACy/CX/QXT/v2/wDhR/wsvwl/0F0/79v/AIUAdZRXJ/8ACy/CX/QXT/v2/wDhR/wsvwl/0F0/79v/AIUAdZRXJ/8ACy/CX/QXT/v2/wDhR/wsvwl/0F0/79v/AIUAdZRXJ/8ACy/CX/QXT/v2/wDhR/wsvwl/0F0/79v/AIUAdZRXJ/8ACy/CX/QXT/v2/wDhR/wsvwl/0F0/79v/AIUAdZRXJ/8ACy/CX/QXT/v2/wDhR/wsvwl/0F0/79v/AIUAdZRXJ/8ACy/CX/QXT/v2/wDhR/wsvwl/0F0/79v/AIUAdZRXJ/8ACy/CX/QXT/v2/wDhR/wsvwl/0F0/79v/AIUAdZRXJ/8ACy/CX/QXT/v2/wDhR/wsvwl/0F0/79v/AIUAdZRXJ/8ACy/CX/QXT/v2/wDhR/wsvwl/0F0/79v/AIUAdZRXJ/8ACy/CX/QXT/v2/wDhR/wsvwl/0F0/79v/AIUAdZSGuU/4WX4S/wCgun/ft/8ACkPxK8Jf9BdP+/b/AOFAHW0Vyf8Awsvwl/0F0/79v/hR/wALL8Jf9BdP+/b/AOFAHWUVyf8Awsvwl/0F0/79v/hR/wALL8Jf9BdP+/b/AOFAHWUVyf8Awsvwl/0F0/79v/hR/wALL8Jf9BdP+/b/AOFAHWUVyf8Awsvwl/0F0/79v/hR/wALL8Jf9BdP+/b/AOFAHWUVyf8Awsvwl/0F0/79v/hR/wALL8Jf9BdP+/b/AOFAHWUVyf8Awsvwl/0F0/79v/hR/wALL8Jf9BdP+/b/AOFAHWUVyf8Awsvwl/0F0/79v/hR/wALL8Jf9BdP+/b/AOFAHWUVyf8Awsvwl/0F0/79v/hR/wALL8Jf9BdP+/b/AOFAHWUVyf8Awsvwl/0F0/79v/hR/wALL8Jf9BdP+/b/AOFAHWUVyf8Awsvwl/0F0/79v/hR/wALL8Jf9BdP+/b/AOFAHWUVyf8Awsvwl/0F0/79v/hR/wALL8Jf9BdP+/b/AOFAHWUVyf8Awsvwl/0F0/79v/hR/wALL8Jf9BdP+/b/AOFAHWUVyf8Awsvwl/0F0/79v/hR/wALL8Jf9BdP+/b/AOFAHWUVyf8Awsvwl/0F0/79v/hR/wALL8Jf9BdP+/b/AOFAHWUVyf8Awsvwl/0F0/79v/hR/wALL8Jf9BdP+/b/AOFAHWUVyf8Awsvwl/0F0/79v/hR/wALL8Jf9BdP+/b/AOFAHWUVyf8Awsvwl/0F0/79v/hR/wALL8Jf9BdP+/b/AOFAHWUVyf8Awsvwl/0F0/79v/hR/wALL8Jf9BdP+/b/AOFAHWUVyf8Awsvwl/0F0/79v/hR/wALL8Jf9BdP+/b/AOFAHVntS1yR+JfhL/oLJ/37f/CtTRvFei+IJ5YdLvVuJIlDOAjDAzjuKANmiiigDl/HujJrHhe5GzMsKmRD34HP8q+eSCCQeMda+qbiITQSRHo6kV8uXq7L+5XsJWH61nMzkQVJJ91KZT5PupUkn0N8Tv8Akneq/wDbH/0alfO1fRPxO/5J3qv/AGx/9GpXztVT3KnuKfuD60lKfuD60hqST6k03/kF2n/XFP8A0EVaqrp3/IMtP+uKf+girVbGy2E715R8b/8Ajx0f/rrL/Ja9Yryf43/8eOjf9dJf5LQB41RRWnZ6R9rtlm87bnIxtoAzKK2v7A/6eP8Ax2j/AIR//p4/8doAxaK2v+Ef/wCnj/x2j/hH/wDp4/8AHaAMWitr/hH/APp4/wDHaP8AhH/+nj/x2gDFora/4R//AKeP/HaP+Ef/AOnj/wAdoAxaK2v+Ef8A+nj/AMdo/wCEf/6eP/HaAMWitr/hH/8Ap4/8do/4R/8A6eP/AB2gDFora/4R/wD6eP8Ax2j/AIR//p4/8doAxaK2v+Ef/wCnj/x2j/hH/wDp4/8AHaAMWitr/hH/APp4/wDHaP8AhH/+nj/x2gDFora/4R//AKeP/HaP+Ef/AOnj/wAdoAxaK2v+Ef8A+nj/AMdo/wCEf/6eP/HaAMWitr/hH/8Ap4/8do/4R/8A6eP/AB2gDFora/4R/wD6eP8Ax2j/AIR//p4/8doAxaK2v+Ef/wCnj/x2j/hH/wDp4/8AHaAMWitr/hH/APp4/wDHaP8AhH/+nj/x2gDFora/4R//AKeP/HaP+Ef/AOnj/wAdoAxaK0L/AEs2UKyebuycdKzqAFooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKAEpaKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAT616l8Ev+Q3qf8A17L/AOhV5dXqPwS/5Deqf9ey/wDoVAHtlFFFAAa+WdQ/5Cd3/wBdn/ma+pq+WtQ/5Cd3/wBdn/maiREirUkn3VqOpJPurUEH0N8Tv+Sd6r/2x/8ARqV87V9E/E7/AJJ3qv8A2x/9GpXztVT3KnuKfuD60hpT9wfWkNSSfUmnf8gy0/64p/6CKtVV07/kGWn/AFxT/wBBFWq1NlsFeT/G/wD48dG/66S/yWvWK8n+N/8Ax46N/wBdJf5LTA8arp9H/wCQbH9T/OuYrp9H/wCQbH9T/OgC/iiiigAooooAKKKKACiiigAopVVnYKqlmPQAZJq1/Zd//wA+c/8A37NAFSirElheRIXktZkUdSyEClXTr11DLaTsp6ERnmgCtRU0tncwDMtvKg9WQimQwy3D7IY3kbrhRk0AMoqx9guzKYvs028DJXYc4p/9l34/5c5/+/ZoAqUVZfT72JGd7WZVUZJKHAqtQAUUUZoAKKKKACiiigApKXNadj4f1PUbWS5trV3iTq3FAGXS0OrRsVcFWXgg0Dk4HWgBKWtSXw7qkWnC/e0cW5/i/wDrVlUALRRmigDK17/jyT/f/oa56uh17/jyT/f/AKVz1ABRRRQAUUUUAFFFFABRRRQAUUUUAFFFFABRRRQAUUUUAFFFFABSUtdR4S8C6n4tdmt9sNshw0z9M+goA5f0pK9mT4HQeR+91aTzf9lRt/lXNeJfhRquiWrXdrIt7CoywT7yj1oA8+zS13/gr4br4s0uW7e9aBkfZt2//Wrpv+FHRf8AQWb/AL5/+tQB4z+NLXrWp/BqPT9LurwaozeTGz7dvXA+lefeH/DGo+JNSNpYR7tp+eQ9EHqaAMXNHWvZrX4HQ+UDdaq/mY58pRj9RWN4h+D2oaZaPdadci7VBkxnhv8ACgDzKitzwv4f/wCE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kDOhPYZFdTzRigDOsJLy6jlF7FsUjAGKyG0+/wBMvDNaqXU+nPHvXU0dqAMKK81eaRR9m2Ln5iRW53/nQPpRigBaKKKACiiigAooooAKKKKAClpKWgApKWkoAp2v/H7c/UVcqna/8ftz9RVzvQAuKMUUUAGKMUUUAFFFFABRiiigAooooAKKKKACiiigAooooAKMUUUAGKKKKACiiigAooooAWiiigD0nwR/yBP+2jV1ArlvBH/IE/7aNXUigCQU8UwU4UAOpaSloAWikpaACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKZ4U/5GjVP+ua/wBKfTPCn/I0ap/1zX+lAHaUUUUAFfLWof8AITu/+uz/AMzX1LXy1qH/ACE7v/rs/wDM1EiJFWpJPurUdSSfdWoIPob4nf8AJO9V/wC2P/o1K+dq+ifid/yTvVf+2P8A6NSvnaqnuVPcU/cH1pKcfuimY5qST6k07/kFWn/XFP8A0EVOar6d/wAgy0/64p/6CKsmtehsthh71yHjX/j40j/ruf6V15rjvG7BJtKZiABMSSTjHSmBC1eV+K/+Rgn/AOA/yr01r215/wBJhz/vivLvFM8La/OVlQjAwQw9KAMiimebH/z0X86PNj/vr+dADqKZ5sf99P8AvqjzY/8Anov50APopnmx/wB9fzpPNj/vr+dAD/pT/Il/55Pj/dNFnJH9utxvQ5kXjPvXda7rEOjLDttoJN474FAHCNG6feRl+optdhY+KdNvZRDd20Ee/gHAxVDxPo8FjtvLZlELn5gD0NAHPVSn/wCQjb/Q1dhU3D7IRvY9lNMvdNvLe8gklgZUAPPWgCWGJ55VijXLscCpruxuLGQJcJsYjOM5p+iyR/2zaAOv+sHetfxm6f2lGN6g7B3oA50UU+3glu2It08w/wCzTrm0ntADPGYx79KAIqK0tE0watdbHYrDjl1x1o1XSZLG6lWJWaCPneaAM2ikj/fSBI/mY9ADmrUum3kEfmSQMF65oArUhpnmx5/1ij8aXzYx/Gv50ASJG8jYRGdvRRmntbXCD54JV9ypFdlZ/ZtD8OLerEkspAJz6k1mjxosqutxbQEEEDByaAOboprTRvIzBlG45AzVuLTbyaPzI7d2X146UAVqKST905SQhGHVScYp8EbXL7IB5jei80ANoqzJpt5EVDwMN3Sq8yG3k2TfI2M4JoASipJIJYoRM6FYz0Yng0yCNrl9kI3sOcKaAEopJSIpCkhCuOCCantrO4u/9RGXHqKAIaKfcQyWrYnUxt/tUkEL3ORAu/HXac4oAbRU8Nlc3BPkws+DjioZ42tpNkw8tvRjQAlFESmdwkI3n0Xmrh0q+VNxtm2+uRQBToppZd5XcpIOMZ71cXS71o/MFuxX1oArUUxmWNirsqkdQTVi2tLi7+a3iMg9RQBFSVNc2k9mubiMxqTjLEVW82Pj51/OgCtbf8ftz9auVQtpEF5c5dcZHernmx/31/OgCSio/Nj/AOeifnR5sf8AfX86AJKKj82P++v50ebH/fX86AJKKj82P/nov50ebH/z0T86AJKKj82P/non50ebH/fX86AJKKj82P8Avr+dHmx/31/OgCSio/Nj/wCei/nR5sf/AD0T86AJKKj82P8Avr+dHmx/31/OgCSio/Nj/vr+dHmx/wDPRfzoAkoqPzY/+ei/nR5sf/PRPzoAkoqPzY/76/nR5kf99fzoAkora0/QUntPtV5crbxn7mepqUaPoxbaNZUk9gP/AK1AGBR3rfk0LTzZXM1tqSymBCxUVzvmx/31/OgCSimebH/fX86PNj/vp+dAHpfgj/kC/wDA2rqhXH+Crq3j0bDXESneeC4rqFvLTH/HzD/38FAFsU4VVF7a/wDP1D/38FPF7af8/MP/AH8FAFmlqv8AbbT/AJ+Yf+/gpfttp/z9Q/8AfwUAT0tV/ttp/wA/UP8A38FL9utP+fqH/v4KAJ6Kg+3Wn/P1D/38FH260/5+of8Av4KAJ6Kg+3Wn/P1D/wB/BR9utP8An6h/7+CgCeioPt1p/wA/UP8A38FH260/5+of+/goAnoqD7daf8/UP/fwUfbrT/n6h/7+CgCeioPt1p/z9Q/9/BR9utP+fqH/AL+CgCeioPt1p/z9Q/8AfwUfbrT/AJ+of+/goAnoqD7daf8AP1D/AN/BR9utP+fqH/v4KAJ6Kg+3Wn/P1D/38FH260/5+of+/goAnoqD7daf8/UP/fwUfbrT/n6h/wC/goAnoqD7daf8/UP/AH8FH260/wCfqH/v4KAJ6Kg+3Wn/AD9Q/wDfwUfbrT/n6h/7+CgCeioPt1p/z9Q/9/BR9utP+fqH/v4KAJ6Kg+3Wn/P1D/38FH260/5+of8Av4KAJ6Kg+3Wn/P1D/wB/BR9utP8An6h/7+CgCeioPt1p/wA/UP8A38FH260/5+of+/goAnoqD7daf8/UP/fwUfbrT/n6h/7+CgCeioPt1p/z9Q/9/BR9utP+fqH/AL+CgCeioPt1p/z9Q/8AfwUfbrT/AJ+of+/goAnoqD7daf8AP1D/AN/BR9utP+fqH/v4KAJ6Kg+3Wn/P1D/38FH260/5+of+/goAnoqD7daf8/UP/fwUfbrT/n6h/wC/goAnoqD7daf8/UP/AH8FH260/wCfqH/v4KAJ6Kg+3Wn/AD9Q/wDfwUfbrT/n6h/7+CgCemeFP+Ro1T/rmv8ASoje2mf+PqH/AL+CneEXWTxNqbowZTGuGByD0oA7eiiigAr5a1D/AJCd3/12f+Zr6kNfLeo/8hO7/wCuz/zNRIiRVqST7q1HipJPupUEH0P8TTj4eap/2y/9GpXzvuNfQ/xO/wCSd6r/ANsf/RqV87VU9yp7jyx2im7qD9wfWkqST6i07/kF2n/XFP8A0EVZNVtN/wCQXaf9cU/9BFWTWpqthhrg/iZGsun2iMPlYyA/kK7w1wvxI/487L/ef+QpjPITotnj7r/99VzGqWMMd9Iqg446mu3NcjrH/IRk/D+VAGT9li9D+dH2WL0P51NS0AQfZYvQ/nR9mj9D+dTUUAQ/ZY/Q/nR9li9D+dTUUALYWsY1G14P+tXv7iuv8fW8bLZ5B6djXLWH/IQtv+uq/wAxXZeOLeadbTyYnkIHO0ZoA8/+zJu4Bz1616JqUSXHgOJ5s7jGp5PeuZ07w7qF7corW7xx5+Z3XFdF4wvYbTS4tKhI3DGQD0AoAfoOljS/Df2y1tzNeSrkAdR2/KrGinVrySaDWLRliZThiuPwqTTLme78JJ9gkAuY1wfYg5x+Vcyde8RLKYj5m8HG3Yc0AVINMisvGUcKAhVn+UZ7Ve8dWqyavCuDkxgAZqlYS3E3ie2e7BExlG4EYNanjZzHrMDjsgIoA2JLGfQ9Chi0i1Mk7gbmA5+ppbO3u9W0W5g1i1KOAdpYYzxU+qXV9LocN1pL5YKCwXnIrlBr3iGbcg8xjjkBDQBP4Nmkt9TfTlI8nknjnIPrS+KNavEvLrTldfs7AqRt5x9ag8IuT4kzIfnIOfrmmeLbWZNcmlMbeW3IbHFAGx4T0mKz0WTUIoDLcuDsHfpxVvR5tauL14tSsmFvICMlcBad4auXufDBhtXAuIlIAPrXPza74iinML+ZvBxjYeaAMnxFpUFhrc8KKQpIfGemeayvssfofzrS1Oa7uLxpL5WExAyGGDjtVOgDsfDmu2TWS6ZqAAUDaGboRTdZ8C27xNdaYSQefLzxj2rnJNLvoYlle2k8thkMFOK6/wADyXv7+OYP9mA+Ut0BoA57wpoUN5rZE8Z2wgsVPPIPQ102pXOuw6oUsbBzaxnaML96q+j3kEPjK8jVgFk3Ae7Z5pPEGoa7puoyeSzfZ2OUIU4A9KAIfHGlwPa2995WyY/K4q3ZW9v4a8LfbI4wbiRcknuTXNalqOr3lqv20SeSTwSpArrNRgbVPB8RtvnZUHC85xQBl6H4nu7vVIba7VHWRsAgYK1S8c28ba4uc52DvTvDOiXzavBcPC0ccLZYuuM0/wAcY/tpf+uYoAv+IoEPg20BB/h7+9Zfga3jXWHwD9z1rd1i3lvPB9sLeNpGAU7VGT1qHwbot1azyXlwjRAjaqsMGgDndU06O88WPAcjfJjrXX6rFf6XawWmh2ZKgcsFzj/9dc1eXAtfGXnMcBJcmun8S3WqQQxXOmvuhI+baM/jQBBqNjJq3hWR9RtjFdRKW5GDwM5rM+H8Eay3fBxtHeqMmt69dW0qN5rRFCHwp4GK0vAf373/AHKAF1DxQtnf/ZdPhQRK5Dn1Oef1pfG1rBc6ZaXZTDnHI965W6/5Cs//AF3b/wBCrsPFn/Iu2X/Af5UASaLZ2mg+Gf7QMYMzpvJPXPYZrBbxfqbO5BjCN0Xb0rpYo/7b8FpDAwMgQZH+0O1cL/Zt75xi+yylwegU0Abfg/SIL/Up76dN5QlgD03HmtmS718awfLsJPsYbAG3qvrmqfga4WG4urOQ7ZGOQD6jjFM1jVPEGn38kW5vLLfIwU4xQBX8c6Rbw3UF2qbWmBDAe3/666GCCaHwtAdHRfN2/n6/jXFarfapeRQnUFkEYJ2FlIz64q1p99rWiBNkUnkvyFZTgj2oAr61qOo3kIs9STaUfcPlwaw/ssfofzr0vXo4dR8MNe3EHlTqgZc9Qc1xGn6Rd6qX+zIH2dcmgDKFnCCSAQT70v2aP3/Orz2c0d8bNh++DbCM96sahot7pcSSXKBFc4GD3oAyvs0fofzo+yxeh/Otax0S91GBpraMNGvUk1RdGjdkb7ynBoAr/ZYvQ/nR9li9D+dTUUAQ/Zo/Q/nR9mj9D+dTUUAQ/ZY/Q/nR9li9D+dTUUAQ/ZYvQ/nR9lj9D+dTUUAQ/Zo/Q/nR9mj9D+dTUUAQ/ZYvQ/nR9li9D+dTUUAQ/ZYvQ/nR9mj9D+dTUUAQ/Zo/Q/nR9mj9D+dTUUAQ/ZYvQ/nXReFfD9pf3ElxP86wfN5QOS1YdWbDUJ9Ou1uIHwynkeooA1rnXZJNcjZoAYIW2LAfTp+ddFLoWmacsutR2zNIU3LCR90mmWsdrrE0WrWMMbXSf6yFjjJx1p8Nv4mTUXuJo4ZIX4MJlGMe1AGDou25sNemKhTLFkqvAHWuW+yx+h/OvQItJutP0/Wpp4Y4UnjOxUfOOvFcL3oAi+yxeh/Oj7LF6H86mooA29G0u2ms9zq2dx6NWmNFs/7r/wDfVQ6D/wAeH/AjWsKAKI0Wz/uv/wB9Uo0Sy/uv/wB9VfFOFAGf/Yll/df/AL6o/sSy/uv/AN9Vo0UAZ/8AYll/df8A76o/sSy/uv8A99Vo0UAZ39iWX91/++qP7Esv7r/99Vo0UAZ39iWX91/++qP7Esv7r/8AfVaNFAGd/Yll/df/AL6o/sSy/uv/AN9Vo0UAZ39iWX91/wDvqj+xLL+6/wD31WjSd6AM/wDsSy/uv/31R/Yll/df/vqtGigDO/sSy/uv/wB9Uf2JZf3X/wC+q0aKAM7+xLL+6/8A31R/Yll/df8A76rRooAzv7Esv7r/APfVH9iWX91/++q0aKAM7+xLL+6//fVH9iWX91/++q0aKAM7+xLL+6//AH1R/Yll/df/AL6rRooAzv7Esv7r/wDfVH9iWX91/wDvqtGigDO/sSy/uv8A99Uf2JZf3X/76rRooAzv7Esv7r/99Uf2JZf3X/76rRooAzv7Esv7r/8AfVH9iWX91/8AvqtGigDO/sSy/uv/AN9Uf2JZf3X/AO+q0aKAM7+xLL+6/wD31R/Yll/df/vqtGigDO/sSy/uv/31R/Yll/df/vqtGigDO/sSy/uv/wB9Uf2JZf3X/wC+q0aKAM7+xLL+6/8A31R/Yll/df8A76rRooAzv7Esv7r/APfVH9iWX91/++q0aKAM7+xLL+6//fVH9iWX91/++q0aKAM7+xLL+6//AH1R/Yll/df/AL6rRooAzv7Esv7r/wDfVH9iWX91/wDvqtGigDN/sSz/ALr/APfVenfC6FLeW4ijztWEYyc/xVwRr0H4af8AH3df9cR/6FQB6PRRRQAV8t6gx/tO7/67P/M19SV8tah/yE7v/rs/8zUSIkV93FPdjhaiqST7q1BB9DfE7/kneq/9sf8A0alfO1fRPxO/5J3qv/bH/wBGpXztVT3KnuOP3KZ3p5+4KbUkn1Fp3/ILtP8Arin/AKCKsGq+nf8AILtP+uKf+girBrVbGy2GmuE+JH/HnZf7z/yFd2a4X4kf8eVl/vP/ACFMDzE965HWP+QjJ+H8q64965HWP+QjJ+H8qAKFJS0UAJRRRQAUUUUAOikaGaOUYJRgwz7Gun/4Tq/wAbW1OPUH/GuWooA6K48Z6nMhVBHCCP4B/jXPyzSTyF5XLsTkk96bRQBf0vWLvSZd9u/ynqh6GtpvHFyVyLO2D/3sH/GuWooAunVJm1UagVXzQ27Han6vq82sXKzToiMF2gJnFZ9FAGtpPiO+0ldkZWSI/wDLOToK0ZvGt06FY7SBMjBbbzXMUUATwXs1teC7ibbIG3Vu3vjC4vbGS3ltocyIUL45Ga5uigC3p2p3Wlz+bbPg91PQ1vnxxcsvNnbmT+8VP+NcrRQBZ1HUJdTvWuZgodgBwOOKq0tFAHT2fjO5t7ZIJLWGRUGASKbeeM7ueBobeKO3BHJQc1zVFADlmkSYSh2Emd27vXSW/jW9jhEc8EU+0Y3OOTXM4ooA19W8RXOrQiGSKKNFOQEGKXSPEl5pCGNNskR/gft9Kx6MUAdhZeKrzU9Zs7fakMRk5CDrwaq+OP8AkNL/ANcxXPW1xJaXCTwttkQ5U4zg1Lf6jc6nOJrpw7gYyAB/KgDV0rxbd6bai32JLGv3d3annxpqJvBPsj2gYEeDtrnaKAJ727e+vJLiQAO5yQOgrW0zxXfadCICEmiA4DjoKwqMUAdFeeMLq5t5IEtoI0kUqSoOeaz9I1u40dpTCkb+YMHfnis2jFAD5JmluHmIG5mLEds5zWlqPiC51KwitJY41SPGGXOayqKANHStbvNIk3QNlT1RuhrYk8c3bD5LWAMf4sH/ABrlqMUATLeTJem6jfZKWL5Hqa6KLxvdiMJNbQSsBwWBrl6KANPV9duNY8tZkjRYySoQetaVt4zuoLdIZLWCQIMAsDmuaooA2NX8R3mrIInCxRD+BOh+tR6Prs+imUwxRv5mM788flWXRQBbm1GWbVG1BlUSGTzNvbNXNY8RXOswxRTRRIsZyNmee3c1kYooA2NK8R3OkWslvBFEyuckvnP86yZJDLK0jdWJJptFABRRRQAUUUUAFFFFABRRRQAUUUUAFFFFABRRRQAUUUUAFJS0UAOSWSPOyRl9dpxT/tNx/wA95f8Avs1FRQA83EzKVM0jA8EFzTBRRQAtFFFAHUaD/wAeH/AjWvWRoP8Ax4f8CNa9ADhSikFKKAFooooAWiiigAooooAKKKKACiiigApO9LSd6AFooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAQ16D8NP+Pu6/64j/ANCrz416D8NP+Pu6/wCuI/8AQqAPR6KKKACvlrUP+Qnd/wDXZ/5mvqWvlrUP+Qnd/wDXZ/5mokRIq1JJ91ajqST7q1BB9DfE7/kneq/9sf8A0alfO1fRPxO/5J3qv/bH/wBGpXztVT3KnuKfuD60lKfuD6001JJ9R6d/yC7T/rin/oIqwar6d/yDLT/rin/oIqya1WxqthhrhfiR/wAeVl/vP/IV3RrhfiR/x52f+8/8hTGeYnvXI6x/yEZPw/lXXHvXI6x/yEZPwoAofhSUYyfX2qx9gu+1tL/3yaAK9FWPsF3/AM+0v/fBo+wXf/PtL/3waAK9FWPsF3/z7S/98Gj7Bd/8+0v/AHwaAK9FWPsF3/z7S/8AfBo+wXf/AD7S/wDfBoAr0VY+wXf/AD7S/wDfBo+wXf8Az7S/98GgCvRVj7Bd/wDPtL/3waPsF3/z7S/98GgCvRVj7Bd/8+0v/fBo+wXf/PtL/wB8GgCvRVj7Bd/8+0v/AHwaPsF3/wA+0v8A3waAK9FWPsF3/wA+0v8A3waPsF3/AM+0v/fBoAr0VY+wXf8Az7S/98Gj7Bd/8+0v/fBoAr0VY+wXf/PtL/3waPsF3/z7S/8AfBoAr0VY+wXf/PtL/wB8Gj7Bd/8APtL/AN8GgCvRVj7Bd/8APtL/AN8Gj7Bd/wDPtL/3waAK9FWPsF3/AM+0v/fBo+wXf/PtL/3waAK9FWPsF3/z7S/98Gj7Bd/8+0v/AHwaAK9FWPsF3/z7S/8AfBo+wXf/AD7S/wDfBoAr0VY+wXf/AD7S/wDfBo+wXf8Az7S/98GgCvRVj7Bd/wDPtL/3waPsF3/z7S/98GgCvRVj7Bd/8+0v/fBo+wXf/PtL/wB8GgCvRVj7Bd/8+0v/AHwaPsF3/wA+0v8A3waAK9FWPsF3/wA+0v8A3waPsF3/AM+0v/fBoAr0VY+wXf8Az7S/98Gj7Bd/8+0v/fBoAr0VY+wXf/PtL/3waPsF3/z7S/8AfBoAr0VY+wXf/PtL/wB8Gj7Bd/8APtL/AN8GgCvRVj7Bd/8APtL/AN8Gj7Bd/wDPtL/3waAK9FWPsF3/AM+0v/fBo+wXf/PtL/3waAK9FWPsF3/z7S/98Gj7Bd/8+0v/AHwaAK9FWPsF3/z7S/8AfBo+wXf/AD7S/wDfBoAr0VY+wXf/AD7S/wDfBo+wXf8Az7S/98GgCvRVj7Bd/wDPtL/3waPsF3/z7S/98GgCvRVj7Bd/8+0v/fBo+wXf/PtL/wB8GgCvRVj7Bd/8+0v/AHwaPsF3/wA+0v8A3waAK9FWPsF3/wA+0v8A3waPsF3/AM+0v/fBoAr0VY+wXf8Az7S/98Gj7Bd/8+0v/fBoAr0VY+wXf/PtL/3waPsF3/z7S/8AfBoAr0VY+wXf/PtL/wB8Gj7Bef8APtL/AN8mgCvmlqSS2ngAMsToCcDcOtR0AdRoP/Hh/wACNa9ZGg/8eI/3jWsKAHCnCminCgBaBRQKAFooooAKKKKACiiigAooooAKTvS0negBaKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKAENeg/DT/j7uv+uI/wDQq8+Neg/DT/j7uv8AriP/AEKgD0eiiigAr5a1D/kJ3f8A12f+Zr6lr5a1D/kJ3f8A12f+ZqJESKtSSfdWo6kk+6tQQfQ3xO/5J3qv/bH/ANGpXztX0T8Tv+Sd6r/2x/8ARqV87VU9yp7in7g+tJTj9wUypJPqPTv+QXaf9cU/9BFWTVbTuNLtP+uKf+girBrXoarYaa4X4kf8edl/vP8AyFd0a4X4kf8AHnZf7z/yFMZ5ia5HWP8AkIyfh/KuuNcjrH/IRk/D+VAGeMg5HBq6NXvwABcMPwFUqO2KALv9sah/z8t+Qo/tjUP+flvyFUqKALv9sah/z8t+Qo/tjUP+flvyFUqKALv9sah/z8t+Qo/tjUP+flvyFUqKALv9sah/z8t+Qo/tjUP+flvyFUqKALv9sah/z8t+Qo/tjUP+flvyFUqKALv9sah/z8t+Qo/tjUP+flvyFUqKALv9sah/z8t+Qo/tjUP+flvyFUqKALv9sah/z8t+Qo/tjUP+flvyFUqKALv9sah/z8t+Qo/tjUP+flvyFUqKALv9sah/z8t+Qo/tjUP+flvyFUqKALv9sah/z8t+Qo/tjUP+flvyFUqKALv9sah/z8t+Qo/tjUP+flvyFUqKALv9sah/z8t+Qo/tjUP+flvyFUqKALv9sah/z8t+Qo/tjUP+flvyFUqKALv9sah/z8t+Qo/tjUP+flvyFUqKALv9sah/z8t+Qo/tjUP+flvyFUqKALv9sah/z8t+Qo/tjUP+flvyFUqKALv9sah/z8t+Qo/tjUP+flvyFUqKALv9sah/z8t+Qo/tjUP+flvyFUqKALv9sah/z8t+Qo/tjUP+flvyFUqKALv9sah/z8t+Qo/tjUP+flvyFUqKALv9sah/z8t+Qo/tjUP+flvyFUqKALv9sah/z8t+Qo/tjUP+flvyFUqKALv9sah/z8t+Qo/tjUP+flvyFUqKALv9sah/z8t+Qo/tjUP+flvyFUqKALv9sah/z8t+Qo/tjUP+flvyFUqKALv9sah/z8t+Qo/tjUP+flvyFUqKALv9sah/z8t+Qo/tjUP+flvyFUqKALv9sah/z8t+Qo/tjUP+flvyFUqKALv9sah/z8t+Qo/tjUP+flvyFUqKALv9sah/z8t+Qo/tjUP+flvyFUqKALv9sah/z8t+Qo/tjUP+flvyFUqKALv9sah/z8t+Qo/tjUP+flvyFUqKALv9sah/z8t+Qo/tjUP+flvyFUqKALv9sah/z8t+Qo/ti/8A+flvyFUqKALE99c3Sqs8pcA5GQKgpMYpaAOo0H/jw/4Ea1hWToP/AB4f8CNa4oAUU4U0U4UALQKKKAFooooAKKKKACiiigAooooAKTvS0negBaKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKAENeg/DT/j7uv8AriP/AEKvPjXoPw0/4+7r/riP/QqAPR6KKKACvlrUP+Qnd/8AXZ/5mvqWvlrUP+Qnd/8AXZ/5mokRIq1JJ91ajqST7q1BB9DfE7/kneq/9sf/AEalfO1fRPxO/wCSd6r/ANsf/RqV87VU9yp7jj9wUynH7g+tJUkn1Fp3/ILtP+uKf+girBqvp3/ILtP+uKf+girBrVbGy2GmuE+JH/HnZf7z/wAhXdnvXC/Ej/jysv8Aef8AkKYHmJ71yOsf8hGT8P5V1x71yOsf8hGT8P5UAUKSlooASiiigAooooAKKSjv3oAWikpaACiinIjyyKkalnY4CqMk0ANoqxd6feWDqt5bSwMwyolQrn86r0AFFFFABRRRQAUUUUAFFFIfWgBaKTvS0AFFWo9Mv57VrqKzne3XrIqEqPxqO1srq+kMdpbyTuBkiNSxx+FAENFPmhkt5mimjaOVDhkYYI/CmUAFFFFABRRRQAUUUUAFFFFABRRRQAUUUUAFFFFABRRRQAUUUUAFFFFABRRRQAUUUUAFFFFABRRRQAUUUUAFFFFABRRRQAUUUUAFFFFABRRRQAUUUUAFFFFAC0UgpaAOo0H/AI8P+BGtesjQf+PD/gRrXoAcKUU0U4UALRRQKAFooooAKKKKACiiigAooooAKTvS0negBaKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKAENeg/DT/j7uv8AriP/AEKvPjXoPw0/4+7r/riP/QqAPR6KKKACvlrUP+Qnd/8AXZ/5mvqWvlrUP+Qnd/8AXZ/5mokRIq1JJ91ajqST7q1BB9DfE7/kneq/9sf/AEalfO1fRPxO/wCSd6r/ANsf/RqV87VU9yp7in7g+tJSn7g+tJUkn1Hp3/ILtP8Arin/AKCKsGq+nf8AILtP+uKf+girBrXoarYYa4X4kf8AHnZ/7z/yFd0a4X4kf8edn/vP/IUxnmJ71yOsf8hGT8K641yOsf8AIRk/CgChSGlpDQAUUUUAFFFFAGz4T0M+IvEtnpm7asrZYj+6OT+gr2XxDr3hP4dvBpUehC4lKAthQfxJavF/DGtv4d8Q2mpou7yn+Yf7J4P6V7vq+ieHPinpcV7aXSpeKmFdT8yn0YUAeaeN/E/hXxBoMR0rTPseoeaCwxyFwc+3pXnqqzsFVSzHoFGc10PirwZqvhK88q9i3Qsf3c6cq1ej/Drw5p+i+D5/Fd/aC7n2s0MRXdwBkYHqaAPHZbK7gTfNbTxr2Z4yBV/wzz4m07I/5brXtfhTxYvje9udH1jw7Hbwuh8pvK4x9SOK8ul0caD8To9OU5SK7Xb7A8j9DQB1fx4VV1vStqgfuX6D3WvJ4opZn2RRvI3oikmvW/jwC2u6So6mJx+q1t6bp9l8OvAMOqf2Wt9qs4UkFN3J/kBQB4ZNa3Nt/r4JYs/89EK/zqGvoTR7y3+JnhzUbXVNFSzu4lJRhHjnHBBxnrXm3w88Gx+IPGM9nd82tiWaUf38Ntx+eKAOMisbudd0NrPIvqkZP8qY9rcJII2glVz0Uocn8K9y8XfEfT/B2pf2Po+jWkzQqBIxUKB7cdTXDar8SRqOu6dqq6LaRtaZJiA+WTOOv5UAQeIPhvfaB4etdVa5W48/GIo0bK59a4p0eNisisrDqGGCK+l/GHjE6D4QsdVGmW1yZ8fuZfur06V89eItabxBrdxqTW0dsZsfuoh8q4AH9KAMytLw/pEmu69Z6bHnM8gUkfwjPWs2vW/gnoatfXev3ICxW67I2bpk9fywKAO28XfD7SZ/BtzFY2UUV1DEHWRM5JUZI/HFfNzKyMVYYZTg17/4C8ctr3jPWbC4fME7F7dG9AOR+leUfEHQW0DxfeW4UiGRjJGfUHk/rmgD0jwWiH4I6ixUE4k5x/tVzvwQVW8WzBlBHkN1FdH4K/5IfqP0k/8AQq534H/8jdN/17tQByXj0AeO9YAGALg/yFc7XR+Pv+R81j/r4P8AIVzlABRRRQAUUUUAFFFFABRRRQAUUUUAFFFFABRRRQAUUUUAFFFFABRRRQAUUUUAFFFFABRRRQAUUUUAFFFFABRRRQAUUUUAFFFFABRgk4HU0UqsVYMOoOaAPQ9K+EWqanpEF62oWlvNcJvitpD87CsHw/4H1PX9cutMQpbta8zySH5Y/rXp3hPxToHiW80EXP2iDVrTEaBSdjHj0PTit2zg8Nw3/it4pJxK8ZF71G0YPSgDxjxf4EvfCUdvPJdQ3drOdqTQH5SfT9KyPD+h3XiPWYNNtMCWU/ePRR6mut8d+KtIvtC07w/okc32S0YuZJjkknPqc962fhPaw6LoureLbtfkt02R57jvj8RQBwPirwve+E9U+wXjK7lA6ug4OaxK9n+I8aeLPh/pnimBB5sYHmqvbcBkH6YrxigAFLQKKAOo0H/jwH+8a1hWToP/AB4D/eNa4oAUU4U0U6gBaKKKAFooooAKKKKACiiigAooooAKTvS0negBaKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKAENeg/DT/j7uv+uI/wDQq8+Neg/DT/j7uv8AriP/AEKgD0eiiigAr5a1D/kJ3f8A12f+Zr6lr5a1D/kJ3f8A12f+ZqJETKtSSfdWo6kk+6tQQfQ3xO/5J3qv/bH/ANGpXztX0R8UCF+HWqknA/c8/wDbVK+c/Oj/AOei/nVT3KnuTfwUw0hmi2ffT86b5sf99T+NTck+p9O/5Bdp/wBcU/8AQRVg1W07H9l2nP8AyxT/ANBFWDWvQ1Ww0964X4kf8eVl/vP/ACFd0a4X4kf8edn/ALz/AMhTGeYtXI6x/wAhGT8P5V1x71yOsf8AIRk/D+VAFCiikNABRRRQAUUUUAPggluZRHDG0jkHCqM1d0vVtS0G+FzYzSwTKcHaSM+x9aueEPEC+GPElvqr2/2hYgw8sHGcgj+telzfFDwbeutzdeGt1wDnk5IP1xQB0niO6/4SH4Nz6hqUIS4+zCVcjHz46j8zUngrVJ1+EsFxpUSTXdrCVEeerKOleXeN/ibdeKLUafaW4tNPHWPOS2PU+lZngvx5qHg65PlAT2chzJCx6/T0oA6l/jbr8UjI9hCrKcEEdP0rlU1y48R/ES11S6jWOWadNyr0GAB/Su2n+K3hWV/tjeFA92TkkuOD69K4DU/Ey6h4zGvrZiJBIjiBT2UAYz+FAHffHRgviLRmPQRsf1Wu+17xLf6b4DttZ0a3S5GxWYei9zXiXxB8bR+Nb+0uIrJ7VYEZdrOGJzj29qveC/iheeF7X+z7q3F7p46R5wy/Q0Aav/C8NeJx9igz9P8A61XPgpqiT+KdX87ak1yjSAepLgkCkk+Knhe2cz2PhVRcN1cuOD+VeZ22t3Vhr7avZN5MwmaVQOgyTx+tAG58SNJu9L8aX5nRts8hlRscMCc/1rkjG6qGZSAeBkV7Evxl0q/s401rQBcTqBlg4wT7ccVxfjnxnbeKxZx2emiwhtd21QQc5x7e1AHovxNieX4W6VIillTZux2zivC+9eq+Hfi3bWnh6LSNb0s3iRrsDBsZHvwa8+8S6jZatr9ze6fZiztZCNkIOduAB/SgDMRDJIqLyWIA+tfRltc6X8Ofh1ZW+rxFzOuJY1XJZmHzV4Dol9b6Zrdpe3VubiGFw7RBsbgPeui8f+Om8aXlu8ds9tbwLhYmfcc9zn8KAO5074g/D7Tr9Luz0VoLgHAlW3AIz15q18ZtITVvDtj4htF3CIDe2OkbdP1Irwo16Vp/xRhh8EN4d1DTJLo+UYxMJQB1yOMduPyoA6jwV/yQ/UfpJ/6FXO/A/wD5G6b/AK4NWXofxDi0jwHdeHG095HmDYmEgAGTnpis3wH4uj8G6y99LaNcq0ZQoj7ev50AQ+Pf+R71j/r4P8hXOVpeINUGt6/e6msRjFxJvCE5x+NZtABRRRQAUUUUAFFFFABRRRQAUUUUAFFFFABRRRQAUUUUAFFFFABRRRQAUUUUAFFFFABRRRQAUUUUAFFFFABRRRQAUUUUAFFFFABSooZ1VuhIBpKKAPozw5pNvoyeH4tM8PxXUc6CSfUSozGeOc4qTSPEsOraz4os7XSLZ5rZCQFxm5ODweK8QsfHPiPTtM/s+11OaO2Awqg/dHsazdM1zUtHvzfWN3JDcH7zr3+tAHpPxI0O1fw7oupppCaVqd1N5Mlqgx64zx9K3tX1zTvh54Q0rQ7nS4tQNxF5ksMhwMnk54Pc15Bq3ijWdbu4bm/vpJZYCDESfukelVdU1nUdbuBcaleS3UqjaGkPQelAHuPhDxLpXjnRtT8Nw6TDpkbRHZFEcqcg5PQe1eE39nJYahcWkq7XicqRUmmatf6Ndi7066ktp8bfMjODg1DdXdxf3Ul1dStLPIdzu3VjQBFRSUtAHUaD/wAeH/AjWvWRoP8Ax4f8CNawoAcKdTacKAFooooAWiik4oAWiiigAooooAKKKKACko4ooAWiiigAooooAKKTiigBaKSigBaKTiloAKKKKACiiigAooooAKKKKACiiigAopKOKAFopKKAFopKKAFooooAKKKKACik4paACiiigAooooAKKKTigANeg/DT/j7uv+uI/wDQq8+r0H4af8fd1/1xH/oVAHo9FGaM0AFfLWof8hO7/wCuz/zNfUhNfK+oyx/2nd/OufOfv7ms5kTIqe/3VqDzov76/nT3miwv7xOnPNRczPoL4ukj4X6wR1/cf+jo6+XPMf8AvV9RfF7/AJJdrP8A2w/9HR18t1s0mbNJkQnk/tS2i3fI7AEetejroenFV/0Zenqf8a80T/kN2f8AvivYFX5F+gpcqDlR7lYoq6fbADAESgD8BU5UVHZ/8eFv/wBc1/lUtVYZGQDmuF+JIxZ2X+8/8hXd461w3xKH+hWX+8/8hQB5eR1rj9Y/5CMn4fyrsT0NcdrH/IRk/D+VAFGikFLQAlGKKWgBMUUtJQAYooooATFLRRQAUYpaQ0AJiloooAPxoozRQAUYpaSgAxRRS0AJRS0lABRiiigBMe9LiiigAxRS0UAJRRRQAUUUUAGKMUtFACYopaKAEopaKAEooooAKKKKACilooASiiigAooooAKMUCloASig0UAFFFFABRilFFACYopaKAEopaKAEooooAKKKKACigGloAT+VFLSUAFJilooAMUtJS0AFKKSloA6jQB/oH/AjWsBWVoH/IP/AOBGtYUALjigUvakFAC0UUooAKXFIOtOoATFJ3p1N70AFFFFABSgUlOHSgBMUh606mt1oAKKKKACgc0Uq9KADFGKWigBMUYpe9FACYpDxTqa3WgAooooAKUdKSlXpQAYoNLSHpQAlFAooAKB1ooHWgBcUYpaKAExRiloFACYoNLSGgBtLRRQAUDk0UL1oAXFGKWigBtFFFABRRRQAuKMUtFADT1r0D4a/wDH3df9cR/6FXn7da9A+Gv/AB93X/XEf+hUAejUGig9KAFABGTXzxfaJp7X9yTbKSZWJ5PqfevohPu14Xdr/ps//XRv50mkxNJmJ/Yenf8APsv5n/GqVlpFlLPdK9upCPgcn3rottZ2nL/pN7/10/xpcqFyo9d+L3/JLtZ/7Yf+jo6+W6+pPi9/yS7Wf+2H/o6OvluqKKyf8huy/wB8V7En3V+lePJ/yG7L/fFewp91fpQB7nZ/8eNv/wBc1/lUxqGy/wCPG3/65r/KpzQAz1rhfiX/AMedl/vP/IV3VcN8Sv8Ajzsf95/5CgDy8jrWZc6Rb3MzSvu3H3rUplAGSdCtO2786T+wrX/a/OtbFGKAMj+wrX/a/Ol/sK1/2vzrUxzTsUAZP9hWv+1+dNOh2uf4vzrYxTCOaAMr+w7X/a/Oj+w7X/a/OtXFGKAMn+w7X0b86UaHa/7X51qkUqjigDK/sO1/2vzpDodr/tfnWtigigDI/sS1/wBr86X+w7X/AGvzrVxRigDJ/sO1z0b86UaHa+jfnWrigCgDL/sO1/2vzo/sK1/2vzrVxS4oAyf7Ctf9r86P7Ctf9r861sUYoAyf7Dtf9r86Q6Ha/wC1+da+KQigDJ/sO1/2vzo/sO19G/OtXFGKAMr+w7X/AGvzoGh2v+1+dauKVRQBlf2Ha/7X50f2Fa/7X51rYoI4oAyP7Ctf9r86P7Dtf9r861QKXFAGT/Ydr/tfnR/Ydr/tfnWtikxzQBmf2Fa/7X50f2Fa/wC1+da2KMUAZP8AYVr/ALX50n9hWv8AtfnWtijFAGV/YVr/ALX50h0K1/2vzrXxSEcUAZA0O1/2vzo/sO1/2vzrVxS4oAyf7Dtf9r86Bodrn+L861sUAc0AZf8AYVr/ALX50f2Fa/7X51rYoxQBj/2Ha5/i/Oj+w7X/AGvzrVxzS4oAyf7Dtf8Aa/Oj+w7X/a/OtbFBFAGUNDtfRvzpf7Ctf9r861QOKXFAGQdCtf8Aa/Ok/sO1/wBr8612FJigDJ/sO1/2vzo/sO1/2vzrWxRigDKGhWv+1+dL/YVr/tfnWoop2KAMn+wrX/a/Ok/sK1z/ABfnWtijFAGV/YVr/tfnR/YVr/tfnWtijFAGQdDtf9r86T+w7X/a/OtYijFAGT/Ydr/tfnR/Ydr/ALX51rYoxQBlDQ7X0b86X+wrX0b861FFLigDJOh2uP4vzpP7Dtf9r861iOKTFAGV/Ydr/tfnR/Ydr/tfnWtijFAGUNCtf9r86d/YVr/tfnWmBzTsUAZX9g2v+1+dL/YNp/tfnWrinUAV7S1jtIvLi6ZzVkCkApwFAAelIOlKaQUALS0lFACjrTqaOtOoAKb3p1N70AFFFFABTh0ptOHSgAprdadTW60AFFFFABSr0pKVelAC0UUUAHeijvRQAU1utOprdaACiiigApV6UlKvSgBaQ9KWkPSgBBRQKKACgdaKB1oAdRRRQAUCigUAFIaWkNACUUUUAFC9aKF60AOooooAbRRRQAUUUUAOooooAa3WvQPhr/x93X/XEf8AoVeft1r0D4a/8fd1/wBcR/6FQB6NQelFB6UAOT7teHXn/H7P/wBdG/nXuKfdrw68/wCP2f8A66N/OgCGs3Tf+Pm9/wCun+NaVZum/wDHze/9dP8AGgD1v4vf8ku1n/th/wCjo6+W6+pPi9/yS7Wf+2H/AKOjr5boArp/yG7L/fFewp91fpXjyf8AIbsv98V7Cn3B9KAPdLP/AI8bf/rmv8qmqGz/AOPG3/65r/KpqAGjrXDfEv8A487H/ef+Qruh1rhfiX/x5WX+8/8AIUAeYUypD3plACUlOoxQAzHNOoxzTsUANxTSOakxTSOaAG4pMU7FLigBmKUDilxTgOKAG4pCKfikYUAMxRinYoxQA3FKopcUqigBMUYp+KTFADcUYp2KXFADMUjCn4pGFADMUYp+KMUAMxSgUuKVRQAmKCOKdigjigCPFLilxzS4oAbikxzT8UY5oAMUYp2KMUANxSYp+KTFACYpCKfikI4oAZijFLilxQA3FAHNOxQBzQAmKXFOxRigCLHNLilxzS4oAbiginYoIoAQDilxSgcUuKAIyKMU5hRigBuKMU7FGKAGqKdihRTsUANxSYp2KMUAJikxT8UYoAjIoxTmFGKAG4oxTsUYoAaop2KFFOxQAw9KaBUhHBpoFACYoxTsUYoAaBzT8UgHNOxQAmKWlxRigAFLRRQAHpSClPSkFAC0UneloAB1p1NHWnUAFN706m96ACiiigApw6U2nDpQAU1utOprdaACiiigApV6UlKvSgBaKKKADvRR3ooAKa3WnU1utABRRRQAUq9KSlXpQAtIelLSHpQAgooFFABQOtFA60AOooooAKBRQKACkNLSGgBKKKKAChetFC9aAHUUUUANooooAKKKKAHUUUUANbrXoHw1/wCPu6/64j/0KvP2616B8Nf+Pu6/64j/ANCoA9GoPSig9KAHJ92vDrz/AI/Z/wDro3869xT7teHXn/H7P/10b+dAENZum/8AHze/9dP8a0qzdN/4+b3/AK6f40Aet/F7/kl2s/8AbD/0dHXy3X1J8Xv+SXaz/wBsP/R0dfLdAFdP+Q3Zf74r2FPuD6V48n/Ibsv98V7Cn3B9KAPdbP8A48bf/rmv8qmqGz/48bf/AK5r/Kp6AGDvXDfEv/jysf8Aef8AkK7odTXC/Ev/AI8rH/ef+QoA8wptPptACUUtFADcc07FJTsUAJimkc0+mnrQAmKKXFLigBtKBxRilHSgApGp2KRhQA2iloxQAlCilpVoAKMUuKMUAJijFLS4oAbSNTsUjUANxRS4pcUANpV6milUUAFB6UtB6UAMxRiloxQAmKO9LijvQAuKMUuKMUAJRilooATFBFLig0AMAoxSijFACYoHWlxR3oAXFGKXFFADMc0Ype9GKAExQRS4oIoAUDijFKOlGKAGsKSnGkoATFGKXFFAAop2KRaXFACYopaKAExRinYpMUANYUmKcaMUAJikpcUUAC0tCiloAQ9KaBTz0poFACUYp1JigBAOafTQOafQAlFLRQAlLRRQAGm040goAKKKKAAdadTR1p1ABTe9OpvegAooooAKcOlNpw6UAFNbrTqa3WgAooooAKVelJSr0oAWiiigA70Ud6KACmt1p1NbrQAUUUUAFKvSkpV6UALSHpS0h6UAIKKBRQAUDrRQOtADqKKKACgUUCgApDS0hoASiiigAoXrRQvWgB1FFFADaKKKACiiigB1FFFADW616B8Nf+Pu6/64j/0KvP2616B8Nf8Aj7uv+uI/9CoA9GoPSig9KAHJ92vDrz/j9n/66N/OvcU+7Xh15/x+z/8AXRv50AQ1m6b/AMfN7/10/wAa0qzdN/4+b3/rp/jQB638Xv8Akl2s/wDbD/0dHXy3X1J8Xv8Akl2s/wDbD/0dHXy3QBXT/kN2X++K9hT7g+lePJ/yG7L/AHxXsKfcH0oA91sv+PG3/wCua/yqfvUFl/x42/8A1zX+VT96AGjrXDfEv/jysv8Aef8AkK7peprhviX/AMeVj/vP/IUAeX02nUlACUUtFACd6dTadigApp607FNPWgAopaKAEpR0oPSlHSgApDTqRqAG0UtFACULRSrQAtFLRQAneiiloASkalxSGgBKKWigBKUUlKKAFpD0pcUHpQAylopaAEo70tJ3oAdRS0UAJQKWkoAKD0ooNADRRQKXFACUd6WkHWgB1FGKMUAN70Ud6WgBKDS0h6UAKKWgdKWgBppKVqKAEopaSgBVpaRadQAlFLSUAFFLikxQAjUlK1FACUUtJQAopaQUuKAEPSminnpTRQAUUtFACDrTqQdadQAlFLRQAlLSCloAQ0gpT0pBQAUUUUAA606mjrTqACm96dTe9ABRRRQAU4dKbTh0oAKa3WnU1utABRRRQAUq9KSlXpQAtFFFAB3oo70UAFNbrTqa3WgAooooAKVelJSr0oAWkPSlpD0oAQUUCigAoHWigdaAHUUUUAFAooFABSGlpDQAlFFFABQvWihetADqKKKAG0UUUAFFFFADqKKKAGt1r0D4a/8AH3df9cR/6FXn7da9A+Gv/H3df9cR/wChUAejUHpRQelADk+7Xh15/wAfs/8A10b+de4p92vDrz/j9n/66N/OgCGs3Tf+Pm9/66f41pVm6b/x83v/AF0/xoA9b+L3/JLtZ/7Yf+jo6+W6+pPi9/yS7Wf+2H/o6OvlugCun/Ibsv8AfFewp9wfSvHk/wCQ3Zf74r2FPuD6UAe62X/Hjb/9c1/lU/eoLL/jxt/+ua/yqfvQAi9TXDfEv/jysf8Aef8AkK7leprhviX/AMeVj/vP/IUAeX0lLSUAFFFFACd6fTO9PoAKaetOpp60AFFFFABSjpSUo6UALSN0paRulACUUUUAFC9aKF60AOooooAKKKKACkalpGoASiiigAoXvRQvU0AOpD0paQ9KAGilpBS0AFHeijvQA6iiigAooooAKQ9KWkPSgBopaQUtABSDrS0g60APooooAb3oo70UAFBooNACjpS0g6UtADWooaigAooooAFp1NWnUAFJ3paO9ABRRRQA1qKGooAKKKKAFWlpFpaAEPSminHpTRQAtFFFAAOtOpo606gAooooABRQKKAEPSkFKelIKACiiigAHWnU0dadQAU3vTqb3oAKKKKACnDpTacOlABTW606mt1oAKKKKAClXpSUq9KAFooooAO9FHeigAprdadTW60AFFFFABSr0pKVelAC0h6UtIelACCigUUAFA60UDrQA6iiigAoFFAoAKQ0tIaAEooooAKF60UL1oAdRRRQA2iiigAooooAdRRRQA1utegfDX/j7uv+uI/9Crz9utegfDX/AI+7r/riP/QqAPRqD0ooPSgByfdrw68/4/Z/+ujfzr3FPu14def8fs//AF0b+dAENZum/wDHze/9dP8AGtKs3Tf+Pm9/66f40Aet/F7/AJJdrP8A2w/9HR18t19SfF7/AJJdrP8A2w/9HR18t0AV0/5Ddl/vivYU+4PpXjyf8huy/wB8V7Cn3B9KAPdbL/jxt/8Armv8qn71BZf8eNv/ANc1/lU/egBF6muG+Jf/AB5WP+8/8hXcr1NcN8S/+PKx/wB5/wCQoA8vpKWkoAKKKKAE70+md6fQAU09adTT1oAKKKKAClHSkpR0oAWkbpS0jdKAEooooAKF60UL1oAdRRRQAUUUUAFI1LSNQAlFFFABQvU0UL1NADqQ9KWkPSgBopaQUtABR3oo70AOooooAKKKKACkPSlpD0oAaKWkFLQAUg60tIOtAD6KKKAG96KO9FABQaKDQAo6UtIOlLQA1qKGooAKKKKABadTVp1ABR3oo70AFFFFADWooaigAooooAVaWkWloAQ9KaKcelNFAC0UUUAA606mjrTqACiiigAFFAooAQ9KQUp6UgoAKKKKAAdadTR1p1ABTe9OpvegAooooAKcOlNpw6UAFNbrTqa3WgAooooAKVelJSr0oAWiiigA70Ud6KACmt1p1NbrQAUUUUAFKvSkpV6UALSHpS0h6UAIKKBRQAUDrRQOtADqKKKACgUUCgApDS0hoASiiigAoXrRQvWgB1FFFADaKKKACiiigB1FFFADW616B8Nf+Pu6/wCuI/8AQq8/brXoHw1/4+7r/riP/QqAPRqD0ooPSgByfdrw68/4/Z/+ujfzr3FPu14def8AH7P/ANdG/nQBDWbpv/Hze/8AXT/GtKs3Tf8Aj5vf+un+NAHrfxe/5JdrP/bD/wBHR18t19SfF7/kl2s/9sP/AEdHXy3QBXT/AJDdl/vivYU+4PpXjyf8huy/3xXsKfcH0oA91sv+PG3/AOua/wAqn71BZf8AHjb/APXNf5VP3oAReprhviX/AMeVj/vP/IV3K9TXDfEv/jysf95/5CgDy+kpaSgAooooATvT6Z3p9ABTT1p1NPWgAooooAKUdKSlHSgBaRulLSN0oASiiigAoXrRQvWgB1FFFABRRRQAUjUtI1ACUUUUAFC9TRQvU0AOpD0paQ9KAGilpBS0AFHeijvQA6iiigAooooAKQ9KWkPSgBopaQUtABSDrS0g60APooooAb3oo70UAFBooNACjpS0g6UtADWooaigAooooAFp1NWnUAFHeijvQAUUUUANaihqKACiiigBVpaRaWgBD0popx6U0UALRRRQADrTqaOtOoAKKKKAAUUCigBD0pBSnpSCgAooooAB1p1NHWnUAFN706m96ACiiigApw6U2nDpQAU1utOprdaACiiigApV6UlKvSgBaKKKADvRR3ooAKa3WnU1utABRRRQAUq9KSlXpQAtIelLSHpQAgooFFABQOtFA60AOooooAKBRQKACkNLSGgBKKKKAChetFC9aAHUUUUANooooAKKKKAHUUUUANbr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RRQAUhGaWigBu2jbTqKAG7aUDFLRQAUUUUAN20badRQA3bQFp1FABRRRQAUUUUAFFFFADcUbadRQA3bQBinUUAFFFFADStG2nUUAN20badRQAgGKWiigBCM0m2nUUAN20badRQAgGKWiigAo70Ud6ACiiigBCM0m2nUUAN20badRQAgGKWiigBD0pNtOooAbto206igBoGKdRRQAUUUUAAooFFABTcU6igBu2jbTqKAEAxS0UUAFNK06igBu2jbTqKAG7acKKKACkIyaWigBu2jbTqKAG7aUDFLRQAUUUUAHeijvRQAU0jNOooAbto206igBu2lAwKWigApDS0UAN20badRQA3bSgYpaKACiiigAoFFAoAKD0oooAbijbTqKAG7aUDFLRQAUUUUAN20badRQA3bRtp1FABRRRQA0jNd/8Nhm7uh/0xH/AKFXBV3/AMNP+Py6/wCuI/8AQqAPRfLNHln1qSloAaowK8NvP+P2f/ro3869zrwy8/4/Z/8Aro386AIazdN/4+b3/rp/jWlWbpv/AB83v/XT/GgD2n4jf8iJqH+9B/6OSvKK9X+I3/Iiah/vQf8Ao5K8ooAKKKKAPc7L/jxt/wDrmv8AKp+9QWX/AB42/wD1zX+VT96AEXqa4X4mf8eVl/vP/IV3S9TXC/Ez/jysv95/5CgDzHvRR3ooAKKKKACiiigAooooAKO9FHegAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKAAUUCigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAO9FHeigAooooAKKKKACiiigAooooAKO9FHegAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKAAUUCigAooooAKDRQaACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAO9FHeigAooooAKKKKACiiigAooooAKO9FHegAooooAKKKKACgUUCgAooooAKKKKACiiigAooooAKKKKACiiigAooooAK7/wCGn/H3df8AXEf+hVwFd/8ADT/j7uv+uI/9CoA9KooooAK8LvP+P2f/AK6N/OvdK8LvP+P2f/ro386AIazdN/4+b3/rp/jWlWbpv/Hze/8AXT/GgD2n4jf8iJqH+9B/6OSvKK9X+I3/ACImof70H/o5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8LE1L/n1tvyb/GgD0qivNf8AhYmpf8+tt+Tf40f8LE1L/n1tvyb/ABoA9K7U0da83/4WJqX/AD6235N/jSf8LE1L/n1tvyb/ABoA9LorzX/hYmpf8+tt+Tf40f8ACxNS/wCfW2/Jv8aAPSqSvNv+Fial/wA+tt+Tf40f8LE1L/n1tvyb/GgD0k0dq81/4WJqX/Prbfk3+NH/AAsTUv8An1tvyb/GgD0uivNf+Fial/z6235N/jR/wsTUv+fW2/Jv8aAPSqK81/4WJqX/AD6235N/jR/wsTUv+fW2/Jv8aAPSB1p1eaf8LE1L/n1tvyb/ABpf+Fial/z6235N/jQB6VRXmv8AwsTUv+fW2/Jv8aP+Fial/wA+tt+Tf40Aek0GvNv+Fial/wA+tt+Tf40f8LE1L/n1tvyb/GgD0kdKWvNP+Fial/z6235N/jS/8LE1L/n1tvyb/GgD0qivNf8AhYmpf8+tt+Tf40f8LE1L/n1tvyb/ABoA9KpvevN/+Fial/z6235N/jSf8LE1L/n1tvyb/GgD0uivNf8AhYmpf8+tt+Tf40f8LE1L/n1tvyb/ABoA9KorzX/hYmpf8+tt+Tf40f8ACxNS/wCfW2/Jv8aAPST0oHSvNv8AhYmpf8+tt+Tf40n/AAsTUv8An1tvyb/GgD0uivNf+Fial/z6235N/jR/wsTUv+fW2/Jv8aAPSqK81/4WJqX/AD6235N/jR/wsTUv+fW2/Jv8aAPSD1p1eaf8LE1L/n1tvyb/ABpf+Fial/z6235N/jQB6VRXmv8AwsTUv+fW2/Jv8aP+Fial/wA+tt+Tf40AelUnavNv+Fial/z6235N/jR/wsTUv+fW2/Jv8aAPSRS15p/wsTUv+fW2/Jv8aX/hYmpf8+tt+Tf40AelUV5r/wALE1L/AJ9bb8m/xo/4WJqX/Prbfk3+NAHpPekNeb/8LE1L/n1tvyb/ABprfELUyMC3tx7gH/GgD0vOK57X/Fljo0bKHEtz0Eanp9a4O98W6veqVNx5SHqqCsRmMjFnJZj1J5oAffapJqF5Jc3Dlnc5+lV/OT3p+B6CjA9BQBH5yZ6n8az9OlUXN5yeZOOPrWpgegoCKM4UDPXigD2Hxv8A8ihff9s//Ri14/XsHjf/AJFC+/7Z/wDoxa8foAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooASloooAKKKKACiiigD/2Q==)

You will be presented with a dialog box that will ask you whether you want to quantize your sequence. Once you click OK, Decent Sampler will automatically convert the MIDI file into a sequence and add it to the sequence manager. You can then use the sequence in your preset.

7. Close the sequence manager window and save your preset.

## Triggering sequences using MIDI

In order to set up your preset so that specific notes trigger specific sequences, you’ll need to make use of the <midi> element in your <DecentSampler> file. Underneath the <midi> element, you can define any number of <note> elements. Each <note> element has the following attributes:

• note (required): The MIDI note number that will trigger the sequence. This can also be a range of notes, such as 24-35.

• enabled (optional): A boolean value that determines whether the note handler is currently functional. The default is true.

• swallowNotes (optional): A boolean value that determines whether the note handler should swallow the incoming MIDI note or pass it on to the playback engine. The default is false, but most people will want to set this to true.

Within your <note> element, you can define any number of bindings. The bindings we will be using will look something like this:

```xml
<DecentSampler pluginVersion="1" minVersion="1.11.1">
<midi>
<!-- We define a listener for the notes 24 through 35...
<note note="24-35" enabled="true" swallowNotes="true">
<binding enabled="true" level="instrument" type="note_sequence" seqIndex="0"␣
seqLoopMode="forward" seqTriggerBehavior="midi_key" seqTransposeWithRootNote="12"␣
seqTrackMidiInputVelocity="1.0" seqPlaybackRate="1"/>
</note>
</midi>
<noteSequences>
```

(continues on next page)

```xml
(continued from previous page)
<sequence name="Maj1Slow" length="4" rate="1">
<note position="0" velocity="1" note="48" length="1"/>
<note position="1" velocity="1" note="52" length="1"/>
<note position="2" velocity="1" note="55" length="1"/>
<note position="3" velocity="1" note="60" length="1"/>
</sequence>
</noteSequences>
</DecentSampler>
```

(continued from previous page)

Here is a description of all of the useful attributes for the <binding> element:

• enabled (optional): A boolean value that determines whether the binding is currently functional. The default is true.

• level (required): When triggering note sequences, this should always be instrument, as these all live at the instrument level.

• type (required): The type of binding. When triggering note sequences, this should always be note_sequence.

• seqIndex (required): The index of the sequence in the sequence manager that you want to trigger. This is a zero-based index.

• seqFollowGlobalTempo (optional): Whether or not the sequence should follow the global tempo. Valid values are true and false. If this is set to false, then playback will be hardcoded at 120BPM. This can be useful if you want to assure that sequences will always play back at the same rate regardless of the DAW clock.

• seqLoopMode (optional): The loop mode of the sequence. This can be forward, reverse, random, or no_loop.

• seqTriggerBehavior (optional): The trigger behavior of the sequence. this can be midi_key, on, or off. When you are triggering a sequence using a MIDI key, this should always be midi_key.

• seqTransposeWithRootNote (optional): The amount by which the sequence should be transposed when it is triggered, relative to the incoming note. For example, if you set this to 24, and the MIDI note that triggered it was 28, the sequence would be transposed up by 4 semitones, since 28 - 24 = 4.

• seqTrackMidiInputVelocity (optional): A floating point value between 0 and 1 that determines whether the sequence should track the velocity of the incoming MIDI note. The default is 1.0.

• seqPlaybackRate (optional): A floating point value that determines the rate at which the sequence should be played. The default is 1.0.

You can download the example file here and try it out for yourself. You will want to look in the example-006-working-with-note-sequences/Example 6A - Triggering Sequences Using MIDI. dspreset file.

## Triggering note sequences using the UI

In order to trigger note sequences using the UI, you will need to make use of the <ui>, <tab>, and various UI controls elements in your <DecentSampler> file. This tutorial assumes you already have something like this set up. If you don’t, you can refer to the <ui> element section for more information.

<DecentSampler pluginVersion="1" minVersion="1.11.1">   
<ui>   
<tab>   
<!-- This button has two states ->   
<button x="200" y="17" width="100" height="30" style="text" parameterName=   
(continues on next page)

```xml
(continued from previous page)
"Sequencer Enabled" value="0" defaultValue="0">
<!-- In the Off state, we silence any sequence that might be playing that has␣
the identifier "sequence_button_1" -->
<state name="Off">
<binding level="instrument" type="note_sequence" seqIndex="0"
seqTriggerBehavior="off" seqPlayerIdentifier="sequence_button_1"/>
</state>
<!-- In the `On` state, we tell the engine to play the sequence "Maj1Slow"␣
(identifier as sequence with index 0).
We will specify that it should be tracked internally using the identifier
"sequence_button_1". That fact that we are specify this idenfitier
now will allow us to turn the sequence off later. -->
<state name="On">
<binding level="instrument" type="note_sequence" seqIndex="0" seqLoopMode=
"forward" seqTriggerBehavior="on" seqTrackMidiInputVelocity="1.0" seqPlaybackRate="1"
seqTranspose="12" seqPlayerIdentifier="sequence_button_1"/>
</state>
</button>
</tab>
</ui>
<noteSequences>
<sequence name="Maj1Slow" length="4" rate="1">
<note position="0" velocity="1" note="48" length="1"/>
<note position="1" velocity="1" note="52" length="1"/>
<note position="2" velocity="1" note="55" length="1"/>
<note position="3" velocity="1" note="60" length="1"/>
</sequence>
</noteSequences>
</DecentSampler>
```

In this example, we have a button that has two states: Off and On. When the button is in the Off state, we silence any sequence that might be playing that has the identifier sequence_button_1. When the button is in the On state, we tell the engine to play the sequence Maj1Slow. We will specify that it should be tracked internally using the identifier sequence_button_1. That fact that we are specifying this identifier now will allow us to turn the sequence off later.

You can download the example file here and try it out for yourself. You will want to look in the example-006-working-with-note-sequences/Example 6B - Triggering Sequences Using UI. dspreset file.

## Conclusion

In this guide, we went over how to create note sequences by hand and how to import them from MIDI files. We also went over how to trigger sequences using MIDI and the UI. We hope this guide has been helpful to you.

## How to control parameters using tags (Example: Mic-level Knobs)

As of version 1.0.2, the best way to implement mic-level knobs is using the new sample tagging feature. It is possible to assign tags to specific samples. In this way, you can specify which type of sound they are:

<sample volume="0.0dB" tags="note,mic1" />   
<sample volume="0.0dB" tags="rt,mic1" />   
<sample volume="0.0dB" tags="note,mic2" />   
<sample volume="0.0dB" tags="rt,mic2" />

You can also assign tags at the group level. You can also mix and match, and the tags specified at the group level will be added to the list of tags already specified at the sample level:

```xml
<group tags="note">
<sample volume="0.0dB" tags="mic1" />
<sample volume="0.0dB" tags="mic2" />
</group>
<group tags="rt">
<sample volume="0.0dB" tags="mic1" />
<sample volume="0.0dB" tags="mic2" />
</group>
```

Then you can make controls with bindings that reference those tags:

```xml
<control x="246" y="115" parameterName="MIC 1" style="linear_bar_vertical" type="float"␣
minValue="0" maxValue="100" value="60" width="20" height="70" trackForegroundColor=
"FFFFFFFF" trackBackgroundColor="FF888888">
<binding type="amp" level="tag" identifier="mic1" parameter="AMP_VOLUME" />
</control>
<control x="346" y="115" parameterName="MIC 2" style="linear_bar_vertical" type="float"␣
minValue="0" maxValue="100" value="60" width="20" height="70" trackForegroundColor=
"FFFFFFFF" trackBackgroundColor="FF888888">
<binding type="amp" level="tag" identifier="mic2" parameter="AMP_VOLUME" />
</control>
```

## How to do voice-muting for drums

Voice muting makes use of the tags functionality, these are text labels that you can use to identify samples or groups of samples. You start by adding tags to all of your groups (you can also add them to individual samples if you’d like). Next, you add a silencedByTags attribute to groups or sample elements that you want to be silenced by other samples. When a sample with a tag matching one of the tags in the silencedByTags is played, it will silence the current sample (or group).

Here’s an example:

```xml
<DecentSampler>
<groups>
<group tags="hihat" silencedByTags="hihat" silencingMode="fast">
<!-- Your samples go here. -->
</group>
</groups>
</DecentSampler>
```

Note the use of the silencingMode attribute (a value of “fast” means we immediately silence, whereas “normal” means we trigger the ADSR release phase). Alternatively, you can use the silencingDecay attribute to specify an exact fade-out time in seconds. For example, silencingDecay="0.02" creates a 20ms fade-out when hi-hats are muted, which can sound more natural than an immediate cutoff.

## How to implement true legato

Let’s walk through how to build a basic true legato instrument. Legato instruments generally consist of either two or three groups:

1. First, there’s the initial looping sustain sample that will get played when the note is first pressed down.

2. Then there is the legato transition sample that will get played when a second note gets pressed

3. (optional) Depending on the implementation, there may be a third looping sustain group that gets played after the legato transition sample plays. In such cases, this third group usually contains the same samples as were used in group 1.

## Step 1: Voice muting

Before we get into legato, let’s talk about voice muting. This is the behavior wherein one set of samples causes another set of samples to stop playing. This can be desirable in situations such as legato instruments where two samples should not be sounding at the same time.

In Decent Sampler, voice-muting makes use of tags. These are text labels that you can use to identify samples or groups of samples. You can add a silencedByTags attribute to groups or sample elements. This consists of a commaseparated list of tags that specify which samples should silence the current samples. When a sample with a tag matching one of the tags in the silencedByTags is played, it will silence the current sample (or group). Here’s an example:

```xml
<DecentSampler>
<groups>
<group tags="sustain" silencedByTags="legato" silencingMode="normal" release="1.0
">
<!-- Your sustain samples go here. -->
</group>
<group tags="legato" silencedByTags="legato" silencingMode="normal" release="1.0
">
<!-- Your legato samples go here. -->
</group>
</groups>
</DecentSampler>
```

In the above scenario, if a legato sample is matched, any sample that might be playing from the “sustain” group will be stopped.

It’s also worth mentioning the silencingMode attribute (a value of fast means we immediately silence that sample, whereas normal means we trigger the ADSR release phase). Alternatively, you can use the silencingDecay attribute to specify an exact fade-out time in seconds, which overrides the silencingMode behavior. For example, silencingDecay="0.05" creates a 50ms fade-out when voices are silenced.

## Step 2: Legato

In order to specify which samples should be triggered first, we use the trigger attribute. This can that can be added to the <group> or individual <sample> tags. The default value is attack, but there are two useful new values:

• first: This value means that this sample will only trigger if no other notes are playing

• legato: This value means that this sample will only trigger if other notes are already playing

Here is an example of how this might look in cunjunction with our example from above:

```xml
<DecentSampler>
<groups>
<group trigger="first" tags="sustain" silencedByTags="legato" silencingMode=
"normal" release="1.0">
<!-- Your sustain samples go here. -->
</group>
<group trigger="legato" tags="legato" silencedByTags="legato" silencingMode=
"normal" release="1.0">
<!-- Your legato samples go here. -->
</group>
</groups>
</DecentSampler>
```

## Step 3: Specifying previous notes

When creating a legato instrument, it is often essential to limit which legato transition gets played based on which note we are transition from. This can be achieved using either the previousNote or the legatoInterval attributes. The previousNote attribute causes the engine to only play this sample if the previously triggered note matches the specified note. Example usage:

```asp
<sample path="Samples/LV_Legato_F2_G2.wav" rootNote="G2" loNote="G2" hiNote="G2"␣
previousNote="F2" start="43000" />
<sample path="Samples/LV_Legato_G2_A2.wav" rootNote="A2" loNote="A2" hiNote="A2"␣
previousNote="G2" start="43000" />
```

The legatoInterval attribute causes the engine to only play the sample if distance between the current note and the previously triggered note is exactly this semitone distance. For example, if the note for which this sample is being triggered is a C3 and the legatoInterval is set to -2, then the sample will only play if the previous note was a D3 because D3 minus two semitones equals C3.

## Step 4: Polyphony

In legato instruments, it is sometimes useful to limit polyphony for a specific sample or set of samples. This is achieve using tags. At the top-level of your file, you can specify a element as follows:

```xml
<DecentSampler>
<groups>
<group tags="some-tag" >
<!-
</group>
</groups>
<tags>
<tag name="some-tag" polyphony="1" />
</tags>
</DecentSampler>
```

## Putting it all together

Here is a full example of a legato instrument:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<DecentSampler pluginVersion="1">
<groups>
```

(continues on next page)

```xml
from previous page)
<group tags="sustain" silencedByTags="legato" trigger="first"
silencingMode="normal" release="0.3">
<sample path="Samples/LV_Sustain_D3.wav" rootNote="D3" loNote="C3" hiNote="D3"/>
<sample path="Samples/LV_Sustain_E3.wav" rootNote="E3" loNote="D#3" hiNote="E3"/>
<!-- More samples go here -->
</group>
<group tags="legato" silencedByTags="legato" trigger="legato"
silencingMode="normal" attack="0.1" decay="0.2" sustain="0" release="1">
previousNote="C#3"/>
<sample path="Samples/LV_Legato_D3_E3.wav" rootNote="E3" loNote="E3"
hiNote="E3" start="43000" previousNote="D3"/>
<sample path="Samples/LV_Legato_E3_F3.wav" rootNote="F3" loNote="F3"
hiNote="F3" start="43000" previousNote="E3"/>
<!-- More samples go here -->
</group>
<group tags="legato-tails" silencedByTags="legato-tails" trigger="legato"
attack="0.3" silencingMode="normal" release="0.3" start="5000">
<sample path="Samples/LV_Sustain_D3.wav" rootNote="D3" loNote="C3" hiNote="D3"/>
<sample path="Samples/LV_Sustain_E3.wav" rootNote="E3" loNote="D#3" hiNote="E3"/>
<!-- More samples go here -->
</group>
</groups>
</DecentSampler>
```

## How to add Keyswitches

As of DecentSampler 1.4.0, it is now possible to implement keyswitches. For a long time, it’s been possible to trigger events when a MIDI continuous controller event is received: for example, using MIDI CCs we can change knob values or group volumes. Well, it is also possible to trigger events using MIDI notes as well. Here’s what the setup for a MIDI note-based event mapping would look like:

```xml
<midi>
<note note="11">
<binding type="general" level="group" position="0" parameter="ENABLED"
translation="fixed_value" translationValue="true" />
<binding type="general" level="group" position="1" parameter="ENABLED"
translation="fixed_value" translationValue="false" />
</note>
<note note="12">
<binding type="general" level="group" position="0" parameter="ENABLED"
translation="fixed_value" translationValue="false" />
<binding type="general" level="group" position="1" parameter="ENABLED"
translation="fixed_value" translationValue="true" />
</note>
</midi>
```

In this example, MIDI note 11 turns on group 0 and turns off group 1, whereas MIDI note 12 does the opposite. Note the use of the fixed_value translation type.

More documentation on this can be found here.

## How to Use FM Synthesis (fm6op)

DecentSampler includes a built-in 6-operator FM synthesizer that implements the 32 classic algorithm topologies from the Yamaha DX7. You can use it alongside traditional samples, as a pure synthesizer, or anywhere else an <oscillator> element is valid.

## Quick Start

Set waveform="fm6op" on an <oscillator> element and configure the FM parameters on the parent <group>:

```xml
<group fmAlgorithm="5"
fmOp1Ratio="1.0" fmOp1Level="0.9"
fmOp2Ratio="14.0" fmOp2Level="0.5"
fmOp2Attack="0.001" fmOp2Decay="0.4" fmOp2Sustain="0.0" fmOp2Release="0.1"
fmOp6Feedback="0.0"
attack="0.001" decay="0.5" sustain="0.0" release="1.5">
<oscillator waveform="fm6op"
loNote="0" hiNote="127" rootNote="60"
loVel="0" hiVel="127" volume="1.0"/>
</group>
```

## Concepts

## Operators

The fm6op oscillator has six operators numbered 1–6. Each operator is a sine-wave oscillator with:

• A frequency ratio (fmOpNRatio) — multiplies the played note’s frequency.

1.0 = fundamental, 2.0 = one octave up, 0.5 = one octave down.

• An output level (fmOpNLevel) — how much the operator contributes (0.0–1.0).

• A feedback amount (fmOpNFeedback) — how much the operator feeds its own output back into its phase input (0.0–1.0). In the DX7 architecture only one operator per algorithm physically carries the feedback path; see the note on feedback below.

• An independent per-operator ADSR envelope (fmOpNAttack, fmOpNDecay, fmOpNSustain, fmOpNRelease).

• An optional DX7-compatible 4-stage rate/level envelope (fmOpNEgType="dx7" plus fmOpNEgRate1–fmOpNEgLevel4).

## Carriers and Modulators

Each algorithm divides operators into two roles:

• Carrier — its output is added to the final audio signal you hear.

• Modulator — its output is fed into the phase input of another operator, adding harmonics and timbral complexity. Modulators are not heard directly.

## Algorithms

The fmAlgorithm attribute (1–32) selects the routing topology. A higher algorithm number generally means more carriers (brighter, additive sound). A lower number generally means deeper modulation chains (more harmonically rich FM timbres).

Some useful algorithm categories:


| Algorithms | Topology | Character |
| --- | --- | --- |
| 1-8 | Deep chains (1 or 2 carriers) | Rich, complex FM timbres — basses, pads, strings |
| 9-19 | Mixed chains + parallel | Balanced, versatile — pianos, plucks, bells |
| 20-28 | 3+ carriers | Bright, additive-leaning — choir, organs, leads |
| 29-32 | 4–6 carriers | Additive/organ-like — drawbar organs, rich pads |



Algorithm 32 is a special case: all six operators are carriers, each producing a sine wave at its own ratio. This is pure additive synthesis.

## Feedback

Each algorithm designates exactly one operator as the feedback source. In the DX7 this is always Op 6 (the fmOp6Feedback attribute), so fmOp6Feedback is the feedback parameter that works in all 32 algorithms. Setting feedback on other operators has no audible effect unless the active algorithm routes them as the feedback source.

Small feedback values (0.0–0.15) add subtle harmonics and warmth. Larger values (0.3–0.6) create bright sawtooth-like waves. Very high values (0.7–1.0) produce noise and distortion.

## Detune

Each operator can be pitch-detuned using DX7-compatible detune values (fmOpNDetune, range -7 to +7). Detune creates subtle pitch offsets that add richness and movement to the sound:

• 0 = no detune (default)

• Positive values (1–7) = sharpen the pitch slightly

• Negative values (-7 to -1) = flatten the pitch slightly

The DX7 detune algorithm produces larger pitch offsets at lower notes and smaller offsets at higher notes, matching the classic hardware behavior. Common use cases:

• Chorus-like thickness: Set operators at the same ratio but slightly different detune values (e.g., Op1 detune=0, Op2 detune=2).

• Detuned stacks: Create evolving textures by detuning multiple carriers in algorithm 32.

• Bell-like inharmonicity: Slight detunes on modulators add metallic character.

## Fixed Frequency Mode

By default, each operator’s frequency is a ratio of the played note (fmOpNMode="ratio"). You can instead set an operator to fixed frequency mode (fmOpNMode="fixed") and specify an absolute frequency in Hz using fmOpNFixedFreq:

```html
<group fmAlgorithm="1"
fmOp1Ratio="1.0" fmOp1Level="0.8"
fmOp2Mode="fixed" fmOp2FixedFreq="987.5" fmOp2Level="0.4">
```

In this example, Op2 always oscillates at 987.5 Hz regardless of which note is played. This creates:

• Metallic/bell timbres: Fixed-frequency modulators add inharmonic partials.

• Detuned octaves: A fixed carrier at a specific frequency creates tension against the played note.

• Special effects: Ring modulation and clangorous textures.

Fixed frequency is often combined with ratio mode operators in the same patch for hybrid harmonic/inharmonic sounds.

## Velocity Sensitivity

Each operator can respond to MIDI velocity using fmOpNVelocitySensitivity (range 0–7, DX7 convention):

• 0 = no velocity response (default) — operator always plays at full level.

• 1–7 = increasing velocity scaling — level varies from near-zero at vel=1 to full at vel=127.

Velocity sensitivity is most useful on:

• Carriers: Make the overall volume respond to playing dynamics.

• Modulators: Make the brightness/harmonic complexity respond to velocity — lighter touches produce simpler tones, harder strikes add more harmonics.

Example: Velocity-sensitive electric piano (algorithm 5):

```xml
<group fmAlgorithm="5"
fmOp1Ratio="1.0" fmOp1Level="0.85" fmOp1VelocitySensitivity="3"
fmOp2Ratio="14.0" fmOp2Level="0.45" fmOp2VelocitySensitivity="6"
fmOp2Attack="0.001" fmOp2Decay="0.6" fmOp2Sustain="0.0" fmOp2Release="0.1">
<oscillator waveform="fm6op" loNote="0" hiNote="127" rootNote="60"/>
</group>
```

Here, Op1 (carrier) has moderate velocity response (3), while Op2 (modulator) has high velocity response (6) — soft notes are mellow, loud notes are bright.

## Per-Operator Envelopes

Each operator has its own ADSR envelope that shapes how that operator’s contribution evolves over time:

• Carrier envelopes shape the timbre loudness over time (like a VCA).

• Modulator envelopes shape the modulation depth: high modulation at attack = bright attack transient; low modulation after decay = simpler sustained tone.

Sentinel value (-1.0 for release): If you omit a per-operator release, or set it to -1.0, the operator has no independent release — it is fully controlled by the outer <group> ADSR’s release stage. This is the default, so you only need to specify per-operator release when you want the operator to decay independently while the note is held.

## DX7-Compatible 4-Stage Operator Envelope

In addition to the standard ADSR envelope, each operator can use a 4-stage rate/level envelope that matches the parameter conventions of classic 6-operator DX7 synthesizers. This mode is activated per-operator by setting fmOpNEgType="dx7".

The DX7 envelope has four segments, each defined by a rate (how fast the level moves) and a target level (where it stops):


| Stage | Attributes Meaning |  |
| --- | --- | --- |
| Attack | fmOpNEgRate1, fmOpNEgLevel1 | Rise from silence to L1 at speed R1 |
| Decay | fmOpNEgRate2, fmOpNEgLevel2 | Move from L1 to L2 at speed R2 |
| Sustain | fmOpNEgRate3, fmOpNEgLevel3 | Move toward L3 at speed R3; held while key is down |
| Release | fmOpNEgRate4, fmOpNEgLevel4 | Move from current level to L4 at speed R4 after key-off |



All rates and levels use the range 0–99, matching standard DX7 patch banks:

• Rate 99 = near-instant transition. Rate 0 = extremely slow.

• Level 99 = full amplitude. Level 0 = silence.

Default flat envelope (instant attack, infinite sustain, instant release):

R1=99 L1=99 ← instant rise to full   
R2=99 L2=99 ← instant move (stays at full)   
R3=0 L3=99 ← rate 0: holds at L3 indefinitely while key is down   
R4=99 L4=0 ← instant fall to silence after key-off

When fmOpNEgType="dx7" is set, the fmOpNAttack/fmOpNDecay/fmOpNSustain/fmOpNRelease ADSR attributes are ignored for that operator.

## Example: Brass patch with DX7 envelope

Classic brass sounds have a fast attack to L1, a small dip to a slightly lower sustain level, and a moderate release. This example uses algorithm 1, with Op1 as the carrier and Op2 as a modulator with its own shape:

```html
<group fmAlgorithm="1"
attack="0.001" decay="0.0" sustain="1.0" release="0.3"
fmOp1Ratio="1.0" fmOp1Level="0.9"
fmOp1EgType="dx7"
fmOp1EgRate1="99" fmOp1EgLevel1="99"
fmOp1EgRate2="86" fmOp1EgLevel2="94"
fmOp1EgRate3="0" fmOp1EgLevel3="94"
fmOp1EgRate4="60" fmOp1EgLevel4="0"
fmOp2Ratio="1.0" fmOp2Level="0.55"
fmOp2EgType="dx7"
fmOp2EgRate1="99" fmOp2EgLevel1="99"
fmOp2EgRate2="70" fmOp2EgLevel2="60"
fmOp2EgRate3="0" fmOp2EgLevel3="60"
fmOp2EgRate4="55" fmOp2EgLevel4="0"
fmOp6Feedback="0.08">
<oscillator waveform="fm6op" loNote="0" hiNote="127" rootNote="60"
```

(continues on next page)

```xml
loVel="0" hiVel="127" volume="1.0"/>
</group>
```

(continued from previous page)

DX7 rate/level values from standard patch banks can be used directly without conversion — the numbers are the same.

## Per-Operator ADSR Example: DX7 E. Piano Transient

The famous DX7 Electric Piano transient comes from the modulator decaying quickly on attack while the carrier sustains:

```xml
<group fmAlgorithm="5"
attack="0.001" decay="0.5" sustain="0.0" release="1.5"
fmOp1Ratio="1.0" fmOp1Level="0.90"
fmOp2Ratio="14.0" fmOp2Level="0.55"
fmOp2Attack="0.001" fmOp2Decay="0.40" fmOp2Sustain="0.0" fmOp2Release="0.15"
fmOp3Ratio="1.0" fmOp3Level="0.70"
fmOp4Ratio="14.0" fmOp4Level="0.40"
fmOp4Attack="0.001" fmOp4Decay="0.30" fmOp4Sustain="0.0" fmOp4Release="0.12"
fmOp5Ratio="1.0" fmOp5Level="0.55"
fmOp6Ratio="14.0" fmOp6Level="0.30" fmOp6Feedback="0.0"
fmOp6Attack="0.001" fmOp6Decay="0.25" fmOp6Sustain="0.0" fmOp6Release="0.10">
<oscillator waveform="fm6op"
loNote="0" hiNote="127" rootNote="60"
loVel="0" hiVel="127" volume="1.0"/>
</group>
```

Here Op6/Op5/Op4/Op3/Op2 are modulators. The modulator envelopes are short (decay=0.25–0.40, sustain=0) so they decay to zero quickly — creating the bright “tine” transient at the note start. The outer <group> ADSR shapes the overall volume fade.

## Real-Time Modulation with Bindings

All FM operator parameters support real-time modulation. This lets you connect LFOs, envelopes, MIDI CCs, or UI knobs to FM parameters for expressive control.

Example: Knob controls modulation depth (morph timbre)

<ui width="812" height="375">   
<tab>   
<labeled-knob x="350" y="30" width="100" height="100"   
label="FM Depth" parameterName="FMDepth"   
minValue="0" maxValue="1" value="0.5">   
<binding type="general" level="group" position="0"   
parameter="OSCILLATOR_FM_OP2_LEVEL"   
translation="linear"   
translationOutputMin="0.0" translationOutputMax="1.0"/>   
</labeled-knob>   
</tab>   
</ui>

## Supported binding parameters


| Parameter | Range | What it controls |
| --- | --- | --- |
| OSCILLATOR_FM_ALGORITHM | 1-32 | Routing topology (usually set once, not modulated) |
| OSCILLATOR_FM_OP1_LEVEL | 0.0-1.0 | Per-operator modulation depth / carrier |
| OSCILLATOR_FM_OP6_LEVEL OSCILLATOR_FM_OP1_RATIO |  | level |
| OSCILLATOR_FM_OP6_RATIO | Any positive value | Per-operator frequency ratio (detune, octave shifts) |
| OSCILLATOR_FM_OP1_FEEDBACK | 0.0-1.0 | Per-operator self-feedback |
| OSCILLATOR_FM_OP6_FEEDBACK |  |  |



See Appendix B for a full listing.

## Classic Sound Recipes

## Drawbar Organ (Algorithm 32 — all carriers)

Algorithm 32 turns the FM engine into a pure additive synthesizer. Map each operator to a harmonic of the fundamental:

```xml
<group fmAlgorithm="32"
attack="0.001" decay="0" sustain="1.0" release="0.05"
fmOp1Ratio="1.0" fmOp1Level="0.85" fmOp1Release="0.05"
fmOp2Ratio="2.0" fmOp2Level="0.65" fmOp2Release="0.05"
fmOp3Ratio="3.0" fmOp3Level="0.45" fmOp3Release="0.05"
fmOp4Ratio="4.0" fmOp4Level="0.30" fmOp4Release="0.05"
fmOp5Ratio="6.0" fmOp5Level="0.18" fmOp5Release="0.05"
fmOp6Ratio="8.0" fmOp6Level="0.10" fmOp6Feedback="0.08" fmOp6Release="0.05">
<oscillator waveform="fm6op" loNote="0" hiNote="127" rootNote="60"
loVel="0" hiVel="127" volume="1.0"/>
</group>
```

Short per-operator releases (0.05 s) produce the characteristic fast key-off click of a tonewheel organ.

## Bell / FM Marimba (Algorithm 5 — three 2-op pairs)

Inharmonic ratios + all-decay envelopes = percussive bell tones:

<group fmAlgorithm="5"   
attack="0.001" decay="0" sustain="1.0" release="20"   
fmOp1Ratio="1.0" fmOp1Level="1.0"   
fmOp1Attack="0.001" fmOp1Decay="4.0" fmOp1Sustain="0.0" fmOp1Release="2.0"   
fmOp2Ratio="3.5" fmOp2Level="0.35"   
fmOp2Attack="0.001" fmOp2Decay="0.15" fmOp2Sustain="0.0" fmOp2Release="0.1"   
fmOp3Ratio="1.0" fmOp3Level="0.70"   
fmOp3Attack="0.001" fmOp3Decay="3.0" fmOp3Sustain="0.0" fmOp3Release="1.5"   
fmOp4Ratio="3.5" fmOp4Level="0.28"   
fmOp4Attack="0.001" fmOp4Decay="0.12" fmOp4Sustain="0.0" fmOp4Release="0.09"   
fmOp5Ratio="1.0" fmOp5Level="0.50"

(continues on next page)

```xml
(continued from previous page)
fmOp5Attack="0.001" fmOp5Decay="2.0" fmOp5Sustain="0.0" fmOp5Release="1.0"
fmOp6Ratio="3.5" fmOp6Level="0.22" fmOp6Feedback="0.0"
fmOp6Attack="0.001" fmOp6Decay="0.10" fmOp6Sustain="0.0" fmOp6Release="0.08">
<oscillator waveform="fm6op" loNote="0" hiNote="127" rootNote="60"
loVel="0" hiVel="127" volume="1.0"/>
</group>
```

Note the long outer release (release="20") — this keeps the voice alive while the per-operator envelopes decay naturally to silence on their own.

Choir Pad (Algorithm 19 — shared modulator with detuned carriers)

Algorithm 19 routes a single Op6 modulator into both Op5 and Op4. Setting their ratios slightly apart creates a beating, chorus-like quality:

```xml
<group fmAlgorithm="19"
attack="1.0" decay="0" sustain="1.0" release="20"
fmOp4Ratio="1.003" fmOp4Level="0.75"
fmOp4Attack="0.90" fmOp4Decay="0.0" fmOp4Sustain="1.0" fmOp4Release="1.5"
fmOp5Ratio="1.0" fmOp5Level="0.85"
fmOp5Attack="0.90" fmOp5Decay="0.0" fmOp5Sustain="1.0" fmOp5Release="1.5"
fmOp6Ratio="1.0" fmOp6Level="0.30" fmOp6Feedback="0.08"
fmOp6Attack="0.50" fmOp6Decay="0.30" fmOp6Sustain="0.20" fmOp6Release="0.5"
fmOp1Ratio="1.0" fmOp1Level="0.60"
fmOp1Attack="0.80" fmOp1Decay="0.0" fmOp1Sustain="1.0" fmOp1Release="1.5"
fmOp2Ratio="1.0" fmOp2Level="0.50"
fmOp2Attack="0.50" fmOp2Decay="1.0" fmOp2Sustain="0.50" fmOp2Release="1.0"
fmOp3Ratio="3.0" fmOp3Level="0.35"
fmOp3Attack="0.30" fmOp3Decay="0.50" fmOp3Sustain="0.30" fmOp3Release="0.5">
<oscillator waveform="fm6op" loNote="0" hiNote="127" rootNote="60"
loVel="0" hiVel="127" volume="1.0"/>
</group>
```

## Tips

• Start simple. Pick algorithm 5 and set only Op1 (carrier) + Op2 (modulator) to non-default values. Gradually raise fmOp2Level from 0 to 1 to hear FM modulation build from a sine wave to a complex timbre.

• Keep loudness consistent. When many operators have high levels the output can clip. Lower volume on the <group> or reduce carrier levels accordingly.

• Per-operator release tuning. Set per-operator releases independently to make different parts of the spectrum fade at different rates — carriers fade slowly (long release), modulators fade faster (short release) for a natural brightness decay.

• Inharmonic ratios for metallic textures. Try ratios like 3.5, 7.07, 1.41 (sqrt(2)), or other non-integers on modulators for bells, gongs, and metallic sounds.

• Operator 6 first. fmOp6Feedback is nearly always the feedback operator. Start by adjusting this first when you want to add harmonic richness.

• Combine with samples. A group containing <sample> elements can share a track with an fm6op group. Use the groups’ output\* routing to send them to different buses if needed.

## 1.13.3 Developer Tools

## How to Validate a Preset File

DecentSampler includes a built-in Validate Preset tool in the Developer Tools menu. It reads the currently loaded .dspreset file directly from disk and reports any structural or file-reference problems—without modifying the file.

## Accessing the Tool

1. Open a .dspreset file in DecentSampler.

2. Click FILE. . . to open the file menu.

3. Navigate to Developer Tools  Validate Preset.

Note: Validate Preset is only available for non-copy-protected sample libraries. It will not appear in the menu (or will be disabled) when a commercially encrypted preset is loaded.

## What Gets Checked

## 1. Preset File Availability

The tool first confirms that the loaded .dspreset URL points to a real, readable file on disk. If no preset is loaded, or if the file has been moved or deleted since it was last opened, this check will fail with an error.

## 2. XML Validity

The entire file is parsed as XML. Any syntax error—such as an unclosed tag, illegal character, or malformed declaration—is reported with the parser’s own error message. All subsequent checks are skipped if the XML is not well-formed.

## 3. File Path References

The following element/attribute pairs are scanned for file path references:


| Element | Attribute | Typical use |
| --- | --- | --- |
| <sample> | path | Audio sample files |
| <effect> | irFile | Convolution reverb IR files |
| <image> | path | Custom image files |
| <ui> | bgImage | Background image |
| (any) | file | Any other file attribute |



For each reference the validator:

• Resolves the path relative to the directory that contains the .dspreset file.

• Performs a case-insensitive directory and file name search, traversing each path component individually.

• Reports an error if the file cannot be found at all.

• Reports an error if the file exists on disk but with different capitalization than the path written in the preset. This is important because paths that work on case-insensitive file systems (macOS, Windows) will silently break on case-sensitive ones (most Linux distributions and some package-based deployments).

## Reading the Report

The report dialog shows:

• The full path to the preset file on disk.

• Whether the XML parsed successfully.

• A count of errors and warnings.

• A numbered list of every issue, prefixed with [ERROR] or [WARNING], and the exact element, attribute name, and offending path string.

A clean preset produces the message:

Validation passed. No issues were found.

## Common Errors and How to Fix Them

## Missing file

ERROR: Missing file reference: <sample> @path="Samples/Cello_A3.wav"

The file Samples/Cello_A3.wav could not be found relative to the preset directory. Check that the file exists and that the path in the <sample> tag is correct.

## Incorrect capitalization

ERROR: Incorrect path capitalization: <sample> @path="samples/cello_A3.wav" (disk path:   
"Samples/Cello_A3.wav")

The file exists, but the folder name is written as samples in the preset while the real folder on disk is Samples. Update the path attribute to match the exact capitalization on disk, otherwise the preset will fail to load on case-sensitive platforms.