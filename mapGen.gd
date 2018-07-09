extends Node

const BLOCK = "res://blocks/blockGen.tscn"
var nextBlock
var block

export var blockSize = Vector2(20,20)
export var cellSize = Vector2(32,32)

var size = cellSize*blockSize

var cardinal = {"n" : Vector2(0,-1), "s": Vector2(0,1), "e" :Vector2(1,0), "w":Vector2(-1,0)}
var matchingCardinal = {"n":"s", "e":"w","w":"e","s":"n"}

var blockGrid = {}
var blocksToFill = []
var setSeed = 2260921286
var randomSeed = true

func _ready():
	
	randomize()
	if randomSeed :
		setSeed = randi()
	seed(setSeed)
	print(setSeed)
	
	block = ResourceLoader.load(BLOCK)
	
	var startBlock = block.instance()
	print(startBlock)
	startBlock.position = Vector2(0,0)
	add_child(startBlock)
	
	var time = OS.get_ticks_msec()
	
	while true :
#		print("searching", startBlock.setSeed)
		if startBlock.entrances["n"].size() > 0 :
			for ent in startBlock.entrances["n"] :
				startBlock.mainEnt = ent
				startBlock._find_paths()
				blockGrid[startBlock.position] = startBlock
			if startBlock.validEnts["s"].size() > 0 :
				break
		startBlock._generate_map()
	
	print("time to find : ", OS.get_ticks_msec()-time)
	startBlock._draw_map()
	print("time to draw : ", OS.get_ticks_msec()-time)
	for dir in cardinal :
		if startBlock.validEnts[dir].size() > 0  && dir != "n":
			_add_block(startBlock, dir)
	print("time to fill : ", OS.get_ticks_msec()-time)

func _input(event):
	
	if event.is_action_pressed("ui_up"):
		if !blocksToFill.empty():
			blocksToFill[0].modulate = Color(1.0,0.0,1.0)
	
	if event.is_action_pressed("ui_accept") :
		var time  = OS.get_ticks_msec()
		if !blocksToFill.empty() :
			print(blocksToFill)
			print("fillingNext")
			blocksToFill[0].modulate = Color(1.0,1.0,1.0)
			_fill_block(blocksToFill.pop_front())
			print("filled")
		print( blockGrid.size(), " blocks in : ", OS.get_ticks_msec()-time )



func _match_ents(ents, face, ents2):
	
	var matching = 0
	
	var mod = cardinal[face].tangent()
	
	for ent in ents :
		for ent2 in ents2 :
			matching = 0
			for block in ent :
				for block2 in ent2:
					if block*mod == block2*mod :
						matching += 1
					if matching >= abs(ent2.size() - ent.size()) + 2 :
						return ent2
	return []
	

func _fill_block(block) :
	
	for dir in cardinal :
		if block.validEnts[dir].size() > 0 :
			_add_block(block, dir, true)

func _add_block(curr, face, fullMatching = false) :

	nextBlock = block.instance()
	nextBlock.position += curr.position+cardinal[face]*size
	add_child(nextBlock)
	
	if blockGrid.has(nextBlock.position) :
		nextBlock.free()
		return false
	
	var time = OS.get_ticks_msec()
	while true :
		if nextBlock.entrances[matchingCardinal[face]].size() > 0 :
			var valid = _match_ents(curr.validEnts[face], face, nextBlock.entrances[matchingCardinal[face]])
			if valid.size() > 1 :
				nextBlock.mainEnt = valid
				nextBlock._find_paths()
				
				var neighbors = _get_neighbors(nextBlock, matchingCardinal[face])
				var count = 0
				if neighbors.size() > 0 && fullMatching :
					for dir in cardinal :
						if neighbors.has(dir) and neighbors[dir].validEnts[matchingCardinal[dir]].size() > 0 :
							if _match_ents(neighbors[dir].validEnts[matchingCardinal[dir]], dir, nextBlock.validEnts[face]).size() > 0 :
								count += 1
						else :
							count += 1
					if count < 4 :
						nextBlock._generate_map()
						continue
				blockGrid[nextBlock.position] = nextBlock
				blocksToFill.push_back(nextBlock)
				nextBlock._draw_map()
				return true
		nextBlock._generate_map()

func _get_neighbors(block, excludeFace = ""):
	
	var out = {}
	
	for dir in cardinal :
		var next  = block.position + cardinal[dir]*size
		if dir != excludeFace && blockGrid.has(next) :
			out[dir] = blockGrid[next]
	
	return out
