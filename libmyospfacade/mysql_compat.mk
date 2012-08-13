
# extra defines for mysql compatibility (to allow use of the mysql console with osp)

# This is temporary, we need to settle how we are going to do the final build across all platforms
# Currently, you must have the mysql 5.0 and 5.1 source installed and compiled to build myosp

MYSQL50_SRC_PATH=/usr/local/mysql
MYSQL51_SRC_PATH=/usr/local/mysql

###############
#  MYSQL 5.0  #
###############

# mystrings lib
MYSTRINGS50_PATH=$(MYSQL50_SRC_PATH)/libmysql/.libs
MYSTRINGS50_OBJ=$(MYSTRINGS50_PATH)/bchange.o \
	$(MYSTRINGS50_PATH)/bcmp.o \
	$(MYSTRINGS50_PATH)/bmove.o \
	$(MYSTRINGS50_PATH)/bmove_upp.o \
	$(MYSTRINGS50_PATH)/ctype-big5.o \
	$(MYSTRINGS50_PATH)/ctype-bin.o \
	$(MYSTRINGS50_PATH)/ctype.o \
	$(MYSTRINGS50_PATH)/ctype-cp932.o \
	$(MYSTRINGS50_PATH)/ctype-czech.o \
	$(MYSTRINGS50_PATH)/ctype-eucjpms.o \
	$(MYSTRINGS50_PATH)/ctype-euc_kr.o \
	$(MYSTRINGS50_PATH)/ctype-extra.o \
	$(MYSTRINGS50_PATH)/ctype-gb2312.o \
	$(MYSTRINGS50_PATH)/ctype-gbk.o \
	$(MYSTRINGS50_PATH)/ctype-latin1.o \
	$(MYSTRINGS50_PATH)/ctype-mb.o \
	$(MYSTRINGS50_PATH)/ctype-simple.o \
	$(MYSTRINGS50_PATH)/ctype-sjis.o \
	$(MYSTRINGS50_PATH)/ctype-tis620.o \
	$(MYSTRINGS50_PATH)/ctype-uca.o \
	$(MYSTRINGS50_PATH)/ctype-ucs2.o \
	$(MYSTRINGS50_PATH)/ctype-ujis.o \
	$(MYSTRINGS50_PATH)/ctype-utf8.o \
	$(MYSTRINGS50_PATH)/ctype-win1250ch.o \
	$(MYSTRINGS50_PATH)/int2str.o \
	$(MYSTRINGS50_PATH)/is_prefix.o \
	$(MYSTRINGS50_PATH)/llstr.o \
	$(MYSTRINGS50_PATH)/longlong2str.o \
	$(MYSTRINGS50_PATH)/my_strtoll10.o \
	$(MYSTRINGS50_PATH)/my_vsnprintf.o \
	$(MYSTRINGS50_PATH)/str2int.o \
	$(MYSTRINGS50_PATH)/str_alloc.o \
	$(MYSTRINGS50_PATH)/strcend.o \
	$(MYSTRINGS50_PATH)/strcont.o \
	$(MYSTRINGS50_PATH)/strend.o \
	$(MYSTRINGS50_PATH)/strfill.o \
	$(MYSTRINGS50_PATH)/strinstr.o \
	$(MYSTRINGS50_PATH)/strmake.o \
	$(MYSTRINGS50_PATH)/strmov.o \
	$(MYSTRINGS50_PATH)/strnlen.o \
	$(MYSTRINGS50_PATH)/strnmov.o \
	$(MYSTRINGS50_PATH)/strtod.o \
	$(MYSTRINGS50_PATH)/strtoll.o \
	$(MYSTRINGS50_PATH)/strtoull.o \
	$(MYSTRINGS50_PATH)/strxmov.o \
	$(MYSTRINGS50_PATH)/strxnmov.o \
	$(MYSTRINGS50_PATH)/xml.o 


# mysys lib
MYSYS50_PATH=$(MYSQL50_SRC_PATH)/libmysql/.libs
MYSYS50_OBJ= $(MYSYS50_PATH)/array.o \
	$(MYSYS50_PATH)/charset.o \
	$(MYSYS50_PATH)/charset-def.o \
	$(MYSYS50_PATH)/default.o \
	$(MYSYS50_PATH)/default_modify.o \
	$(MYSYS50_PATH)/errors.o \
	$(MYSYS50_PATH)/hash.o \
	$(MYSYS50_PATH)/list.o \
	$(MYSYS50_PATH)/md5.o \
    $(MYSYS50_PATH)/mf_arr_appstr.o \
	$(MYSYS50_PATH)/mf_cache.o \
	$(MYSYS50_PATH)/mf_dirname.o \
	$(MYSYS50_PATH)/mf_fn_ext.o \
	$(MYSYS50_PATH)/mf_format.o \
	$(MYSYS50_PATH)/mf_iocache2.o \
	$(MYSYS50_PATH)/mf_iocache.o \
	$(MYSYS50_PATH)/mf_loadpath.o \
	$(MYSYS50_PATH)/mf_pack.o \
	$(MYSYS50_PATH)/mf_path.o \
    $(MYSYS50_PATH)/mf_qsort.o \
	$(MYSYS50_PATH)/mf_tempfile.o \
	$(MYSYS50_PATH)/mf_unixpath.o \
	$(MYSYS50_PATH)/mf_wcomp.o \
	$(MYSYS50_PATH)/mulalloc.o \
	$(MYSYS50_PATH)/my_alloc.o \
	$(MYSYS50_PATH)/my_chsize.o \
	$(MYSYS50_PATH)/my_compress.o \
	$(MYSYS50_PATH)/my_create.o \
	$(MYSYS50_PATH)/my_delete.o \
	$(MYSYS50_PATH)/my_div.o \
	$(MYSYS50_PATH)/my_error.o \
	$(MYSYS50_PATH)/my_file.o \
	$(MYSYS50_PATH)/my_fopen.o \
	$(MYSYS50_PATH)/my_fstream.o \
	$(MYSYS50_PATH)/my_getopt.o \
	$(MYSYS50_PATH)/my_getwd.o \
	$(MYSYS50_PATH)/my_init.o \
	$(MYSYS50_PATH)/my_lib.o \
	$(MYSYS50_PATH)/my_malloc.o \
	$(MYSYS50_PATH)/my_messnc.o \
	$(MYSYS50_PATH)/my_net.o \
	$(MYSYS50_PATH)/my_once.o \
	$(MYSYS50_PATH)/my_open.o \
	$(MYSYS50_PATH)/my_port.o \
	$(MYSYS50_PATH)/my_pread.o \
	$(MYSYS50_PATH)/my_pthread.o \
	$(MYSYS50_PATH)/my_read.o \
	$(MYSYS50_PATH)/my_realloc.o \
	$(MYSYS50_PATH)/my_rename.o \
	$(MYSYS50_PATH)/my_seek.o \
	$(MYSYS50_PATH)/my_sleep.o \
	$(MYSYS50_PATH)/my_static.o \
	$(MYSYS50_PATH)/my_symlink.o \
	$(MYSYS50_PATH)/my_thr_init.o \
	$(MYSYS50_PATH)/my_write.o \
	$(MYSYS50_PATH)/safemalloc.o \
	$(MYSYS50_PATH)/sha1.o \
	$(MYSYS50_PATH)/string.o \
	$(MYSYS50_PATH)/thr_mutex.o \
	$(MYSYS50_PATH)/typelib.o


# common
MYCMN50_PATH=$(MYSQL50_SRC_PATH)/libmysql/.libs
MYCMN50_OBJ=$(MYCMN50_PATH)/net.o $(MYCMN50_PATH)/get_password.o $(MYCMN50_PATH)/pack.o $(MYCMN50_PATH)/my_time.o

# more dependencies found in testing
MYDEP50_PATH=$(MYSQL50_SRC_PATH)/libmysql/.libs
MYDEP50_OBJ=$(MYDEP50_PATH)/vio.o $(MYDEP50_PATH)/viosocket.o  $(MYDEP50_PATH)/viosslfactories.o

MYSQL50_OBJ=$(MYSTRINGS50_OBJ) $(MYSYS50_OBJ) $(MYCMN50_OBJ) $(MYDEP50_OBJ)




###############
#  MYSQL 5.1  #
###############

# mystrings lib
MYSTRINGS51_PATH=$(MYSQL51_SRC_PATH)/libmysql/.libs
MYSTRINGS51_OBJ=$(MYSTRINGS51_PATH)/bchange.o \
	$(MYSTRINGS51_PATH)/bmove.o \
	$(MYSTRINGS51_PATH)/bmove_upp.o \
	$(MYSTRINGS51_PATH)/ctype-big5.o \
	$(MYSTRINGS51_PATH)/ctype-bin.o \
	$(MYSTRINGS51_PATH)/ctype.o \
	$(MYSTRINGS51_PATH)/ctype-cp932.o \
	$(MYSTRINGS51_PATH)/ctype-czech.o \
	$(MYSTRINGS51_PATH)/ctype-eucjpms.o \
	$(MYSTRINGS51_PATH)/ctype-euc_kr.o \
	$(MYSTRINGS51_PATH)/ctype-extra.o \
	$(MYSTRINGS51_PATH)/ctype-gb2312.o \
	$(MYSTRINGS51_PATH)/ctype-gbk.o \
	$(MYSTRINGS51_PATH)/ctype-latin1.o \
	$(MYSTRINGS51_PATH)/ctype-mb.o \
	$(MYSTRINGS51_PATH)/ctype-simple.o \
	$(MYSTRINGS51_PATH)/ctype-sjis.o \
	$(MYSTRINGS51_PATH)/ctype-tis620.o \
	$(MYSTRINGS51_PATH)/ctype-uca.o \
	$(MYSTRINGS51_PATH)/ctype-ucs2.o \
	$(MYSTRINGS51_PATH)/ctype-ujis.o \
	$(MYSTRINGS51_PATH)/ctype-utf8.o \
	$(MYSTRINGS51_PATH)/ctype-win1250ch.o \
	$(MYSTRINGS51_PATH)/int2str.o \
	$(MYSTRINGS51_PATH)/is_prefix.o \
	$(MYSTRINGS51_PATH)/llstr.o \
	$(MYSTRINGS51_PATH)/longlong2str.o \
	$(MYSTRINGS51_PATH)/my_strtoll10.o \
	$(MYSTRINGS51_PATH)/my_vsnprintf.o \
	$(MYSTRINGS51_PATH)/str2int.o \
	$(MYSTRINGS51_PATH)/str_alloc.o \
	$(MYSTRINGS51_PATH)/strcend.o \
	$(MYSTRINGS51_PATH)/strcont.o \
	$(MYSTRINGS51_PATH)/strend.o \
	$(MYSTRINGS51_PATH)/strfill.o \
	$(MYSTRINGS51_PATH)/strinstr.o \
	$(MYSTRINGS51_PATH)/strmake.o \
	$(MYSTRINGS51_PATH)/strmov.o \
	$(MYSTRINGS51_PATH)/strnlen.o \
	$(MYSTRINGS51_PATH)/strnmov.o \
	$(MYSTRINGS51_PATH)/strtod.o \
	$(MYSTRINGS51_PATH)/strtoll.o \
	$(MYSTRINGS51_PATH)/strtoull.o \
	$(MYSTRINGS51_PATH)/strxmov.o \
	$(MYSTRINGS51_PATH)/strxnmov.o \
	$(MYSTRINGS51_PATH)/xml.o 


# mysys lib
MYSYS51_PATH=$(MYSQL51_SRC_PATH)/libmysql/.libs
MYSYS51_OBJ=$(MYSYS51_PATH)/array.o \
	$(MYSYS51_PATH)/charset.o \
	$(MYSYS51_PATH)/charset-def.o \
	$(MYSYS51_PATH)/default.o \
	$(MYSYS51_PATH)/default_modify.o \
	$(MYSYS51_PATH)/errors.o \
	$(MYSYS51_PATH)/hash.o \
	$(MYSYS51_PATH)/list.o \
	$(MYSYS51_PATH)/md5.o \
	$(MYSYS51_PATH)/mf_arr_appstr.o \
	$(MYSYS51_PATH)/mf_cache.o \
	$(MYSYS51_PATH)/mf_dirname.o \
	$(MYSYS51_PATH)/mf_fn_ext.o \
	$(MYSYS51_PATH)/mf_format.o \
	$(MYSYS51_PATH)/mf_iocache2.o \
	$(MYSYS51_PATH)/mf_iocache.o \
	$(MYSYS51_PATH)/mf_loadpath.o \
	$(MYSYS51_PATH)/mf_pack.o \
	$(MYSYS51_PATH)/mf_path.o \
	$(MYSYS51_PATH)/mf_qsort.o \
	$(MYSYS51_PATH)/mf_tempfile.o \
	$(MYSYS51_PATH)/mf_unixpath.o \
	$(MYSYS51_PATH)/mf_wcomp.o \
	$(MYSYS51_PATH)/mulalloc.o \
	$(MYSYS51_PATH)/my_alloc.o \
	$(MYSYS51_PATH)/my_chsize.o \
	$(MYSYS51_PATH)/my_compress.o \
	$(MYSYS51_PATH)/my_create.o \
	$(MYSYS51_PATH)/my_delete.o \
	$(MYSYS51_PATH)/my_div.o \
	$(MYSYS51_PATH)/my_error.o \
	$(MYSYS51_PATH)/my_file.o \
	$(MYSYS51_PATH)/my_fopen.o \
	$(MYSYS51_PATH)/my_fstream.o \
	$(MYSYS51_PATH)/my_getopt.o \
	$(MYSYS51_PATH)/my_getsystime.o \
	$(MYSYS51_PATH)/my_getwd.o \
	$(MYSYS51_PATH)/my_init.o \
	$(MYSYS51_PATH)/my_lib.o \
	$(MYSYS51_PATH)/my_malloc.o \
	$(MYSYS51_PATH)/my_messnc.o \
	$(MYSYS51_PATH)/my_net.o \
	$(MYSYS51_PATH)/my_once.o \
	$(MYSYS51_PATH)/my_open.o \
	$(MYSYS51_PATH)/my_pread.o \
	$(MYSYS51_PATH)/my_pthread.o \
	$(MYSYS51_PATH)/my_read.o \
	$(MYSYS51_PATH)/my_realloc.o \
	$(MYSYS51_PATH)/my_rename.o \
	$(MYSYS51_PATH)/my_static.o \
	$(MYSYS51_PATH)/my_seek.o \
	$(MYSYS51_PATH)/my_sleep.o \
	$(MYSYS51_PATH)/my_symlink.o \
	$(MYSYS51_PATH)/my_sync.o \
	$(MYSYS51_PATH)/my_thr_init.o \
	$(MYSYS51_PATH)/my_write.o \
	$(MYSYS51_PATH)/safemalloc.o \
	$(MYSYS51_PATH)/sha1.o \
	$(MYSYS51_PATH)/string.o \
	$(MYSYS51_PATH)/thr_mutex.o \
	$(MYSYS51_PATH)/typelib.o 





# common
MYCMN51_PATH=$(MYSQL51_SRC_PATH)/libmysql/.libs
MYCMN51_OBJ=$(MYCMN51_PATH)/net.o $(MYCMN51_PATH)/get_password.o $(MYCMN51_PATH)/pack.o $(MYCMN51_PATH)/my_time.o

# more dependencies found in testing
MYDEP51_PATH=$(MYSQL51_SRC_PATH)/libmysql/.libs
MYDEP51_OBJ=$(MYDEP51_PATH)/vio.o $(MYDEP51_PATH)/viosocket.o $(MYDEP51_PATH)/viosslfactories.o

MYSQL51_OBJ=$(MYSTRINGS51_OBJ) $(MYSYS51_OBJ) $(MYCMN51_OBJ) $(MYDEP51_OBJ)


