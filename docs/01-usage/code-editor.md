# Code Editor

The **Code Editor** tab (`CodeEditorView`) displays the generated preset source code in real time as you edit your instrument visually.

## Features

- **Format Selector:** Switch between **Decent Sampler (XML)** (`.dspreset`) and **SFZ** export formats.
- **Syntax Highlighting:** Custom highlighters (`XmlHighlighter` and `SfzHighlighter`) provide colorized code for tags, attributes, values, comments, and headers.
- **Refresh:** Click **Refresh** to manually re-transpile the project model into code at any time.
- **Copy to Clipboard:** Click **Copy to Clipboard** to copy the full generated text for external use or manual testing.

## Real-Time Inspection

The code view is read-only and always mirrors the state of your project. Any changes made in the **Sampling**, **Node Map**, or **UI Designer** tabs immediately update the generated code when transpiled.

*Last verified: 2026-08-18*
