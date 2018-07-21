#include <gdnative_api_struct.gen.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

static int SIZE_X = 20;
static int SIZE_Y = 20;

int birth = 4;
int death = 3;


int map[20][20];
int fillCopy[20][20];

bool check_path(int x, int y, int endX, int endY);

bool has_path(int x, int y, int endX, int endY);

void smooth_map(int count, int extra);

void print_map(int map[][SIZE_Y]);

int get_neighbors(int x, int y, int range);

void copy_map(int map[SIZE_X][SIZE_Y], int copy[][SIZE_Y]);

void smooth(int map[][SIZE_Y], int range);

void fill_map(godot_dictionary dictionary);

void strays();
bool is_stray( int xLoc, int yLoc);
void clean_strays(int count);

void fill(int x, int y);
int find_fill(int x, int y);
int fill_check(int x, int y);

godot_variant fill_ents(int size);

godot_variant new_godot_vector2(double x, double y);

godot_variant simple_build_dictionary();

godot_variant find_entrances(godot_variant ents);

const godot_gdnative_core_api_struct *api = NULL;
const godot_gdnative_ext_nativescript_api_struct *nativescript_api = NULL;

GDCALLINGCONV void *simple_constructor(godot_object *p_instance, void *p_method_data);
GDCALLINGCONV void simple_destructor(godot_object *p_instance, void *p_method_data, void *p_user_data);

godot_variant simple_get_map(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);
godot_variant simple_get_ents(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);
godot_variant simple_fill_ents(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);
godot_variant simple_check_path(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);

void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options *p_options) {
	api = p_options->api_struct;

	// now find our extensions
	for (int i = 0; i < api->num_extensions; i++) {
		switch (api->extensions[i]->type) {
			case GDNATIVE_EXT_NATIVESCRIPT: {
				nativescript_api = (godot_gdnative_ext_nativescript_api_struct *)api->extensions[i];
			}; break;
			default: break;
		};
	};	
}

void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options *p_options) {
	api = NULL;
	nativescript_api = NULL;
}

void GDN_EXPORT godot_nativescript_init(void *desc) {
	godot_instance_create_func create = { NULL, NULL, NULL };
	create.create_func = &simple_constructor;

	godot_instance_destroy_func destroy = { NULL, NULL, NULL };
	destroy.destroy_func = &simple_destructor;



	godot_nativescript_register_class(desc, "SIMPLE", "Node", create, destroy);

        {
                godot_instance_method get_map = {
                        .method = &simple_get_map,
                        .method_data = 0,
                        .free_func = 0
                };

		godot_instance_method get_ents = {
                        .method = &simple_get_ents,
                        .method_data = 0,
                        .free_func = 0
                };

		godot_instance_method fill_ents = {
                        .method = &simple_fill_ents,
                        .method_data = 0,
                        .free_func = 0
                };

		godot_instance_method check_path = {
                        .method = &simple_check_path,
                        .method_data = 0,
                        .free_func = 0
                };

               godot_method_attributes attr = {
                        .rpc_type = GODOT_METHOD_RPC_MODE_DISABLED
                };

                godot_nativescript_register_method(desc, "SIMPLE", "get_map", attr, get_map);
		
                godot_nativescript_register_method(desc, "SIMPLE", "get_ents", attr, get_ents);

                godot_nativescript_register_method(desc, "SIMPLE", "fill_ents", attr, fill_ents);

                godot_nativescript_register_method(desc, "SIMPLE", "check_path", attr, check_path);
        }

}

GDCALLINGCONV void *simple_constructor(godot_object *p_instance, void *p_method_data) {
	printf("SIMPLE._init()\n");
	
	return 0;
}

GDCALLINGCONV void simple_destructor(godot_object *p_instance, void *p_method_data, void *p_user_data) {
	printf("SIMPLE._byebye()\n");

}

godot_variant simple_fill_ents(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args){

	godot_variant ret;

	godot_dictionary entrances;

	entrances = godot_variant_as_dictionary(p_args[0]);

	int fillCut = godot_variant_as_int(p_args[1]);
	
	godot_array dirs = godot_dictionary_keys(&entrances);

	for( int i = 0; i < godot_array_size(&dirs); i++){
		godot_variant dir = godot_array_get(&dirs, i);
		godot_variant entrancesV = godot_dictionary_get(&entrances, &dir);	
		godot_array ents = godot_variant_as_array(&entrancesV);
		for(int j = 0; j < godot_array_size(&ents); j++){
			godot_variant entV = godot_array_get(&ents, j);
			godot_array ent = godot_variant_as_array(&entV);
			godot_variant vector2var = godot_array_get(&ent,0);
			godot_vector2 cell = godot_variant_as_vector2(&vector2var);
			double xx = godot_vector2_get_x(&cell);
			double yy = godot_vector2_get_y(&cell);
			int x = (int) xx;
			int y = (int) yy;
			if( find_fill(x,y) < fillCut){
				fill(x,y);
			}
			godot_variant_destroy(&entV);
			godot_array_destroy(&ent);
			godot_variant_destroy(&vector2var);
		}	
		godot_variant_destroy(&dir);
		godot_variant_destroy(&entrancesV);
		godot_array_destroy(&ents);
	}

	godot_array_destroy(&dirs);

	ret = simple_build_dictionary();

	godot_dictionary_destroy(&entrances);

	return ret;

}


godot_variant simple_get_ents(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {
	godot_variant ret;

	godot_dictionary dictIn;
	
	dictIn = godot_variant_as_dictionary(p_args[1]);

	fill_map(dictIn);

	godot_dictionary_destroy(&dictIn);

	ret = find_entrances(*p_args[0]);

	return ret; 
}

godot_variant simple_get_map(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {
	godot_variant ret;
	
	godot_dictionary dictIn;

	dictIn = godot_variant_as_dictionary(p_args[0]);
	
	int smooth = godot_variant_as_int(p_args[1]);

	birth = godot_variant_as_int(p_args[2]);

	death = godot_variant_as_int(p_args[3]);

	int alternate = godot_variant_as_int(p_args[4]);
	
	int strays = godot_variant_as_int(p_args[5]);

	fill_map(dictIn);

	godot_dictionary_destroy(&dictIn);

	smooth_map(smooth ,alternate);

	clean_strays(strays);

	ret = simple_build_dictionary();

	return ret;
}

//pass map, vector2start, vector2end, returns bool;
godot_variant simple_check_path(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {
	godot_variant ret;

	godot_dictionary dictIn = godot_variant_as_dictionary(p_args[0]);

	fill_map(dictIn);
	
	godot_vector2 start = godot_variant_as_vector2(p_args[1]);	
	godot_vector2 end = godot_variant_as_vector2(p_args[2]);

	int x = (int)godot_vector2_get_x(&start);
	int y = (int)godot_vector2_get_y(&start);	

	int endX = (int)godot_vector2_get_x(&end);
	int endY = (int)godot_vector2_get_y(&end);	

	bool has = check_path(x,y,endX,endY);

	godot_variant_new_bool(&ret, has);

	godot_dictionary_destroy(&dictIn);

	return ret; 
}

bool check_path(int x, int y, int endX, int endY){

	copy_map(map, fillCopy);
	return has_path;
}

bool has_path(int x, int y, int endX, int endY){
	
	if ( x >= 0 && y >= 0 && x < SIZE_X && y < SIZE_Y &&  fillCopy[x][y] == -1){
		if( x == endX && y == endY){
			return true;
		}
		fillCopy[x][y] = 0;
		return has_path(x+1,y, endX, endY);
		return has_path(x+1,y, endX, endY);
		return has_path(x+1,y, endX, endY);
		return has_path(x+1,y, endX, endY);
	}
	return false;
}

void fill_map(godot_dictionary dictionary){


	for( int y = 0; y < SIZE_Y; y++){
		for( int x = 0; x < SIZE_X; x++){
			godot_variant cell;
		        cell = new_godot_vector2((double)x,(double)y);
			godot_variant value;
			value =api->godot_dictionary_get(&dictionary, &cell);
			int v = api->godot_variant_as_int(&value);
			map[x][y] = v;
			api->godot_variant_destroy(&value);
			api->godot_variant_destroy(&cell);
		
		}
	}
}

void smooth_map(int count, int extra)
{

	for(int i = 0; i < count; i++){
		smooth(map, extra);
	}
}

void clean_strays(int count){

	for(int i =0; i < count; i++){
		strays();	
	}
}

void strays(){
	
	int copy[SIZE_X][SIZE_Y];
	copy_map(map, copy);

	for( int y = 0; y < SIZE_Y; y++){
		for(int x = 0; x < SIZE_X; x++){
		if (map[x][y] == 0 && is_stray(x,y))	
			copy[x][y] = -1;
		}
	}

	copy_map(copy, map);
}

bool is_stray( int xLoc, int yLoc){
	
	int count = 0;

	for(int x = xLoc-1; x <= xLoc+1; x++){
			if ( x >= 0 && x < SIZE_X ){
				if( map[x][yLoc] == 0){
					if(x != xLoc ){
						count += 1;
					}
				}
			}
	}
	for(int y = yLoc-1; y <= yLoc+1; y++){
			if ( y >= 0 && y < SIZE_Y ){
				if( map[xLoc][y] == 0){
					if(y != yLoc){
						count += 1;
					}
			}
		}
	}	
	return count <= 1;

}
// size lies to you, it only goes to 2
int get_neighbors(int xLoc, int yLoc, int size){
	
	int count = 0;

	for(int x = xLoc-1; x <= xLoc+1; x++){
		for(int y = yLoc-1; y <= yLoc+1; y++){
			if ( x >= 0 && y >= 0 && x < SIZE_X && y < SIZE_Y ){
				if( map[x][y] == 0){
					if(x != xLoc || y != yLoc){
						count += 1;
						if(size >= 2){
							count += get_neighbors(x, y,size-1);
						}
					}
				}
			} else{	
			count += 1;
			}
		}
	}
	return count;
}


void fill(int x, int y){

	if ( x >= 0 && y >= 0 && x < SIZE_X && y < SIZE_Y &&  map[x][y] == -1){
		map[x][y] = 0;
		fill(x+1,y);
		fill(x-1,y);
		fill(x,y-1);
		fill(x,y+1);
	}
}

int find_fill(int x, int y){

	copy_map(map, fillCopy);
	return fill_check(x,y);
}

int fill_check(int x, int y){

	int total = 0;

	if ( x >= 0 && y >= 0 && x < SIZE_X && y < SIZE_Y){
		if(fillCopy[x][y] == -1){
			fillCopy[x][y] = 1;
			total += fill_check(x+1,y);
			total += fill_check(x-1,y);
			total += fill_check(x,y-1);
			total += fill_check(x,y+1);
			total += 1;
		}
	}
	return total;

}


void smooth(int map[][SIZE_Y], int range){
	
	int copy[SIZE_X][SIZE_Y];
	copy_map(map, copy);

	for( int y = 0; y < SIZE_Y; y++){
		for(int x = 0; x < SIZE_X; x++){
			int nCount = get_neighbors(x,y,range);
			if( map[x][y] == 0 && nCount < death){
				copy[x][y] = -1;
			}if( map[x][y] == -1 && nCount > birth){
				copy[x][y] = 0;
			}
		}
	}

	copy_map(copy, map);
}

void copy_map(int map[SIZE_X][SIZE_Y], int copy[][SIZE_Y]){
	
	for( int y = 0; y < SIZE_Y; y++){
		for(int x = 0; x < SIZE_X; x++){
			copy[x][y] = map[x][y];	
		}
	}
	return;
}

godot_variant new_godot_vector2(double x, double y){

	godot_vector2 v;
	godot_variant ret;
	godot_variant rx;
	godot_variant ry;
	
	godot_real ryr;
	godot_real rxr;

	api->godot_variant_new_real(&rx, x);
	api->godot_variant_new_real(&ry, y);
	
	ryr = api->godot_variant_as_real(&ry);
	rxr = api->godot_variant_as_real(&rx);

	api->godot_vector2_set_x(&v, rxr);
	api->godot_vector2_set_y(&v, ryr);

	api->godot_variant_destroy(&rx);
	api->godot_variant_destroy(&ry);

	api->godot_variant_new_vector2(&ret, &v);

	return ret;
}

godot_variant simple_build_dictionary(){
	
	godot_dictionary dict;
	godot_dictionary_new(&dict);
	godot_variant ret;

	for(int x = 0; x < SIZE_X; x++){
		for(int y = 0; y < SIZE_Y; y++){
			godot_variant cell;
		        cell = new_godot_vector2((double)x,(double)y);
			godot_variant value;
			godot_variant_new_int(&value, map[x][y]);
			godot_dictionary_set(&dict, &cell, &value);
			godot_variant_destroy(&value);
			godot_variant_destroy(&cell);
		}
	}
	godot_variant_new_dictionary(&ret, &dict);
	godot_dictionary_destroy(&dict);
	return ret;
}
godot_variant find_entrances(godot_variant ents){
	
	int entSize = 2;

	godot_dictionary dictionary = godot_variant_as_dictionary(&ents);
	godot_array dirs = godot_dictionary_keys(&dictionary);
	
	godot_variant n = godot_array_get(&dirs, 0);
	godot_variant s = godot_array_get(&dirs, 1);
	godot_variant e = godot_array_get(&dirs, 2);
	godot_variant w = godot_array_get(&dirs, 3);
	
	godot_variant cell1;
	godot_variant cell2;
	
	godot_array ent1;
	godot_array ent2;

	godot_array_new(&ent1);
	godot_array_new(&ent2);

	for(int y = 0; y < SIZE_Y; y++){
		cell1 = new_godot_vector2(0,y);
		if(map[0][y] == -1){
			godot_array_push_back(&ent1, &cell1);
		}else{
			if( godot_array_size(&ent1) < 1){
				godot_array_pop_back(&ent1);
			}else{
				godot_variant hold;
				godot_variant_new_array(&hold,&ent1);
				godot_variant temp = godot_dictionary_get(&dictionary, &w);
				godot_array aTemp = godot_variant_as_array(&temp);
				godot_array_push_back(&aTemp, &hold);

				godot_variant_destroy(&hold);
				godot_variant_destroy(&temp);
				godot_array_destroy(&aTemp);
				godot_array_destroy(&ent1);

				godot_array_new(&ent1);
			}
		}
		cell2 = new_godot_vector2(SIZE_X-1,y);
		if(map[SIZE_X-1][y] == -1){
			godot_array_push_back(&ent2, &cell2);
		}else{
			if( godot_array_size(&ent2) < 1){
				godot_array_pop_back(&ent2);
			}else{
				godot_variant hold;
				godot_variant_new_array(&hold,&ent2);
				godot_variant temp = godot_dictionary_get(&dictionary, &e);
				godot_array aTemp = godot_variant_as_array(&temp);
				godot_array_push_back(&aTemp, &hold);

				godot_variant_destroy(&hold);
				godot_variant_destroy(&temp);
				godot_array_destroy(&aTemp);
				godot_array_destroy(&ent2);

				godot_array_new(&ent2);
			}
		}
	}
	if (godot_array_size(&ent1) > 1){
		godot_variant hold;
		godot_variant_new_array(&hold, &ent1);
		godot_variant temp = godot_dictionary_get(&dictionary, &w);
		godot_array aTemp = godot_variant_as_array(&temp);
		godot_array_push_back(&aTemp, &hold);

		godot_variant_destroy(&hold);
		godot_variant_destroy(&temp);
		godot_array_destroy(&aTemp);
		godot_array_destroy(&ent1);

		godot_array_new(&ent1);
	}
	if (godot_array_size(&ent2) > 1){
		godot_variant hold;

		godot_variant_new_array(&hold, &ent2);
		godot_variant temp = godot_dictionary_get(&dictionary, &e);
		godot_array aTemp = godot_variant_as_array(&temp);
		godot_array_push_back(&aTemp, &hold);

		godot_variant_destroy(&hold);
		godot_variant_destroy(&temp);
		godot_array_destroy(&aTemp);
		godot_array_destroy(&ent2);

		godot_array_new(&ent2);
	}

	for(int x = 0; x < SIZE_X; x++){
		cell1 = new_godot_vector2(x,0);
		if(map[x][0] == -1){
			godot_array_push_back(&ent1, &cell1);
		}else{
			if( godot_array_size(&ent1) < 1){
				godot_array_pop_back(&ent1);
			}else{
				godot_variant hold;
				godot_variant_new_array(&hold,&ent1);
				godot_variant temp = godot_dictionary_get(&dictionary, &n);
				godot_array aTemp = godot_variant_as_array(&temp);
				godot_array_push_back(&aTemp, &hold);

				godot_variant_destroy(&hold);
				godot_variant_destroy(&temp);
				godot_array_destroy(&aTemp);
				godot_array_destroy(&ent1);

				godot_array_new(&ent1);
			}
		}
		cell2 = new_godot_vector2(x,SIZE_Y-1);
		if(map[x][SIZE_Y-1] == -1){
			godot_array_push_back(&ent2, &cell2);
		}else{
			if( godot_array_size(&ent2) < 1){
				godot_array_pop_back(&ent2);
			}else{
				godot_variant hold;
				godot_variant_new_array(&hold,&ent2);
				godot_variant temp = godot_dictionary_get(&dictionary, &s);
				godot_array aTemp = godot_variant_as_array(&temp);

				godot_array_push_back(&aTemp, &hold);
				godot_variant_destroy(&hold);
				godot_variant_destroy(&temp);
				godot_array_destroy(&aTemp);
				godot_array_destroy(&ent2);

				godot_array_new(&ent2);
			}
		}
	}
	if (godot_array_size(&ent1) > 1){
		godot_variant hold;
		godot_variant_new_array(&hold, &ent1);
		godot_variant temp = godot_dictionary_get(&dictionary, &n);
		godot_array aTemp = godot_variant_as_array(&temp);
		godot_array_push_back(&aTemp, &hold);

		godot_variant_destroy(&hold);
		godot_variant_destroy(&temp);
		godot_array_destroy(&aTemp);
		godot_array_destroy(&ent1);

		godot_array_new(&ent1);
	}
	if (godot_array_size(&ent2) > 1){
		godot_variant hold;
		godot_variant_new_array(&hold, &ent2);
		godot_variant temp = godot_dictionary_get(&dictionary, &s);
		godot_array aTemp = godot_variant_as_array(&temp);
		godot_array_push_back(&aTemp, &hold);

		godot_variant_destroy(&hold);
		godot_variant_destroy(&temp);
		godot_array_destroy(&aTemp);
		godot_array_destroy(&ent1);

		godot_array_new(&ent2);
	}

	//free all memory here
	godot_array_destroy(&dirs);
	
	godot_variant_destroy(&n);
	godot_variant_destroy(&s);
	godot_variant_destroy(&e);
	godot_variant_destroy(&w);
	
	godot_variant_destroy(&cell2);
	godot_variant_destroy(&cell1);

	godot_array_destroy(&ent1);
	godot_array_destroy(&ent2);
	
	//godot_dictionary_destroy(&dictionary);
	return ents;
}

