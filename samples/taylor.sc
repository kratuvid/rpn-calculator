:xx var
1 :x var

1 :denominator var
1 :counter var

1 1 / :result var


5 times
  $x $xx * :x var
  $denominator $counter * :denominator var

  $x $denominator / $result + :result var

  $counter 1 + :counter var
end-times

vars