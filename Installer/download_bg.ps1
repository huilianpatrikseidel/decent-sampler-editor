Add-Type -AssemblyName System.Drawing

$url = "https://jrsoftware.org/images/is-welcome-stellar-backimage-org-150.png"
$imgFile = "D:\Development\projects\SamplerEditor\Installer\stellar-bg.png"
$bmpFile = "D:\Development\projects\SamplerEditor\Installer\WizardImage.bmp"

Invoke-WebRequest -Uri $url -OutFile $imgFile
$img = [System.Drawing.Image]::FromFile($imgFile)
$img.Save($bmpFile, [System.Drawing.Imaging.ImageFormat]::Bmp)
$img.Dispose()
