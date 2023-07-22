/************************************************************
                        Course          :       CSC506
                        Source          :       mesi.cc
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
#include "mesi.h"
#include "cache.h"

void MESI::PrRd(ulong addr, int processor_number) 
{
     cache_state state;
     current_cycle++; 
     reads++;
     cache_line * line = find_line(addr); 
	if(line == NULL || line->get_state()==I)
  	{ 
             read_misses++;
             cache_line *newline = allocate_line(addr);
		if((sharers_exclude(addr,processor_number)==0))
		  memory_transactions++;
		else
		 cache2cache++;
		if((sharers_exclude(addr,processor_number)==0))
		   {			
			
		   I2E++;
		   bus_reads++;
		   newline->set_state(E);			
		   }	
		else
		{
 		  I2S++; 
		  newline->set_state(S);
		  bus_reads++;		   
		}
              sendBusRd(addr, processor_number);
       }
	 else
	 {
           state=line->get_state();
	        if(state == M || state == E || state == S)
               { 
	         update_LRU(line);
               }
         }
}

void MESI::PrWr(ulong addr, int processor_number) 
{
    cache_state state;
    current_cycle++;
    writes++;
    cache_line * line = find_line(addr);
    if(line == NULL || line->get_state() == I)
      { 
	    write_misses++;
        cache_line *newline = allocate_line(addr); 		
	if((sharers_exclude(addr,processor_number)==0))
           memory_transactions++;            
	else
	   cache2cache++;
	I2M++;
        newline->set_state(M);
	bus_readxs++;
	sendBusRdX(addr, processor_number);
      }
    else 
	{
	  state=line->get_state();
	  if (state == M)
	    {
	       update_LRU(line);
            }
          
          else if (state == E)
            {
              E2M++;
              line->set_state(M);
              update_LRU(line);
             }
	   else if (state == S)
             {
	       S2M++;      
	       line->set_state(M);
               update_LRU(line);
               bus_upgrades++;
               sendBusUpgr(addr, processor_number);
              }
           
     }
 
}


void MESI::BusRd(ulong addr) 
{
cache_state state;    
  cache_line * line=find_line(addr);
    if (line!=NULL) 
       { 		
	state = line->get_state();
	  if (state == M)
            {
              flushes++;
	      interventions++;
		 memory_transactions++;
		 write_backs++;
	      line->set_state(S);
	      //M2O++;
            }
           
           else if (state == E)
            {
               line->set_state(S);
	        interventions++;
            }
	}
}

void MESI::BusRdX(ulong addr) 
{
cache_line * line=find_line(addr);
   if (line!=NULL)	
     { 
	cache_state state; 
        state = line->get_state();
	if (state == M)
          {
           invalidations++;
	       flushes++;
		   write_backs++;
		   memory_transactions++;
	       line->set_state(I);
	  }
       
         else if(state == E)
          {
            line->set_state(I);
            invalidations++;
          }
        else if(state == S)
          {
            line->set_state(I);
            invalidations++;
          }
       
      } 
}


void MESI::BusUpgr(ulong addr) 
{
cache_line *line = find_line(addr);
    if(line!=NULL)
    {
      cache_state state;
      state = line->get_state();
      if( state == S)
       {
        line->set_state(I);
        invalidations++;
       }
     }
}



bool MESI::writeback_needed(cache_state state) 
{

	 if(state == M)
	//edit this function to return the correct boolean value
	   return true;
          else
           return false;
}

cache_line * MESI::allocate_line_dir(ulong addr) {
	return NULL;
}

/**********Directory Functions*****************/
void MESI::PrRdDir(ulong addr, int processor_number) {

}

void MESI::PrWrDir(ulong addr, int processor_number) {
}

void MESI::signalRd(ulong addr, int processor_number){

	
}


void MESI::signalRdX(ulong addr, int processor_number){
	
}

void MESI::signalUpgr(ulong addr, int processor_number){

}

void MESI::Int(ulong addr) {
	
}


void MESI::Inv(ulong addr) {
	
}
