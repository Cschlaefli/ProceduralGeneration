extends Node

const BLOCK = "res://blocks/blockGen.tscn"
var nextBlock
var block

export var blockSize = Vector2(20, 20)
export var cellSize = Vector2(40,40)

var size = cellSize*blockSize

var setSeed = 1234
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
		for x in 5:
			var pos = Vector2(x,y)*size
			var block = _gen_block(3,14, 7,6, pos, 2)
#_gen_block(8, 40,3,4, pos)
	


# 2smooth, 15 fill, 2 stray, 3 death, 8 birth, extended check
# makes cool bubbles

func _gen_block(smooth, fill, death, birth, pos, alt = 1 ) :
	
	var nextBlock = block.instance()
	nextBlock.height = blockSize.y
	nextBlock.width = blockSize.x
	nextBlock.position = pos
	nextBlock.fillPercentage = fill
	nextBlock.smoothCount  = smooth
	nextBlock.birthLimit = birth
	nextBlock.deathLimit = death
	nextBlock.alternate = alt
	nextBlock._generate_map()
	nextBlock._show_ents()
	nextBlock._draw_map()
	add_child(nextBlock)
	return nextBlock


