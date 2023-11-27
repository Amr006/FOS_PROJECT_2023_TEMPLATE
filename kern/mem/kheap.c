#include "kheap.h"

#include <inc/memlayout.h>
#include <inc/dynamic_allocator.h>
#include "memory_manager.h"


int initialize_kheap_dynamic_allocator(uint32 daStart, uint32 initSizeToAllocate, uint32 daLimit)
{
	//TODO: [PROJECT'23.MS2 - #01] [1] KERNEL HEAP - initialize_kheap_dynamic_allocator()
	//Initialize the dynamic allocator of kernel heap with the given start address, size & limit
	//All pages in the given range should be allocated
	//Remember: call the initialize_dynamic_allocator(..) to complete the initialization
	//Return:
	//	On success: 0
	//	Otherwise (if no memory OR initial size exceed the given limit): E_NO_MEM

	//Comment the following line(s) before start coding...
	//panic("not implemented yet");
	if (initSizeToAllocate > daLimit - daStart) {
	        return E_NO_MEM;
	}
	for(int i=0;i<initSizeToAllocate;i+=PAGE_SIZE){
		uint32 va = daStart+i;
		struct FrameInfo *ptr=NULL;
		int ret = allocate_frame(&ptr);
		if(ret==E_NO_MEM)return E_NO_MEM;
		ret = map_frame(ptr_page_directory,ptr,va,PERM_WRITEABLE);
	}
	kheap_start = daStart;
	kheap_segment_break = daStart + initSizeToAllocate;
	kheap_hard_limit = daLimit;

	initialize_dynamic_allocator(daStart,initSizeToAllocate);

	return 0;
}

void* sbrk(int increment)
{
	//TODO: [PROJECT'23.MS2 - #02] [1] KERNEL HEAP - sbrk()
	/* increment > 0: move the segment break of the kernel to increase the size of its heap,
	 * 				you should allocate pages and map them into the kernel virtual address space as necessary,
	 * 				and returns the address of the previous break (i.e. the beginning of newly mapped memory).
	 * increment = 0: just return the current position of the segment break
	 * increment < 0: move the segment break of the kernel to decrease the size of its heap,
	 * 				you should deallocate pages that no longer contain part of the heap as necessary.
	 * 				and returns the address of the new break (i.e. the end of the current heap space).
	 *
	 * NOTES:
	 * 	1) You should only have to allocate or deallocate pages if the segment break crosses a page boundary
	 * 	2) New segment break should be aligned on page-boundary to avoid "No Man's Land" problem
	 * 	3) Allocating additional pages for a kernel dynamic allocator will fail if the free frames are exhausted
	 * 		or the break exceed the limit of the dynamic allocator. If sbrk fails, kernel should panic(...)
	 */

	uint32 old_break = kheap_segment_break;
		if(increment == 0){
			return (void *)kheap_segment_break;
		}
		else if(increment > 0){


			uint32 current_break = kheap_segment_break;

			if(old_break%PAGE_SIZE == 0)
			{
				uint32 new_break = kheap_segment_break + ROUNDUP(increment,PAGE_SIZE);

				if(new_break > kheap_hard_limit){
					panic("no extra space in dynamic allocator");
				}
				else{

					int number_of_needed_pages = ROUNDUP(increment,PAGE_SIZE) / PAGE_SIZE;

					while(number_of_needed_pages != 0){

						struct FrameInfo * new_frame;
						int alloc_ret = allocate_frame(&new_frame);
						if(alloc_ret == E_NO_MEM){
							panic("no extra space in dynamic allocator");
						}
						int map_ret = map_frame(ptr_page_directory,new_frame,current_break,PERM_WRITEABLE);
						if(map_ret == E_NO_MEM){

							free_frame(new_frame);
							panic("no extra space in dynamic allocator");
						}
						new_frame->va = current_break;
						number_of_needed_pages--;
						current_break = current_break + PAGE_SIZE;
					}


					kheap_segment_break = new_break;
					return (void *)old_break;
				}
			}
			else
			{
				uint32 currentsegment_up =ROUNDUP(kheap_segment_break,PAGE_SIZE);
				uint32 segment_diff = currentsegment_up -kheap_segment_break;
				if(increment>segment_diff)
				{

					uint32 new_break =ROUNDUP((increment+kheap_segment_break),PAGE_SIZE);
					uint32 segment_diff = currentsegment_up -new_break;
					uint32 numofpages = segment_diff/PAGE_SIZE;

					if(new_break > kheap_hard_limit){
						panic("no extra space in dynamic allocator");
					}
					else{

							while(numofpages > 0){
								struct FrameInfo * new_frame;
								allocate_frame(&new_frame);
								map_frame(ptr_page_directory,new_frame,currentsegment_up,PERM_WRITEABLE);
								new_frame->va = currentsegment_up;
								numofpages--;
								currentsegment_up = currentsegment_up + PAGE_SIZE;
							}


							kheap_segment_break = new_break;
							return (void *)old_break;
						}
				}
				else
				{
					kheap_segment_break+=increment;
					return (void*)old_break;
				}




			}
		}
		else{

			uint32 size = increment * -1;

			if(kheap_segment_break%PAGE_SIZE==0)
			{

				if(size < PAGE_SIZE){
					kheap_segment_break = kheap_segment_break - size;
					return (void*)kheap_segment_break;
				}
				else{
					uint32 newsize = ROUNDDOWN(size,PAGE_SIZE);
					uint32 offset = size-newsize;
					uint32 numofpages = newsize/PAGE_SIZE;
					uint32 currentbreak = kheap_segment_break-PAGE_SIZE;
					while(numofpages>0)
					{
						unmap_frame(ptr_page_directory,currentbreak);
						numofpages--;
						currentbreak-=PAGE_SIZE;
					}
					kheap_segment_break-=size;
					return (void*)kheap_segment_break;
				}
			}
			else
			{
				uint32 diff = kheap_segment_break - ROUNDDOWN(kheap_segment_break,PAGE_SIZE);
				if(size>diff)
				{
					uint32 fregmentation = PAGE_SIZE-diff;
					uint32 currentbreak = kheap_segment_break+fregmentation;
					uint32 newsize = ROUNDDOWN((size + fregmentation),PAGE_SIZE);
					uint32 numofpages = newsize/PAGE_SIZE;
					currentbreak-=PAGE_SIZE;
					while(numofpages>0)
					{
						unmap_frame(ptr_page_directory,currentbreak);
						numofpages--;
						currentbreak-=PAGE_SIZE;
					}
					kheap_segment_break-=size;
					return (void*)kheap_segment_break;
				}
				else
				{
					kheap_segment_break-=size;
					return (void*)kheap_segment_break;
				}
			}

		}

	//MS2: COMMENT THIS LINE BEFORE START CODING====
//	return (void*)-1 ;
//	panic("not implemented yet");
//	uint32 rounded_Break = 0;
//	uint32 old_Break = kheap_segment_break;
//		if (increment > 0 ){
//			if (old_Break != kheap_start){
//				rounded_Break = ROUNDUP(kheap_segment_break, PAGE_SIZE);
//			}else{
//				rounded_Break = kheap_segment_break;
//			}
//			unsigned int SIZE = ROUNDUP(increment,PAGE_SIZE);
//			kheap_segment_break += SIZE;
//			uint32 new_Limit = rounded_Break + SIZE;
//				if(new_Limit <= kheap_hard_limit){
//				for (uint32 va = rounded_Break; va<=new_Limit;va+=PAGE_SIZE){
////					cprintf("SKAKR\t %d \n\n\n\n", va);
//					uint32 *ptr_page_table=NULL;
//					struct FrameInfo *frame=get_frame_info(ptr_page_directory,va,&ptr_page_table);
//					if(frame != NULL){
//						panic("THERE ARE NO FREE FRAMES");
//						return (void *)-1;
//					}else{
//						struct FrameInfo *ptr=NULL;
//						int ret = allocate_frame(&ptr);
//						ret = map_frame(ptr_page_directory,ptr,va,PERM_WRITEABLE);
//				}
//		}
////				cprintf("SKAKR2 \n\n\n\n\n");
//				return (void *)old_Break;
//
//				}else{
//					panic("BREAK EXCEEDED HARD LIMIT !");
//					return (void *)-1;
//				}
//	}else if(increment == 0){
//		return (void *)kheap_segment_break;
//	}else{
//		unsigned int SIZE = increment;
//		kheap_segment_break -= SIZE;
//		if(kheap_segment_break >= kheap_start){
//			if (increment % PAGE_SIZE == 0){
//			for(uint32 va = old_Break; va >= kheap_segment_break; va-= PAGE_SIZE){
//				uint32 *ptr_page_table=NULL;
//				struct FrameInfo *frame=get_frame_info(ptr_page_directory,va,&ptr_page_table);
//				free_frame(frame);
//				unmap_frame(ptr_page_directory, va);
//			}
//			}else{
//				if (increment / PAGE_SIZE > 0){
//					for(uint32 va = old_Break; va >= kheap_segment_break; va--){
//						if(va % PAGE_SIZE == 0){
//									uint32 *ptr_page_table=NULL;
//									struct FrameInfo *frame=get_frame_info(ptr_page_directory,va,&ptr_page_table);
//									free_frame(frame);
//									unmap_frame(ptr_page_directory, va);
//								}
//					}
//				}
//			}
//			return (void *)kheap_segment_break;
//		}else{
//			panic("BREAK GOT LESS THAN KERNEL_HEAP_START !");
//			return (void *)-1;
//		}
//	}
}
void* kmalloc(unsigned int size)
{
	//TODO: [PROJECT'23.MS2 - #03] [1] KERNEL HEAP - kmalloc()
	//refer to the project presentation and documentation for details
	// use "isKHeapPlacementStrategyFIRSTFIT() ..." functions to check the current strategy

	//change this "return" according to your answer
	//kpanic_into_prompt("kmalloc() is not implemented yet...!!");

	//cprintf("the size is %d and the SIZE is %d",size,SIZE);
	if(size <= DYN_ALLOC_MAX_BLOCK_SIZE){
		return (void*)alloc_block_FF(size);
	}

	unsigned int SIZE = ROUNDUP(size,PAGE_SIZE);
	int numOfFrames=SIZE/PAGE_SIZE;
	uint32 va= kheap_hard_limit+PAGE_SIZE;
	if(isKHeapPlacementStrategyFIRSTFIT()){
		int counter=0;
		for(uint32 address=va;address<KERNEL_HEAP_MAX;address+=PAGE_SIZE){
			uint32 *ptr_page_table=NULL;
			struct FrameInfo *frame=get_frame_info(ptr_page_directory,address,&ptr_page_table);
			if(frame==NULL){
				counter++;
			}else{
				counter=0;
			}
			if(counter==numOfFrames){
				if(address>=KERNEL_HEAP_MAX)
					return NULL;
				uint32 start_address = address-(numOfFrames-1)*PAGE_SIZE;
				int counter=0;
				for(uint32 add=start_address;add<=address;add+=PAGE_SIZE){
					struct FrameInfo *ptr=NULL;
					int ret = allocate_frame(&ptr);
					ret = map_frame(ptr_page_directory,ptr,add,PERM_WRITEABLE);
					ptr->numberOfFrames=numOfFrames;
				}
				return (void*)start_address;
			}
		}
	}


	return NULL;
}

void kfree(void* virtual_address)
{
	//TODO: [PROJECT'23.MS2 - #04] [1] KERNEL HEAP - kfree()
	//refer to the project presentation and documentation for details
	// Write your code here, remove the panic and write your code

	    if(((uint32)virtual_address>=kheap_start)&&((uint32)virtual_address<=kheap_segment_break)){
	    	free_block(virtual_address);
	    }
	    else if(((uint32)virtual_address >= (kheap_hard_limit + PAGE_SIZE)) && ((uint32)virtual_address <= KERNEL_HEAP_MAX)){
	    	uint32 *ptr_page_table=NULL;
	    	struct FrameInfo *frame=get_frame_info(ptr_page_directory,(uint32)virtual_address,&ptr_page_table);
	    	uint32 va = (uint32)virtual_address + PAGE_SIZE * frame->numberOfFrames;
	    	for(uint32 add=(uint32)virtual_address;add<va;add+=PAGE_SIZE){
	    		frame = get_frame_info(ptr_page_directory,add,&ptr_page_table);
	    		frame->va = 0 ;
	    		unmap_frame(ptr_page_directory,add);
	        }
	   }else{
		   panic("kfree() painc");
	   }

}

unsigned int kheap_virtual_address(unsigned int physical_address)
{
	//TODO: [PROJECT'23.MS2 - #05] [1] KERNEL HEAP - kheap_virtual_address()
	//refer to the project presentation and documentation for details
	// Write your code here, remove the panic and write your code
	//EFFICIENT IMPLEMENTATION ~O(1) IS REQUIRED ==================
	//change this "return" according to your answer
	uint32 offset = physical_address%PAGE_SIZE;
	struct FrameInfo * ptr_frame_info = NULL ;
	//	cprintf("physical_address %x\n",physical_address);
		ptr_frame_info = to_frame_info(ROUNDDOWN((uint32)physical_address,PAGE_SIZE));
	//	cprintf("virt %x\n",ptr_frame_info->va);

		if(ptr_frame_info != NULL)
		{
			if((unsigned int)ptr_frame_info->va == 0)
			{
				return 0 ;
			}
			return (unsigned int)ptr_frame_info->va + offset;
		}
		return 0;



}

unsigned int kheap_physical_address(unsigned int virtual_address)
{
	//TODO: [PROJECT'23.MS2 - #06] [1] KERNEL HEAP - kheap_physical_address()
	//refer to the project presentation and documentation for details
	// Write your code here, remove the panic and write your code
	//panic("kheap_physical_address() is not implemented yet...!!");

	//change this "return" according to your answer
	uint32 offset = virtual_address & 0x00000FFF;
	uint32 *ptr_page_table;
		struct FrameInfo * my_frame;
		my_frame = get_frame_info(ptr_page_directory,virtual_address,&ptr_page_table);
		if(my_frame == NULL){
			return 0;
		}
		else{
			return to_physical_address(my_frame) + offset;
		}

}


void kfreeall()
{
	panic("Not implemented!");

}

void kshrink(uint32 newSize)
{
	panic("Not implemented!");
}

void kexpand(uint32 newSize)
{
	panic("Not implemented!");
}




//=================================================================================//
//============================== BONUS FUNCTION ===================================//
//=================================================================================//
// krealloc():

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to kmalloc().
//	A call with new_size = zero is equivalent to kfree().

void *krealloc(void *virtual_address, uint32 new_size)
{
	//TODO: [PROJECT'23.MS2 - BONUS#1] [1] KERNEL HEAP - krealloc()
	// Write your code here, remove the panic and write your code
	return NULL;
	panic("krealloc() is not implemented yet...!!");
}
