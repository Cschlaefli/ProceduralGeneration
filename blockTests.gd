extends Node

const BLOCK = "res://blocks/blockGen.tscn"
var nextBlock
var block

export var blockSize = Vector2(20, 20)
export var cellSize = Vector2(32,32)

var size = cellSize*blockSize

var setSeed = 1291490227
var randomSeed = true

var totalGenerationAttempts = 0

func _ready():
	
	randomize()
	if randomSeed :
		setSeed = randi()
	seed(setSeed)
	print(setSeed)
	
	block = ResourceLoader.load(BLOCK)
	
	var start = Vector2(0,0)
	
	for y in 5:
#		seed(setSeed)
		for x in 10 :
			var pos = Vector2(x,y)*size
			_gen_block(pos, 8, 40, 2, 3, 4, false)
#			_gen_block(pos, 2, 15, 2, 3, 8, true)
	
	

# 2smooth, 15 fill, 2 stray, 3 death, 8 birth, extended check
# makes cool bubbles

func _gen_block(position, smoothing, fill, stray, death, birth, extendedCheck):
	var newBlock = block.instance()
	
	newBlock.deathLimit = death
	newBlock.birthLimit = birth
	newBlock.extendedCheck = extendedCheck
	
	newBlock.height = blockSize.y
	newBlock.width = blockSize.x
	newBlock.position = position
	newBlock.strayCount = stray
	newBlock.smoothCount = smoothing
	newBlock.fillPercentage = fill
	newBlock._generate_map()
	newBlock._draw_map()
	add_child(newBlock)
	


