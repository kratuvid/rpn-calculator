3 :quadratic_roots begin
  :=c var :=b var :=a var

  $=b 2 ^
  4 $=a * $=c *
  -
  0.5 ^
  :=det var

  $=b neg $=det +
  2 $=a *
  /

  $=b neg $=det -
  2 $=a *
  /

  :=c del :=b del :=a del
  :=det del
  stack
end

@quadratic_roots