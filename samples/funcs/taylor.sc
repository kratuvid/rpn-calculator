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

  50 times
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