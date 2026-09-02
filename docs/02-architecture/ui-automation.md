# Driving the UI from outside the process

The app has no automation hooks of its own — no debug server, no socket, no `QWebChannel`,
nothing embedded in the code. External UI automation is still possible, and works today,
through the OS accessibility layer: **Windows UI Automation (UIA)**, driven from PowerShell
via the `System.Windows.Automation` / `UIAutomationClient` / `UIAutomationTypes` assemblies.
This is generic — it works against any native Win32/Qt window, not something this project
had to add support for — but Qt's accessibility bridge (`QAccessible`) is what makes the
widget tree visible to it at all, so it is worth documenting here rather than treating each
rediscovery as a one-off.

Confirmed end-to-end on 2026-09-01: launch `Deploy\DecentSamplerEditor.exe`, open a project
from the Library grid, and select through all five `MainTabBar` tabs, screenshotting each.

## Five things that cost real time to get right

**`Process.MainWindowHandle` hands you the splash screen if you ask too early.** The splash
is a real top-level window (`ClassName == "SplashScreen"`) owned by the same PID, and
`AutomationElement.FromHandle` on it succeeds but returns an element with an empty `Name`,
empty `ClassName`, and zero descendants — which looks like a bug in the query rather than
the wrong window. Enumerate top-level windows for the PID from the desktop root instead, and
explicitly skip `ClassName -eq 'SplashScreen'`, polling until a window with a real size
(`Width -gt 200 -and Height -gt 200`) appears:

```powershell
$desktop = [System.Windows.Automation.AutomationElement]::RootElement
$pidCond = New-Object System.Windows.Automation.PropertyCondition(
    [System.Windows.Automation.AutomationElement]::ProcessIdProperty, $p.Id)
# poll $desktop.FindAll([System.Windows.Automation.TreeScope]::Children, $pidCond),
# skipping ClassName -eq 'SplashScreen', until one shows up with a real bounding rect
```

**`AutomationId` is not unique across `MainTabBar`'s children.** Every `TabItem` under it
(Libraries, Sampling, Node Map, UI Designer, Code Editor) reports the same `AutomationId`
(`...CustomTitleBar.MainTabBar`); only `Current.Name` — the visible label, e.g.
`"3. Node Map"` — distinguishes them. Select tabs by `ControlType` + `Name`, not by
`AutomationId`. Don't assume any other control's `AutomationId` is unique either without
checking the actual enumerated output first.

**Prefer semantic patterns; keep a raw-click fallback.** `SelectionItemPattern.Select()`
works cleanly on `MainTabBar`'s `TabItem`s and needed no fallback in the confirmed run.
`GetCurrentPattern` throws when a pattern isn't supported, so wrap it in try/catch and fall
back to a real click at the element's `BoundingRectangle` center via `user32.dll`
`SetCursorPos` + `mouse_event` (`0x0002`/`0x0004` for left down/up) when it does. The
fallback is the one that always works, because it's OS input rather than app cooperation —
worth keeping even where the semantic pattern currently succeeds, in case a future widget
doesn't wire it up.

**Screenshot with `PrintWindow` + `PW_RENDERFULLCONTENT` (flag `2`), not
`SetForegroundWindow` plus a region grab.** `SetForegroundWindow` is subject to Windows'
focus-stealing prevention and is not reliable to call from an unattended script.
`PrintWindow` with `PW_RENDERFULLCONTENT` captured the Node Map's canvas correctly in
testing — that view renders its own content rather than using stock widget painting, and is
exactly the kind of surface a plain `PrintWindow` (flag `0`) can render blank for:

```csharp
[DllImport("user32.dll")] static extern bool PrintWindow(IntPtr hwnd, IntPtr hdc, uint flags);
// flags = 2 (PW_RENDERFULLCONTENT)
```

**The whole flow has to run in a single script invocation.** A process started by one
PowerShell invocation does not reliably survive to be driven by a second, separate
invocation — consistent with per-invocation job-object cleanup in sandboxed shells. Launch,
wait for the real window, interact, screenshot, and `Stop-Process` all belong in one script.

## Minimal skeleton

```powershell
Add-Type -AssemblyName UIAutomationClient, UIAutomationTypes, System.Drawing
Add-Type @'
using System;
using System.Runtime.InteropServices;
public class Win32Input {
    [DllImport("user32.dll")] public static extern bool SetCursorPos(int x, int y);
    [DllImport("user32.dll")] public static extern void mouse_event(uint flags, uint dx, uint dy, uint dwData, IntPtr extraInfo);
    [DllImport("user32.dll")] public static extern bool PrintWindow(IntPtr hwnd, IntPtr hdc, uint flags);
    public static void Click(int x, int y) {
        SetCursorPos(x, y); System.Threading.Thread.Sleep(100);
        mouse_event(0x0002, 0, 0, 0, IntPtr.Zero); System.Threading.Thread.Sleep(60);
        mouse_event(0x0004, 0, 0, 0, IntPtr.Zero);
    }
}
'@
# launch $p = Start-Process ... -PassThru
# poll desktop children for $p.Id, skipping SplashScreen, for the real MainWindow
# FindAll(Descendants) from that root, select by ControlType + Name
# GetCurrentPattern(SelectionItemPattern/InvokePattern) in try/catch, else Win32Input.Click
# Save-Shot: Bitmap sized to BoundingRectangle, Graphics.GetHdc(), PrintWindow(hwnd, hdc, 2)
# Stop-Process -Id $p.Id -Force
```

Use a **single-quoted** here-string (`@' ... '@`) for the embedded C#/P-Invoke block, so
PowerShell never tries to interpolate anything inside it.

*Last verified: 2026-09-01*
