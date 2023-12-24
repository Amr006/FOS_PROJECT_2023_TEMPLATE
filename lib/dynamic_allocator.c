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
bool is_initialized = 0;
void initialize_dynamic_allocator(uint32 daStart, uint32 initSizeOfAllocatedSpace)
{
	//=========================================
	//DON'T CHANGE THESE LINES=================
	if (initSizeOfAllocatedSpace == 0)
		return ;
	is_initialized = 1;
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
    //test

	if(size==0){
		return NULL;
	}
	if(!is_initialized){
		uint32 required_size = size + sizeOfMetaData();
		uint32 da_start = (uint32)sbrk(required_size);
		uint32 da_break = (uint32)sbrk(0);
		initialize_dynamic_allocator(da_start, da_break - da_start);
	}
	struct BlockMetaData *element;
	struct BlockMetaData *Lastelement;
	uint32 reqsize=size+sizeOfMetaData();
	int allocated=0;
	LIST_FOREACH(element , &MemoryData)
	{

		uint32 sizeBlock = element->size;
		if(element->is_free == 1 && (reqsize)  == sizeBlock){
			allocated=1;
			element->is_free=0;
			char * returnedAdress=(char *)element+sizeOfMetaData();
		    return (void *)returnedAdress;
		}
		else if( element->is_free == 1 && (reqsize) < sizeBlock)
		{
			allocated=1;
			element->is_free = 0;
			if(element->size-reqsize > sizeOfMetaData()){
				char* address=(char*)element+reqsize;
				struct BlockMetaData *metadata = (struct BlockMetaData *)address;
				metadata->size=element->size-reqsize;
			    element->size=reqsize;
				metadata->is_free=1;
				LIST_INSERT_AFTER(&MemoryData,element,metadata);
			}
		    char * returnedAdress=(char *)element+sizeOfMetaData();
		    return (void *)returnedAdress;
		}
		Lastelement=element;
	}

	if(allocated == 0){
		uint32* ptr = (uint32 *) sbrk((size + sizeOfMetaData()));
		if (ptr != (void *)-1) {
			struct BlockMetaData * tmp2 = (struct BlockMetaData *) ((uint32) ptr
					+ size + sizeOfMetaData());
			tmp2->is_free = 1;
			tmp2->size = PAGE_SIZE - (size + sizeOfMetaData());
			element = (struct BlockMetaData *) ptr;
			element->size = size + sizeOfMetaData();
			element->is_free = 0;
			LIST_INSERT_TAIL(&MemoryData, element);
			LIST_INSERT_TAIL(&MemoryData, tmp2);
			return (struct BlockMetaData *) ((uint32) element + sizeOfMetaData());
	}
}

	return NULL;


}



//=========================================
// [5] ALLOCATE BLOCK BY BEST FIT:
//=========================================
void *alloc_block_BF(uint32 size)
{
	    if (size == 0) {
	        return NULL;
	    }


	    uint32 reqSize = size + sizeOfMetaData();

	    struct BlockMetaData *element;
	    struct BlockMetaData *bestFit = NULL;

	    LIST_FOREACH(element, &MemoryData) {
	        if (element->is_free && element->size >= reqSize) {
	            if (bestFit == NULL || element->size < bestFit->size) {
	                bestFit = element;
	            }
	        }
	    }

	    if (bestFit != NULL) {
	        bestFit->is_free = 0;
	        uint32 blockSize = bestFit->size;


	        if (blockSize - reqSize > sizeOfMetaData()) {
	            char *address = (char *)bestFit + reqSize;
	            struct BlockMetaData *metadata = (struct BlockMetaData *)address;
	            metadata->size = blockSize - reqSize;
	            metadata->is_free = 1;
	            bestFit->size = reqSize;
	            LIST_INSERT_AFTER(&MemoryData, bestFit, metadata);
	        }else{
	        	bestFit->size = reqSize;
	        }

	        return (void *)((char *)bestFit + sizeOfMetaData());
	    } else {

	        void *res = sbrk(reqSize);
	        if (res == (void *)-1) {
	            return NULL;
	        } else {
	            struct BlockMetaData *metadata = (struct BlockMetaData *)res;
	            metadata->size = reqSize;
	            metadata->is_free = 0;
	            LIST_INSERT_HEAD(&MemoryData, metadata);
	            return (void *)((char *)metadata + sizeOfMetaData());
	        }
	    }


	//TODO: [PROJECT'23.MS1 - BONUS] [3] DYNAMIC ALLOCATOR - alloc_block_BF()
	//panic("alloc_block_BF is not implemented yet");
	return NULL;
}

//=========================================
// [6] ALLOCATE BLOCK BY WORST FIT:
//=========================================
void *alloc_block_WF(uint32 size)
{
	//panic("alloc_block_WF is not implemented yet");
	return NULL;
}

//=========================================
// [7] ALLOCATE BLOCK BY NEXT FIT:
//=========================================
void *alloc_block_NF(uint32 size)
{
	if(size==0){
		return NULL;
	}
	struct BlockMetaData *element;
	uint32 reqsize=size+sizeOfMetaData();
	int allocated=0;
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

	struct BlockMetaData *currmetadata=	((struct BlockMetaData *)va - 1);
	struct BlockMetaData *prev = LIST_PREV(currmetadata);
	struct BlockMetaData *next = LIST_NEXT(currmetadata);
	if(next==NULL&&prev!=NULL){
			struct BlockMetaData *prevmetadata=prev;
			if(prevmetadata->is_free==0){
				currmetadata->is_free=1;
			}else{
				prevmetadata->size+=currmetadata->size;
				currmetadata->size=0;
				currmetadata->is_free= 0;
				LIST_REMOVE(&MemoryData,currmetadata);
				currmetadata = 0;
			}
	}else if(next!=NULL&&prev==NULL){
		struct BlockMetaData *nextmetadata=next;
		if(nextmetadata->is_free==0){
			currmetadata->is_free=1;
		}else{
			currmetadata->size+=nextmetadata->size;
			currmetadata->is_free=1;
			nextmetadata->size=0;
			nextmetadata->is_free=0;
			LIST_REMOVE(&MemoryData,nextmetadata);
			nextmetadata = 0;
		}
	}else if(next!=NULL&&prev!=NULL){
		        struct BlockMetaData *prevmetadata=prev;
				struct BlockMetaData *nextmetadata=next;
				    if(prevmetadata->is_free==0&&nextmetadata->is_free==0){
						currmetadata->is_free=1;
					}else if(prevmetadata->is_free==1&&nextmetadata->is_free==0){
						prevmetadata->size+=currmetadata->size;
						currmetadata->size= 0 ;
						currmetadata->is_free= 0;
						LIST_REMOVE(&MemoryData,currmetadata);
						currmetadata = 0;
					}else if(prevmetadata->is_free==0&&nextmetadata->is_free==1){
						currmetadata->size+=nextmetadata->size;
						currmetadata->is_free=1;
						nextmetadata->size= 0;
						nextmetadata->is_free= 0;
						LIST_REMOVE(&MemoryData,nextmetadata);
						nextmetadata = 0;
					}else if(prevmetadata->is_free==1&&nextmetadata->is_free==1){
						prevmetadata->size+=(nextmetadata->size+currmetadata->size);
						nextmetadata->size= 0 ;
						nextmetadata->is_free= 0;
						currmetadata->size= 0 ;
						currmetadata->is_free= 0;
						LIST_REMOVE(&MemoryData,currmetadata);
						LIST_REMOVE(&MemoryData,nextmetadata);
						currmetadata = 0, nextmetadata = 0;
					}
	}

}

//=========================================
// [4] REALLOCATE BLOCK BY FIRST FIT:
//=========================================
void *realloc_block_FF(void* va, uint32 new_size){
	//TODO: [PROJECT'23.MS1 - #8] [3] DYNAMIC ALLOCATOR - realloc_block_FF()
		if(va != NULL)
		{
			struct BlockMetaData *currMetaData = ((struct BlockMetaData *)va - 1);
			//(va, 0)
			if(new_size == 0)
			{
//				cprintf("free\n"); //done
				free_block(va);
				return NULL ;
			}

			if(currMetaData->size > new_size + sizeOfMetaData())
			{
//				cprintf("currMetaData->size > new_size\n"); //done
				int reqsize=new_size+sizeOfMetaData();
				currMetaData->is_free = 0;
				char* address=(char*)currMetaData+reqsize;
				struct BlockMetaData *metadata = (struct BlockMetaData *)address;
				if(currMetaData->size-reqsize > sizeOfMetaData())
				{
					metadata->size=currMetaData->size-reqsize;
					metadata->is_free=1;
					currMetaData->size=reqsize;
					LIST_INSERT_AFTER(&MemoryData,currMetaData,metadata);
					free_block(metadata + 1);//need to do this manually
				}
				char * returnedAdress=(char *)currMetaData+sizeOfMetaData();
				return returnedAdress;
			}
			if(currMetaData->size == new_size + sizeOfMetaData())
			{
//				cprintf("currMetaData->size == new_size + sizeOfMetaData() \n"); //done
				currMetaData->is_free = 0;
				char * returnedAdress=(char *)currMetaData+sizeOfMetaData();
				return returnedAdress;
			}

			if(currMetaData->prev_next_info.le_next != NULL)
			{
//				cprintf("currMetaData->prev_next_info.le_next != NULL \n"); //done
				struct BlockMetaData *nextMetadata = currMetaData->prev_next_info.le_next;
				if(nextMetadata->is_free == 1 && nextMetadata->size + currMetaData->size >= new_size + sizeOfMetaData() )
				{
					int remaining = nextMetadata->size + currMetaData->size - new_size - sizeOfMetaData() ;
//					cprintf("remaining %d \n" , remaining);
//					cprintf("nextMetadata->is_free == 1 && nextMetadata->size + currMetaData->size >= new_size \n");
					//nextMetadata->size = nextMetadata->size + currMetaData->size - new_size - sizeOfMetaData();

					currMetaData->is_free = 0 ;

					if(remaining > sizeOfMetaData())
					{
						currMetaData->size = new_size + sizeOfMetaData() ;
						char* address=(char*)currMetaData+new_size+sizeOfMetaData();
						struct BlockMetaData *metadata = (struct BlockMetaData *)address;
						metadata->size= remaining;
						metadata->is_free=1;
						nextMetadata->is_free = 0 ;
						nextMetadata->size = 0 ;
						LIST_REMOVE(&MemoryData,nextMetadata);
						LIST_INSERT_AFTER(&MemoryData,currMetaData,metadata);


						//free_block(nextMetadata + 1);//need to do this manually
					}else{
						currMetaData->size += nextMetadata->size ;
					}


					char * returnedAdress=(char *)currMetaData+sizeOfMetaData();
					return returnedAdress;
				}else
				{
//					cprintf("NOT nextMetadata->is_free == 1 && nextMetadata->size + currMetaData->size >= new_size \n");
					//currMetaData->is_free = 1 ;

//					cprintf("Free free_block(va)");
					void* va2 = alloc_block_FF(new_size) ;
					if(va2==NULL)
						return va;
					struct BlockMetaData *target2 = ((struct BlockMetaData *)va2 -1);
					target2->is_free = 0 ;

					uint32 endVa = (uint32) va2 + new_size ;
					short val = *(short*)(va);

					for(short *ptr = (short*) va2 ; ptr <= (short*)endVa ; ptr++)
					{

						*(ptr) = val ;
					}

					free_block(va);
					return va2 ;

				}
			}else
			{
//				cprintf("NOT currMetaData->prev_next_info.le_next != NULL \n");
				//currMetaData->is_free = 1 ;
				free_block(va);
				void* va2 = alloc_block_FF(new_size) ;
				struct BlockMetaData *target2 = ((struct BlockMetaData *)va2 -1);
				target2->is_free = 0 ;
				uint32 endVa = (uint32) va2 + new_size ;
				short val = *(short*)(va);
				for(short *ptr = (short*) va2 ; ptr <= (short*)endVa ; ptr++)
				{
					*(ptr) = val ;
				}

				return va2 ;

			}





		}else if (new_size != 0)	//(NULL, n)
		{
			//cprintf("new_size != 0 \n"); //done
			return alloc_block_FF(new_size);
		}else{	//(NULL, 0)
//			cprintf("(NULL, 0) \n"); //done
			return NULL ;
		}
	//	panic("realloc_block_FF is not implemented yet");
//		cprintf("The End !!!!!! \n");
		return NULL;
}

