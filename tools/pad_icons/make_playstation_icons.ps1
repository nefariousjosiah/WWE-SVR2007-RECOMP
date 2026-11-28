# Draws the PlayStation button pictures (res/pad_icons/*.png) that the game shows in place of its
# Xbox ones when a PlayStation controller is in use (src/pad_icons.cpp), in the style of the
# PlayStation 3 prompts (tools/pad_icons/ps3_style.ps1). All are this port's own drawings.
#
# playstation.png has the size of the fonts' button icon page (menuHD.pac: 256 x 512) and holds the
# drawings at the places of the Xbox buttons they stand in for; the rest is transparent, and the
# game's page keeps everything else (the D-pad, stars, WWE logo, and the arrows beside two of the
# stick pictures). SvR 2007's Guide button picture becomes a PS button. Two fonts have pages of
# their own: font 40's has narrower LT / RT and wider LB / RB spots (playstation_40.png), and the
# FNTM font's is 512 x 512 with the sticks with arrows besides (playstation_512.png). The
# controller pictures of the controls screen and their silhouettes are replaced whole.
#   powershell -ExecutionPolicy Bypass -File tools\pad_icons\make_playstation_icons.ps1
param([string]$Dir = "$PSScriptRoot\..\..\res\pad_icons")
$ErrorActionPreference = "Stop"
. "$PSScriptRoot\ps3_style.ps1"

# The places of the Xbox buttons on the game's page (x, y, width, height), from its icon glyphs.
# LSUpDown and LSSides are the stick parts of the "LS with arrows" pictures (U+E01A, U+E019).
$Cells = [ordered]@{
  Square   = @(1, 0, 55, 56);    Triangle = @(57, 0, 55, 56)
  Circle   = @(113, 0, 55, 56);  Cross    = @(169, 0, 55, 56)
  Select   = @(0, 56, 58, 56);   Start    = @(59, 56, 58, 56);   PS = @(178, 56, 57, 56)
  L2       = @(0, 112, 52, 56);  R2       = @(53, 112, 52, 56)
  L1       = @(106, 112, 70, 56); R1      = @(177, 112, 70, 56)
  LStick   = @(0, 168, 61, 56);  RStick   = @(62, 168, 60, 56)
  L3       = @(176, 168, 59, 56); R3      = @(198, 224, 56, 56)
  LSUpDown = @(0, 224, 58, 56);  LSSides  = @(119, 224, 51, 56)
}

# All the page's buttons; $l2w: the width of L2 / R2.
function DrawButtons($cells, [float]$l2w) {
  foreach ($k in "Triangle", "Circle", "Cross", "Square") { FaceButton $cells[$k] $k 23 }
  LabelButton $cells.LStick "L" 22; LabelButton $cells.RStick "R" 22
  LabelButton $cells.L3 "L3" 22; LabelButton $cells.R3 "R3" 22
  LabelButton $cells.LSUpDown "L" 22; LabelButton $cells.LSSides "L" 22
  LabelButton $cells.PS "PS" 23
  Shoulder $cells.L1 "L1" 62 30 20; Shoulder $cells.R1 "R1" 62 30 20
  Shoulder $cells.L2 "L2" $l2w 44 19; Shoulder $cells.R2 "R2" $l2w 44 19
  NamePill $cells.Select "SELECT" 56 26 12 0.84; NamePill $cells.Start "START" 56 26 12.5 0.86
}

$b = NewCanvas 256 512
DrawButtons $Cells 46
SaveCanvas $b "$Dir\playstation.png"

# Font 40's page (FNT2 and FNTG font 40): the same but for the shoulder buttons' row.
$Cells40 = [ordered]@{}
foreach ($k in $Cells.Keys) { $Cells40[$k] = $Cells[$k] }
$Cells40.L2 = @(0, 112, 39, 56); $Cells40.R2 = @(40, 112, 41, 56)
$Cells40.L1 = @(82, 112, 82, 56); $Cells40.R1 = @(165, 112, 82, 56)
$b = NewCanvas 256 512
DrawButtons $Cells40 36
SaveCanvas $b "$Dir\playstation_40.png"

# A stick's top in the PS3 look (black, silver rim) with its letter, 33 x 23 around (cx, cy): laid
# over the game's stick in place of its "LS" / "RS" (24 x 14), inside the stick, so the arrows
# around stay.
function StickTop([float]$cx, [float]$cy, [string]$text) {
  $w = 33; $h = 23; $x = $cx - $w / 2; $y = $cy - $h / 2
  $rect = New-Object System.Drawing.RectangleF $x, $y, $w, $h
  $path = RoundRect $x $y $w $h 7
  $body = New-Object System.Drawing.Drawing2D.LinearGradientBrush $rect, (Color "3C3C3C"),
    (Color "050505"), 90
  $g.FillPath($body, $path)
  $g.DrawPath((New-Object System.Drawing.Pen (Color "C8C8C8"), 1.4), $path)
  $gloss = New-Object System.Drawing.RectangleF ($x + $w * 0.14), ($y + $h * 0.08), ($w * 0.72),
    ($h * 0.42)
  $shine = New-Object System.Drawing.Drawing2D.LinearGradientBrush $gloss, (Color "FFFFFF" 60),
    (Color "FFFFFF" 0), 90
  $g.FillEllipse($shine, $gloss)
  Label $text $cx ($cy + 0.5) 17
}

# The FNTM font's page (512 x 512): its top left is the page above again; below that and on the
# right are the sticks with arrows, 78 x 76 each (LS: three columns from x 0, rows from y 280; RS:
# three columns from x 256, rows from y 0). src/pad_icons.cpp lays the stick tops over the game's.
$b = NewCanvas 512 512
DrawButtons $Cells 46
for ($row = 0; $row -lt 3; $row++) {
  for ($col = 0; $col -lt 3; $col++) {
    StickTop (42 + 78 * $col) (317 + 76 * $row) "L"
    StickTop (299.5 + 78 * $col) (37 + 76 * $row) "R"
  }
}
SaveCanvas $b "$Dir\playstation_512.png"

# The controls screen's controller (menuHD.pac contro_01A, 128 x 128, and contro_02A, 256 x 128) and
# the silhouettes drawn behind them (contro_01B / 02B): a DualShock 3 in the same place. The wide
# ones hold a bigger controller, as tall as the picture.
foreach ($outline in $false, $true) {
  $suffix = if ($outline) { "_shadow" } else { "" }
  $b = NewCanvas 128 128
  DualShock3 128 128 $outline
  SaveCanvas $b "$Dir\controller$suffix.png"
  $b = NewCanvas 256 128
  $scale = 1.56
  $g.TranslateTransform(127.5 - 64 * $scale, 63.5 - 68.5 * $scale)
  DualShock3 (128 * $scale) (128 * $scale) $outline
  SaveCanvas $b "$Dir\controller_wide$suffix.png"
}

# The loading tips' pictures (game2dHD.pac TIPS / TIPI, 1024 x 1024). The controller diagram's
# controller: its place painted the panel's black and given a DualShock 3 (tip_controller.png; the
# lines to its buttons stay, up to the controller). The screenshots' stick and button pictures:
# ours laid over them, as big (tip_sticks.png, tip_stop.png).
function At([float]$cx, [float]$cy) { @(($cx - 1), ($cy - 1), 2, 2) }
$b = NewCanvas 1024 1024
$g.FillRectangle((New-Object System.Drawing.SolidBrush (Color "050002" 214)), 356, 146, 314, 216)
$scale = 2.84
$g.TranslateTransform(513 - 64 * $scale, 254 - 68.5 * $scale)
DualShock3 (128 * $scale) (128 * $scale) $false
SaveCanvas $b "$Dir\tip_controller.png"
$b = NewCanvas 1024 1024
LabelButton (At 625.5 146.5) "L" 23; LabelButton (At 731 146.5) "R" 23
LabelButton (At 650.5 422) "L" 22; LabelButton (At 698 422) "R" 22
SaveCanvas $b "$Dir\tip_sticks.png"
$b = NewCanvas 1024 1024
FaceButton (At 652 359) "Circle" 11.5; LabelButton (At 653 391) "R" 20.5
SaveCanvas $b "$Dir\tip_stop.png"

# Every button on its own, for the game's other button pictures (src/pad_icons_tables.h, kPrompts).
ButtonSheet "$Dir\buttons.png"
