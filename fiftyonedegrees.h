/**
 *
 * @author hungnv
 */
#ifndef FIFTYONEDEGREES_H
#define FIFTYONEDEGREES_H

#define FIFTYONEDEGREES_VERSION "0.1"
#define FIFTYONEDEGREESDB_VERSION "v3.2"
#include "deviceDetection/pattern/51Degrees.h"

#ifdef ZTS
#include "TSRM.h"
#endif
#pragma once 

//
//#ifdef ZTS
//#define FIFTYDEGREE_G(v) ZEND_TSRMG(fiftyonedegrees_globals_id, zend_fiftyonedegrees_globals *, v)
//#ifdef COMPILE_DL_FIFTYDEGREES
//ZEND_TSRMLS_CACHE_EXTERN()
//#endif
//#else
//#define FIFTYDEGREE_G(v) (fiftyonedegrees_globals.v)
//#endif


typedef struct {
    fiftyoneDegreesProvider provider;
    zval provider_obj_zval;
    zend_object std;
} fiftyone_degrees_t;

typedef struct {
    char* user_agent;
//    zval provider_obj_zval;
    zend_object std;
} fiftyone_degrees_workset_t;


extern zend_module_entry fiftyonedegrees_module_entry;
#define phpext_fiftyonedegrees_ptr &fiftyonedegrees_module_entry;


#if defined(ZTS) && defined(COMPILE_DL_FIFTYDEGREES)
ZEND_TSRMLS_CACHE_EXTERN();
#endif

#endif

