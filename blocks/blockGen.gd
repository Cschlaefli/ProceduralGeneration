extends TileMap

const GEN = preload("res://bin/test.gdns")
var generator
var alternate = 1

var width = 20
var height = 20


var smoothCount = 8
var strayCount = 2

var deathLimit = 3
var birthLimit = 4

var extendedCheck = false

var entSize = 1
var entCap = 8
var fillCut = 10


var corners = [Vector2(0,0), Vector2(width-1,0), Vector2(width-1, height-1), Vector2(0, height-1)]

const cardinalDirs = [Vector2(0,1), Vector2(1,0), Vector2(-1,0), Vector2(0,-1)]
const diagDirs = [Vector2(1,1), Vector2(-1,1),Vector2(-1,-1),Vector2(1,-1)]

const dirNames = ["n", "s", "e", "w"]

#var randomSeed = true
var setSeed = 224124172


var fillPercentage = 40

var map = {}

var entrances
var validEnts = { "n" : [], "e" : [], "s" : [], "w": []}

var mainEnt
var paths = []



func _init():
	generator = GEN.new()
	pass
#
#func _ready():
#	_generate_map()
#	_draw_map()

func _consolidate_entrances():
	
	for corner in corners :
		var cell = map[corner]
		if cell == -1 :
			var neighbors = _get_neighbors_value(corner, true)
			for n in neighbors :
				if n == 0 :
					map[corner] = 0
					break
	
	for d in dirNames :
		var dir = entrances[d]
		
		for ent in dir :
			var fill = _find_fill(ent[0])
			if fill.size() < fillCut + ent.size() :
				for cell in fill :
					map[cell] = 0
	
	entrances = _find_entrances()
	
	for d in dirNames :
		var dir = entrances[d]
		#this keeps breaking
		if dir.size() > 1 :
			for x in range(1, dir.size()-1) :
				if _has_path(dir[0], dir[x]) :
					for block in dir[x] :
						dir[0].append(block)
					entrances[d].remove(x)
	
	

func _find_paths():
	
	for dir in dirNames :
		validEnts[dir].clear()
		for ent in entrances[dir] :
			if _has_path(mainEnt, ent) :
				validEnts[dir].append(ent)
	

func _find_fill(node):
	var q = []
	var visited = {}
	
	visited[node] = true
	q.push_front(node)
	
	while !q.empty():
		var curr = q.pop_front()
		
		for next in _get_neighbors(curr):
			if map[next] == -1 && !visited.has(next) : #.has() may not work here
				q.push_back(next)
				visited[next] = true
		
	
	return visited

func _generate_map():
	
	var time = OS.get_ticks_msec()
	
	_random_fill_map() 
	map = generator.get_map(map, smoothCount, birthLimit, deathLimit, alternate)
	
	for x in strayCount :
		_clean_strays()
	
	entrances = _find_entrances()
#	time = OS.get_ticks_msec()-time
#	print("_find_ent time : ", OS.get_ticks_msec()-time)
	_consolidate_entrances()
#	time = OS.get_ticks_msec()-time
#	print("Consolidate time : ", OS.get_ticks_msec()-time)
	

func _draw_map():
	var time = OS.get_ticks_msec()
	
	
	var x = 0
	var y = 0
	while x < width :
		while y < height :
			var cell = Vector2(x,y)
			if get_cell(x,y) != map[cell]:
				set_cell(x,y, map[cell])
			y += 1
		y = 0
		x += 1
#	time = OS.get_ticks_msec()-time
#	print("Time to draw : ", time)
#	print("Time per block : ", time/(height*width))
#	print("Timer per drawn block ", time/_raw_fill())

func _clean_strays():
	var copy = map
	for x in width :
		for y in height:
			var cell = Vector2(x,y)
			if !_get_cardinal_wall_count(cell) > 1 :
				copy[cell] = -1
	map = copy

func _get_cardinal_wall_count(cell):
	
	var neighbors = _get_neighbors(cell)
	
	var wallCount = neighbors.size()
	
	for neighbor in neighbors :
		wallCount += map[neighbor]
	
	return wallCount

func _get_surrounding_wall_count(cell, extra = false):
	var neighbors = _get_neighbors(cell, true)
	
	var wallCount = 8 - neighbors.size()
	
	for neighbor in neighbors :
		if extra :
			var extras = _get_neighbors(neighbor)
			for ex in extras :
				if map[ex] >= 0:
					wallCount += 1
		if map[neighbor] >= 0:
			wallCount += 1
	
	return wallCount

func _copy_map(input) :
	
	var copy = {}
	
	for x in width :
		for y in height:
			var cell = Vector2(x,y)
			copy[cell] = map[cell]
	
	return copy


func _smooth_map():
	var x = width-1
	var y = height-1
	
	var copy = _copy_map(map)
	
	while x > -1:
		while y > -1:
			var cell = Vector2(x,y)
			if map[cell] != 1 :
				var neighborTiles = _get_surrounding_wall_count(cell, extendedCheck)
				
				if map[cell] == 0:
					if neighborTiles < deathLimit:
						copy[cell] = -1
				elif map[cell] == -1:
					if neighborTiles > birthLimit:
							copy[cell] = 0
			y -= 1
		y = height-1
		x -= 1
	map = copy

func _random_fill_map():
	
	for x in width:
		for y in height:
			var cell = Vector2(x,y)
			if rand_range(0,100) < fillPercentage :
				map[cell] = 0
			else :
				map[cell] = -1

func _find_entrances():
	
	var ents = { "n" : [], "s" : [], "e" : [], "w" : [] }
	
	var y = 0
	var x = 0
	var ent2 = []
	var ent1 = []
	
	while y < height:
#		if map[0][y] == -1 && (_get_neighbors_value(Vector2(0,y)).count(-1) > 1 || ent1.size() > entSize) :
		var cell1 = Vector2(0,y)
		if map[cell1] == -1 :
			ent1.push_back(cell1)
		else :
			if ent1.size() < entSize :
				ent1.pop_back()
			else :
				ents["w"].push_back(ent1.duplicate())
				ent1 = []
#		if map[width-1][y] == -1 && (_get_neighbors_value(Vector2(width-1,y)).count(-1) > 1 || ent2.size() > entSize) :
		var cell2 = Vector2(width-1,y)
		if map[cell2] == -1 :
			ent2.push_back(cell2)
		else :
			if ent2.size() < entSize :
				ent2.pop_back()
			else:
				ents["e"].push_back(ent2.duplicate())
				ent2 = []
		y += 1
	if ent1.size() > entSize :
		ents["w"].push_front(ent1)
	if ent2.size() > entSize :
		ents["e"].push_front(ent2)
	
	ent2 = []
	ent1 = []
	
	while x < width:
#		if map[x][0] == -1 && _get_neighbors_value(Vector2(x,0)).count(-1) > 1 :
		var cell1 = Vector2(x,0)
		if map[cell1] == -1 :
			ent1.push_back(cell1)
		else :
			if ent1.size() < entSize :
				ent1.pop_back()
			else :
				ents["n"].push_back(ent1.duplicate())
				ent1 = []
#		if map[x][height-1] == -1 && _get_neighbors_value(Vector2(x,height-1)).count(-1) > 1:
		var cell2 = Vector2(x,height-1)
		if map[cell2] == -1 :
			ent2.push_back(cell2)
		else :
			if ent2.size() < entSize :
				ent2.pop_back()
			else :
				ents["s"].push_back(ent2.duplicate())
				ent2 = []
		x += 1
	
	if ent1.size() > entSize :
		ents["n"].push_front(ent1)
	if ent2.size() > entSize :
		ents["s"].push_front(ent2)
	
	var entCount = 0
	
	for x in ents.values() :
		entCount += x.size()
	
	
	ents["count"] = entCount
	return ents

func _show_ents():
	for dirs in dirNames :
#		for ents in validEnts[dirs] :
		if validEnts[dirs].size() > 0 :
			var ents = validEnts[dirs][0]
			for ent in ents :
				if map[ent] == -1 :
					map[ent] = 1

func _get_neighbors_value(node, diag = false):
	
	var out = []
	
	for neigh in _get_neighbors(node,diag) :
		out.append(map[neigh])
	return out

func _get_neighbors(node, diag = false):
	
	var out = []
	
	for dir in diagDirs+cardinalDirs if diag else cardinalDirs :
		var check = node + dir
		if check.x >= 0 && check.y >= 0 && check.x < width && check.y < height :
			out.append(check)
	
	return out

func _has_path(starts, ends):
	
	var q = []
	
	for start in starts :
		var visited = {}
		q.push_front(start)
		visited[start] = true
		
		while !q.empty() :
			var curr = q.pop_back()
			for next in _get_neighbors(curr) :
				if ends.has(next) :
					return true
				if !visited.has(next) && map[next] == -1 :
#					if next.y +1 < height :
#						if map[next.x][next.y+1] == -1 :
#							q.push_front(next)
#							visited[next] = true
#					else :
						q.push_front(next)
						visited[next] = true
	return false

func _raw_fill():
	var count = 0
	
	for x in map :
		for y in x :
			if y >= 0 :
				count += 1
	return count

func _test_maps() :
	var dirCount = {"n": 0,"s" :0,"e":0,"w":0, "paths":0}
	
	for x in 100 :
		_generate_map()
		for dir in dirNames :
			if entrances[dir].size() != 0 :
				if mainEnt == [] :
					mainEnt = entrances[dir][0]
					_find_paths()
					var count = 0
					for dir2 in dirNames :
						if validEnts[dir2].size() > 1 && dir != dir2 :
							count += 1
					dirCount["paths"] += count
				dirCount[dir] += 1
		mainEnt = []
	print(dirCount)
	
