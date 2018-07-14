#include <gdnative_api_struct.gen.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

static int SIZE_X = 20;
static int SIZE_Y = 20;

int birth = 4;
int death = 3;

const int FILL = 40;

int map[20][20];

void smooth_map(int count, int extra);

void print_map(int map[][SIZE_Y]);

int get_neighbors(int x, int y, int map[][SIZE_Y],int range);

void gen_map(int map[SIZE_X][SIZE_Y]);

void copy_map(int map[SIZE_X][SIZE_Y], int copy[][SIZE_Y]);

void smooth(int map[][SIZE_Y], int range);



const godot_gdnative_core_api_struct *api = NULL;
const godot_gdnative_ext_nativescript_api_struct *nativescript_api = NULL;

GDCALLINGCONV void *simple_constructor(godot_object *p_instance, void *p_method_data);
GDCALLINGCONV void simple_destructor(godot_object *p_instance, void *p_method_data, void *p_user_data);
godot_variant simple_get_map(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);
godot_variant simple_alt_map(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);

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

                godot_instance_method alt_map = {
                        .method = &simple_alt_map,
                        .method_data = 0,
                        .free_func = 0
		};
                godot_method_attributes attr = {
                        .rpc_type = GODOT_METHOD_RPC_MODE_DISABLED
                };

                godot_nativescript_register_method(desc, "SIMPLE", "get_map", attr, get_map);
		
		godot_nativescript_register_method(desc, "SIMPLE", "get_alt_map", attr, alt_map);

        }

}

GDCALLINGCONV void *simple_constructor(godot_object *p_instance, void *p_method_data) {
	printf("SIMPLE._init()\n");
	
	return 0;
}

GDCALLINGCONV void simple_destructor(godot_object *p_instance, void *p_method_data, void *p_user_data) {
	printf("SIMPLE._byebye()\n");

	api->godot_free(p_user_data);
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

	//api->godot_destroy(&ryr);
	//api->godot_destroy(&rxr);

	api->godot_variant_destroy(&rx);
	api->godot_variant_destroy(&ry);

	api->godot_variant_new_vector2(&ret, &v);

	//api->godot_vector2_destroy(&v);

	return ret;

}

godot_variant simple_build_dictionary(){
	
	godot_dictionary dict;
	api->godot_dictionary_new(&dict);
	godot_variant ret;

	for(int x = 0; x < SIZE_X; x++){
		for(int y = 0; y < SIZE_Y; y++){
			godot_variant cell;
		        cell = new_godot_vector2((double)x,(double)y);
			godot_variant value;
			api->godot_variant_new_int(&value, map[x][y]);
			api->godot_dictionary_set(&dict, &cell, &value);
			api->godot_variant_destroy(&value);
			api->godot_variant_destroy(&cell);
		}
	}
	godot_variant_new_dictionary(&ret, &dict);
	godot_dictionary_destroy(&dict);
	return ret;
}

void fill_map(godot_dictionary dictionary);


godot_variant simple_alt_map(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

	godot_variant ret;
	
	birth = 8;
	death = 3;
	godot_dictionary dictIn;

	dictIn = godot_variant_as_dictionary(*p_args);

	fill_map(dictIn);

	smooth_map(4,2);

	ret = simple_build_dictionary();

	return ret;
}
godot_variant simple_get_map(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {
	godot_variant ret;
	
	birth = 4;
	godot_dictionary dictIn;

	dictIn = godot_variant_as_dictionary(*p_args);

	fill_map(dictIn);

	//godot_string s;

	//s = godot_string_num(p_num_args);

	//godot_print(&s);

	smooth_map(8,1);

	ret = simple_build_dictionary();

	return ret;
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
	godot_dictionary_destroy(&dictionary);
}

void smooth_map(int count, int extra)
{

	for(int i = 0; i < count; i++){
		smooth(map, extra);
	}
}


int get_neighbors(int xLoc, int yLoc, int map[][SIZE_Y], int size){
	
	int count = 0;

	for(int x = xLoc-1; x <= xLoc+1; x++){
		for(int y = yLoc-1; y <= yLoc+1; y++){
			if ( x >= 0 && y >= 0 && x < SIZE_X && y < SIZE_Y ){
				if( map[x][y] == 0){
					if(x != xLoc || y != yLoc){
						count += 1;
					}
					if(size ==2){
						count += get_neighbors(x, y, map, 1);
					}
				}
			} else{	
			count += 1;
			}
		}
	}
	return count;
}

void smooth(int map[][SIZE_Y], int range){
	
	int copy[SIZE_X][SIZE_Y];
	copy_map(map, copy);

	for( int y = 0; y < SIZE_Y; y++){
		for(int x = 0; x < SIZE_X; x++){
			int nCount = get_neighbors(x,y, map, range);
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

void gen_map(int map[SIZE_X][SIZE_Y] ){

	for( int y = 0; y < SIZE_Y; y++){
		for(int x = 0; x < SIZE_X; x++){
			if( rand() % 101 < FILL){
				map[x][y] = 0;
			}else{
				map[x][y] = -1;
			}
		}
	}
	return;
}

