# The look of this port's PlayStation button pictures: drawn in the style of the PlayStation 3
# versions' prompts (black glossy buttons with a silver rim, the four coloured symbols, dark
# L1/R1/L2/R2, START and SELECT, a black DualShock 3), all this port's own drawings. Dot-sourced by
# make_playstation_icons.ps1, which says where each button goes on the game's page.

Add-Type -AssemblyName System.Drawing

$script:Fonts = New-Object System.Drawing.Text.PrivateFontCollection
$script:Fonts.AddFontFile((Resolve-Path "$PSScriptRoot\..\..\res\fonts\Roboto-Medium.ttf").Path)
$script:Family = $script:Fonts.Families[0]

function NewCanvas([int]$w, [int]$h) {
  $b = New-Object System.Drawing.Bitmap $w, $h, ([System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
  $script:g = [System.Drawing.Graphics]::FromImage($b)
  $g.SmoothingMode = [System.Drawing.Drawing2D.SmoothingMode]::AntiAlias
  $g.TextRenderingHint = [System.Drawing.Text.TextRenderingHint]::AntiAliasGridFit
  $g.PixelOffsetMode = [System.Drawing.Drawing2D.PixelOffsetMode]::HighQuality
  $g.Clear([System.Drawing.Color]::Transparent)
  $b
}
function SaveCanvas($b, [string]$path) {
  $g.Dispose()
  New-Item -ItemType Directory -Force (Split-Path $path) | Out-Null
  $b.Save($path, [System.Drawing.Imaging.ImageFormat]::Png); $b.Dispose()
  "wrote $path"
}

function Color($hex, $a = 255) {
  $v = [Convert]::ToInt32($hex, 16)
  [System.Drawing.Color]::FromArgb($a, ($v -shr 16) -band 255, ($v -shr 8) -band 255, $v -band 255)
}
function Pt([float]$x, [float]$y) { New-Object System.Drawing.PointF $x, $y }
function RoundRect([float]$x, [float]$y, [float]$w, [float]$h, [float]$r) {
  $p = New-Object System.Drawing.Drawing2D.GraphicsPath
  $r = [Math]::Min($r, [Math]::Min($w, $h) / 2); $d = 2 * $r
  $p.AddArc($x, $y, $d, $d, 180, 90); $p.AddArc($x + $w - $d, $y, $d, $d, 270, 90)
  $p.AddArc($x + $w - $d, $y + $h - $d, $d, $d, 0, 90); $p.AddArc($x, $y + $h - $d, $d, $d, 90, 90)
  $p.CloseFigure(); $p
}
function Center($cell) { @(($cell[0] + $cell[2] / 2), ($cell[1] + $cell[3] / 2)) }
function SymbolPen($hex, [float]$width) {
  $p = New-Object System.Drawing.Pen (Color $hex), $width
  $p.StartCap = [System.Drawing.Drawing2D.LineCap]::Round
  $p.EndCap = [System.Drawing.Drawing2D.LineCap]::Round
  $p.LineJoin = [System.Drawing.Drawing2D.LineJoin]::Round
  $p
}
function Label($text, [float]$cx, [float]$cy, [float]$size, $hex = "FFFFFF", [float]$squeeze = 1.0) {
  $path = New-Object System.Drawing.Drawing2D.GraphicsPath
  $fmt = New-Object System.Drawing.StringFormat
  $fmt.Alignment = [System.Drawing.StringAlignment]::Center
  $fmt.LineAlignment = [System.Drawing.StringAlignment]::Center
  $path.AddString($text, $Family, [int][System.Drawing.FontStyle]::Regular, $size, (Pt 0 0), $fmt)
  $m = New-Object System.Drawing.Drawing2D.Matrix
  $m.Translate($cx, $cy); $m.Scale($squeeze, 1.0)
  $path.Transform($m)
  $g.FillPath((New-Object System.Drawing.SolidBrush (Color $hex)), $path)
  $g.DrawPath((New-Object System.Drawing.Pen (Color $hex), ([Math]::Max(0.6, $size / 24))), $path)
}

# A shape in the PS3 prompts' look: a soft shadow, black glossy body, silver rim, highlight.
function Glossy($path, [System.Drawing.RectangleF]$rect, [float]$rim) {
  $shadow = $path.Clone(); $m = New-Object System.Drawing.Drawing2D.Matrix; $m.Translate(1, 1.5)
  $shadow.Transform($m)
  $g.FillPath((New-Object System.Drawing.SolidBrush (Color "000000" 120)), $shadow)
  $g.FillPath((New-Object System.Drawing.Drawing2D.LinearGradientBrush $rect, (Color "3C3C3C"), (Color "050505"), 90), $path)
  $g.DrawPath((New-Object System.Drawing.Pen (Color "C8C8C8"), $rim), $path)
  $gloss = New-Object System.Drawing.RectangleF ($rect.X + $rect.Width * 0.14), ($rect.Y + $rect.Height * 0.08),
    ($rect.Width * 0.72), ($rect.Height * 0.42)
  $g.FillEllipse((New-Object System.Drawing.Drawing2D.LinearGradientBrush $gloss, (Color "FFFFFF" 60), (Color "FFFFFF" 0), 90), $gloss)
}
function Ball([float]$cx, [float]$cy, [float]$r) {
  $rect = New-Object System.Drawing.RectangleF ($cx - $r), ($cy - $r), (2 * $r), (2 * $r)
  $p = New-Object System.Drawing.Drawing2D.GraphicsPath; $p.AddEllipse($rect)
  Glossy $p $rect ([Math]::Max(1.6, $r / 10))
}

# The face buttons: the symbol in its colour on a black button. $kind: Triangle Circle Cross Square.
function FaceButton($cell, [string]$kind, [float]$r) {
  $c = Center $cell; $cx = $c[0]; $cy = $c[1]
  Ball $cx $cy $r
  $w = $r * 0.2; $s = $r * 0.5
  switch ($kind) {
    "Triangle" {
      $g.DrawPolygon((SymbolPen "3ED79E" $w), [System.Drawing.PointF[]]@(
        (Pt $cx ($cy - $s * 1.02)), (Pt ($cx + $s * 1.06) ($cy + $s * 0.78)), (Pt ($cx - $s * 1.06) ($cy + $s * 0.78))))
    }
    "Circle" { $g.DrawEllipse((SymbolPen "FF5A5A" $w), $cx - $s * 0.9, $cy - $s * 0.9, $s * 1.8, $s * 1.8) }
    "Cross" {
      $p = SymbolPen "7FAEFF" $w; $k = $s * 0.86
      $g.DrawLine($p, $cx - $k, $cy - $k, $cx + $k, $cy + $k); $g.DrawLine($p, $cx + $k, $cy - $k, $cx - $k, $cy + $k)
    }
    "Square" { $k = $s * 0.8; $g.DrawRectangle((SymbolPen "F48BD0" $w), $cx - $k, $cy - $k, 2 * $k, 2 * $k) }
  }
}

# A stick, or a button with a label: L, R, L3, R3, PS.
function LabelButton($cell, [string]$text, [float]$r) {
  $c = Center $cell
  Ball $c[0] $c[1] $r
  $size = if ($text.Length -eq 1) { $r * 1.12 } else { $r * 0.9 }
  Label $text $c[0] ($c[1] + $r * 0.04) $size
}

# L1 / R1 (wide, low) and L2 / R2 (a trigger: taller, rounder top).
function Shoulder($cell, [string]$text, [float]$w, [float]$h, [float]$size) {
  $c = Center $cell; $x = $c[0] - $w / 2; $y = $c[1] - $h / 2
  if ($text -like "?1") {
    $path = RoundRect $x $y $w $h ($h * 0.42)
  } else {
    $path = New-Object System.Drawing.Drawing2D.GraphicsPath
    $r = $w * 0.38
    $path.AddArc($x, $y, 2 * $r, 2 * $r, 180, 90)
    $path.AddArc($x + $w - 2 * $r, $y, 2 * $r, 2 * $r, 270, 90)
    $path.AddArc($x + $w - $h * 0.3, $y + $h - $h * 0.3, $h * 0.3, $h * 0.3, 0, 90)
    $path.AddArc($x, $y + $h - $h * 0.3, $h * 0.3, $h * 0.3, 90, 90)
    $path.CloseFigure()
  }
  Glossy $path (New-Object System.Drawing.RectangleF $x, $y, $w, $h) ([Math]::Max(1.6, $h / 16))
  Label $text $c[0] ($c[1] + $h * 0.03) $size
}

# START / SELECT: a black pill with the name.
function NamePill($cell, [string]$text, [float]$w, [float]$h, [float]$size, [float]$squeeze = 1.0) {
  $c = Center $cell; $x = $c[0] - $w / 2; $y = $c[1] - $h / 2
  Glossy (RoundRect $x $y $w $h ($h / 2)) (New-Object System.Drawing.RectangleF $x, $y, $w, $h) ([Math]::Max(1.6, $h / 15))
  Label $text $c[0] ($c[1] + $h * 0.03) $size "FFFFFF" $squeeze
}

# A DualShock 3 seen from above, in a w x h picture: a black glossy body with a white outline
# (filled outline first, so it runs around the whole), its sticks, D-pad, buttons and PS button.
# $outlineOnly: the silhouette (black, white outline, no details).
function DualShock3([float]$w, [float]$h, [bool]$outlineOnly) {
  $sx = $w / 128; $sy = $h / 128
  $shape = New-Object System.Drawing.Drawing2D.GraphicsPath
  $shape.FillMode = [System.Drawing.Drawing2D.FillMode]::Winding
  $m = New-Object System.Drawing.Drawing2D.Matrix
  $shape.AddPath((RoundRect 16 38 96 36 17), $false)        # body
  $shape.AddPath((RoundRect 20 33 28 14 7), $false)         # L1 / R1 tops
  $shape.AddPath((RoundRect 80 33 28 14 7), $false)
  foreach ($side in -1, 1) {                                # grips, rounded and leaning out
    $grip = New-Object System.Drawing.Drawing2D.GraphicsPath
    $grip.AddEllipse(-15, -27, 30, 54)
    $gm = New-Object System.Drawing.Drawing2D.Matrix
    $gm.Translate(64 + $side * 36, 77); $gm.Rotate(-$side * 18)
    $grip.Transform($gm)
    $shape.AddPath($grip, $false)
  }
  $m.Scale($sx, $sy); $shape.Transform($m)
  $outline = New-Object System.Drawing.Pen (Color "FFFFFF"), (7 * $sx)
  $outline.LineJoin = [System.Drawing.Drawing2D.LineJoin]::Round
  $g.DrawPath($outline, $shape)
  $bounds = $shape.GetBounds()
  $g.FillPath((New-Object System.Drawing.Drawing2D.LinearGradientBrush $bounds, (Color "2E2E2E"), (Color "050505"), 90), $shape)
  if ($outlineOnly) { return }
  $grey = New-Object System.Drawing.SolidBrush (Color "5A5A5A")
  $dark = New-Object System.Drawing.SolidBrush (Color "121212")
  $rim = New-Object System.Drawing.Pen (Color "6E6E6E"), (1.2 * $sx)
  foreach ($x in 47, 81) {                                  # sticks
    $g.FillEllipse($dark, ($x - 9) * $sx, 60 * $sy, 18 * $sx, 18 * $sy)
    $g.DrawEllipse($rim, ($x - 9) * $sx, 60 * $sy, 18 * $sx, 18 * $sy)
    $g.FillEllipse((New-Object System.Drawing.SolidBrush (Color "2A2A2A")), ($x - 5.5) * $sx, 63.5 * $sy, 11 * $sx, 11 * $sy)
  }
  foreach ($a in 0, 90, 180, 270) {                         # D-pad: four pads around (32, 53)
    $t = $a * [Math]::PI / 180; $dx = [Math]::Cos($t); $dy = [Math]::Sin($t)
    $cx = 32 + $dx * 5.5; $cy = 53 + $dy * 5.5
    $px = -$dy; $py = $dx
    $g.FillPolygon($grey, [System.Drawing.PointF[]]@(
      (Pt (($cx + $dx * 4 + $px * 2.6) * $sx) (($cy + $dy * 4 + $py * 2.6) * $sy)),
      (Pt (($cx + $dx * 4 - $px * 2.6) * $sx) (($cy + $dy * 4 - $py * 2.6) * $sy)),
      (Pt (($cx - $dx * 2 - $px * 2.6) * $sx) (($cy - $dy * 2 - $py * 2.6) * $sy)),
      (Pt (($cx - $dx * 3.5) * $sx) (($cy - $dy * 3.5) * $sy)),
      (Pt (($cx - $dx * 2 + $px * 2.6) * $sx) (($cy - $dy * 2 + $py * 2.6) * $sy))))
  }
  $buttons = @(@(96, 46, "3ED79E"), @(103, 53, "FF5A5A"), @(96, 60, "7FAEFF"), @(89, 53, "F48BD0"))
  foreach ($b in $buttons) {                                # face buttons: dark with a coloured mark
    $g.FillEllipse($dark, ($b[0] - 4) * $sx, ($b[1] - 4) * $sy, 8 * $sx, 8 * $sy)
    $g.FillEllipse((New-Object System.Drawing.SolidBrush (Color $b[2])), ($b[0] - 1.8) * $sx, ($b[1] - 1.8) * $sy, 3.6 * $sx, 3.6 * $sy)
  }
  $g.FillEllipse($dark, 59.5 * $sx, 51 * $sy, 9 * $sx, 9 * $sy)          # PS button
  $g.DrawEllipse($rim, 59.5 * $sx, 51 * $sy, 9 * $sx, 9 * $sy)
  $g.FillRectangle($grey, 50 * $sx, 47 * $sy, 7 * $sx, 2.6 * $sy)        # SELECT
  $g.FillPolygon($grey, [System.Drawing.PointF[]]@((Pt (71 * $sx) (46.3 * $sy)), (Pt (78 * $sx) (48.3 * $sy)), (Pt (71 * $sx) (50.3 * $sy))))  # START
}

# The D-pad: four black pads around the middle, each pointed toward it, with a grey arrow.
function DPad($cell, [float]$r) {
  $c = Center $cell
  foreach ($a in 0, 90, 180, 270) {
    $t = $a * [Math]::PI / 180; $dx = [Math]::Cos($t); $dy = [Math]::Sin($t); $px = -$dy; $py = $dx
    $at = { param($along, $across) Pt ($c[0] + $dx * $along + $px * $across) ($c[1] + $dy * $along + $py * $across) }
    $w = $r * 0.34
    $path = New-Object System.Drawing.Drawing2D.GraphicsPath
    $path.AddPolygon([System.Drawing.PointF[]]@((& $at ($r * 0.12) 0), (& $at ($r * 0.42) $w),
      (& $at $r $w), (& $at $r (-$w)), (& $at ($r * 0.42) (-$w))))
    Glossy $path $path.GetBounds() ([Math]::Max(1.6, $r / 14))
    $g.FillPolygon((New-Object System.Drawing.SolidBrush (Color "9A9A9A")), [System.Drawing.PointF[]]@(
      (& $at ($r * 0.86) 0), (& $at ($r * 0.62) ($w * 0.5)), (& $at ($r * 0.62) (-$w * 0.5))))
  }
}

# Every PlayStation button, one 128 x 128 square each in a row, in the order of Btn in
# src/pad_icons.cpp (buttons.png): src/pad_icons.cpp draws them, as large as they fit, in the places
# of the Xbox buttons on the game's own button pictures (the in-match prompts and the like).
$ButtonOrder = "Cross", "Circle", "Square", "Triangle", "L1", "R1", "L2", "R2", "L3", "R3",
  "LStick", "RStick", "Start", "Select", "PS", "DPad"
function ButtonSheet([string]$path) {
  $b = NewCanvas (128 * $ButtonOrder.Count) 128
  for ($i = 0; $i -lt $ButtonOrder.Count; $i++) {
    $cell = @((128 * $i), 0, 128, 128); $name = $ButtonOrder[$i]
    switch ($name) {
      { $_ -in "Cross", "Circle", "Square", "Triangle" } { FaceButton $cell $name 58 }
      { $_ -in "L1", "R1" } { Shoulder $cell $name 120 56 38 }
      { $_ -in "L2", "R2" } { Shoulder $cell $name 90 82 44 }
      { $_ -in "L3", "R3", "PS" } { LabelButton $cell $name 58 }
      "LStick" { LabelButton $cell "L" 58 }
      "RStick" { LabelButton $cell "R" 58 }
      "Start" { NamePill $cell "START" 124 46 22 0.9 }
      "Select" { NamePill $cell "SELECT" 124 46 21 0.86 }
      "DPad" { DPad $cell 60 }
    }
  }
  SaveCanvas $b $path
}
