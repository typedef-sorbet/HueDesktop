function calc_alpha(r, g, b)
{
    // interpret rgb as a 3D vector, calculate magnitude
    var res = Math.sqrt(r*r + g*g + b*b) / 441.6729559300637
    console.log("Alpha: " + res)
    return res
}
