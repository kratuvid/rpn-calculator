3 :quadratic-roots defun
  :c var :b var :a var

  $b 2 ^
  4 $a * $c *
  -
  0.5 ^
  :det var

  $b neg $det +
  2 $a *
  /

  $b neg $det -
  2 $a *
  /
end

50 :much var
1 :a var
$much times

  1 :b var
  $much times
  
    1 :c var
    $much times
      $a top pop $b top pop $c top pop
      $a $b $c @quadratic-roots top pop top pop

      $c 1 + :c set
    end-times

    $b 1 + :b set
  end-times

  $a 1 + :a set
end-times
