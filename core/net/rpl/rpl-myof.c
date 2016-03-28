#include "net/rpl/rpl-private.h"
#include "lib/memb.h"
#include <string.h>
#define DEBUG DEBUG_FULL
#include "net/uip-debug.h"


static void reset(rpl_dag_t *);
static rpl_parent_t *best_parent(rpl_parent_t *, rpl_parent_t *);
static rpl_dag_t *best_dag(rpl_dag_t *, rpl_dag_t *);
static rpl_rank_t calculate_rank(rpl_parent_t *, rpl_rank_t);
static void update_metric_container(rpl_instance_t *);

typedef uint16_t rpl_path_metric_t;

rpl_of_t rpl_myof = {
  reset,
  NULL,
  best_parent,
  best_dag,
  calculate_rank,
  update_metric_container,
  0
};

#define DEFAULT_RANK_INCREMENT  RPL_MIN_HOPRANKINC

#define MIN_DIFFERENCE (RPL_MIN_HOPRANKINC + RPL_MIN_HOPRANKINC / 2)

static void reset(rpl_dag_t *dag)
{
  PRINTF("RPL: Resetting OF0\n");
}


static rpl_rank_t calculate_rank(rpl_parent_t *p, rpl_rank_t base_rank)
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

}


static rpl_dag_t *
best_dag(rpl_dag_t *d1, rpl_dag_t *d2)
{
  if(d1->grounded) {
    if (!d2->grounded) {
      return d1;
    }
  } else if(d2->grounded) {
    return d2;
  }

  if(d1->preference < d2->preference) {
    return d2;
  } else {
    if(d1->preference > d2->preference) {
      return d1;
    }
  }

  if(d2->rank < d1->rank) {
    return d2;
  } else {
    return d1;
  }
}


static rpl_parent_t * best_parent(rpl_parent_t *p1, rpl_parent_t *p2)
{
  rpl_rank_t r1, r2;
  rpl_dag_t *dag;
  /*_____________________________________________________ DEBUG ______________________________________________________________________*/

  /*
  PRINTF("RPL: Comparing parent ");
  PRINT6ADDR(rpl_get_parent_ipaddr(p1));
  PRINTF(" (p1->mc.obj.hopcount %d, rank %d) with parent ",p1->mc.obj.hopcount, p1->rank);
  PRINT6ADDR(rpl_get_parent_ipaddr(p2));
  PRINTF(" (p2->mc.obj.hopcount %d, rank %d)\n",p2->mc.obj.hopcount, p2->rank);
  */

  /*_____________________________________________________ DEBUG ______________________________________________________________________*/

  r1 = DAG_RANK(p1->rank, p1->dag->instance) * RPL_MIN_HOPRANKINC  + p1->mc.obj.hopcount;
  r2 = DAG_RANK(p2->rank, p1->dag->instance) * RPL_MIN_HOPRANKINC  + p2->mc.obj.hopcount;
  /* Compare two parents by looking both and their rank and at the ETX
     for that parent. We choose the parent that has the most
     favourable combination. */

  dag = (rpl_dag_t *)p1->dag; /* Both parents must be in the same DAG. */
  if(r1 < r2 + MIN_DIFFERENCE && r1 > r2 - MIN_DIFFERENCE)
  {
    return dag->preferred_parent;
  }
  else if(r1 < r2)
  {
  	return p1;
  }
  else
  {
    return p2;
  }
}


static int initialized = 0 ;

static void init_MC_HOPCOUNT(rpl_instance_t *instance)
{
	instance->mc.type = RPL_DAG_MC;
	instance->mc.flags = RPL_DAG_MC_FLAG_P;
	instance->mc.aggr = RPL_DAG_MC_AGGR_ADDITIVE;
	instance->mc.prec = 0;
	instance->mc.type = RPL_DAG_MC_HOPCOUNT;
	initialized = 1 ;

}



static rpl_path_metric_t calculate_path_metric(rpl_parent_t *p)
{
	rpl_path_metric_t path ;
	if (p==NULL)
	{
		return(0);
	}
	else
	{
		//path = p->mc.obj.hopcount + 1 ;
		//memcpy(&p->mc.obj.hopcount,&path,sizeof(p->mc.obj.hopcount));

		/*_____________________________________________________ DEBUG ______________________________________________________________________*/

/*
						PRINTF("calculate_path_metric : ");
						PRINT6ADDR(rpl_get_parent_ipaddr(p));
						PRINTF(" (p->mc.obj.hopcount %d, rank %d) \n",p->mc.obj.hopcount, p->rank);
*/
		/*_____________________________________________________ DEBUG ______________________________________________________________________*/


		return(p->mc.obj.hopcount + 1);
	}
}



static void update_metric_container(rpl_instance_t *instance)
{
	/* initialisations */
	rpl_path_metric_t path ;
	rpl_dag_t *dag;
	rpl_parent_t *p ;
	dag = instance->current_dag;
	p = dag->preferred_parent;

	if(!initialized) init_MC_HOPCOUNT(instance);
	  if (!dag->joined) {
	    PRINTF("RPL: Cannot update the metric container when not joined\n");
	    return;
	  }
	if (instance->current_dag->rank == ROOT_RANK(instance))
	{
		path = 0;
	}
	else
	{
		path = calculate_path_metric(dag->preferred_parent);
	}
	/***********************/

	memcpy(&instance->mc.obj.hopcount,&path,sizeof(instance->mc.obj.hopcount));




	/*_____________________________________________________ DEBUG ______________________________________________________________________*/


					PRINTF("update_metric_container ");
					PRINT6ADDR(rpl_get_parent_ipaddr(p));
					PRINTF(" (instance->current_dag->preferred_parent->mc.obj.hopcount %d, rank %d) \n",p->mc.obj.hopcount, p->rank);


	/*____________________________________________________ DEBUG ______________________________________________________________________*/



}
