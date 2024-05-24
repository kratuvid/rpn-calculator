1 :e_x defun
  :x var

  1 :numerator var
  1 :denominator var
  1 :result var

  1 :counter var
  15 times
  	 $numerator $x * :numerator set
	 $denominator $counter * :denominator set

	 $numerator $denominator / $result + :result set

	 $counter 1 + :counter set
  end-times

  $result
end

2 @e_x top