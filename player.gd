extends KinematicBody2D

const GRAVITY = 1024
const WALK_SPEED = 1536
const JUMP_SPEED = 256

const AIR_CAP = 368
const TERMINAL_VELOCITY = 2048



const RUN_CAP = 256
const WALK_CAP = 386
const WEIGHT = 1536


var leftCheck
var rightCheck
var groundCheck

var velocity = Vector2()
var moveInput = Vector2(0,0)
var charFacing = -1
var jumping = false
var grounded = false
var jumpTime = 0

var clinging = false

var canInput = true

var camera

func _ready():
	
	camera = get_node("Camera2D")
	camera.make_current()
	
	groundCheck = get_node("raycasts/groundCheck")
	groundCheck.add_exception(self)
	
	leftCheck = get_node("raycasts/leftCheck")
	leftCheck.add_exception(self)
	rightCheck = get_node("raycasts/rightCheck")
	rightCheck.add_exception(self)


func _input(event):
	if event.is_action_pressed("jump") :
		if grounded :
			_jump()
	
	

func _jump():
	jumping = true
	jumpTime = .25
	velocity.y = 0

func _cling():
	if(grounded || jumping):
		return
	clinging  = true
	velocity.x = 0
	velocity.y = 0
	if Input.is_action_pressed("up") :
		velocity.y = -128
	if(Input.is_action_pressed("down")):
		velocity.y = 128

func _process(delta):
	
	
#	gravity
	if velocity.y < TERMINAL_VELOCITY && !grounded :
		velocity.y += delta * GRAVITY
	
	
	if Input.is_action_pressed("left") && canInput :
		moveInput.x = -1
	elif Input.is_action_pressed("right") && canInput :
		moveInput.x = 1
	else:
		moveInput.x = 0
	
	if groundCheck.is_colliding() && !jumping :
		grounded = true
	else:
		grounded = false
	
	if jumpTime > 0 :
		jumpTime -= delta
	elif jumpTime < 0 :
		jumpTime = 0
		jumping  = false
	
	if jumping && jumpTime >0 :
		grounded = false
		velocity.y = -JUMP_SPEED
	
	if Input.is_action_pressed("up") :
		moveInput.y = -1
	elif Input.is_action_pressed("down") :
		moveInput.y = -1
	else:
		moveInput.y = 0
	
	
	if grounded :
		if(moveInput.x != 0):
			charFacing = moveInput.x
			velocity.x += moveInput.x*WALK_SPEED*delta
			if(abs(velocity.x) > RUN_CAP):
				velocity.x = RUN_CAP*moveInput.x
		else:
			if(velocity.x > WEIGHT*delta):
				velocity.x -= WEIGHT*delta
			elif(velocity.x < -WEIGHT*delta):
				velocity.x += WEIGHT*delta
			else:
				velocity.y = 0
				velocity.x = 0
	else :
		if(moveInput.x != 0):
			charFacing = moveInput.x
			velocity.x += WALK_SPEED*moveInput.x*delta*2
			if(abs(velocity.x) > AIR_CAP):
				velocity.x = AIR_CAP*moveInput.x
	
	if Input.is_action_pressed("grab"):
		if leftCheck.is_colliding() && charFacing ==-1 :
			_cling()
		elif rightCheck.is_colliding()&& charFacing ==1 :
			_cling()
		else :
			clinging = false
	else:
		clinging = false
	
	var motion = velocity
	
	move_and_slide(motion)
	