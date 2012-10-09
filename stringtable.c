
#include "stringtable.h"
#include "strhash.h"

#define TABLE_NEW_SIZE 15

struct stringnode {			//typedefd as *stringnode_ref
	uint32_t hash_code;
	char *string_entry;
	stringnode_ref next;
};

struct stringtable {		//typedefd as *stringtable_ref
	size_t table_length;	//size of table including empty entries
	int entries;
	double load;
	stringnode_ref *node_ref_array;
};

stringtable_ref new_stringtable(void) {
	stringtable_ref new_table = malloc(sizeof (struct stringtable));
	assert(new_table != NULL);
	new_table->table_length = TABLE_NEW_SIZE;
	new_table->entries = 0;
	new_table->load = 0;
	new_table->node_ref_array = malloc(new_table->table_length * 
								  sizeof (stringnode_ref));
	
	for (size_t index = 0; index < new_table->table_length; index++) {
		new_table->node_ref_array[index] = NULL;
	}
	
	assert (new_table->node_ref_array != NULL);
	
	return new_table;
}

void delete_stringtable (stringtable_ref table) {
	for (size_t i = 0; i < table->table_length; i++) {
		stringnode_ref node_ref = table->node_ref_array[i];
		stringnode_ref temp_node = NULL;
		
		if (node_ref == NULL) {
			free(node_ref);
		}
	
		while (node_ref != NULL) {				
			temp_node = node_ref;
			node_ref = node_ref->next;
			free(temp_node->string_entry);
			free(temp_node);
		}
	}
	
	free(table->node_ref_array);
	free(table);
}

void debugdump_stringtable(stringtable_ref table, FILE* file_ptr) {
	for (size_t i = 0; i < table->table_length; i++) {
		stringnode_ref node_ref = table->node_ref_array[i];
	
		if (node_ref != NULL) {
			fprintf(file_ptr, "%8Zu%12u   %s\n", i, node_ref->hash_code, 
					node_ref->string_entry);
			node_ref = node_ref->next;
		}
		while (node_ref != NULL) {			
			fprintf(file_ptr, "%12u   %s\n", node_ref->hash_code,
					node_ref->string_entry);
			node_ref = node_ref->next;
		}
	
	}
}

	
stringnode_ref intern_stringtable(stringtable_ref table, cstring str) {
	char *str_buffer = NULL;
	str_buffer = strdup(str);
	
	hashcode_t hashVal = strhash(str_buffer) % table->table_length;
	
	stringnode_ref node_ref = table->node_ref_array[hashVal];
	
	stringnode_ref new_node = malloc(sizeof (struct stringnode));
	new_node->hash_code = strhash(str_buffer);
	new_node->string_entry = str_buffer;
	new_node->next = NULL;
	
	if (node_ref == NULL) {
		table->node_ref_array[hashVal] = new_node;
		table->entries += 1;
		table->load = (double) (table->entries) / (table->table_length);
	
		if (table->load >= 0.5) {
			realloc_stringtable(table);
		}
		return new_node;
	}

		
	
	//Collision probing by linked list of node_ref
	while (node_ref != NULL) {			//If the node exists, go next node
		if (node_ref->hash_code == hashVal)
			return node_ref;
		if (node_ref->next == NULL)
			break;
		node_ref = node_ref->next;
	}
	
	node_ref->next = new_node;
	
	
	DEBUGF('s', "hashheader: %u \thashno: %12u \tstring: %s\n", 
			hashVal, 
			table->node_ref_array[hashVal]->hash_code, 
			table->node_ref_array[hashVal]->string_entry);
	
	table->entries += 1;
	table->load = (double) (table->entries) / (table->table_length);
	
	if (table->load >= 0.5) {
		realloc_stringtable(table);
	}

	return new_node;
}

void realloc_stringtable(stringtable_ref table) {
	int old_length = table->table_length;
	int new_length = (table->table_length)*2 + 1;
	table->table_length = new_length;
	
	//new_array will replace table array in the stringtable_ref pointer
	stringnode_ref *new_array = malloc(new_length * sizeof(stringnode_ref));
	assert(new_array != NULL);
	
	for (int index = 0; index < new_length; index++) {
		new_array[index] = NULL;
	}
	
	//Moving all entries from old table into new table
	for (int i = 0; i < old_length; i++) {
		//Go through all the nodes in the linked list of old table
		stringnode_ref old_node_ref = table->node_ref_array[i];
		while (old_node_ref != NULL) {	
			hashcode_t hashVal = old_node_ref->hash_code % new_length;
			
			printf("%d %s\n", hashVal, old_node_ref->string_entry);

			//Skip filled nodes in the linked list of new table
			stringnode_ref new_node_ref = new_array[hashVal];
			
			if (new_array[hashVal] == NULL) {
				new_array[hashVal] = old_node_ref;
				old_node_ref = old_node_ref->next;
				continue;
			}
			
			while (new_node_ref != NULL) {
				if (new_node_ref->next == NULL)
					break;
				new_node_ref = new_node_ref->next;
			}
			
			new_node_ref->next = old_node_ref;
			old_node_ref = old_node_ref->next;
			
			/*
			if (new_array[hashVal] == NULL)
				printf("node %d is NULL2\n", hashVal);
			
			printf("%d %s\n", hashVal, new_array[hashVal]->string_entry);
			*/
		}
	}
	
	free (table->node_ref_array);
	
	table->node_ref_array = new_array;
	
	/*
	printf("\nstring %s hashcode %d\n\n", 
			table->node_ref_array[8]->string_entry, 
			table->node_ref_array[8]->hash_code);
	
	//test debug
	for (size_t i = 0; i < table->table_length; i++) {
		printf("iter %Zu\n", i);
		stringnode_ref node_ref = table->node_ref_array[i];
	
		if (node_ref != NULL) {
			printf("%8Zu%12u   %s\n", i, node_ref->hash_code, 
					node_ref->string_entry);
			node_ref = node_ref->next;
		}
		
		while (node_ref != NULL) {			
			printf("%12u   %s\n", node_ref->hash_code,
					node_ref->string_entry);
			node_ref = node_ref->next;
		}
	
	}
	*/
		
}
	
cstring peek_stringtable(stringnode_ref node_ref) {
	return node_ref->string_entry;
}
	
hashcode_t hashcode_stringtable(stringnode_ref node_ref) {
	return node_ref->hash_code;
}


