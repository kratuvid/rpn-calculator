4 :det2x2 begin
  :.22 var :.21 var
  :.12 var :.11 var

  $.11 $.22 *
  $.21 $.12 *
  -
end

9 :det3x3 begin
  :.33 var :.32 var :.31 var
  :.23 var :.22 var :.21 var
  :.13 var :.12 var :.11 var

  $.11
  $.22 $.23 $.32 $.33 @det2x2
  *

  $.12
  $.21 $.23 $.31 $.33 @det2x2
  *
  -

  $.13
  $.21 $.22 $.31 $.32 @det2x2
  *
  +
end

2 -3 5
6 0 4
1 5 -7 @det3x3 top