/**
 *
 * @author hungnv
 */
#ifndef FIFTYDEGREES_H
#define FIFTYDEGREES_H

#define FIFTYDEGREES_VERSION "0.1"
#define FIFTYONEDEGREESDB_VERSION "v3.2"
#include "deviceDetection/pattern/51Degrees.h"
#include "deviceDetection/trie/51Degrees.h"

#ifdef ZTS
#include "TSRM.h"
#endif
#pragma once 

//
//#ifdef ZTS
//#define FIFTYDEGREE_G(v) ZEND_TSRMG(fiftydegrees_globals_id, zend_fiftydegrees_globals *, v)
//#ifdef COMPILE_DL_FIFTYDEGREES
//ZEND_TSRMLS_CACHE_EXTERN()
//#endif
//#else
//#define FIFTYDEGREE_G(v) (fiftydegrees_globals.v)
//#endif


typedef struct {
    fiftyoneDegreesProvider provider;
    zval provider_obj_zval;
    zend_object std;
} ffdegreess_t;

typedef struct {
    fiftyoneDegreesWorkset *workset;
    zval provider_obj_zval;
    zend_object std;
} ffdegreess_workset_t;


extern zend_module_entry fiftydegrees_module_entry;
#define phpext_fiftydegrees_ptr &fiftydegrees_module_entry;


#if defined(ZTS) && defined(COMPILE_DL_FIFTYDEGREES)
ZEND_TSRMLS_CACHE_EXTERN();
#endif

#endif

