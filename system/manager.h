#pragma once 

#include "helper.h"
#include "global.h"

class row_t;
class txn_man;
class LogPendingTable;
class workload;

class Manager {
public:
	void 			init();
	// returns the next timestamp.
	ts_t			get_ts(uint64_t thread_id);

	// For MVCC. To calculate the min active ts in the system
	void 			add_ts(uint64_t thd_id, ts_t ts);
	ts_t 			get_min_ts(uint64_t tid);
	ts_t 			get_min_ts() { return _min_ts; }
	uint64_t 		get_max_ts();

	// HACK! the following mutexes are used to model a centralized
	// lock/timestamp manager. 
 	void 			lock_row(row_t * row);
	void 			release_row(row_t * row);
	
	txn_man * 		get_txn_man(int thd_id) { return _all_txns[thd_id]; };
	void 			set_txn_man(txn_man * txn);
	
	uint64_t 		get_epoch() { return *_epoch; };
	void 	 		update_epoch();

	// For Logging
	//bool 			is_log_pending(uint64_t txn_id);
	void 			add_log_pending(uint64_t txn_id, uint64_t * predecessors, 
		uint32_t predecessor_size);
	void 			remove_log_pending(uint64_t txn_id);

    // per-thread random number generator
    void            init_rand(uint64_t thd_id) {  srand48_r(thd_id, &_buffer); }
    uint64_t        rand_uint64();
    double          rand_double();

    // thread id
    void            set_thd_id(uint64_t thread_id) { _thread_id = thread_id; }
    uint64_t        get_thd_id() { return _thread_id; }

	// workload 
    void            set_workload(workload * wl) { _workload = wl; }
    workload *      get_workload()  { return _workload; }

	// SILO Epochs
	void 			update_max_epoch(uint64_t epoch);
	uint64_t 		get_ready_epoch();
private:
	// for SILO
	volatile uint64_t * 			_epoch;		
	volatile uint64_t * 			_last_epoch_update_time;
	volatile uint64_t **			_max_epochs;


	pthread_mutex_t ts_mutex;
	uint64_t *		timestamp;
	pthread_mutex_t mutexes[BUCKET_CNT];
	uint64_t 		hash(row_t * row);
	ts_t volatile ** all_ts;
	txn_man ** 		_all_txns;
	// for MVCC 
	volatile ts_t	_last_min_ts_time;
	volatile uint64_t _min_ts;

    // per-thread random number
    static __thread drand48_data _buffer;

    // thread id
	static __thread uint64_t _thread_id;

	// workload 
	workload * _workload;
};
