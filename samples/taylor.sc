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

5 :_base_sin_cos_x defun
  :counter var :sol var :den var :num var
  :x var

  1 :alt var

  15 times
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

1 :sin_x defun
  :x var
  $x $x 1 $x 1 @_base_sin_cos_x
end

1 :cos_x defun
  :x var
  $x 1 1 1 0 @_base_sin_cos_x
end

1 :rad defun
  $pi 180 / *
end

1 :deg defun
  180 $pi / *
end

0 :angle var
25 times
   $angle top pop
   $angle @rad :angle_rad var

   $angle_rad cos top pop
   $angle_rad @cos_x top pop

   $angle 15 + :angle set
end-times