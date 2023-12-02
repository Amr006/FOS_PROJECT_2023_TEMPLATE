#include <inc/lib.h>
#include <inc/environment_definitions.h>
#include<inc/dynamic_allocator.h>


//==================================================================================//
//============================== GIVEN FUNCTIONS ===================================//
//==================================================================================//

uint32 addSize[1024 * 1024] = {0};


int FirstTimeFlag = 1;
void InitializeUHeap()
{
	if(FirstTimeFlag)
	{
#if UHP_USE_BUDDY
		initialize_buddy();
		cprintf("BUDDY SYSTEM IS INITIALIZED\n");
#endif
		FirstTimeFlag = 0;
	}
}

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//

//=============================================
// [1] CHANGE THE BREAK LIMIT OF THE USER HEAP:
//=============================================
/*2023*/
void* sbrk(int increment)
{
	return (void*) sys_sbrk(increment);
}

//=================================
// [2] ALLOCATE SPACE IN USER HEAP:
//=================================
void* malloc(uint32 size)
{
	//==============================================================
	//DON'T CHANGE THIS CODE========================================
	InitializeUHeap();
	if (size == 0) return NULL ;
	//==============================================================
	//TODO: [PROJECT'23.MS2 - #09] [2] USER HEAP - malloc() [User Side]
	// Write your code here, remove the panic and write your code
	//	panic("malloc() is not implemented yet...!!");

	if (size <= DYN_ALLOC_MAX_BLOCK_SIZE){

		return alloc_block_FF(size);
	}
		size = ROUNDUP(size,PAGE_SIZE);
		uint32 limit = myEnv->limit;
		int numOfFrames=size/PAGE_SIZE;
		uint32 va = limit+PAGE_SIZE;
		if(sys_isUHeapPlacementStrategyFIRSTFIT()){

			int counter=0;
			for(uint32 address=va;address<USER_HEAP_MAX;address+=PAGE_SIZE){

				if(addSize[address / PAGE_SIZE] == 0){
					counter++;
				}else{
					counter=0;
				}

				if(counter==numOfFrames){

					uint32 start_address = address-(size - PAGE_SIZE);
					for (uint32 address2 = start_address; address2 <= address; address2+=PAGE_SIZE){
						addSize[address2 / PAGE_SIZE] = address;
					}
					sys_allocate_user_mem(start_address, size);
					return (void*)start_address;
				}
			}
		}
	return NULL;

}
//=================================
// [3] FREE SPACE FROM USER HEAP:
//=================================
void free(void* virtual_address)
{
	//TODO: [PROJECT'23.MS2 - #11] [2] USER HEAP - free() [User Side]
	// Write your code here, remove the panic and write your code
	uint32 size = 0;
	uint32 hard_limit = myEnv->limit;
	uint32 rounded_address = (uint32)virtual_address;
	if (rounded_address < USER_HEAP_START || rounded_address >= USER_HEAP_MAX){
		 panic("kfree() panic");
	}
		else if((rounded_address>=myEnv->start)&&(rounded_address < hard_limit)){
	    	free_block(virtual_address);
	    }
	    else if (rounded_address < USER_HEAP_MAX && rounded_address >= (hard_limit + PAGE_SIZE)){
	    	size = (addSize[rounded_address / PAGE_SIZE] - rounded_address);
	    	uint32 for_condition = addSize[rounded_address / PAGE_SIZE];
	    	for (uint32 va = rounded_address ; va <= for_condition ; va += PAGE_SIZE){
	    		addSize[va / PAGE_SIZE] = 0;
	    	}
			sys_free_user_mem(rounded_address, size);
	   }
}


//=================================
// [4] ALLOCATE SHARED VARIABLE:
//=================================
void* smalloc(char *sharedVarName, uint32 size, uint8 isWritable)
{
	//==============================================================
	//DON'T CHANGE THIS CODE========================================
	InitializeUHeap();
	if (size == 0) return NULL ;
	//==============================================================
	panic("smalloc() is not implemented yet...!!");
	return NULL;
}

//========================================
// [5] SHARE ON ALLOCATED SHARED VARIABLE:
//========================================
void* sget(int32 ownerEnvID, char *sharedVarName)
{
	//==============================================================
	//DON'T CHANGE THIS CODE========================================
	InitializeUHeap();
	//==============================================================
	// Write your code here, remove the panic and write your code
	panic("sget() is not implemented yet...!!");
	return NULL;
}


//==================================================================================//
//============================== BONUS FUNCTIONS ===================================//
//==================================================================================//

//=================================
// REALLOC USER SPACE:
//=================================
//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to malloc().
//	A call with new_size = zero is equivalent to free().

//  Hint: you may need to use the sys_move_user_mem(...)
//		which switches to the kernel mode, calls move_user_mem(...)
//		in "kern/mem/chunk_operations.c", then switch back to the user mode here
//	the move_user_mem() function is empty, make sure to implement it.
void *realloc(void *virtual_address, uint32 new_size)
{
	//==============================================================
	//DON'T CHANGE THIS CODE========================================
	InitializeUHeap();
	//==============================================================

	// Write your code here, remove the panic and write your code
	panic("realloc() is not implemented yet...!!");
	return NULL;

}


//=================================
// FREE SHARED VARIABLE:
//=================================
//	This function frees the shared variable at the given virtual_address
//	To do this, we need to switch to the kernel, free the pages AND "EMPTY" PAGE TABLES
//	from main memory then switch back to the user again.
//
//	use sys_freeSharedObject(...); which switches to the kernel mode,
//	calls freeSharedObject(...) in "shared_memory_manager.c", then switch back to the user mode here
//	the freeSharedObject() function is empty, make sure to implement it.

void sfree(void* virtual_address)
{
	// Write your code here, remove the panic and write your code
	panic("sfree() is not implemented yet...!!");
}


//==================================================================================//
//========================== MODIFICATION FUNCTIONS ================================//
//==================================================================================//

void expand(uint32 newSize)
{
	panic("Not Implemented");

}
void shrink(uint32 newSize)
{
	panic("Not Implemented");

}
void freeHeap(void* virtual_address)
{
	panic("Not Implemented");

}
