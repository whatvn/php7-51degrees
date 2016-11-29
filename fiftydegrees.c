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
#include "fiftydegrees.h"

#include "zend_exceptions.h"
#include "zend_interfaces.h"

#include "deviceDetection/pattern/51Degrees.h"
#include "deviceDetection/trie/51Degrees.h"

static zend_object_handlers fiftydegrees_obj_handlers;
static zend_object_handlers fiftydegreesMatch_obj_handlers;
static zend_class_entry *fiftydegrees_ce;
static zend_class_entry *fiftydegrees_workset_ce;

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

static inline ffdegreess_t* fiftydegrees_obj_fetch(zend_object* obj) {
    return (ffdegreess_t*) ((char*) (obj) - XtOffsetOf(ffdegreess_t, std));
}

static inline ffdegreess_workset_t* fiftydegrees_workset_obj_fetch(zend_object* obj) {
    return (ffdegreess_workset_t*) ((char*) (obj) - XtOffsetOf(ffdegreess_workset_t, std));
}

#define Z_FIFTYDEGREES_P(zv) fiftydegrees_obj_fetch(Z_OBJ_P((zv)))
#define Z_FIFTYDEGREES_WORKSET_P(zv) fiftydegrees_workset_obj_fetch(Z_OBJ_P((zv)))

static inline zend_object* fiftydegrees_obj_new(zend_class_entry *ce) {
    ffdegreess_t* ffdegrees;
    ffdegrees = ecalloc(1, sizeof (ffdegreess_t) + zend_object_properties_size(ce));
    zend_object_std_init(&ffdegrees->std, ce);
    object_properties_init(&ffdegrees->std, ce);
    ffdegrees->std.handlers = &fiftydegrees_obj_handlers;
    return &ffdegrees->std;
}

static void fiftydegrees_obj_free(zend_object *object) {
    ffdegreess_t* ffdegrees;
    ffdegrees = fiftydegrees_obj_fetch(object);
    if (!ffdegrees) {
        return;
    }
    if (&ffdegrees->provider != NULL) {
        if (ffdegrees->provider.activePool != NULL) {
            //            fiftyoneDegreesWorksetPoolCacheDataSetFree((fiftyoneDegreesWorksetPool*) ffdegrees->provider.activePool);
            //            fiftyoneDegreesProviderFree(&ffdegrees->provider);
        }
    }
    if (!Z_ISNULL(ffdegrees->provider_obj_zval)) {
        zval_ptr_dtor(&ffdegrees->provider_obj_zval);
    }

    zend_object_std_dtor(&ffdegrees->std);
    efree(ffdegrees);
}

static inline zend_object* fiftydegreesMath_obj_new(zend_class_entry *ce) {
    ffdegreess_workset_t* ffdegrees_workset;
    ffdegrees_workset = ecalloc(1, sizeof (ffdegreess_workset_t) + zend_object_properties_size(ce));
    zend_object_std_init(&ffdegrees_workset->std, ce);
    object_properties_init(&ffdegrees_workset->std, ce);
    ffdegrees_workset->std.handlers = &fiftydegreesMatch_obj_handlers;
    return &ffdegrees_workset->std;
}

static void fiftydegreesMatch_obj_free(zend_object *object) {
    ffdegreess_workset_t* ffdegrees_workset;
    ffdegrees_workset = fiftydegrees_workset_obj_fetch(object);
    if (!ffdegrees_workset) {
        return;
    }
    if (ffdegrees_workset->workset) {
        fiftyoneDegreesWorksetRelease(ffdegrees_workset->workset);
        ffdegrees_workset->workset = NULL;
    }
    if (!Z_ISNULL(ffdegrees_workset->provider_obj_zval)) {
        zval_ptr_dtor(&ffdegrees_workset->provider_obj_zval);
    }
    zend_object_std_dtor(&ffdegrees_workset->std);
    efree(ffdegrees_workset);
}

fiftyoneDegreesProvider provider;

ZEND_BEGIN_ARG_INFO_EX(arginfo_fiftydegrees_none, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fiftydegrees_get_match, 0, 0, 1)
ZEND_ARG_INFO(0, user_agent)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fiftydegrees_get_value, 0, 0, 1)
ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

typedef size_t strlen_t;



PHP_MINFO_FUNCTION(fiftydegrees) {
    php_info_print_table_start();
    php_info_print_table_header(2, "fiftydegrees support", "enabled");
    php_info_print_table_row(2, "fiftydegrees module version", FIFTYDEGREES_VERSION);
    php_info_print_table_row(2, "51degrees db version", FIFTYONEDEGREESDB_VERSION);
    php_info_print_table_row(2, "author", "hungnv");
    php_info_print_table_end();
    DISPLAY_INI_ENTRIES();
}

PHP_METHOD(fiftydegrees, __construct) {
    ffdegreess_t* ffdegrees;
    return_value = getThis();
    ffdegrees = Z_FIFTYDEGREES_P(return_value);
    char* data_file = INI_STR("FiftyOneDegreesPatternV3.data_file");
    char *properties = INI_STR("FiftyOneDegreesPatternV3.property_list");
    int cacheSize = INI_INT("FiftyOneDegreesPatternV3.cache_size");
    int poolSize = INI_INT("FiftyOneDegreesPatternV3.pool_size");
    if (zend_parse_parameters_none() == FAILURE) {
        RETURN_FALSE;
    }
    fiftyoneDegreesDataSetInitStatus status = fiftyoneDegreesInitProviderWithPropertyString(
            data_file, &provider, properties, poolSize, cacheSize);
    if (status != DATA_SET_INIT_STATUS_SUCCESS) {
        RETURN_FALSE;
    }
    ffdegrees->provider = provider;
    php_error_docref(NULL, E_WARNING, ffdegrees->provider.activePool->dataSet->fileName);
    ZVAL_COPY(return_value, &ffdegrees->provider_obj_zval);

    RETURN_TRUE;
}

PHP_METHOD(fiftydegrees, get_match) {
    strlen_t len = 0;
    char *user_agent = NULL;
    ffdegreess_t* ffdegrees;
    ffdegreess_workset_t* ffdegrees_workset;
    zval *object = getThis();
    ffdegrees = Z_FIFTYDEGREES_P(object);
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &user_agent, &len) == FAILURE) {
        RETURN_FALSE;
    }
    if (!&ffdegrees->provider) {
        RETURN_FALSE;
    }
    fiftyoneDegreesWorkset *ws = NULL;
    ws = fiftyoneDegreesProviderWorksetGet(&ffdegrees->provider);
    fiftyoneDegreesMatch(ws, user_agent);
    if (ws == NULL) {
        RETURN_FALSE;
    }
    object_init_ex(return_value, fiftydegrees_workset_ce);
    ffdegrees_workset = Z_FIFTYDEGREES_WORKSET_P(return_value);
    ffdegrees_workset->workset = ws;
    ZVAL_COPY(&ffdegrees_workset->provider_obj_zval, object);
}

PHP_METHOD(fiftydegreesMatch, get_value) {
    strlen_t len = 0;
    char *property_name = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &property_name, &len) == FAILURE) {
        RETURN_FALSE;
    }
    zval *object = getThis();
    ffdegreess_workset_t* ffdegrees_workset;
    ffdegrees_workset = Z_FIFTYDEGREES_WORKSET_P(object);
    if (!ffdegrees_workset->workset) {
        RETURN_FALSE;
    }
    if (!ffdegrees_workset->workset->dataSet) {
        RETURN_FALSE;
    }
    int32_t requiredPropertyIndex = fiftyoneDegreesGetRequiredPropertyIndex(ffdegrees_workset->workset->dataSet, property_name);

    if (requiredPropertyIndex) {
        fiftyoneDegreesSetValues(ffdegrees_workset->workset, requiredPropertyIndex);
        const fiftyoneDegreesAsciiString* valueName = fiftyoneDegreesGetString(ffdegrees_workset->workset->dataSet, ffdegrees_workset->workset->values[0]->nameOffset);
        const char* value = &(valueName->firstByte);
        ZVAL_STRING(return_value, (char*)value);
        return;
    } else {
        RETURN_FALSE;
    }
}

zend_function_entry fiftydegrees_methods[] = {
    PHP_ME(fiftydegrees, __construct, arginfo_fiftydegrees_none, ZEND_ACC_CTOR | ZEND_ACC_PUBLIC)
    PHP_ME(fiftydegrees, get_match, arginfo_fiftydegrees_get_match, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

zend_function_entry fiftydegreesMatch_methods[] = {
    PHP_ME(fiftydegreesMatch, get_value, arginfo_fiftydegrees_get_value, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

PHP_MINIT_FUNCTION(fiftydegrees) {

    REGISTER_INI_ENTRIES();

    zend_class_entry ce;
    memcpy(&fiftydegreesMatch_obj_handlers, zend_get_std_object_handlers(), sizeof (zend_object_handlers));
    memcpy(&fiftydegrees_obj_handlers, zend_get_std_object_handlers(), sizeof (zend_object_handlers));

    INIT_CLASS_ENTRY(ce, "fiftydegrees", fiftydegrees_methods);
    fiftydegrees_ce = zend_register_internal_class(&ce);
    fiftydegrees_ce->create_object = fiftydegrees_obj_new;
    zend_declare_property_long(fiftydegrees_ce, ZEND_STRL("fd"), 0, ZEND_ACC_PUBLIC TSRMLS_CC);
    memcpy(&fiftydegrees_obj_handlers, zend_get_std_object_handlers(), sizeof (fiftydegrees_obj_handlers));
    fiftydegrees_obj_handlers.offset = XtOffsetOf(ffdegreess_t, std);
    fiftydegrees_obj_handlers.free_obj = fiftydegrees_obj_free;

    INIT_CLASS_ENTRY(ce, "fiftydegreesMatch", fiftydegreesMatch_methods);
    fiftydegrees_workset_ce = zend_register_internal_class(&ce);
    fiftydegrees_workset_ce->create_object = fiftydegreesMath_obj_new;
    zend_declare_property_long(fiftydegrees_workset_ce, ZEND_STRL("fd"), 0, ZEND_ACC_PUBLIC TSRMLS_CC);
    memcpy(&fiftydegreesMatch_obj_handlers, zend_get_std_object_handlers(), sizeof (fiftydegreesMatch_obj_handlers));
    fiftydegreesMatch_obj_handlers.offset = XtOffsetOf(ffdegreess_workset_t, std);
    fiftydegreesMatch_obj_handlers.free_obj = fiftydegreesMatch_obj_free;

    return SUCCESS;

}

PHP_MSHUTDOWN_FUNCTION(fiftydegrees) {
    UNREGISTER_INI_ENTRIES();
    return SUCCESS;
}

zend_module_entry fiftydegrees_module_entry = {
    STANDARD_MODULE_HEADER,
    "fiftydegrees",
    NULL,
    PHP_MINIT(fiftydegrees),
    PHP_MSHUTDOWN(fiftydegrees),
    PHP_RINIT(fiftydegrees),
    NULL,
    PHP_MINFO(fiftydegrees),
    FIFTYDEGREES_VERSION,
    STANDARD_MODULE_PROPERTIES
};

ZEND_GET_MODULE(fiftydegrees)