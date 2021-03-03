# 	Abhinav Singhal		Abhishek Kumar
# 	2019CS50768			2019CS10458

# Important observations regarding the problem statement:
# 1.	Input: Postfix expression as a string.
# 2.	operands are integers in range 0-9 only.
# 3.	we will be limited to using +,- and * operators only -> Only 32-bit Integer registers are enough.
# 4.	Output: Value of the Postfix expression.
# 5.	Since given C++ program gives Postfix expression without whitespaces, we assume in the input that there are no whitespaces.



# Major Design considerations and implementations:
# 1.	We allocate some initial space for input string (currently max length of string = 1000 bytes/characters)
# 2.	In ASCII, Each character occupies exactly 1 byte. Therefore we need to increment address in string traversal by 1 only (as against 4 for integers).
# 3.	We will be maintaining an abstract data structure: stack of integer operands.
# 4.	We assume value of expression can be expressed within 32 bit integer register without overflow at any point of execution.
# 5.	if we see an integer between 0 to 9, we push into the stack.
# 6.	if we see an operator +,- or * we try to do the operation with top two elements of the stack (after popping them) and push the result into the stack.

# data had to be stored among various function calls, hence we have used 's' registers everywhere for safety => they remember values stored in them.
# Using 't' registers would lead to loss of data at many steps.
# stack has integers as its elements not characters => 4 bytes for each element.

# When do we Raise Exceptions: 
# 1.	Input expression contains characters other than integers 0-9 and +,- and * or a whiteSpace.
# 2.	If there are less than 2 operands in the stack currently and we see an operator in the input string.
# 3.	If after the entire input is read, more than 1 operand is present in the stack.
# 4.	If max length alloted to string or stack is exceeeded. 		(WARNING: NOT HANDLED YET)
# 5. 	If Overflow occurs at any point of execution 				(WARNING: NOT HANDLED YET)


	#Begin 
	
	.data

inputMsg1: .asciiz "Enter Postfix Expression >> "

LF: .asciiz "\n"
outputMsg1: .asciiz "Value of Expression << "
whitespaceErrorMsg: .asciiz "WhitespaceError: Blank space detected in Postfix String -> Program Terminated."
UndefinedCharErrorMsg1: .asciiz "UndefinedCharError: Unknown Character < \""
UndefinedCharErrorMsg2: .asciiz "\" > detected in Postfix String -> Program Terminated."
LessThanTwoOperandsInStackErrorMsg: .asciiz "LessThanTwoOperandsInStackError: Cannot apply binary operator as less than 2 operands present -> Program Terminated."
StackSizeMoreThanOneInTheEndErrorMsg: .asciiz "StackSizeMoreThanOneInTheEndError: More than one element remaining in stack at the end -> Program Terminated."
StackOverflowErrorMsg: .asciiz "StackOverflowError: Integer Stack limit exceeded -> Program Terminated."


maxLenOfUserInput = 1000 # in Bytes/Characters
maxStackSize = 4000 #4000/4 = 1000 elements because we store integers in stack. 
maxStackElements = 1000

	.align 0	# aligns userAns array in memory for 2^0 = 1 byte elements
userAns: .space maxLenOfUserInput	#Assign memory for string input
	.align 2	# aligns stack in memory for 2^2 = 4 byte (integer) elements.
stack: .space  maxStackSize			#Assign memory for Abstract data stucture => stack.

	.text
	.globl main
	#.ent main
	
	
# $s0 = Pointer/Memory Address of the Current character of userAns
# $s1 = Actual current character of userAns 
# $s2 = 
# $s3 = 
# $s4 = 
# $s5 = 
# $s6 = Pointer/Memory Address of the empty location NEXT to the last filled operand's memory address.
# $s7 = Numbers of operands currently in the stack.



# $t0 = 
# $t1 = These three used for on the spot manipulation (add,sub,mult) of data (temporary variables).
# $t2 = 



############################################### Main Begin ##############################################################
main:
	#print String "Enter Postfix Expression >> " 
	li $v0,4 					#code for syscall is 4 and v0 has been used;
	la $a0, inputMsg1 
	syscall

	li $v0, 8 					# call code, read string
	la $a0, userAns 			# addr, where to put chars
	li $a1, maxLenOfUserInput 	# max chars for string
	syscall
	
	#Now the string entered by the user is stored in userAns memory address.
	# Input done successfully !
	
start:
	
	la $s0, userAns		# we are just loading address of userAns string into $s0
	la $s6, stack		# loading address of stack into $s6.
	li $s7, 0			# Initially number of operands in the stack = 0.
	
Loop:
	lb $s1, 0($s0)		#'load byte' of your string... in other words, 
                        # take the next byte/ character of your string and put it in $t1
	
	beq $s1, 10, AlmostEnd	#saw the NULL character "\0" => end of string.
	
	beq $s1, 43, popAdd	# 43 in ascii stands for '+' character.
	beq $s1, 45, popSubs	# 45 => '-'
	beq $s1, 42, popMult   # 42 => multiply
	beq $s1, 32, sawWhitespaceError	# 32 => whiteSpace 
	blt $s1, 48, UndefinedCharError	# ascii < '0'
	bgt $s1, 57, UndefinedCharError # ascii > '9'
	
	# now it means $s1 :: current character :: is a integer between 0 and 9. Hence, we push it onto the stack.
	addi $s1, $s1, -48  	#converts t1's ascii value to dec value	#very imp step 
	j push
	
Resume:
	addi $s0, $s0, 1 	# Move the pointer in the userAns to the next character's address.
	j Loop 


################ MAIN END #################### BASIC STACK OPERATIONS BEGIN ##################################################################

push:
	beq $s7, maxStackElements, StackOverflowError
	addi $s7,$s7,1 	# increment number of operands in the stack.
	sw $s1, 0($s6) 	# storing $s1 character into top of stack at $s6.
	addi $s6,$s6,4	# increment $s6 to the next empty location.
	j Resume


popAdd:
	blt $s7,2, LessThanTwoOperandsInStackError
	addi $s6,$s6,-4		# $s6 points to the top element of the stack.
	lw $t0, 0($s6)		# load word in $s6 to $t0
	
	addi $s6,$s6,-4		# $s6 points to the second from the top element of the stack.
	lw $t1, 0($s6)		# load character in $s6 to $t1
	
	add $t2,$t0,$t1 	# add the top two elements of the stack and store in $t2.
	
	sw $t2, 0($s6)		# store the sum back again in $s6.
	addi $s6,$s6, 4		# increment $s6 to next (empty) position in the stack.
	
	addi $s7,$s7,-1 	# decrease number of operands in the stack.
	j Resume
	
popSubs:
	blt $s7,2, LessThanTwoOperandsInStackError
	addi $s6,$s6,-4		# $s6 points to the top element of the stack.
	lw $t0, 0($s6)		# load word in $s6 to $t0
	
	addi $s6,$s6,-4		# $s6 points to the second from the top element of the stack.
	lw $t1, 0($s6)		# load character in $s6 to $t1
	
	sub $t2,$t1,$t0 	# subtract the top two elements (second - first) of the stack and store in $t2.
	
	sw $t2, 0($s6)		# store the difference back again in $s6.
	addi $s6,$s6, 4		# increment $s6 to next (empty) position in the stack.
	
	addi $s7,$s7,-1 	# decrease number of operands in the stack.
	j Resume
	
popMult:
	blt $s7,2, LessThanTwoOperandsInStackError
	addi $s6,$s6,-4		# $s6 points to the top element of the stack.
	lw $t0, 0($s6)		# load word in $s6 to $t0
	
	addi $s6,$s6,-4		# $s6 points to the second from the top element of the stack.
	lw $t1, 0($s6)		# load character in $s6 to $t1
	
	mul $t2,$t0,$t1 	# multiply the top two elements of the stack and store in $t2.
	
	sw $t2, 0($s6)		# store the product back again in $s6.
	addi $s6,$s6, 4		# increment $s6 to next (empty) position in the stack.
	
	addi $s7,$s7,-1 	# decrease number of operands in the stack.
	j Resume


#################################### BASIC STACK OPERATIONS END ###################### END BEGIN #########################################
AlmostEnd:
	bgt $s7,1, StackSizeMoreThanOneInTheEndError	# note that $s7 cannot be less than 1 else we would already have raised LessThanTwoOperandsInStackError at some point of time.
	# else, location behind $s6 has our answer!
	
	addi $s6,$s6,-4		# $s6 points to our answer.
	lw $t0, 0($s6)		# load word in $s6 to $t0
	
	#print String "Value of Expression << " 
	li $v0,4 					#code for syscall is 4 and v0 has been used;
	la $a0, outputMsg1 
	syscall
	
	#print answer
	li $v0,1     #prepare system call 
	move $a0,$t0 #copy t0 to a0 
	syscall      #print integer 
	
	j end 
	
end:
	#print String
	li $v0,4
	la $a0, LF
	syscall
	
	li $v0, 10	# smooth exit code
	syscall
	
	
#################################### END END ####################### ERROR HANDLING BEGIN ##########################################
sawWhitespaceError:
	#print String error msg 
	#li $v0,4 					
	#la $a0, whitespaceErrorMsg
	#syscall
	
	#j end
	# instead of raising an exception we are just ignoring whitespaces now.
	j Resume 
	
UndefinedCharError:
	#print String error msg 
	li $v0,4 					
	la $a0,UndefinedCharErrorMsg1
	syscall
	
	
	# print	unknown character char
	li $v0, 11		# code is 11 to print a character 
    la $a0, 0($s1)
    syscall
	
	#print String error msg 
	li $v0,4 					
	la $a0,UndefinedCharErrorMsg2
	syscall
	
	j end 
	
LessThanTwoOperandsInStackError:
	#print String error msg 
	li $v0,4 					
	la $a0, LessThanTwoOperandsInStackErrorMsg
	syscall
	
	j end 


StackSizeMoreThanOneInTheEndError:
	#print String error msg 
	li $v0,4 					
	la $a0, StackSizeMoreThanOneInTheEndErrorMsg
	syscall
	
	j end 

StackOverflowError:
	#print String error msg 
	li $v0,4 					
	la $a0, StackOverflowErrorMsg
	syscall
	
	j end 


####################### ERROR HANDLING END ##################### END OF ASSIGNMENT 	###################################