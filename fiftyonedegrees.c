/**
 *
 * @author hungnv
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_globals.h"
#include "ext/standard/info.h"
#include "ext/standard/basic_functions.h"
#include "fiftyonedegrees.h"
#include "zend_exceptions.h"
#include "zend_interfaces.h"

static zend_object_handlers fiftyonedegrees_obj_handlers;
static zend_object_handlers fiftyonedegreesMatch_obj_handlers;
static zend_class_entry *fiftyonedegrees_ce;
static zend_class_entry *fiftyonedegrees_workset_ce;

PHP_INI_BEGIN()
PHP_INI_ENTRY("FiftyOneDegreesPatternV3.data_file", "/data/51Degrees-LiteV3.2.dat",
        PHP_INI_ALL, NULL)
PHP_INI_ENTRY("FiftyOneDegreesPatternV3.property_list", "BrowserName,BrowserVersion,PlatformName,PlatformVersion,DeviceType,IsTablet,IsMobile,ScreenPixelsWidth,ScreenPixelsHeight",
        PHP_INI_ALL, NULL)
PHP_INI_ENTRY("FiftyOneDegreesPatternV3.cache_size", "10000",
        PHP_INI_ALL, NULL)
PHP_INI_ENTRY("FiftyOneDegreesPatternV3.pool_size", "25",
        PHP_INI_ALL, NULL)
PHP_INI_END()

static inline fiftyone_degrees_t* fiftyonedegrees_obj_fetch(zend_object* obj) {
    return (fiftyone_degrees_t*) ((char*) (obj) - XtOffsetOf(fiftyone_degrees_t, std));
}

static inline fiftyone_degrees_workset_t* fiftyonedegrees_workset_obj_fetch(zend_object* obj) {
    return (fiftyone_degrees_workset_t*) ((char*) (obj) - XtOffsetOf(fiftyone_degrees_workset_t, std));
}

#define Z_FIFTYDEGREES_P(zv) fiftyonedegrees_obj_fetch(Z_OBJ_P((zv)))
#define Z_FIFTYDEGREES_WORKSET_P(zv) fiftyonedegrees_workset_obj_fetch(Z_OBJ_P((zv)))
#define CALL_METHOD_BASE(classname, name) zim_##classname##_##name
#define CALL_METHOD(classname, name, retval)                  \
  CALL_METHOD_BASE(classname, name)(NULL, retval);

static inline zend_object* fiftyonedegrees_obj_new(zend_class_entry *ce) {
    fiftyone_degrees_t* ffdegrees;
    ffdegrees = ecalloc(1, sizeof (fiftyone_degrees_t) + zend_object_properties_size(ce));
    zend_object_std_init(&ffdegrees->std, ce);
    object_properties_init(&ffdegrees->std, ce);
    ffdegrees->std.handlers = &fiftyonedegrees_obj_handlers;
    return &ffdegrees->std;
}

static void fiftyonedegrees_obj_free(zend_object *object) {
    fiftyone_degrees_t* ffdegrees;
    ffdegrees = fiftyonedegrees_obj_fetch(object);
    if (!ffdegrees) {
        return;
    }
    /*
    if (&ffdegrees->provider != NULL) {
        if (ffdegrees->provider.activePool != NULL) {
            //            fiftyoneDegreesWorksetPoolCacheDataSetFree((fiftyoneDegreesWorksetPool*) ffdegrees->provider.activePool);
            //            fiftyoneDegreesProviderFree(&ffdegrees->provider);
        }
    }
    */ 
    if (!Z_ISNULL(ffdegrees->provider_obj_zval)) {
        zval_ptr_dtor(&ffdegrees->provider_obj_zval);
    }

    zend_object_std_dtor(&ffdegrees->std);
    efree(ffdegrees);
}

static inline zend_object* fiftyonedegreesMatch_obj_new(zend_class_entry *ce) {
    fiftyone_degrees_workset_t* ffdegrees_workset;
    ffdegrees_workset = ecalloc(1, sizeof (fiftyone_degrees_workset_t) + zend_object_properties_size(ce));
    zend_object_std_init(&ffdegrees_workset->std, ce);
    object_properties_init(&ffdegrees_workset->std, ce);
    ffdegrees_workset->std.handlers = &fiftyonedegreesMatch_obj_handlers;
    return &ffdegrees_workset->std;
}

static void fiftyonedegreesMatch_obj_free(zend_object *object) {
    fiftyone_degrees_workset_t* ffdegrees_workset;
    ffdegrees_workset = fiftyonedegrees_workset_obj_fetch(object);
    if (!ffdegrees_workset) {
        return;
    }
    /*
    if (ffdegrees_workset->workset) {
        fiftyoneDegreesWorksetRelease(ffdegrees_workset->workset);
        fiftyoneDegreesWorksetRelease(ffdegrees_workset->workset);
        //        ffdegrees_workset->workset = NULL;
    }

    if (ffdegrees_workset->user_agent) {
        efree(ffdegrees_workset->user_agent);
    }
    if (!Z_ISNULL(ffdegrees_workset->provider_obj_zval)) {
        zval_ptr_dtor(&ffdegrees_workset->provider_obj_zval);
    }
     */
    zend_object_std_dtor(&ffdegrees_workset->std);
    efree(ffdegrees_workset);
}

fiftyoneDegreesProvider provider;

ZEND_BEGIN_ARG_INFO_EX(arginfo_fiftyonedegrees_none, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fiftyonedegrees_get_match, 0, 0, 1)
ZEND_ARG_INFO(0, user_agent)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fiftyonedegrees_get_value, 0, 0, 1)
ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

typedef size_t strlen_t;

PHP_MINFO_FUNCTION(fiftyonedegrees) {
    php_info_print_table_start();
    php_info_print_table_header(2, "fiftyonedegrees support", "enabled");
    php_info_print_table_row(2, "fiftyonedegrees module version", FIFTYONEDEGREES_VERSION);
    php_info_print_table_row(2, "51degrees db version", FIFTYONEDEGREESDB_VERSION);
    php_info_print_table_row(2, "author", "hungnv");
    php_info_print_table_end();
    DISPLAY_INI_ENTRIES();
}

PHP_METHOD(fiftyonedegrees, __construct) {
    fiftyone_degrees_t* ffdegrees;
    return_value = getThis();
    ffdegrees = Z_FIFTYDEGREES_P(return_value);
    char* data_file = INI_STR("FiftyOneDegreesPatternV3.data_file");
    char *properties = INI_STR("FiftyOneDegreesPatternV3.property_list");
    int cacheSize = INI_INT("FiftyOneDegreesPatternV3.cache_size");
    int poolSize = INI_INT("FiftyOneDegreesPatternV3.pool_size");
    if (zend_parse_parameters_none() == FAILURE) {
        RETURN_FALSE;
    }
    if (provider.activePool == NULL) {
        fiftyoneDegreesDataSetInitStatus status = fiftyoneDegreesInitProviderWithPropertyString(
                data_file, &provider, properties, poolSize, cacheSize);
        if (status != DATA_SET_INIT_STATUS_SUCCESS) {
            RETURN_FALSE;
        }
    }
    ffdegrees->provider = provider;
    ZVAL_COPY(return_value, &ffdegrees->provider_obj_zval);
    //    RETURN_TRUE;
}

PHP_METHOD(fiftyonedegrees, get_match) {
    strlen_t len = 0;
    char* user_agent;
    fiftyone_degrees_t* ffdegrees;
    fiftyone_degrees_workset_t* ffdegrees_workset;
    zval *object = getThis();
    ffdegrees = Z_FIFTYDEGREES_P(object);
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &user_agent, &len) == FAILURE) {
        RETURN_FALSE;
    }
    if (!&ffdegrees->provider) {
        RETURN_FALSE;
    }
    object_init_ex(return_value, fiftyonedegrees_workset_ce);
    ffdegrees_workset = Z_FIFTYDEGREES_WORKSET_P(return_value);
    ffdegrees_workset->user_agent = strdup(user_agent);
    /*
    ZVAL_COPY(&ffdegrees_workset->provider_obj_zval, object);
    */ 
}

PHP_METHOD(fiftyonedegreesMatch, get_value) {
    strlen_t len = 0;
    char* property_name = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &property_name, &len) == FAILURE) {
        RETURN_FALSE;
    }
    zval *object = getThis();
    fiftyone_degrees_workset_t* ffdegrees_workset;
    ffdegrees_workset = Z_FIFTYDEGREES_WORKSET_P(object);
    if (!provider.activePool) {
        RETURN_FALSE;
    }
    fiftyoneDegreesWorkset* ws = fiftyoneDegreesProviderWorksetGet(&provider);
    if (!ws) {
        RETURN_FALSE;
    }
    fiftyoneDegreesMatch(ws, ffdegrees_workset->user_agent);
    int32_t requiredPropertyIndex = fiftyoneDegreesGetRequiredPropertyIndex(ws->dataSet, property_name);
    if (requiredPropertyIndex) {
        fiftyoneDegreesSetValues(ws, requiredPropertyIndex);
        const fiftyoneDegreesAsciiString* valueName = fiftyoneDegreesGetString(ws->dataSet, ws->values[0]->nameOffset);
        const char* value = &(valueName->firstByte);
        fiftyoneDegreesWorksetRelease(ws);
        ZVAL_STRING(return_value, (char*) value);
        return;
    }
    RETURN_FALSE;
}

zend_function_entry fiftyonedegrees_methods[] = {
    PHP_ME(fiftyonedegrees, __construct, arginfo_fiftyonedegrees_none, ZEND_ACC_CTOR | ZEND_ACC_PUBLIC)
    PHP_ME(fiftyonedegrees, get_match, arginfo_fiftyonedegrees_get_match, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

zend_function_entry fiftyonedegreesMatch_methods[] = {
    PHP_ME(fiftyonedegreesMatch, get_value, arginfo_fiftyonedegrees_get_value, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

PHP_MINIT_FUNCTION(fiftyonedegrees) {

    REGISTER_INI_ENTRIES();

    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "fiftyonedegrees", fiftyonedegrees_methods);
    fiftyonedegrees_ce = zend_register_internal_class(&ce);
    fiftyonedegrees_ce->create_object = fiftyonedegrees_obj_new;
    zend_declare_property_long(fiftyonedegrees_ce, ZEND_STRL("fd"), 0, ZEND_ACC_PUBLIC TSRMLS_CC);
    memcpy(&fiftyonedegrees_obj_handlers, zend_get_std_object_handlers(), sizeof (fiftyonedegrees_obj_handlers));
    fiftyonedegrees_obj_handlers.offset = XtOffsetOf(fiftyone_degrees_t, std);
    fiftyonedegrees_obj_handlers.free_obj = fiftyonedegrees_obj_free;

    INIT_CLASS_ENTRY(ce, "fiftyonedegreesMatch", fiftyonedegreesMatch_methods);
    fiftyonedegrees_workset_ce = zend_register_internal_class(&ce);
    fiftyonedegrees_workset_ce->create_object = fiftyonedegreesMatch_obj_new;
    zend_declare_property_long(fiftyonedegrees_workset_ce, ZEND_STRL("fd"), 0, ZEND_ACC_PUBLIC TSRMLS_CC);
    memcpy(&fiftyonedegreesMatch_obj_handlers, zend_get_std_object_handlers(), sizeof (fiftyonedegreesMatch_obj_handlers));
    fiftyonedegreesMatch_obj_handlers.offset = XtOffsetOf(fiftyone_degrees_workset_t, std);
    fiftyonedegreesMatch_obj_handlers.free_obj = fiftyonedegreesMatch_obj_free;
    return SUCCESS;

}

PHP_RINIT_FUNCTION(fiftyonedegrees) {
#if defined(COMPILE_DL_FIFTYDEGREES) && defined(ZTS)
    ZEND_TSRMLS_CACHE_UPDATE();
#endif
    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(fiftyonedegrees) {
    UNREGISTER_INI_ENTRIES();
    if (provider.activePool != NULL) {
        fiftyoneDegreesProviderFree(&provider);
    }
    return SUCCESS;
}

zend_module_entry fiftyonedegrees_module_entry = {
    STANDARD_MODULE_HEADER,
    "fiftyonedegrees",
    NULL,
    PHP_MINIT(fiftyonedegrees),
    PHP_MSHUTDOWN(fiftyonedegrees),
    PHP_RINIT(fiftyonedegrees),
    NULL,
    PHP_MINFO(fiftyonedegrees),
    FIFTYONEDEGREES_VERSION,
    STANDARD_MODULE_PROPERTIES
};

ZEND_GET_MODULE(fiftyonedegrees)