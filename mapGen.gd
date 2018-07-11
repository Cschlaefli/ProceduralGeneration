extends Node

const BLOCK = "res://blocks/blockGen.tscn"
var nextBlock
var block

export var blockSize = Vector2(20,20)
export var cellSize = Vector2(32,32)

var size = cellSize*blockSize

var topLimit = Vector2(0,0)*size
var botLimit = Vector2(0,8)*size
var leftLimit = Vector2(-5, 0)*size
var rightLimit = Vector2(5, 0)*size

# 2smooth, 15 fill, 2 stray, 3 birth, 8 death, extended check
# 40 fill, 4 death, 8 smooth normal

var cardinal = {"n" : Vector2(0,-1), "s": Vector2(0,1), "e" :Vector2(1,0), "w":Vector2(-1,0)}
var matchingCardinal = {"n":"s", "e":"w","w":"e","s":"n"}

var blockGrid = {}
var blocksToFill = []
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
	
	_add_start_block()

func _add_start_block():
	
	var startBlock = _gen_block( 8, 40, 4, false, Vector2(0,0))
	
	var time = OS.get_ticks_msec()
	
	while true :
		if startBlock.entrances["n"].size() > 0 :
			for ent in startBlock.entrances["n"] :
				startBlock.mainEnt = ent
				startBlock._find_paths()
				blockGrid[startBlock.position] = startBlock
			if startBlock.validEnts["s"].size() > 0 :
				break
		startBlock._generate_map()
	
	startBlock._draw_map()
	add_child(startBlock)	
	
	print("time to find : ", OS.get_ticks_msec()-time)
#	startBlock._draw_map()
#	print("time to draw : ", OS.get_ticks_msec()-time)
	for dir in cardinal :
		if startBlock.validEnts[dir].size() > 0  && dir != "n":
			_add_block(startBlock, dir)
	print("time to fill : ", OS.get_ticks_msec()-time)


func _input(event):
	
	if event.is_action_pressed("ui_up"):
		if !blocksToFill.empty():
			blocksToFill[0].modulate = Color(1.0,0.0,1.0)
	
	if event.is_action_pressed("shift"):
		_find_valid_ents()
	
	if event.is_action_pressed("ui_select"):
		var time  = OS.get_ticks_msec()
		while !blocksToFill.empty() : 
			_fill_block(blocksToFill.pop_front())
		time = OS.get_ticks_msec()
		print ( "seed : ",  setSeed)
		print ( "Time taken : " , time)
		print ( "Total Generations attempts : ", totalGenerationAttempts)
		print ( "Total size : ", blockGrid.size() )
		print ( "Average generation attempts : ", totalGenerationAttempts/blockGrid.size())
		print ( "Time per generation : ", time/totalGenerationAttempts)
		print ( "Time per block : ", time/blockGrid.size())
	
	if event.is_action_pressed("ui_cancel") :
		var time = OS.get_ticks_msec()
		_draw_full_map()
		print("Time to draw : " , OS.get_ticks_msec()-time)
	
	if event.is_action_pressed("ui_accept") :
		var time  = OS.get_ticks_msec()
		if !blocksToFill.empty() :
			print("fillingNext")
			blocksToFill[0].modulate = Color(1.0,1.0,1.0)
			_fill_block(blocksToFill.pop_front())
			print("filled")
		print( blockGrid.size(), " blocks in : ", OS.get_ticks_msec()-time )

func _draw_full_map():
	for block in blockGrid.values() :
		block._draw_map()

func _find_valid_ents():
	for block in blockGrid.values() :
		block._show_ents()
		block._draw_map()

func _match_ents(ents, face, ents2, loose = false):
	
	var matching = 0
	
	var mod = cardinal[face].tangent()
	
	var time = OS.get_ticks_msec()
	
	for ent in ents :
		for ent2 in ents2 :
			matching = 0
			for block in ent :
				for block2 in ent2:
					if abs(ent2.size() - ent.size()) > 4 :
						continue
					if block*mod == block2*mod :
						matching += 1
					if ent2.size() <= 3 || ent.size() <= 3 :
						if matching >= 2 :
							return ent2
					elif matching >= ent2.size()-2 && matching >= ent.size()-2 :
						return ent2
	return []

func _fill_block(block, valid = true) :
	
	for dir in cardinal :
		if block.validEnts[dir].size() > 0 :
			_add_block(block, dir, true)

func _gen_block( smooth, fill, birth, extended, pos ) :
	
	var nextBlock = block.instance()
	nextBlock.height = blockSize.y
	nextBlock.width = blockSize.x
	nextBlock.position = pos
	nextBlock.smoothCount = smooth
	nextBlock.fillPercentage = fill
	nextBlock.birthLimit = birth
	nextBlock.extendedCheck = extended
	nextBlock._generate_map()
	
	return nextBlock
	


func _add_block(curr, face, fullMatching = false) :
	
	
	var pos = curr.position+cardinal[face]*size
	
	if blockGrid.has(pos) :
		return true
	
	
	var noEntSide = []
	
	if pos.y <= topLimit.y :
		noEntSide.append("n")
	
	if pos.y >= botLimit.y : 
		noEntSide.append("s")
	
	if pos.x <= leftLimit.x :
		noEntSide.append("w")
	
	if pos.x >= rightLimit.x :
		noEntSide.append("e")
	
	var nextBlock
	
	if noEntSide.size() > 0 :
		nextBlock = _gen_block( 8, 40, 4, false, pos)
	else :
		nextBlock = _gen_block( 2, 15, 8, true, pos)
	
	
	var genAttempts = 0
	var time = OS.get_ticks_msec()
	while true :
		genAttempts += 1
		if genAttempts > 2000 :
			print ("fail to find")
			return false
		var nextEnts = nextBlock.entrances[matchingCardinal[face]]
		#used to be validents
		var currValidEnts = curr.entrances[face]
		
		var vEnts = false
		for side in noEntSide :
			if side != face :
				if nextBlock.entrances[side].size() != 0 :
					nextBlock._generate_map()
					vEnts = true
					break
		if vEnts :
			continue
		
		if nextEnts.size() > 0 :
			var valid = _match_ents(currValidEnts, face, nextEnts)
			if valid.size() > 1 :
				nextBlock.mainEnt = valid
				nextBlock._find_paths()
				var neighbors = _get_neighbors(nextBlock, matchingCardinal[face])
				var count = 0
				if !neighbors.empty() && fullMatching :
					for dir in cardinal :
						if neighbors.has(dir) :
							var neighborEnts = neighbors[dir].entrances[matchingCardinal[dir]]
							if neighborEnts.size() > 0 :
								# used to be next.ValidEnts
								if _match_ents(neighborEnts, dir, nextBlock.entrances[dir]).size() > 0 :
									count += 1
							else :
								count += 1
						else : 
							count += 1
					if count < 4 :
						nextBlock._generate_map()
						continue
				blockGrid[nextBlock.position] = nextBlock
				blocksToFill.push_back(nextBlock)
				print("block added in : ", OS.get_ticks_msec()-time)
#				nextBlock._draw_map()
				totalGenerationAttempts += genAttempts
				print("generation attempts : ", genAttempts)
				nextBlock._draw_map()
				add_child(nextBlock)
				return true
		nextBlock._generate_map()

func _get_neighbors(block, excludeFace = ""):
	
	var out = {}
	
	for dir in cardinal :
		var next  = block.position + cardinal[dir]*size
		if dir != excludeFace && blockGrid.has(next) :
			out[dir] = blockGrid[next]
	
#	print(out)
	
	return out
