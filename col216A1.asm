# 	Abhinav Singhal		Abhishek Kumar
# 	2019CS50768			2019CS10458

# Major Design considerations :
# for n < 2 return area = 0 Error: Does not make mathematical sense
# we subtract all area below x-axis from the area above x-axis 
# Assumes x coordinates of input points are sorted. If not we raise Bad Input exception

## INPUT INTERFACE 
# Input format: each new individual coordinate will be taken on a newline, for example if the points are (1,2), (3,5) and (7,2) then input will be -
# >> 1
# >> 2
# >> 3
# >> 5
# >> 7
# >> 2
# This is so because taking input of two integer coordinates separated by a whitespace on a single line is not so easy to read.
# Here, ">> " is automatically generated for the user. The user simply has to give the values of the coordinates and press enter/return after each coordinate given.
# Not doing so will result in incorrect input.
# Since we have a finite representation for integer coordinates and area also, we need to specify a bound on input coordinate
# Current Design incorporates Coordinates and Area with limits of 32 bit floating point representation. 

		
# How are we calculating the area? by calculating the modulus of area of trapezium formed by 2 consecutive points and the x axis if both points are on the same side,
# else calculating the modulus of the area of the two triangles formed on both the sides of the x-axis by simple math. 
	
	#Begin 
	.text
	.globl main
	.ent main
	
	
# $t0 = n 
# $t1 = previous x coordinate
# $t2 = previous y coordinate 
# $t3 = current x coordinate
# $t4 = current y coordinate 
# $t5 = one Trapezium Area 
# $t6 = $t3 - $t1 
# $t7 = $t2 + $t4 


########## Main Begin ############
main:
	#print String "n: " 
	li $v0,4 #code for syscall is 4 and v0 has been used;
	la $a0, msg1 
	syscall
	
	#get int 'n' input
	li $v0,5
	syscall
	move $t0,$v0
	
	blt $t0,2, NumPtsLessThan2  # give error if number of points is less than 2.
	
	#print String "Enter x-coordinate " 
	li $v0,4
	la $a0, msg2
	syscall
	
	#get int input: first x coordinate 
	li $v0,5
	syscall
	move $t1,$v0
	
	#print String "Enter y-coordinate " 
	li $v0,4
	la $a0, msg3
	syscall
	
	#get int input: first y coordinate 
	li $v0,5
	syscall
	move $t2,$v0
	
	sub $t0,$t0,1  # we have read one point already. only n-1 left to be taken
	
while:
	beq $t0,0,giveValidOutput
	
	#print String "Enter x-coordinate " 
	li $v0,4
	la $a0, msg2
	syscall
	
	#get int input: next x coordinate 
	li $v0,5
	syscall
	move $t3,$v0
	
	bgt $t1,$t3, unsortedxError		# give error if input points are not sorted according to x-coordinate.
	
	#print String "Enter y-coordinate " 
	li $v0,4
	la $a0, msg3
	syscall
	
	#get int input: next y coordinate 
	li $v0,5
	syscall
	move $t4,$v0
	
	mul $t9, $t4, $t2
	blt $t9, 0, signChangeYCase
	
	sub $t0,$t0,1  # one more input point read successfully
	sub $t6,$t3,$t1	# current x - prev x coordinate 
	add $t7,$t2,$t4 # current y + prev y coordinate 
	
	mul $t5,$t6,$t7	# product of the above two into $t5
	
	abs $t5, $t5
	
	mtc1 $t5, $f6
	cvt.s.w $f6, $f6
	add.s $f8, $f8, $f6	# add this to calc area 
	j moving
	
	
moving:
	move $t1,$t3	# current x = next x
	move $t2,$t4 	# current y = next y
	j while
	
signChangeYCase:
	sub $t0, $t0, 1	#decrease counter
	sub $t6, $t3, $t1	#(current x - previous x)=height
	mul $t8, $t2, $t2	#previous y square
	mul $t9, $t4, $t4	#current y square
	add $t7, $t8, $t9	#current y squared + previous y squared
	mul $t5, $t6, $t7	#height*(y1^2+y2^2)=a (suppose)
	sub $t7, $t4, $t2	#(y2-y1)
	mtc1 $t7, $f10		#(y2-y1) sent to float register
	cvt.s.w $f10, $f10	#(y2-y1) converted to float
	mtc1 $t5, $f12		#a sent to float register
	cvt.s.w $f12, $f12	#a converted to float
	div.s $f12, $f12, $f10	#a/(y1-y2)=2*area
	abs.s $f12, $f12	#converted to positive incase it was negative
	add.s $f8, $f8, $f12	#added it to final area
	j moving		

	
	

	
############### Main End #####################
############### Giving outputs Begin #########
giveValidOutput:
	l.s $f2, two
	div.s $f8, $f8, $f2		# now $f8 contains calc area by 2
	l.s $f2, hundred
	mul.s $f8, $f8, $f2
	cvt.w.s $f8, $f8
	mfc1 $t8, $f8
	li $t9, 100
	div $t8, $t9
	#print String "Area: " 
	li $v0,4
	la $a0, msg4
	syscall
	
	#print int
	li $v0, 1
	mflo $a0	# integer part of area 
	syscall
	
	li $v0,4
	la $a0,msg5
	syscall
	
	li $v0, 1
	mfhi $a0
	syscall 
	
	j end
	
######### Give Outputs End ############
######### Error Handling Begin ########

unsortedxError:
	#print String
	li $v0,4
	la $a0, errorMsg2
	syscall
	j end 
	
NumPtsLessThan2:
	#print String
	li $v0,4 
	la $a0, errorMsg1 
	syscall
	j end 
	
	
######## Error Handling End ##########
end:
	#print String
	li $v0,4
	la $a0, LF
	syscall
	
	li $v0, 10	# smooth exit code
	syscall
	
	
	.data
msg1: .asciiz "n: "
msg2: .asciiz "Enter x-coordinate >> "
msg3: .asciiz "Enter y-coordinate >> "
msg4: .asciiz "Area: "
msg5: .asciiz "."
LF: .asciiz "\n"
two: .float 2.0
hundred: .float 100.0

errorMsg1: .asciiz "badInputException :: Given n is less than 2 -> Does not make mathematical sense -> Program Terminated"
errorMsg2: .asciiz "badInputException :: Given input points are not x-sorted -> Program Terminated"
