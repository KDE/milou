var isHighDPI = theme.defaultFont.mSize.height >= 20

// Normal
var CategoryWidthNormal = 120
var PlasmoidWidthNormal = 450
var IconSizeNormal = 16

// High DPI
var CategoryWidthHighDPI = 220
var PlasmoidWidthHighDPI = 650
var IconSizeHighDPI = 32

// This is the width of the side component which displays the categories
// of the results, such as "Applications", "Audio", "Video", etc
var CategoryWidth = isHighDPI ? CategoryWidthHighDPI : CategoryWidthNormal
var CategoryRightMargin = 10

// The Maximum and Minimum width of the Plasmoid
var PlasmoidWidth = isHighDPI ? PlasmoidWidthHighDPI : PlasmoidWidthNormal

var IconSize = isHighDPI ? IconSizeHighDPI : IconSizeNormal
