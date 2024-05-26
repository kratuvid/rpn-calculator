1 :e-x defun
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
  $x 30 $x 1 $x 1 @_base_sin_cos
end

1 :cos defun
  :x var
  $x 30 1 1 1 0 @_base_sin_cos
end

2 :sin-flex defun
  :many var :x var
  $x $many $x 1 $x 1 @_base_sin_cos
end

2 :cos-flex defun
  :many var :x var
  $x $many 1 1 1 0 @_base_sin_cos
end

1 :atan defun
  :x var
  $x $x * :x-sq var
  1 :alt var
  $x :res var
  $x :num var
  1 :den var

  80 times
  	 $alt neg :alt set
	 $num $x-sq * :num set
	 $den 2 + :den set

	 $num $alt * $den /
	 $res + :res set
  end-times

  $res
end

1 :rad defun
  0.017453292519943295 *
end

1 :deg defun
  57.29577951308232 *
end

70 :iter var 0 :angle var
0 times
   :--- println

   $angle @rad :angle_rad var
   :deg:` print $angle top pop
   :rad:` print $angle_rad top pop
   :` println

   $angle_rad sin :sin_real var
   $angle_rad cos :cos_real var
   $angle_rad $iter @sin_flex :sin_approx var
   $angle_rad $iter @cos_flex :cos_approx var
   $sin_real $sin_approx - :sin_error var
   $cos_real $cos_approx - :cos_error var

   :sin:` print $sin_real top pop
   :approx:` print $sin_approx top pop
   :error:` print $sin_error top pop
   :` println

   :cos:` print $cos_real top pop
   :approx:` print $cos_approx top pop
   :error:` print $cos_error top pop
   :--- println

   $angle 15 + :angle set
end-times
:iter del :angle del