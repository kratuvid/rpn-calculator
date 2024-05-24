1 :e_x defun
  :x var

  1 :numerator var
  1 :denominator var
  1 :result var

  1 :counter var
  10 times
  	 $numerator $x * :numerator set
	 $denominator $counter * :denominator set

	 $numerator $denominator / $result + :result set

	 $counter 1 + :counter set
  end-times

  $result
end

1 :sin_x defun
  :x var

  1 :alt var
  $x :num var
  1 :den var
  $x :sol var

  1 :counter var
  5 times
	 $alt neg :alt set
  	 $num $x * $x * :num set
  	 $den $counter 1 + * $counter 2 + * :den set

	 $sol
	 $alt $num *
	 $den
	 /
	 + :sol set

  	 $counter 2 + :counter set
  end-times

  $sol
end

1 :rad defun
  $pi 180 / *
end

1 :deg defun
  180 $pi / *
end

180 @rad @sin_x
stack
clear