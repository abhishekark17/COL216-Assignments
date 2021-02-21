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
# Not doing so will result in incorrect input. How to detect such incorrect input ?? still need to incorporate in the code. 
# Since we have a finite representation for integer coordinates and area also, we need to specify a bound on input coordinate
# Current Design incorporates Coordinates and Area with limits of 32 bit representation. 

		
# How are we calculating the area? by calculating the area of trapezium formed by 2 consecutive points and the x axis.
	
	#Begin 
	.text
	.globl main
	
	
# $t0 = n 
# $t1 = previous x coordinate
# $t2 = previous y coordinate 
# $t3 = current x coordinate
# $t4 = current y coordinate 
# $t5 = one Trapezium Area 
# $t6 = $t3 - $t1 
# $t7 = $t2 + $t4 
# $t8 = calc area 
# $t9 = Remainder (calc Area / 2)

#in the end we will divide by 2.


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
	
	#print String ">> " 
	li $v0,4
	la $a0, msg2
	syscall
	
	#get int input: first x coordinate 
	li $v0,5
	syscall
	move $t1,$v0
	
	#print String ">> " 
	li $v0,4
	la $a0, msg2
	syscall
	
	#get int input: first y coordinate 
	li $v0,5
	syscall
	move $t2,$v0
	
	sub $t0,$t0,1  # we have read one point already. only n-1 left to be taken
	
while:
	beq $t0,0,giveValidOutput
	
	#print String ">> " 
	li $v0,4
	la $a0, msg2
	syscall
	
	#get int input: next x coordinate 
	li $v0,5
	syscall
	move $t3,$v0
	
	bgt $t1,$t3, unsortedxError
	
	#print String ">> " 
	li $v0,4
	la $a0, msg2
	syscall
	
	#get int input: next y coordinate 
	li $v0,5
	syscall
	move $t4,$v0
	
	sub $t0,$t0,1  # one more input point read successfully
	sub $t6,$t3,$t1	# current x - prev x coordinate 
	add $t7,$t2,$t4 # current y + prev y coordinate 
	
	mul $t5,$t6,$t7	# product of the above two into $t5 
	add $t8,$t8,$t5	# add this to calc area 
	
	move $t1,$t3	# current x = next x
	move $t2,$t4 	# current y = next y
	j while

	
############### Main End #####################
############### Giving outputs Begin #########
giveValidOutput:
	rem $t9, $t8, 2		# $t9 contains remainder 
	div $t8, $t8, 2		# now $t8 contains calc area by 2
	
	#print String "Area: " 
	li $v0,4
	la $a0, msg3
	syscall
	
	#print int
	li $v0,1
	move $a0,$t8	# integer part of area 
	syscall 
	
	beq $t9,$zero,giveIntOutput
	j giveFloatOutput 
	
giveIntOutput:
	#print String ".00\n"
	li $v0,4
	la $a0, outputStrEven
	syscall
	j end 
	
giveFloatOutput:
	#print String ".50\n" 
	li $v0,4
	la $a0, outputStrOdd
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
msg2: .asciiz ">> "
msg3: .asciiz "Area: "
LF: .asciiz "\n"

errorMsg1: .asciiz "badInputException :: Given n is less than 2 -> Does not make mathematical sense -> Program Terminated"
errorMsg2: .asciiz "badInputException :: Given input points are not x-sorted -> Program Terminated"

outputStrEven: .asciiz ".00"
outputStrOdd: .asciiz ".50"