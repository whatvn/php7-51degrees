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
ZEND_DECLARE_MODULE_GLOBALS(fiftyonedegrees);

fiftyoneDegreesProvider provider;

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

#define Z_FIFTYONEDEGREES_P(zv) fiftyonedegrees_obj_fetch(Z_OBJ_P((zv)))
#define Z_FIFTYONEDEGREES_WORKSET_P(zv) fiftyonedegrees_workset_obj_fetch(Z_OBJ_P((zv)))

#ifdef ZTS
#include "TSRM.h"
#define FIFTYONEDEGREES_G(v) TSRMG(fiftyonedegrees_globals_id, zend_fiftyonedegrees_globals*, v)
#else
#define FIFTYONEDEGREES_G(v) (fiftyonedegrees_globals.v)
#endif

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

static void fiftyonedegreesMatch_obj_free(zend_object *object TSRMLS_CC) {
    printf("Destructing...\n");
    fiftyone_degrees_workset_t* ffdegrees_workset;
    ffdegrees_workset = fiftyonedegrees_workset_obj_fetch(object);
    if (!ffdegrees_workset) {
        return;
    }
    if (ffdegrees_workset->user_agent) {
        free(ffdegrees_workset->user_agent);
        ffdegrees_workset->user_agent = NULL;
    }

    if (!Z_ISNULL(ffdegrees_workset->workset_obj_zval)) {
        zval_ptr_dtor(&ffdegrees_workset->workset_obj_zval);
    }
    zend_object_std_dtor(&ffdegrees_workset->std);
    efree(ffdegrees_workset);
}


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
    if (zend_parse_parameters_none() == FAILURE) {
        RETURN_FALSE;
    }
    fiftyone_degrees_t* ffdegrees;
    ffdegrees = Z_FIFTYONEDEGREES_P(return_value);
}

PHP_METHOD(fiftyonedegrees, get_match) {
    strlen_t len = 0;
    char* user_agent;
    fiftyone_degrees_workset_t* ffdegrees_workset;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &user_agent, &len) == FAILURE) {
        RETURN_FALSE;
    }
    object_init_ex(return_value, fiftyonedegrees_workset_ce);
    ffdegrees_workset = Z_FIFTYONEDEGREES_WORKSET_P(return_value);
    ffdegrees_workset->user_agent = strdup(user_agent);
    zend_update_property_string(fiftyonedegrees_workset_ce, return_value, "user_agent", strlen("user_agent"), user_agent TSRMLS_CC);
    ZVAL_COPY(&ffdegrees_workset->workset_obj_zval, return_value);
}

PHP_METHOD(fiftyonedegreesMatch, get_value) {
    strlen_t len = 0;
    char* property_name = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &property_name, &len) == FAILURE) {
        RETURN_FALSE;
    }
    fiftyone_degrees_workset_t* ffdegrees_workset;
    ffdegrees_workset = Z_FIFTYONEDEGREES_WORKSET_P(getThis());
    fiftyoneDegreesProvider* provider = FIFTYONEDEGREES_G(provider);
    fiftyoneDegreesWorkset* ws = fiftyoneDegreesProviderWorksetGet(provider);
    fiftyoneDegreesMatch(ws, ffdegrees_workset->user_agent);
    int32_t requiredPropertyIndex = fiftyoneDegreesGetRequiredPropertyIndex(ws->dataSet, property_name);
    if (requiredPropertyIndex) {
        fiftyoneDegreesSetValues(ws, requiredPropertyIndex);
        const fiftyoneDegreesAsciiString* valueName = fiftyoneDegreesGetString(ws->dataSet, ws->values[0]->nameOffset);
        const char* value = &(valueName->firstByte);
        ZVAL_STRING(return_value, (char*) value);
        fiftyoneDegreesWorksetRelease(ws);
        ws = NULL;
    } else {
        return NULL;
    }
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

fiftyoneDegreesProvider* provider_init() {
    char* data_file = INI_STR("FiftyOneDegreesPatternV3.data_file");
    char *properties = INI_STR("FiftyOneDegreesPatternV3.property_list");
    int cacheSize = INI_INT("FiftyOneDegreesPatternV3.cache_size");
    int poolSize = INI_INT("FiftyOneDegreesPatternV3.pool_size");
    fiftyoneDegreesDataSetInitStatus status = fiftyoneDegreesInitProviderWithPropertyString(
            data_file, &provider, properties, poolSize, cacheSize);
    if (status != DATA_SET_INIT_STATUS_SUCCESS) {
        php_error_docref(NULL, E_CORE_ERROR, "Cannot initialize database provider, error_code: %d", status);
        return NULL;
    }
    return &provider;
}

static int fiftyonedegrees_init_globals(zend_fiftyonedegrees_globals *fiftyonedegrees_globals TSRMLS_CC) {
    if (fiftyonedegrees_globals->initialised == 1) {
        return SUCCESS;
    }
    fiftyonedegrees_globals->provider = provider_init();
    if (fiftyonedegrees_globals->provider == NULL) {
        php_error_docref(NULL, E_CORE_ERROR, "Cannot initialize database provider, provider is null");
        return FAILURE;
    }
    fiftyonedegrees_globals->initialised = 1;
    return SUCCESS;
}

static void fiftyonedegrees_destroy_globals(zend_fiftyonedegrees_globals *fiftyonedegrees_globals TSRMLS_CC) {
    if (!fiftyonedegrees_globals) {
        return;
    }
    fiftyonedegrees_globals->initialised = 0;
    fiftyoneDegreesProviderFree(fiftyonedegrees_globals->provider);
}

PHP_MINIT_FUNCTION(fiftyonedegrees) {

    REGISTER_INI_ENTRIES();

#ifdef ZTS
    ts_allocate_id(&fiftyonedegrees_globals_id,
            sizeof (zend_fiftyonedegrees_globals),
            (ts_allocate_ctor) fiftyonedegrees_init_globals,
            (ts_allocate_dtor) NULL);
    //            (ts_allocate_dtor) fiftyonedegrees_destroy_globals);
#else
    fiftyonedegrees_init_globals(&fiftyonedegrees_global TSRMLS_CC);
#endif

    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "fiftyonedegrees", fiftyonedegrees_methods);
    fiftyonedegrees_ce = zend_register_internal_class(&ce TSRMLS_CC);
    fiftyonedegrees_ce->create_object = fiftyonedegrees_obj_new;
    memcpy(&fiftyonedegrees_obj_handlers, zend_get_std_object_handlers(), sizeof (fiftyonedegrees_obj_handlers));
    fiftyonedegrees_obj_handlers.offset = XtOffsetOf(fiftyone_degrees_t, std);
    fiftyonedegrees_obj_handlers.free_obj = fiftyonedegrees_obj_free;

    INIT_CLASS_ENTRY(ce, "fiftyonedegreesMatch", fiftyonedegreesMatch_methods);
    fiftyonedegrees_workset_ce = zend_register_internal_class(&ce TSRMLS_CC);
    zend_declare_property_string(fiftyonedegrees_workset_ce, "user_agent", strlen("user_agent"), "", ZEND_ACC_PUBLIC TSRMLS_CC);
    fiftyonedegrees_workset_ce->create_object = fiftyonedegreesMatch_obj_new;
    memcpy(&fiftyonedegreesMatch_obj_handlers, zend_get_std_object_handlers(), sizeof (fiftyonedegreesMatch_obj_handlers));
    fiftyonedegreesMatch_obj_handlers.offset = XtOffsetOf(fiftyone_degrees_workset_t, std);
    fiftyonedegreesMatch_obj_handlers.dtor_obj = fiftyonedegreesMatch_obj_free;

    return SUCCESS;

}

PHP_RINIT_FUNCTION(fiftyonedegrees) {
#if defined(COMPILE_DL_FIFTYONEDEGREES) && defined(ZTS)
    ZEND_TSRMLS_CACHE_UPDATE();
#endif
    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(fiftyonedegrees) {
    UNREGISTER_INI_ENTRIES();
#ifndef ZTS
    fiftyonedegrees_destroy_globals(&fiftyonedegrees_globals TSRMLS_CC);
#endif
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