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

typedef struct {
    char* user_agent;
    zend_object std;
} fiftyone_degrees_t;

//typedef struct {
//    zval workset_obj_zval;
//    zend_object std;
//} fiftyone_degrees_workset_t;

ZEND_BEGIN_MODULE_GLOBALS(fiftyonedegrees)
fiftyoneDegreesProvider* provider;
int initialised;
ZEND_END_MODULE_GLOBALS(fiftyonedegrees)

extern ZEND_DECLARE_MODULE_GLOBALS(fiftyonedegrees);


extern zend_module_entry fiftyonedegrees_module_entry;
#define phpext_fiftyonedegrees_ptr &fiftyonedegrees_module_entry;


#if defined(ZTS) && defined(COMPILE_DL_FIFTYONEDEGREES)
ZEND_TSRMLS_CACHE_EXTERN();
#endif

#endif

