/************************************************************
                        Course          :       CSC506
                        Source          :       MSI_up.cc
                        Instructor      :       Ed Gehringer
                        Email Id        :       efg@ncsu.edu
------------------------------------------------------------
        (c) Please do not replicate this code without consulting
                the owner & instructor! Thanks!
*************************************************************/
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
using namespace std;
#include "main.h"
#include "MSI_up.h"
#include "cache.h"
#include "io_function.h"

// MSI Processor Read
void MSI_up::PrRd(ulong addr, int processor_number) 
{
 cache_state state;
 current_cycle++;
 reads++;
 cache_line * line = find_line(addr);
  if (line == NULL) {
    cache_line* newline = allocate_line(addr);
    read_misses++;
    memory_transactions++;
    I2S++;
    bus_reads++;
    newline->set_state(S);
    sendBusRd(addr, processor_number);
  }
  else {
  	state=line->get_state();
    if (state == I)	{
    	cache_line *newline = allocate_line(addr);
    	read_misses++;
    	memory_transactions++;
    	bus_reads++;
    	I2S++;
    	newline->set_state(S);
      sendBusRd(addr, processor_number);
    }
    else if (state == M || state == S) {
    	update_LRU(line);
    }
  }
}
//MSI Processor Write
void MSI_up::PrWr(ulong addr, int processor_number) {
  cache_state state;
  current_cycle++;
  writes++;
  cache_line * line = find_line(addr);
  if (line == NULL || line -> get_state() == I) {
  	cache_line *newline = allocate_line(addr);
  	write_misses++;
  	memory_transactions++;
  	I2M++;
  	bus_readxs++;
  	newline->set_state(M);
  	sendBusRdX(addr, processor_number);
  }
  else {
  	state=line->get_state();
  	if (state == M){
  		update_LRU(line);
  	}
  	else if (state == S){
  	S2M++;
  	bus_upgrades++;
  	update_LRU(line);
  	line->set_state(M);
  	sendBusUpgr(addr, processor_number);
  	}
  }
}

cache_line* MSI_up::allocate_line(ulong addr) 
{
    ulong tag;
    cache_state state;

    cache_line *victim = find_line_to_replace(addr);
    assert(victim != 0);
    state = victim->get_state();
    if (writeback_needed(state))
    {
    	write_backs++;
      memory_transactions++;
    }
    tag = tag_field(addr);
    victim->set_tag(tag);
    victim->set_state(I);
    return victim;
}
// MSI Bus Read
void MSI_up::BusRd(ulong addr) {
  cache_state state;
  cache_line * line=find_line(addr);
  if (line!=NULL) {
  	state = line->get_state();
  	if (state == M) {
  		flushes++;
  	  write_backs++;
  	  memory_transactions++;
  	  interventions++;
  	  M2S++;
  	  line->set_state(S);
  	}
  }
}

// MSI Bus ReadX
void MSI_up::BusRdX(ulong addr) {
  cache_line * line=find_line(addr);
  if (line!=NULL) {
    cache_state state;
    state=line->get_state();
    if (state == S) {
    	invalidations++;
    	line->set_state(I);
    }
    else if (state == M)
    {
    	flushes++;
    	invalidations++;
    	write_backs++;
    	memory_transactions++;
    	line->set_state(I);
    }
  }
}
// MSI Bus Upgrade

void MSI_up::BusUpgr(ulong addr) 
{
	cache_line *line = find_line(addr);
	if (line!= NULL){
		cache_state state;
		state = line -> get_state();
		if (state == S) {
			invalidations ++;
			line -> set_state(I);
		}
	}
}

//If Writeback is Needed
bool MSI_up::writeback_needed(cache_state state) 
{
	if (state == M) {
		return true;
	}
	else {
		return false;
	}
}
