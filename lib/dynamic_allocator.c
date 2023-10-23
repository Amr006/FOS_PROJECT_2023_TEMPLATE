/*
 * dynamic_allocator.c
 *
 *  Created on: Sep 21, 2023
 *      Author: HP
 */
#include <inc/assert.h>
#include <inc/string.h>
#include "../inc/dynamic_allocator.h"


//==================================================================================//
//============================== GIVEN FUNCTIONS ===================================//
//==================================================================================//

//=====================================================
// 1) GET BLOCK SIZE (including size of its meta data):
//=====================================================
uint32 get_block_size(void* va)
{
	struct BlockMetaData *curBlkMetaData = ((struct BlockMetaData *)va - 1) ;
	return curBlkMetaData->size ;
}

//===========================
// 2) GET BLOCK STATUS:
//===========================
int8 is_free_block(void* va)
{
	struct BlockMetaData *curBlkMetaData = ((struct BlockMetaData *)va - 1) ;
	return curBlkMetaData->is_free ;
}

//===========================================
// 3) ALLOCATE BLOCK BASED ON GIVEN STRATEGY:
//===========================================
void *alloc_block(uint32 size, int ALLOC_STRATEGY)
{
	void *va = NULL;
	switch (ALLOC_STRATEGY)
	{
	case DA_FF:
		va = alloc_block_FF(size);
		break;
	case DA_NF:
		va = alloc_block_NF(size);
		break;
	case DA_BF:
		va = alloc_block_BF(size);
		break;
	case DA_WF:
		va = alloc_block_WF(size);
		break;
	default:
		cprintf("Invalid allocation strategy\n");
		break;
	}
	return va;
}

//===========================
// 4) PRINT BLOCKS LIST:
//===========================

void print_blocks_list(struct MemBlock_LIST list)
{
	cprintf("=========================================\n");
	struct BlockMetaData* blk ;
	cprintf("\nDynAlloc Blocks List:\n");
	LIST_FOREACH(blk, &list)
	{
		cprintf("(size: %d, isFree: %d)\n", blk->size, blk->is_free) ;
	}
	cprintf("=========================================\n");

}
//
////********************************************************************************//
////********************************************************************************//

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//

//==================================
// [1] INITIALIZE DYNAMIC ALLOCATOR:
//==================================
struct MemBlock_LIST MemoryData;
void initialize_dynamic_allocator(uint32 daStart, uint32 initSizeOfAllocatedSpace)
{
	//=========================================
	//DON'T CHANGE THESE LINES=================
	if (initSizeOfAllocatedSpace == 0)
		return ;
	//=========================================
	//=========================================
	struct BlockMetaData *metadata = (struct BlockMetaData *)daStart;
	metadata->is_free=1;
	metadata->prev_next_info.le_next=NULL;
	metadata->prev_next_info.le_prev=NULL;
	metadata->size=initSizeOfAllocatedSpace;


	LIST_INIT(&MemoryData);
	LIST_INSERT_HEAD(&MemoryData,metadata);
	LIST_LAST(&MemoryData) = metadata;

	//TODO: [PROJECT'23.MS1 - #5] [3] DYNAMIC ALLOCATOR - initialize_dynamic_allocator()
	//panic("initialize_dynamic_allocator is not implemented yet");
}

//=========================================
// [4] ALLOCATE BLOCK BY FIRST FIT:
//=========================================
void *alloc_block_FF(uint32 size)
{
	//TODO: [PROJECT'23.MS1 - #6] [3] DYNAMIC ALLOCATOR - alloc_block_FF()
//	panic("alloc_block_FF is not implemented yet");
	//cprintf("Invalid allocation strategy\n");
	if(size==0){
		return NULL;
	}
	struct BlockMetaData *element ;
	int i = 1 ;
	int done=0;

	LIST_FOREACH(element , &MemoryData)
	{
		int sizeBlock = element->size - sizeOfMetaData() ;
		//fit the block
		//cprintf("Invalid allocation strategy %d : \n" , element->size);
		if ( element->is_free == 1 && (size)  == sizeBlock)
		{
			done=1;
			element->is_free = 0;
			void *allocatedSpace = (void*)((char*)element + sizeOfMetaData());
			return (allocatedSpace) ;
		}else if( element->is_free == 1 && (size) < sizeBlock)
		{
			done=1;
			element->is_free = 0;
			//cprintf("Invalid allocation strategy %d : \n" , ((void*)((char*)element + size + sizeOfMetaData())));
			struct BlockMetaData *metadata = (struct BlockMetaData *)((void*)((char*)element + size + sizeOfMetaData()));
			metadata->is_free=1;
			metadata->prev_next_info.le_next=NULL;
			metadata->prev_next_info.le_prev= element;
			element->prev_next_info.le_next=metadata;
			metadata->size = element->size - size - sizeOfMetaData();
			element->size = size + sizeOfMetaData() ;
			LIST_INSERT_TAIL(&MemoryData,metadata);
			void *allocatedSpace = (void*)((char*)element + sizeOfMetaData());
			return (allocatedSpace);
		}
	}
	if(done == 0){
		//cprintf("Invalid allocation strategy\n");
		int *ret=(int *)sbrk(size);
		return NULL;
		if(ret == (void*)-1){
			return NULL;
		}

	}
	return NULL;

}



//=========================================
// [5] ALLOCATE BLOCK BY BEST FIT:
//=========================================
void *alloc_block_BF(uint32 size)
{
	//TODO: [PROJECT'23.MS1 - BONUS] [3] DYNAMIC ALLOCATOR - alloc_block_BF()
	panic("alloc_block_BF is not implemented yet");
	return NULL;
}

//=========================================
// [6] ALLOCATE BLOCK BY WORST FIT:
//=========================================
void *alloc_block_WF(uint32 size)
{
	panic("alloc_block_WF is not implemented yet");
	return NULL;
}

//=========================================
// [7] ALLOCATE BLOCK BY NEXT FIT:
//=========================================
void *alloc_block_NF(uint32 size)
{
	panic("alloc_block_NF is not implemented yet");
	return NULL;
}

//===================================================
// [8] FREE BLOCK WITH COALESCING:
//===================================================
void free_block(void *va)
{
	if(va==NULL)
		return;
	//TODO: [PROJECT'23.MS1 - #7] [3] DYNAMIC ALLOCATOR - free_block()
	//panic("free_block is not implemented yet");
	//struct BlockMetaData *metadata=	(struct BlockMetaData *)(void*)va - sizeOfMetaData();
	struct BlockMetaData *currmetadata=	((struct BlockMetaData *)va - 1);

	void* curVA = (void*) KERNEL_HEAP_START ;


	if(currmetadata->prev_next_info.le_next==NULL&&currmetadata->prev_next_info.le_prev!=NULL){
			struct BlockMetaData *prevmetadata=currmetadata->prev_next_info.le_prev;
			if(prevmetadata->is_free==0){
				currmetadata->is_free=1;
				cprintf("Invalid allocation strategy %d : \n" , 1);
			}else{
				prevmetadata->size+=currmetadata->size;

				currmetadata->size= 0 ;
				currmetadata->is_free= 0;
				LIST_REMOVE(&MemoryData,currmetadata);
				prevmetadata->prev_next_info.le_next=NULL;
				cprintf("Invalid allocation strategy %d : \n" , 2);
			}
	}else if(currmetadata->prev_next_info.le_next!=NULL&&currmetadata->prev_next_info.le_prev==NULL){
		struct BlockMetaData *nextmetadata=currmetadata->prev_next_info.le_next;
		if(nextmetadata->is_free==0){
			currmetadata->is_free=1;
			cprintf("Invalid allocation strategy %d : \n" , 3);
		}else{
			cprintf("Invalid allocation strategy %d : \n" , 20);
			currmetadata->size+=nextmetadata->size;
			currmetadata->prev_next_info.le_next=nextmetadata->prev_next_info.le_next;
			LIST_REMOVE(&MemoryData,nextmetadata);
			nextmetadata->size=0;
			nextmetadata->is_free=0;
			currmetadata->is_free=1;
			cprintf("Invalid allocation strategy %d : \n" , 4);
		}
	}else{
		        struct BlockMetaData *prevmetadata=currmetadata->prev_next_info.le_prev;
				struct BlockMetaData *nextmetadata=currmetadata->prev_next_info.le_next;
				cprintf("Invalid allocation strategy %d : \n" , 15);
				cprintf("Invalid allocation strategy %d : \n" , currmetadata->size);
				    if(prevmetadata->is_free==0&&nextmetadata->is_free==0){
						currmetadata->is_free=1;
						cprintf("Invalid allocation strategy %d : \n" , 5);
					}else if(prevmetadata->is_free==1&&nextmetadata->is_free==0){
						prevmetadata->size+=currmetadata->size;
						prevmetadata->prev_next_info.le_next=nextmetadata;
						nextmetadata->prev_next_info.le_prev=prevmetadata;
						currmetadata->size= 0 ;
						currmetadata->is_free= 0;
						LIST_REMOVE(&MemoryData,currmetadata);
						cprintf("Invalid allocation strategy %d : \n" , 6);
					}else if(prevmetadata->is_free==0&&nextmetadata->is_free==1){
						currmetadata->size+=nextmetadata->size;
						currmetadata->prev_next_info.le_next=nextmetadata->prev_next_info.le_next;
						nextmetadata->prev_next_info.le_next->prev_next_info.le_prev=currmetadata;
						currmetadata->is_free=1;
						nextmetadata->size= 0 ;
						nextmetadata->is_free= 0;
						LIST_REMOVE(&MemoryData,nextmetadata);
						cprintf("Invalid allocation strategy %d : \n" , 7);
					}else{
						prevmetadata->size+=(nextmetadata->size+currmetadata->size);
						cprintf("Invalid allocation strategy %d : \n" , 8);

						nextmetadata->size= 0 ;
						nextmetadata->is_free= 0;
						currmetadata->size= 0 ;
						currmetadata->is_free= 0;
						LIST_REMOVE(&MemoryData,nextmetadata);
						LIST_REMOVE(&MemoryData,currmetadata);
					}
	}

}

//=========================================
// [4] REALLOCATE BLOCK BY FIRST FIT:
//=========================================
void *realloc_block_FF(void* va, uint32 new_size)
{
	//TODO: [PROJECT'23.MS1 - #8] [3] DYNAMIC ALLOCATOR - realloc_block_FF()
	panic("realloc_block_FF is not implemented yet");
	return NULL;
}
