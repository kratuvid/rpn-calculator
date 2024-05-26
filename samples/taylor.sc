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

5 :_base_sin_cos defun
  :counter var :sol var :den var :num var
  :many var :x var

  1 :alt var

  $many times
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

1 :sin defun
  :x var
  $x 10 $x 1 $x 1 @_base_sin_cos
end

1 :cos defun
  :x var
  $x 10 1 1 1 0 @_base_sin_cos
end

2 :sin_flex defun
  :many var :x var
  $x $many $x 1 $x 1 @_base_sin_cos
end

2 :cos_flex defun
  :many var :x var
  $x $many 1 1 1 0 @_base_sin_cos
end

1 :rad defun
  $pi 180 / *
end

1 :deg defun
  180 $pi / *
end

0 :angle var
25 times
   :deg:~ print $angle top pop
   $angle @rad :angle_rad var
   :rad:~ print $angle_rad top pop

   :sin:~ print $angle_rad @sin top pop
   :cos:~ print $angle_rad @cos top pop

   $angle 15 + :angle set

   :~ println
end-times