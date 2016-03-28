
#include "net/rpl/rpl-private.h"

#define DEBUG DEBUG_FULL
#include "net/uip-debug.h"

static void reset(rpl_dag_t *);
static rpl_parent_t *best_parent(rpl_parent_t *, rpl_parent_t *);
static rpl_dag_t *best_dag(rpl_dag_t *, rpl_dag_t *);
static rpl_rank_t calculate_rank(rpl_parent_t *, rpl_rank_t);
static void update_metric_container(rpl_instance_t *);

rpl_of_t rpl_of0 = {
  reset,
  NULL,
  best_parent,
  best_dag,
  calculate_rank,
  update_metric_container,
  0
};


#define DEFAULT_RANK_INCREMENT  10

#define MIN_DIFFERENCE (10 + 10 / 2)

static void
reset(rpl_dag_t *dag)
{
  PRINTF("RPL: Resetting OF0\n");
}


static rpl_rank_t calculate_rank(rpl_parent_t *p, rpl_rank_t base_rank)
{
	rpl_rank_t increment;

	if(p == NULL) 
	{
      return 0;
    }
    base_rank = p->rank;
  

  
  return base_rank + 1;

}



static rpl_dag_t * best_dag(rpl_dag_t *d1, rpl_dag_t *d2)
{
	if(d1->grounded) 
	{
		if (!d2->grounded) 
		{
			return d1;
		}
	} 
	else if(d2->grounded) 
	{
			return d2;
	}

	if(d1->preference < d2->preference) 
	{
    return d2;
	} 
	else 
	{
		if(d1->preference > d2->preference) 
		{
			return d1;
		}
	}

	if(d2->rank < d1->rank) 
	{
		return d2;
	} 
	else 
	{
		return d1;
	}
}

static rpl_parent_t *
best_parent(rpl_parent_t *p1, rpl_parent_t *p2)
{
	rpl_rank_t r1, r2;
	rpl_dag_t *dag;
  
	PRINTF("RPL: Comparing parent ");
	PRINT6ADDR(rpl_get_parent_ipaddr(p1));
	PRINTF(" (confidence %d, rank %d) with parent ", p1->link_metric, p1->rank);
	PRINT6ADDR(rpl_get_parent_ipaddr(p2));
	PRINTF(" (confidence %d, rank %d)\n", p2->link_metric, p2->rank);

	r1 = DAG_RANK(p1->rank, p1->dag->instance) * RPL_MIN_HOPRANKINC  + p1->link_metric;
	r2 = DAG_RANK(p2->rank, p1->dag->instance) * RPL_MIN_HOPRANKINC  + p2->link_metric;
  
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


static void update_metric_container(rpl_instance_t *instance)
	{
		instance->mc.type = RPL_DAG_MC_NONE;
	}
