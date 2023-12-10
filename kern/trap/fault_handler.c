/*
 * fault_handler.c
 *
 *  Created on: Oct 12, 2022
 *      Author: HP
 */

#include "trap.h"
#include <kern/proc/user_environment.h>
#include "../cpu/sched.h"
#include "../disk/pagefile_manager.h"
#include "../mem/memory_manager.h"
int x = 0;
//2014 Test Free(): Set it to bypass the PAGE FAULT on an instruction with this length and continue executing the next one
// 0 means don't bypass the PAGE FAULT
uint8 bypassInstrLength = 0;

//===============================
// REPLACEMENT STRATEGIES
//===============================
//2020
void setPageReplacmentAlgorithmLRU(int LRU_TYPE)
{
	assert(LRU_TYPE == PG_REP_LRU_TIME_APPROX || LRU_TYPE == PG_REP_LRU_LISTS_APPROX);
	_PageRepAlgoType = LRU_TYPE ;
}
void setPageReplacmentAlgorithmCLOCK(){_PageRepAlgoType = PG_REP_CLOCK;}
void setPageReplacmentAlgorithmFIFO(){_PageRepAlgoType = PG_REP_FIFO;}
void setPageReplacmentAlgorithmModifiedCLOCK(){_PageRepAlgoType = PG_REP_MODIFIEDCLOCK;}
/*2018*/ void setPageReplacmentAlgorithmDynamicLocal(){_PageRepAlgoType = PG_REP_DYNAMIC_LOCAL;}
/*2021*/ void setPageReplacmentAlgorithmNchanceCLOCK(int PageWSMaxSweeps){_PageRepAlgoType = PG_REP_NchanceCLOCK;  page_WS_max_sweeps = PageWSMaxSweeps;}

//2020
uint32 isPageReplacmentAlgorithmLRU(int LRU_TYPE){return _PageRepAlgoType == LRU_TYPE ? 1 : 0;}
uint32 isPageReplacmentAlgorithmCLOCK(){if(_PageRepAlgoType == PG_REP_CLOCK) return 1; return 0;}
uint32 isPageReplacmentAlgorithmFIFO(){if(_PageRepAlgoType == PG_REP_FIFO) return 1; return 0;}
uint32 isPageReplacmentAlgorithmModifiedCLOCK(){if(_PageRepAlgoType == PG_REP_MODIFIEDCLOCK) return 1; return 0;}
/*2018*/ uint32 isPageReplacmentAlgorithmDynamicLocal(){if(_PageRepAlgoType == PG_REP_DYNAMIC_LOCAL) return 1; return 0;}
/*2021*/ uint32 isPageReplacmentAlgorithmNchanceCLOCK(){if(_PageRepAlgoType == PG_REP_NchanceCLOCK) return 1; return 0;}

//===============================
// PAGE BUFFERING
//===============================
void enableModifiedBuffer(uint32 enableIt){_EnableModifiedBuffer = enableIt;}
uint8 isModifiedBufferEnabled(){  return _EnableModifiedBuffer ; }

void enableBuffering(uint32 enableIt){_EnableBuffering = enableIt;}
uint8 isBufferingEnabled(){  return _EnableBuffering ; }

void setModifiedBufferLength(uint32 length) { _ModifiedBufferLength = length;}
uint32 getModifiedBufferLength() { return _ModifiedBufferLength;}

//===============================
// FAULT HANDLERS
//===============================

//Handle the table fault
void table_fault_handler(struct Env * curenv, uint32 fault_va)
{
	//panic("table_fault_handler() is not implemented yet...!!");
	//Check if it's a stack page
	uint32* ptr_table;
#if USE_KHEAP
	{
		ptr_table = create_page_table(curenv->env_page_directory, (uint32)fault_va);
	}
#else
	{
		__static_cpt(curenv->env_page_directory, (uint32)fault_va, &ptr_table);
	}
#endif
}

//Handle the page fault

void page_fault_handler(struct Env * curenv, uint32 fault_va)
{
#if USE_KHEAP
		struct WorkingSetElement *victimWSElement = NULL;
		uint32 wsSize = LIST_SIZE(&(curenv->page_WS_list));
#else
		int iWS =curenv->page_last_WS_index;
		uint32 wsSize = env_page_ws_get_size(curenv);
#endif
	if(isPageReplacmentAlgorithmFIFO()){
		if(wsSize < (curenv->page_WS_max_size))
		{
			 //cprintf("PLACEMENT=========================WS Size = %d\n", wsSize );
				        //TODO: [PROJECT'23.MS2 - #15] [3] PAGE FAULT HANDLER - Placement
				        // Write your code here, remove the panic and write your code

				        //panic("page_fault_handler().PLACEMENT is not implemented yet...!!");
				        //refer to the project presentation and documentation for details

						//1)allocating the page then mapping
						struct FrameInfo *ptr=(void*)NULL;
					    int rete= allocate_frame((void*)&ptr);
					    if(rete!=E_NO_MEM)
					    {

					          map_frame(curenv->env_page_directory, (void*)ptr ,fault_va, PERM_USER|PERM_WRITEABLE);
					    }



				        int rd=pf_read_env_page(curenv, (void*)fault_va);

				        if(rd==E_PAGE_NOT_EXIST_IN_PF)
				        {
				            if(!((fault_va>=USTACKBOTTOM)&&(fault_va<USTACKTOP)) && !((fault_va>=USER_HEAP_START)&&(fault_va<USER_HEAP_MAX)))
				            {
				                sched_kill_env(curenv->env_id);
				            }
				        }

				            struct WorkingSetElement* rett=env_page_ws_list_create_element(curenv,fault_va);

				            struct WorkingSetElement* checkNull_Pointer = curenv->page_last_WS_element;

				            if (checkNull_Pointer != NULL){
				          	LIST_INSERT_BEFORE(&(curenv->page_WS_list), checkNull_Pointer, rett);
				            }


				            if (checkNull_Pointer == NULL){
				            LIST_INSERT_TAIL(&(curenv->page_WS_list),rett);

				            if(curenv->page_WS_max_size==curenv->page_WS_list.size){

				            		curenv->page_last_WS_element = curenv->page_WS_list.lh_first;
								}
								else{
									curenv-> page_last_WS_element=(void*)NULL;

							}

				            }

	}
		else
			{
				//cprintf("REPLACEMENT=========================WS Size = %d\n", wsSize );
				//refer to the project presentation and documentation for details
					//TODO: [PROJECT'23.MS3 - #1] [1] PAGE FAULT HANDLER - FIFO Replacement
					// Write your code here, remove the panic and write your code
		//			panic("page_fault_handler() FIFO Replacement is not implemented yet...!!")

					struct FrameInfo *ptr=(void*)NULL;
					int rete= allocate_frame((void*)&ptr);
					if(rete!=E_NO_MEM)
					{
						  map_frame(curenv->env_page_directory, (void*)ptr ,fault_va, PERM_USER|PERM_WRITEABLE|PERM_PRESENT);
					}
					int rd=pf_read_env_page(curenv, (void*)fault_va);

					struct WorkingSetElement *WS_element = curenv->page_last_WS_element;
					struct WorkingSetElement *element_New = env_page_ws_list_create_element(curenv, fault_va);
					struct WorkingSetElement* nexT_element = curenv->page_last_WS_element->prev_next_info.le_next;

					if(curenv->page_last_WS_element == LIST_LAST(&curenv->page_WS_list)){
						struct WorkingSetElement* brev_element = curenv->page_last_WS_element->prev_next_info.le_prev;
						LIST_REMOVE(&curenv->page_WS_list, WS_element);
						uint32 *ptr_page_table = NULL;
						struct FrameInfo * old_frame = get_frame_info(curenv->env_page_directory, WS_element->virtual_address,&ptr_page_table);
						uint32 perm = pt_get_page_permissions(curenv->env_page_directory,WS_element->virtual_address);
						if ((perm&PERM_MODIFIED)){
							int ret = pf_update_env_page(curenv, WS_element->virtual_address, old_frame);
						}
						pt_set_page_permissions(curenv->env_page_directory,WS_element->virtual_address,0, PERM_PRESENT);
						unmap_frame(curenv->env_page_directory, WS_element->virtual_address);
						LIST_INSERT_TAIL(&curenv->page_WS_list,element_New);
						curenv->page_last_WS_element = LIST_FIRST(&curenv->page_WS_list);
						pt_set_page_permissions(curenv->env_page_directory,fault_va,PERM_PRESENT,0);
						return;
					}

					LIST_REMOVE(&curenv->page_WS_list, WS_element);
					uint32 *ptr_page_table = NULL;
					struct FrameInfo * old_frame = get_frame_info(curenv->env_page_directory, WS_element->virtual_address,&ptr_page_table);
					uint32 perm = pt_get_page_permissions(curenv->env_page_directory,WS_element->virtual_address);
					pt_set_page_permissions(curenv->env_page_directory,WS_element->virtual_address,0, PERM_PRESENT);
					if ((perm&PERM_MODIFIED)){
						int ret = pf_update_env_page(curenv, WS_element->virtual_address, old_frame);
					}
					LIST_INSERT_BEFORE(&curenv->page_WS_list, nexT_element, element_New);
					curenv->page_last_WS_element = element_New->prev_next_info.le_next;
					unmap_frame(curenv->env_page_directory, WS_element->virtual_address);
					pt_set_page_permissions(curenv->env_page_directory,fault_va,PERM_PRESENT,0);
		}
	}

		if(isPageReplacmentAlgorithmLRU(PG_REP_LRU_LISTS_APPROX))
		{
			 //TODO: [PROJECT'23.MS3 - #2] [1] PAGE FAULT HANDLER - LRU Replacement
				  struct WorkingSetElement * elementt;
				  bool flagg=0;
				  LIST_FOREACH(elementt, & (curenv -> SecondList)) //bndwr 3aleh fel second
			 {
			       if (ROUNDDOWN(elementt->virtual_address,PAGE_SIZE) == ROUNDDOWN(fault_va,PAGE_SIZE))
			      {
			          flagg=1;
					  LIST_REMOVE( & (curenv -> SecondList), elementt); //remove tail of the secondlist
					  LIST_INSERT_HEAD( & (curenv -> ActiveList), elementt);
					  pt_set_page_permissions(curenv -> env_page_directory, elementt->virtual_address, PERM_PRESENT, 0);
					  struct WorkingSetElement * var=curenv->ActiveList.lh_last;
					  LIST_REMOVE(&(curenv -> ActiveList), var);
					  LIST_INSERT_HEAD( &(curenv -> SecondList), var);
					  pt_set_page_permissions(curenv -> env_page_directory, var->virtual_address, 0, PERM_PRESENT);
					  break;

			       }
			 }
				  if(flagg==0){
			      //1)allocating the page then mapping
				  struct FrameInfo * ptr = (void *) NULL;
				  int rete = allocate_frame((void *) & ptr);
				  if (rete != E_NO_MEM) {
					map_frame(curenv -> env_page_directory, ptr , fault_va, PERM_USER | PERM_WRITEABLE);
				  }
				  int rd = pf_read_env_page(curenv, (void *) fault_va);
				  if (rd == E_PAGE_NOT_EXIST_IN_PF) {
					if (!((fault_va >= USTACKBOTTOM) && (fault_va < USTACKTOP)) && !((fault_va >= USER_HEAP_START) && (fault_va < USER_HEAP_MAX))) {
					  sched_kill_env(curenv -> env_id);
					}
				  }

				//Scenario 1: Placement

				if ((curenv -> ActiveList.size + curenv -> SecondList.size) < (curenv -> page_WS_max_size)) //lw lesa fe makan sawa' fl ola aw l tnya
				{
					struct WorkingSetElement * elm = env_page_ws_list_create_element(curenv, fault_va);


				  if ((curenv -> ActiveList.size) < (curenv -> ActiveListSize)) //activelist is not full
				  {

				  LIST_INSERT_HEAD( & (curenv -> ActiveList), elm); //han insert fl head 3ady msh hnhtag n3ml haga tnya
				  }
				  else{
				  struct WorkingSetElement * temp = curenv -> ActiveList.lh_last;
				  LIST_REMOVE( & (curenv -> ActiveList), temp);
				  LIST_INSERT_HEAD( & (curenv -> SecondList), temp); //insert last element in list1 in list2
				  LIST_INSERT_HEAD( & (curenv -> ActiveList), elm);
				  pt_set_page_permissions(curenv -> env_page_directory, temp->virtual_address , 0, PERM_PRESENT); //updating the present bit by zero
				  }
				  }//activelist is full
				  //Scenario 1: Replacement
				else{
					 uint32 var2= curenv->SecondList.lh_last->virtual_address;
					uint32 page_permissions = pt_get_page_permissions(curenv -> env_page_directory, var2);
					 if (page_permissions & PERM_MODIFIED) //lw modified
					  {
						//update 3al disk
						uint32 * ptrr_page_table;
						struct FrameInfo * ptr_frame_info = get_frame_info(curenv -> env_page_directory, var2, & ptrr_page_table);
						pf_update_env_page(curenv, var2, (struct FrameInfo * ) ptr_frame_info);
			}
					 unmap_frame(curenv -> env_page_directory, var2); //unmapping
					 env_page_ws_invalidate(curenv,var2);
					 struct WorkingSetElement * temp = curenv -> ActiveList.lh_last;
					 struct WorkingSetElement * elm = env_page_ws_list_create_element(curenv, fault_va);

					 LIST_REMOVE( & (curenv -> ActiveList), temp);
					 LIST_INSERT_HEAD( & (curenv -> SecondList), temp); //insert last element in list1 in list2
					 LIST_INSERT_HEAD( & (curenv -> ActiveList), elm);
					 pt_set_page_permissions(curenv -> env_page_directory, temp->virtual_address , 0, PERM_PRESENT);



			}
			//TODO: [PROJECT'23.MS3 - BONUS] [1] PAGE FAULT HANDLER - O(1) implementation of LRU replacement
		}
	}
}

void __page_fault_handler_with_buffering(struct Env * curenv, uint32 fault_va)
{
	panic("this function is not required...!!");
}



