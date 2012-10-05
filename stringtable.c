
#include "stringtable.h"
#include "strhash.h"

#define TABLE_NEW_SIZE 31

struct stringnode {			//typedefd as *stringnode_ref
	uint32_t hash_code;
	char *string_entry;
	stringnode_ref next;
};

struct stringtable {		//typedefd as *stringtable_ref
	size_t table_length;	//size of table including empty entries
	int entries;
	double load;
	stringnode_ref *table_node_ref;
};

stringtable_ref new_stringtable(void) {
	stringtable_ref new_table = malloc(sizeof (struct stringtable));
	assert(new_table != NULL);
	new_table->table_length = TABLE_NEW_SIZE;
	new_table->entries = 0;
	new_table->load = 0;
	new_table->table_node_ref = malloc(new_table->table_length * 
								  sizeof (stringnode_ref));
	
	for (size_t index = 0; index < new_table->table_length; index++) {
		new_table->table_node_ref[index] = NULL;
	}
	
	assert (new_table->table_node_ref != NULL);
	//DEBUGF
	
	return new_table;
}
/*
void debugdump_stringtable(stringtable_ref table, FILE* file_ptr) {
	

}
*/
	
stringnode_ref intern_stringtable(stringtable_ref table, cstring str) {
	char *str_buffer = NULL;
	str_buffer = strndup(str, 128);
	
	hashcode_t hashVal = strhash(str_buffer) % table->table_length;
	
	stringnode_ref node_ref = table->table_node_ref[hashVal];
	
	//Collision probing by linked list of node_ref
	while (node_ref != NULL) {			//If the word exists, go next node
		if (node_ref->hash_code == hashVal)
			return node_ref;
		node_ref = node_ref->next;
	}
	
	stringnode_ref new_node = malloc(sizeof (struct stringnode));
	table->table_node_ref[hashVal] = new_node;
	new_node->hash_code = hashVal;
	new_node->string_entry = str_buffer;
	new_node->next = NULL;
	
	table->entries += 1;
	table->load = (double) (table->entries) / (table->table_length);
	
	if (table->load >= 0.4) {
		;
		//NEED TO IMPLEMENT
		//realloc_stringtable(table);
	}
	
	return new_node;
}

void realloc_stringtable(stringtable_ref old_table) {
	int old_length = old_table->table_length;
	int new_length = (old_table->table_length)*2 + 1;
	old_table->table_length = new_length;
	
	//new_table will replace old_table in the stringtable_ref pointer
	stringnode_ref *new_table = malloc((new_length * 
										sizeof(stringnode_ref));
	assert(new_table != NULL);
	
	for (size_t index = 0; index < new_length; index++) {
		new_table[index] = NULL;
	}
	
	//Moving all entries from old table into new table
	for (int i = 0; i < old_length; i++) {
		//Go through all the nodes in the linked list of old table
		while ((old_node_ref = old_table->table_node_ref[i]) != NULL) {
			hashcode_t hashVal = old_node_ref->hash_code % new_length;

			//Skip filled nodes in the linked list of new table
			stringnode_ref new_node_ref = new_table[hashVal];
			while (new_node_ref != NULL) {
				new_node_ref = new_node_ref->next;
			}
			
			new_node_ref = old_node_ref;
			
}
	
cstring peek_stringtable(stringnode_ref node_ref) {
	return node_ref->string_entry;
}
	
hashcode_t hashcode_stringtable(stringnode_ref node_ref) {
	return node_ref->hash_code;
}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	

