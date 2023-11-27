#include <inc/lib.h>
#include <inc/environment_definitions.h>
#include<lib/syscall.c>

//==================================================================================//
//============================== GIVEN FUNCTIONS ===================================//
//==================================================================================//
//int arr_CheckFree[3000] = {1};
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
		alloc_block_FF(size);
		return NULL;
	}
//	else {
//		unsigned int SIZE = ROUNDUP(size,PAGE_SIZE);
////		cprintf("the size is %u\n", SIZE);
////		uint32 limit = sys_getKlimit();
//		uint32 limit = USER_HEAP_START + DYN_ALLOC_MAX_SIZE;
//		int numOfFrames=SIZE/PAGE_SIZE;
////		cprintf("the number of frames are %d\n", numOfFrames);
//		uint32 va = limit+PAGE_SIZE;
////		cprintf("the start address of the page alloc is %u\n", va);
//		if(sys_isUHeapPlacementStrategyFIRSTFIT()){
//			int counter=0;
////			cprintf("amora1\n");
//			for(uint32 address=va;address<USER_HEAP_MAX;address+=PAGE_SIZE){
//			    uint32 PERM = sys_get_page_premission(address);
//				if(((PERM & PERM_AVAILABLE) != PERM_AVAILABLE)){
////					cprintf("accepted address: %u\n", address);
//					counter++;
//				}else{
//					counter=0;
//				}
//				if(counter==numOfFrames){
//                    uint32 FreeCount = myEnv->freeCounter;
//                    volatile int * startArray =  myEnv->startAdd;
//                    volatile int * startSize =  myEnv->sizeAdd;
//					uint32 start_address = address-(((numOfFrames)*PAGE_SIZE));
//                    startArray[FreeCount] = start_address;
//					startSize[FreeCount] = SIZE;
//					FreeCount++;
//					for(uint32 add=start_address;add<=address;add+=PAGE_SIZE){
//						struct FrameInfo *ptr=NULL;
//						cprintf("the address marked is: %u", add);
//						sys_allocate_user_mem(add, PAGE_SIZE);
//					}
//					return (void*)start_address;
//				}
//			}
//		}

//	}
	return (void*) (USER_HEAP_START + DYN_ALLOC_MAX_SIZE + PAGE_SIZE);
	//Use sys_isUHeapPlacementStrategyFIRSTFIT() and	sys_isUHeapPlacementStrategyBESTFIT()
	//to check the current strategy

}

//=================================
// [3] FREE SPACE FROM USER HEAP:
//=================================
void free(void* virtual_address)
{
	//TODO: [PROJECT'23.MS2 - #11] [2] USER HEAP - free() [User Side]
	// Write your code here, remove the panic and write your code
//	uint32 FreeCount = myEnv->freeCounter; // sys_getFreeCounter();
//	volatile int * startArray = myEnv->startAdd; //sys_getStartAddr();
//	volatile int * startSize = myEnv->sizeAdd; // sys_getStartSize();
//	uint32 Start = myEnv->start; // sys_getStart();
//	uint32 SegBreak = myEnv->seg_break; // sys_getSeg();
//	uint32 limit = myEnv->limit; // sys_getKlimit();
//	if(((uint32)virtual_address>=Start)&&((uint32)virtual_address<= SegBreak)){
//    	free_block(virtual_address);
//    }
//    else
//    	if(((uint32)virtual_address >= (limit + PAGE_SIZE)) && ((uint32)virtual_address < KERNEL_HEAP_MAX)){
//    	for(int i = 0 ; i <  FreeCount ; i++)
//    			{
//    				if((void *)startArray[i] == virtual_address)
//    				{
//    					sys_free_user_mem((uint32)virtual_address,  startSize[i]);
//    				}
//    			}
//   }else{
//	   panic("kfree() painc");
//   }
//	panic("free() is not implemented yet...!!");
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
