:samples/funcs/taylor.sc file

70 :iter var 0 :angle var
25 times
   $angle @rad :angle-rad var
   :deg:` print $angle top pop
   :rad:` print $angle-rad top pop
   :` println

   $angle-rad sin :sin-real var
   $angle-rad cos :cos-real var
   $angle-rad $iter @sin-flex :sin-approx var
   $angle-rad $iter @cos-flex :cos-approx var
   $sin-real $sin-approx - :sin-error var
   $cos-real $cos-approx - :cos-error var

   :sin:` print $sin-real top pop
   :approx:` print $sin-approx top pop
   :error:` print $sin-error top pop
   :` println

   :cos:` print $cos-real top pop
   :approx:` print $cos-approx top pop
   :error:` print $cos-error top pop
   :--- println

   $angle 15 + :angle set
end-times
:iter del :angle del