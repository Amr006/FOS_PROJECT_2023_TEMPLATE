#include "sched.h"

#include <inc/assert.h>

#include <kern/proc/user_environment.h>
#include <kern/trap/trap.h>
#include <kern/mem/kheap.h>
#include <kern/mem/memory_manager.h>
#include <kern/tests/utilities.h>
#include <kern/cmd/command_prompt.h>


uint32 isSchedMethodRR(){if(scheduler_method == SCH_RR) return 1; return 0;}
uint32 isSchedMethodMLFQ(){if(scheduler_method == SCH_MLFQ) return 1; return 0;}
uint32 isSchedMethodBSD(){if(scheduler_method == SCH_BSD) return 1; return 0;}

//===================================================================================//
//============================ SCHEDULER FUNCTIONS ==================================//
//===================================================================================//

//===================================
// [1] Default Scheduler Initializer:
//===================================
void sched_init()
{
	old_pf_counter = 0;

	sched_init_RR(INIT_QUANTUM_IN_MS);

	init_queue(&env_new_queue);
	init_queue(&env_exit_queue);
	scheduler_status = SCH_STOPPED;
}

//=========================
// [2] Main FOS Scheduler:
//=========================
void
fos_scheduler(void)
{
	//	cprintf("inside scheduler\n");

	chk1();
	scheduler_status = SCH_STARTED;

	//This variable should be set to the next environment to be run (if any)
	struct Env* next_env = NULL;

	if (scheduler_method == SCH_RR)
	{
		// Implement simple round-robin scheduling.
		// Pick next environment from the ready queue,
		// and switch to such environment if found.
		// It's OK to choose the previously running env if no other env
		// is runnable.

		//If the curenv is still exist, then insert it again in the ready queue
		if (curenv != NULL)
		{
			enqueue(&(env_ready_queues[0]), curenv);
		}

		//Pick the next environment from the ready queue
		next_env = dequeue(&(env_ready_queues[0]));

		//Reset the quantum
		//2017: Reset the value of CNT0 for the next clock interval
		kclock_set_quantum(quantums[0]);
		//uint16 cnt0 = kclock_read_cnt0_latch() ;
		//cprintf("CLOCK INTERRUPT AFTER RESET: Counter0 Value = %d\n", cnt0 );

	}
	else if (scheduler_method == SCH_MLFQ)
	{
		next_env = fos_scheduler_MLFQ();
	}
	else if (scheduler_method == SCH_BSD)
	{
		next_env = fos_scheduler_BSD();
	}
	//temporarily set the curenv by the next env JUST for checking the scheduler
	//Then: reset it again
	struct Env* old_curenv = curenv;
	curenv = next_env ;
	chk2(next_env) ;
	curenv = old_curenv;

	//sched_print_all();

	if(next_env != NULL)
	{
		//		cprintf("\nScheduler select program '%s' [%d]... counter = %d\n", next_env->prog_name, next_env->env_id, kclock_read_cnt0());
		//		cprintf("Q0 = %d, Q1 = %d, Q2 = %d, Q3 = %d\n", queue_size(&(env_ready_queues[0])), queue_size(&(env_ready_queues[1])), queue_size(&(env_ready_queues[2])), queue_size(&(env_ready_queues[3])));
		env_run(next_env);
	}
	else
	{
		/*2015*///No more envs... curenv doesn't exist any more! return back to command prompt
		curenv = NULL;
		//lcr3(K_PHYSICAL_ADDRESS(ptr_page_directory));
		lcr3(phys_page_directory);

		//cprintf("SP = %x\n", read_esp());

		scheduler_status = SCH_STOPPED;
		//cprintf("[sched] no envs - nothing more to do!\n");
		while (1)
			run_command_prompt(NULL);

	}
}

//=============================
// [3] Initialize RR Scheduler:
//=============================
void sched_init_RR(uint8 quantum)
{

	// Create 1 ready queue for the RR
	num_of_ready_queues = 1;
#if USE_KHEAP
	sched_delete_ready_queues();
	env_ready_queues = kmalloc(sizeof(struct Env_Queue));
	quantums = kmalloc(num_of_ready_queues * sizeof(uint8)) ;
#endif
	quantums[0] = quantum;
	kclock_set_quantum(quantums[0]);
	init_queue(&(env_ready_queues[0]));

	//=========================================
	//DON'T CHANGE THESE LINES=================
	scheduler_status = SCH_STOPPED;
	scheduler_method = SCH_RR;
	//=========================================
	//=========================================
}

//===============================
// [4] Initialize MLFQ Scheduler:
//===============================
void sched_init_MLFQ(uint8 numOfLevels, uint8 *quantumOfEachLevel)
{
#if USE_KHEAP
	//=========================================
	//DON'T CHANGE THESE LINES=================
	sched_delete_ready_queues();
	//=========================================
	//=========================================

	//=========================================
	//DON'T CHANGE THESE LINES=================
	scheduler_status = SCH_STOPPED;
	scheduler_method = SCH_MLFQ;
	//=========================================
	//=========================================
#endif
}

//===============================
// [5] Initialize BSD Scheduler:
//===============================
void sched_init_BSD(uint8 numOfLevels, uint8 quantum)
{

#if USE_KHEAP
	//TODO: [PROJECT'23.MS3 - #4] [2] BSD SCHEDULER - sched_init_BSD
	//Your code is here
	//Comment the following line
	//panic("Not implemented yet");


	num_of_ready_queues =numOfLevels;
	env_ready_queues = kmalloc(sizeof(struct Env_Queue)*numOfLevels);
	for(int i = 0 ; i <numOfLevels;i++)
	{
		init_queue(&(env_ready_queues[i]));
	}
	*quantums=quantum;
	//DON'T CHANGE THESE LINES=================
	scheduler_status = SCH_STOPPED;
	scheduler_method = SCH_BSD;
	//=========================================
	//=========================================
#endif

}


//=========================
// [6] MLFQ Scheduler:
//=========================
struct Env* fos_scheduler_MLFQ()
{
	panic("not implemented");
	return NULL;
}

//=========================
// [7] BSD Scheduler:
//=========================
struct Env* fos_scheduler_BSD() {
	struct Env* env = NULL;

	// Prioritize exiting environments if running
	if (curenv) {
		curenv->env_status=ENV_READY;
		enqueue(&env_ready_queues[curenv->priority], curenv);
		for (int level = num_of_ready_queues - 1; level >= 0; level--) {
			  if (env_ready_queues[level].size) {
			    env = dequeue(&env_ready_queues[level]);
			    if (env) {
			      // Randomly select a quantum to avoid deterministic scheduling
			    	kclock_set_quantum (*quantums);
			    	env->env_status=ENV_RUNNABLE;
			    	return env;
			    }
			  }
			}
	}else{
		sched_insert_exit(curenv);
		for (int level = num_of_ready_queues - 1; level >= 0; level--) {
			  if (env_ready_queues[level].size) {
			    env = dequeue(&env_ready_queues[level]);
			    if (env) {
			      // Randomly select a quantum to avoid deterministic scheduling
			    	kclock_set_quantum (*quantums);
			    	env->env_status=ENV_RUNNABLE;
			    	return env;
			    }
			  }
			}
	}


	// Iterate through ready queues in reverse priority order


	// Calculate load average with a different formula

	loadAvg=fix_int(0);

	return NULL;

}

//========================================
// [8] Clock Interrupt Handler
//	  (Automatically Called Every Quantum)
//========================================
void clock_interrupt_handler()
{
	//TODO: [PROJECT'23.MS3 - #5] [2] BSD SCHEDULER - Your code is here

	{
		// Calculate the total number of ready processes
		int total_ready_processes = 0;
		for (int i = 0; i < num_of_ready_queues; i++) {
		    total_ready_processes += env_ready_queues[i].size;
		}

		// Calculate ticks required for one second
		int nearest_quantum = ROUNDUP(1000, *quantums);
		int needed_ticks = nearest_quantum / *quantums;

		// Update metrics and priorities
		{
		    // Update load average periodically
		    if (timer_ticks() % needed_ticks == 0) {
		        fixed_point_t prev_load_avg = loadAvg;
		        loadAvg = fix_add(fix_mul(fix_int(59 / 60), prev_load_avg), fix_mul(fix_int(1 / 60), fix_int(total_ready_processes)));
		    }

		    // Update recent CPU for the running process on each timer tick
		    fixed_point_t prev_recent_cpu = curenv->recent_cpu;
		    curenv->recent_cpu = fix_mul(fix_div(fix_mul(loadAvg, fix_int(2)), fix_add(fix_mul(loadAvg, fix_int(2)), fix_int(1))), fix_add(prev_recent_cpu, fix_int(curenv->nice)));

		    // Update recent CPU for every process periodically
		    if (timer_ticks() % needed_ticks == 0) {
		        for (int i = 0; i < num_of_ready_queues; i++) {
		            struct Env *iterator = env_ready_queues[i].lh_first;
		            for (int j = 0; j < env_ready_queues[i].size; j++) {
		                fixed_point_t prev_recent_cpu_iter = iterator->recent_cpu;
		                iterator->recent_cpu = fix_mul(fix_div(fix_mul(loadAvg, fix_int(2)), fix_add(fix_mul(loadAvg, fix_int(2)), fix_int(1))), fix_add(prev_recent_cpu_iter, fix_int(iterator->nice)));
		                iterator = LIST_NEXT(iterator);
		            }
		        }
		    }

		    // Recalculate priority periodically
		    if (timer_ticks() % 4 == 0) {
		        // Change priority for the running process
		        int current_priority = fix_round(fix_sub(fix_sub(fix_int(num_of_ready_queues - 1), fix_div(curenv->recent_cpu, fix_int(4))), fix_mul(fix_int(curenv->nice), fix_int(2))));
		        curenv->priority = (current_priority >= PRI_MIN && current_priority <= num_of_ready_queues - 1) ? current_priority : ((current_priority < PRI_MIN) ? PRI_MIN : num_of_ready_queues - 1);

		        // Change priority for ready processes
		        struct Env_Queue process_queue;
		        init_queue(&process_queue);

		        for (int i = 0; i < num_of_ready_queues; i++) {
		            while (env_ready_queues[i].size != 0) {
		                struct Env *returned_env = dequeue(&env_ready_queues[i]);
		                int current_priority = fix_round(fix_sub(fix_sub(fix_int(num_of_ready_queues - 1), fix_div(returned_env->recent_cpu, fix_int(4))), fix_mul(fix_int(returned_env->nice), fix_int(2))));
		                returned_env->priority = (current_priority >= PRI_MIN && current_priority <= num_of_ready_queues - 1) ? current_priority : ((current_priority < PRI_MIN) ? PRI_MIN : num_of_ready_queues - 1);
		                enqueue(&process_queue, returned_env);
		            }
		        }

		        // Enqueue processes based on updated priority
		        while (process_queue.size != 0) {
		            struct Env *returned_env = dequeue(&process_queue);
		            enqueue(&env_ready_queues[returned_env->priority], returned_env);
		        }
		    }
		}

	}

	/********DON'T CHANGE THIS LINE***********/
	ticks++;
	if(isPageReplacmentAlgorithmLRU(PG_REP_LRU_TIME_APPROX))
	{
		update_WS_time_stamps();
	}
	//cprintf("Clock Handler\n") ;
	fos_scheduler();
	/*****************************************/
}

//===================================================================
// [9] Update LRU Timestamp of WS Elements
//	  (Automatically Called Every Quantum in case of LRU Time Approx)
//===================================================================
void update_WS_time_stamps()
{
	struct Env *curr_env_ptr = curenv;

	if(curr_env_ptr != NULL)
	{
		struct WorkingSetElement* wse ;
		{
			int i ;
#if USE_KHEAP
			LIST_FOREACH(wse, &(curr_env_ptr->page_WS_list))
			{
#else
			for (i = 0 ; i < (curr_env_ptr->page_WS_max_size); i++)
			{
				wse = &(curr_env_ptr->ptr_pageWorkingSet[i]);
				if( wse->empty == 1)
					continue;
#endif
				//update the time if the page was referenced
				uint32 page_va = wse->virtual_address ;
				uint32 perm = pt_get_page_permissions(curr_env_ptr->env_page_directory, page_va) ;
				uint32 oldTimeStamp = wse->time_stamp;

				if (perm & PERM_USED)
				{
					wse->time_stamp = (oldTimeStamp>>2) | 0x80000000;
					pt_set_page_permissions(curr_env_ptr->env_page_directory, page_va, 0 , PERM_USED) ;
				}
				else
				{
					wse->time_stamp = (oldTimeStamp>>2);
				}
			}
		}

		{
			int t ;
			for (t = 0 ; t < __TWS_MAX_SIZE; t++)
			{
				if( curr_env_ptr->__ptr_tws[t].empty != 1)
				{
					//update the time if the page was referenced
					uint32 table_va = curr_env_ptr->__ptr_tws[t].virtual_address;
					uint32 oldTimeStamp = curr_env_ptr->__ptr_tws[t].time_stamp;

					if (pd_is_table_used(curr_env_ptr->env_page_directory, table_va))
					{
						curr_env_ptr->__ptr_tws[t].time_stamp = (oldTimeStamp>>2) | 0x80000000;
						pd_set_table_unused(curr_env_ptr->env_page_directory, table_va);
					}
					else
					{
						curr_env_ptr->__ptr_tws[t].time_stamp = (oldTimeStamp>>2);
					}
				}
			}
		}
	}
}

