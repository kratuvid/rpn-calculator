:samples/funcs/quadratic.sc file

10 :much var
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
