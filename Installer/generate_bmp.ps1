Add-Type -AssemblyName System.Drawing

$width = 164
$height = 314
$bmp = New-Object System.Drawing.Bitmap($width, $height)
$graphics = [System.Drawing.Graphics]::FromImage($bmp)
$brush = New-Object System.Drawing.SolidBrush([System.Drawing.Color]::FromArgb(255, 30, 30, 30))
$graphics.FillRectangle($brush, 0, 0, $width, $height)
$font = New-Object System.Drawing.Font("Arial", 20, [System.Drawing.FontStyle]::Bold)
$textBrush = New-Object System.Drawing.SolidBrush([System.Drawing.Color]::White)
$graphics.DrawString("DS", $font, $textBrush, 60, 140)
$graphics.Dispose()
$bmp.Save("D:\Development\projects\SamplerEditor\Installer\WizardImage.bmp", [System.Drawing.Imaging.ImageFormat]::Bmp)
$bmp.Dispose()

$width = 55
$height = 55
$bmp = New-Object System.Drawing.Bitmap($width, $height)
$graphics = [System.Drawing.Graphics]::FromImage($bmp)
$brush = New-Object System.Drawing.SolidBrush([System.Drawing.Color]::FromArgb(255, 30, 30, 30))
$graphics.FillRectangle($brush, 0, 0, $width, $height)
$font = New-Object System.Drawing.Font("Arial", 12, [System.Drawing.FontStyle]::Bold)
$textBrush = New-Object System.Drawing.SolidBrush([System.Drawing.Color]::White)
$graphics.DrawString("DS", $font, $textBrush, 12, 18)
$graphics.Dispose()
$bmp.Save("D:\Development\projects\SamplerEditor\Installer\WizardSmallImage.bmp", [System.Drawing.Imaging.ImageFormat]::Bmp)
$bmp.Dispose()
