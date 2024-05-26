0 :counter var
0 :counter2 var

10 :many var
$many times
  $counter 1 + :counter set
  10 times
  	 $counter2 1 + :counter2 set
  end-times
end-times

$counter top
$counter2 top