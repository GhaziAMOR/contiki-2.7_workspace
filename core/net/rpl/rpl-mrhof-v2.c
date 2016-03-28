/**
 * \addtogroup uip6
 * @{
 */
/*
 * Copyright (c) 2010, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */
/**
 * \file
 *         The Minimum Rank with Hysteresis Objective Function (MRHOF)
 *
 *         This implementation uses the estimated number of 
 *         transmissions (ETX) as the additive routing metric,
 *         and also provides stubs for the energy metric.
 *
 * \author Joakim Eriksson <joakime@sics.se>, Nicolas Tsiftes <nvt@sics.se>
 */

#include "net/rpl/rpl-private.h"
#include "net/nbr-table.h"

#define DEBUG DEBUG_FULL
#include "net/uip-debug.h"

static void reset(rpl_dag_t *);
static rpl_parent_t *best_parent(rpl_parent_t *, rpl_parent_t *);
static rpl_dag_t *best_dag(rpl_dag_t *, rpl_dag_t *);
static rpl_rank_t calculate_rank(rpl_parent_t *, rpl_rank_t);
static void update_metric_container(rpl_instance_t *);

rpl_of_t rpl_mrhof = {
  reset,
  NULL,
  best_parent,
  best_dag,
  calculate_rank,
  update_metric_container,
  0
};



/* Reject parents that have a higher link metric than the following. */
#define MAX_LINK_METRIC			10

/* Reject parents that have a higher path cost than the following. */
#define MAX_PATH_COST			100

/*
 * The rank must differ more than 1/PARENT_SWITCH_THRESHOLD_DIV in order
 * to switch preferred parent.
 */
#define PARENT_SWITCH_THRESHOLD_DIV	2

typedef uint16_t rpl_path_metric_t;


static void
reset(rpl_dag_t *sag)
{
  PRINTF("RPL: Reset MRHOF\n");
}




static rpl_rank_t
calculate_rank(rpl_parent_t *p, rpl_rank_t base_rank)
{
	
	
	
rpl_rank_t increment;

if(base_rank == 0) 
{
	if(p == NULL) 
	{
      return INFINITE_RANK;
    }
    base_rank = p->rank;
}

increment = p != NULL ?
                p->dag->instance->min_hoprankinc :
                DEFAULT_RANK_INCREMENT;

  if((rpl_rank_t)(base_rank + increment) < base_rank) {
    PRINTF("RPL: OF0 rank %d incremented to infinite rank due to wrapping\n",
        base_rank);
    return INFINITE_RANK;
  }
  return base_rank + increment;
  /*rpl_rank_t new_rank;
  rpl_rank_t rank_increase;

  if(p == NULL) {
    if(base_rank == 0) {
      return INFINITE_RANK;
    }
    rank_increase = 1;
  } else {
    rank_increase = p->link_metric;
    if(base_rank == 0) {
      base_rank = p->rank;
    }
  }

  if(INFINITE_RANK - base_rank < rank_increase) {
    /* Reached the maximum rank. */
   // new_rank = INFINITE_RANK;
 // } else {
   /* Calculate the rank based on the new rank information from DIO or
      stored otherwise. */
   // new_rank = base_rank + rank_increase;
 // }

  //return new_rank;



}

static rpl_dag_t *
best_dag(rpl_dag_t *d1, rpl_dag_t *d2)
{
  if(d1->grounded != d2->grounded) {
    return d1->grounded ? d1 : d2;
  }

  if(d1->preference != d2->preference) {
    return d1->preference > d2->preference ? d1 : d2;
  }

  return d1->rank < d2->rank ? d1 : d2;
}





static rpl_parent_t *
best_parent(rpl_parent_t *p1, rpl_parent_t *p2)
{
  rpl_rank_t r1, r2;
  rpl_dag_t *dag;
  
  PRINTF("RPL: Comparing parent ");
  PRINT6ADDR(rpl_get_parent_ipaddr(p1));
  PRINTF(" (confidence %d, rank %d) with parent ",
        p1->link_metric, p1->rank);
  PRINT6ADDR(rpl_get_parent_ipaddr(p2));
  PRINTF(" (confidence %d, rank %d)\n",
        p2->link_metric, p2->rank);


  r1 = DAG_RANK(p1->rank, p1->dag->instance) * RPL_MIN_HOPRANKINC  +
         p1->link_metric;
  r2 = DAG_RANK(p2->rank, p1->dag->instance) * RPL_MIN_HOPRANKINC  +
         p2->link_metric;
  /* Compare two parents by looking both and their rank and at the ETX
     for that parent. We choose the parent that has the most
     favourable combination. */

  dag = (rpl_dag_t *)p1->dag; /* Both parents must be in the same DAG. */
  if(r1 < r2 + MIN_DIFFERENCE &&
     r1 > r2 - MIN_DIFFERENCE) {
    return dag->preferred_parent;
  } else if(r1 < r2) {
    return p1;
  } else {
    return p2;
  }
}

static rpl_path_metric_t calculate_path_metric(rpl_parent_t *p ) 
{
	
	if (p==NULL) return 0 ; 
	return (p->mc.obj.hopcount + 1 ) ;  
  
}


/*static rpl_path_metric_t
calculate_path_metric(rpl_parent_t *p)
{
  if(p == NULL) {
    return MAX_PATH_COST * RPL_DAG_MC_ETX_DIVISOR;
  }

#if RPL_DAG_MC == RPL_DAG_MC_NONE
  return p->rank + (uint16_t)p->link_metric;
#elif RPL_DAG_MC == RPL_DAG_MC_ETX
  return p->mc.obj.etx + (uint16_t)p->link_metric;
#elif RPL_DAG_MC == RPL_DAG_MC_ENERGY
  return p->mc.obj.energy.energy_est + (uint16_t)p->link_metric;
#else
#error "Unsupported RPL_DAG_MC configured. See rpl.h."
#endif /* RPL_DAG_MC */
//}

#if RPL_DAG_MC == RPL_DAG_MC_NONE
static void
update_metric_container(rpl_instance_t *instance)
{
  instance->mc.type = RPL_DAG_MC;
}
#else
static void
update_metric_container(rpl_instance_t *instance)
{
	
  rpl_path_metric_t path_metric;
  rpl_dag_t *dag;
//#if RPL_DAG_MC == RPL_DAG_MC_ENERGY
 // uint8_t type;
//#endif

  instance->mc.type = RPL_DAG_MC;
  instance->mc.flags = RPL_DAG_MC_FLAG_P;
  instance->mc.aggr = RPL_DAG_MC_AGGR_ADDITIVE;
  instance->mc.prec = 0;

  dag = instance->current_dag;

  if (!dag->joined) {
    PRINTF("RPL: Cannot update the metric container when not joined\n");
    return;
  }
//#define ROOT_RANK(instance)             (instance)->min_hoprankinc 
  if(dag->rank == ROOT_RANK(instance)) {
    path_metric = 0;
  } else {
    path_metric = calculate_path_metric(dag->preferred_parent);
  }

#if RPL_DAG_MC == RPL_DAG_MC_HOPCOUNT
  instance->mc.length = sizeof(instance->mc.obj.hopcount);
  instance->mc.obj.hopcount = path_metric;

  PRINTF("RPL: My path ETX to the root is %u.%u\n",
	
#endif /* RPL_DAG_MC == RPL_DAG_MC_HOPCOUNT */
}
#endif /* RPL_DAG_MC == RPL_DAG_MC_NONE */
