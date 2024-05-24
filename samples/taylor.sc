:x var

1 :numerator var
1 :denominator var

1 :result var

1 :counter var
15 times
  $numerator $x * :numerator varg
  $denominator $counter * :denominator varg

  $numerator $denominator / $result + :result varg

  $counter 1 + :counter varg
end-times

vars